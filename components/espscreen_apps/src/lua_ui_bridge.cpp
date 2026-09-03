#include "espscreen/apps/lua_ui_bridge.hpp"
#include "ui_helpers.hpp"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}
#include "lvgl.h"
#include <cstdio>

namespace espscreen::apps {

LuaUiBridge* LuaUiBridge::self(lua_State* state) {
    return *static_cast<LuaUiBridge**>(lua_getextraspace(state));
}

void LuaUiBridge::install(lua_State* state, core::ApplicationSurface surface,
                          ILuaUiActions& actions) {
    state_ = state;
    root_ = ui::lvgl_root(surface);
    lv_obj_clean(root_);
    lv_obj_set_flex_flow(root_, LV_FLEX_FLOW_COLUMN);
    actions_ = &actions;
    callback_count_ = 0;
    *static_cast<LuaUiBridge**>(lua_getextraspace(state_)) = this;
    lua_newtable(state_);
    const luaL_Reg api[]{{"label", ui_label}, {"button", ui_button},
                         {"textarea", ui_textarea}, {"get_text", ui_get_text},
                         {"set_text", ui_set_text}, {nullptr, nullptr}};
    luaL_setfuncs(state_, api, 0);
    lua_setglobal(state_, "ui");
}

void LuaUiBridge::reset() {
    actions_ = nullptr;
    state_ = nullptr;
    if (root_) lv_obj_clean(root_);
    callbacks_ = {};
    callback_count_ = 0;
    root_ = nullptr;
}

int LuaUiBridge::ui_label(lua_State* state) {
    auto* label = lv_label_create(self(state)->root_);
    lv_label_set_text(label, luaL_checkstring(state, 1));
    lua_pushlightuserdata(state, label);
    return 1;
}

int LuaUiBridge::ui_button(lua_State* state) {
    auto& bridge = *self(state);
    const char* text = luaL_checkstring(state, 1);
    const char* function = luaL_checkstring(state, 2);
    if (bridge.callback_count_ >= bridge.callbacks_.size())
        return luaL_error(state, "button limit reached");
    auto* button = lv_button_create(bridge.root_);
    auto* label = lv_label_create(button);
    lv_label_set_text(label, text);
    lv_obj_center(label);
    auto& callback = bridge.callbacks_[bridge.callback_count_++];
    callback.bridge = &bridge;
    std::snprintf(callback.function, sizeof(callback.function), "%s", function);
    lv_obj_add_event_cb(button, &button_callback, LV_EVENT_CLICKED, &callback);
    lua_pushlightuserdata(state, button);
    return 1;
}

int LuaUiBridge::ui_textarea(lua_State* state) {
    auto* area = lv_textarea_create(self(state)->root_);
    if (lua_gettop(state)) lv_textarea_set_text(area, luaL_checkstring(state, 1));
    lv_obj_set_width(area, LV_PCT(100));
    lua_pushlightuserdata(state, area);
    return 1;
}

int LuaUiBridge::ui_get_text(lua_State* state) {
    auto* object = static_cast<lv_obj_t*>(lua_touserdata(state, 1));
    luaL_argcheck(state, object, 1, "widget required");
    lua_pushstring(state, lv_textarea_get_text(object));
    return 1;
}

int LuaUiBridge::ui_set_text(lua_State* state) {
    auto* object = static_cast<lv_obj_t*>(lua_touserdata(state, 1));
    luaL_argcheck(state, object, 1, "widget required");
    const char* text = luaL_checkstring(state, 2);
    if (lv_obj_check_type(object, &lv_label_class))
        lv_label_set_text(object, text);
    else if (lv_obj_check_type(object, &lv_textarea_class))
        lv_textarea_set_text(object, text);
    return 0;
}

void LuaUiBridge::button_callback(lv_event_t* event) {
    auto* callback = static_cast<Callback*>(lv_event_get_user_data(event));
    if (!callback || !callback->bridge) return;
    auto& bridge = *callback->bridge;
    if (bridge.state_ && bridge.actions_)
        bridge.actions_->invoke(callback->function);
}

} // namespace espscreen::apps
