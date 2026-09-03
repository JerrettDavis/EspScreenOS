#include "espscreen/resources/resource_broker.hpp"
#include <limits>

namespace espscreen::resources {
using core::ErrorCode;
using core::Status;

ResourceBroker::Slot* ResourceBroker::find(ResourceKey key) {
    for (auto& slot : slots_) if (slot.active && slot.key == key) return &slot;
    return nullptr;
}

const ResourceBroker::Slot* ResourceBroker::find(ResourceKey key) const {
    for (const auto& slot : slots_) if (slot.active && slot.key == key) return &slot;
    return nullptr;
}

Status ResourceBroker::reserve(ResourceKey key, OwnerId owner, LeaseMode mode) {
    if (key.index < 0 || owner.value == 0) return {ErrorCode::invalid_argument, "invalid resource or owner"};
    Slot* slot = find(key);
    if (!slot) {
        for (auto& candidate : slots_) {
            if (!candidate.active) {
                candidate = {};
                candidate.active = true;
                candidate.key = key;
                candidate.mode = mode;
                candidate.owners[0] = OwnerRef{owner, 1, true};
                return Status::success();
            }
        }
        return {ErrorCode::capacity_exceeded, "resource table full"};
    }

    for (auto& ref : slot->owners) {
        if (ref.active && ref.owner == owner) {
            if (ref.references == std::numeric_limits<std::uint16_t>::max()) {
                return {ErrorCode::capacity_exceeded, "lease reference overflow"};
            }
            ++ref.references;
            return Status::success();
        }
    }

    if (slot->mode == LeaseMode::exclusive || mode == LeaseMode::exclusive) {
        return {ErrorCode::conflict, "resource is already leased incompatibly"};
    }
    for (auto& ref : slot->owners) {
        if (!ref.active) {
            ref = OwnerRef{owner, 1, true};
            return Status::success();
        }
    }
    return {ErrorCode::capacity_exceeded, "owner table full for resource"};
}

Status ResourceBroker::release(ResourceKey key, OwnerId owner) {
    Slot* slot = find(key);
    if (!slot) return {ErrorCode::not_found, "resource is not leased"};
    OwnerRef* found = nullptr;
    for (auto& ref : slot->owners) if (ref.active && ref.owner == owner) { found = &ref; break; }
    if (!found) return {ErrorCode::not_owner, "owner does not hold resource"};
    if (--found->references == 0) *found = {};
    bool any = false;
    for (const auto& ref : slot->owners) any = any || ref.active;
    if (!any) *slot = {};
    return Status::success();
}

bool ResourceBroker::held(ResourceKey key) const { return find(key) != nullptr; }

bool ResourceBroker::held_by(ResourceKey key, OwnerId owner) const {
    const Slot* slot = find(key);
    if (!slot) return false;
    for (const auto& ref : slot->owners) if (ref.active && ref.owner == owner) return true;
    return false;
}

std::uint16_t ResourceBroker::reference_count(ResourceKey key, OwnerId owner) const {
    const Slot* slot = find(key);
    if (!slot) return 0;
    for (const auto& ref : slot->owners) if (ref.active && ref.owner == owner) return ref.references;
    return 0;
}

std::size_t ResourceBroker::resource_count() const {
    std::size_t count = 0;
    for (const auto& slot : slots_) if (slot.active) ++count;
    return count;
}

void ResourceBroker::reset() { slots_ = {}; }

} // namespace espscreen::resources
