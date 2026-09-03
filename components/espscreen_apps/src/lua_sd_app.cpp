#include "espscreen/apps/lua_sd_app.hpp"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}
#include <cstdio>

namespace espscreen::apps {

void LuaApplicationRuntime::restrict_environment() {
    for (const char* name : {"dofile", "loadfile", "load", "require", "os",
                             "io", "package", "debug"}) {
        lua_pushnil(state_);
        lua_setglobal(state_, name);
    }
}

bool LuaApplicationRuntime::inspect(std::string_view path,
                                    ExternalApplicationDescriptor& descriptor) {
    if (path.empty()) return false;
    auto* state = luaL_newstate();
    if (!state) return false;
    const bool loaded = luaL_loadfile(state, path.data()) == LUA_OK &&
                        lua_pcall(state, 0, 0, 0) == LUA_OK;
    if (!loaded) {
        lua_close(state);
        return false;
    }
    lua_getglobal(state, "APP_ID");
    lua_getglobal(state, "APP_TITLE");
    const char* id = lua_tostring(state, -2);
    const char* title = lua_tostring(state, -1);
    const bool valid = id && title && *id && *title;
    std::snprintf(descriptor.id.data(), descriptor.id.size(), "%s", id ? id : "");
    std::snprintf(descriptor.title.data(), descriptor.title.size(), "%s",
                  title ? title : "");
    lua_close(state);
    return valid;
}

core::Status LuaApplicationRuntime::start(std::string_view path,
                                          core::ApplicationSurface surface) {
    state_ = luaL_newstate();
    if (!state_) return {core::ErrorCode::unavailable, "Lua allocation failed"};
    luaL_openlibs(state_);
    ui_.install(state_, surface, *this);
    restrict_environment();
    if (luaL_loadfile(state_, path.data()) != LUA_OK ||
        lua_pcall(state_, 0, 0, 0) != LUA_OK || !call("build")) {
        return {core::ErrorCode::invalid_argument, "SD app failed"};
    }
    return core::Status::success();
}

void LuaApplicationRuntime::stop() {
    ui_.reset();
    if (state_) {
        lua_close(state_);
        state_ = nullptr;
    }
}

void LuaApplicationRuntime::invoke(std::string_view function) {
    if (state_) (void)call(function);
}

bool LuaApplicationRuntime::call(std::string_view function) {
    lua_getglobal(state_, function.data());
    if (!lua_isfunction(state_, -1)) {
        lua_pop(state_, 1);
        return false;
    }
    return lua_pcall(state_, 0, 0, 0) == LUA_OK;
}

core::IApplication* LuaSdAppFactory::create(std::string_view path) {
    if (count_ >= applications_.size()) return nullptr;
    auto& candidate = applications_[count_];
    if (!candidate.configure(runtimes_[count_], path)) return nullptr;
    ++count_;
    return &candidate;
}

void LuaSdAppFactory::restore(std::size_t checkpoint) {
    if (checkpoint <= count_) count_ = checkpoint;
}

} // namespace espscreen::apps
