#include "espscreen/ui/shell_input_policy.hpp"

#include <cstdlib>

namespace espscreen::ui {

InputDecision ShellInputPolicy::update(const TouchSample &sample,
                                       ShellSurface surface) {
  InputDecision decision{.activity = sample.pressed,
                         .touch_started = sample.pressed && !was_pressed_};
  if (sample.pressed) {
    last_x_ = sample.x;
    last_y_ = sample.y;
    if (!was_pressed_) {
      tracking_ = true;
      start_x_ = sample.x;
      start_y_ = sample.y;
    }
  } else if (was_pressed_ && tracking_) {
    const int delta_x = last_x_ - start_x_;
    const int delta_y = last_y_ - start_y_;
    const bool vertical = std::abs(delta_y) > std::abs(delta_x);
    if (surface == ShellSurface::application && start_y_ <= 90 &&
        delta_y >= 50 && vertical)
      decision.intent = ShellInputIntent::toggle_status;
    else if (surface == ShellSurface::status && delta_y <= -50 && vertical)
      decision.intent = ShellInputIntent::toggle_status;
    tracking_ = false;
  }
  was_pressed_ = sample.pressed;
  return decision;
}

ShellInputIntent ShellInputPolicy::gesture(ShellSurface surface,
                                           InputDirection direction) {
  if (surface == ShellSurface::application && direction == InputDirection::up)
    return ShellInputIntent::toggle_drawer;
  if (surface == ShellSurface::drawer && direction == InputDirection::down)
    return ShellInputIntent::toggle_drawer;
  if (surface == ShellSurface::settings && direction == InputDirection::right)
    return ShellInputIntent::toggle_settings;
  return ShellInputIntent::none;
}

} // namespace espscreen::ui
