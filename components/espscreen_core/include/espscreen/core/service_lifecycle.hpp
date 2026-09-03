#pragma once

#include "espscreen/core/health.hpp"
#include "espscreen/core/status.hpp"

namespace espscreen::core {

struct ServiceStartDecision final {
    HealthState health;
    bool participates;
    bool aborts_startup;
};

[[nodiscard]] constexpr ServiceStartDecision decide_service_start(
    bool critical, const Status& result) {
    if (result.ok()) {
        return {.health = HealthState::healthy,
                .participates = true,
                .aborts_startup = false};
    }
    if (critical) {
        return {.health = HealthState::failed,
                .participates = false,
                .aborts_startup = true};
    }
    return {.health = HealthState::degraded,
            .participates = true,
            .aborts_startup = false};
}

} // namespace espscreen::core
