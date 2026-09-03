#pragma once

#include "espscreen/core/application.hpp"
#include <string_view>

struct lua_State;

namespace espscreen::apps {

class ILuaUiActions {
public:
    virtual ~ILuaUiActions() = default;
    virtual void invoke(std::string_view function) = 0;
};

class ILuaUi {
public:
    virtual ~ILuaUi() = default;
    virtual void install(lua_State* state, core::ApplicationSurface root,
                         ILuaUiActions& actions) = 0;
    virtual void reset() = 0;
};

} // namespace espscreen::apps
