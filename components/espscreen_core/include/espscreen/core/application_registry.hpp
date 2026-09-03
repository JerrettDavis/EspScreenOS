#pragma once

#include "espscreen/core/status.hpp"
#include <span>

namespace espscreen::core {

class IApplication;

class IApplicationRegistrar {
public:
    virtual ~IApplicationRegistrar() = default;
    [[nodiscard]] virtual Status register_apps(
        std::span<IApplication* const> applications) = 0;
};

} // namespace espscreen::core
