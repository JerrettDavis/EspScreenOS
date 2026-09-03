#pragma once

#include "espscreen/ui/shell_capabilities.hpp"

namespace espscreen::hal {
class AudioDevice;
class HapticsController;
}

namespace espscreen::runtime {

class FirmwareShellFeedback final : public ui::IShellFeedback {
public:
    FirmwareShellFeedback(hal::AudioDevice& audio, hal::HapticsController& haptics)
        : audio_(audio), haptics_(haptics) {}

    void set_audio_volume(std::uint8_t percent) override;
    void pulse(std::uint8_t intensity, std::uint16_t duration_ms) override;
    void tone(std::uint16_t hz, std::uint16_t duration_ms,
              std::uint8_t volume) override;

private:
    hal::AudioDevice& audio_;
    hal::HapticsController& haptics_;
};

} // namespace espscreen::runtime
