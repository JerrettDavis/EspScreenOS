#pragma once

#include "espscreen/core/status.hpp"
#include "espscreen/core/component_lifecycle.hpp"

namespace espscreen::runtime {

class FirmwareLvglFoundation final {
public:
    [[nodiscard]] core::Status start();
    void stop();

private:
    core::ComponentLifecycle lifecycle_{};
};

} // namespace espscreen::runtime
