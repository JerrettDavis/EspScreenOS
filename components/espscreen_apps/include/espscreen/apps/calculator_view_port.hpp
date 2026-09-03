#pragma once

#include "espscreen/core/application.hpp"

#include <string_view>

namespace espscreen::apps {

class ICalculatorActions {
public:
    virtual ~ICalculatorActions() = default;
    virtual void press(std::string_view key) = 0;
};

class ICalculatorView {
public:
    virtual ~ICalculatorView() = default;
    virtual void build(core::ApplicationSurface surface, std::string_view display,
                       ICalculatorActions& actions) = 0;
    virtual void render(std::string_view display) = 0;
    virtual void reset() = 0;
};

} // namespace espscreen::apps
