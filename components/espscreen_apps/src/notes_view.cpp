#include "espscreen/apps/notes_view.hpp"
#include "lvgl.h"
#include "ui_helpers.hpp"
#include <array>

namespace espscreen::apps {

void NotesView::build(core::ApplicationSurface surface, INotesActions& actions) {
    root_ = ui::lvgl_root(surface);
    ui::prepare_page(root_);

    auto* toolbar = lv_obj_create(root_);
    lv_obj_set_size(toolbar, LV_PCT(100), 40);
    lv_obj_set_flex_flow(toolbar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(toolbar, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(toolbar, 3, 0);
    lv_obj_set_style_pad_column(toolbar, 6, 0);
    ui::static_container(toolbar);

    constexpr std::array names{"Load", "Save"};
    constexpr std::array commands{NotesCommand::load, NotesCommand::save};
    for (std::size_t index = 0; index < names.size(); ++index) {
        auto* button = lv_button_create(toolbar);
        lv_obj_set_size(button, 64, 32);
        lv_obj_set_style_radius(button, 10, 0);
        lv_obj_set_style_shadow_width(button, 0, 0);
        auto* label = lv_label_create(button);
        lv_label_set_text(label, names[index]);
        lv_obj_center(label);
        actions_[index] = {this, &actions, commands[index]};
        lv_obj_add_event_cb(button, &action_callback, LV_EVENT_CLICKED, &actions_[index]);
    }

    status_ = lv_label_create(toolbar);
    lv_label_set_long_mode(status_, LV_LABEL_LONG_DOT);
    lv_obj_set_flex_grow(status_, 1);
    editor_ = lv_textarea_create(root_);
    lv_obj_set_width(editor_, LV_PCT(100));
    lv_obj_set_flex_grow(editor_, 1);
    lv_textarea_set_max_length(editor_, maximum_note_bytes);
    lv_textarea_set_placeholder_text(editor_, "Write something...");
    auto* keyboard = lv_keyboard_create(root_);
    lv_obj_set_size(keyboard, LV_PCT(100), LV_PCT(42));
    lv_keyboard_set_textarea(keyboard, editor_);
}

void NotesView::render(const NotesPresentation& presentation, const char* loaded_text) {
    if (presentation.apply_loaded_text && editor_)
        lv_textarea_set_text(editor_, loaded_text ? loaded_text : "");
    if (presentation.status && status_) lv_label_set_text(status_, presentation.status);
}

void NotesView::reset() {
    if (root_) lv_obj_clean(root_);
    root_ = editor_ = status_ = nullptr;
    actions_ = {};
}

void NotesView::action_callback(lv_event_t* event) {
    auto* action = static_cast<Action*>(lv_event_get_user_data(event));
    if (!action || !action->actions) return;
    const auto text = action->command == NotesCommand::save && action->view->editor_
                          ? std::string_view{lv_textarea_get_text(action->view->editor_)}
                          : std::string_view{};
    action->actions->submit(action->command, text);
}

} // namespace espscreen::apps
