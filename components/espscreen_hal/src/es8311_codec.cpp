#include "espscreen/hal/es8311_codec.hpp"
#include "espscreen/board/bus_host.hpp"
#include "espscreen/hal/es8311_codec_plan.hpp"
#include "espscreen/hal/i2c_bus.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace espscreen::hal {

Es8311Codec::~Es8311Codec() { stop(); }

bool Es8311Codec::write(std::uint8_t address, std::uint8_t value) {
    return i2c_.write(board::bus_index(config_.control_host),
                      config_.codec_i2c_address, address, &value, 1);
}

core::Status Es8311Codec::start() {
    if (acquired_) return core::Status::success();
    auto status = i2c_.acquire(board::bus_index(config_.control_host),
                               config_.pins.sda, config_.pins.scl, 400000);
    if (!status) return status;
    acquired_ = true;
    for (const auto& command : es8311_startup_plan) {
        if (!write(command.address, command.value)) {
            stop();
            return {core::ErrorCode::io_error, "es8311 register write failed"};
        }
        if (command.delay_after_ms > 0)
            vTaskDelay(pdMS_TO_TICKS(command.delay_after_ms));
    }
    return core::Status::success();
}

void Es8311Codec::stop() {
    if (!acquired_) return;
    i2c_.release(board::bus_index(config_.control_host));
    acquired_ = false;
}

} // namespace espscreen::hal
