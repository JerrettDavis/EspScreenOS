#pragma once

#include <cstdint>

namespace espscreen::hal {

struct BatteryMeasurement final {
    std::int32_t millivolts{-1};
    std::int8_t percent{-1};
};

[[nodiscard]] BatteryMeasurement measure_battery(
    std::int32_t adc_raw,
    float voltage_divider,
    std::uint16_t empty_millivolts,
    std::uint16_t full_millivolts,
    std::int32_t adc_max = 4095,
    std::int32_t adc_reference_millivolts = 3300);

} // namespace espscreen::hal
