#pragma once
#include "espscreen/core/status.hpp"
#include <cstddef>
#include <string_view>
namespace espscreen::core {

class IApplicationTextFiles {
public:
    virtual ~IApplicationTextFiles() = default;
    [[nodiscard]] virtual Status read_text(std::string_view name, char* destination,
                                           std::size_t capacity,
                                           std::size_t& bytes_read) = 0;
    [[nodiscard]] virtual Status write_text(std::string_view name,
                                            std::string_view contents) = 0;
};

struct ApplicationFileEntry final {
    std::string_view name;
    std::string_view path;
};

class IApplicationFileVisitor {
public:
    virtual ~IApplicationFileVisitor() = default;
    [[nodiscard]] virtual bool visit(ApplicationFileEntry entry) = 0;
};

class IApplicationDirectory {
public:
    virtual ~IApplicationDirectory() = default;
    [[nodiscard]] virtual Status visit_files(std::string_view relative_directory,
                                             IApplicationFileVisitor& visitor) = 0;
};

} // namespace espscreen::core
