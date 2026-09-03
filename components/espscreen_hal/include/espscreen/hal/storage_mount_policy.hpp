#pragma once

#include <cstdint>

namespace espscreen::hal {

class StorageMountPolicy final {
public:
    explicit constexpr StorageMountPolicy(std::uint64_t retry_interval_us = 2'000'000)
        : retry_interval_us_(retry_interval_us) {}

    [[nodiscard]] bool should_attempt(std::uint64_t now_us) const;
    [[nodiscard]] bool mounted() const { return mounted_; }

    void mount_succeeded();
    void mount_failed(std::uint64_t now_us);
    void stop();

private:
    std::uint64_t retry_interval_us_;
    std::uint64_t last_failure_us_{0};
    bool mounted_{false};
    bool retry_waiting_{false};
};

} // namespace espscreen::hal
