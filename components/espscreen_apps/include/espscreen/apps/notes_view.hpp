#pragma once

#include "espscreen/apps/notes_view_port.hpp"
#include <array>
#include <string_view>

struct lv_event_t;
struct lv_obj_t;

namespace espscreen::apps {

class NotesView final : public INotesView {
public:
    void build(core::ApplicationSurface root, INotesActions& actions) override;
    void render(const NotesPresentation& presentation,
                const char* loaded_text) override;
    void reset() override;

private:
    struct Action final {
        NotesView* view;
        INotesActions* actions;
        NotesCommand command;
    };
    static void action_callback(lv_event_t* event);

    lv_obj_t* root_{nullptr};
    lv_obj_t* editor_{nullptr};
    lv_obj_t* status_{nullptr};
    std::array<Action, 2> actions_{};
};

} // namespace espscreen::apps
