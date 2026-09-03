#pragma once
#include "espscreen/board/descriptor.hpp"
namespace espscreen::generated {
inline constexpr board::BoardDescriptor kSelectedBoard{
 1,"hosyond-esp32s3-28-ili9341","Hosyond ESP32-S3 2.8-inch","Hosyond","ES3C28P","vendor-derived",
 {board::McuTarget::esp32s3,16,8,240},
 {board::DisplayDriver::ili9341,board::BusKind::spi,board::BusHost::spi2,320,240,0,40000000,board::ColorOrder::bgr,false,true,30,20,{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}},
 {board::TouchDriver::ft6336,board::BusKind::i2c,board::BusHost::i2c0,400000,0,{200,3900,200,3900},{-1,-1,-1,-1,-1,-1}},
 {board::StorageDriver::sdmmc,board::BusHost::sdmmc0,"/sd",6,20000000,{-1,-1,-1,-1,-1,-1,-1,-1}},
 {board::AudioDriver::es8311,board::BusHost::i2c0,board::BusHost::i2s0,16000,24,{-1,-1,-1,-1,-1,-1,-1,-1}},
 {-1,2.0f,3200,4200,-1,-1},
 {-1,20000,20,100,40,true},
 {board::HeaderConfig{},board::HeaderConfig{},board::HeaderConfig{},board::HeaderConfig{},board::HeaderConfig{},board::HeaderConfig{},board::HeaderConfig{},board::HeaderConfig{}},0
};
}
