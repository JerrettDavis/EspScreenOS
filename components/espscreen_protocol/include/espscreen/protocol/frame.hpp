#pragma once
#include "espscreen/core/status.hpp"
#include "espscreen/protocol/cobs.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
namespace espscreen::protocol {
inline constexpr std::uint16_t kMagic=0x5345;inline constexpr std::uint8_t kVersion=1;inline constexpr std::size_t kMaxPayload=1024;
enum class MessageType:std::uint8_t{request=1,response=2,event=3,error=4,ping=5,pong=6};
struct FrameView{MessageType type{MessageType::request};std::uint32_t request_id{0};std::span<const std::uint8_t> payload{};};
struct MutableFrame{MessageType type{MessageType::request};std::uint32_t request_id{0};std::array<std::uint8_t,kMaxPayload> payload{};std::size_t payload_size{0};};
[[nodiscard]]std::uint32_t crc32(std::span<const std::uint8_t> bytes);
[[nodiscard]]CodecResult encode_frame(const FrameView& frame,std::span<std::uint8_t> output,bool delimiter=true);
[[nodiscard]]core::Status decode_frame(std::span<const std::uint8_t> encoded,MutableFrame& output);
}
