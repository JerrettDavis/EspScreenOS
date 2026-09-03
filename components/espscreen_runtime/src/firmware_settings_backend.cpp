#include "espscreen/runtime/firmware_settings_backend.hpp"
#include "nvs.h"
#include <array>
#include <cstring>

namespace espscreen::runtime {
namespace {

core::Status committed(esp_err_t write_result, nvs_handle_t handle) {
    if (write_result != ESP_OK || nvs_commit(handle) != ESP_OK)
        return {core::ErrorCode::io_error, "settings write failed"};
    return core::Status::success();
}

} // namespace

core::Status FirmwareSettingsBackend::open() {
    nvs_handle_t handle{};
    if (nvs_open("espscreen", NVS_READWRITE, &handle) != ESP_OK)
        return {core::ErrorCode::io_error, "unable to open settings namespace"};
    handle_ = handle;
    return core::Status::success();
}

void FirmwareSettingsBackend::close() {
    if (!handle_) return;
    (void)nvs_commit(handle_);
    nvs_close(handle_);
    handle_ = 0;
}

core::Status FirmwareSettingsBackend::get_string(const char* key, char* destination,
                                                 std::size_t capacity) const {
    std::size_t size = capacity;
    const auto result = nvs_get_str(handle_, key, destination, &size);
    if (result == ESP_ERR_NVS_NOT_FOUND)
        return {core::ErrorCode::not_found, "setting not found"};
    if (result == ESP_ERR_NVS_INVALID_LENGTH)
        return {core::ErrorCode::capacity_exceeded, "destination too small"};
    return result == ESP_OK ? core::Status::success()
                            : core::Status{core::ErrorCode::io_error,
                                           "settings read failed"};
}

core::Status FirmwareSettingsBackend::set_string(const char* key, std::string_view value) {
    std::array<char, 512> buffer{};
    if (value.size() >= buffer.size())
        return {core::ErrorCode::capacity_exceeded, "setting is too large"};
    std::memcpy(buffer.data(), value.data(), value.size());
    return committed(nvs_set_str(handle_, key, buffer.data()), handle_);
}

core::Status FirmwareSettingsBackend::get_i32(const char* key, std::int32_t& value) const {
    const auto result = nvs_get_i32(handle_, key, &value);
    if (result == ESP_ERR_NVS_NOT_FOUND)
        return {core::ErrorCode::not_found, "setting not found"};
    return result == ESP_OK ? core::Status::success()
                            : core::Status{core::ErrorCode::io_error,
                                           "settings read failed"};
}

core::Status FirmwareSettingsBackend::set_i32(const char* key, std::int32_t value) {
    return committed(nvs_set_i32(handle_, key, value), handle_);
}

core::Status FirmwareSettingsBackend::erase(const char* key) {
    const auto result = nvs_erase_key(handle_, key);
    if (result != ESP_OK && result != ESP_ERR_NVS_NOT_FOUND)
        return {core::ErrorCode::io_error, "settings erase failed"};
    return committed(ESP_OK, handle_);
}

} // namespace espscreen::runtime
