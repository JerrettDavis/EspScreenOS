#include "espscreen/ui/shell_settings.hpp"

#include <algorithm>

namespace espscreen::ui {

void ShellSettings::restore(const PersistedSettingDescriptor& setting,
                            std::int32_t value) {
  switch (setting.encoding) {
  case PersistedSettingEncoding::dark_theme:
    dark_theme_ = value != 0;
    break;
  case PersistedSettingEncoding::enabled_gate:
    if (value == 0) model_.*setting.value = 0;
    break;
  case PersistedSettingEncoding::scalar:
    model_.*setting.value = std::clamp(value, setting.minimum, setting.maximum);
    break;
  }
}

SettingEffects ShellSettings::change(SettingId setting,
                                     std::int32_t slider_value) {
  const auto* definition = descriptor(setting);
  if (!definition) return {};
  model_.*definition->value =
      std::clamp(slider_value, definition->slider_minimum,
                 definition->slider_maximum) * definition->model_scale;
  return definition->effects;
}

const SettingDescriptor* ShellSettings::descriptor(SettingId setting) {
  for (const auto& definition : settings)
    if (definition.id == setting) return &definition;
  return nullptr;
}

std::int32_t ShellSettings::persisted_value(
    const PersistedSettingDescriptor& setting) const {
  switch (setting.encoding) {
  case PersistedSettingEncoding::dark_theme: return dark_theme_ ? 1 : 0;
  case PersistedSettingEncoding::enabled_gate:
    return model_.*setting.value ? 1 : 0;
  case PersistedSettingEncoding::scalar: return model_.*setting.value;
  }
  return 0;
}

std::array<PersistentSetting, ShellSettings::persisted_settings.size()>
ShellSettings::persistent_values() const {
  std::array<PersistentSetting, persisted_settings.size()> values{};
  for (std::size_t index = 0; index < persisted_settings.size(); ++index) {
    const auto setting = persisted_settings[index];
    values[index] = {setting.key, persisted_value(setting)};
  }
  return values;
}

} // namespace espscreen::ui
