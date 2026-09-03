#pragma once
#include "espscreen/core/clock.hpp"
#include "espscreen/core/component_lifecycle.hpp"
#include "espscreen/core/event_channel.hpp"
#include "espscreen/core/service.hpp"
#include "espscreen/core/system_telemetry.hpp"
#include <cstdint>
#include <string_view>

namespace espscreen::services {

class IDiagnosticsSource {
public:
    virtual ~IDiagnosticsSource() = default;
    [[nodiscard]] virtual core::SystemTelemetry sample(std::uint64_t now_us) = 0;
};

class DiagnosticsService final : public core::IService {
public:
    DiagnosticsService(IDiagnosticsSource& source, core::IEventSink& events,
                       core::IMonotonicClock& clock)
        : source_(source), events_(events), clock_(clock) {}
    [[nodiscard]] std::string_view id() const override { return "diagnostics"; }
    [[nodiscard]] core::Status start() override;
    void stop() override;
    void tick() override;
    [[nodiscard]] const core::SystemTelemetry& latest() const { return snapshot_; }
private:
    IDiagnosticsSource& source_;
    core::IEventSink& events_;
    core::IMonotonicClock& clock_;
    core::SystemTelemetry snapshot_{};
    std::uint64_t next_sample_us_{0};
    core::ComponentLifecycle lifecycle_{};
};

} // namespace espscreen::services
