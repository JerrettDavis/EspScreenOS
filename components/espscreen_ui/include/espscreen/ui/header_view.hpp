#pragma once

#include "espscreen/ui/shell_actions.hpp"
#include "espscreen/ui/theme.hpp"

#include <cstdint>

struct lv_event_t;
struct lv_obj_t;

namespace espscreen::ui {

class HeaderView final {
public:
  void build(lv_obj_t *parent, IShellActionSink &actions);
  void reset();

  void set_title(const char *title);
  void update(const StatusViewModel &model);
  void set_hidden(bool hidden);
  void apply_theme(const ThemePalette &palette);

  [[nodiscard]] lv_obj_t *object() const { return root_; }

private:
  static void theme_callback(lv_event_t *event);
  void dispatch(HeaderAction action) const;

  lv_obj_t *root_{nullptr};
  lv_obj_t *metrics_{nullptr};
  lv_obj_t *title_{nullptr};
  lv_obj_t *theme_button_{nullptr};
  lv_obj_t *theme_label_{nullptr};
  lv_obj_t *battery_{nullptr};
  lv_obj_t *memory_{nullptr};
  lv_obj_t *storage_{nullptr};
  IShellActionSink *actions_{nullptr};
  bool compact_{false};
};

} // namespace espscreen::ui
