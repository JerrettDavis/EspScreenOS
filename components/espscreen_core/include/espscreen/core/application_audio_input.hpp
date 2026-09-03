#pragma once

#include "espscreen/core/status.hpp"
#include <cstddef>
#include <cstdint>

namespace espscreen::core {

class IApplicationAudioInput {
public:
    virtual ~IApplicationAudioInput() = default;
    [[nodiscard]] virtual std::uint32_t audio_sample_rate() const = 0;
    [[nodiscard]] virtual Status read_audio(std::int16_t* mono,
                                            std::size_t frames,
                                            std::size_t& frames_read,
                                            std::uint32_t timeout_ms) = 0;
};

} // namespace espscreen::core
