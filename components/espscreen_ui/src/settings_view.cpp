#include "espscreen/ui/settings_view.hpp"

#include "motion.hpp"
#include "lvgl.h"

#include <cstdio>

namespace espscreen::ui {
namespace {

constexpr std::size_t index_of(SettingId setting) {
  return static_cast<std::size_t>(setting);
}

void make_static(lv_obj_t *object) {
  lv_obj_remove_flag(object, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_scrollbar_mode(object, LV_SCROLLBAR_MODE_OFF);
}

void make_clear(lv_obj_t *object) {
  lv_obj_set_style_bg_opa(object, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(object, 0, 0);
  lv_obj_set_style_pad_all(object, 0, 0);
  make_static(object);
}

void set_percent(lv_obj_t *label, std::int32_t value, bool off_when_zero) {
  char text[24];
  if (off_when_zero && value == 0)
    std::snprintf(text, sizeof(text), "Off");
  else
    std::snprintf(text, sizeof(text), "%ld%%", static_cast<long>(value));
  lv_label_set_text(label, text);
}

void set_value(lv_obj_t* label, const SettingDescriptor& setting,
               std::int32_t value) {
  if (setting.presentation == SettingPresentation::minutes_or_never) {
    char text[24];
    if (value)
      std::snprintf(text, sizeof(text), "%ld min",
                    static_cast<long>((value + 59) / 60));
    else
      std::snprintf(text, sizeof(text), "Never");
    lv_label_set_text(label, text);
    return;
  }
  set_percent(label, value,
              setting.presentation == SettingPresentation::percent_or_off);
}

} // namespace

void SettingsView::build(lv_obj_t *parent, const SettingsViewModel &model,
                         IShellActionSink &actions) {
  if (root_)
    return;

  actions_ = &actions;
  const int width = lv_display_get_horizontal_resolution(nullptr);
  const int height = lv_display_get_vertical_resolution(nullptr);
  const bool compact = height <= 320;
  root_ = lv_obj_create(parent);
  lv_obj_add_flag(root_, LV_OBJ_FLAG_FLOATING);
  lv_obj_set_pos(root_, 0, 0);
  lv_obj_set_size(root_, width, height);
  lv_obj_set_flex_flow(root_, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_all(root_, compact ? 10 : 16, 0);
  lv_obj_set_style_pad_row(root_, 8, 0);
  lv_obj_set_style_radius(root_, 0, 0);
  lv_obj_set_scroll_dir(root_, LV_DIR_VER);
  lv_obj_set_scrollbar_mode(root_, LV_SCROLLBAR_MODE_ACTIVE);

  auto *header = lv_obj_create(root_);
  lv_obj_set_size(header, LV_PCT(100), 44);
  lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(header, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);
  make_clear(header);
  auto *title = lv_label_create(header);
  lv_label_set_text(title, "Settings");
  lv_obj_set_style_text_color(title, lv_color_hex(0x5B7CFA), 0);
  lv_obj_set_flex_grow(title, 1);
  auto *close = lv_button_create(header);
  lv_obj_set_size(close, 42, 36);
  auto *close_label = lv_label_create(close);
  lv_label_set_text(close_label, LV_SYMBOL_LEFT);
  lv_obj_center(close_label);
  lv_obj_add_event_cb(close, &SettingsView::close_callback, LV_EVENT_CLICKED,
                      this);

  for (const auto& setting : ShellSettings::settings)
    add_row(setting, model.*setting.value / setting.model_scale);

  auto *hint = lv_label_create(root_);
  lv_label_set_text(hint, "Scale applies after restart");
  lv_obj_set_style_text_opa(hint, LV_OPA_60, 0);
  update(model);
  lv_obj_add_flag(root_, LV_OBJ_FLAG_HIDDEN);
}

void SettingsView::add_row(const SettingDescriptor& setting,
                           std::int32_t current) {
  auto *box = lv_obj_create(root_);
  lv_obj_set_size(box, LV_PCT(100), 58);
  lv_obj_set_flex_flow(box, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_all(box, 5, 0);
  lv_obj_set_style_pad_row(box, 4, 0);
  make_static(box);
  auto *header = lv_obj_create(box);
  lv_obj_set_size(header, LV_PCT(100), 18);
  lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
  make_clear(header);
  auto *name = lv_label_create(header);
  lv_label_set_text(name, setting.title.data());
  lv_obj_set_flex_grow(name, 1);

  const auto index = index_of(setting.id);
  labels_[index] = lv_label_create(header);
  auto *slider = lv_slider_create(box);
  lv_obj_set_width(slider, LV_PCT(100));
  lv_obj_set_height(slider, 12);
  lv_slider_set_range(slider, setting.slider_minimum, setting.slider_maximum);
  lv_slider_set_value(slider, current, LV_ANIM_ON);
  bindings_[index] = {this, setting.id};
  lv_obj_add_event_cb(slider, &SettingsView::changed_callback,
                      LV_EVENT_VALUE_CHANGED, &bindings_[index]);
  lv_obj_add_event_cb(slider, &SettingsView::released_callback,
                      LV_EVENT_RELEASED, &bindings_[index]);
}

void SettingsView::reset() {
  root_ = nullptr;
  labels_.fill(nullptr);
  actions_ = nullptr;
}

void SettingsView::update(const SettingsViewModel &model) {
  if (!root_)
    return;
  for (const auto& setting : ShellSettings::settings)
    set_value(labels_[index_of(setting.id)], setting, model.*setting.value);
}

void SettingsView::show() {
  if (root_)
    motion::show(root_, lv_display_get_horizontal_resolution(nullptr) / 2);
}

void SettingsView::hide() {
  if (root_)
    motion::hide(root_, lv_display_get_horizontal_resolution(nullptr) / 2);
}

void SettingsView::set_hidden(bool hidden) {
  if (!root_)
    return;
  if (hidden)
    lv_obj_add_flag(root_, LV_OBJ_FLAG_HIDDEN);
  else
    lv_obj_remove_flag(root_, LV_OBJ_FLAG_HIDDEN);
}

void SettingsView::apply_theme(const ThemePalette &palette) {
  if (!root_)
    return;
  lv_obj_set_style_bg_color(root_, lv_color_hex(palette.surface), 0);
  lv_obj_set_style_border_color(root_, lv_color_hex(palette.border), 0);
  lv_obj_set_style_border_width(root_, 1, 0);
}

void SettingsView::close_callback(lv_event_t *event) {
  auto *view = static_cast<SettingsView *>(lv_event_get_user_data(event));
  view->dispatch({.kind = SettingsActionKind::close});
}

void SettingsView::changed_callback(lv_event_t *event) {
  const auto *binding =
      static_cast<SliderBinding *>(lv_event_get_user_data(event));
  binding->view->dispatch(
      {.kind = SettingsActionKind::value_changed,
       .setting = binding->setting,
       .value = lv_slider_get_value(lv_event_get_target_obj(event))});
}

void SettingsView::released_callback(lv_event_t *event) {
  const auto *binding =
      static_cast<SliderBinding *>(lv_event_get_user_data(event));
  binding->view->dispatch(
      {.kind = SettingsActionKind::commit, .setting = binding->setting});
}

void SettingsView::dispatch(SettingsAction action) const {
  if (actions_)
    actions_->on(action);
}

} // namespace espscreen::ui
