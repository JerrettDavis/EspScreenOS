#include "espscreen/runtime/firmware_runtime.hpp"
#include "esp_log.h"
#include "lvgl.h"

namespace espscreen::runtime {
namespace { constexpr const char* tag = "EspScreenOS"; }

FirmwareRuntime::FirmwareRuntime(core::IRuntimeWork& work,
                                 core::IRuntimeClock& clock)
    : loop_(work, *this, clock, this) {}

[[noreturn]] void FirmwareRuntime::run() { loop_.run(); }

std::uint32_t FirmwareRuntime::handle() { return lv_timer_handler(); }
bool FirmwareRuntime::animations_running() { return lv_anim_count_running() > 0; }
void FirmwareRuntime::first_frame(std::int64_t handler_us) {
    ESP_LOGI(tag, "First LVGL frame completed in %lld ms", handler_us / 1000);
}
void FirmwareRuntime::animation_pacing(const core::AnimationPacing& pacing) {
    ESP_LOGI(tag, "Animation pacing: samples=%lu avg=%lldus max=%lldus handler_max=%lldus late=%lu",
             static_cast<unsigned long>(pacing.samples), pacing.average_gap_us,
             pacing.maximum_gap_us, pacing.maximum_handler_us,
             static_cast<unsigned long>(pacing.late_samples));
}

} // namespace espscreen::runtime
