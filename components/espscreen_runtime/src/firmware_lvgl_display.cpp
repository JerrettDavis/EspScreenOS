#include "firmware_lvgl_display.hpp"

#include "esp_heap_caps.h"
#include "esp_memory_utils.h"
#include "lvgl.h"
#include <cstring>

namespace espscreen::runtime {

FirmwareLvglDisplay::~FirmwareLvglDisplay() { stop(); }

core::Status FirmwareLvglDisplay::allocate_draw_buffers() {
    const auto plan = display_.buffer_plan();
    draw_buffer_bytes_ = plan.draw_buffer_bytes;
    if (!draw_buffer_bytes_)
        return {core::ErrorCode::invalid_argument, "invalid LVGL draw buffer size"};
    for (const auto strategy : plan.strategies) {
        const auto capabilities =
            strategy.draw_placement == hal::DisplayBufferPlacement::internal_dma
                ? MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA
                : MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT;
        draw_buffer_1_ = heap_caps_malloc(draw_buffer_bytes_, capabilities);
        draw_buffer_2_ = heap_caps_malloc(draw_buffer_bytes_, capabilities);
        if (draw_buffer_1_ && draw_buffer_2_) return core::Status::success();
        if (draw_buffer_1_) heap_caps_free(draw_buffer_1_);
        if (draw_buffer_2_) heap_caps_free(draw_buffer_2_);
        draw_buffer_1_ = draw_buffer_2_ = nullptr;
    }
    return {core::ErrorCode::unavailable, "LVGL draw buffer allocation failed"};
}

core::Status FirmwareLvglDisplay::start() {
    if (lv_display_) return core::Status::success();
    if (display_.health().state != hal::DeviceState::ready)
        return {core::ErrorCode::unavailable, "display hardware unavailable"};
    if (const auto allocated = allocate_draw_buffers(); !allocated) return allocated;
    lv_display_ = lv_display_create(display_.width(), display_.height());
    if (!lv_display_) {
        stop();
        return {core::ErrorCode::unavailable, "LVGL display allocation failed"};
    }
    lv_display_set_user_data(lv_display_, this);
    lv_display_set_flush_cb(
        lv_display_, reinterpret_cast<lv_display_flush_cb_t>(&flush_callback));
    lv_display_set_flush_wait_cb(lv_display_, &flush_wait_callback);
    lv_display_set_buffers(lv_display_, draw_buffer_1_, draw_buffer_2_,
                           draw_buffer_bytes_, LV_DISPLAY_RENDER_MODE_PARTIAL);
    if (display_.requires_full_width_updates())
        lv_display_add_event_cb(lv_display_, &invalidate_area_callback,
                                LV_EVENT_INVALIDATE_AREA, this);
    return core::Status::success();
}

void FirmwareLvglDisplay::flush_callback(lv_display_t* native_display,
                                         const void* raw_area,
                                         std::uint8_t* pixels) {
    auto* self = static_cast<FirmwareLvglDisplay*>(
        lv_display_get_user_data(native_display));
    if (!self || !raw_area || !pixels) {
        lv_display_flush_ready(native_display);
        return;
    }
    const auto& area = *static_cast<const lv_area_t*>(raw_area);
    const hal::DisplayTransferArea transfer_area{area.x1, area.y1, area.x2, area.y2};
    if (self->display_.requires_rgb565_byte_swap()) {
        const auto width = transfer_area.width();
        const auto height = transfer_area.height();
        const auto* active = lv_display_get_buf_active(native_display);
        const std::size_t stride =
            active ? active->header.stride : static_cast<std::size_t>(width) * 2;
        for (int row = 0; row < height; ++row) {
            auto* source = pixels + static_cast<std::size_t>(row) * stride;
            lv_draw_sw_rgb565_swap(source, static_cast<std::uint32_t>(width));
            auto* packed = pixels + static_cast<std::size_t>(row) * width * 2;
            if (source != packed)
                std::memmove(packed, source, static_cast<std::size_t>(width) * 2);
        }
    }
    const auto result = self->display_.transfer(
        transfer_area, pixels, esp_ptr_dma_capable(pixels));
    if (!result.status || result.state == hal::DisplayTransferState::completed)
        lv_display_flush_ready(native_display);
}

void FirmwareLvglDisplay::flush_wait_callback(lv_display_t* native_display) {
    auto* self = static_cast<FirmwareLvglDisplay*>(
        lv_display_get_user_data(native_display));
    if (self) (void)self->display_.wait_for_transfer();
}

void FirmwareLvglDisplay::invalidate_area_callback(lv_event_t* event) {
    auto* self = static_cast<FirmwareLvglDisplay*>(lv_event_get_user_data(event));
    auto* area = static_cast<lv_area_t*>(lv_event_get_param(event));
    if (self && area) {
        area->x1 = 0;
        area->x2 = static_cast<std::int32_t>(self->display_.width()) - 1;
    }
}

void FirmwareLvglDisplay::stop() {
    if (lv_display_) {
        lv_display_delete(lv_display_);
        lv_display_ = nullptr;
    }
    if (draw_buffer_1_) heap_caps_free(draw_buffer_1_);
    if (draw_buffer_2_) heap_caps_free(draw_buffer_2_);
    draw_buffer_1_ = draw_buffer_2_ = nullptr;
    draw_buffer_bytes_ = 0;
}

} // namespace espscreen::runtime
