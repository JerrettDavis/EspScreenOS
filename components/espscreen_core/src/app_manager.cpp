#include "espscreen/core/app_manager.hpp"
#include "espscreen/core/application_jobs.hpp"

namespace espscreen::core {
namespace {

Status start_transactionally(IApplication& application, ApplicationSurface surface,
                             IApplicationJobs& jobs) {
    const auto result = application.start(surface);
    if (result) return result;
    jobs.quiesce();
    application.stop();
    return result;
}

} // namespace

Status AppManager::register_app(IApplication& app) {
    IApplication* candidate=&app;
    return register_apps(std::span<IApplication* const>{&candidate,1});
}

Status AppManager::register_apps(std::span<IApplication* const> candidates) {
    if (state_ == LifecycleState::transitioning)
        return {ErrorCode::conflict, "cannot register applications during transition"};

    constexpr RegistryErrors errors{
        .capacity = "application capacity exceeded",
        .null_item = "application is null",
        .duplicate = "application already registered",
    };
    return apps_.add(candidates, errors);
}

Status AppManager::clear() {
    if (state_ != LifecycleState::inactive)
        return {ErrorCode::conflict,"cannot clear active applications"};
    apps_.clear();
    return Status::success();
}

Status AppManager::activate(std::string_view id, ApplicationSurface surface) {
    if (state_ == LifecycleState::transitioning)
        return {ErrorCode::conflict, "application transition already in progress"};
    IApplication* next = apps_.find(id);
    if (!next) return {ErrorCode::not_found, "application not found"};
    if (next == active_) return Status::success();
    if (!surface)
        return {ErrorCode::unavailable, "application surface unavailable"};
    IApplication* previous = active_;
    state_ = LifecycleState::transitioning;
    if (previous) {
        jobs_.quiesce();
        previous->stop();
    }
    const Status result = start_transactionally(*next, surface, jobs_);
    if (!result.ok()) {
        if (previous && start_transactionally(*previous, surface, jobs_).ok()) {
            active_ = previous;
            state_ = LifecycleState::active;
        } else {
            active_ = nullptr;
            state_ = LifecycleState::inactive;
        }
        return result;
    }
    active_ = next;
    state_ = LifecycleState::active;
    return Status::success();
}

void AppManager::stop() {
    if (state_ == LifecycleState::transitioning) return;
    if (state_ == LifecycleState::active) {
        state_ = LifecycleState::transitioning;
        jobs_.quiesce();
        active_->stop();
    }
    active_ = nullptr;
    state_ = LifecycleState::inactive;
}

void AppManager::tick() {
    if (state_ == LifecycleState::active) active_->tick();
}

void AppManager::dispatch(const Event& event) {
    if (state_ == LifecycleState::active) active_->on_event(event);
}

} // namespace espscreen::core
