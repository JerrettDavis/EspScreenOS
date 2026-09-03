# Board profiles

Board profiles are TOML files under `boards/`. They are the only hand-edited source of hardware truth. Generated files are committed so changes can be reviewed and CI can detect stale output.

## Major sections

```toml
schema_version = 1
id = "example-board"
display_name = "Human-readable board name"
vendor = "Vendor"
family = "electrical-family"
maturity = "vendor-derived"
hardware_match = ["silkscreen", "resolution", "controller"]
source_urls = ["source repository or vendor page"]
source_notes = "Revision-specific qualification notes"

[mcu]
target = "esp32s3"
flash_mb = 16
psram_mb = 8
cpu_mhz = 240

[display]
driver = "st77922"
bus = "qspi"
host = "spi2"
width = 240
height = 320
rotation = 0
clock_hz = 80000000
color_order = "rgb"
invert = false
backlight_active_high = true
draw_buffer_lines = 96
transfer_buffer_lines = 16
```

Display, touch, storage, and audio sections each include a `pins` table. A pin value of `-1` means not connected or not controllable. Headers are metadata aliases and do not reserve a GPIO by themselves.

## Semantic validation

`tools/boardlib.py` checks:

- Required sections and supported enum values
- MCU GPIO range
- Classic ESP32 input-only output misuse
- Duplicate fixed-pin assignments unless an explicit sharing group matches
- QSPI data-line completeness
- Header label and pin counts
- Flash-size support and generated partition bounds
- ADC2/Wi-Fi and boot-strapping warnings on classic ESP32
- Profile identity, file-name agreement, maturity, and provenance

Run:

```bash
python tools/espscreen.py validate --board example-board
python tools/espscreen.py generate --board example-board
python tools/espscreen.py generate --board example-board --check
```

## Shared buses

A `share_group` explains intentional electrical sharing in the manifest. For example, FT6336 touch and ES8311 audio control may both use `control_i2c`. Sharing does not imply that arbitrary GPIO manipulation is safe. The runtime exposes a shared bus lease while retaining exclusive ownership of the physical pins as bus pins.

## Memory tuning

`draw_buffer_lines` controls LVGL’s partial render buffer. `transfer_buffer_lines` controls the internal DMA staging chunk. Increase the former on PSRAM-equipped boards for fewer render iterations. Keep the latter small enough for reliable internal-memory allocation under Wi-Fi load.

For a 240-pixel-wide RGB565 panel, 16 transfer lines require 7,680 bytes. For a 480-pixel-wide panel, 12 lines require 11,520 bytes.

## Generated output

Each profile produces:

- `selected_board.hpp`: constexpr C++ descriptor
- `sdkconfig.defaults`: target flash, PSRAM, and partition settings
- `partitions.csv`: board-size-specific OTA and asset layout
- `board.json`: canonical machine-readable release metadata
- `manifest.sha256`: deterministic generation digest

Never edit generated output directly.
