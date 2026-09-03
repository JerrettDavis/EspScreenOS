#pragma once

#include "espscreen/core/status.hpp"
#include <array>
#include <cstddef>
#include <cstdint>

namespace espscreen::resources {

enum class ResourceKind : std::uint8_t { gpio, spi_bus, i2c_bus, i2s_bus, uart, timer, adc };
enum class LeaseMode : std::uint8_t { exclusive, shared };

struct ResourceKey final {
    ResourceKind kind{ResourceKind::gpio};
    std::int16_t index{-1};
    friend constexpr bool operator==(const ResourceKey&, const ResourceKey&) = default;
};

struct OwnerId final {
    std::uint32_t value{0};
    friend constexpr bool operator==(const OwnerId&, const OwnerId&) = default;
};

class ResourceBroker final {
public:
    static constexpr std::size_t kMaxResources = 32;
    static constexpr std::size_t kMaxOwnersPerResource = 8;

    [[nodiscard]] core::Status reserve(ResourceKey key, OwnerId owner, LeaseMode mode);
    [[nodiscard]] core::Status release(ResourceKey key, OwnerId owner);
    [[nodiscard]] bool held(ResourceKey key) const;
    [[nodiscard]] bool held_by(ResourceKey key, OwnerId owner) const;
    [[nodiscard]] std::uint16_t reference_count(ResourceKey key, OwnerId owner) const;
    [[nodiscard]] std::size_t resource_count() const;
    void reset();

private:
    struct OwnerRef { OwnerId owner{}; std::uint16_t references{0}; bool active{false}; };
    struct Slot {
        ResourceKey key{};
        LeaseMode mode{LeaseMode::exclusive};
        std::array<OwnerRef, kMaxOwnersPerResource> owners{};
        bool active{false};
    };
    [[nodiscard]] Slot* find(ResourceKey key);
    [[nodiscard]] const Slot* find(ResourceKey key) const;
    std::array<Slot, kMaxResources> slots_{};
};

} // namespace espscreen::resources
