#pragma once

#include "espscreen/board/descriptor.hpp"
#include "espscreen/resources/resource_broker.hpp"
#include <array>
#include <cstddef>

namespace espscreen::hal {

struct ResourceReservation final {
    resources::ResourceKey key{};
    resources::LeaseMode mode{resources::LeaseMode::exclusive};
};

class BoardResourcePlan final {
public:
    static constexpr std::size_t maximum_reservations = 48;
    [[nodiscard]] std::size_t size() const { return size_; }
    [[nodiscard]] const ResourceReservation* begin() const { return reservations_.data(); }
    [[nodiscard]] const ResourceReservation* end() const { return begin() + size_; }
    [[nodiscard]] const ResourceReservation& operator[](std::size_t index) const { return reservations_[index]; }
    [[nodiscard]] bool contains(resources::ResourceKey key) const;

private:
    friend BoardResourcePlan fixed_resource_plan(const board::BoardDescriptor&);
    void add(resources::ResourceKey, resources::LeaseMode mode = resources::LeaseMode::exclusive);
    std::array<ResourceReservation, maximum_reservations> reservations_{};
    std::size_t size_{0};
};

[[nodiscard]] BoardResourcePlan fixed_resource_plan(const board::BoardDescriptor& descriptor);

} // namespace espscreen::hal
