#pragma once

#include <cstdint>

namespace espscreen::core {

class IMonotonicClock {
public:
    virtual ~IMonotonicClock() = default;
    [[nodiscard]] virtual std::int64_t now_us() const = 0;
};

} // namespace espscreen::core
