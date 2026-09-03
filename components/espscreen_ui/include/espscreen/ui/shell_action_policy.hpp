#pragma once

#include "espscreen/ui/shell_actions.hpp"
#include "espscreen/ui/shell_input_policy.hpp"
#include <array>
#include <cstddef>
#include <cstdint>

namespace espscreen::ui {

enum class ShellCommandKind : std::uint8_t {
    toggle_drawer,
    toggle_settings,
    toggle_status,
    toggle_theme,
    activate_application,
    activate_application_id,
    change_setting,
    adjust_brightness,
    persist_settings,
};

struct ShellCommand final {
    ShellCommandKind kind{ShellCommandKind::toggle_drawer};
    const char* application_id{nullptr};
    std::size_t application_index{0};
    SettingId setting{SettingId::brightness};
    std::int32_t value{0};
};

struct ShellActionPlan final {
    std::array<ShellCommand, 2> commands{};
    std::size_t count{0};
};

[[nodiscard]] ShellActionPlan plan_for(ShellInputIntent intent);
[[nodiscard]] ShellActionPlan plan_for(NavigationAction action);
[[nodiscard]] ShellActionPlan plan_for(SettingsAction action);
[[nodiscard]] ShellActionPlan plan_for(DrawerAction action);
[[nodiscard]] ShellActionPlan plan_for(HeaderAction action);
[[nodiscard]] ShellActionPlan plan_for(StatusAction action);

} // namespace espscreen::ui
