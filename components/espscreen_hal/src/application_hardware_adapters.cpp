#include "espscreen/hal/application_hardware_adapters.hpp"
#include "espscreen/hal/application_board_info.hpp"
#include "driver/gpio.h"

namespace espscreen::hal {

core::ApplicationBoardInfo ApplicationBoardAdapter::board_info() const { return application_board_info(descriptor_); }

core::Status ApplicationGpioAdapter::acquire_gpio(std::int16_t pin,std::uint32_t owner){return resources_.reserve({resources::ResourceKind::gpio,pin},resources::OwnerId{owner},resources::LeaseMode::exclusive);}
core::Status ApplicationGpioAdapter::release_gpio(std::int16_t pin,std::uint32_t owner){return resources_.release({resources::ResourceKind::gpio,pin},resources::OwnerId{owner});}
core::Status ApplicationGpioAdapter::configure_output(std::int16_t pin){if(!GPIO_IS_VALID_OUTPUT_GPIO(pin))return {core::ErrorCode::invalid_argument,"invalid output GPIO"};gpio_config_t config{};config.pin_bit_mask=1ULL<<pin;config.mode=GPIO_MODE_INPUT_OUTPUT;config.pull_up_en=GPIO_PULLUP_ENABLE;return gpio_config(&config)==ESP_OK?core::Status::success():core::Status{core::ErrorCode::io_error,"GPIO configuration failed"};}
core::Status ApplicationGpioAdapter::write_gpio(std::int16_t pin,bool high){if(!GPIO_IS_VALID_OUTPUT_GPIO(pin))return {core::ErrorCode::invalid_argument,"invalid output GPIO"};return gpio_set_level(static_cast<gpio_num_t>(pin),high?1:0)==ESP_OK?core::Status::success():core::Status{core::ErrorCode::io_error,"GPIO write failed"};}

} // namespace espscreen::hal
