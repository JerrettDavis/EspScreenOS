# Troubleshooting

## Unknown board

Run `python tools/espscreen.py list-boards` and pass the exact profile ID. A product title is not accepted as a target.

## ESP-IDF is not active

`doctor` reports missing `idf.py` or `IDF_PATH`. Run the ESP-IDF installation’s export script in the current shell.

## Blank display with backlight

Verify controller, resolution, CS/DC/reset pins, color bus type, and panel clock. A QSPI ST77922 profile cannot drive a classic SPI ST7796 panel. Reduce `clock_hz` while qualifying signal integrity.

## Correct image, wrong colors

Change `color_order` between `rgb` and `bgr`, then regenerate. Do not compensate in application styles.

## Image mirrored or clipped

Correct profile rotation and panel geometry. Touch rotation must be checked independently at all four corners.

## Touch does not respond

Confirm resistive versus capacitive hardware. For XPT2046, verify IRQ and the dedicated software-SPI pins. For FT6336, verify address `0x38`, reset, and shared I2C control pins.

## SD card absent

Absence is nonfatal. Use a known FAT32 card, verify the profile chooses SDSPI or SDMMC correctly, and inspect the health screen. A shared SPI expansion device must use a unique chip select.

## Audio unavailable

Classic boards need the correct speaker/header GPIO and any onboard amplifier enable. S3 boards need ES8311 control I2C, I2S clocks, and the board’s actual speaker path. Begin at low volume.

## DMA allocation failure

Reduce `transfer_buffer_lines` first, then `draw_buffer_lines`. Internal DMA staging competes with Wi-Fi and other driver allocations even when large PSRAM remains free.

## Build switches MCU unexpectedly

Use the CLI instead of invoking a shared `idf.py build` directory. `build/<board>` contains a target marker and is isolated per profile.

## Provisioning AP

When no Wi-Fi credentials exist, connect to `EspScreen-XXXX` and open `192.168.4.1`. The base image does not require network access for local applications.
