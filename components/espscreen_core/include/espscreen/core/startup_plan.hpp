#pragma once
#include "espscreen/core/status.hpp"
#include <concepts>
#include <cstdint>
#include <span>
#include <string_view>

namespace espscreen::core {

using StartupAction = Status (*)(void* context);
using ShutdownAction = void (*)(void* context);

struct StartupStep final {
    std::string_view name;
    bool critical{true};
    StartupAction start{nullptr};
    ShutdownAction rollback{nullptr};
    void* context{nullptr};
};

class IStartupObserver;

class StartupSession final {
public:
    [[nodiscard]] constexpr bool active() const { return active_; }
    [[nodiscard]] constexpr bool completed(std::size_t index) const {
        return index < step_count_ && (completed_ & (std::uint32_t{1} << index)) != 0;
    }
    [[nodiscard]] constexpr std::size_t step_count() const { return step_count_; }

private:
    friend Status execute_startup_plan(std::span<const StartupStep>,
                                       IStartupObserver*, StartupSession*);
    friend void shutdown_startup_plan(std::span<const StartupStep>, StartupSession*);
    std::uint32_t completed_{};
    std::size_t step_count_{};
    bool active_{};
};

template <typename Component>
concept StartupComponent = requires(Component& component) {
    { component.start() } -> std::same_as<Status>;
};

template <StartupComponent Component>
[[nodiscard]] StartupStep startup_step(std::string_view name, bool critical,
                                       Component& component) {
    ShutdownAction rollback = nullptr;
    if constexpr (requires(Component& value) {
                      { value.stop() } -> std::same_as<void>;
                  }) {
        rollback = [](void* context) { static_cast<Component*>(context)->stop(); };
    }
    return {name,
            critical,
            [](void* context) { return static_cast<Component*>(context)->start(); },
            rollback,
            &component};
}

template <typename Component>
concept TransactionalStartupComponent = StartupComponent<Component> &&
    requires(Component& component) {
        { component.stop() } -> std::same_as<void>;
    };

template <TransactionalStartupComponent Component>
[[nodiscard]] StartupStep transactional_startup_step(
    std::string_view name, bool critical, Component& component) {
    return {name,
            critical,
            [](void* context) {
                auto& value = *static_cast<Component*>(context);
                const auto result = value.start();
                if (!result) value.stop();
                return result;
            },
            [](void* context) { static_cast<Component*>(context)->stop(); },
            &component};
}

class IStartupObserver {
public:
    virtual ~IStartupObserver() = default;
    virtual void optional_step_failed(std::string_view name, const Status& status) = 0;
};

[[nodiscard]] Status execute_startup_plan(std::span<const StartupStep> steps,
                                          IStartupObserver* observer = nullptr,
                                          StartupSession* session = nullptr);
void shutdown_startup_plan(std::span<const StartupStep> steps,
                           StartupSession* session = nullptr);

} // namespace espscreen::core
