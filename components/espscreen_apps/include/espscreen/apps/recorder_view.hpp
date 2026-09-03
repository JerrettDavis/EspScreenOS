#pragma once

#include "espscreen/apps/recorder_view_port.hpp"
#include <array>
#include <cstddef>

struct lv_event_t;
struct lv_obj_t;

namespace espscreen::apps {

class RecorderView final : public IRecorderView {
public:
    void build(core::ApplicationSurface root, IRecorderActions& actions) override;
    void render(const RecorderPresentation& presentation,
                std::size_t sample_count) override;
    void reset() override;

private:
    struct Action final {
        IRecorderActions* actions;
        RecorderCommand command;
    };
    static void action_callback(lv_event_t* event);

    lv_obj_t* root_{nullptr};
    lv_obj_t* status_{nullptr};
    std::array<Action, 2> actions_{};
};

} // namespace espscreen::apps
