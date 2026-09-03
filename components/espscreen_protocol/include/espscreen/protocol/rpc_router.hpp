#pragma once

#include "espscreen/core/status.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace espscreen::protocol {

class IRpcEndpoint {
public:
    virtual ~IRpcEndpoint() = default;
    [[nodiscard]] virtual core::Status handle(
        std::span<const std::uint8_t> request,
        std::span<std::uint8_t> response,
        std::size_t& response_size) = 0;
};

struct RpcRoute final {
    std::uint16_t method{0};
    IRpcEndpoint* endpoint{nullptr};
};

class RpcRouter final {
public:
    static constexpr std::size_t maximum_routes = 24;

    [[nodiscard]] core::Status register_routes(std::span<const RpcRoute> routes);
    [[nodiscard]] core::Status invoke(
        std::uint16_t method,
        std::span<const std::uint8_t> request,
        std::span<std::uint8_t> response,
        std::size_t& response_size) const;
    [[nodiscard]] std::size_t route_count() const { return route_count_; }

private:
    std::array<RpcRoute, maximum_routes> routes_{};
    std::size_t route_count_{0};
};

} // namespace espscreen::protocol
