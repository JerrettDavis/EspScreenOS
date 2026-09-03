#include "firmware_lvgl_foundation.hpp"

#include "esp_timer.h"
#include "lvgl.h"

namespace espscreen::runtime {

core::Status FirmwareLvglFoundation::start() {
    const auto transition = lifecycle_.begin_start();
    if (!transition.status || !transition.acquire) return transition.status;
    lv_init();
    lv_tick_set_cb([] {
        return static_cast<std::uint32_t>(esp_timer_get_time() / 1000U);
    });
    lifecycle_.started();
    return core::Status::success();
}

void FirmwareLvglFoundation::stop() {
    if (!lifecycle_.begin_stop()) return;
    lv_deinit();
    lifecycle_.stopped();
}

} // namespace espscreen::runtime
