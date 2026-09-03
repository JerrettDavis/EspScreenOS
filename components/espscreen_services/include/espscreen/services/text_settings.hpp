#pragma once

#include "espscreen/core/status.hpp"
#include <cstddef>
#include <string_view>

namespace espscreen::services {

class ITextSettings {
public:
    virtual ~ITextSettings() = default;
    [[nodiscard]] virtual core::Status get_text(
        const char* key, char* destination, std::size_t capacity) const = 0;
    [[nodiscard]] virtual core::Status set_text(
        const char* key, std::string_view value) = 0;
    [[nodiscard]] virtual core::Status erase(const char* key) = 0;
};

} // namespace espscreen::services
