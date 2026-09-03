#pragma once

#include "espscreen/ui/shell_navigation.hpp"
#include <cstdint>

namespace espscreen::ui {

struct ShellVisibility final {
    bool header;
    bool content;
    bool navigation;
    bool drawer;
    bool settings;
    bool status;
};

enum class SurfaceAnimation : std::uint8_t {
    none,
    application,
    drawer_vertical,
    drawer_horizontal,
    settings_horizontal,
    status_vertical,
};

struct ShellPresentation final {
    ShellVisibility visibility;
    SurfaceAnimation egress;
    SurfaceAnimation ingress;
};

[[nodiscard]] constexpr ShellVisibility visibility_for(ShellSurface surface) {
    switch (surface) {
    case ShellSurface::application: return {true, true, true, false, false, false};
    case ShellSurface::drawer: return {false, false, false, true, false, false};
    case ShellSurface::settings: return {false, false, false, false, true, false};
    case ShellSurface::status: return {false, false, false, false, false, true};
    }
    return {true, true, true, false, false, false};
}

[[nodiscard]] constexpr SurfaceAnimation surface_animation(ShellSurface surface,
                                                            ShellSurface other) {
    switch (surface) {
    case ShellSurface::application: return SurfaceAnimation::application;
    case ShellSurface::drawer:
        return other == ShellSurface::settings ? SurfaceAnimation::drawer_horizontal
                                               : SurfaceAnimation::drawer_vertical;
    case ShellSurface::settings: return SurfaceAnimation::settings_horizontal;
    case ShellSurface::status: return SurfaceAnimation::status_vertical;
    }
    return SurfaceAnimation::none;
}

[[nodiscard]] constexpr ShellPresentation present(ShellSurface from, ShellSurface to) {
    if (from == to) return {visibility_for(to), SurfaceAnimation::none,
                            SurfaceAnimation::none};
    return {visibility_for(to), surface_animation(from, to), surface_animation(to, from)};
}

} // namespace espscreen::ui
