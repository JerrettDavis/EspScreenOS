#pragma once
#include "espscreen/ui/theme.hpp"
#include "espscreen/ui/shell_actions.hpp"
#include "espscreen/core/application_host.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

struct lv_event_t;
struct lv_obj_t;

namespace espscreen::ui {

class DrawerView final {
public:
  static constexpr std::size_t capacity = core::max_application_count;
  void build(lv_obj_t *parent, std::span<const DrawerItem> items,
             IShellActionSink &actions);
  void reset();

  void show(std::int32_t x_offset, std::int32_t y_offset = 0);
  void hide(std::int32_t x_offset, std::int32_t y_offset = 0);
  void set_hidden(bool hidden);
  void move_background();
  void apply_theme(const ThemePalette &palette);

  [[nodiscard]] lv_obj_t *object() const { return root_; }

private:
  struct ActionBinding final {
    DrawerView *view;
    DrawerAction action;
  };

  static void action_callback(lv_event_t *event);
  void add_tile(lv_obj_t *parent, const DrawerItem &item, std::int32_t width,
                bool compact, ActionBinding &binding);
  void dispatch(DrawerAction action) const;

  lv_obj_t *root_{nullptr};
  lv_obj_t *grid_{nullptr};
  std::array<ActionBinding, capacity + 2> bindings_{};
  IShellActionSink *actions_{nullptr};
};

} // namespace espscreen::ui
