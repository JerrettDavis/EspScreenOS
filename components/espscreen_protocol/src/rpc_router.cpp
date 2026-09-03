#include "espscreen/protocol/rpc_router.hpp"

namespace espscreen::protocol {
namespace {

bool valid(const RpcRoute& route) {
    return route.method != 0 && route.endpoint;
}

bool same_method(const RpcRoute& left, const RpcRoute& right) {
    return left.method == right.method;
}

} // namespace

core::Status RpcRouter::register_routes(std::span<const RpcRoute> candidates) {
    if (candidates.size() > maximum_routes - route_count_)
        return {core::ErrorCode::capacity_exceeded, "rpc route table full"};

    for (std::size_t index = 0; index < candidates.size(); ++index) {
        if (!valid(candidates[index]))
            return {core::ErrorCode::invalid_argument, "invalid rpc route"};
        for (std::size_t existing = 0; existing < route_count_; ++existing)
            if (same_method(candidates[index], routes_[existing]))
                return {core::ErrorCode::already_exists,
                        "rpc method already registered"};
        for (std::size_t earlier = 0; earlier < index; ++earlier)
            if (same_method(candidates[index], candidates[earlier]))
                return {core::ErrorCode::already_exists,
                        "duplicate rpc method in route batch"};
    }

    for (const auto& route : candidates) routes_[route_count_++] = route;
    return core::Status::success();
}

core::Status RpcRouter::invoke(std::uint16_t method,
                               std::span<const std::uint8_t> request,
                               std::span<std::uint8_t> response,
                               std::size_t& response_size) const {
    response_size = 0;
    for (std::size_t index = 0; index < route_count_; ++index)
        if (routes_[index].method == method)
            return routes_[index].endpoint->handle(request, response,
                                                   response_size);
    return {core::ErrorCode::not_found, "rpc method not found"};
}

} // namespace espscreen::protocol
