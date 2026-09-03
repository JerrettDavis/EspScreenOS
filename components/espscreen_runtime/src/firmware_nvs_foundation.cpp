#include "firmware_nvs_foundation.hpp"

#include "nvs_flash.h"

namespace espscreen::runtime {

core::Status FirmwareNvsFoundation::start() {
    const auto transition = lifecycle_.begin_start();
    if (!transition.status || !transition.acquire) return transition.status;
    auto result = nvs_flash_init();
    if (result == ESP_ERR_NVS_NO_FREE_PAGES ||
        result == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        result = nvs_flash_erase();
        if (result == ESP_OK) result = nvs_flash_init();
    }
    if (result != ESP_OK) {
        lifecycle_.start_failed();
        return {core::ErrorCode::io_error, "NVS initialization failed"};
    }
    lifecycle_.started();
    return core::Status::success();
}

void FirmwareNvsFoundation::stop() {
    if (!lifecycle_.begin_stop()) return;
    (void)nvs_flash_deinit();
    lifecycle_.stopped();
}

} // namespace espscreen::runtime
