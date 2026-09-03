#pragma once
#include "espscreen/core/status.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
namespace espscreen::protocol {
struct CodecResult { core::Status status{}; std::size_t bytes{0}; };
[[nodiscard]] CodecResult cobs_encode(std::span<const std::uint8_t> input,std::span<std::uint8_t> output);
[[nodiscard]] CodecResult cobs_decode(std::span<const std::uint8_t> input,std::span<std::uint8_t> output);
}
