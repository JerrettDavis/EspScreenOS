#pragma once

#include "espscreen/board/bus_host.hpp"
#include "espscreen/resources/resource_broker.hpp"

namespace espscreen::expansion {

// Projects a declarative board header onto the resource identity leased by expansion code.
// An unsupported or incomplete descriptor produces an invalid key (index < 0), which the
// broker rejects without acquiring anything.
[[nodiscard]] constexpr resources::ResourceKey header_resource_key(
    const board::HeaderConfig& header) {
    using board::HeaderKind;
    using resources::ResourceKind;

    switch (header.kind) {
        case HeaderKind::i2c:
            return {ResourceKind::i2c_bus,
                    board::bus_index_for(header.host, board::BusFamily::i2c)};
        case HeaderKind::spi:
            return {ResourceKind::spi_bus,
                    board::bus_index_for(header.host, board::BusFamily::spi)};
        case HeaderKind::uart:
            return {ResourceKind::uart,
                    board::bus_index_for(header.host, board::BusFamily::uart)};
        case HeaderKind::gpio:
        case HeaderKind::power:
            return {ResourceKind::gpio,
                    static_cast<std::int16_t>(header.pin_count ? header.pins[0] : -1)};
    }
    return {ResourceKind::gpio, -1};
}

} // namespace espscreen::expansion
