#pragma once

#include "espscreen/ui/shell_capabilities.hpp"

namespace espscreen::hal {
class DisplayDevice;
}

namespace espscreen::runtime {

class FirmwareShellDisplay final : public ui::IShellDisplay {
public:
    explicit FirmwareShellDisplay(hal::DisplayDevice& display) : display_(display) {}

    void set_backlight(std::uint8_t percent) override;

private:
    hal::DisplayDevice& display_;
};

} // namespace espscreen::runtime
