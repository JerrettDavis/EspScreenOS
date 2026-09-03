#pragma once

#include "espscreen/core/application_haptics.hpp"
#include "espscreen/core/event_channel.hpp"
#include "espscreen/core/event_mailbox_port.hpp"
#include "espscreen/core/runtime_loop.hpp"
#include "espscreen/core/service_manager.hpp"
#include "espscreen/core/service_runtime_work.hpp"
#include "espscreen/core/settings_store.hpp"
#include "espscreen/hal/audio_device.hpp"
#include "espscreen/hal/display_device.hpp"
#include "espscreen/hal/power_device.hpp"
#include "espscreen/hal/storage_device.hpp"
#include "espscreen/hal/touch_device.hpp"
#include "espscreen/runtime/firmware_asset_store.hpp"
#include "espscreen/runtime/firmware_diagnostics_source.hpp"
#include "espscreen/runtime/firmware_network_transport.hpp"
#include "espscreen/runtime/firmware_provisioning_server.hpp"
#include "espscreen/runtime/firmware_settings_backend.hpp"
#include "espscreen/runtime/firmware_system_restart.hpp"
#include "espscreen/runtime/firmware_wifi_platform.hpp"
#include "espscreen/services/asset_service.hpp"
#include "espscreen/services/diagnostics_service.hpp"
#include "espscreen/services/network_service.hpp"
#include "espscreen/services/settings_service.hpp"
#include "espscreen/services/wifi_settings.hpp"
#include <cstdint>

namespace espscreen::runtime {

struct FirmwareServiceDependencies final {
    hal::DisplayDevice& display;
    hal::TouchDevice& touch;
    hal::StorageDevice& storage;
    hal::AudioDevice& audio;
    hal::PowerDevice& power;
    core::IEventSink& events;
    core::IEventOutbox& mailbox;
    core::IApplicationHaptics& haptics;
    core::IMonotonicClock& clock;
};

class FirmwareServices final {
public:
    explicit FirmwareServices(FirmwareServiceDependencies dependencies);

    [[nodiscard]] core::Status start();
    void stop();
    [[nodiscard]] core::ISettingsStore& settings() { return settings_; }
    [[nodiscard]] core::IRuntimeWork& runtime_work() { return runtime_work_; }

private:
    FirmwareDiagnosticsSource diagnostics_source_;
    FirmwareSettingsBackend settings_backend_{};
    FirmwareAssetStore asset_store_{};
    FirmwareSystemRestart system_restart_;
    FirmwareProvisioningServer provisioning_server_;
    FirmwareWifiPlatform wifi_platform_{};
    FirmwareNetworkTransport network_transport_;
    services::SettingsService settings_;
    services::WifiSettings wifi_settings_;
    services::AssetService assets_;
    services::DiagnosticsService diagnostics_;
    services::NetworkService network_;
    core::ServiceManager manager_;
    core::ServiceRuntimeWork runtime_work_;
};

} // namespace espscreen::runtime
