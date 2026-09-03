#include "espscreen/ui/shell_settings_persistence.hpp"

namespace espscreen::ui {

ShellSettingsLoadResult restore_shell_settings(const core::ISettingsStore& store,
                                               ShellSettings& settings) {
    ShellSettingsLoadResult result{};
    for (const auto& descriptor : ShellSettings::persisted_settings) {
        std::int32_t value{};
        if (store.get_i32(descriptor.key.data(), value)) {
            settings.restore(descriptor, value);
            ++result.restored;
        } else {
            ++result.unavailable;
        }
    }
    return result;
}

core::Status persist_shell_settings(core::ISettingsStore& store,
                                    const ShellSettings& settings) {
    for (const auto& setting : settings.persistent_values())
        if (const auto status = store.set_i32(setting.key.data(), setting.value);
            !status)
            return status;
    return core::Status::success();
}

core::Status persist_shell_theme(core::ISettingsStore& store,
                                 const ShellSettings& settings) {
    for (const auto& descriptor : ShellSettings::persisted_settings) {
        if (descriptor.encoding != PersistedSettingEncoding::dark_theme) continue;
        return store.set_i32(descriptor.key.data(), settings.dark_theme() ? 1 : 0);
    }
    return {core::ErrorCode::not_found, "theme persistence descriptor missing"};
}

} // namespace espscreen::ui
