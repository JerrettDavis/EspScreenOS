#pragma once

#include <cstdint>

namespace espscreen::ui {

struct ThemePalette final {
  bool dark;
  std::uint32_t page;
  std::uint32_t surface;
  std::uint32_t raised;
  std::uint32_t text;
  std::uint32_t muted;
  std::uint32_t border;
  std::uint32_t accent;
  std::uint32_t success;
};

[[nodiscard]] constexpr ThemePalette theme_palette(bool dark) {
  return {.dark = dark,
          .page = dark ? 0x090E1AU : 0xF1F4F9U,
          .surface = dark ? 0x131B2BU : 0xFFFFFFU,
          .raised = dark ? 0x202B40U : 0xE9EEF7U,
          .text = dark ? 0xF5F7FCU : 0x182033U,
          .muted = dark ? 0x98A6BFU : 0x667085U,
          .border = dark ? 0x27344CU : 0xDCE3EEU,
          .accent = 0x5B7CFAU,
          .success = 0x2CC9A0U};
}

} // namespace espscreen::ui
