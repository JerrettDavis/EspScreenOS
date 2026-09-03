#pragma once
#include "espscreen/board/descriptor.hpp"
namespace espscreen::generated {
inline constexpr board::BoardDescriptor kSelectedBoard{
 1,"hosyond-esp32s3-35-st77922","Hosyond/LCDWiki ES3C35P 3.5-inch","Hosyond/LCDWiki","ES3C35P","vendor-derived",
 {board::McuTarget::esp32s3,16,8,240},
 {board::DisplayDriver::st77922,board::BusKind::qspi,board::BusHost::spi2,320,480,0,40000000,board::ColorOrder::bgr,false,true,40,40,{12,11,-1,10,-1,-1,41,13,14,9}},
 {board::TouchDriver::cst328,board::BusKind::i2c,board::BusHost::i2c0,400000,0,{200,3900,200,3900},{39,38,-1,-1,47,48}},
 {board::StorageDriver::sdmmc,board::BusHost::sdmmc0,"/sd",6,20000000,{5,4,6,-1,7,2,3,-1}},
 {board::AudioDriver::es8311,board::BusHost::i2c0,board::BusHost::i2s0,16000,24,{1,17,18,21,15,16,38,39}},
 {8,2.0f,3200,4200,-1,-1},
 {45,20000,20,100,40,true},
 {board::HeaderConfig{"EXP",board::HeaderKind::gpio,board::BusHost::none,{46,-1,-1,-1,-1,-1,-1,-1},{"IO46 (not motor)","","","","","","",""},1,false,""},board::HeaderConfig{"I2C",board::HeaderKind::i2c,board::BusHost::i2c0,{38,39,-1,-1,-1,-1,-1,-1},{"SDA","SCL","","","","","",""},2,true,""},board::HeaderConfig{},board::HeaderConfig{},board::HeaderConfig{},board::HeaderConfig{},board::HeaderConfig{},board::HeaderConfig{}},2
};
}
