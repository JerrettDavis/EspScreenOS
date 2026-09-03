#pragma once

#include "espscreen/core/status.hpp"

namespace espscreen::services {

class IAssetStore {
public:
    virtual ~IAssetStore() = default;
    [[nodiscard]] virtual core::Status mount() = 0;
    virtual void unmount() = 0;
    [[nodiscard]] virtual const char* root() const = 0;
};

} // namespace espscreen::services
