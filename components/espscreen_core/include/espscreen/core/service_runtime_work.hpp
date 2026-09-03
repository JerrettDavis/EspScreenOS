#pragma once

#include "espscreen/core/runtime_loop.hpp"
#include "espscreen/core/service_manager.hpp"

namespace espscreen::core {

class ServiceRuntimeWork final : public IRuntimeWork {
public:
    explicit ServiceRuntimeWork(ServiceManager& services)
        : services_(services) {}

    void tick() override { services_.tick(); }

private:
    ServiceManager& services_;
};

} // namespace espscreen::core
