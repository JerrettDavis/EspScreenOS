# Peripheral APIs

## Display

`DisplayDevice` owns only panel, bus, backlight PWM, transfer synchronization, and DMA-staging mechanics. It exposes typed pixel-transfer and completion operations without depending on LVGL. The runtime-owned `FirmwareLvglDisplay` creates the LVGL display, allocates partial draw buffers according to the declarative buffer plan, translates invalidation and flush callbacks, and preserves asynchronous direct-DMA transfers. Applications normally interact with LVGL only. Diagnostics and power policy may call `set_backlight(0..100)`.

## Touch

`TouchDevice` exposes calibrated hardware samples through `read(TouchPoint&)` and has no UI-framework dependency. Resistive profiles store four calibration endpoints. Capacitive profiles read controller coordinates and apply board rotation. At the runtime edge, `FirmwareShellInput` owns LVGL pointer registration and delegates release debounce, re-press suppression, clock-wrap-safe timing, and retained coordinates to the framework-independent `TouchInputFilter`.

Calibration, rotation, and display-bound clamping are pure `map_touch_axis` and `transform_touch_point` operations. Controller drivers decode raw coordinates and delegate geometry, so reversed resistive calibration endpoints and all four rotations are host-testable without ESP-IDF or LVGL.

A future calibration application can persist per-board overrides in NVS without changing the manifest’s factory defaults.

## Storage

`StorageDevice` mounts removable media at `/sd` through SDSPI or one-bit SDMMC. `stats()` reports mount state and capacity. Absence at boot is nonfatal. Code must not assume an open file remains valid after media removal.

Mount state and retry eligibility live in the framework-independent `StorageMountPolicy`. Failed mounts are retried after a deterministic interval using wrap-safe elapsed-time arithmetic; the device adapter is responsible only for ESP-IDF bus and filesystem resources.

## Audio

`AudioDevice` supports:

- PWM tone generation on classic boards
- ES8311 codec initialization and stereo 16-bit I2S output on S3 boards
- `tone()` for feedback and diagnostics
- `write_pcm()` for bounded streaming from an application or service

Codec or speaker failure is optional and appears in diagnostics.

Volume composition, PWM duty mapping, tone length, and stereo sine generation are pure operations in `audio_signal`. Hardware adapters schedule playback and translate generated PCM into LEDC or I2S calls, keeping signal behavior host-testable and reusable by future platforms.

Haptic requests are normalized into a pure `HapticsPulsePlan` containing stop semantics, bounded intensity and duration, running duty, and optional startup kick. The FreeRTOS worker interprets that plan and owns cancellation mechanics; it does not duplicate policy state.

## Power

`PowerDevice::sample()` returns:

- Battery millivolts after the configured divider
- Linear estimated percentage between profile empty/full thresholds
- USB-detect state when wired
- Ambient ADC raw value when wired
- Sample timestamp

The percentage is intentionally a simple estimate. A fuel-gauge expansion service should replace it when accurate state of charge matters.

`PowerDevice` owns ADC and GPIO acquisition. Battery voltage conversion and bounded percentage calculation live in the pure `measure_battery` policy, including invalid calibration handling. Sampling averages only successful ADC reads; unavailable battery or ambient samples retain the public `-1` sentinel instead of converting stale data.

## Expansion headers

`BoardDescriptor::headers` describes names, kinds, labels, sharing, and notes. `ExpansionManager` maps a named header to a resource lease.

```cpp
espscreen::expansion::ExpansionManager expansion(
    hardware.descriptor(), hardware.resources());

constexpr espscreen::board::OwnerId owner{0x53454E53};
auto status = expansion.acquire_header("I2C", owner);
```

A shared lease permits multiple well-behaved devices on a bus. It does not resolve address collisions, incompatible clock limits, missing level shifting, or excessive load.

At platform startup, `fixed_resource_plan()` projects the selected board descriptor into a deduplicated set of GPIO and bus reservations. Resource topology therefore remains declarative and host-testable; `HardwarePlatform` only applies the resulting plan to the broker.

## UART

UART0 is commonly shared with USB boot logs on classic boards. Use it only when the host can tolerate logs and bootloader traffic, or add a board profile exposing another UART. Remote protocols should use the common COBS/CRC frame format so USB CDC, UART, BLE, and TCP transports remain interoperable.
