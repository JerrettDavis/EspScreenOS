#pragma once
#include "espscreen/core/event.hpp"
#include "espscreen/core/event_mailbox_port.hpp"
#include "espscreen/core/status.hpp"
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>

namespace espscreen::core {
// Fixed-capacity task-to-UI mailbox. It is safe between FreeRTOS tasks, but intentionally not an ISR API.
template <std::size_t Capacity = 32>
class EventMailbox final : public IEventInbox, public IEventOutbox {
public:
    [[nodiscard]] Status post(const Event& event) override {
        if (!event.queue_safe()) return {ErrorCode::invalid_argument, "borrowed event payload cannot be queued"};
        Guard guard(lock_);
        if (count_ == Capacity) return {ErrorCode::capacity_exceeded, "event mailbox full"};
        queue_[tail_] = event;
        tail_ = (tail_ + 1) % Capacity;
        ++count_;
        return Status::success();
    }

    [[nodiscard]] bool try_pop(Event& event) override {
        Guard guard(lock_);
        if (count_ == 0) return false;
        event = queue_[head_];
        head_ = (head_ + 1) % Capacity;
        --count_;
        return true;
    }

    [[nodiscard]] std::size_t size() const {
        Guard guard(lock_);
        return count_;
    }

private:
    struct SpinLock final {
#ifdef _MSC_VER
        std::atomic_flag value = ATOMIC_FLAG_INIT;
#else
        volatile std::uint32_t value{0};
#endif
    };
    class Guard final {
    public:
        explicit Guard(SpinLock& lock) : lock_(lock) {
#ifdef _MSC_VER
            while (lock_.value.test_and_set(std::memory_order_acquire)) {}
#else
            while (__sync_lock_test_and_set(&lock_.value, 1U)) {}
#endif
        }
        ~Guard() {
#ifdef _MSC_VER
            lock_.value.clear(std::memory_order_release);
#else
            __sync_lock_release(&lock_.value);
#endif
        }
    private:
        SpinLock& lock_;
    };
    std::array<Event, Capacity> queue_{};
    std::size_t head_{0}, tail_{0}, count_{0};
    mutable SpinLock lock_{};
};
} // namespace espscreen::core
