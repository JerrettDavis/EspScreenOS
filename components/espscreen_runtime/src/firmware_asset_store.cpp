#include "espscreen/runtime/firmware_asset_store.hpp"
#include "esp_spiffs.h"

namespace espscreen::runtime {

core::Status FirmwareAssetStore::mount() {
    esp_vfs_spiffs_conf_t configuration{};
    configuration.base_path = root();
    configuration.partition_label = "assets";
    configuration.max_files = 8;
    configuration.format_if_mount_failed = false;
    if (esp_vfs_spiffs_register(&configuration) != ESP_OK)
        return {core::ErrorCode::unavailable, "asset partition unavailable"};
    return core::Status::success();
}

void FirmwareAssetStore::unmount() {
    (void)esp_vfs_spiffs_unregister("assets");
}

} // namespace espscreen::runtime
