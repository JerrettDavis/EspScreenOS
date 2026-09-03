# EspScreenOS

[![Host validation](https://github.com/JerrettDavis/EspScreenOS/actions/workflows/ci.yml/badge.svg)](https://github.com/JerrettDavis/EspScreenOS/actions/workflows/ci.yml)
[![Firmware matrix](https://github.com/JerrettDavis/EspScreenOS/actions/workflows/firmware.yml/badge.svg)](https://github.com/JerrettDavis/EspScreenOS/actions/workflows/firmware.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

EspScreenOS is a reusable ESP-IDF/FreeRTOS firmware platform for ESP32 display boards. It provides one application framework, one peripheral API, and one validation toolchain across multiple screen sizes, touch controllers, storage arrangements, audio paths, and ESP32 families.

It is not a general-purpose operating system. It is a deterministic embedded platform intended to replace board-specific sketches with generated board support packages and ordinary plug-in applications.

## What is included

- Capability-first TOML board profiles with semantic pin-conflict validation
- Deterministic generation of C++ descriptors, `sdkconfig.defaults`, and partition tables
- ESP-IDF 5.4.x, FreeRTOS, and LVGL 9.2 application runtime
- SPI and QSPI display paths, including a separate internal DMA staging buffer when LVGL draws in PSRAM
- Resistive XPT2046 and capacitive FT6336 touch paths
- SDSPI and one-bit SDMMC storage
- PWM speaker and ES8311/I2S audio services
- Battery voltage, estimated state of charge, USB-detect, ambient-light, heap, PSRAM, and storage telemetry
- NVS settings, SPIFFS assets, Wi-Fi station mode, fallback provisioning AP, and a small health API
- Fixed-capacity event bus, cross-task mailbox, service manager, application manager, resource broker, framed RPC protocol, and expansion-header leases
- A default LVGL shell with Status, Test, and Board applications
- Host tests, manifest tests, firmware build matrix, release packaging, and hardware qualification checklists

## Supported profiles

| Profile | MCU | Display | Touch | SD | Audio |
|---|---|---|---|---|---|
| `hosyond-esp32-35-st7796u` | ESP32-32E | 480×320 ST7796U | XPT2046 | SDSPI | PWM |
| `hosyond-esp32-40-st7796s` | ESP32-32E | 480×320 ST7796S | XPT2046 | SDSPI | PWM |
| `hosyond-esp32-32-st7789p3` | ESP32 | 240×320 ST7789P3 family | XPT2046 | SDSPI | PWM |
| `diymalls-esp32-2432s032c-i` | ESP32 | 240×320 ST7789 family | FT6336 | SDSPI | PWM |
| `hosyond-esp32s3-28-ili9341` | ESP32-S3 | 240×320 ILI9341 | FT6336G | SDMMC | ES8311 |
| `hosyond-esp32s3-35-st77922` | ESP32-S3 | 320×480 ST77922 QSPI | FT6336G | SDMMC | ES8311 |

Board listing titles are not hardware revision identifiers. Match the controller, touch type, resolution, MCU family, PCB marking, and profile `hardware_match` fields before flashing. See [Supported boards](docs/supported-boards.md) and [Hardware validation](docs/hardware-validation.md).

## Quick start

Install and activate ESP-IDF 5.4.x, then run:

```bash
python tools/espscreen.py doctor
python tools/espscreen.py validate --all
python tools/espscreen.py test
python tools/espscreen.py build --board hosyond-esp32-35-st7796u
python tools/espscreen.py flash --board hosyond-esp32-35-st7796u --port COM5
python tools/espscreen.py monitor --board hosyond-esp32-35-st7796u --port COM5
```

On Linux or WSL, the serial port will usually resemble `/dev/ttyUSB0` or `/dev/ttyACM0`.

Every board receives its own build directory under `build/<board-id>`, so switching targets does not leak `sdkconfig` state between ESP32 and ESP32-S3 builds.

## Repository layout

```text
boards/                    Human-edited hardware manifests
components/espscreen_core  Fixed-capacity runtime primitives
components/espscreen_board Board descriptor and resource leases
components/espscreen_hal   Display, touch, SD, power, and audio drivers
components/espscreen_*     Services, UI, apps, protocol, and expansion APIs
generated/                 Deterministic board-specific build input
main/                      Default firmware composition root
tests/                     Host and manifest tests
tools/espscreen.py         Board, build, flash, monitor, test, and package CLI
docs/                      Architecture and development handbook
```

## Application model

Applications implement `IApplication` and know nothing about LCD pins or controller types. Activation receives only an opaque content surface; stable hardware, storage, and job dependencies are injected through narrow consumer-owned ports, and runtime events are typed.

```cpp
class MyApp final : public espscreen::core::IApplication {
public:
    std::string_view id() const override { return "my-app"; }
    std::string_view title() const override { return "My App"; }
    espscreen::core::Status start(espscreen::core::ApplicationSurface surface) override;
    void stop() override;
};
```

Register the app in the firmware application catalog; no board conditionals belong in application code. See [Application development](docs/application-development.md).

## Design rules

1. Display startup is critical. Optional peripherals report degraded health and do not prevent the launcher from booting.
2. Fixed hardware pins are reserved centrally. Applications request named buses, headers, or GPIO leases through the resource broker.
3. LVGL is owned by one task. Background tasks post compact events to the fixed-capacity mailbox; the UI task drains and dispatches them.
4. Board profiles are generated and validated. Hand-written `#ifdef BOARD_X` pin maps are not accepted in application components.
5. Large LVGL draw buffers may live in PSRAM, but panel transfers use DMA-capable internal memory when required.
6. Missing media, battery sensing, audio codecs, or expansion devices are expected runtime states, not boot failures.

## Validation

```bash
python tools/espscreen.py validate --all
python tools/espscreen.py generate --all --check
python tools/espscreen.py test
```

The firmware workflow builds all six targets in an ESP-IDF container. Hardware qualification remains board-revision-specific and is recorded using the checklist in `docs/hardware-validation.md`.

## GPIO45 vibration motor (3.5-inch ESP32-S3)

The `hosyond-esp32s3-35-st77922` profile reserves GPIO45 exclusively as a 20 kHz PWM control signal for an external logic-level N-channel MOSFET. GPIO45 must never power a motor directly. Connect GPIO45 through about 100 ohms to the MOSFET gate, source to common GND, motor negative to drain, and motor positive to 3.3 V. Fit a flyback diode across the motor.

**A roughly 10 kΩ resistor from MOSFET gate to GND is REQUIRED.** Firmware drives GPIO45 LOW before display/UI initialization and writes zero duty before and after LEDC attachment, but software cannot control the earliest power-on/reset interval. GPIO45 is an ESP32-S3 strapping pin; the pulldown keeps the motor off and preserves its LOW strap. GPIO46 is not used for the motor.

The Test application stores only the enable preference and intensity. High, Medium, Low, and Off controls select 100%, 65%, 35%, and zero PWM intensity; running modes have a five-second safety cutoff. Tests and slider previews are bounded pulses, and active motor state is never persisted or restored.

## Documentation

Start with [Architecture](docs/architecture.md), [Build and flash](docs/build-and-flash.md), [Board profiles](docs/board-profiles.md), [Peripheral APIs](docs/peripheral-apis.md), and [Migration from existing projects](docs/migration.md).

## License

MIT. See [LICENSE](LICENSE).
