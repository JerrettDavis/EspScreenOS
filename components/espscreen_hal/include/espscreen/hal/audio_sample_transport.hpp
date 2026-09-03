#pragma once

#include "espscreen/core/status.hpp"
#include <cstddef>
#include <cstdint>

namespace espscreen::hal {

// Consumer-owned port for interleaved stereo sample transport.
class IAudioSampleTransport {
public:
    virtual ~IAudioSampleTransport() = default;
    [[nodiscard]] virtual core::Status start() = 0;
    [[nodiscard]] virtual core::Status restart_output() = 0;
    virtual void suspend_output() = 0;
    [[nodiscard]] virtual core::Status write_stereo(const std::int16_t* samples,
                                                    std::size_t frames) = 0;
    [[nodiscard]] virtual core::Status read_stereo(std::int16_t* samples,
                                                   std::size_t frames,
                                                   std::size_t& frames_read,
                                                   std::uint32_t timeout_ms) = 0;
    virtual void stop() = 0;
};

} // namespace espscreen::hal
