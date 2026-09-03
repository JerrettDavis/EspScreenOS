#include "espscreen/apps/hardware_test_view.hpp"
#include "lvgl.h"
#include "ui_helpers.hpp"
#include <array>
#include <cstdio>

namespace espscreen::apps {
namespace {

lv_obj_t* make_action(lv_obj_t* parent, const char* text) {
    auto* button = lv_button_create(parent);
    lv_obj_set_size(button, LV_PCT(48), 42);
    lv_obj_set_style_radius(button, 12, 0);
    lv_obj_set_style_shadow_width(button, 0, 0);
    auto* label = lv_label_create(button);
    lv_label_set_text(label, text);
    lv_obj_center(label);
    return button;
}

} // namespace

void HardwareTestView::build(core::ApplicationSurface surface,
                             const HardwareTestViewModel& model,
                             IHardwareTestActions& actions) {
    root_ = ui::lvgl_root(surface);
    ui::prepare_page(root_, true);
    auto* title = lv_label_create(root_);
    lv_label_set_text(title, "Haptics & peripherals");
    ui::heading(title);
    result_ = lv_label_create(root_);
    set_message(model.haptics_available ? "Motor is off" : "Motor unavailable on this board");
    lv_label_set_long_mode(result_, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(result_, LV_PCT(100));
    lv_obj_set_style_text_opa(result_, LV_OPA_70, 0);

    auto* enable_card = lv_obj_create(root_);
    lv_obj_set_size(enable_card, LV_PCT(100), 48);
    lv_obj_set_flex_flow(enable_card, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(enable_card, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(enable_card, 8, 0);
    ui::card(enable_card);
    auto* enable_label = lv_label_create(enable_card);
    lv_label_set_text(enable_label, "Enable haptics");
    lv_obj_set_flex_grow(enable_label, 1);
    auto* toggle = lv_switch_create(enable_card);
    if (model.enabled) lv_obj_add_state(toggle, LV_STATE_CHECKED);
    actions_[0] = {this, &actions, {HardwareViewActionKind::enabled_changed, 0}};
    lv_obj_add_event_cb(toggle, &action_callback, LV_EVENT_VALUE_CHANGED, &actions_[0]);

    auto* intensity_card = lv_obj_create(root_);
    lv_obj_set_size(intensity_card, LV_PCT(100), 62);
    lv_obj_set_flex_flow(intensity_card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(intensity_card, 7, 0);
    lv_obj_set_style_pad_row(intensity_card, 6, 0);
    ui::card(intensity_card);
    intensity_label_ = lv_label_create(intensity_card);
    set_intensity(model.intensity);
    auto* slider = lv_slider_create(intensity_card);
    lv_slider_set_range(slider, 0, 100);
    lv_slider_set_value(slider, model.intensity, LV_ANIM_OFF);
    lv_obj_set_width(slider, LV_PCT(100));
    actions_[1] = {this, &actions, {HardwareViewActionKind::intensity_changed, 0}};
    lv_obj_add_event_cb(slider, &action_callback, LV_EVENT_VALUE_CHANGED, &actions_[1]);
    actions_[2] = {this, &actions, {HardwareViewActionKind::intensity_committed, 0}};
    lv_obj_add_event_cb(slider, &action_callback, LV_EVENT_RELEASED, &actions_[2]);

    auto* presets = lv_obj_create(root_);
    lv_obj_set_size(presets, LV_PCT(100), 46);
    lv_obj_set_flex_flow(presets, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(presets, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(presets, 2, 0);
    lv_obj_set_style_pad_column(presets, 4, 0);
    ui::static_container(presets);
    for (std::size_t index = 0; index < HardwareTestModel::presets.size(); ++index) {
        const auto preset = HardwareTestModel::presets[index];
        auto* button = lv_button_create(presets);
        lv_obj_set_size(button, LV_PCT(23), 38);
        lv_obj_set_style_radius(button, 10, 0);
        lv_obj_set_style_shadow_width(button, 0, 0);
        auto* label = lv_label_create(button);
        lv_label_set_text(label, preset.name);
        lv_obj_center(label);
        actions_[index + 3] = {this, &actions,
                               {HardwareViewActionKind::preview, preset.intensity}};
        lv_obj_add_event_cb(button, &action_callback, LV_EVENT_CLICKED,
                            &actions_[index + 3]);
    }

    auto* action_grid = lv_obj_create(root_);
    lv_obj_set_size(action_grid, LV_PCT(100), 96);
    lv_obj_set_flex_flow(action_grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(action_grid, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_all(action_grid, 3, 0);
    lv_obj_set_style_pad_row(action_grid, 5, 0);
    ui::static_container(action_grid);
    constexpr std::array names{"Test motor", "Stop motor", "Speaker tone", "Refresh SD"};
    for (std::size_t index = 0; index < names.size(); ++index) {
        auto* button = make_action(action_grid, names[index]);
        actions_[index + 7] = {
            this, &actions,
            {HardwareViewActionKind::perform, static_cast<std::int32_t>(index)}};
        lv_obj_add_event_cb(button, &action_callback, LV_EVENT_CLICKED,
                            &actions_[index + 7]);
    }
}

void HardwareTestView::set_intensity(std::uint8_t intensity) {
    if (!intensity_label_) return;
    char text[40]{};
    std::snprintf(text, sizeof(text), "Motor intensity | %u%%", intensity);
    lv_label_set_text(intensity_label_, text);
}

void HardwareTestView::set_message(const char* message) {
    if (result_) lv_label_set_text(result_, message ? message : "");
}

void HardwareTestView::show_storage(bool mounted, std::uint64_t free_bytes) {
    char status[64]{};
    std::snprintf(status, sizeof(status),
                  mounted ? "SD ready | %llu MB free" : "SD card not mounted",
                  static_cast<unsigned long long>(free_bytes / (1024 * 1024)));
    set_message(status);
}

void HardwareTestView::reset() {
    if (root_) lv_obj_clean(root_);
    root_ = result_ = intensity_label_ = nullptr;
    actions_ = {};
}

void HardwareTestView::action_callback(lv_event_t* event) {
    auto* binding = static_cast<ActionBinding*>(lv_event_get_user_data(event));
    if (!binding || !binding->actions) return;
    auto action = binding->action;
    if (action.kind == HardwareViewActionKind::enabled_changed)
        action.value = lv_obj_has_state(lv_event_get_target_obj(event), LV_STATE_CHECKED);
    else if (action.kind == HardwareViewActionKind::intensity_changed)
        action.value = lv_slider_get_value(lv_event_get_target_obj(event));
    binding->actions->perform(action);
}

} // namespace espscreen::apps
