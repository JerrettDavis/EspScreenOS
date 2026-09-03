#include "firmware_hardware_safety.hpp"

#include "espscreen/hal/haptics_controller.hpp"

namespace espscreen::runtime {

core::Status FirmwareHardwareSafety::start() {
    return haptics_.prepare_safe_state();
}

} // namespace espscreen::runtime
