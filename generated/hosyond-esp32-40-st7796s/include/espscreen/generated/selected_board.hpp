#pragma once
#include "espscreen/board/descriptor.hpp"
namespace espscreen::generated {
inline constexpr board::BoardDescriptor kSelectedBoard{
 1,"hosyond-esp32-40-st7796s","Hosyond ESP32 4.0-inch","Hosyond","ESP32-3248","derived-verified-family",
 {board::McuTarget::esp32,4,0,240},
 {board::DisplayDriver::st7796s,board::BusKind::spi,board::BusHost::spi2,480,320,0,40000000,board::ColorOrder::bgr,false,true,30,20,{14,13,12,15,2,-1,27,-1,-1,-1}},
 {board::TouchDriver::xpt2046,board::BusKind::soft_spi,board::BusHost::i2c0,400000,0,{200,3900,200,3900},{25,32,39,33,36,-1}},
 {board::StorageDriver::sdspi,board::BusHost::spi3,"/sd",6,20000000,{18,23,19,5,-1,-1,-1,-1}},
 {board::AudioDriver::pwm,board::BusHost::none,board::BusHost::none,16000,24,{26,-1,-1,-1,-1,-1,-1,-1}},
 {-1,2.0f,3200,4200,-1,-1},
 {-1,20000,20,100,40,true},
 {board::HeaderConfig{},board::HeaderConfig{},board::HeaderConfig{},board::HeaderConfig{},board::HeaderConfig{},board::HeaderConfig{},board::HeaderConfig{},board::HeaderConfig{}},0
};
}
