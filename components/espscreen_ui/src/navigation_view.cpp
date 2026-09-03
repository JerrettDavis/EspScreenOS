#include "espscreen/ui/navigation_view.hpp"

#include "lvgl.h"

#include <cstring>

namespace espscreen::ui {
namespace {

struct NavigationItem final {
  const char *application_id;
  const char *icon;
  const char *label;
  NavigationActionKind action;
};

constexpr NavigationItem items[] = {
    {"dashboard", LV_SYMBOL_HOME, "Home",
     NavigationActionKind::open_application},
    {nullptr, LV_SYMBOL_LIST, "Apps", NavigationActionKind::open_drawer},
    {"hardware", LV_SYMBOL_WARNING, "Test",
     NavigationActionKind::open_application},
    {"board", LV_SYMBOL_SETTINGS, "Board",
     NavigationActionKind::open_application},
};

} // namespace

void NavigationView::build(lv_obj_t *parent, IShellActionSink &actions) {
  if (root_)
    return;

  actions_ = &actions;
  const bool compact = lv_display_get_vertical_resolution(nullptr) <= 320;
  root_ = lv_obj_create(parent);
  lv_obj_set_size(root_, LV_PCT(100), compact ? 48 : 58);
  lv_obj_set_flex_flow(root_, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(root_, LV_FLEX_ALIGN_SPACE_EVENLY,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_all(root_, compact ? 3 : 5, 0);
  lv_obj_set_style_radius(root_, 16, 0);
  lv_obj_remove_flag(root_, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_scrollbar_mode(root_, LV_SCROLLBAR_MODE_OFF);

  for (std::size_t index = 0; index < std::size(items); ++index) {
    auto *button = lv_button_create(root_);
    buttons_[index] = button;
    lv_obj_set_size(button, compact ? 82 : 66, compact ? 38 : 46);
    lv_obj_set_flex_flow(button,
                         compact ? LV_FLEX_FLOW_ROW : LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(button, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_radius(button, 12, 0);
    lv_obj_set_style_shadow_width(button, 0, 0);
    lv_obj_set_style_pad_all(button, 3, 0);
    lv_obj_set_style_pad_column(button, 5, 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x5B7CFA),
                              LV_STATE_CHECKED);
    lv_obj_set_style_text_color(button, lv_color_hex(0xFFFFFF),
                                LV_STATE_CHECKED);
    auto *icon = lv_label_create(button);
    lv_label_set_text(icon, items[index].icon);
    auto *label = lv_label_create(button);
    lv_label_set_text(label, items[index].label);

    bindings_[index] = {
        this, {items[index].action, items[index].application_id}};
    lv_obj_add_event_cb(button, &NavigationView::action_callback,
                        LV_EVENT_CLICKED, &bindings_[index]);
  }
}

void NavigationView::reset() {
  root_ = nullptr;
  buttons_.fill(nullptr);
  actions_ = nullptr;
}

void NavigationView::select(const char *application_id, bool drawer_selected) {
  for (std::size_t index = 0; index < std::size(items); ++index) {
    const bool selected =
        (drawer_selected &&
         items[index].action == NavigationActionKind::open_drawer) ||
        (!drawer_selected && application_id && items[index].application_id &&
         std::strcmp(items[index].application_id, application_id) == 0);
    if (selected)
      lv_obj_add_state(buttons_[index], LV_STATE_CHECKED);
    else
      lv_obj_remove_state(buttons_[index], LV_STATE_CHECKED);
  }
}

void NavigationView::set_hidden(bool hidden) {
  if (!root_)
    return;
  if (hidden)
    lv_obj_add_flag(root_, LV_OBJ_FLAG_HIDDEN);
  else
    lv_obj_remove_flag(root_, LV_OBJ_FLAG_HIDDEN);
}

void NavigationView::apply_theme(const ThemePalette &palette) {
  if (!root_)
    return;
  lv_obj_set_style_bg_color(root_, lv_color_hex(palette.surface), 0);
  lv_obj_set_style_border_color(root_, lv_color_hex(palette.border), 0);
  lv_obj_set_style_border_width(root_, 1, 0);
}

void NavigationView::action_callback(lv_event_t *event) {
  const auto *binding =
      static_cast<ActionBinding *>(lv_event_get_user_data(event));
  binding->view->dispatch(binding->action);
}

void NavigationView::dispatch(NavigationAction action) const {
  if (actions_)
    actions_->on(action);
}

} // namespace espscreen::ui
