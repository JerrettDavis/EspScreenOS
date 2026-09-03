#include "espscreen/hal/haptics_controller.hpp"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
namespace espscreen::hal {namespace {constexpr char TAG[]="Haptics";constexpr auto mode=LEDC_LOW_SPEED_MODE;constexpr auto timer=LEDC_TIMER_3;constexpr auto channel=LEDC_CHANNEL_7;constexpr auto resolution=LEDC_TIMER_10_BIT;}
HapticsController::~HapticsController(){stop();}
core::Status HapticsController::force_pin_off(){if(config_.pin<0)return core::Status::success();gpio_config_t cfg{};cfg.pin_bit_mask=1ULL<<config_.pin;cfg.mode=GPIO_MODE_OUTPUT;cfg.pull_down_en=GPIO_PULLDOWN_ENABLE;if(gpio_config(&cfg)!=ESP_OK||gpio_set_level(static_cast<gpio_num_t>(config_.pin),0)!=ESP_OK)return {core::ErrorCode::io_error,"haptics safe state failed"};return core::Status::success();}
core::Status HapticsController::prepare_safe_state(){return force_pin_off();}
core::Status HapticsController::initialize(){if(config_.pin<0)return {core::ErrorCode::unavailable,"haptics not configured"};if(const auto safe=force_pin_off();!safe)return safe;ledc_timer_config_t t{};t.speed_mode=mode;t.duty_resolution=resolution;t.timer_num=timer;t.freq_hz=config_.pwm_frequency_hz;t.clk_cfg=LEDC_AUTO_CLK;if(ledc_timer_config(&t)!=ESP_OK){(void)force_pin_off();return {core::ErrorCode::io_error,"haptics PWM timer failed"};}ledc_channel_config_t c{};c.gpio_num=config_.pin;c.speed_mode=mode;c.channel=channel;c.intr_type=LEDC_INTR_DISABLE;c.timer_sel=timer;c.duty=0;if(ledc_channel_config(&c)!=ESP_OK){(void)force_pin_off();return {core::ErrorCode::io_error,"haptics PWM channel failed"};}write_duty(0);initialized_=true;TaskHandle_t task{};if(xTaskCreate(&HapticsController::worker_entry,"haptics",3072,this,5,&task)!=pdPASS){initialized_=false;write_duty(0);(void)force_pin_off();return {core::ErrorCode::unavailable,"haptics worker failed"};}task_=task;ESP_LOGI(TAG,"Haptics initialized: GPIO%d, %lu Hz, duty=0",config_.pin,static_cast<unsigned long>(config_.pwm_frequency_hz));return core::Status::success();}
void HapticsController::write_duty(std::uint8_t p){if(config_.pin<0)return;(void)ledc_set_duty(mode,channel,HapticsLogic::pwm_duty(p));(void)ledc_update_duty(mode,channel);if(p==0)(void)gpio_set_level(static_cast<gpio_num_t>(config_.pin),0);}
core::Status HapticsController::set_intensity(int p){return pulse(p,5000);}
core::Status HapticsController::pulse(int p,std::uint32_t ms){if(!initialized_)return {core::ErrorCode::unavailable,"haptics unavailable"};const auto plan=logic_.plan_pulse(p,ms,config_.startup_kick_enabled,config_.startup_kick_duty,config_.startup_kick_ms);if(plan.stop){off("empty pulse");return core::Status::success();}requested_intensity_=plan.intensity_percent;requested_duration_ms_=plan.duration_ms;auto g=command_generation_.fetch_add(1)+1;ESP_LOGI(TAG,"Pulse requested: intensity=%d duration=%lu generation=%lu",static_cast<int>(plan.intensity_percent),static_cast<unsigned long>(plan.duration_ms),static_cast<unsigned long>(g));xTaskNotifyGive(static_cast<TaskHandle_t>(task_));return core::Status::success();}
core::Status HapticsController::test(){return pulse(100,180);}
void HapticsController::off(const char* reason){requested_intensity_=0;requested_duration_ms_=0;command_generation_.fetch_add(1);write_duty(0);running_=false;if(task_)xTaskNotifyGive(static_cast<TaskHandle_t>(task_));ESP_LOGI(TAG,"Forced OFF: %s",reason?reason:"unspecified");}
void HapticsController::stop(){off("shutdown");initialized_=false;if(task_){vTaskDelete(static_cast<TaskHandle_t>(task_));task_=nullptr;}if(config_.pin>=0){(void)ledc_stop(mode,channel,0);(void)force_pin_off();}}
void HapticsController::worker_entry(void* p){static_cast<HapticsController*>(p)->worker();}
void HapticsController::worker(){for(;;){
 ulTaskNotifyTake(pdTRUE,portMAX_DELAY);if(!initialized_)continue;
 auto g=command_generation_.load();const auto plan=logic_.plan_pulse(requested_intensity_.load(),requested_duration_ms_.load(),config_.startup_kick_enabled,config_.startup_kick_duty,config_.startup_kick_ms);
 if(plan.stop){write_duty(0);running_=false;continue;}
 if(plan.startup_kick_ms>0){write_duty(plan.startup_kick_duty_percent);running_=true;if(ulTaskNotifyTake(pdTRUE,pdMS_TO_TICKS(plan.startup_kick_ms))||g!=command_generation_.load()){if(requested_intensity_.load()>0)xTaskNotifyGive(static_cast<TaskHandle_t>(task_));continue;}}
 if(g!=command_generation_.load()){if(requested_intensity_.load()>0)xTaskNotifyGive(static_cast<TaskHandle_t>(task_));continue;}write_duty(plan.running_duty_percent);running_=true;ESP_LOGI(TAG,"Intensity %u%% -> duty %u%%",static_cast<unsigned>(plan.intensity_percent),static_cast<unsigned>(plan.running_duty_percent));
 if(ulTaskNotifyTake(pdTRUE,pdMS_TO_TICKS(plan.duration_ms))||g!=command_generation_.load()){ESP_LOGI(TAG,"Pulse cancelled: generation=%lu",static_cast<unsigned long>(g));if(requested_intensity_.load()>0)xTaskNotifyGive(static_cast<TaskHandle_t>(task_));continue;}
 write_duty(0);running_=false;ESP_LOGI(TAG,"Pulse completed: generation=%lu",static_cast<unsigned long>(g));}}
}
