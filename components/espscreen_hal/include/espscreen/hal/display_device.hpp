#pragma once
#include "espscreen/board/descriptor.hpp"
#include "espscreen/core/status.hpp"
#include "espscreen/hal/device_state.hpp"
#include "espscreen/hal/display_buffer_plan.hpp"
#include "espscreen/hal/display_transfer_plan.hpp"
#include "espscreen/hal/display_transfer.hpp"
#include <cstddef>
#include <cstdint>

namespace espscreen::hal {

class DisplayDevice final {
public:
    explicit DisplayDevice(const board::DisplayConfig& config) : config_(config) {}
    ~DisplayDevice();
    [[nodiscard]] core::Status start();
    void stop();
    [[nodiscard]] core::Status set_backlight(std::uint8_t percent);
    [[nodiscard]] DisplayTransferResult transfer(DisplayTransferArea area,
                                                 const std::uint8_t* pixels,
                                                 bool source_dma_capable);
    [[nodiscard]] core::Status wait_for_transfer();
    [[nodiscard]] std::uint16_t width() const { return config_.width; }
    [[nodiscard]] std::uint16_t height() const { return config_.height; }
    [[nodiscard]] DisplayBufferPlan buffer_plan() const;
    [[nodiscard]] bool requires_full_width_updates() const;
    [[nodiscard]] bool requires_rgb565_byte_swap() const;
    [[nodiscard]] DeviceHealth health() const { return health_; }

private:
    static bool transfer_done_callback(void*, void*, void* user_context);
    [[nodiscard]] core::Status create_panel();
    [[nodiscard]] core::Status allocate_staging_buffer();

    board::DisplayConfig config_{};
    void* panel_{nullptr};
    void* io_{nullptr};
    void* dma_transfer_buffer_{nullptr};
    std::size_t dma_transfer_bytes_{0};
    void* transfer_done_{nullptr};
    bool bus_initialized_{false};
    DeviceHealth health_{};
};

} // namespace espscreen::hal
