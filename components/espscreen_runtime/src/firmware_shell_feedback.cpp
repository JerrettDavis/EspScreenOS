#include "espscreen/runtime/firmware_shell_feedback.hpp"

#include "espscreen/hal/audio_device.hpp"
#include "espscreen/hal/haptics_controller.hpp"

namespace espscreen::runtime {

void FirmwareShellFeedback::set_audio_volume(std::uint8_t percent) {
    audio_.set_volume(percent);
}

void FirmwareShellFeedback::pulse(std::uint8_t intensity,
                                  std::uint16_t duration_ms) {
    (void)haptics_.pulse(intensity, duration_ms);
}

void FirmwareShellFeedback::tone(std::uint16_t hz, std::uint16_t duration_ms,
                                 std::uint8_t volume) {
    (void)audio_.request_tone(hz, duration_ms, volume);
}

} // namespace espscreen::runtime
