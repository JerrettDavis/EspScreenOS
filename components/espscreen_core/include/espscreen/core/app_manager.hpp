#pragma once
#include "espscreen/core/application.hpp"
#include "espscreen/core/application_host.hpp"
#include "espscreen/core/application_registry.hpp"
#include "espscreen/core/application_jobs.hpp"
#include "espscreen/core/bounded_unique_registry.hpp"
#include "espscreen/core/status.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>

namespace espscreen::core {

class AppManager final : public IApplicationRegistrar, public IApplicationHost {
public:
    static constexpr std::size_t kMaxApps = max_application_count;
    explicit AppManager(IApplicationJobs& jobs) : jobs_(jobs) {}
    [[nodiscard]] Status register_app(IApplication& app);
    [[nodiscard]] Status register_apps(std::span<IApplication* const> apps) override;
    [[nodiscard]] Status clear();
    [[nodiscard]] Status activate(std::string_view id, ApplicationSurface surface) override;
    void stop() override;
    void tick() override;
    void dispatch(const Event& event) override;
    [[nodiscard]] IApplication* active() const override {
        return state_ == LifecycleState::active ? active_ : nullptr;
    }
    [[nodiscard]] IApplication* at(std::size_t index) const override { return index < apps_.size() ? apps_[index] : nullptr; }
    [[nodiscard]] std::size_t count() const override { return apps_.size(); }

private:
    enum class LifecycleState : std::uint8_t { inactive, active, transitioning };

    BoundedUniqueRegistry<IApplication, kMaxApps> apps_{};
    IApplication* active_{nullptr};
    LifecycleState state_{LifecycleState::inactive};
    IApplicationJobs& jobs_;
};

} // namespace espscreen::core
