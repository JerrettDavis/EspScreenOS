#pragma once

#include "espscreen/ui/shell_capabilities.hpp"
#include "espscreen/hal/touch_device.hpp"
#include "espscreen/hal/touch_input_filter.hpp"

struct lv_indev_t;

namespace espscreen::runtime {

class FirmwareShellInput final : public ui::IShellInput {
public:
    explicit FirmwareShellInput(hal::TouchDevice& touch) : touch_(touch) {}
    ~FirmwareShellInput();
    [[nodiscard]] core::Status start();
    void stop();
    [[nodiscard]] ui::TouchSample touch_sample() const override;
private:
    static void read_callback(lv_indev_t* input, void* data);
    hal::TouchDevice& touch_;
    hal::TouchInputFilter filter_{};
    lv_indev_t* input_{nullptr};
    ui::TouchSample latest_{};
};

} // namespace espscreen::runtime
