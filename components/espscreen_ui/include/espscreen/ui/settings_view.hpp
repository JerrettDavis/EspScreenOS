#pragma once
#include "espscreen/ui/shell_settings.hpp"
#include "espscreen/ui/theme.hpp"
#include "espscreen/ui/shell_actions.hpp"

#include <array>
#include <cstdint>

struct lv_event_t;
struct lv_obj_t;

namespace espscreen::ui {

class SettingsView final {
public:
  void build(lv_obj_t *parent, const SettingsViewModel &model,
             IShellActionSink &actions);
  void reset();

  void update(const SettingsViewModel &model);
  void show();
  void hide();
  void set_hidden(bool hidden);
  void apply_theme(const ThemePalette &palette);

  [[nodiscard]] lv_obj_t *object() const { return root_; }

private:
  struct SliderBinding final {
    SettingsView *view;
    SettingId setting;
  };

  static constexpr std::size_t setting_count = ShellSettings::settings.size();
  static void close_callback(lv_event_t *event);
  static void changed_callback(lv_event_t *event);
  static void released_callback(lv_event_t *event);
  void add_row(const SettingDescriptor& setting, std::int32_t current);
  void dispatch(SettingsAction action) const;

  lv_obj_t *root_{nullptr};
  std::array<lv_obj_t *, setting_count> labels_{};
  std::array<SliderBinding, setting_count> bindings_{};
  IShellActionSink *actions_{nullptr};
};

} // namespace espscreen::ui
