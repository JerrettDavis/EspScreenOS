"""Board profile validation and deterministic ESP-IDF input generation."""
from __future__ import annotations
import json
from dataclasses import dataclass
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]
VALID_MATURITY={"verified-reference","derived-verified-family","vendor-derived","provisional"}

@dataclass(frozen=True)
class Finding:
 severity:str
 message:str

def board_ids(): return sorted(p.stem for p in (ROOT/"boards").glob("*.json"))
def load_board(board_id):
 b=json.loads((ROOT/"boards"/f"{board_id}.json").read_text())
 b["display"].setdefault("transfer_buffer_lines",20)
 return b
def validate_board(b):
 out=[]
 merged=_merged(b)
 if b.get("id") not in board_ids(): out.append(Finding("error","profile id does not match filename"))
 if b.get("maturity") not in VALID_MATURITY: out.append(Finding("error","invalid maturity"))
 for key in ("hardware_match","source_urls","mcu","display","touch","storage","audio"):
  if not b.get(key): out.append(Finding("error",f"missing {key}"))
 if b.get("display",{}).get("transfer_buffer_lines",20)<=0: out.append(Finding("error","transfer buffer must be nonzero"))
 for header in b.get("headers",[]):
  kind=header.get("kind")
  compatible={"i2c":{"i2c0","i2c1"},"spi":{"spi2","spi3"},"uart":{"uart0","uart1","uart2"}}
  if kind in compatible and header.get("host") not in compatible[kind]:
   out.append(Finding("error",f'{header.get("name","unnamed")} {kind} header requires a compatible host'))
 audio=b.get("audio",{})
 if audio.get("driver") in {"es8311","i2s"} and audio.get("data_host") not in {"i2s0","i2s1"}:
  out.append(Finding("error","digital audio requires an I2S data host"))
 if audio.get("driver")=="es8311" and audio.get("control_host") not in {"i2c0","i2c1"}:
  out.append(Finding("error","ES8311 audio requires an I2C control host"))
 display=merged["display"]
 if display.get("bus") in {"spi","qspi"} and display.get("host") not in {"spi2","spi3"}:
  out.append(Finding("error","SPI display requires a compatible SPI host"))
 touch=merged["touch"]
 if touch.get("bus")=="i2c" and touch.get("host") not in {"i2c0","i2c1"}:
  out.append(Finding("error","I2C touch requires a compatible I2C host"))
 storage=merged["storage"]
 if storage.get("driver")=="sdspi" and storage.get("host") not in {"spi2","spi3"}:
  out.append(Finding("error","SDSPI storage requires a compatible SPI host"))
 if storage.get("driver")=="sdmmc" and storage.get("host")!="sdmmc0":
  out.append(Finding("error","SDMMC storage requires a compatible SDMMC host"))
 return out

def _merged(b):
 s3=b["mcu"]["target"]=="esp32s3"
 defaults={"schema_version":1,"display_name":b["id"],"vendor":"unknown","family":"unknown","maturity":"provisional","headers":[],
  "mcu":{"target":"esp32","flash_mb":4,"psram_mb":0,"cpu_mhz":240},
  "display":{"driver":"st7789","bus":"spi","host":"spi2","width":320,"height":240,"rotation":0,"clock_hz":40000000,"color_order":"bgr","invert":False,"backlight_active_high":True,"draw_buffer_lines":30,"transfer_buffer_lines":20,"pins":{}},
  "touch":{"driver":"none","bus":"none","host":"i2c0","clock_hz":400000,"rotation":0,"calibration":[200,3900,200,3900],"pins":{}},
  "storage":{"driver":"none","host":"sdmmc0" if s3 else "spi3","mount_point":"/sd","max_files":6,"clock_hz":20000000,"pins":{}},
  "audio":{"driver":"none","control_host":"none","data_host":"none","sample_rate":16000,"codec_i2c_address":24,"pins":{}},
  "power":{"battery_adc":-1,"battery_divider":2.0,"battery_empty_mv":3200,"battery_full_mv":4200,"usb_detect":-1,"ambient_light_adc":-1}}
 defaults["haptics"]={"pin":-1,"pwm_frequency_hz":20000,"minimum_running_duty":20,"startup_kick_duty":100,"startup_kick_ms":40,"startup_kick_enabled":True}
 # Known classic-family pin defaults; profiles override any field.
 if not s3:
  defaults["display"]["pins"]={"sclk":14,"mosi":13,"miso":12,"cs":15,"dc":2,"reset":-1,"backlight":27}
  defaults["touch"]["pins"]={"sclk":25,"mosi":32,"miso":39,"cs":33,"irq":36,"reset":-1}
  defaults["storage"]["pins"]={"clk":18,"cmd":23,"d0":19,"cs":5}
  defaults["audio"]["pins"]={"speaker":26}
 def merge(dst,src):
  for k,v in src.items():
   if isinstance(v,dict) and isinstance(dst.get(k),dict): merge(dst[k],v)
   else: dst[k]=v
 merge(defaults,b);return defaults

