#pragma once

#include "espscreen/core/application.hpp"
#include "espscreen/core/application_memory.hpp"
#include "espscreen/core/application_jobs.hpp"
#include "espscreen/core/application_audio_input.hpp"
#include "espscreen/core/application_audio_output.hpp"
#include "espscreen/core/atomic_command_slot.hpp"
#include "espscreen/apps/recorder_buffer_plan.hpp"
#include "espscreen/apps/recorder_view_port.hpp"
#include <atomic>
#include <cstddef>
#include <cstdint>

namespace espscreen::apps {

class RecorderApp final : public core::IApplication, private IRecorderActions,
                          private core::IApplicationJob {
public:
    RecorderApp(IRecorderView& view, core::IApplicationMemory& memory,
                core::IApplicationJobs& jobs, core::IApplicationAudioInput& input,
                core::IApplicationAudioOutput& output)
        : view_(view), input_(input), output_(output), jobs_(jobs), memory_(memory) {}

    [[nodiscard]] std::string_view id() const override { return "recorder"; }
    [[nodiscard]] std::string_view title() const override { return "Audio"; }
    [[nodiscard]] core::Status start(core::ApplicationSurface surface) override;
    void stop() override;
    void tick() override;

private:
    void submit(RecorderCommand command) override;
    void run() override;

    IRecorderView& view_;
    core::IApplicationAudioInput& input_;
    core::IApplicationAudioOutput& output_;
    core::IApplicationJobs& jobs_;
    core::IApplicationMemory& memory_;
    std::int16_t* samples_{nullptr};
    std::int16_t* playback_{nullptr};
    RecorderBufferPlan buffer_plan_{};
    std::size_t sample_count_{0};
    core::AtomicCommandSlot<RecorderCommand, RecorderCommand::none> command_{};
    std::atomic<RecorderState> state_{RecorderState::ready};
    std::atomic<std::size_t> captured_{0};
};

} // namespace espscreen::apps
