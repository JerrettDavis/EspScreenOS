#pragma once

#include "espscreen/core/status.hpp"

namespace espscreen::runtime {

[[nodiscard]] core::Status start_firmware();
[[noreturn]] void run_firmware();

} // namespace espscreen::runtime
