#pragma once
#include "espscreen/board/descriptor.hpp"
#include "espscreen/core/status.hpp"
#include "espscreen/hal/device_state.hpp"
#include "espscreen/hal/es8311_codec.hpp"
#include "espscreen/hal/audio_sample_transport.hpp"
#include <cstddef>
#include <cstdint>
#include <atomic>

namespace espscreen::hal {
class II2cBus;
class AudioDevice final {
public:
 AudioDevice(const board::AudioConfig& config, II2cBus& i2c, IAudioSampleTransport& samples) : config_(config), codec_(config,i2c), samples_(samples) {}
 ~AudioDevice(); [[nodiscard]] core::Status start(); void stop(); void set_volume(std::uint8_t percent) { volume_=percent>100?100:percent; } [[nodiscard]] std::uint8_t volume() const { return volume_; } [[nodiscard]] core::Status tone(std::uint16_t hz, std::uint16_t duration_ms, std::uint8_t volume=50); [[nodiscard]] core::Status request_tone(std::uint16_t hz, std::uint16_t duration_ms, std::uint8_t volume=50);
 [[nodiscard]] core::Status write_pcm(const std::int16_t* interleaved_stereo, std::size_t frames); [[nodiscard]] core::Status read_pcm(std::int16_t* mono, std::size_t frames, std::size_t& frames_read, std::uint32_t timeout_ms=1000); [[nodiscard]] DeviceHealth health() const { return health_; }
private:
 [[nodiscard]] core::Status start_pwm(); [[nodiscard]] core::Status start_es8311(); [[nodiscard]] core::Status prepare_pcm_resources(); [[nodiscard]] core::Status tone_i2s(std::uint16_t hz,std::uint16_t duration,std::uint8_t volume); [[nodiscard]] core::Status write_pcm_unlocked(const std::int16_t* data,std::size_t frames); static void tone_worker_entry(void*); void tone_worker();
 board::AudioConfig config_{}; Es8311Codec codec_; IAudioSampleTransport& samples_; void* tone_task_{nullptr}; void* tx_mutex_{nullptr}; std::int16_t* tone_buffer_{nullptr}; std::int16_t* capture_buffer_{nullptr}; std::atomic<bool> tone_worker_running_{false}; std::atomic<std::uint16_t> requested_hz_{0}; std::atomic<std::uint16_t> requested_duration_ms_{0}; std::atomic<std::uint8_t> requested_volume_{0}; std::uint8_t volume_{70}; DeviceHealth health_{};
};
}
