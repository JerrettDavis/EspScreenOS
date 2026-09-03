#include "espscreen/hal/power_measurement.hpp"
#include <algorithm>

namespace espscreen::hal {

BatteryMeasurement measure_battery(std::int32_t adc_raw,
                                   float voltage_divider,
                                   std::uint16_t empty_millivolts,
                                   std::uint16_t full_millivolts,
                                   std::int32_t adc_max,
                                   std::int32_t adc_reference_millivolts) {
    if (adc_max <= 0 || adc_reference_millivolts <= 0 || voltage_divider <= 0.0F)
        return {};

    adc_raw = std::clamp(adc_raw, std::int32_t{0}, adc_max);
    const auto adc_millivolts =
        static_cast<float>(adc_raw) * adc_reference_millivolts / adc_max;
    const auto battery_millivolts =
        static_cast<std::int32_t>(adc_millivolts * voltage_divider);
    if (full_millivolts <= empty_millivolts)
        return {.millivolts = battery_millivolts, .percent = -1};

    const auto percent = (battery_millivolts - empty_millivolts) * 100 /
                         (full_millivolts - empty_millivolts);
    return {.millivolts = battery_millivolts,
            .percent = static_cast<std::int8_t>(
                std::clamp(percent, std::int32_t{0}, std::int32_t{100}))};
}

} // namespace espscreen::hal
