#pragma once
#include "espscreen/core/event.hpp"
#include "espscreen/core/event_channel.hpp"
#include "espscreen/core/status.hpp"
#include <array>
#include <cstddef>
#include <cstdint>

namespace espscreen::core {

template <std::size_t MaxSubscribers = 24>
class EventBus final : public IEventSource, public IEventSink {
public:
    using Token = EventSubscription;

    [[nodiscard]] Status subscribe(IEventObserver& observer, Token& token) override {
        for (const auto& slot : slots_)
            if (slot.active && slot.observer == &observer)
                return {ErrorCode::already_exists,
                        "event observer already subscribed"};
        for (auto& slot : slots_) {
            if (!slot.active) {
                slot = Slot{&observer, next_token_++, true};
                token.value = slot.token;
                return Status::success();
            }
        }
        return {ErrorCode::capacity_exceeded, "event subscriber capacity exceeded"};
    }

    [[nodiscard]] Status unsubscribe(Token token) override {
        if (token.value == 0) return {ErrorCode::invalid_argument, "invalid subscription token"};
        for (auto& slot : slots_) {
            if (slot.active && slot.token == token.value) {
                slot = {};
                return Status::success();
            }
        }
        return {ErrorCode::not_found, "subscription not found"};
    }

    void publish(const Event& event) const override {
        // Snapshot-by-value prevents a callback from invalidating traversal state.
        const auto snapshot = slots_;
        for (const auto& slot : snapshot) {
            if (slot.active && slot.observer) slot.observer->on_event(event);
        }
    }

    [[nodiscard]] std::size_t subscriber_count() const {
        std::size_t count = 0;
        for (const auto& slot : slots_) if (slot.active) ++count;
        return count;
    }

private:
    struct Slot {
        IEventObserver* observer{nullptr};
        std::uint32_t token{0};
        bool active{false};
    };
    std::array<Slot, MaxSubscribers> slots_{};
    std::uint32_t next_token_{1};
};

} // namespace espscreen::core
