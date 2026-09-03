#pragma once

#include "lvgl.h"
#include <array>
#include <cstdint>

namespace espscreen::ui::motion {

constexpr std::uint32_t quick_ms = 120;
constexpr std::uint32_t standard_ms = 220;

class ButtonStyle final {
public:
    ButtonStyle();
    void apply(lv_obj_t* object) const;
private:
    std::array<lv_style_prop_t, 3> properties_{};
    lv_style_transition_dsc_t transition_{};
};
void enter(lv_obj_t* object, int offset_x = 20, int offset_y = 0);
void show(lv_obj_t* object, int from_x, int from_y = 0);
void hide(lv_obj_t* object, int to_x, int to_y = 0);
void pulse(lv_obj_t* object);

} // namespace espscreen::ui::motion
