#pragma once

#include "espscreen/core/status.hpp"
#include <atomic>
#include <cstdint>
#include <string_view>

namespace espscreen::core {

class AtomicComponentLifecycle final {
public:
    struct Start final {
        Status status{};
        bool acquire{false};
    };

    [[nodiscard]] Start begin_start(std::string_view conflict_message) {
        auto expected = State::stopped;
        if (state_.compare_exchange_strong(expected, State::starting,
                                           std::memory_order_acq_rel,
                                           std::memory_order_acquire))
            return {.acquire = true};
        if (expected == State::running) return {};
        return {{ErrorCode::conflict, conflict_message}, false};
    }

    void started() { state_.store(State::running, std::memory_order_release); }

    [[nodiscard]] bool begin_stop() {
        const auto previous = state_.exchange(State::stopping, std::memory_order_acq_rel);
        return previous != State::stopped && previous != State::stopping;
    }

    void stopped() { state_.store(State::stopped, std::memory_order_release); }
    [[nodiscard]] bool running() const {
        return state_.load(std::memory_order_acquire) == State::running;
    }

private:
    enum class State : std::uint8_t { stopped, starting, running, stopping };
    std::atomic<State> state_{State::stopped};
};

} // namespace espscreen::core
