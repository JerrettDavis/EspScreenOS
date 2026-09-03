#pragma once

#include "espscreen/core/application.hpp"
#include <cstdint>

namespace espscreen::apps {

class IGpioToolboxActions {
public:
    virtual ~IGpioToolboxActions() = default;
    virtual void write(std::int16_t pin, bool high) = 0;
};

class IGpioToolboxView {
public:
    virtual ~IGpioToolboxView() = default;
    virtual void build(core::ApplicationSurface root,
                       IGpioToolboxActions& actions) = 0;
    [[nodiscard]] virtual bool add_pin(const char* header_name, std::int16_t pin) = 0;
    virtual void finish() = 0;
    virtual void reset() = 0;
};

} // namespace espscreen::apps
