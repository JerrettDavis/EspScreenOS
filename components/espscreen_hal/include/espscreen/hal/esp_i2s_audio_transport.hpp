#pragma once

#include "espscreen/board/descriptor.hpp"
#include "espscreen/hal/audio_sample_transport.hpp"

namespace espscreen::hal {

class EspI2sAudioTransport final : public IAudioSampleTransport {
public:
    explicit EspI2sAudioTransport(const board::AudioConfig& config) : config_(config) {}
    ~EspI2sAudioTransport();
    [[nodiscard]] core::Status start() override;
    [[nodiscard]] core::Status restart_output() override;
    void suspend_output() override;
    [[nodiscard]] core::Status write_stereo(const std::int16_t* samples,
                                            std::size_t frames) override;
    [[nodiscard]] core::Status read_stereo(std::int16_t* samples,
                                           std::size_t frames,
                                           std::size_t& frames_read,
                                           std::uint32_t timeout_ms) override;
    void stop() override;

private:
    board::AudioConfig config_{};
    void* transmit_channel_{nullptr};
    void* receive_channel_{nullptr};
    bool output_enabled_{false};
};

} // namespace espscreen::hal
