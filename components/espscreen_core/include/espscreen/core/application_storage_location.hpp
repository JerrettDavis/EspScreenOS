#pragma once

namespace espscreen::core {

class IApplicationStorageLocation {
public:
    virtual ~IApplicationStorageLocation() = default;
    [[nodiscard]] virtual const char* storage_mount_point() const = 0;
};

} // namespace espscreen::core
