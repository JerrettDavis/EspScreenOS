#include "espscreen/hal/esp_i2s_audio_transport.hpp"
#include "espscreen/board/bus_host.hpp"
#include "driver/i2s_std.h"
#include "freertos/FreeRTOS.h"

namespace espscreen::hal {

EspI2sAudioTransport::~EspI2sAudioTransport() { stop(); }

core::Status EspI2sAudioTransport::start() {
    if (transmit_channel_) return core::Status::success();
    i2s_chan_config_t channel = I2S_CHANNEL_DEFAULT_CONFIG(
        static_cast<i2s_port_t>(board::bus_index(config_.data_host)), I2S_ROLE_MASTER);
    i2s_chan_handle_t transmit = nullptr;
    i2s_chan_handle_t receive = nullptr;
    if (i2s_new_channel(&channel, &transmit,
                        config_.pins.din >= 0 ? &receive : nullptr) != ESP_OK)
        return {core::ErrorCode::io_error, "i2s channel failed"};
    transmit_channel_ = transmit;
    receive_channel_ = receive;

    i2s_std_config_t standard{};
    standard.clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(config_.sample_rate);
    standard.slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(
        I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
    standard.gpio_cfg.mclk = static_cast<gpio_num_t>(config_.pins.mclk);
    standard.gpio_cfg.bclk = static_cast<gpio_num_t>(config_.pins.bclk);
    standard.gpio_cfg.ws = static_cast<gpio_num_t>(config_.pins.ws);
    standard.gpio_cfg.dout = static_cast<gpio_num_t>(config_.pins.dout);
    standard.gpio_cfg.din = static_cast<gpio_num_t>(config_.pins.din);
    standard.gpio_cfg.invert_flags = {false, false, false};
    if (i2s_channel_init_std_mode(transmit, &standard) != ESP_OK ||
        i2s_channel_enable(transmit) != ESP_OK) {
        stop();
        return {core::ErrorCode::io_error, "i2s output initialization failed"};
    }
    output_enabled_ = true;
    if (receive && (i2s_channel_init_std_mode(receive, &standard) != ESP_OK ||
                    i2s_channel_enable(receive) != ESP_OK)) {
        stop();
        return {core::ErrorCode::io_error, "i2s input initialization failed"};
    }
    return core::Status::success();
}

core::Status EspI2sAudioTransport::restart_output() {
    if (!transmit_channel_)
        return {core::ErrorCode::unsupported, "pcm output unavailable"};
    suspend_output();
    if (i2s_channel_enable(static_cast<i2s_chan_handle_t>(transmit_channel_)) != ESP_OK)
        return {core::ErrorCode::io_error, "i2s output restart failed"};
    output_enabled_ = true;
    return core::Status::success();
}

void EspI2sAudioTransport::suspend_output() {
    if (!transmit_channel_ || !output_enabled_) return;
    (void)i2s_channel_disable(static_cast<i2s_chan_handle_t>(transmit_channel_));
    output_enabled_ = false;
}

core::Status EspI2sAudioTransport::write_stereo(const std::int16_t* samples,
                                                std::size_t frames) {
    if (!transmit_channel_)
        return {core::ErrorCode::unsupported, "pcm output unavailable"};
    if (!output_enabled_) {
        if (const auto enabled = restart_output(); !enabled) return enabled;
    }
    std::size_t written = 0;
    if (i2s_channel_write(static_cast<i2s_chan_handle_t>(transmit_channel_), samples,
                          frames * 2 * sizeof(std::int16_t), &written,
                          portMAX_DELAY) != ESP_OK)
        return {core::ErrorCode::io_error, "i2s write failed"};
    return core::Status::success();
}

core::Status EspI2sAudioTransport::read_stereo(std::int16_t* samples,
                                               std::size_t frames,
                                               std::size_t& frames_read,
                                               std::uint32_t timeout_ms) {
    frames_read = 0;
    if (!receive_channel_)
        return {core::ErrorCode::unsupported, "microphone input unavailable"};
    std::size_t bytes = 0;
    if (i2s_channel_read(static_cast<i2s_chan_handle_t>(receive_channel_), samples,
                         frames * 2 * sizeof(std::int16_t), &bytes,
                         pdMS_TO_TICKS(timeout_ms)) != ESP_OK)
        return {core::ErrorCode::io_error, "i2s read failed"};
    frames_read = bytes / (sizeof(std::int16_t) * 2);
    return core::Status::success();
}

void EspI2sAudioTransport::stop() {
    if (receive_channel_) {
        (void)i2s_channel_disable(static_cast<i2s_chan_handle_t>(receive_channel_));
        (void)i2s_del_channel(static_cast<i2s_chan_handle_t>(receive_channel_));
        receive_channel_ = nullptr;
    }
    if (transmit_channel_) {
        suspend_output();
        (void)i2s_del_channel(static_cast<i2s_chan_handle_t>(transmit_channel_));
        transmit_channel_ = nullptr;
    }
}

} // namespace espscreen::hal
