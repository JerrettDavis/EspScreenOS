#pragma once

#include "espscreen/core/status.hpp"
#include "espscreen/hal/display_device.hpp"
#include <cstdint>

struct lv_display_t;
struct lv_event_t;

namespace espscreen::runtime {

class FirmwareLvglDisplay final {
public:
    explicit FirmwareLvglDisplay(hal::DisplayDevice& display) : display_(display) {}
    ~FirmwareLvglDisplay();

    [[nodiscard]] core::Status start();
    void stop();

private:
    [[nodiscard]] core::Status allocate_draw_buffers();
    static void flush_callback(lv_display_t* display, const void* area,
                               std::uint8_t* pixels);
    static void flush_wait_callback(lv_display_t* display);
    static void invalidate_area_callback(lv_event_t* event);

    hal::DisplayDevice& display_;
    lv_display_t* lv_display_{nullptr};
    void* draw_buffer_1_{nullptr};
    void* draw_buffer_2_{nullptr};
    std::size_t draw_buffer_bytes_{0};
};

} // namespace espscreen::runtime
