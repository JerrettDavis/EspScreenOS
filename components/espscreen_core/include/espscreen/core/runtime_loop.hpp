#pragma once
#include "espscreen/core/clock.hpp"
#include <cstdint>

namespace espscreen::core {

struct AnimationPacing final {
    std::uint32_t samples{0};
    std::uint32_t late_samples{0};
    std::int64_t average_gap_us{0};
    std::int64_t maximum_gap_us{0};
    std::int64_t maximum_handler_us{0};
};

class IRuntimeWork {
public:
    virtual ~IRuntimeWork() = default;
    virtual void tick() = 0;
};

class IUiTimers {
public:
    virtual ~IUiTimers() = default;
    [[nodiscard]] virtual std::uint32_t handle() = 0;
    [[nodiscard]] virtual bool animations_running() = 0;
};

class IRuntimeClock : public IMonotonicClock {
public:
    virtual ~IRuntimeClock() = default;
    virtual void delay_ms(std::uint32_t duration) = 0;
};

class IRuntimeTelemetry {
public:
    virtual ~IRuntimeTelemetry() = default;
    virtual void first_frame(std::int64_t handler_us) = 0;
    virtual void animation_pacing(const AnimationPacing& pacing) = 0;
};

class RuntimeLoop final {
public:
    RuntimeLoop(IRuntimeWork& work, IUiTimers& ui, IRuntimeClock& clock,
                IRuntimeTelemetry* telemetry = nullptr)
        : work_(work), ui_(ui), clock_(clock), telemetry_(telemetry) {}

    void step();
    [[noreturn]] void run();

private:
    IRuntimeWork& work_;
    IUiTimers& ui_;
    IRuntimeClock& clock_;
    IRuntimeTelemetry* telemetry_;
    bool first_frame_{true};
    bool was_animating_{false};
    std::int64_t last_loop_us_{0};
    std::int64_t maximum_gap_us_{0};
    std::int64_t maximum_handler_us_{0};
    std::int64_t sum_gap_us_{0};
    std::uint32_t animation_samples_{0};
    std::uint32_t late_samples_{0};
};

} // namespace espscreen::core
