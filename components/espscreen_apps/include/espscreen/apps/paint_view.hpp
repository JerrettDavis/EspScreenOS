#pragma once

#include "espscreen/apps/paint_view_port.hpp"

struct lv_event_t;
struct lv_obj_t;

namespace espscreen::apps {

class PaintView final : public IPaintView {
public:
    [[nodiscard]] PaintCanvasRequirements prepare(
        core::ApplicationSurface root) override;
    void attach_canvas(void* buffer,
                       const PaintCanvasRequirements& requirements) override;
    void reset() override;

private:
    void draw_to(std::int32_t x, std::int32_t y);
    static void clear_callback(lv_event_t* event);
    static void pointer_callback(lv_event_t* event);
    static void release_callback(lv_event_t* event);

    lv_obj_t* root_{nullptr};
    lv_obj_t* clear_{nullptr};
    lv_obj_t* canvas_{nullptr};
    std::int32_t previous_x_{0};
    std::int32_t previous_y_{0};
    bool has_previous_{false};
};

} // namespace espscreen::apps
