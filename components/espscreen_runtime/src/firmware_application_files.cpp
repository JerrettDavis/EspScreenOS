#include "espscreen/runtime/firmware_application_files.hpp"
#include "espscreen/runtime/application_file_path.hpp"

#include <array>
#include <cerrno>
#include <cstdio>
#ifdef _MSC_VER
#include <filesystem>
#include <string>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

namespace espscreen::runtime {
namespace {
FILE* open_file(const char* path, const char* mode) {
#ifdef _MSC_VER
    FILE* file = nullptr;
    return fopen_s(&file, path, mode) == 0 ? file : nullptr;
#else
    return std::fopen(path, mode);
#endif
}
} // namespace

core::Status FirmwareApplicationFiles::read_text(std::string_view name, char* destination,
                                                  std::size_t capacity,
                                                  std::size_t& bytes_read) {
    bytes_read = 0;
    if (!destination || capacity == 0)
        return {core::ErrorCode::invalid_argument, "invalid text destination"};
    std::array<char, 256> path{};
    if (const auto status = build_application_file_path(
            storage_.storage_mount_point(), name, path.data(), path.size());
        !status)
        return status;
    FILE* file = open_file(path.data(), "rb");
    if (!file)
        return {errno == ENOENT ? core::ErrorCode::not_found : core::ErrorCode::io_error,
                "application text file open failed"};
    bytes_read = std::fread(destination, 1, capacity - 1, file);
    const bool oversized = bytes_read == capacity - 1 && std::fgetc(file) != EOF;
    const bool read_failed = std::ferror(file) != 0;
    const bool close_failed = std::fclose(file) != 0;
    const bool failed = read_failed || close_failed;
    destination[bytes_read] = '\0';
    if (failed) return {core::ErrorCode::io_error, "application text file read failed"};
    if (oversized)
        return {core::ErrorCode::capacity_exceeded, "application text file too large"};
    return core::Status::success();
}

core::Status FirmwareApplicationFiles::write_text(std::string_view name,
                                                   std::string_view contents) {
    std::array<char, 256> path{};
    if (const auto status = build_application_file_path(
            storage_.storage_mount_point(), name, path.data(), path.size());
        !status)
        return status;
    FILE* file = open_file(path.data(), "wb");
    if (!file) return {core::ErrorCode::io_error, "application text file open failed"};
    const auto written = std::fwrite(contents.data(), 1, contents.size(), file);
    const bool failed = written != contents.size() || std::fclose(file) != 0;
    return failed ? core::Status{core::ErrorCode::io_error,
                                 "application text file write failed"}
                  : core::Status::success();
}

core::Status FirmwareApplicationFiles::visit_files(
    std::string_view relative_directory, core::IApplicationFileVisitor& visitor) {
    std::array<char, 256> directory_path{};
    if (const auto status = build_application_directory_path(
            storage_.storage_mount_point(), relative_directory,
            directory_path.data(), directory_path.size());
        !status)
        return status;

#ifdef _MSC_VER
    std::error_code error;
    std::filesystem::directory_iterator entries{directory_path.data(), error};
    if (error) return core::Status::success();
    for (const auto& entry : entries) {
        if (!entry.is_regular_file(error)) continue;
        const auto name = entry.path().filename().string();
        const auto path = entry.path().string();
        if (!visitor.visit({.name = name, .path = path})) break;
    }
#else
    DIR* entries = opendir(directory_path.data());
    if (!entries) return core::Status::success();
    while (const auto* entry = readdir(entries)) {
        const std::string_view name{entry->d_name};
        if (name == "." || name == "..") continue;
        std::array<char, 256> path{};
        const int length = std::snprintf(path.data(), path.size(), "%s/%.*s",
                                         directory_path.data(),
                                         static_cast<int>(name.size()), name.data());
        if (length < 0 || static_cast<std::size_t>(length) >= path.size()) {
            closedir(entries);
            return {core::ErrorCode::capacity_exceeded, "application file path too long"};
        }
        struct stat metadata {};
        if (stat(path.data(), &metadata) != 0 || !S_ISREG(metadata.st_mode)) continue;
        if (!visitor.visit({.name = name, .path = path.data()})) break;
    }
    closedir(entries);
#endif
    return core::Status::success();
}

} // namespace espscreen::runtime
