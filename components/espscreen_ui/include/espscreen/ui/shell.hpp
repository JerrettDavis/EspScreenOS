#pragma once
#include "espscreen/core/application_host.hpp"
#include "espscreen/core/component_lifecycle.hpp"
#include "espscreen/core/event_channel.hpp"
#include "espscreen/core/system_telemetry.hpp"
#include "espscreen/core/runtime_loop.hpp"
#include "espscreen/ui/display_power_policy.hpp"
#include "espscreen/ui/shell_view_port.hpp"
#include "espscreen/ui/shell_input_policy.hpp"
#include "espscreen/ui/shell_capabilities.hpp"
#include "espscreen/ui/shell_action_policy.hpp"
#include "espscreen/ui/shell_navigation.hpp"
#include "espscreen/ui/shell_presentation.hpp"

namespace espscreen::ui {
class Shell final : public core::IRuntimeWork, private core::IEventObserver,
                    private IShellActionSink {
public:
 Shell(core::IApplicationHost& apps,core::IEventSource& events,ShellCapabilities capabilities,
       IShellView& view)
     :apps_(apps),events_(events),capabilities_(capabilities),view_(view){}
 [[nodiscard]]core::Status start();void stop();void tick() override;
private:
 void on_event(const core::Event&) override;void on(InputDirection) override;void on(StatusAction) override;void on(NavigationAction) override;void on(SettingsAction) override;void on(DrawerAction) override;void on(HeaderAction) override;void execute(const ShellActionPlan&);void execute(const ShellCommand&);void build();void refresh_status(const core::SystemTelemetry&);void apply_theme();void apply_presentation(const ShellPresentation&,bool=false);void toggle_theme();void toggle_drawer();void toggle_settings();void toggle_status_panel();[[nodiscard]]core::Status activate_app(std::size_t);void activate_id(const char*);void select_nav(const char*);void change_setting(SettingId,std::int32_t);void apply_power_settings();void persist_settings();void note_activity();[[nodiscard]]std::uint32_t now_ms()const;
 core::IApplicationHost& apps_;core::IEventSource& events_;ShellCapabilities capabilities_;core::EventSubscription subscription_{};
 ShellSettings preferences_{};ShellInputPolicy input_policy_{};DisplayPowerPolicy display_power_{};ShellNavigation navigation_{};IShellView& view_;
 core::ComponentLifecycle lifecycle_{};bool subscribed_{false};
};
}
