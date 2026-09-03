#pragma once

#include "espscreen/ui/shell_settings.hpp"

#include <cstddef>
#include <cstdint>

namespace espscreen::ui {

struct DrawerItem final {
  const char *id;
  const char *title;
};

enum class HeaderAction : std::uint8_t { toggle_theme };
enum class InputDirection : std::uint8_t { up, down, left, right };
enum class NavigationActionKind : std::uint8_t { open_application, open_drawer };
struct NavigationAction final {
  NavigationActionKind kind;
  const char *application_id{nullptr};
};
enum class DrawerActionKind : std::uint8_t {
  close,
  activate_application,
  open_settings,
};
struct DrawerAction final {
  DrawerActionKind kind;
  std::size_t application_index{0};
};
enum class SettingsActionKind : std::uint8_t { close, value_changed, commit };
struct SettingsAction final {
  SettingsActionKind kind;
  SettingId setting{SettingId::brightness};
  std::int32_t value{0};
};
enum class StatusAction : std::uint8_t {
  close,
  toggle_theme,
  dim,
  brighten,
  open_settings,
};
struct StatusViewModel final {
  std::uint32_t free_heap_bytes{0};
  std::int32_t battery_percent{-1};
  bool storage_mounted{false};
  std::uint64_t storage_free_bytes{0};
};

class IShellActionSink {
public:
  virtual ~IShellActionSink() = default;
  virtual void on(HeaderAction) = 0;
  virtual void on(NavigationAction) = 0;
  virtual void on(DrawerAction) = 0;
  virtual void on(SettingsAction) = 0;
  virtual void on(StatusAction) = 0;
  virtual void on(InputDirection) = 0;
};

} // namespace espscreen::ui
