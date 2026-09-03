# Performance and memory

## Rendering

The runtime uses LVGL partial rendering rather than full-frame buffers. Profile-specific line counts balance memory, redraw overhead, and bus throughput. S3 profiles place larger draw buffers in PSRAM when available, while panel transfers use a bounded internal DMA staging buffer when the draw source is not DMA-capable.

Display buses are configured for 40 MHz on classic boards, 60 MHz on the 2.8-inch S3 SPI profile, and 80 MHz on the 3.5-inch S3 QSPI profile. Hardware qualification should reduce a clock when cable, panel, or board revision integrity does not sustain it.

## Allocation policy

- Core registries and event paths are fixed-capacity.
- No exceptions or RTTI.
- Draw-buffer sizes are known from the manifest.
- Service-owned snapshots avoid per-update allocation.
- Optional filesystems and networking may allocate through ESP-IDF; diagnostics track current, minimum, and largest free heap.
- Application start/stop must not show continuous minimum-heap decline during switching tests.

## FreeRTOS

The main task owns LVGL. Background services should block on events and post compact messages. Avoid increasing task count merely to isolate simple periodic work; `IService::tick()` is suitable for bounded low-frequency sampling.

## Tuning sequence

1. Establish stable functionality at a conservative display clock.
2. Measure flush duration and UI responsiveness.
3. Increase draw-buffer lines while observing minimum internal heap under Wi-Fi load.
4. Increase panel clock only after a soak test.
5. Keep transfer staging in internal DMA memory even when PSRAM DMA appears to work on one target.
6. Record target-specific values in the profile, not application code.
