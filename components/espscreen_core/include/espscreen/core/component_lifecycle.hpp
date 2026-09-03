#pragma once

#include "espscreen/core/status.hpp"
#include <cstdint>

namespace espscreen::core {

class ComponentLifecycle final {
public:
    struct Start final {
        Status status{};
        bool acquire{false};
    };

    [[nodiscard]] constexpr Start begin_start() {
        if (state_ == State::running) return {};
        if (state_ != State::stopped)
            return {{ErrorCode::conflict, "component lifecycle transition in progress"}, false};
        state_ = State::starting;
        return {.acquire = true};
    }

    constexpr void started() { state_ = State::running; }
    constexpr void start_failed() { state_ = State::stopped; }

    [[nodiscard]] constexpr bool begin_stop() {
        if (state_ == State::stopped || state_ == State::stopping) return false;
        state_ = State::stopping;
        return true;
    }

    constexpr void stopped() { state_ = State::stopped; }
    [[nodiscard]] constexpr bool running() const { return state_ == State::running; }
    [[nodiscard]] constexpr bool idle() const { return state_ == State::stopped; }

private:
    enum class State : std::uint8_t { stopped, starting, running, stopping };
    State state_{State::stopped};
};

} // namespace espscreen::core
