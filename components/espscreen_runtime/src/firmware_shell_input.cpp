#include "espscreen/runtime/firmware_shell_input.hpp"

#include "lvgl.h"

namespace espscreen::runtime {

FirmwareShellInput::~FirmwareShellInput() { stop(); }

core::Status FirmwareShellInput::start() {
    if (input_) return core::Status::success();
    if (touch_.health().state != hal::DeviceState::ready)
        return {core::ErrorCode::unavailable, "touch hardware unavailable"};
    input_ = lv_indev_create();
    if (!input_)
        return {core::ErrorCode::unavailable, "LVGL input allocation failed"};
    lv_indev_set_type(input_, LV_INDEV_TYPE_POINTER);
    lv_indev_set_user_data(input_, this);
    lv_indev_set_read_cb(input_, reinterpret_cast<lv_indev_read_cb_t>(&read_callback));
    lv_timer_set_period(lv_indev_get_read_timer(input_), 8);
    return core::Status::success();
}

void FirmwareShellInput::stop() {
    if (input_) {
        lv_indev_delete(input_);
        input_ = nullptr;
    }
    filter_.reset();
    latest_ = {};
}

void FirmwareShellInput::read_callback(lv_indev_t* input, void* raw) {
    auto* self = static_cast<FirmwareShellInput*>(lv_indev_get_user_data(input));
    auto* data = static_cast<lv_indev_data_t*>(raw);
    if (!self) {
        data->state = LV_INDEV_STATE_RELEASED;
        return;
    }
    hal::TouchPoint sample{};
    const auto point = self->filter_.update(self->touch_.read(sample), sample,
                                             lv_tick_get());
    self->latest_ = {.pressed = point.pressed, .x = point.x, .y = point.y};
    data->state = point.pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
    data->point.x = point.x;
    data->point.y = point.y;
}

ui::TouchSample FirmwareShellInput::touch_sample() const { return latest_; }

} // namespace espscreen::runtime
