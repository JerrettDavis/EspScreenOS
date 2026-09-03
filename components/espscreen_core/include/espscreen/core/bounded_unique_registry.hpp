#pragma once

#include "espscreen/core/fixed_vector.hpp"
#include "espscreen/core/status.hpp"
#include <cstddef>
#include <span>
#include <string_view>

namespace espscreen::core {

struct RegistryErrors final {
    std::string_view capacity;
    std::string_view null_item;
    std::string_view duplicate;
};

template <typename Item, std::size_t Capacity>
class BoundedUniqueRegistry final {
public:
    [[nodiscard]] Status add(std::span<Item* const> candidates,
                             const RegistryErrors& errors) {
        const Status validation = validate(candidates, errors);
        if (!validation.ok()) return validation;

        for (Item* candidate : candidates) {
            (void)items_.push_back(candidate);
        }
        return Status::success();
    }

    [[nodiscard]] Item* find(std::string_view id) const {
        for (Item* item : items_) {
            if (item->id() == id) return item;
        }
        return nullptr;
    }

    constexpr void clear() { items_.clear(); }
    [[nodiscard]] constexpr std::size_t size() const { return items_.size(); }
    [[nodiscard]] constexpr Item* operator[](std::size_t index) const { return items_[index]; }
    [[nodiscard]] constexpr auto begin() const { return items_.begin(); }
    [[nodiscard]] constexpr auto end() const { return items_.end(); }

private:
    [[nodiscard]] Status validate(std::span<Item* const> candidates,
                                  const RegistryErrors& errors) const {
        if (candidates.size() > Capacity - items_.size()) {
            return {ErrorCode::capacity_exceeded, errors.capacity};
        }

        for (std::size_t index = 0; index < candidates.size(); ++index) {
            Item* candidate = candidates[index];
            if (!candidate) return {ErrorCode::invalid_argument, errors.null_item};

            for (Item* existing : items_) {
                if (same_identity(existing, candidate)) {
                    return {ErrorCode::already_exists, errors.duplicate};
                }
            }
            for (std::size_t prior = 0; prior < index; ++prior) {
                if (same_identity(candidates[prior], candidate)) {
                    return {ErrorCode::already_exists, errors.duplicate};
                }
            }
        }
        return Status::success();
    }

    [[nodiscard]] static bool same_identity(const Item* left, const Item* right) {
        return left == right || left->id() == right->id();
    }

    FixedVector<Item*, Capacity> items_{};
};

} // namespace espscreen::core
