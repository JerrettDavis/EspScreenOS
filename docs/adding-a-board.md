# Adding a board

## 1. Collect evidence

Obtain the schematic, vendor example, or a known working source project. Record the exact PCB revision. Identify every fixed output before powering external headers: panel bus, backlight polarity, touch reset, SD mode, speaker path, codec control bus, battery divider, and any enable rails.

## 2. Copy the closest electrical family

```bash
cp boards/hosyond-esp32-35-st7796u.toml boards/my-board.toml
```

Change `id`, identity fields, maturity, evidence, MCU geometry, and every capability section. Do not preserve a pin merely because two boards have the same screen size.

## 3. Validate and generate

```bash
python tools/espscreen.py validate --board my-board
python tools/espscreen.py generate --board my-board
```

Fix errors rather than suppressing them. Warnings about strapping pins may be legitimate fixed board wiring, but must remain visible in review.

## 4. Add a driver only when the controller is new

A new pin arrangement does not require a driver. A new panel, touch controller, storage mode, codec, or power monitor requires:

1. Add an enum value in `descriptor.hpp`.
2. Add generator mapping through the enum name.
3. Implement a HAL branch behind the existing device interface.
4. Keep optional-probe failure nonfatal unless the display itself cannot operate.
5. Add host tests for configuration and isolated driver tests where possible.
6. Add a hardware qualification record.

## 5. Build in isolation

```bash
python tools/espscreen.py build --board my-board
```

Every board has a separate build directory. Never reuse a manually configured global `sdkconfig` when qualifying a target.

## 6. First-flash qualification

Start with USB only. Complete `docs/hardware-validation.md`, then update maturity and source notes. Commit the profile, generated output, and qualification record together.

## 7. Add CI

Add the profile ID to the firmware workflow matrix and `EXPECTED` in `tests/python/test_board_profiles.py`. The exact supported matrix is intentional; an untested profile should not appear silently.
