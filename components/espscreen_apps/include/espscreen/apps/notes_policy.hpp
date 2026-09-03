#pragma once

#include "espscreen/core/status.hpp"
#include <cstddef>
#include <cstdint>

namespace espscreen::apps {

inline constexpr std::size_t maximum_note_bytes = 4096;

enum class NotesCommand : std::uint8_t { none, load, save };
enum class NotesState : std::uint8_t {
    ready, loading, saving, loaded, saved, load_missing, load_error, save_error
};

struct NotesPresentation final {
    const char* status{nullptr};
    bool apply_loaded_text{false};
    bool settle{false};
};

[[nodiscard]] constexpr bool can_submit_notes(NotesCommand pending, NotesState state) {
    return pending == NotesCommand::none && state != NotesState::loading &&
           state != NotesState::saving;
}

[[nodiscard]] constexpr NotesState pending_notes_state(NotesCommand command) {
    return command == NotesCommand::load ? NotesState::loading : NotesState::saving;
}

[[nodiscard]] constexpr NotesState completed_notes_state(
    NotesCommand command, const core::Status& result) {
    if (result) return command == NotesCommand::load ? NotesState::loaded : NotesState::saved;
    if (command == NotesCommand::load && result.code() == core::ErrorCode::not_found)
        return NotesState::load_missing;
    return command == NotesCommand::load ? NotesState::load_error : NotesState::save_error;
}

[[nodiscard]] constexpr NotesPresentation present_notes(NotesState state) {
    switch (state) {
    case NotesState::loading: return {.status = "Loading..."};
    case NotesState::saving: return {.status = "Saving..."};
    case NotesState::loaded: return {.status = "Loaded", .apply_loaded_text = true, .settle = true};
    case NotesState::saved: return {.status = "Saved", .settle = true};
    case NotesState::load_missing: return {.status = "New note (SD optional)", .settle = true};
    case NotesState::load_error: return {.status = "Load failed", .settle = true};
    case NotesState::save_error: return {.status = "Save failed: insert SD", .settle = true};
    default: return {};
    }
}

} // namespace espscreen::apps
