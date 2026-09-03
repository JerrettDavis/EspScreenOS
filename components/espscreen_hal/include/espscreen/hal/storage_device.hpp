#pragma once
#include "espscreen/board/descriptor.hpp"
#include "espscreen/core/status.hpp"
#include "espscreen/hal/device_state.hpp"
#include "espscreen/hal/storage_mount_policy.hpp"
#include <cstdint>

namespace espscreen::hal {
struct StorageStats final { bool mounted{false}; std::uint64_t total_bytes{0}; std::uint64_t free_bytes{0}; };
class StorageDevice final {
public:
 explicit StorageDevice(const board::StorageConfig& config) : config_(config) {}
 ~StorageDevice();
 [[nodiscard]] core::Status start(); void stop(); void poll(); [[nodiscard]] StorageStats stats() const; [[nodiscard]] DeviceHealth health() const { return health_; }
private:
 board::StorageConfig config_{}; void* card_{nullptr}; bool bus_initialized_{false}; StorageMountPolicy mount_policy_{}; DeviceHealth health_{};
};
}
