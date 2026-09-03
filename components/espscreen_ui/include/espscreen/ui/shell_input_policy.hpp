#pragma once

#include "espscreen/ui/shell_navigation.hpp"

#include "espscreen/ui/shell_actions.hpp"

#include <cstdint>

namespace espscreen::ui {

enum class ShellInputIntent : std::uint8_t {
  none,
  toggle_drawer,
  toggle_settings,
  toggle_status,
};

struct TouchSample final {
  bool pressed{false};
  std::int16_t x{0};
  std::int16_t y{0};
};

struct InputDecision final {
  bool activity{false};
  bool touch_started{false};
  ShellInputIntent intent{ShellInputIntent::none};
};

class ShellInputPolicy final {
public:
  [[nodiscard]] InputDecision update(const TouchSample &sample,
                                     ShellSurface surface);
  [[nodiscard]] static ShellInputIntent gesture(ShellSurface surface,
                                                InputDirection direction);
  void reset() { *this = {}; }

private:
  bool was_pressed_{false};
  bool tracking_{false};
  std::int16_t start_x_{0};
  std::int16_t start_y_{0};
  std::int16_t last_x_{0};
  std::int16_t last_y_{0};
};

} // namespace espscreen::ui
