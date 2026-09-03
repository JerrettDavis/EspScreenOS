#pragma once

#include <array>
#include <cstdint>
#include <string_view>

namespace espscreen::ui {

enum class SettingId : std::uint8_t {
  brightness,
  idle_brightness,
  screen_timeout,
  interface_scale,
  audio_volume,
  haptic_feedback,
  touch_sound,
};

struct SettingsViewModel final {
  std::int32_t brightness{85};
  std::int32_t idle_brightness{30};
  std::int32_t timeout_seconds{120};
  std::int32_t scale_percent{100};
  std::int32_t audio_percent{70};
  std::int32_t haptic_percent{45};
  std::int32_t touch_sound_percent{0};
};

struct SettingEffects final {
  bool display_changed{false};
  bool audio_volume_changed{false};
};

enum class SettingPresentation : std::uint8_t {
  percent,
  percent_or_off,
  minutes_or_never,
};

struct SettingDescriptor final {
  SettingId id;
  std::string_view title;
  std::int32_t SettingsViewModel::* value;
  std::int32_t slider_minimum;
  std::int32_t slider_maximum;
  std::int32_t model_scale{1};
  SettingEffects effects{};
  SettingPresentation presentation{SettingPresentation::percent};
};

struct PersistentSetting final {
  std::string_view key;
  std::int32_t value;
};

enum class PersistedSettingEncoding : std::uint8_t {
  scalar,
  dark_theme,
  enabled_gate,
};

struct PersistedSettingDescriptor final {
  std::string_view key;
  std::int32_t SettingsViewModel::* value{nullptr};
  std::int32_t minimum{0};
  std::int32_t maximum{0};
  PersistedSettingEncoding encoding{PersistedSettingEncoding::scalar};
};

class ShellSettings final {
public:
  static constexpr std::array settings{
      SettingDescriptor{SettingId::brightness, "Brightness",
                        &SettingsViewModel::brightness, 10, 100, 1,
                        {.display_changed = true}},
      SettingDescriptor{SettingId::idle_brightness, "Idle brightness",
                        &SettingsViewModel::idle_brightness, 5, 80, 1,
                        {.display_changed = true}},
      SettingDescriptor{SettingId::screen_timeout, "Screen timeout",
                        &SettingsViewModel::timeout_seconds, 0, 20, 30,
                        {.display_changed = true}, SettingPresentation::minutes_or_never},
      SettingDescriptor{SettingId::interface_scale, "Interface scale",
                        &SettingsViewModel::scale_percent, 9, 12, 10,
                        {.display_changed = true}},
      SettingDescriptor{SettingId::audio_volume, "Audio volume",
                        &SettingsViewModel::audio_percent, 0, 100, 1,
                        {.audio_volume_changed = true}, SettingPresentation::percent_or_off},
      SettingDescriptor{SettingId::touch_sound, "Sound on touch",
                        &SettingsViewModel::touch_sound_percent, 0, 100, 1, {},
                        SettingPresentation::percent_or_off},
      SettingDescriptor{SettingId::haptic_feedback, "Vibration on touch",
                        &SettingsViewModel::haptic_percent, 0, 100, 1, {},
                        SettingPresentation::percent_or_off}};

  static constexpr std::array persisted_settings{
      PersistedSettingDescriptor{"ui_dark", nullptr, 0, 0,
                                 PersistedSettingEncoding::dark_theme},
      PersistedSettingDescriptor{"brightness", &SettingsViewModel::brightness, 10, 100},
      PersistedSettingDescriptor{"dim_pct", &SettingsViewModel::idle_brightness, 5, 80},
      PersistedSettingDescriptor{"timeout_s", &SettingsViewModel::timeout_seconds, 0, 600},
      PersistedSettingDescriptor{"ui_scale", &SettingsViewModel::scale_percent, 90, 120},
      PersistedSettingDescriptor{"audio_pct", &SettingsViewModel::audio_percent, 0, 100},
      PersistedSettingDescriptor{"audio_haptic_pct", &SettingsViewModel::touch_sound_percent,
                                 0, 100},
      PersistedSettingDescriptor{"haptic_pct", &SettingsViewModel::haptic_percent, 0, 100},
      PersistedSettingDescriptor{"haptic_en", &SettingsViewModel::haptic_percent, 0, 0,
                                 PersistedSettingEncoding::enabled_gate}};

  void restore(const PersistedSettingDescriptor& setting, std::int32_t value);
  [[nodiscard]] SettingEffects change(SettingId setting,
                                      std::int32_t slider_value);
  void toggle_theme() { dark_theme_ = !dark_theme_; }

  [[nodiscard]] bool dark_theme() const { return dark_theme_; }
  [[nodiscard]] const SettingsViewModel &model() const { return model_; }
  [[nodiscard]] std::array<PersistentSetting, persisted_settings.size()>
  persistent_values() const;

private:
  bool dark_theme_{true};
  SettingsViewModel model_{};
  [[nodiscard]] std::int32_t persisted_value(
      const PersistedSettingDescriptor& setting) const;
  [[nodiscard]] static const SettingDescriptor* descriptor(SettingId setting);
};

} // namespace espscreen::ui
