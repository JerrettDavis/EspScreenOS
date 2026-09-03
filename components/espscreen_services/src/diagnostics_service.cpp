#include "espscreen/services/diagnostics_service.hpp"

namespace espscreen::services {

core::Status DiagnosticsService::start() {
    const auto transition = lifecycle_.begin_start();
    if (!transition.status || !transition.acquire) return transition.status;
    next_sample_us_ = 0;
    lifecycle_.started();
    return core::Status::success();
}

void DiagnosticsService::stop() {
    if (!lifecycle_.begin_stop()) return;
    lifecycle_.stopped();
}

void DiagnosticsService::tick() {
    if (!lifecycle_.running()) return;
    const auto now = static_cast<std::uint64_t>(clock_.now_us());
    if (now < next_sample_us_) return;
    next_sample_us_ = now + 1000000;
    snapshot_ = source_.sample(now);
    events_.publish({.kind=core::EventKind::telemetry_updated,.timestamp_us=now,.payload={.telemetry=&snapshot_}});
}

} // namespace espscreen::services
