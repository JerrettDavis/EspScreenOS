#pragma once

#include "espscreen/board/descriptor.hpp"
#include <cstdint>

namespace espscreen::board {

enum class BusFamily : std::uint8_t { none, spi, i2c, i2s, sdmmc, uart };

[[nodiscard]] constexpr BusFamily bus_family(BusHost host) {
    switch (host) {
        case BusHost::spi2:
        case BusHost::spi3: return BusFamily::spi;
        case BusHost::i2c0:
        case BusHost::i2c1: return BusFamily::i2c;
        case BusHost::i2s0:
        case BusHost::i2s1: return BusFamily::i2s;
        case BusHost::sdmmc0: return BusFamily::sdmmc;
        case BusHost::uart0:
        case BusHost::uart1:
        case BusHost::uart2: return BusFamily::uart;
        case BusHost::none: return BusFamily::none;
    }
    return BusFamily::none;
}

[[nodiscard]] constexpr std::int8_t bus_index(BusHost host) {
    switch (host) {
        case BusHost::i2c0:
        case BusHost::i2s0:
        case BusHost::sdmmc0:
        case BusHost::uart0: return 0;
        case BusHost::i2c1:
        case BusHost::i2s1:
        case BusHost::uart1: return 1;
        case BusHost::spi2:
        case BusHost::uart2: return 2;
        case BusHost::spi3: return 3;
        case BusHost::none: return -1;
    }
    return -1;
}

[[nodiscard]] constexpr bool host_belongs_to(BusHost host, BusFamily family) {
    return bus_family(host) == family;
}

[[nodiscard]] constexpr std::int8_t bus_index_for(BusHost host, BusFamily family) {
    if (!host_belongs_to(host, family)) return -1;
    return bus_index(host);
}

} // namespace espscreen::board
