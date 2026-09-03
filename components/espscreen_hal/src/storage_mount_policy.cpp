#include "espscreen/hal/storage_mount_policy.hpp"

namespace espscreen::hal {

bool StorageMountPolicy::should_attempt(std::uint64_t now_us) const {
    return !mounted_ &&
           (!retry_waiting_ || now_us - last_failure_us_ >= retry_interval_us_);
}

void StorageMountPolicy::mount_succeeded() {
    mounted_ = true;
    retry_waiting_ = false;
}

void StorageMountPolicy::mount_failed(std::uint64_t now_us) {
    mounted_ = false;
    last_failure_us_ = now_us;
    retry_waiting_ = true;
}

void StorageMountPolicy::stop() {
    mounted_ = false;
    retry_waiting_ = false;
    last_failure_us_ = 0;
}

} // namespace espscreen::hal
