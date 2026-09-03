#pragma once

#include "espscreen/board/descriptor.hpp"
#include "espscreen/resources/resource_broker.hpp"
#include "espscreen/core/application_audio_input.hpp"
#include "espscreen/core/application_audio_output.hpp"
#include "espscreen/core/application_tone.hpp"
#include "espscreen/core/application_board.hpp"
#include "espscreen/core/application_gpio.hpp"
#include "espscreen/core/application_haptics.hpp"
#include "espscreen/core/application_storage_location.hpp"
#include "espscreen/core/application_storage_status.hpp"
#include "espscreen/hal/audio_device.hpp"
#include "espscreen/hal/haptics_controller.hpp"
#include "espscreen/hal/storage_device.hpp"

namespace espscreen::hal {

class ApplicationBoardAdapter final : public core::IApplicationBoard {
public:
    explicit ApplicationBoardAdapter(const board::BoardDescriptor& descriptor)
        : descriptor_(descriptor) {}
    [[nodiscard]] core::ApplicationBoardInfo board_info() const override;
private:
    const board::BoardDescriptor& descriptor_;
};

class ApplicationStorageAdapter final : public core::IApplicationStorageLocation,
                                        public core::IApplicationStorageStatus {
public:
    ApplicationStorageAdapter(const board::StorageConfig& config, StorageDevice& device)
        : config_(config), device_(device) {}
    [[nodiscard]] const char* storage_mount_point() const override { return config_.mount_point; }
    [[nodiscard]] bool storage_mounted() const override { return device_.stats().mounted; }
    [[nodiscard]] std::uint64_t storage_free_bytes() const override { return device_.stats().free_bytes; }
private:
    const board::StorageConfig& config_;
    StorageDevice& device_;
};

class ApplicationHapticsAdapter final : public core::IApplicationHaptics {
public:
    explicit ApplicationHapticsAdapter(HapticsController& device) : device_(device) {}
    [[nodiscard]] bool haptics_available() const override { return device_.available(); }
    [[nodiscard]] core::Status pulse_haptics(std::uint8_t intensity,
                                              std::uint16_t duration_ms) override {
        return device_.pulse(intensity, duration_ms);
    }
    void stop_haptics(const char* reason) override { device_.off(reason); }
private:
    HapticsController& device_;
};

class ApplicationAudioAdapter final : public core::IApplicationTone,
                                      public core::IApplicationAudioInput,
                                      public core::IApplicationAudioOutput {
public:
    ApplicationAudioAdapter(const board::AudioConfig& config, AudioDevice& device)
        : config_(config), device_(device) {}
    [[nodiscard]] std::uint32_t audio_sample_rate() const override { return config_.sample_rate; }
    [[nodiscard]] core::Status request_tone(std::uint16_t hz, std::uint16_t duration_ms,
                                             std::uint8_t volume) override {
        return device_.request_tone(hz, duration_ms, volume);
    }
    [[nodiscard]] core::Status read_audio(std::int16_t* mono, std::size_t frames,
                                          std::size_t& frames_read,
                                          std::uint32_t timeout_ms) override {
        return device_.read_pcm(mono, frames, frames_read, timeout_ms);
    }
    [[nodiscard]] core::Status write_audio(const std::int16_t* stereo,
                                            std::size_t frames) override {
        return device_.write_pcm(stereo, frames);
    }
private:
    const board::AudioConfig& config_;
    AudioDevice& device_;
};

class ApplicationGpioAdapter final : public core::IApplicationGpio {
public:
    explicit ApplicationGpioAdapter(resources::ResourceBroker& resources) : resources_(resources) {}
    [[nodiscard]] core::Status acquire_gpio(std::int16_t pin, std::uint32_t owner) override;
    [[nodiscard]] core::Status release_gpio(std::int16_t pin, std::uint32_t owner) override;
    [[nodiscard]] core::Status configure_output(std::int16_t pin) override;
    [[nodiscard]] core::Status write_gpio(std::int16_t pin, bool high) override;
private:
    resources::ResourceBroker& resources_;
};

} // namespace espscreen::hal
