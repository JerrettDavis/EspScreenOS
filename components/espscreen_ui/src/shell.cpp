#include "espscreen/ui/shell.hpp"
#include "espscreen/ui/shell_settings_persistence.hpp"
#include <algorithm>
#include <array>
#include <cstdio>

namespace espscreen::ui {
core::Status Shell::start() {
  const auto transition = lifecycle_.begin_start();
  if (!transition.status || !transition.acquire) return transition.status;
  (void)restore_shell_settings(capabilities_.settings, preferences_);
  build();
  view_.decorate_content_buttons();
  apply_power_settings();
  capabilities_.feedback.set_audio_volume(
      static_cast<std::uint8_t>(preferences_.model().audio_percent));
  auto s = events_.subscribe(*this, subscription_);
  if (!s) {
    stop();
    return s;
  }
  subscribed_ = true;
  if (apps_.count()) {
    s = activate_app(0);
    if (!s) {
      stop();
      return s;
    }
  }
  lifecycle_.started();
  return core::Status::success();
}
void Shell::apply_theme() {
  const auto palette = theme_palette(preferences_.dark_theme());
  view_.apply_theme(palette);
}
void Shell::toggle_theme() {
  preferences_.toggle_theme();
  apply_theme();
  (void)persist_shell_theme(capabilities_.settings, preferences_);
  note_activity();
}
void Shell::select_nav(const char *id) {
  view_.select_navigation(
      navigation_.is(ShellSurface::application) ? id : nullptr,
      navigation_.is(ShellSurface::drawer));
}
core::Status Shell::activate_app(std::size_t i) {
  if (i >= apps_.count())
    return {core::ErrorCode::not_found, "application index not found"};
  auto *app = apps_.at(i);
  const auto activated = apps_.activate(app->id(), view_.content_surface());
  if (!activated) return activated;
  const auto previous = navigation_.surface();
  navigation_.show_application();
  view_.move_drawer_background();
  auto presentation = present(previous, navigation_.surface());
  presentation.ingress = SurfaceAnimation::application;
  apply_presentation(presentation, true);
  view_.set_title(app->title().data());
  select_nav(app->id().data());
  view_.decorate_content_buttons();
  view_.invalidate();
  note_activity();
  return core::Status::success();
}
void Shell::activate_id(const char *id) {
  for (std::size_t i = 0; i < apps_.count(); ++i)
    if (apps_.at(i)->id() == id) {
      (void)activate_app(i);
      return;
    }
  if (!navigation_.is(ShellSurface::drawer))
    toggle_drawer();
}
void Shell::toggle_drawer() {
  const auto previous = navigation_.surface();
  navigation_.toggle_drawer();
  apply_presentation(present(previous, navigation_.surface()));
  select_nav(nullptr);
  note_activity();
}
void Shell::toggle_settings() {
  const auto previous = navigation_.surface();
  navigation_.toggle_settings();
  apply_presentation(present(previous, navigation_.surface()));
  select_nav(nullptr);
  view_.invalidate();
  note_activity();
}

void Shell::apply_presentation(const ShellPresentation &presentation,
                               bool application_activated) {
  view_.apply_presentation(presentation, application_activated);
}

void Shell::change_setting(SettingId setting, std::int32_t value) {
  const auto effects = preferences_.change(setting, value);
  if (effects.audio_volume_changed)
    capabilities_.feedback.set_audio_volume(
        static_cast<std::uint8_t>(preferences_.model().audio_percent));
  view_.update_settings(preferences_.model());
  note_activity();
  if (effects.display_changed)
    apply_power_settings();
}

void Shell::build() {
  std::array<DrawerItem, core::max_application_count> catalog{};
  for (std::size_t index = 0; index < apps_.count(); ++index) {
    const auto *app = apps_.at(index);
    catalog[index] = {.id = app->id().data(), .title = app->title().data()};
  }
  view_.build(std::span<const DrawerItem>{catalog.data(), apps_.count()},
              preferences_.model(), *this);
  apply_theme();
  (void)display_power_.note_activity(now_ms());
}

void Shell::apply_power_settings() {
  const auto &settings = preferences_.model();
  view_.set_scale(settings.scale_percent);
  const auto target = display_power_.configure(
      {.brightness = settings.brightness,
       .dim_percent = settings.idle_brightness,
       .timeout_seconds = settings.timeout_seconds});
  capabilities_.display.set_backlight(target);
}
void Shell::persist_settings() {
  (void)persist_shell_settings(capabilities_.settings, preferences_);
}
void Shell::note_activity() {
  if (const auto target = display_power_.note_activity(now_ms()))
    capabilities_.display.set_backlight(*target);
}
void Shell::tick() {
  if (!lifecycle_.running()) return;
  apps_.tick();
  const TouchSample sample = capabilities_.input.touch_sample();
  const auto decision = input_policy_.update(sample, navigation_.surface());
  if (decision.activity)
    note_activity();
  if (decision.touch_started) {
    const auto &settings = preferences_.model();
    if (settings.haptic_percent > 0)
      capabilities_.feedback.pulse(
          static_cast<std::uint8_t>(settings.haptic_percent), 30);
    if (settings.touch_sound_percent > 0)
      capabilities_.feedback.tone(
          1800, 10,
          static_cast<std::uint8_t>(settings.touch_sound_percent));
  }
  execute(plan_for(decision.intent));
  if (const auto target = display_power_.update(now_ms()))
    capabilities_.display.set_backlight(*target);
}
void Shell::on_event(const core::Event &e) {
  if (!lifecycle_.running()) return;
  apps_.dispatch(e);
  if (e.kind == core::EventKind::telemetry_updated && e.payload.telemetry)
    refresh_status(
        *e.payload.telemetry);
  else if (e.kind == core::EventKind::connectivity_changed) {
    view_.update_network(e.value0);
  }
}
std::uint32_t Shell::now_ms() const {
  return static_cast<std::uint32_t>(capabilities_.clock.now_us() / 1000);
}
void Shell::on(InputDirection direction) {
  execute(plan_for(ShellInputPolicy::gesture(navigation_.surface(), direction)));
}
void Shell::refresh_status(const core::SystemTelemetry &s) {
  const StatusViewModel model{.free_heap_bytes = s.free_heap,
                              .battery_percent = s.power.battery_percent,
                              .storage_mounted = s.storage.mounted,
                              .storage_free_bytes = s.storage.free_bytes};
  view_.update_status(model);
}
void Shell::stop() {
  if (!lifecycle_.begin_stop()) return;
  apps_.stop();
  if (subscribed_) (void)events_.unsubscribe(subscription_);
  subscribed_ = false;
  subscription_ = {};
  view_.reset();
  input_policy_.reset();
  lifecycle_.stopped();
}
void Shell::on(StatusAction action) {
  execute(plan_for(action));
}

void Shell::on(NavigationAction action) {
  execute(plan_for(action));
}

void Shell::on(SettingsAction action) {
  execute(plan_for(action));
}

void Shell::on(DrawerAction action) {
  execute(plan_for(action));
}

void Shell::on(HeaderAction action) {
  execute(plan_for(action));
}

void Shell::execute(const ShellActionPlan &plan) {
  for (std::size_t index = 0; index < plan.count; ++index)
    execute(plan.commands[index]);
}

void Shell::execute(const ShellCommand &command) {
  switch (command.kind) {
  case ShellCommandKind::toggle_drawer: toggle_drawer(); break;
  case ShellCommandKind::toggle_settings: toggle_settings(); break;
  case ShellCommandKind::toggle_status: toggle_status_panel(); break;
  case ShellCommandKind::toggle_theme: toggle_theme(); break;
  case ShellCommandKind::activate_application:
    (void)activate_app(command.application_index);
    break;
  case ShellCommandKind::activate_application_id:
    if (command.application_id) activate_id(command.application_id);
    break;
  case ShellCommandKind::change_setting:
    change_setting(command.setting, command.value);
    break;
  case ShellCommandKind::adjust_brightness:
    change_setting(SettingId::brightness,
                   preferences_.model().brightness + command.value);
    break;
  case ShellCommandKind::persist_settings: persist_settings(); break;
  }
}

void Shell::toggle_status_panel() {
  if (view_.ensure_status()) apply_theme();
  const auto previous = navigation_.surface();
  navigation_.toggle_status();
  apply_presentation(present(previous, navigation_.surface()));
  select_nav(nullptr);
  note_activity();
}
} // namespace espscreen::ui
