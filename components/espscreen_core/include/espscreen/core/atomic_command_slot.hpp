#pragma once

#include <atomic>
#include <type_traits>

namespace espscreen::core {

template <typename Command, Command Empty>
class AtomicCommandSlot final {
    static_assert(std::is_enum_v<Command>);

public:
    [[nodiscard]] bool try_reserve(Command command) {
        if (command == Empty) return false;
        auto expected = Empty;
        return value_.compare_exchange_strong(
            expected, command, std::memory_order_acq_rel, std::memory_order_acquire);
    }

    [[nodiscard]] Command peek() const {
        return value_.load(std::memory_order_acquire);
    }

    [[nodiscard]] Command take() {
        return value_.exchange(Empty, std::memory_order_acq_rel);
    }

    void reset() { value_.store(Empty, std::memory_order_release); }

private:
    std::atomic<Command> value_{Empty};
};

} // namespace espscreen::core
