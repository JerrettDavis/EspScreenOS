#pragma once

#include "espscreen/board/bus_host.hpp"
#include <cstdint>

namespace espscreen::hal {

enum class AudioStartupKind : std::uint8_t { unavailable, pwm, codec_i2s, direct_i2s };

struct AudioHardwarePlan final {
    AudioStartupKind startup{AudioStartupKind::unavailable};
    std::int8_t control_bus{-1};
    std::int8_t data_bus{-1};
    std::uint32_t sample_rate{0};

    [[nodiscard]] constexpr bool valid() const {
        switch (startup) {
            case AudioStartupKind::pwm: return true;
            case AudioStartupKind::codec_i2s:
                return control_bus >= 0 && data_bus >= 0 && sample_rate == 16000;
            case AudioStartupKind::direct_i2s: return data_bus >= 0 && sample_rate > 0;
            case AudioStartupKind::unavailable: return false;
        }
        return false;
    }
};

[[nodiscard]] constexpr AudioHardwarePlan audio_hardware_plan(
    const board::AudioConfig& config) {
    switch (config.driver) {
        case board::AudioDriver::pwm:
            return {.startup = AudioStartupKind::pwm};
        case board::AudioDriver::es8311:
            return {.startup = AudioStartupKind::codec_i2s,
                    .control_bus = board::bus_index_for(config.control_host, board::BusFamily::i2c),
                    .data_bus = board::bus_index_for(config.data_host, board::BusFamily::i2s),
                    .sample_rate = config.sample_rate};
        case board::AudioDriver::i2s:
            return {.startup = AudioStartupKind::direct_i2s,
                    .data_bus = board::bus_index_for(config.data_host, board::BusFamily::i2s),
                    .sample_rate = config.sample_rate};
        case board::AudioDriver::none:
            return {};
    }
    return {};
}

} // namespace espscreen::hal
