#pragma once

#include "espscreen/apps/notes_policy.hpp"
#include "espscreen/core/application.hpp"
#include <string_view>

namespace espscreen::apps {

class INotesActions {
public:
    virtual ~INotesActions() = default;
    virtual void submit(NotesCommand command, std::string_view text) = 0;
};

class INotesView {
public:
    virtual ~INotesView() = default;
    virtual void build(core::ApplicationSurface root, INotesActions& actions) = 0;
    virtual void render(const NotesPresentation& presentation,
                        const char* loaded_text) = 0;
    virtual void reset() = 0;
};

} // namespace espscreen::apps
