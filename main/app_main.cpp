#include "espscreen/runtime/firmware_application.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace {
constexpr const char* tag = "EspScreenOS";

[[noreturn]] void halt(const espscreen::core::Status& failure) {
    const auto message = failure.message();
    ESP_LOGE(tag, "Startup failed: %.*s", static_cast<int>(message.size()), message.data());
    while (true) vTaskDelay(portMAX_DELAY);
}
}

extern "C" void app_main() {
    if (const auto result = espscreen::runtime::start_firmware(); !result) halt(result);
    espscreen::runtime::run_firmware();
}
