#include "espscreen/apps/paint_view.hpp"
#include "lvgl.h"
#include "ui_helpers.hpp"
#include <algorithm>

namespace espscreen::apps {

PaintCanvasRequirements PaintView::prepare(core::ApplicationSurface surface) {
    root_ = ui::lvgl_root(surface);
    ui::prepare_page(root_);
    auto* toolbar = lv_obj_create(root_);
    lv_obj_set_size(toolbar, LV_PCT(100), 38);
    lv_obj_set_flex_flow(toolbar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(toolbar, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(toolbar, 2, 0);
    ui::static_container(toolbar);
    auto* title = lv_label_create(toolbar);
    lv_label_set_text(title, "Canvas");
    ui::heading(title);
    lv_obj_set_flex_grow(title, 1);
    clear_ = lv_button_create(toolbar);
    lv_obj_set_size(clear_, 64, 32);
    lv_obj_set_style_radius(clear_, 10, 0);
    lv_obj_set_style_shadow_width(clear_, 0, 0);
    auto* label = lv_label_create(clear_);
    lv_label_set_text(label, "Clear");
    lv_obj_center(label);

    lv_obj_update_layout(root_);
    const auto width = std::max(64, static_cast<int>(lv_obj_get_content_width(root_)));
    const auto height =
        std::max(64, static_cast<int>(lv_obj_get_content_height(root_) - 46));
    return {.bytes = LV_DRAW_BUF_SIZE(width, height, LV_COLOR_FORMAT_RGB565),
            .width = width,
            .height = height};
}

void PaintView::attach_canvas(void* buffer, const PaintCanvasRequirements& requirements) {
    canvas_ = lv_canvas_create(root_);
    lv_canvas_set_buffer(canvas_, buffer, requirements.width, requirements.height,
                         LV_COLOR_FORMAT_RGB565);
    lv_obj_add_flag(canvas_, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(canvas_, LV_OBJ_FLAG_SCROLLABLE);
    lv_canvas_fill_bg(canvas_, lv_color_hex(0xffffff), LV_OPA_COVER);
    has_previous_ = false;
    lv_obj_add_event_cb(clear_, &clear_callback, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(canvas_, &pointer_callback, LV_EVENT_PRESSED, this);
    lv_obj_add_event_cb(canvas_, &pointer_callback, LV_EVENT_PRESSING, this);
    lv_obj_add_event_cb(canvas_, &release_callback, LV_EVENT_RELEASED, this);
}

void PaintView::draw_to(std::int32_t x, std::int32_t y) {
    x = LV_CLAMP(0, x - lv_obj_get_x(canvas_), lv_obj_get_width(canvas_) - 1);
    y = LV_CLAMP(0, y - lv_obj_get_y(canvas_), lv_obj_get_height(canvas_) - 1);
    if (!has_previous_) {
        previous_x_ = x;
        previous_y_ = y;
    }
    lv_layer_t layer;
    lv_canvas_init_layer(canvas_, &layer);
    lv_draw_line_dsc_t line;
    lv_draw_line_dsc_init(&line);
    line.color = lv_color_hex(0x17202a);
    line.width = 6;
    line.round_start = 1;
    line.round_end = 1;
    line.p1 = {previous_x_, previous_y_};
    line.p2 = {x, y};
    lv_draw_line(&layer, &line);
    lv_canvas_finish_layer(canvas_, &layer);
    previous_x_ = x;
    previous_y_ = y;
    has_previous_ = true;
}

void PaintView::clear_callback(lv_event_t* event) {
    auto& view = *static_cast<PaintView*>(lv_event_get_user_data(event));
    lv_canvas_fill_bg(view.canvas_, lv_color_hex(0xffffff), LV_OPA_COVER);
    view.has_previous_ = false;
}

void PaintView::pointer_callback(lv_event_t* event) {
    auto* input = lv_indev_active();
    if (!input) return;
    auto& view = *static_cast<PaintView*>(lv_event_get_user_data(event));
    if (lv_event_get_code(event) == LV_EVENT_PRESSED) view.has_previous_ = false;
    lv_point_t point;
    lv_indev_get_point(input, &point);
    view.draw_to(point.x, point.y);
}

void PaintView::release_callback(lv_event_t* event) {
    static_cast<PaintView*>(lv_event_get_user_data(event))->has_previous_ = false;
}

void PaintView::reset() {
    if (root_) lv_obj_clean(root_);
    root_ = clear_ = canvas_ = nullptr;
    previous_x_ = previous_y_ = 0;
    has_previous_ = false;
}

} // namespace espscreen::apps
