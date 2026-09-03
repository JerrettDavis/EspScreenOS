#include "espscreen/apps/gpio_toolbox_view.hpp"
#include "lvgl.h"
#include "ui_helpers.hpp"
#include <cstdio>

namespace espscreen::apps {

void GpioToolboxView::build(core::ApplicationSurface surface,
                            IGpioToolboxActions& actions) {
    root_ = ui::lvgl_root(surface);
    ui::prepare_page(root_);
    auto* title = lv_label_create(root_);
    lv_label_set_text(title, "Expansion GPIO");
    ui::heading(title);
    status_ = lv_label_create(root_);
    lv_label_set_text(status_, "Board-safe, available header pins");
    lv_obj_set_style_text_opa(status_, LV_OPA_60, 0);
    list_ = lv_obj_create(root_);
    lv_obj_set_width(list_, LV_PCT(100));
    lv_obj_set_flex_grow(list_, 1);
    lv_obj_set_flex_flow(list_, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scroll_dir(list_, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(list_, LV_SCROLLBAR_MODE_ACTIVE);
    lv_obj_set_style_pad_all(list_, 4, 0);
    lv_obj_set_style_pad_row(list_, 6, 0);
    for (auto& binding : bindings_) binding = {&actions, -1};
    count_ = 0;
}

bool GpioToolboxView::add_pin(const char* header_name, std::int16_t pin) {
    if (!list_ || count_ >= bindings_.size()) return false;
    auto* row = lv_obj_create(list_);
    lv_obj_set_size(row, LV_PCT(100), 44);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(row, 7, 0);
    ui::card(row);
    char text[48]{};
    std::snprintf(text, sizeof(text), "%s | GPIO %d", header_name, pin);
    auto* label = lv_label_create(row);
    lv_label_set_text(label, text);
    lv_obj_set_flex_grow(label, 1);
    auto* toggle = lv_switch_create(row);
    bindings_[count_].pin = pin;
    lv_obj_add_event_cb(toggle, &pin_callback, LV_EVENT_VALUE_CHANGED,
                        &bindings_[count_++]);
    return true;
}

void GpioToolboxView::finish() {
    if (count_ == 0 && status_)
        lv_label_set_text(status_, "No unreserved GPIO header pins");
}

void GpioToolboxView::reset() {
    if (root_) lv_obj_clean(root_);
    root_ = status_ = list_ = nullptr;
    bindings_ = {};
    count_ = 0;
}

void GpioToolboxView::pin_callback(lv_event_t* event) {
    auto* binding = static_cast<PinBinding*>(lv_event_get_user_data(event));
    if (!binding || !binding->actions) return;
    const bool high = lv_obj_has_state(lv_event_get_target_obj(event), LV_STATE_CHECKED);
    binding->actions->write(binding->pin, high);
}

} // namespace espscreen::apps
