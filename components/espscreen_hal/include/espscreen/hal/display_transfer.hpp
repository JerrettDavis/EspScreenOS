#pragma once

#include "espscreen/core/status.hpp"

namespace espscreen::hal {

enum class DisplayTransferState { completed, pending };

struct DisplayTransferResult final {
    core::Status status;
    DisplayTransferState state{DisplayTransferState::completed};
};

} // namespace espscreen::hal
