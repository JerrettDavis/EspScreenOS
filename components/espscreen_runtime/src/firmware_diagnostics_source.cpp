#include "espscreen/runtime/firmware_diagnostics_source.hpp"
#include "espscreen/hal/device_health_projection.hpp"
#include "esp_heap_caps.h"

namespace espscreen::runtime {
core::SystemTelemetry FirmwareDiagnosticsSource::sample(std::uint64_t now_us) {
    storage_.poll();
    const auto power = power_.sample();
    const auto storage = storage_.stats();
    return {
        .uptime_ms = now_us / 1000,
        .free_heap = heap_caps_get_free_size(MALLOC_CAP_8BIT),
        .minimum_free_heap = heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT),
        .largest_free_block = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT),
        .free_psram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM),
        .power = {.battery_mv = power.battery_mv,
                  .battery_percent = power.battery_percent,
                  .usb_present = power.usb_present,
                  .ambient_raw = power.ambient_raw},
        .storage = {.mounted = storage.mounted,
                    .total_bytes = storage.total_bytes,
                    .free_bytes = storage.free_bytes},
        .display = hal::capability_state(display_.health()),
        .touch = hal::capability_state(touch_.health()),
        .sd = hal::capability_state(storage_.health()),
        .audio = hal::capability_state(audio_.health()),
        .power_state = hal::capability_state(power_.health()),
    };
}

} // namespace espscreen::runtime
