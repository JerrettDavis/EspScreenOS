#pragma once

#include "espscreen/core/status.hpp"
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace espscreen::services {

class ISettingsBackend {
public:
    virtual ~ISettingsBackend() = default;
    [[nodiscard]] virtual core::Status open() = 0;
    virtual void close() = 0;
    [[nodiscard]] virtual core::Status get_string(const char* key, char* destination,
                                                   std::size_t capacity) const = 0;
    [[nodiscard]] virtual core::Status set_string(const char* key,
                                                   std::string_view value) = 0;
    [[nodiscard]] virtual core::Status get_i32(const char* key,
                                                std::int32_t& value) const = 0;
    [[nodiscard]] virtual core::Status set_i32(const char* key, std::int32_t value) = 0;
    [[nodiscard]] virtual core::Status erase(const char* key) = 0;
};

} // namespace espscreen::services
