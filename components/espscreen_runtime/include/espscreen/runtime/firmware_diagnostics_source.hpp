#pragma once
#include "espscreen/hal/audio_device.hpp"
#include "espscreen/hal/display_device.hpp"
#include "espscreen/hal/power_device.hpp"
#include "espscreen/hal/storage_device.hpp"
#include "espscreen/hal/touch_device.hpp"
#include "espscreen/services/diagnostics_service.hpp"

namespace espscreen::runtime {

class FirmwareDiagnosticsSource final : public services::IDiagnosticsSource {
public:
    FirmwareDiagnosticsSource(hal::DisplayDevice& display, hal::TouchDevice& touch,
                              hal::StorageDevice& storage, hal::AudioDevice& audio,
                              hal::PowerDevice& power)
        : display_(display), touch_(touch), storage_(storage), audio_(audio), power_(power) {}
    [[nodiscard]] core::SystemTelemetry sample(std::uint64_t now_us) override;
private:
    hal::DisplayDevice& display_;
    hal::TouchDevice& touch_;
    hal::StorageDevice& storage_;
    hal::AudioDevice& audio_;
    hal::PowerDevice& power_;
};

} // namespace espscreen::runtime
