#include "espscreen/apps/external_application.hpp"
#include <cstdio>

namespace espscreen::apps {

bool ExternalApplication::configure(IExternalApplicationRuntime& runtime,
                                    std::string_view path) {
    if (!lifecycle_.idle() || path.empty() || path.size() >= path_.size()) return false;
    std::snprintf(path_.data(), path_.size(), "%.*s", static_cast<int>(path.size()),
                  path.data());
    ExternalApplicationDescriptor descriptor{};
    if (!runtime.inspect(path_.data(), descriptor) || descriptor.id[0] == '\0' ||
        descriptor.title[0] == '\0')
        return false;
    descriptor_ = descriptor;
    runtime_ = &runtime;
    return true;
}

core::Status ExternalApplication::start(core::ApplicationSurface surface) {
    if (!runtime_) return {core::ErrorCode::unavailable, "External app is not configured"};
    const auto transition = lifecycle_.begin_start();
    if (!transition.status || !transition.acquire) return transition.status;
    const auto result = runtime_->start(path_.data(), surface);
    if (!result) {
        runtime_->stop();
        lifecycle_.start_failed();
        return result;
    }
    lifecycle_.started();
    return core::Status::success();
}

void ExternalApplication::stop() {
    if (!lifecycle_.begin_stop()) return;
    runtime_->stop();
    lifecycle_.stopped();
}

} // namespace espscreen::apps
