#pragma once
#include "espscreen/board/descriptor.hpp"
#include "espscreen/core/status.hpp"
#include "espscreen/hal/device_state.hpp"
#include "espscreen/hal/touch_point.hpp"
#include <cstdint>

namespace espscreen::hal {
class II2cBus;
class TouchDevice final {
public:
    TouchDevice(const board::TouchConfig& touch, const board::DisplayConfig& display,
                II2cBus& i2c) : config_(touch), display_(display), i2c_(i2c) {}
    ~TouchDevice();
    [[nodiscard]] core::Status start();
    void stop();
    [[nodiscard]] bool read(TouchPoint& point);
    [[nodiscard]] DeviceHealth health() const { return health_; }
private:
    bool read_xpt2046(TouchPoint& point);
    bool read_ft6336(TouchPoint& point);
    bool read_cst328(TouchPoint& point);
    std::uint16_t xpt_read_axis(std::uint8_t command);
    std::uint8_t soft_spi_transfer(std::uint8_t value);
    board::TouchConfig config_{}; board::DisplayConfig display_{}; II2cBus& i2c_; DeviceHealth health_{}; bool i2c_acquired_{false}; std::uint8_t maximum_touches_{0};
};
}
