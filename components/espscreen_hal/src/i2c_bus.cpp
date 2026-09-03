#include "espscreen/hal/i2c_bus.hpp"
#include "driver/i2c.h"
#include <cstring>

namespace espscreen::hal {
core::Status EspI2cBus::acquire(int port, int sda, int scl, std::uint32_t clock_hz) {
 const auto decision=leases_.acquire({port,sda,scl,clock_hz});
 if (!decision.status || !decision.initialize_hardware) return decision.status;
 i2c_config_t cfg{}; cfg.mode=I2C_MODE_MASTER; cfg.sda_io_num=static_cast<gpio_num_t>(sda); cfg.scl_io_num=static_cast<gpio_num_t>(scl);
 cfg.sda_pullup_en=GPIO_PULLUP_ENABLE; cfg.scl_pullup_en=GPIO_PULLUP_ENABLE; cfg.master.clk_speed=clock_hz;
 if (i2c_param_config(static_cast<i2c_port_t>(port),&cfg)!=ESP_OK || i2c_driver_install(static_cast<i2c_port_t>(port),I2C_MODE_MASTER,0,0,0)!=ESP_OK) {
  (void)leases_.release(port);
  return {core::ErrorCode::io_error,"i2c initialization failed"};
 }
 return core::Status::success();
}
void EspI2cBus::release(int port) {
 if (leases_.release(port)) (void)i2c_driver_delete(static_cast<i2c_port_t>(port));
}
bool EspI2cBus::read(int port,std::uint8_t address,std::uint8_t reg,void* data,std::size_t size) {
 return i2c_master_write_read_device(static_cast<i2c_port_t>(port),address,&reg,1,static_cast<std::uint8_t*>(data),size,pdMS_TO_TICKS(100))==ESP_OK;
}
bool EspI2cBus::read16(int port,std::uint8_t address,std::uint16_t reg,void* data,std::size_t size) {
 const std::uint8_t address_bytes[]{static_cast<std::uint8_t>(reg>>8),static_cast<std::uint8_t>(reg)};
 return i2c_master_write_read_device(static_cast<i2c_port_t>(port),address,address_bytes,sizeof(address_bytes),static_cast<std::uint8_t*>(data),size,pdMS_TO_TICKS(100))==ESP_OK;
}
bool EspI2cBus::write(int port,std::uint8_t address,std::uint8_t reg,const void* data,std::size_t size) {
 std::uint8_t buffer[32]; if (size+1>sizeof(buffer)) return false; buffer[0]=reg; std::memcpy(buffer+1,data,size);
 return i2c_master_write_to_device(static_cast<i2c_port_t>(port),address,buffer,size+1,pdMS_TO_TICKS(100))==ESP_OK;
}
}
