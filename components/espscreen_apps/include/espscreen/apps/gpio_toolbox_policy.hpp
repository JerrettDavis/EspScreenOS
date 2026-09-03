#pragma once

#include "espscreen/core/application_board.hpp"
#include "espscreen/core/fixed_vector.hpp"
#include <cstdint>

namespace espscreen::apps {

struct GpioPinCandidate final {
    const char* header_name;
    std::int16_t pin;
};

using GpioPinCatalog = core::FixedVector<GpioPinCandidate, 32>;

[[nodiscard]] constexpr GpioPinCatalog gpio_pin_catalog(
    const core::ApplicationBoardInfo& board) {
    GpioPinCatalog result;
    for (std::size_t header_index = 0; header_index < board.header_count;
         ++header_index) {
        const auto& header = board.headers[header_index];
        if (!header.gpio) continue;
        for (std::size_t pin_index = 0; pin_index < header.pin_count; ++pin_index) {
            const auto pin = static_cast<std::int16_t>(header.pins[pin_index]);
            if (pin < 0) continue;
            bool duplicate = false;
            for (const auto& candidate : result)
                if (candidate.pin == pin) duplicate = true;
            if (!duplicate && !result.push_back({header.name, pin})) return result;
        }
    }
    return result;
}

} // namespace espscreen::apps
