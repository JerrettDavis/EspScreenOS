#pragma once
#include <cstdint>
#include <string_view>

namespace espscreen::core {

enum class ErrorCode : std::uint16_t {
    ok = 0,
    invalid_argument,
    not_found,
    already_exists,
    capacity_exceeded,
    busy,
    conflict,
    not_owner,
    unsupported,
    unavailable,
    timeout,
    io_error,
    corrupt,
    internal_error,
};

class Status final {
public:
    constexpr Status() = default;
    constexpr Status(ErrorCode code, std::string_view message = {}) : code_(code), message_(message) {}

    [[nodiscard]] constexpr bool ok() const { return code_ == ErrorCode::ok; }
    [[nodiscard]] constexpr explicit operator bool() const { return ok(); }
    [[nodiscard]] constexpr ErrorCode code() const { return code_; }
    [[nodiscard]] constexpr std::string_view message() const { return message_; }

    static constexpr Status success() { return {}; }

private:
    ErrorCode code_{ErrorCode::ok};
    std::string_view message_{};
};

} // namespace espscreen::core
