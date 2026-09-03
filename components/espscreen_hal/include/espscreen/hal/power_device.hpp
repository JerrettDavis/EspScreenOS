#pragma once
#include "espscreen/board/descriptor.hpp"
#include "espscreen/core/status.hpp"
#include "espscreen/hal/device_state.hpp"
#include <cstdint>

namespace espscreen::hal {
struct PowerSnapshot final {
 std::int32_t battery_mv{-1}; std::int8_t battery_percent{-1}; bool usb_present{false}; std::int32_t ambient_raw{-1}; std::uint64_t sampled_at_us{0};
};
class PowerDevice final {
public:
 explicit PowerDevice(const board::PowerConfig& config) : config_(config) {}
 ~PowerDevice(); [[nodiscard]] core::Status start(); void stop(); [[nodiscard]] PowerSnapshot sample(); [[nodiscard]] DeviceHealth health() const { return health_; }
private:
 board::PowerConfig config_{}; void* adc_handle_{nullptr}; int adc_unit_{-1}; int battery_channel_{-1}; int ambient_channel_{-1}; DeviceHealth health_{};
};
}
