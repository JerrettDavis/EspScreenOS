# Release checklist

## Portable gate

1. Run `python tools/espscreen.py validate --all`.
2. Run `python tools/espscreen.py generate --all --check`.
3. Run `python tools/espscreen.py test` and confirm both Python and host C++ tests pass.
4. Review profile warnings and source provenance changes.

## Firmware gate

1. Use ESP-IDF 5.4.2 or the provided development container.
2. Build every board in the CI matrix.
3. Archive `build/<board>/flasher_args.json`, binaries, maps, and size reports.
4. Check IRAM/DRAM, flash, task stack high-water marks, and LVGL frame timing.

## Physical gate

1. Execute `docs/hardware-validation.md` on the exact board revision.
2. Capture photos, boot logs, touch calibration, battery-divider measurements, and audio/storage results.
3. Update the profile provenance and qualification evidence.
4. Change profile maturity to `qualified` only after all blocking checks pass.

## Release gate

1. Update `VERSION`, `CHANGELOG.md`, and `RELEASE_NOTES.md`.
2. Create each deterministic firmware ZIP with `python tools/espscreen.py package --board <board-id>`.
3. Verify checksums and install/flash instructions from a clean checkout.
