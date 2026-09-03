#include "espscreen/apps/board_info_view.hpp"
#include "lvgl.h"
#include "ui_helpers.hpp"

namespace espscreen::apps {
namespace {

void add_text_card(lv_obj_t* parent, const char* text) {
    auto* card = lv_obj_create(parent);
    lv_obj_set_width(card, LV_PCT(100));
    lv_obj_set_height(card, LV_SIZE_CONTENT);
    ui::card(card);
    auto* label = lv_label_create(card);
    lv_label_set_text(label, text);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(label, LV_PCT(100));
}

} // namespace

void BoardInfoView::build(core::ApplicationSurface surface,
                          const BoardInfoModel& model) {
    root_ = ui::lvgl_root(surface);
    ui::prepare_page(root_, true);
    auto* title = lv_label_create(root_);
    lv_label_set_text(title, model.title.data());
    ui::heading(title);
    add_text_card(root_, model.summary.data());
    for (std::size_t index = 0; index < model.header_count; ++index)
        add_text_card(root_, model.headers[index].data());
}

void BoardInfoView::reset() {
    if (root_) lv_obj_clean(root_);
    root_ = nullptr;
}

} // namespace espscreen::apps
