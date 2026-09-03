#pragma once

#include "espscreen/core/status.hpp"
#include <array>
#include <cstddef>
#include <cstdint>

namespace espscreen::hal {

struct I2cLeaseRequest {
    int port{};
    int sda{-1};
    int scl{-1};
    std::uint32_t clock_hz{};
};

struct I2cAcquireDecision {
    core::Status status{};
    bool initialize_hardware{};
};

template<std::size_t PortCount>
class I2cLeaseTable final {
public:
    [[nodiscard]] constexpr I2cAcquireDecision acquire(I2cLeaseRequest request) {
        if (request.port < 0 || request.port >= static_cast<int>(PortCount) ||
            request.sda < 0 || request.scl < 0 || request.clock_hz == 0)
            return {{core::ErrorCode::invalid_argument, "invalid i2c configuration"}, false};

        auto& port = ports_[static_cast<std::size_t>(request.port)];
        if (port.references != 0) {
            if (port.sda != request.sda || port.scl != request.scl ||
                port.clock_hz != request.clock_hz)
                return {{core::ErrorCode::conflict,
                         "i2c port already uses different configuration"}, false};
            ++port.references;
            return {core::Status::success(), false};
        }

        port = {request.clock_hz, 1, request.sda, request.scl};
        return {core::Status::success(), true};
    }

    [[nodiscard]] constexpr bool release(int port_index) {
        if (port_index < 0 || port_index >= static_cast<int>(PortCount)) return false;
        auto& port = ports_[static_cast<std::size_t>(port_index)];
        if (port.references == 0) return false;
        if (--port.references != 0) return false;
        port = {};
        return true;
    }

    [[nodiscard]] constexpr int reference_count(int port_index) const {
        return port_index >= 0 && port_index < static_cast<int>(PortCount)
            ? ports_[static_cast<std::size_t>(port_index)].references : 0;
    }

private:
    struct PortState {
        std::uint32_t clock_hz{};
        int references{};
        int sda{-1};
        int scl{-1};
    };
    std::array<PortState, PortCount> ports_{};
};

} // namespace espscreen::hal
