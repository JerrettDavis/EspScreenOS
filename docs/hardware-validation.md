# Hardware validation and qualification

Create one completed copy of this checklist per PCB revision. Keep USB power only until the fixed pin map passes the early stages.

## Record

- Profile ID:
- PCB marking and revision:
- Purchase/source:
- MCU module marking:
- Flash and PSRAM observed:
- Display controller evidence:
- Touch controller evidence:
- Tester and date:
- Firmware commit:
- Generated manifest SHA-256:

## Stage 1: static review

- [ ] Controller, resolution, touch type, and MCU match `hardware_match`
- [ ] Output roles do not use classic ESP32 input-only pins
- [ ] Shared pins have an explicit electrical reason
- [ ] Backlight and enable polarities are sourced
- [ ] Battery divider and maximum input are known
- [ ] Speaker path is known to be direct, amplified, or codec-driven
- [ ] External headers have voltage and ground orientation documented
- [ ] `python tools/espscreen.py validate --board <id>` has no errors

## Stage 2: USB-only boot

- [ ] Bootloader connects without forcing unusual strapping levels
- [ ] Serial log prints the expected profile ID
- [ ] Reported flash and PSRAM match the profile
- [ ] Panel resets and displays the launcher
- [ ] Colors are correct; red and blue are not exchanged
- [ ] Rotation, dimensions, and clipping are correct
- [ ] Backlight 25%, 100%, and off behave correctly
- [ ] Reboot and OTA partition selection remain valid

## Stage 3: touch

- [ ] Press/release is stable
- [ ] All four corners map inside the visible panel
- [ ] X and Y are not swapped or mirrored
- [ ] Resistive touch remains stable while the display is transferring
- [ ] Capacitive reset and interrupt behavior survive warm reboot

## Stage 4: removable storage

- [ ] Boot succeeds with no card
- [ ] Known FAT32 card mounts
- [ ] Capacity and free space are plausible
- [ ] Read/write test passes
- [ ] UI remains responsive during transfer
- [ ] Card failure produces degraded health rather than reboot

## Stage 5: audio

- [ ] Low-volume test tone is audible
- [ ] No sustained DC or excessive heat is observed
- [ ] ES8311 control and I2S clocks are present when applicable
- [ ] Audio failure does not block the launcher

## Stage 6: power and headers

- [ ] Battery ADC is checked against a multimeter at two voltages
- [ ] Divider ratio is corrected in the profile if needed
- [ ] USB-detect polarity is correct when available
- [ ] I2C header detects a known device through a shared lease
- [ ] UART loopback or host exchange passes
- [ ] SPI header uses a separate chip select and coexists with SD
- [ ] Exposed GPIO direction and voltage limits are documented

## Stage 7: soak and performance

- [ ] One-hour UI/network/SD soak without watchdog resets
- [ ] Minimum free heap stabilizes
- [ ] No DMA allocation failure under Wi-Fi load
- [ ] Repeated app switching does not reduce free heap continuously
- [ ] Touch remains responsive during full-screen redraws
- [ ] Battery operation and charge behavior are safe for the actual board

## Qualification result

- [ ] Pass, profile may be promoted
- [ ] Conditional pass, limitations recorded in `source_notes`
- [ ] Fail, profile remains provisional and flashing stays blocked
