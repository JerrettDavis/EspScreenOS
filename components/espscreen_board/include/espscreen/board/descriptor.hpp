#pragma once
#include <array>
#include <cstdint>

namespace espscreen::board {

enum class McuTarget : std::uint8_t { esp32, esp32s3 };
enum class DisplayDriver : std::uint8_t { st7789, ili9341, st7796u, st7796s, st77922 };
enum class TouchDriver : std::uint8_t { none, xpt2046, ft6336, gt911, cst816, cst328 };
enum class StorageDriver : std::uint8_t { none, sdspi, sdmmc };
enum class AudioDriver : std::uint8_t { none, pwm, es8311, i2s };
enum class BusKind : std::uint8_t { none, spi, qspi, soft_spi, i2c };
enum class BusHost : std::uint8_t { none, spi2, spi3, i2c0, i2c1, i2s0, i2s1, sdmmc0, uart0, uart1, uart2 };
enum class ColorOrder : std::uint8_t { rgb, bgr };
enum class HeaderKind : std::uint8_t { gpio, i2c, spi, uart, power };

struct McuConfig { McuTarget target{McuTarget::esp32}; std::uint8_t flash_mb{4}; std::uint8_t psram_mb{0}; std::uint16_t cpu_mhz{240}; };
struct DisplayPins { std::int8_t sclk{-1}, mosi{-1}, miso{-1}, cs{-1}, dc{-1}, reset{-1}, backlight{-1}, data1{-1}, data2{-1}, data3{-1}; };
struct TouchPins { std::int8_t sclk{-1}, mosi{-1}, miso{-1}, cs{-1}, irq{-1}, reset{-1}; };
struct StoragePins { std::int8_t clk{-1}, cmd{-1}, d0{-1}, cs{-1}, d1{-1}, d2{-1}, d3{-1}, card_detect{-1}; };
struct AudioPins { std::int8_t speaker{-1}, mclk{-1}, bclk{-1}, ws{-1}, dout{-1}, din{-1}, sda{-1}, scl{-1}; };

struct DisplayConfig {
    DisplayDriver driver{DisplayDriver::st7789}; BusKind bus{BusKind::spi}; BusHost host{BusHost::spi2};
    std::uint16_t width{0}, height{0}; std::uint8_t rotation{0}; std::uint32_t clock_hz{0};
    ColorOrder color_order{ColorOrder::rgb}; bool invert{false}; bool backlight_active_high{true};
    std::uint16_t draw_buffer_lines{0}; std::uint16_t transfer_buffer_lines{0}; DisplayPins pins{};
};
struct TouchConfig {
    TouchDriver driver{TouchDriver::none}; BusKind bus{BusKind::none}; BusHost host{BusHost::none};
    std::uint32_t clock_hz{0}; std::uint8_t rotation{0}; std::array<std::int16_t,4> calibration{}; TouchPins pins{};
};
struct StorageConfig {
    StorageDriver driver{StorageDriver::none}; BusHost host{BusHost::none}; const char* mount_point{"/sd"};
    std::uint8_t max_files{4}; std::uint32_t clock_hz{0}; StoragePins pins{};
};
struct AudioConfig {
    AudioDriver driver{AudioDriver::none}; BusHost control_host{BusHost::none}; BusHost data_host{BusHost::none};
    std::uint32_t sample_rate{0}; std::uint8_t codec_i2c_address{0}; AudioPins pins{};
};
struct PowerConfig {
    std::int8_t battery_adc{-1}; float battery_divider{1.0f}; std::uint16_t battery_empty_mv{3200};
    std::uint16_t battery_full_mv{4200}; std::int8_t usb_detect{-1}; std::int8_t ambient_light_adc{-1};
};
struct HapticsConfig {
    std::int8_t pin{-1}; std::uint32_t pwm_frequency_hz{20000}; std::uint8_t minimum_running_duty{20};
    std::uint8_t startup_kick_duty{100}; std::uint16_t startup_kick_ms{40}; bool startup_kick_enabled{true};
};
struct HeaderConfig {
    const char* name{""}; HeaderKind kind{HeaderKind::gpio}; BusHost host{BusHost::none}; std::array<std::int8_t,8> pins{};
    std::array<const char*,8> labels{}; std::uint8_t pin_count{0}; bool shared{false}; const char* note{""};
};
struct BoardDescriptor {
    std::uint8_t schema_version{1}; const char* id{""}; const char* display_name{""}; const char* vendor{""};
    const char* family{""}; const char* maturity{""}; McuConfig mcu{}; DisplayConfig display{}; TouchConfig touch{};
    StorageConfig storage{}; AudioConfig audio{}; PowerConfig power{}; HapticsConfig haptics{}; std::array<HeaderConfig,8> headers{};
    std::uint8_t header_count{0};
};

constexpr bool valid_pin(std::int8_t pin) { return pin >= 0; }

} // namespace espscreen::board
