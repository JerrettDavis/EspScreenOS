#pragma once

#include "espscreen/core/settings_store.hpp"
#include "espscreen/ui/shell_settings.hpp"
#include <cstddef>

namespace espscreen::ui {

struct ShellSettingsLoadResult final {
    std::size_t restored{0};
    std::size_t unavailable{0};
};

[[nodiscard]] ShellSettingsLoadResult restore_shell_settings(
    const core::ISettingsStore& store, ShellSettings& settings);

[[nodiscard]] core::Status persist_shell_settings(
    core::ISettingsStore& store, const ShellSettings& settings);

[[nodiscard]] core::Status persist_shell_theme(
    core::ISettingsStore& store, const ShellSettings& settings);

} // namespace espscreen::ui
