#include "espscreen/runtime/firmware_application_jobs.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

namespace espscreen::runtime {

core::Status FirmwareApplicationJobs::start() {
    const auto transition =
        lifecycle_.begin_start("application job executor is transitioning");
    if (!transition.status || !transition.acquire) return transition.status;
    queue_ = xQueueCreate(8, sizeof(core::IApplicationJob*));
    worker_stopped_ = xSemaphoreCreateBinary();
    idle_ = xSemaphoreCreateBinary();
    if (!queue_ || !worker_stopped_ || !idle_) {
        stop();
        return {core::ErrorCode::unavailable, "application job queue allocation failed"};
    }
    TaskHandle_t task{};
    if (xTaskCreate(&worker_entry, "app_jobs", 6144, this, 3, &task) != pdPASS) {
        stop();
        return {core::ErrorCode::unavailable, "application worker allocation failed"};
    }
    worker_task_ = task;
    lifecycle_.started();
    return core::Status::success();
}

core::Status FirmwareApplicationJobs::submit(core::IApplicationJob& job) {
    if (!lifecycle_.running())
        return {core::ErrorCode::invalid_argument, "application job executor unavailable"};
    pending_.fetch_add(1);
    if (!lifecycle_.running()) {
        complete_pending();
        return {core::ErrorCode::busy, "application job executor is stopping"};
    }
    auto* queued = &job;
    if (xQueueSend(static_cast<QueueHandle_t>(queue_), &queued, 0) != pdTRUE) {
        complete_pending();
        return {core::ErrorCode::capacity_exceeded, "application job queue full"};
    }
    return core::Status::success();
}

void FirmwareApplicationJobs::quiesce() {
    while (pending_.load() != 0 && idle_)
        (void)xSemaphoreTake(static_cast<SemaphoreHandle_t>(idle_), portMAX_DELAY);
}

void FirmwareApplicationJobs::worker_entry(void* context) {
    static_cast<FirmwareApplicationJobs*>(context)->worker();
}

void FirmwareApplicationJobs::complete_pending() {
    if (pending_.fetch_sub(1) == 1 && idle_)
        xSemaphoreGive(static_cast<SemaphoreHandle_t>(idle_));
}

void FirmwareApplicationJobs::worker() {
    core::IApplicationJob* job{};
    while (xQueueReceive(static_cast<QueueHandle_t>(queue_), &job, portMAX_DELAY) == pdTRUE) {
        if (!job) break;
        job->run();
        complete_pending();
    }
    xSemaphoreGive(static_cast<SemaphoreHandle_t>(worker_stopped_));
    worker_task_ = nullptr;
    vTaskDelete(nullptr);
}

void FirmwareApplicationJobs::stop() {
    if (!lifecycle_.begin_stop()) return;
    quiesce();
    if (queue_ && worker_task_) {
        core::IApplicationJob* stop_request{};
        (void)xQueueSend(static_cast<QueueHandle_t>(queue_), &stop_request, portMAX_DELAY);
        if (worker_task_ && worker_stopped_ &&
            xSemaphoreTake(static_cast<SemaphoreHandle_t>(worker_stopped_),
                           pdMS_TO_TICKS(5000)) != pdTRUE) {
            vTaskDelete(static_cast<TaskHandle_t>(worker_task_));
            worker_task_ = nullptr;
        }
    }
    if (queue_) vQueueDelete(static_cast<QueueHandle_t>(queue_));
    if (worker_stopped_) vSemaphoreDelete(static_cast<SemaphoreHandle_t>(worker_stopped_));
    if (idle_) vSemaphoreDelete(static_cast<SemaphoreHandle_t>(idle_));
    queue_ = worker_stopped_ = idle_ = nullptr;
    pending_ = 0;
    lifecycle_.stopped();
}

} // namespace espscreen::runtime
