#pragma once

#include "espscreen/core/status.hpp"
#include <cstddef>
#include <cstdint>

namespace espscreen::core {

class IApplicationAudioOutput {
public:
    virtual ~IApplicationAudioOutput() = default;
    [[nodiscard]] virtual Status write_audio(
        const std::int16_t* interleaved_stereo, std::size_t frames) = 0;
};

} // namespace espscreen::core
