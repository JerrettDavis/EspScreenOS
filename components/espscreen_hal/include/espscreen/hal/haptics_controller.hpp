#pragma once
#include "espscreen/board/descriptor.hpp"
#include "espscreen/core/status.hpp"
#include "espscreen/hal/haptics_logic.hpp"
#include <atomic>
#include <cstdint>

namespace espscreen::hal {

class HapticsController final {
public:
    explicit HapticsController(const board::HapticsConfig& config)
        : config_(config), logic_(config.minimum_running_duty) {}
    ~HapticsController();

    [[nodiscard]] core::Status prepare_safe_state();
    core::Status initialize();
    void off(const char* reason = "requested");
    core::Status set_intensity(int intensity_percent);
    core::Status pulse(int intensity_percent, std::uint32_t duration_ms);
    core::Status test();
    void stop();
    bool available() const { return initialized_; }
    bool running() const { return running_; }

private:
    [[nodiscard]] core::Status force_pin_off();
    static void worker_entry(void* context);
    void worker();
    void write_duty(std::uint8_t intensity_percent);

    const board::HapticsConfig& config_;
    HapticsLogic logic_;
    void* task_{nullptr};
    std::atomic<std::uint32_t> command_generation_{0};
    std::atomic<int> requested_intensity_{0};
    std::atomic<std::uint32_t> requested_duration_ms_{0};
    std::atomic<bool> initialized_{false};
    std::atomic<bool> running_{false};
};

}
