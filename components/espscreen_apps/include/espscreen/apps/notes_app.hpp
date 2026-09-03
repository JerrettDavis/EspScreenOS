#pragma once

#include "espscreen/core/application.hpp"
#include "espscreen/core/application_files.hpp"
#include "espscreen/core/application_jobs.hpp"
#include "espscreen/core/atomic_command_slot.hpp"
#include "espscreen/apps/notes_policy.hpp"
#include "espscreen/apps/notes_view_port.hpp"
#include <array>
#include <atomic>
#include <cstdint>

namespace espscreen::apps {

class NotesApp final : public core::IApplication, private INotesActions,
                       private core::IApplicationJob {
public:
    NotesApp(INotesView& view, core::IApplicationTextFiles& files,
             core::IApplicationJobs& jobs)
        : view_(view), jobs_(jobs), files_(files) {}

    [[nodiscard]] std::string_view id() const override { return "notes"; }
    [[nodiscard]] std::string_view title() const override { return "Notes"; }
    [[nodiscard]] core::Status start(core::ApplicationSurface surface) override;
    void stop() override;
    void tick() override;

private:
    [[nodiscard]] bool begin(NotesCommand command);
    void enqueue(NotesCommand command);
    void run() override;
    void submit(NotesCommand command, std::string_view text) override;

    INotesView& view_;
    core::IApplicationJobs& jobs_;
    core::IApplicationTextFiles& files_;
    std::array<char, maximum_note_bytes + 1> transfer_{};
    std::size_t transfer_size_{0};
    core::AtomicCommandSlot<NotesCommand, NotesCommand::none> command_{};
    std::atomic<NotesState> state_{NotesState::ready};
};

} // namespace espscreen::apps
