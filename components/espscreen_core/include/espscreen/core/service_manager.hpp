#pragma once
#include "espscreen/core/bounded_unique_registry.hpp"
#include "espscreen/core/clock.hpp"
#include "espscreen/core/component_lifecycle.hpp"
#include "espscreen/core/fixed_vector.hpp"
#include "espscreen/core/health.hpp"
#include "espscreen/core/service.hpp"
#include "espscreen/core/status.hpp"
#include <cstddef>
#include <span>

namespace espscreen::core {

class ServiceManager final {
public:
    static constexpr std::size_t kMaxServices = 20;
    explicit ServiceManager(IMonotonicClock& clock) : clock_(clock) {}

    [[nodiscard]] Status register_service(IService& service, bool critical = false);
    [[nodiscard]] Status register_services(std::span<const ServiceRegistration> services);
    [[nodiscard]] Status start_all();
    void stop_all();
    void clear();
    void tick();

    [[nodiscard]] std::size_t count() const { return services_.size(); }
    [[nodiscard]] const HealthRecord* health(std::size_t index) const;
    [[nodiscard]] const HealthRecord* find_health(std::string_view id) const;

private:
    BoundedUniqueRegistry<IService, kMaxServices> services_{};
    FixedVector<HealthRecord, kMaxServices> health_{};
    std::size_t started_count_{0};
    ComponentLifecycle lifecycle_{};
    IMonotonicClock& clock_;
};

} // namespace espscreen::core
