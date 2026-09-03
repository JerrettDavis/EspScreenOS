# Supported boards and hardware matching

Use profile IDs, not screen size alone. Several unrelated boards are sold under nearly identical listing titles, and suffixes such as `R`, `C`, and `C_I` change the touch controller and wiring.

## Target matrix

### `hosyond-esp32-35-st7796u`

- Match: ESP32-32E, 3.5-inch 320×480 panel, ST7796U, resistive touch
- Display: SPI2, GPIO 14/13/12/15/2, backlight GPIO 27
- Touch: XPT2046 software SPI, GPIO 25/32/39/33, IRQ 36
- SD: SPI3, GPIO 18/23/19/5
- Audio: PWM speaker GPIO 26
- Analog: ambient GPIO 34, battery GPIO 35
- Common expansion: I2C 21/22; UART0 1/3; shared SD/SPI header

### `hosyond-esp32-40-st7796s`

- Match: ESP32-32E, 4.0-inch 320×480 panel, ST7796S, resistive touch
- Uses the same broad ESP32-3248 electrical family as the 3.5-inch classic profile
- Confirm the backlight pin and PCB revision before first flash

### `hosyond-esp32-32-st7789p3`

- Match: ESP32, 3.2-inch 240×320 panel, ST7789P3-family controller, resistive touch
- Display backlight is GPIO 21 in this profile
- Touch and SD use the classic dedicated pin groups
- Expansion I2C is represented as GPIO 22/27

### `diymalls-esp32-2432s032c-i`

- Match the exact `ESP32-2432S032C_I` marking
- Capacitive FT6336 touch on a dedicated I2C control bus
- Do not flash this profile onto the resistive `R` variant

### `hosyond-esp32s3-28-ili9341`

- Match: ESP32-S3, 2.8-inch 240×320, ILI9341, capacitive FT6336G
- 16 MB flash and 8 MB PSRAM profile
- One-bit SDMMC and ES8311/I2S audio
- Touch and codec share the control I2C bus

### `hosyond-esp32s3-35-st77922`

- Match: ESP32-S3, 3.5-inch 240×320, ST77922 QSPI, capacitive FT6336G
- QSPI data lines and the panel command encoding differ from classic SPI boards
- 16 MB flash, 8 MB PSRAM, one-bit SDMMC, and ES8311/I2S audio

## Confidence labels

- `verified-reference`: imported from a working source project and checked against the family pinout.
- `derived-verified-family`: a close board revision uses a validated electrical family, with a small number of revision-sensitive fields called out.
- `vendor-derived`: profile follows vendor examples or supplied board material and still requires the first-flash checklist on each PCB revision.
- `provisional`: buildable for development, but the CLI blocks flashing unless `--allow-provisional` is supplied.

The profile’s `source_urls`, `source_notes`, and `hardware_match` fields are included in generated `board.json` and release metadata. Keep that file with any distributed binary.

## Safe identification

Before flashing, photograph or record:

1. Front and back PCB markings
2. ESP32 module marking
3. Display controller and touch controller from the vendor example or schematic
4. Flash and PSRAM capacity
5. Screen resolution and physical touch type
6. USB connector and bridge type
7. Board revision, date code, or batch marking

A wrong display profile usually produces a blank or distorted screen. A wrong power, audio, or expansion mapping can be more consequential. Do not connect a battery, speaker, or external powered device until the fixed pin map has passed validation.
