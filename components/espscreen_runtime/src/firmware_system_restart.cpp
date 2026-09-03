#include "espscreen/runtime/firmware_system_restart.hpp"
#include "esp_system.h"

namespace espscreen::runtime {

void FirmwareSystemRestart::restart() {
    haptics_.stop_haptics("system restart");
    esp_restart();
}

} // namespace espscreen::runtime
