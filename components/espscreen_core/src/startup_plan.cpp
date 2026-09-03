#include "espscreen/core/startup_plan.hpp"
#include "espscreen/core/fixed_vector.hpp"

namespace espscreen::core {

Status execute_startup_plan(std::span<const StartupStep> steps, IStartupObserver* observer,
                            StartupSession* session) {
    constexpr std::size_t maximum_steps = 16;
    if (steps.size() > maximum_steps)
        return {ErrorCode::capacity_exceeded, "startup plan exceeds capacity"};
    for (const auto& step : steps)
        if (step.start == nullptr)
            return {ErrorCode::invalid_argument, "startup step has no action"};

    if (session != nullptr) {
        session->completed_ = 0;
        session->step_count_ = steps.size();
        session->active_ = false;
    }

    FixedVector<const StartupStep*, maximum_steps> completed;
    for (std::size_t step_index = 0; step_index < steps.size(); ++step_index) {
        const auto& step = steps[step_index];
        const auto result = step.start(step.context);
        if (result) {
            (void)completed.push_back(&step);
            if (session != nullptr)
                session->completed_ |= std::uint32_t{1} << step_index;
            continue;
        }
        if (!step.critical) {
            if (observer != nullptr) observer->optional_step_failed(step.name, result);
            continue;
        }
        for (std::size_t index = completed.size(); index > 0; --index) {
            const auto* prior = completed[index - 1];
            if (prior->rollback != nullptr) prior->rollback(prior->context);
        }
        if (session != nullptr) {
            session->completed_ = 0;
            session->step_count_ = 0;
        }
        return result;
    }
    if (session != nullptr) session->active_ = true;
    return Status::success();
}

void shutdown_startup_plan(std::span<const StartupStep> steps, StartupSession* session) {
    if (session != nullptr && !session->active_) return;
    for (std::size_t index = steps.size(); index > 0; --index) {
        if (session != nullptr && !session->completed(index - 1)) continue;
        const auto& step = steps[index - 1];
        if (step.rollback != nullptr) step.rollback(step.context);
    }
    if (session != nullptr) {
        session->completed_ = 0;
        session->step_count_ = 0;
        session->active_ = false;
    }
}

} // namespace espscreen::core
