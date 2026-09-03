#include "espscreen/apps/dashboard_view.hpp"
#include "lvgl.h"
#include "ui_helpers.hpp"

namespace espscreen::apps {
namespace {

void add_metric(lv_obj_t* parent, const char* icon, const char* initial,
                lv_obj_t*& value) {
    auto* card = lv_obj_create(parent);
    lv_obj_set_width(card, LV_PCT(100));
    lv_obj_set_height(card, 48);
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(card, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(card, 9, 0);
    lv_obj_set_style_pad_column(card, 10, 0);
    ui::card(card);
    auto* symbol = lv_label_create(card);
    lv_label_set_text(symbol, icon);
    ui::heading(symbol);
    value = lv_label_create(card);
    lv_label_set_text(value, initial);
    lv_label_set_long_mode(value, LV_LABEL_LONG_DOT);
    lv_obj_set_flex_grow(value, 1);
}

} // namespace

void DashboardView::build(core::ApplicationSurface surface) {
    root_ = ui::lvgl_root(surface);
    ui::prepare_page(root_);
    auto* intro = lv_obj_create(root_);
    lv_obj_set_width(intro, LV_PCT(100));
    lv_obj_set_height(intro, 54);
    lv_obj_set_flex_flow(intro, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(intro, 8, 0);
    lv_obj_set_style_pad_row(intro, 2, 0);
    ui::card(intro);
    auto* title = lv_label_create(intro);
    lv_label_set_text(title, "System overview");
    ui::heading(title);
    auto* subtitle = lv_label_create(intro);
    lv_label_set_text(subtitle, "Live device health at a glance");
    lv_obj_set_style_text_opa(subtitle, LV_OPA_60, 0);
    add_metric(root_, LV_SYMBOL_LOOP, "Uptime --", uptime_);
    add_metric(root_, LV_SYMBOL_SETTINGS, "Memory --", memory_);
    add_metric(root_, LV_SYMBOL_BATTERY_EMPTY, "Power --", power_);
    add_metric(root_, LV_SYMBOL_WIFI, "Peripherals --", peripherals_);
}

void DashboardView::render(const DashboardModel& model) {
    if (!ready()) return;
    lv_label_set_text(uptime_, model.uptime.data());
    lv_label_set_text(memory_, model.memory.data());
    lv_label_set_text(power_, model.power.data());
    lv_label_set_text(peripherals_, model.peripherals.data());
}

void DashboardView::reset() {
    if (root_) lv_obj_clean(root_);
    root_ = uptime_ = memory_ = power_ = peripherals_ = nullptr;
}

} // namespace espscreen::apps
