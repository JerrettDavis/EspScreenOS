#pragma once

#include "espscreen/core/application.hpp"
#include "espscreen/ui/shell_actions.hpp"
#include "espscreen/ui/shell_input_policy.hpp"
#include "espscreen/ui/shell_presentation.hpp"
#include "espscreen/ui/theme.hpp"

#include <cstdint>
#include <span>

namespace espscreen::ui {

class IShellView {
public:
  virtual ~IShellView() = default;
  virtual void build(std::span<const DrawerItem>, const SettingsViewModel &,
                     IShellActionSink &) = 0;
  virtual void reset() = 0;
  virtual void decorate_content_buttons() = 0;
  virtual void apply_theme(const ThemePalette &) = 0;
  virtual void apply_presentation(const ShellPresentation &, bool = false) = 0;
  [[nodiscard]] virtual bool ensure_status() = 0;
  virtual void set_scale(std::int32_t) = 0;
  virtual void invalidate() = 0;
  virtual void set_title(const char *) = 0;
  virtual void select_navigation(const char *, bool) = 0;
  virtual void move_drawer_background() = 0;
  virtual void update_settings(const SettingsViewModel &) = 0;
  virtual void update_status(const StatusViewModel &) = 0;
  virtual void update_network(std::int32_t) = 0;
  [[nodiscard]] virtual core::ApplicationSurface content_surface() const = 0;
};

} // namespace espscreen::ui