def _enum(group,value): return f"board::{group}::{value}"
def _pins(p,names): return ",".join(str(p.get(x,-1)) for x in names)
def _cpp(raw):
 b=_merged(raw);d=b["display"];t=b["touch"];s=b["storage"];a=b["audio"];p=b["power"];hc=b["haptics"]
 headers=[]
 for h in b["headers"]:
  pins=(h.get("pins",[])+[-1]*8)[:8];labels=(h.get("labels",[])+[""]*8)[:8]
  host=h.get("host","none")
  headers.append(f'board::HeaderConfig{{"{h["name"]}",board::HeaderKind::{h["kind"]},board::BusHost::{host},{{{",".join(map(str,pins))}}},{{{",".join(json.dumps(x) for x in labels)}}},{len(h.get("pins",[]))},{str(h.get("shared",False)).lower()},"{h.get("note","")}"}}')
 headers += ["board::HeaderConfig{}"]*(8-len(headers))
 return f'''#pragma once
#include "espscreen/board/descriptor.hpp"
namespace espscreen::generated {{
inline constexpr board::BoardDescriptor kSelectedBoard{{
 {b['schema_version']},"{b['id']}","{b['display_name']}","{b['vendor']}","{b['family']}","{b['maturity']}",
 {{{_enum('McuTarget',b['mcu']['target'])},{b['mcu']['flash_mb']},{b['mcu']['psram_mb']},{b['mcu']['cpu_mhz']}}},
 {{{_enum('DisplayDriver',d['driver'])},{_enum('BusKind',d['bus'])},{_enum('BusHost',d['host'])},{d['width']},{d['height']},{d['rotation']},{d['clock_hz']},{_enum('ColorOrder',d['color_order'])},{str(d['invert']).lower()},{str(d['backlight_active_high']).lower()},{d['draw_buffer_lines']},{d['transfer_buffer_lines']},{{{_pins(d['pins'],['sclk','mosi','miso','cs','dc','reset','backlight','data1','data2','data3'])}}}}},
 {{{_enum('TouchDriver',t['driver'])},{_enum('BusKind',t['bus'])},{_enum('BusHost',t['host'])},{t['clock_hz']},{t['rotation']},{{{','.join(map(str,t['calibration']))}}},{{{_pins(t['pins'],['sclk','mosi','miso','cs','irq','reset'])}}}}},
 {{{_enum('StorageDriver',s['driver'])},{_enum('BusHost',s['host'])},"{s['mount_point']}",{s['max_files']},{s['clock_hz']},{{{_pins(s['pins'],['clk','cmd','d0','cs','d1','d2','d3','card_detect'])}}}}},
 {{{_enum('AudioDriver',a['driver'])},{_enum('BusHost',a['control_host'])},{_enum('BusHost',a['data_host'])},{a['sample_rate']},{a['codec_i2c_address']},{{{_pins(a['pins'],['speaker','mclk','bclk','ws','dout','din','sda','scl'])}}}}},
 {{{p['battery_adc']},{p['battery_divider']}f,{p['battery_empty_mv']},{p['battery_full_mv']},{p['usb_detect']},{p['ambient_light_adc']}}},
 {{{hc['pin']},{hc['pwm_frequency_hz']},{hc['minimum_running_duty']},{hc['startup_kick_duty']},{hc['startup_kick_ms']},{str(hc['startup_kick_enabled']).lower()}}},
 {{{','.join(headers)}}},{len(b['headers'])}
}};
}}
'''

def _outputs(board_id):
 b=load_board(board_id);m=_merged(b);flash=m["mcu"]["flash_mb"]
 part="# Name, Type, SubType, Offset, Size, Flags\nnvs,data,nvs,0x9000,0x6000,\notadata,data,ota,0xf000,0x2000,\nphy_init,data,phy,0x11000,0x1000,\nfactory,app,factory,0x20000,0x300000,\nassets,data,spiffs,0x320000,0x0E0000,\n"
 if flash>=16: part=part.replace("0x300000","0x600000").replace("0x320000,0x0E0000","0x620000,0x9E0000")
 sdk=f"CONFIG_IDF_TARGET=\"{m['mcu']['target']}\"\nCONFIG_ESPTOOLPY_FLASHSIZE_{flash}MB=y\nCONFIG_PARTITION_TABLE_CUSTOM=y\nCONFIG_PARTITION_TABLE_CUSTOM_FILENAME=\"generated/{board_id}/partitions.csv\"\n"
 sdk+="CONFIG_ESP_MAIN_TASK_STACK_SIZE=8192\n"
 sdk+="CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ_240=y\nCONFIG_ESP_DEFAULT_CPU_FREQ_MHZ=240\nCONFIG_COMPILER_OPTIMIZATION_PERF=y\n"
 sdk+="CONFIG_FREERTOS_HZ=1000\nCONFIG_ESP_TASK_WDT_TIMEOUT_S=15\n# CONFIG_ESP_TASK_WDT_CHECK_IDLE_TASK_CPU0 is not set\nCONFIG_LV_DEF_REFR_PERIOD=16\n"
 sdk+="CONFIG_LV_FONT_MONTSERRAT_12=y\nCONFIG_LV_FONT_MONTSERRAT_14=y\nCONFIG_LV_FONT_MONTSERRAT_16=y\n"
 if m['mcu']['psram_mb']: sdk+="CONFIG_SPIRAM=y\nCONFIG_SPIRAM_MODE_OCT=y\nCONFIG_SPIRAM_SPEED_80M=y\nCONFIG_SPIRAM_SPEED=80\n"
 return {"include/espscreen/generated/selected_board.hpp":_cpp(b),"sdkconfig.defaults":sdk,"partitions.csv":part,"board.json":json.dumps(m,indent=2,sort_keys=True)+"\n"}

def generate(board_id,check=False):
 findings=validate_board(load_board(board_id));base=ROOT/"generated"/board_id;ok=not any(x.severity=="error" for x in findings)
 for rel,text in _outputs(board_id).items():
  path=base/rel
  if check:
   if not path.exists() or path.read_text()!=text: ok=False;findings.append(Finding("error",f"stale generated file: {rel}"))
  else: path.parent.mkdir(parents=True,exist_ok=True);path.write_text(text)
 return ok,findings
