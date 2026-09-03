#include "espscreen/runtime/firmware_shell_display.hpp"

#include "espscreen/hal/display_device.hpp"

namespace espscreen::runtime {

void FirmwareShellDisplay::set_backlight(std::uint8_t percent) {
    (void)display_.set_backlight(percent);
}

} // namespace espscreen::runtime
