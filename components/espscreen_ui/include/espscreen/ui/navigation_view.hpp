#pragma once
#include "espscreen/ui/theme.hpp"
#include "espscreen/ui/shell_actions.hpp"

#include <array>
#include <cstdint>

struct lv_event_t;
struct lv_obj_t;

namespace espscreen::ui {

class NavigationView final {
public:
  void build(lv_obj_t *parent, IShellActionSink &actions);
  void reset();

  void select(const char *application_id, bool drawer_selected);
  void set_hidden(bool hidden);
  void apply_theme(const ThemePalette &palette);

  [[nodiscard]] lv_obj_t *object() const { return root_; }

private:
  struct ActionBinding final {
    NavigationView *view;
    NavigationAction action;
  };

  static void action_callback(lv_event_t *event);
  void dispatch(NavigationAction action) const;

  lv_obj_t *root_{nullptr};
  std::array<lv_obj_t *, 4> buttons_{};
  std::array<ActionBinding, 4> bindings_{};
  IShellActionSink *actions_{nullptr};
};

} // namespace espscreen::ui
