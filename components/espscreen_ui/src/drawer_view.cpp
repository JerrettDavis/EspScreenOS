#include "espscreen/ui/drawer_view.hpp"

#include "motion.hpp"
#include "lvgl.h"

#include <algorithm>
#include <string_view>

namespace espscreen::ui {
namespace {

struct AppIcon final {
  const char *symbol;
  std::uint32_t color;
};

AppIcon icon_for(std::string_view id) {
  if (id == "dashboard")
    return {LV_SYMBOL_HOME, 0x5B7CFA};
  if (id == "calculator")
    return {LV_SYMBOL_PLUS, 0x2CC9A0};
  if (id == "gpio")
    return {LV_SYMBOL_CHARGE, 0xF6A84B};
  if (id == "recorder")
    return {LV_SYMBOL_AUDIO, 0xED647A};
  if (id == "paint")
    return {LV_SYMBOL_TINT, 0xA875F5};
  if (id == "notes")
    return {LV_SYMBOL_EDIT, 0x48A9E6};
  if (id == "hardware")
    return {LV_SYMBOL_WARNING, 0xF07D48};
  if (id == "board")
    return {LV_SYMBOL_BARS, 0x54BFA3};
  return {LV_SYMBOL_FILE, 0x8795AA};
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

} // namespace

void DrawerView::build(lv_obj_t *parent, std::span<const DrawerItem> items,
                       IShellActionSink &actions) {
  if (root_)
    return;

  actions_ = &actions;
  const int screen_width = lv_display_get_horizontal_resolution(nullptr);
  const int screen_height = lv_display_get_vertical_resolution(nullptr);
  const bool compact = screen_height <= 320;
  const int padding = compact ? 6 : 10;
  const int gap = compact ? 4 : 6;
  root_ = lv_obj_create(parent);
  lv_obj_add_flag(root_, LV_OBJ_FLAG_FLOATING);
  lv_obj_set_pos(root_, 0, 0);
  lv_obj_set_size(root_, screen_width, screen_height);
  lv_obj_set_flex_flow(root_, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_all(root_, padding, 0);
  lv_obj_set_style_pad_row(root_, gap, 0);
  lv_obj_set_style_radius(root_, 0, 0);
  make_static(root_);

  auto *header = lv_obj_create(root_);
  lv_obj_set_size(header, LV_PCT(100), compact ? 34 : 40);
  lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(header, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);
  make_clear(header);
  auto *title = lv_label_create(header);
  lv_label_set_text(title, "All apps");
  lv_obj_set_style_text_color(title, lv_color_hex(0x5B7CFA), 0);
  lv_obj_set_flex_grow(title, 1);
  auto *close = lv_button_create(header);
  lv_obj_set_size(close, compact ? 36 : 40, compact ? 30 : 34);
  lv_obj_set_style_radius(close, 10, 0);
  auto *close_label = lv_label_create(close);
  lv_label_set_text(close_label, LV_SYMBOL_CLOSE);
  lv_obj_center(close_label);
  bindings_[0] = {this, {.kind = DrawerActionKind::close}};
  lv_obj_add_event_cb(close, &DrawerView::action_callback, LV_EVENT_CLICKED,
                      &bindings_[0]);

  grid_ = lv_obj_create(root_);
  lv_obj_set_width(grid_, LV_PCT(100));
  lv_obj_set_flex_grow(grid_, 1);
  lv_obj_set_flex_flow(grid_, LV_FLEX_FLOW_ROW_WRAP);
  lv_obj_set_flex_align(grid_, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START,
                        LV_FLEX_ALIGN_START);
  lv_obj_set_scroll_dir(grid_, LV_DIR_VER);
  lv_obj_set_scrollbar_mode(grid_, LV_SCROLLBAR_MODE_ACTIVE);
  lv_obj_set_style_border_width(grid_, 0, 0);
  lv_obj_set_style_pad_all(grid_, 2, 0);
  lv_obj_set_style_pad_row(grid_, gap, 0);
  lv_obj_set_style_pad_column(grid_, gap, 0);
  const int columns = std::max(4, screen_width / 88);
  const int grid_width = screen_width - padding * 2 - 4;
  const int tile_width = (grid_width - gap * (columns - 1)) / columns;

  const auto count = std::min(items.size(), capacity);
  for (std::size_t index = 0; index < count; ++index) {
    bindings_[index + 1] = {
        this, {.kind = DrawerActionKind::activate_application,
               .application_index = index}};
    add_tile(grid_, items[index], tile_width, compact, bindings_[index + 1]);
  }
  const DrawerItem settings{.id = "settings", .title = "Settings"};
  bindings_[count + 1] = {this, {.kind = DrawerActionKind::open_settings}};
  add_tile(grid_, settings, tile_width, compact, bindings_[count + 1]);
  lv_obj_add_flag(root_, LV_OBJ_FLAG_HIDDEN);
}

void DrawerView::add_tile(lv_obj_t *parent, const DrawerItem &item,
                          std::int32_t width, bool compact,
                          ActionBinding &binding) {
  auto *tile = lv_button_create(parent);
  lv_obj_set_size(tile, width, compact ? 54 : 64);
  lv_obj_set_flex_flow(tile, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(tile, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_radius(tile, 11, 0);
  lv_obj_set_style_shadow_width(tile, 0, 0);
  lv_obj_set_style_pad_all(tile, 2, 0);
  lv_obj_set_style_pad_row(tile, 2, 0);
  const auto icon = icon_for(item.id);
  auto *badge = lv_obj_create(tile);
  lv_obj_set_size(badge, compact ? 27 : 31, compact ? 27 : 31);
  lv_obj_set_style_radius(badge, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(badge, lv_color_hex(icon.color), 0);
  lv_obj_set_style_bg_opa(badge, LV_OPA_20, 0);
  lv_obj_set_style_border_width(badge, 0, 0);
  lv_obj_set_style_pad_all(badge, 0, 0);
  make_static(badge);
  auto *icon_label = lv_label_create(badge);
  lv_label_set_text(icon_label, icon.symbol);
  lv_obj_set_style_text_color(icon_label, lv_color_hex(icon.color), 0);
  lv_obj_center(icon_label);
  auto *name = lv_label_create(tile);
  lv_label_set_text(name, item.title);
  lv_label_set_long_mode(name, LV_LABEL_LONG_DOT);
  lv_obj_set_width(name, LV_PCT(100));
  lv_obj_set_style_text_align(name, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_add_event_cb(tile, &DrawerView::action_callback, LV_EVENT_CLICKED,
                      &binding);
}

void DrawerView::reset() {
  root_ = grid_ = nullptr;
  actions_ = nullptr;
}

void DrawerView::show(std::int32_t x_offset, std::int32_t y_offset) {
  if (root_)
    motion::show(root_, x_offset, y_offset);
}

void DrawerView::hide(std::int32_t x_offset, std::int32_t y_offset) {
  if (root_)
    motion::hide(root_, x_offset, y_offset);
}

void DrawerView::set_hidden(bool hidden) {
  if (!root_)
    return;
  if (hidden)
    lv_obj_add_flag(root_, LV_OBJ_FLAG_HIDDEN);
  else
    lv_obj_remove_flag(root_, LV_OBJ_FLAG_HIDDEN);
}

void DrawerView::move_background() {
  if (root_)
    lv_obj_move_background(root_);
}

void DrawerView::apply_theme(const ThemePalette &palette) {
  if (!root_)
    return;
  lv_obj_set_style_bg_color(root_, lv_color_hex(palette.surface), 0);
  lv_obj_set_style_border_color(root_, lv_color_hex(palette.border), 0);
  lv_obj_set_style_border_width(root_, 1, 0);
  lv_obj_set_style_bg_color(grid_, lv_color_hex(palette.surface), 0);
}

void DrawerView::action_callback(lv_event_t *event) {
  const auto *binding =
      static_cast<ActionBinding *>(lv_event_get_user_data(event));
  binding->view->dispatch(binding->action);
}

void DrawerView::dispatch(DrawerAction action) const {
  if (actions_)
    actions_->on(action);
}

} // namespace espscreen::ui
