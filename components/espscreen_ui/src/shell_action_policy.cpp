#include "espscreen/ui/shell_action_policy.hpp"

namespace espscreen::ui {
namespace {

ShellActionPlan one(ShellCommand command) {
    return {.commands = {command}, .count = 1};
}

ShellActionPlan two(ShellCommand first, ShellCommand second) {
    return {.commands = {first, second}, .count = 2};
}

} // namespace

ShellActionPlan plan_for(ShellInputIntent intent) {
    switch (intent) {
    case ShellInputIntent::toggle_drawer:
        return one({.kind = ShellCommandKind::toggle_drawer});
    case ShellInputIntent::toggle_settings:
        return one({.kind = ShellCommandKind::toggle_settings});
    case ShellInputIntent::toggle_status:
        return one({.kind = ShellCommandKind::toggle_status});
    case ShellInputIntent::none:
        return {};
    }
    return {};
}

ShellActionPlan plan_for(NavigationAction action) {
    if (action.kind == NavigationActionKind::open_drawer)
        return one({.kind = ShellCommandKind::toggle_drawer});
    if (action.application_id)
        return one({.kind = ShellCommandKind::activate_application_id,
                    .application_id = action.application_id});
    return {};
}

ShellActionPlan plan_for(SettingsAction action) {
    switch (action.kind) {
    case SettingsActionKind::close:
        return one({.kind = ShellCommandKind::toggle_settings});
    case SettingsActionKind::value_changed:
        return one({.kind = ShellCommandKind::change_setting,
                    .setting = action.setting,
                    .value = action.value});
    case SettingsActionKind::commit:
        return one({.kind = ShellCommandKind::persist_settings});
    }
    return {};
}

ShellActionPlan plan_for(DrawerAction action) {
    switch (action.kind) {
    case DrawerActionKind::close:
        return one({.kind = ShellCommandKind::toggle_drawer});
    case DrawerActionKind::activate_application:
        return one({.kind = ShellCommandKind::activate_application,
                    .application_index = action.application_index});
    case DrawerActionKind::open_settings:
        return one({.kind = ShellCommandKind::toggle_settings});
    }
    return {};
}

ShellActionPlan plan_for(HeaderAction action) {
    if (action == HeaderAction::toggle_theme)
        return one({.kind = ShellCommandKind::toggle_theme});
    return {};
}

ShellActionPlan plan_for(StatusAction action) {
    switch (action) {
    case StatusAction::close:
        return one({.kind = ShellCommandKind::toggle_status});
    case StatusAction::toggle_theme:
        return one({.kind = ShellCommandKind::toggle_theme});
    case StatusAction::dim:
        return two({.kind = ShellCommandKind::adjust_brightness, .value = -10},
                   {.kind = ShellCommandKind::persist_settings});
    case StatusAction::brighten:
        return two({.kind = ShellCommandKind::adjust_brightness, .value = 10},
                   {.kind = ShellCommandKind::persist_settings});
    case StatusAction::open_settings:
        return two({.kind = ShellCommandKind::toggle_status},
                   {.kind = ShellCommandKind::toggle_settings});
    }
    return {};
}

} // namespace espscreen::ui
