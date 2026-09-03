# EspScreenOS 0.1.0

Initial platform release for the supported ESP32 and ESP32-S3 smart-display families.

## Included

- Capability-first TOML board profiles with deterministic generated build artifacts.
- ESP-IDF/FreeRTOS runtime, LVGL shell, fixed-capacity application and service registries.
- Display, touch, SD, power/battery, settings, assets, diagnostics, Wi-Fi provisioning, audio, and expansion-resource abstractions.
- Framed transport-neutral RPC primitives for UART, USB-hosted, Wi-Fi, and future BLE adapters.
- Dashboard, hardware test bench, and board inspector applications.
- Host tests, profile tests, repository-contract tests, CI board matrix, release packaging, and hardware qualification procedures.

## Qualification status

Profiles are deliberately marked `candidate` until the hardware checklist has been completed on the exact board revision. Candidate status does not prevent local builds, flashing, or profile overrides; it prevents inferred vendor pin maps from being presented as physically proven.
