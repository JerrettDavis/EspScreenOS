#include "espscreen/apps/calculator_view.hpp"
#include "lvgl.h"
#include "ui_helpers.hpp"

namespace espscreen::apps {
namespace {

constexpr const char* keypad[] = {
    "C", "+/-", "%", "/", "\n", "7", "8", "9", "*", "\n",
    "4", "5",   "6", "-", "\n", "1", "2", "3", "+", "\n",
    "0", ".",   "BS", "=", "",
};

} // namespace

void CalculatorView::build(core::ApplicationSurface surface, std::string_view display,
                           ICalculatorActions& actions) {
    root_ = ui::lvgl_root(surface);
    actions_ = &actions;
    ui::prepare_page(root_);
    display_ = lv_label_create(root_);
    render(display);
    lv_label_set_long_mode(display_, LV_LABEL_LONG_CLIP);
    lv_obj_set_size(display_, LV_PCT(100), 52);
    lv_obj_set_style_text_align(display_, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_set_style_pad_all(display_, 12, 0);
    auto* matrix = lv_buttonmatrix_create(root_);
    lv_buttonmatrix_set_map(matrix, keypad);
    lv_obj_set_width(matrix, LV_PCT(100));
    lv_obj_set_flex_grow(matrix, 1);
    lv_obj_set_style_pad_all(matrix, 5, 0);
    lv_obj_set_style_pad_row(matrix, 5, LV_PART_MAIN);
    lv_obj_set_style_pad_column(matrix, 5, LV_PART_MAIN);
    lv_obj_set_style_radius(matrix, 12, LV_PART_ITEMS);
    lv_obj_set_style_shadow_width(matrix, 0, LV_PART_ITEMS);
    lv_obj_add_event_cb(matrix, &CalculatorView::button_callback,
                        LV_EVENT_VALUE_CHANGED, this);
}

void CalculatorView::render(std::string_view display) {
    if (display_)
        lv_label_set_text_fmt(display_, "%.*s", static_cast<int>(display.size()),
                              display.data());
}

void CalculatorView::reset() {
    if (root_) lv_obj_clean(root_);
    root_ = display_ = nullptr;
    actions_ = nullptr;
}

void CalculatorView::button_callback(lv_event_t* event) {
    auto& view = *static_cast<CalculatorView*>(lv_event_get_user_data(event));
    auto* target = lv_event_get_target_obj(event);
    const auto selected = lv_buttonmatrix_get_selected_button(target);
    const char* key = lv_buttonmatrix_get_button_text(target, selected);
    if (key && view.actions_) view.actions_->press(key);
}

} // namespace espscreen::apps
