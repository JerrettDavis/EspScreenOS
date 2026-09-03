#include "espscreen/core/service_manager.hpp"
#include "espscreen/core/service_lifecycle.hpp"

namespace espscreen::core {
Status ServiceManager::register_service(IService& service, bool critical) {
    const ServiceRegistration candidate{service, critical};
    return register_services(std::span<const ServiceRegistration>{&candidate, 1});
}

Status ServiceManager::register_services(
    std::span<const ServiceRegistration> candidates) {
    if (!lifecycle_.idle())
        return {ErrorCode::conflict, "cannot register services while running"};

    constexpr RegistryErrors errors{
        .capacity = "service capacity exceeded",
        .null_item = "service is null",
        .duplicate = "service already registered",
    };
    if (candidates.size() > kMaxServices)
        return {ErrorCode::capacity_exceeded, errors.capacity};
    FixedVector<IService*, kMaxServices> services;
    for (const auto& candidate : candidates)
        (void)services.push_back(&candidate.service);
    const Status result = services_.add(
        std::span<IService* const>{services.begin(), services.size()}, errors);
    if (!result.ok()) return result;

    for (const auto& candidate : candidates) {
        (void)health_.push_back(HealthRecord{
            candidate.service.id(), HealthState::unknown, {}, 0, candidate.critical});
    }
    return Status::success();
}

Status ServiceManager::start_all() {
    const auto transition = lifecycle_.begin_start();
    if (!transition.status) return transition.status;
    if (!transition.acquire)
        return {ErrorCode::conflict, "services already started"};
    started_count_ = 0;
    for (std::size_t i = 0; i < services_.size(); ++i) {
        auto& record = health_[i];
        record.state = HealthState::starting;
        record.changed_at_us = static_cast<std::uint64_t>(clock_.now_us());
        const Status result = services_[i]->start();
        const auto decision = decide_service_start(record.critical, result);
        record.state = decision.health;
        record.detail = result.ok() ? std::string_view{} : result.message();
        record.changed_at_us = static_cast<std::uint64_t>(clock_.now_us());
        if (decision.aborts_startup) {
            services_[i]->stop();
            for (std::size_t j = started_count_; j > 0; --j) {
                services_[j - 1]->stop();
                health_[j - 1].state = HealthState::stopped;
                health_[j - 1].changed_at_us = static_cast<std::uint64_t>(clock_.now_us());
            }
            started_count_ = 0;
            lifecycle_.start_failed();
            return result;
        }
        if (decision.participates) ++started_count_;
    }
    lifecycle_.started();
    return Status::success();
}

void ServiceManager::stop_all() {
    if (!lifecycle_.begin_stop()) return;
    for (std::size_t i = started_count_; i > 0; --i) {
        services_[i - 1]->stop();
        health_[i - 1].state = HealthState::stopped;
        health_[i - 1].changed_at_us = static_cast<std::uint64_t>(clock_.now_us());
    }
    started_count_ = 0;
    lifecycle_.stopped();
}

void ServiceManager::clear(){stop_all();services_.clear();health_.clear();}

void ServiceManager::tick() {
    if (!lifecycle_.running()) return;
    for (std::size_t i = 0; i < started_count_; ++i) services_[i]->tick();
}

const HealthRecord* ServiceManager::health(std::size_t index) const {
    return index < health_.size() ? &health_[index] : nullptr;
}

const HealthRecord* ServiceManager::find_health(std::string_view id) const {
    for (const auto& record : health_) if (record.id == id) return &record;
    return nullptr;
}

} // namespace espscreen::core
