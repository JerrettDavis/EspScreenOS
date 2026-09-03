#include "espscreen/hal/display_device.hpp"
#include "espscreen/hal/display_buffer_plan.hpp"
#include "espscreen/hal/display_transfer_plan.hpp"
#include "espscreen/board/bus_host.hpp"
#include "dcs_panel.hpp"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/spi_master.h"
#include "esp_heap_caps.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_st77922.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <algorithm>
#include <cstring>

namespace espscreen::hal {
namespace { constexpr const char* TAG="display"; spi_host_device_t host(std::int8_t index){return index==2?SPI2_HOST:SPI3_HOST;} }
DisplayDevice::~DisplayDevice(){stop();}
core::Status DisplayDevice::start(){
 if(health_.state==DeviceState::ready)return core::Status::success();
 health_.state=DeviceState::starting;
 const auto buffers=display_buffer_plan(config_.width,config_.draw_buffer_lines,config_.transfer_buffer_lines);if(!buffers){health_={DeviceState::failed,core::ErrorCode::invalid_argument,"invalid display buffer plan"};return {core::ErrorCode::invalid_argument,"invalid display buffer plan"};}
 auto status=create_panel();if(!status){health_={DeviceState::failed,status.code(),"display panel initialization failed"};return status;}
 status=allocate_staging_buffer();if(!status){stop();health_={DeviceState::failed,status.code(),"display staging buffer allocation failed"};return status;}
 (void)set_backlight(100);health_={DeviceState::ready,core::ErrorCode::ok,"ready"};return core::Status::success();
}
core::Status DisplayDevice::create_panel(){
 if(!board::host_belongs_to(config_.host,board::BusFamily::spi))return {core::ErrorCode::invalid_argument,"display requires an SPI host"};
 const auto h=host(board::bus_index(config_.host));spi_bus_config_t bus{};bus.sclk_io_num=config_.pins.sclk;
 if(config_.bus==board::BusKind::qspi){bus.data0_io_num=config_.pins.mosi;bus.data1_io_num=config_.pins.data1;bus.data2_io_num=config_.pins.data2;bus.data3_io_num=config_.pins.data3;}else{bus.mosi_io_num=config_.pins.mosi;bus.miso_io_num=config_.pins.miso;bus.quadwp_io_num=-1;bus.quadhd_io_num=-1;}
 bus.max_transfer_sz=static_cast<int>(display_buffer_plan(config_.width,config_.draw_buffer_lines,config_.transfer_buffer_lines).maximum_spi_transfer_bytes);bus.flags=SPICOMMON_BUSFLAG_MASTER;
 if(spi_bus_initialize(h,&bus,SPI_DMA_CH_AUTO)!=ESP_OK)return {core::ErrorCode::io_error,"spi bus initialization failed"};
 bus_initialized_=true;
 transfer_done_=xSemaphoreCreateBinary();if(!transfer_done_)return {core::ErrorCode::unavailable,"display semaphore allocation failed"};
 esp_lcd_panel_io_spi_config_t io{};io.dc_gpio_num=config_.pins.dc;io.cs_gpio_num=config_.pins.cs;io.pclk_hz=config_.clock_hz;io.lcd_cmd_bits=config_.bus==board::BusKind::qspi?32:8;io.lcd_param_bits=8;io.spi_mode=0;io.trans_queue_depth=2;io.on_color_trans_done=reinterpret_cast<esp_lcd_panel_io_color_trans_done_cb_t>(&DisplayDevice::transfer_done_callback);io.user_ctx=this;io.flags.quad_mode=config_.bus==board::BusKind::qspi;
 if(esp_lcd_new_panel_io_spi(static_cast<esp_lcd_spi_bus_handle_t>(h),&io,reinterpret_cast<esp_lcd_panel_io_handle_t*>(&io_))!=ESP_OK)return {core::ErrorCode::io_error,"lcd panel io failed"};
 esp_lcd_panel_dev_config_t panel_cfg{};panel_cfg.reset_gpio_num=config_.pins.reset;panel_cfg.rgb_ele_order=config_.color_order==board::ColorOrder::bgr?LCD_RGB_ELEMENT_ORDER_BGR:LCD_RGB_ELEMENT_ORDER_RGB;panel_cfg.bits_per_pixel=16;
 esp_err_t err=ESP_FAIL;
 if(config_.driver==board::DisplayDriver::st7789)err=esp_lcd_new_panel_st7789(reinterpret_cast<esp_lcd_panel_io_handle_t>(io_),&panel_cfg,reinterpret_cast<esp_lcd_panel_handle_t*>(&panel_));
 else if(config_.driver==board::DisplayDriver::st77922)err=detail::new_dcs_panel(reinterpret_cast<esp_lcd_panel_io_handle_t>(io_),config_,reinterpret_cast<esp_lcd_panel_handle_t*>(&panel_));
 else err=detail::new_dcs_panel(reinterpret_cast<esp_lcd_panel_io_handle_t>(io_),config_,reinterpret_cast<esp_lcd_panel_handle_t*>(&panel_));
 if(err!=ESP_OK)return {core::ErrorCode::io_error,"lcd panel driver failed"};
 esp_lcd_panel_reset(reinterpret_cast<esp_lcd_panel_handle_t>(panel_));esp_lcd_panel_init(reinterpret_cast<esp_lcd_panel_handle_t>(panel_));
 // The known-good ST77922 sequence already programs inversion and MADCTL. Sending
 // generic transforms here would undo those board-specific values (INVON/RGB).
 if(config_.driver!=board::DisplayDriver::st77922){
  esp_lcd_panel_invert_color(reinterpret_cast<esp_lcd_panel_handle_t>(panel_),config_.invert);
  const bool swap=config_.rotation==1||config_.rotation==3;esp_lcd_panel_swap_xy(reinterpret_cast<esp_lcd_panel_handle_t>(panel_),swap);
  esp_lcd_panel_mirror(reinterpret_cast<esp_lcd_panel_handle_t>(panel_),config_.rotation==2||config_.rotation==3,config_.rotation==1||config_.rotation==2);
 }
 if(board::valid_pin(config_.pins.backlight)){ledc_timer_config_t t{};t.speed_mode=LEDC_LOW_SPEED_MODE;t.timer_num=LEDC_TIMER_0;t.duty_resolution=LEDC_TIMER_10_BIT;t.freq_hz=20000;t.clk_cfg=LEDC_AUTO_CLK;ledc_timer_config(&t);ledc_channel_config_t c{};c.gpio_num=config_.pins.backlight;c.speed_mode=LEDC_LOW_SPEED_MODE;c.channel=LEDC_CHANNEL_0;c.timer_sel=LEDC_TIMER_0;c.duty=0;ledc_channel_config(&c);}return core::Status::success();
}
core::Status DisplayDevice::allocate_staging_buffer(){
 const auto plan=display_buffer_plan(config_.width,config_.draw_buffer_lines,config_.transfer_buffer_lines);if(!plan)return {core::ErrorCode::invalid_argument,"invalid display buffer plan"};dma_transfer_bytes_=plan.staging_buffer_bytes;
 dma_transfer_buffer_=heap_caps_malloc(dma_transfer_bytes_,MALLOC_CAP_INTERNAL|MALLOC_CAP_DMA);if(!dma_transfer_buffer_)return {core::ErrorCode::unavailable,"DMA staging buffer allocation failed"};return core::Status::success();
}
bool DisplayDevice::transfer_done_callback(void*,void*,void* user){auto* self=static_cast<DisplayDevice*>(user);if(!self)return false;BaseType_t wake=pdFALSE;if(self->transfer_done_)xSemaphoreGiveFromISR(reinterpret_cast<SemaphoreHandle_t>(self->transfer_done_),&wake);return wake==pdTRUE;}
DisplayTransferResult DisplayDevice::transfer(DisplayTransferArea a,const std::uint8_t* pixels,bool source_dma_capable){auto panel=reinterpret_cast<esp_lcd_panel_handle_t>(panel_);if(health_.state!=DeviceState::ready||!pixels)return {{core::ErrorCode::unavailable,"display is not ready"},DisplayTransferState::completed};
 if(source_dma_capable){xSemaphoreTake(reinterpret_cast<SemaphoreHandle_t>(transfer_done_),0);const auto err=esp_lcd_panel_draw_bitmap(panel,a.x1,a.y1,a.x2+1,a.y2+1,pixels);if(err!=ESP_OK){ESP_LOGE(TAG,"display transfer start failed: %s",esp_err_to_name(err));return {{core::ErrorCode::io_error,"display transfer failed"},DisplayTransferState::completed};}return {core::Status::success(),DisplayTransferState::pending};}
 for(std::int32_t row=0;row<a.height();){const auto chunk=staged_display_transfer_chunk(a,dma_transfer_bytes_,row);if(!chunk)return {{core::ErrorCode::invalid_argument,"DMA staging buffer is smaller than one display row"},DisplayTransferState::completed};std::memcpy(dma_transfer_buffer_,pixels+chunk.source_offset,chunk.byte_count);xSemaphoreTake(reinterpret_cast<SemaphoreHandle_t>(transfer_done_),0);const auto err=esp_lcd_panel_draw_bitmap(panel,a.x1,chunk.y1,a.x2+1,chunk.y_end,dma_transfer_buffer_);if(err!=ESP_OK){ESP_LOGE(TAG,"display transfer start failed at row %d: %s",static_cast<int>(row),esp_err_to_name(err));return {{core::ErrorCode::io_error,"display transfer failed"},DisplayTransferState::completed};}if(xSemaphoreTake(reinterpret_cast<SemaphoreHandle_t>(transfer_done_),pdMS_TO_TICKS(1000))!=pdTRUE)return {{core::ErrorCode::timeout,"display transfer timed out"},DisplayTransferState::completed};row+=chunk.row_count;}return {core::Status::success(),DisplayTransferState::completed};
}
core::Status DisplayDevice::wait_for_transfer(){if(!transfer_done_)return {core::ErrorCode::unavailable,"display transfer synchronization unavailable"};return xSemaphoreTake(reinterpret_cast<SemaphoreHandle_t>(transfer_done_),pdMS_TO_TICKS(1000))==pdTRUE?core::Status::success():core::Status{core::ErrorCode::timeout,"display transfer timed out"};}
DisplayBufferPlan DisplayDevice::buffer_plan()const{return display_buffer_plan(config_.width,config_.draw_buffer_lines,config_.transfer_buffer_lines);}
bool DisplayDevice::requires_full_width_updates()const{return config_.driver==board::DisplayDriver::st77922;}
bool DisplayDevice::requires_rgb565_byte_swap()const{return config_.driver==board::DisplayDriver::st77922;}
core::Status DisplayDevice::set_backlight(std::uint8_t percent){if(!board::valid_pin(config_.pins.backlight))return {core::ErrorCode::unsupported,"no backlight control"};percent=std::min<std::uint8_t>(percent,100);std::uint32_t duty=(1023u*percent)/100u;if(!config_.backlight_active_high)duty=1023u-duty;ledc_set_duty(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_0,duty);ledc_update_duty(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_0);return core::Status::success();}
void DisplayDevice::stop(){if(panel_){esp_lcd_panel_del(reinterpret_cast<esp_lcd_panel_handle_t>(panel_));panel_=nullptr;}if(io_){esp_lcd_panel_io_del(reinterpret_cast<esp_lcd_panel_io_handle_t>(io_));io_=nullptr;}if(bus_initialized_){spi_bus_free(host(board::bus_index(config_.host)));bus_initialized_=false;}if(transfer_done_){vSemaphoreDelete(reinterpret_cast<SemaphoreHandle_t>(transfer_done_));transfer_done_=nullptr;}if(dma_transfer_buffer_)heap_caps_free(dma_transfer_buffer_);dma_transfer_buffer_=nullptr;health_.state=DeviceState::stopped;}
}
