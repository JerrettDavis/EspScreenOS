#include "espscreen/apps/recorder_view.hpp"
#include "lvgl.h"
#include "ui_helpers.hpp"
#include <array>
#include <cstdio>

namespace espscreen::apps {

void RecorderView::build(core::ApplicationSurface surface, IRecorderActions& actions) {
    root_ = ui::lvgl_root(surface);
    ui::prepare_page(root_);

    auto* title = lv_label_create(root_);
    lv_label_set_text(title, "Microphone recorder");
    ui::heading(title);

    status_ = lv_label_create(root_);
    lv_obj_set_style_text_opa(status_, LV_OPA_60, 0);

    auto* spacer = lv_obj_create(root_);
    lv_obj_set_width(spacer, LV_PCT(100));
    lv_obj_set_flex_grow(spacer, 1);
    lv_obj_set_style_bg_opa(spacer, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(spacer, 0, 0);
    ui::static_container(spacer);

    auto* row = lv_obj_create(root_);
    lv_obj_set_size(row, LV_PCT(100), 56);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    ui::static_container(row);

    constexpr std::array names{LV_SYMBOL_AUDIO " Record", LV_SYMBOL_PLAY " Play"};
    constexpr std::array commands{RecorderCommand::record, RecorderCommand::play};
    for (std::size_t index = 0; index < names.size(); ++index) {
        auto* button = lv_button_create(row);
        lv_obj_set_size(button, 110, 44);
        lv_obj_set_style_radius(button, 14, 0);
        lv_obj_set_style_shadow_width(button, 0, 0);
        auto* label = lv_label_create(button);
        lv_label_set_text(label, names[index]);
        lv_obj_center(label);
        actions_[index] = {&actions, commands[index]};
        lv_obj_add_event_cb(button, &action_callback, LV_EVENT_CLICKED, &actions_[index]);
    }
    render(present_recorder(RecorderState::ready), 0);
}

void RecorderView::render(const RecorderPresentation& presentation,
                          std::size_t sample_count) {
    if (!status_) return;
    if (!presentation.include_sample_count) {
        lv_label_set_text(status_, presentation.status);
        return;
    }
    char text[64]{};
    std::snprintf(text, sizeof(text), presentation.status,
                  static_cast<unsigned>(sample_count));
    lv_label_set_text(status_, text);
}

void RecorderView::reset() {
    if (root_) lv_obj_clean(root_);
    root_ = status_ = nullptr;
    actions_ = {};
}

void RecorderView::action_callback(lv_event_t* event) {
    auto* action = static_cast<Action*>(lv_event_get_user_data(event));
    if (action && action->actions) action->actions->submit(action->command);
}

} // namespace espscreen::apps
