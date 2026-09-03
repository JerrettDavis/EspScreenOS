#pragma once
#include "espscreen/board/descriptor.hpp"
#include "espscreen/resources/resource_broker.hpp"
#include "espscreen/hal/audio_device.hpp"
#include "espscreen/hal/esp_i2s_audio_transport.hpp"
#include "espscreen/hal/display_device.hpp"
#include "espscreen/hal/haptics_controller.hpp"
#include "espscreen/hal/i2c_bus.hpp"
#include "espscreen/hal/power_device.hpp"
#include "espscreen/hal/storage_device.hpp"
#include "espscreen/hal/touch_device.hpp"
#include "espscreen/core/startup_plan.hpp"
#include <array>

namespace espscreen::hal {
class HardwarePlatform final {
public:
 explicit HardwarePlatform(const board::BoardDescriptor& descriptor);
 [[nodiscard]] core::Status start(); void stop();
 [[nodiscard]] const board::BoardDescriptor& descriptor() const { return descriptor_; }
 DisplayDevice& display() { return display_; } TouchDevice& touch() { return touch_; } StorageDevice& storage() { return storage_; }
 PowerDevice& power() { return power_; } AudioDevice& audio() { return audio_; } resources::ResourceBroker& resources() { return resources_; }
 HapticsController& haptics() { return haptics_; }
private:
 class FixedResourceLifecycle final {
 public:
  FixedResourceLifecycle(const board::BoardDescriptor& descriptor,
                         resources::ResourceBroker& resources)
      : descriptor_(descriptor), resources_(resources) {}
  [[nodiscard]] core::Status start();
  void stop();
 private:
  const board::BoardDescriptor& descriptor_;
  resources::ResourceBroker& resources_;
 };
 class OptionalHapticsLifecycle final {
 public:
  OptionalHapticsLifecycle(bool enabled, HapticsController& haptics)
      : enabled_(enabled), haptics_(haptics) {}
  [[nodiscard]] core::Status start();
  void stop();
 private:
  bool enabled_;
  HapticsController& haptics_;
 };
 using LifecyclePlan = std::array<core::StartupStep, 7>;
 [[nodiscard]] LifecyclePlan lifecycle_plan();
 const board::BoardDescriptor& descriptor_; resources::ResourceBroker resources_{}; EspI2cBus i2c_{}; DisplayDevice display_; TouchDevice touch_; StorageDevice storage_; PowerDevice power_; EspI2sAudioTransport audio_samples_; AudioDevice audio_; HapticsController haptics_; FixedResourceLifecycle fixed_resources_; OptionalHapticsLifecycle optional_haptics_; core::StartupSession lifecycle_{};
};
}
