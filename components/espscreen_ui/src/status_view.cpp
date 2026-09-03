#include "espscreen/ui/status_view.hpp"

#include "motion.hpp"
#include "lvgl.h"

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

void StatusView::build(lv_obj_t *parent, IShellActionSink &actions,
                       const motion::ButtonStyle &button_style) {
  if (root_)
    return;

  actions_ = &actions;
  const int width = lv_display_get_horizontal_resolution(nullptr);
  const int height = lv_display_get_vertical_resolution(nullptr);
  root_ = lv_obj_create(parent);
  lv_obj_add_flag(root_, LV_OBJ_FLAG_FLOATING);
  lv_obj_set_pos(root_, 0, 0);
  lv_obj_set_size(root_, width, height);
  lv_obj_set_flex_flow(root_, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_all(root_, 12, 0);
  lv_obj_set_style_pad_row(root_, 8, 0);
  lv_obj_set_style_radius(root_, 0, 0);
  make_static(root_);

  auto *header = lv_obj_create(root_);
  lv_obj_set_size(header, LV_PCT(100), 38);
  lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(header, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);
  make_clear(header);
  auto *title = lv_label_create(header);
  lv_label_set_text(title, "Quick status");
  lv_obj_set_style_text_color(title, lv_color_hex(0x5B7CFA), 0);
  lv_obj_set_flex_grow(title, 1);

  auto *close = lv_button_create(header);
  lv_obj_set_size(close, 42, 32);
  auto *close_label = lv_label_create(close);
  lv_label_set_text(close_label, LV_SYMBOL_UP);
  lv_obj_center(close_label);
  action_bindings_[0] = {this, StatusAction::close};
  lv_obj_add_event_cb(close, &StatusView::action_callback, LV_EVENT_CLICKED,
                      &action_bindings_[0]);

  auto *metrics = lv_obj_create(root_);
  lv_obj_set_size(metrics, LV_PCT(100), 86);
  lv_obj_set_flex_flow(metrics, LV_FLEX_FLOW_ROW_WRAP);
  lv_obj_set_flex_align(metrics, LV_FLEX_ALIGN_SPACE_BETWEEN,
                        LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_all(metrics, 8, 0);
  make_static(metrics);
  for (auto **label : {&battery_, &network_, &memory_, &storage_}) {
    *label = lv_label_create(metrics);
    lv_obj_set_width(*label, LV_PCT(47));
    lv_label_set_long_mode(*label, LV_LABEL_LONG_DOT);
  }
  lv_label_set_text(battery_, LV_SYMBOL_BATTERY_EMPTY " Battery --");
  lv_label_set_text(network_, LV_SYMBOL_WIFI " Network --");
  lv_label_set_text(memory_, LV_SYMBOL_DRIVE " Heap --");
  lv_label_set_text(storage_, LV_SYMBOL_SD_CARD " Storage --");

  auto *action_row = lv_obj_create(root_);
  lv_obj_set_width(action_row, LV_PCT(100));
  lv_obj_set_flex_grow(action_row, 1);
  lv_obj_set_flex_flow(action_row, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(action_row, LV_FLEX_ALIGN_SPACE_EVENLY,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  make_static(action_row);
  constexpr const char *names[] = {
      LV_SYMBOL_EYE_OPEN " Theme", LV_SYMBOL_MINUS " Dim",
      LV_SYMBOL_PLUS " Bright", LV_SYMBOL_SETTINGS " Settings"};
  constexpr StatusAction action_ids[] = {
      StatusAction::toggle_theme, StatusAction::dim, StatusAction::brighten,
      StatusAction::open_settings};
  for (std::size_t index = 0; index < std::size(names); ++index) {
    auto *button = lv_button_create(action_row);
    lv_obj_set_size(button, 100, 52);
    lv_obj_set_style_radius(button, 14, 0);
    auto *label = lv_label_create(button);
    lv_label_set_text(label, names[index]);
    lv_obj_center(label);
    action_bindings_[index + 1] = {this, action_ids[index]};
    lv_obj_add_event_cb(button, &StatusView::action_callback, LV_EVENT_CLICKED,
                        &action_bindings_[index + 1]);
    button_style.apply(button);
  }
  button_style.apply(close);
  lv_obj_add_flag(root_, LV_OBJ_FLAG_HIDDEN);
}

void StatusView::reset() {
  root_ = battery_ = memory_ = storage_ = network_ = nullptr;
  actions_ = nullptr;
}

void StatusView::show() {
  if (!root_)
    return;
  lv_obj_remove_flag(root_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_move_foreground(root_);
  motion::show(root_, 0, -lv_display_get_vertical_resolution(nullptr));
}

void StatusView::hide() {
  if (root_)
    motion::hide(root_, 0, -lv_display_get_vertical_resolution(nullptr));
}

void StatusView::set_hidden(bool hidden) {
  if (!root_)
    return;
  if (hidden)
    lv_obj_add_flag(root_, LV_OBJ_FLAG_HIDDEN);
  else
    lv_obj_remove_flag(root_, LV_OBJ_FLAG_HIDDEN);
}

void StatusView::apply_theme(const ThemePalette &palette) {
  if (!root_)
    return;
  lv_obj_set_style_bg_color(root_, lv_color_hex(palette.surface), 0);
  lv_obj_set_style_border_color(root_, lv_color_hex(palette.border), 0);
  lv_obj_set_style_border_width(root_, 1, 0);
}

void StatusView::update(const StatusViewModel &model) {
  if (!root_)
    return;
  lv_label_set_text_fmt(memory_, LV_SYMBOL_DRIVE " Heap %luK",
                        static_cast<unsigned long>(model.free_heap_bytes / 1024));
  if (model.battery_percent >= 0)
    lv_label_set_text_fmt(battery_, LV_SYMBOL_BATTERY_FULL " Battery %d%%",
                          static_cast<int>(model.battery_percent));
  else
    lv_label_set_text(battery_, LV_SYMBOL_BATTERY_EMPTY " Battery n/a");
  if (model.storage_mounted)
    lv_label_set_text_fmt(
        storage_, LV_SYMBOL_SD_CARD " Free %lluM",
        static_cast<unsigned long long>(model.storage_free_bytes /
                                        (1024 * 1024)));
  else
    lv_label_set_text(storage_, LV_SYMBOL_SD_CARD " No card");
}

void StatusView::update_network(std::int32_t state) {
  if (!network_)
    return;
  const char *text = state == 2   ? "Connected"
                     : state == 3 ? "Setup hotspot"
                     : state == 1 ? "Connecting"
                                  : "Offline";
  lv_label_set_text_fmt(network_, LV_SYMBOL_WIFI " %s", text);
}

void StatusView::action_callback(lv_event_t *event) {
  const auto *binding =
      static_cast<ActionBinding *>(lv_event_get_user_data(event));
  binding->view->dispatch(binding->action);
}

void StatusView::dispatch(StatusAction action) const {
  if (actions_)
    actions_->on(action);
}

} // namespace espscreen::ui
