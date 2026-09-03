#pragma once

#include "espscreen/apps/lua_ui.hpp"
#include <array>
#include <cstddef>
#include <string_view>

struct lua_State;
struct lv_event_t;
struct lv_obj_t;

namespace espscreen::apps {

class LuaUiBridge final : public ILuaUi {
public:
    void install(lua_State* state, core::ApplicationSurface root,
                 ILuaUiActions& actions) override;
    void reset() override;

private:
    struct Callback final {
        LuaUiBridge* bridge{nullptr};
        char function[32]{};
    };

    [[nodiscard]] static LuaUiBridge* self(lua_State* state);
    static int ui_label(lua_State* state);
    static int ui_button(lua_State* state);
    static int ui_textarea(lua_State* state);
    static int ui_get_text(lua_State* state);
    static int ui_set_text(lua_State* state);
    static void button_callback(lv_event_t* event);

    lua_State* state_{nullptr};
    lv_obj_t* root_{nullptr};
    ILuaUiActions* actions_{nullptr};
    std::array<Callback, 16> callbacks_{};
    std::size_t callback_count_{0};
};

} // namespace espscreen::apps
