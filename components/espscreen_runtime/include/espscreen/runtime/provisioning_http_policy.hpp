#pragma once

#include "espscreen/core/status.hpp"
#include <cstddef>

namespace espscreen::runtime {

inline constexpr std::size_t maximum_provisioning_form_bytes = 512;

enum class ProvisioningHttpResult { accepted, bad_request, server_error };

[[nodiscard]] constexpr ProvisioningHttpResult provisioning_http_result(
    const core::Status& status) {
    if (status) return ProvisioningHttpResult::accepted;
    return status.code() == core::ErrorCode::invalid_argument ||
                   status.code() == core::ErrorCode::capacity_exceeded
               ? ProvisioningHttpResult::bad_request
               : ProvisioningHttpResult::server_error;
}

} // namespace espscreen::runtime
