#pragma once

#include "espscreen/core/application.hpp"
#include "espscreen/core/fixed_vector.hpp"
#include "espscreen/core/status.hpp"
#include <cstddef>
#include <span>

namespace espscreen::runtime {

struct ApplicationCatalogEntry final {
    bool enabled{false};
    core::IApplication* application{nullptr};
};

template <std::size_t Capacity>
[[nodiscard]] core::Status select_enabled_applications(
    std::span<const ApplicationCatalogEntry> catalog,
    core::FixedVector<core::IApplication*, Capacity>& selected) {
    selected.clear();
    for (const auto& entry : catalog) {
        if (!entry.enabled) continue;
        if (!entry.application) {
            selected.clear();
            return {core::ErrorCode::invalid_argument,
                    "enabled application catalog entry is null"};
        }
        if (!selected.push_back(entry.application)) {
            selected.clear();
            return {core::ErrorCode::capacity_exceeded,
                    "application catalog capacity exceeded"};
        }
    }
    return core::Status::success();
}

} // namespace espscreen::runtime
