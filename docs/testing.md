# Testing strategy

## Host tests

`tests/host` compiles board-independent C++ with the desktop compiler and warnings as errors. It covers:

- Fixed-capacity vector behavior
- Event subscriptions and safe self-unsubscribe
- Cross-task mailbox ordering and capacity
- Shared resource leases with per-owner re-entrant counts
- Optional service degradation and critical rollback
- Application switching and rollback
- Compile-time application-context type contracts
- COBS, frame CRC, corruption rejection, and RPC routing
- Expansion header lease mapping
- Display idle transitions, preference bounds, wake behavior, and tick wraparound
- Shell navigation transitions and single-active-surface invariants

## Python tests

`tests/python` validates all board manifests, deterministic generation, flash bounds, profile identity, source provenance, PSRAM/SDMMC expectations, and source-level regression guards for DMA staging and resource ownership.

## Firmware builds

The GitHub firmware workflow builds every profile in a pinned ESP-IDF container. Each matrix entry receives its own generated descriptor and target-specific `sdkconfig`.

## Hardware tests

Software tests cannot prove a marketplace board revision’s electrical mapping. The qualification checklist verifies panel, touch, SD, audio, power, headers, soak behavior, and memory stability. Promote maturity only after recording evidence against a specific PCB revision.

## Commands

```bash
python tools/espscreen.py validate --all
python tools/espscreen.py generate --all --check
python tools/espscreen.py test
```

For a complete local firmware matrix with ESP-IDF active:

```bash
for board in $(python tools/espscreen.py list-boards | tail -n +3 | awk '{print $1}'); do
  python tools/espscreen.py build --board "$board"
done
```
