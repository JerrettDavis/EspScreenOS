#include "espscreen/hal/power_device.hpp"
#include "espscreen/hal/power_measurement.hpp"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_timer.h"
namespace espscreen::hal {
PowerDevice::~PowerDevice(){stop();}
core::Status PowerDevice::start(){health_.state=DeviceState::starting;if(config_.battery_adc<0&&config_.ambient_light_adc<0&&config_.usb_detect<0){health_={DeviceState::degraded,core::ErrorCode::unsupported,"power sensing not wired"};return {core::ErrorCode::unsupported,"power sensing not wired"};}
 adc_unit_t unit;adc_channel_t ch;if(config_.battery_adc>=0&&adc_oneshot_io_to_channel(config_.battery_adc,&unit,&ch)==ESP_OK){adc_unit_=unit;battery_channel_=ch;}if(config_.ambient_light_adc>=0&&adc_oneshot_io_to_channel(config_.ambient_light_adc,&unit,&ch)==ESP_OK){if(adc_unit_<0)adc_unit_=unit;if(adc_unit_==unit)ambient_channel_=ch;}
 if(adc_unit_>=0){adc_oneshot_unit_init_cfg_t init{};init.unit_id=static_cast<adc_unit_t>(adc_unit_);init.ulp_mode=ADC_ULP_MODE_DISABLE;adc_oneshot_unit_handle_t h;if(adc_oneshot_new_unit(&init,&h)!=ESP_OK)return {core::ErrorCode::io_error,"adc initialization failed"};adc_handle_=h;adc_oneshot_chan_cfg_t cfg{};cfg.atten=ADC_ATTEN_DB_12;cfg.bitwidth=ADC_BITWIDTH_DEFAULT;if(battery_channel_>=0)adc_oneshot_config_channel(h,static_cast<adc_channel_t>(battery_channel_),&cfg);if(ambient_channel_>=0)adc_oneshot_config_channel(h,static_cast<adc_channel_t>(ambient_channel_),&cfg);}
 if(config_.usb_detect>=0){gpio_set_direction(static_cast<gpio_num_t>(config_.usb_detect),GPIO_MODE_INPUT);gpio_pullup_en(static_cast<gpio_num_t>(config_.usb_detect));}health_={DeviceState::ready,core::ErrorCode::ok,"ready"};return core::Status::success();}
void PowerDevice::stop(){if(adc_handle_){adc_oneshot_del_unit(static_cast<adc_oneshot_unit_handle_t>(adc_handle_));adc_handle_=nullptr;}health_.state=DeviceState::stopped;}
PowerSnapshot PowerDevice::sample(){PowerSnapshot s{};s.sampled_at_us=esp_timer_get_time();if(adc_handle_&&battery_channel_>=0){int raw=0,total=0,successful=0;for(int i=0;i<8;++i){if(adc_oneshot_read(static_cast<adc_oneshot_unit_handle_t>(adc_handle_),static_cast<adc_channel_t>(battery_channel_),&raw)==ESP_OK){total+=raw;++successful;}}if(successful){const auto battery=measure_battery(total/successful,config_.battery_divider,config_.battery_empty_mv,config_.battery_full_mv);s.battery_mv=battery.millivolts;s.battery_percent=battery.percent;}}if(adc_handle_&&ambient_channel_>=0){int raw=0;if(adc_oneshot_read(static_cast<adc_oneshot_unit_handle_t>(adc_handle_),static_cast<adc_channel_t>(ambient_channel_),&raw)==ESP_OK)s.ambient_raw=raw;}if(config_.usb_detect>=0)s.usb_present=gpio_get_level(static_cast<gpio_num_t>(config_.usb_detect))!=0;return s;}
}
