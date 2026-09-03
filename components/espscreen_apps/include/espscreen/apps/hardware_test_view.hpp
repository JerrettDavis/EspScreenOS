#pragma once

#include "espscreen/apps/hardware_test_model.hpp"
#include "espscreen/apps/hardware_test_view_port.hpp"
#include <array>
#include <cstdint>

struct lv_event_t;
struct lv_obj_t;

namespace espscreen::apps {

class HardwareTestView final : public IHardwareTestView {
public:
    void build(core::ApplicationSurface root, const HardwareTestViewModel& model,
               IHardwareTestActions& actions) override;
    void set_intensity(std::uint8_t intensity) override;
    void set_message(const char* message) override;
    void show_storage(bool mounted, std::uint64_t free_bytes) override;
    void reset() override;

private:
    struct ActionBinding final {
        HardwareTestView* view;
        IHardwareTestActions* actions;
        HardwareViewAction action;
    };
    static void action_callback(lv_event_t* event);

    lv_obj_t* root_{nullptr};
    lv_obj_t* result_{nullptr};
    lv_obj_t* intensity_label_{nullptr};
    std::array<ActionBinding, 11> actions_{};
};

} // namespace espscreen::apps
