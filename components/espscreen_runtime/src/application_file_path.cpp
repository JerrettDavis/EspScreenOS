#include "espscreen/runtime/application_file_path.hpp"

#include <cstdio>

namespace espscreen::runtime {
namespace {

core::Status build_path(std::string_view mount_point, std::string_view segment,
                        const char* invalid_message, const char* capacity_message,
                        char* destination, std::size_t capacity) {
    if (mount_point.empty() || !is_application_path_segment(segment))
        return {core::ErrorCode::invalid_argument, invalid_message};
    if (!destination || capacity == 0)
        return {core::ErrorCode::invalid_argument, "invalid path destination"};

    const bool mount_has_separator = mount_point.back() == '/' || mount_point.back() == '\\';
    const int length = std::snprintf(
        destination, capacity, mount_has_separator ? "%.*s%.*s" : "%.*s/%.*s",
        static_cast<int>(mount_point.size()), mount_point.data(),
        static_cast<int>(segment.size()), segment.data());
    if (length < 0 || static_cast<std::size_t>(length) >= capacity)
        return {core::ErrorCode::capacity_exceeded, capacity_message};
    return core::Status::success();
}

} // namespace

core::Status build_application_file_path(std::string_view mount_point,
                                         std::string_view filename,
                                         char* destination,
                                         std::size_t capacity) {
    return build_path(mount_point, filename, "invalid application filename",
                      "application path too long", destination, capacity);
}

core::Status build_application_directory_path(std::string_view mount_point,
                                              std::string_view directory,
                                              char* destination,
                                              std::size_t capacity) {
    return build_path(mount_point, directory, "invalid application directory",
                      "application directory path too long", destination, capacity);
}

} // namespace espscreen::runtime
