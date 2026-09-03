#pragma once

#include "espscreen/core/status.hpp"
#include <cstddef>
#include <string_view>

namespace espscreen::apps {

[[nodiscard]] constexpr bool is_lua_application_file(std::string_view filename) {
    return filename.size() > 4 && filename.ends_with(".lua") &&
           filename.find('/') == std::string_view::npos &&
           filename.find('\\') == std::string_view::npos;
}

[[nodiscard]] core::Status build_sd_application_path(
    std::string_view mount_point, std::string_view filename,
    char* destination, std::size_t capacity);

} // namespace espscreen::apps
