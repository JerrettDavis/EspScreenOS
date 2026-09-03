#include "espscreen/ui/header_view.hpp"

#include "motion.hpp"
#include "lvgl.h"

#include <cstdio>

namespace espscreen::ui {
namespace {

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

} // namespace

void HeaderView::build(lv_obj_t *parent, IShellActionSink &actions) {
  if (root_)
    return;
  actions_ = &actions;
  compact_ = lv_display_get_vertical_resolution(nullptr) <= 320;
  root_ = lv_obj_create(parent);
  lv_obj_set_size(root_, LV_PCT(100), compact_ ? 42 : 58);
  lv_obj_set_flex_flow(root_,
                       compact_ ? LV_FLEX_FLOW_ROW : LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_all(root_, compact_ ? 6 : 8, 0);
  lv_obj_set_style_pad_row(root_, 2, 0);
  lv_obj_set_style_radius(root_, 14, 0);
  make_static(root_);

  auto *title_row = lv_obj_create(root_);
  lv_obj_set_size(title_row, compact_ ? LV_PCT(52) : LV_PCT(100),
                  compact_ ? LV_PCT(100) : 28);
  lv_obj_set_flex_flow(title_row, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(title_row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);
  make_clear(title_row);
  title_ = lv_label_create(title_row);
  lv_label_set_text(title_, "EspScreen OS");
  lv_label_set_long_mode(title_, LV_LABEL_LONG_DOT);
  lv_obj_set_flex_grow(title_, 1);

  theme_button_ = lv_button_create(title_row);
  lv_obj_set_size(theme_button_, compact_ ? 38 : 82, 28);
  lv_obj_set_style_radius(theme_button_, 14, 0);
  lv_obj_set_style_shadow_width(theme_button_, 0, 0);
  theme_label_ = lv_label_create(theme_button_);
  lv_obj_center(theme_label_);
  lv_obj_add_event_cb(theme_button_, &HeaderView::theme_callback,
                      LV_EVENT_CLICKED, this);

  metrics_ = lv_obj_create(root_);
  lv_obj_set_size(metrics_, compact_ ? LV_PCT(48) : LV_PCT(100),
                  compact_ ? LV_PCT(100) : 18);
  lv_obj_set_flex_flow(metrics_, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(metrics_, LV_FLEX_ALIGN_SPACE_BETWEEN,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  make_clear(metrics_);
  memory_ = lv_label_create(metrics_);
  lv_label_set_text(memory_, compact_ ? "--K" : "Heap --");
  battery_ = lv_label_create(metrics_);
  lv_label_set_text(battery_, LV_SYMBOL_BATTERY_EMPTY " --");
  storage_ = lv_label_create(metrics_);
  lv_label_set_text(storage_, LV_SYMBOL_SD_CARD " --");
  if (compact_)
    lv_obj_add_flag(storage_, LV_OBJ_FLAG_HIDDEN);
}

void HeaderView::reset() {
  root_ = metrics_ = title_ = theme_button_ = theme_label_ = battery_ =
      memory_ = storage_ = nullptr;
  actions_ = nullptr;
}

void HeaderView::set_title(const char *title) {
  if (title_)
    lv_label_set_text(title_, title);
}

void HeaderView::update(const StatusViewModel &model) {
  if (!root_)
    return;
  char text[64];
  std::snprintf(text, sizeof(text), compact_ ? "%luK" : "Heap %luK",
                static_cast<unsigned long>(model.free_heap_bytes / 1024));
  lv_label_set_text(memory_, text);
  if (model.battery_percent >= 0)
    std::snprintf(text, sizeof(text), LV_SYMBOL_BATTERY_FULL " %d%%",
                  static_cast<int>(model.battery_percent));
  else
    std::snprintf(text, sizeof(text), LV_SYMBOL_BATTERY_EMPTY " n/a");
  lv_label_set_text(battery_, text);
  if (model.storage_mounted)
    std::snprintf(
        text, sizeof(text), LV_SYMBOL_SD_CARD " %lluM",
        static_cast<unsigned long long>(model.storage_free_bytes /
                                        (1024 * 1024)));
  else
    std::snprintf(text, sizeof(text), LV_SYMBOL_SD_CARD " absent");
  lv_label_set_text(storage_, text);
}

void HeaderView::set_hidden(bool hidden) {
  if (!root_)
    return;
  if (hidden)
    lv_obj_add_flag(root_, LV_OBJ_FLAG_HIDDEN);
  else
    lv_obj_remove_flag(root_, LV_OBJ_FLAG_HIDDEN);
}

void HeaderView::apply_theme(const ThemePalette &palette) {
  if (!root_)
    return;
  lv_obj_set_style_bg_color(root_, lv_color_hex(palette.surface), 0);
  lv_obj_set_style_border_color(root_, lv_color_hex(palette.border), 0);
  lv_obj_set_style_border_width(root_, 1, 0);
  lv_obj_set_style_text_color(title_, lv_color_hex(palette.accent), 0);
  lv_obj_set_style_text_color(metrics_, lv_color_hex(palette.muted), 0);
  lv_obj_set_style_bg_color(theme_button_, lv_color_hex(palette.raised), 0);
  lv_obj_set_style_text_color(theme_button_, lv_color_hex(palette.text), 0);
  lv_label_set_text(theme_label_,
                    compact_ ? LV_SYMBOL_EYE_OPEN
                             : (palette.dark ? LV_SYMBOL_EYE_OPEN " Light"
                                             : LV_SYMBOL_EYE_CLOSE " Dark"));
}

void HeaderView::theme_callback(lv_event_t *event) {
  auto *view = static_cast<HeaderView *>(lv_event_get_user_data(event));
  motion::pulse(view->theme_button_);
  view->dispatch(HeaderAction::toggle_theme);
}

void HeaderView::dispatch(HeaderAction action) const {
  if (actions_)
    actions_->on(action);
}

} // namespace espscreen::ui
