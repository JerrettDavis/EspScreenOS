#include "espscreen/services/wifi_credentials.hpp"
#include <cstddef>

namespace espscreen::services {
namespace {

int hex_digit(char value) {
    if (value >= '0' && value <= '9') return value - '0';
    if (value >= 'a' && value <= 'f') return value - 'a' + 10;
    if (value >= 'A' && value <= 'F') return value - 'A' + 10;
    return -1;
}

core::Status decode(std::string_view source, char* destination, std::size_t capacity) {
    std::size_t written = 0;
    for (std::size_t index = 0; index < source.size(); ++index) {
        char value = source[index];
        if (value == '+') {
            value = ' ';
        } else if (value == '%') {
            if (index + 2 >= source.size())
                return {core::ErrorCode::invalid_argument, "incomplete URL escape"};
            const int high = hex_digit(source[index + 1]);
            const int low = hex_digit(source[index + 2]);
            if (high < 0 || low < 0)
                return {core::ErrorCode::invalid_argument, "invalid URL escape"};
            value = static_cast<char>((high << 4) | low);
            index += 2;
            if (value == '\0')
                return {core::ErrorCode::invalid_argument, "encoded null is not allowed"};
        }
        if (written + 1 >= capacity)
            return {core::ErrorCode::capacity_exceeded, "credential field is too long"};
        destination[written++] = value;
    }
    destination[written] = '\0';
    return core::Status::success();
}

} // namespace

core::Status parse_wifi_credentials(std::string_view form, WifiCredentials& output) {
    output = {};
    bool found_ssid = false;
    std::size_t position = 0;
    while (position <= form.size()) {
        const auto separator = form.find('&', position);
        const auto end = separator == std::string_view::npos ? form.size() : separator;
        const auto pair = form.substr(position, end - position);
        const auto equals = pair.find('=');
        const auto key = pair.substr(0, equals);
        const auto value = equals == std::string_view::npos
                               ? std::string_view{}
                               : pair.substr(equals + 1);
        core::Status result;
        if (key == "ssid") {
            result = decode(value, output.ssid.data(), output.ssid.size());
            found_ssid = result.ok() && !value.empty();
        } else if (key == "password") {
            result = decode(value, output.password.data(), output.password.size());
        }
        if (!result) return result;
        if (separator == std::string_view::npos) break;
        position = separator + 1;
    }
    if (!found_ssid || output.ssid[0] == '\0')
        return {core::ErrorCode::invalid_argument, "Wi-Fi name is required"};
    return core::Status::success();
}

} // namespace espscreen::services
