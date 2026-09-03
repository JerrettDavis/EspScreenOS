#pragma once

#include "espscreen/core/application.hpp"
#include "espscreen/core/status.hpp"

#include <cstddef>
#include <string_view>

namespace espscreen::core {

inline constexpr std::size_t max_application_count = 16;

class IApplicationHost {
public:
    virtual ~IApplicationHost() = default;
    [[nodiscard]] virtual Status activate(std::string_view id,
                                          ApplicationSurface surface) = 0;
    virtual void stop() = 0;
    virtual void tick() = 0;
    virtual void dispatch(const Event& event) = 0;
    [[nodiscard]] virtual IApplication* active() const = 0;
    [[nodiscard]] virtual IApplication* at(std::size_t index) const = 0;
    [[nodiscard]] virtual std::size_t count() const = 0;
};

} // namespace espscreen::core
