#include "espscreen/services/settings_service.hpp"

namespace espscreen::services {

core::Status SettingsService::start() {
    const auto transition = lifecycle_.begin_start();
    if (!transition.status || !transition.acquire) return transition.status;
    const auto status = backend_.open();
    if (status) lifecycle_.started();
    else lifecycle_.start_failed();
    return status;
}

void SettingsService::stop() {
    if (!lifecycle_.begin_stop()) return;
    backend_.close();
    lifecycle_.stopped();
}

core::Status SettingsService::get_text(const char* key, char* destination,
                                       std::size_t capacity) const {
    if (!lifecycle_.running() || !key || !destination || capacity == 0)
        return {core::ErrorCode::invalid_argument, "invalid settings read"};
    return backend_.get_string(key, destination, capacity);
}

core::Status SettingsService::set_text(const char* key, std::string_view value) {
    if (!lifecycle_.running() || !key || value.size() > 511)
        return {core::ErrorCode::invalid_argument, "invalid settings write"};
    return backend_.set_string(key, value);
}

core::Status SettingsService::get_i32(const char* key, std::int32_t& value) const {
    if (!lifecycle_.running() || !key)
        return {core::ErrorCode::invalid_argument, "invalid settings read"};
    return backend_.get_i32(key, value);
}

core::Status SettingsService::set_i32(const char* key, std::int32_t value) {
    if (!lifecycle_.running() || !key)
        return {core::ErrorCode::invalid_argument, "invalid settings write"};
    return backend_.set_i32(key, value);
}

core::Status SettingsService::erase(const char* key) {
    if (!lifecycle_.running() || !key)
        return {core::ErrorCode::invalid_argument, "invalid settings erase"};
    return backend_.erase(key);
}

} // namespace espscreen::services
