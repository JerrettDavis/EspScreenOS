#pragma once

#include "espscreen/core/status.hpp"
#include <cstddef>
#include <string_view>

namespace espscreen::runtime {

[[nodiscard]] constexpr bool is_application_path_segment(std::string_view value) {
    return !value.empty() && value != "." && value != ".." &&
           value.find('/') == std::string_view::npos &&
           value.find('\\') == std::string_view::npos;
}

[[nodiscard]] core::Status build_application_file_path(
    std::string_view mount_point, std::string_view filename, char* destination,
    std::size_t capacity);

[[nodiscard]] core::Status build_application_directory_path(
    std::string_view mount_point, std::string_view directory, char* destination,
    std::size_t capacity);

} // namespace espscreen::runtime
