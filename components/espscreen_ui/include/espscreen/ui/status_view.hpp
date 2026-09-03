#pragma once
#include "espscreen/ui/theme.hpp"
#include "espscreen/ui/shell_actions.hpp"

#include <array>
#include <cstdint>

struct lv_obj_t;
struct lv_event_t;

namespace espscreen::ui {
namespace motion { class ButtonStyle; }

class StatusView final {
public:
  void build(lv_obj_t *parent, IShellActionSink &actions,
             const motion::ButtonStyle &button_style);
  void reset();

  void show();
  void hide();
  void set_hidden(bool hidden);
  void apply_theme(const ThemePalette &palette);
  void update(const StatusViewModel &model);
  void update_network(std::int32_t state);

  [[nodiscard]] lv_obj_t *object() const { return root_; }

private:
  struct ActionBinding final {
    StatusView *view;
    StatusAction action;
  };

  static void action_callback(lv_event_t *event);
  void dispatch(StatusAction action) const;

  lv_obj_t *root_{nullptr};
  lv_obj_t *battery_{nullptr};
  lv_obj_t *memory_{nullptr};
  lv_obj_t *storage_{nullptr};
  lv_obj_t *network_{nullptr};
  IShellActionSink *actions_{nullptr};
  std::array<ActionBinding, 5> action_bindings_{};
};

} // namespace espscreen::ui
