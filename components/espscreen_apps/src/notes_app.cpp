#include "espscreen/apps/notes_app.hpp"
#include "espscreen/core/application_files.hpp"
#include "espscreen/core/application_jobs.hpp"
#include <algorithm>

namespace espscreen::apps {
core::Status NotesApp::start(core::ApplicationSurface surface) {
    view_.build(surface, *this);
    if (begin(NotesCommand::load)) enqueue(NotesCommand::load);
    return core::Status::success();
}

void NotesApp::submit(NotesCommand command, std::string_view text) {
    if (!begin(command)) return;
    if (command == NotesCommand::save) {
        transfer_size_ = std::min(text.size(), maximum_note_bytes);
        std::copy_n(text.data(), transfer_size_, transfer_.data());
        transfer_[transfer_size_] = '\0';
    }
    enqueue(command);
}

bool NotesApp::begin(NotesCommand command) {
    if (!can_submit_notes(command_.peek(), state_.load())) return false;
    if (!command_.try_reserve(command)) return false;
    state_ = pending_notes_state(command);
    return true;
}

void NotesApp::enqueue(NotesCommand command) {
    if (!jobs_.submit(*this)) {
        command_.reset();
        state_ = completed_notes_state(
            command, {core::ErrorCode::unavailable, "job submission failed"});
    }
}

void NotesApp::run() {
    const auto command = command_.take();
    core::Status result{core::ErrorCode::invalid_argument, "invalid notes command"};
    if (command == NotesCommand::save) {
        result = files_.write_text(
            "notes.txt", std::string_view{transfer_.data(), transfer_size_});
    } else if (command == NotesCommand::load) {
        result = files_.read_text(
            "notes.txt", transfer_.data(), transfer_.size(), transfer_size_);
    }
    state_ = completed_notes_state(command, result);
}

void NotesApp::tick() {
    const auto presentation = present_notes(state_.load());
    view_.render(presentation, transfer_.data());
    if (presentation.settle) state_ = NotesState::ready;
}

void NotesApp::stop() {
    view_.reset();
    transfer_size_ = 0;
    transfer_[0] = '\0';
    command_.reset();
    state_ = NotesState::ready;
}

} // namespace espscreen::apps
