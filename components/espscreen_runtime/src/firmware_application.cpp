#include "espscreen/runtime/firmware_application.hpp"
#include "espscreen/core/event_bus.hpp"
#include "espscreen/core/event_mailbox.hpp"
#include "espscreen/core/event_relay.hpp"
#include "espscreen/core/startup_plan.hpp"
#include "espscreen/core/runtime_work_sequence.hpp"
#include "espscreen/board/selected.hpp"
#include "espscreen/hal/hardware_platform.hpp"
#include "firmware_lvgl_display.hpp"
#include "firmware_applications.hpp"
#include "firmware_capabilities.hpp"
#include "firmware_services.hpp"
#include "firmware_shell.hpp"
#include "espscreen/runtime/firmware_application_jobs.hpp"
#include "espscreen/runtime/firmware_clock.hpp"
#include "espscreen/runtime/firmware_runtime.hpp"
#include "espscreen/runtime/firmware_shell_input.hpp"
#include "firmware_hardware_safety.hpp"
#include "firmware_lvgl_foundation.hpp"
#include "firmware_nvs_foundation.hpp"
#include "esp_log.h"
#include "sdkconfig.h"
#include <array>

namespace espscreen::runtime {
namespace {
constexpr const char* tag = "EspScreenOS";

class FirmwareApplication final : private core::IStartupObserver {
public:
    FirmwareApplication();
    [[nodiscard]] core::Status start();
    [[noreturn]] void run();

private:
    void optional_step_failed(std::string_view name, const core::Status& status) override;

    FirmwareClock clock_{};
    hal::HardwarePlatform hardware_{board::selected_board()};
    FirmwareHardwareSafety hardware_safety_;
    FirmwareNvsFoundation nvs_foundation_{};
    FirmwareLvglFoundation lvgl_foundation_{};
    FirmwareLvglDisplay lvgl_display_;
    FirmwareShellInput touch_input_;
    FirmwareCapabilities capabilities_;
    core::EventBus<> events_{};
    core::EventMailbox<> mailbox_{};
    core::EventRelay event_relay_;
    FirmwareApplicationJobs application_jobs_{};
    FirmwareServices services_;
    FirmwareApplications applications_;
    apps::ConfiguredApplicationDiscovery application_discovery_;
    FirmwareShell shell_;
    core::RuntimeWorkSequence<3> runtime_work_;
    FirmwareRuntime runtime_;
};

FirmwareApplication& firmware_application() {
    static FirmwareApplication application;
    return application;
}
}

FirmwareApplication::FirmwareApplication()
    : hardware_safety_(hardware_.haptics()),
      lvgl_display_(hardware_.display()), touch_input_(hardware_.touch()),
      capabilities_({.board = hardware_.descriptor(),
                     .resources = hardware_.resources(),
                     .storage = hardware_.storage(),
                     .haptics = hardware_.haptics(),
                     .audio = hardware_.audio(),
                     .display = hardware_.display(),
                     .input = touch_input_}),
      event_relay_(mailbox_, events_),
      services_({.display = hardware_.display(),
                 .touch = hardware_.touch(),
                 .storage = hardware_.storage(),
                 .audio = hardware_.audio(),
                 .power = hardware_.power(),
                 .events = events_,
                 .mailbox = mailbox_,
                 .haptics = capabilities_.haptics(),
                 .clock = clock_}),
      applications_({.dashboard = CONFIG_ESPSCREEN_APP_DASHBOARD != 0,
                     .calculator = CONFIG_ESPSCREEN_APP_CALCULATOR != 0,
                     .gpio_toolbox = CONFIG_ESPSCREEN_APP_GPIO_TOOLBOX != 0,
                     .recorder = CONFIG_ESPSCREEN_APP_RECORDER != 0,
                     .paint = CONFIG_ESPSCREEN_APP_PAINT != 0,
                     .notes = CONFIG_ESPSCREEN_APP_NOTES != 0,
                     .hardware_test = CONFIG_ESPSCREEN_APP_HARDWARE_TEST != 0,
                     .board_info = CONFIG_ESPSCREEN_APP_BOARD_INFO != 0},
                    {.board = capabilities_.board(),
                     .gpio = capabilities_.gpio(),
                     .memory = capabilities_.memory(),
                     .text_files = capabilities_.text_files(),
                     .jobs = application_jobs_,
                     .haptics = capabilities_.haptics(),
                     .tone = capabilities_.tone(),
                     .audio_input = capabilities_.audio_input(),
                     .audio_output = capabilities_.audio_output(),
                     .storage_status = capabilities_.storage_status(),
                     .settings = services_.settings()}),
      application_discovery_(applications_, capabilities_.application_directory(),
                             CONFIG_ESPSCREEN_SD_APPS != 0),
      shell_(applications_.host(), events_,
             capabilities_.shell_capabilities(services_.settings(), clock_),
             capabilities_.haptics()),
      runtime_work_(std::array<core::IRuntimeWork*, 3>{
          &services_.runtime_work(), &event_relay_, &shell_.runtime_work()}),
      runtime_(runtime_work_, clock_) {}

core::Status FirmwareApplication::start() {
    ESP_LOGI(tag, "Booting board profile %s", hardware_.descriptor().id);
    const std::array steps{
        core::startup_step("hardware safe state", true, hardware_safety_),
        core::transactional_startup_step("NVS foundation", true, nvs_foundation_),
        core::transactional_startup_step("LVGL foundation", true, lvgl_foundation_),
        core::startup_step("hardware platform", true, hardware_),
        core::startup_step("LVGL display adapter", true, lvgl_display_),
        core::startup_step("touch UI adapter", false, touch_input_),
        core::startup_step("application jobs", true, application_jobs_),
        core::startup_step("services", true, services_),
        core::startup_step("built-in applications", true, applications_),
        core::startup_step("SD applications", false, application_discovery_),
        core::startup_step("UI shell", true, shell_),
    };
    const auto result = core::execute_startup_plan(steps, this);
    if (!result) return result;

    const auto touch = hardware_.touch().health();
    ESP_LOGI(tag, "Touch state=%d: %s", static_cast<int>(touch.state),
             touch.detail ? touch.detail : "");
    events_.publish({.kind=core::EventKind::boot_complete,
                     .timestamp_us=static_cast<std::uint64_t>(clock_.now_us())});
    return core::Status::success();
}

[[noreturn]] void FirmwareApplication::run() { runtime_.run(); }

void FirmwareApplication::optional_step_failed(std::string_view name,
                                                const core::Status& status) {
    const auto detail = status.message().empty() ? std::string_view{"unavailable"} : status.message();
    ESP_LOGW(tag, "Optional startup step %.*s degraded: %.*s",
             static_cast<int>(name.size()), name.data(),
             static_cast<int>(detail.size()), detail.data());
}

core::Status start_firmware() { return firmware_application().start(); }

[[noreturn]] void run_firmware() { firmware_application().run(); }

} // namespace espscreen::runtime
