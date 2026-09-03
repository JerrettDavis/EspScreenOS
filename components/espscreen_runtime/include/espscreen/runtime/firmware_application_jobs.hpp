#pragma once

#include "espscreen/core/application_jobs.hpp"
#include "espscreen/core/atomic_component_lifecycle.hpp"
#include <atomic>
#include <cstdint>

namespace espscreen::runtime {

class FirmwareApplicationJobs final : public core::IApplicationJobs {
public:
    [[nodiscard]] core::Status start();
    void stop();
    [[nodiscard]] core::Status submit(core::IApplicationJob& job) override;
    void quiesce() override;

private:
    static void worker_entry(void* context);
    void worker();
    void complete_pending();

    void* queue_{nullptr};
    void* worker_task_{nullptr};
    void* worker_stopped_{nullptr};
    void* idle_{nullptr};
    std::atomic<std::size_t> pending_{0};
    core::AtomicComponentLifecycle lifecycle_{};
};

} // namespace espscreen::runtime
