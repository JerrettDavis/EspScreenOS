#include "espscreen/apps/sd_app_path.hpp"
#include <cstring>

namespace espscreen::apps {

core::Status build_sd_application_path(std::string_view mount_point,
                                       std::string_view filename,
                                       char* destination,
                                       std::size_t capacity) {
    if (!destination || capacity == 0 || mount_point.empty() ||
        !is_lua_application_file(filename))
        return {core::ErrorCode::invalid_argument, "invalid SD application path"};

    while (mount_point.ends_with('/'))
        mount_point.remove_suffix(1);
    constexpr std::string_view directory{"/apps/"};
    const auto required = mount_point.size() + directory.size() + filename.size() + 1;
    if (required > capacity)
        return {core::ErrorCode::capacity_exceeded, "SD application path too long"};

    auto* output = destination;
    std::memcpy(output, mount_point.data(), mount_point.size());
    output += mount_point.size();
    std::memcpy(output, directory.data(), directory.size());
    output += directory.size();
    std::memcpy(output, filename.data(), filename.size());
    output[filename.size()] = '\0';
    return core::Status::success();
}

} // namespace espscreen::apps
