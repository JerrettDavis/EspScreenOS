#pragma once

#include "espscreen/core/status.hpp"
#include <cstdint>

namespace espscreen::core {

class ISettingsStore {
public:
    virtual ~ISettingsStore() = default;
    [[nodiscard]] virtual Status get_i32(const char* key,
                                         std::int32_t& value) const = 0;
    [[nodiscard]] virtual Status set_i32(const char* key,
                                         std::int32_t value) = 0;
};

} // namespace espscreen::core
