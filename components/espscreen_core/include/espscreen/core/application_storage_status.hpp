#pragma once

#include <cstdint>

namespace espscreen::core {

class IApplicationStorageStatus {
public:
    virtual ~IApplicationStorageStatus() = default;
    [[nodiscard]] virtual bool storage_mounted() const = 0;
    [[nodiscard]] virtual std::uint64_t storage_free_bytes() const = 0;
};

} // namespace espscreen::core
