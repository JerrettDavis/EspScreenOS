#pragma once
#include "espscreen/ui/drawer_view.hpp"
#include "espscreen/ui/header_view.hpp"
#include "espscreen/ui/navigation_view.hpp"
#include "espscreen/ui/settings_view.hpp"
#include "espscreen/ui/shell_view_port.hpp"
#include "espscreen/ui/status_view.hpp"
#include <memory>
struct lv_event_t; struct lv_obj_t;
namespace espscreen::ui {
namespace motion { class ButtonStyle; }
class ShellView final : public IShellView {
public:
 ShellView();
 ~ShellView() override;
 void build(std::span<const DrawerItem>,const SettingsViewModel&,IShellActionSink&) override;
 void reset() override; void decorate_content_buttons() override;
 void apply_theme(const ThemePalette&) override; void apply_presentation(const ShellPresentation&,bool=false) override;
 [[nodiscard]] bool ensure_status() override;
 void set_scale(std::int32_t) override;
 void invalidate() override;
 void set_title(const char* value) override {header_.set_title(value);} void select_navigation(const char* id,bool drawer) override {navigation_.select(id,drawer);} void move_drawer_background() override {drawer_.move_background();}
 void update_settings(const SettingsViewModel& value) override {settings_.update(value);} void update_status(const StatusViewModel& value) override {header_.update(value);status_.update(value);} void update_network(std::int32_t state) override {status_.update_network(state);}
 [[nodiscard]]core::ApplicationSurface content_surface()const override{return core::ApplicationSurface{content_};}
private:
 static void gesture_callback(lv_event_t*);
 void decorate_buttons(lv_obj_t*);
 std::unique_ptr<motion::ButtonStyle> button_style_;
 lv_obj_t* root_{nullptr};lv_obj_t* content_{nullptr};IShellActionSink* actions_{nullptr};HeaderView header_{};NavigationView navigation_{};DrawerView drawer_{};SettingsView settings_{};StatusView status_{};
};
}
