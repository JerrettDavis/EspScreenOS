#pragma once

#include <cstdint>

namespace espscreen::ui {

enum class ShellSurface : std::uint8_t { application, drawer, settings, status };

// Owns shell navigation transitions independently of LVGL presentation.
class ShellNavigation final {
public:
    [[nodiscard]] ShellSurface surface() const { return surface_; }
    [[nodiscard]] bool is(ShellSurface surface) const { return surface_ == surface; }

    void show_application() { surface_ = ShellSurface::application; }
    void toggle_drawer() {
        surface_ = is(ShellSurface::drawer) ? ShellSurface::application : ShellSurface::drawer;
    }
    void toggle_settings() {
        surface_ = is(ShellSurface::settings) ? ShellSurface::drawer : ShellSurface::settings;
    }
    void toggle_status() {
        surface_ = is(ShellSurface::status) ? ShellSurface::application : ShellSurface::status;
    }

private:
    ShellSurface surface_{ShellSurface::application};
};

} // namespace espscreen::ui
