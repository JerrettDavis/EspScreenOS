#pragma once
#include "espscreen/core/runtime_loop.hpp"

namespace espscreen::runtime {

class FirmwareRuntime final : private core::IUiTimers,
                              private core::IRuntimeTelemetry {
public:
    FirmwareRuntime(core::IRuntimeWork& work, core::IRuntimeClock& clock);
    [[noreturn]] void run();

private:
    [[nodiscard]] std::uint32_t handle() override;
    [[nodiscard]] bool animations_running() override;
    void first_frame(std::int64_t handler_us) override;
    void animation_pacing(const core::AnimationPacing& pacing) override;

    core::RuntimeLoop loop_;
};

} // namespace espscreen::runtime
