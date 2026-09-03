# Build and flash

## Prerequisites

- ESP-IDF 5.4.x activated in the current shell
- Python 3.10 or newer; Python 3.11 or newer avoids the `tomli` fallback
- CMake, Ninja, Git, and a supported USB serial driver

The repository pins its tested ESP-IDF line in `idf-version.txt` and LVGL in `main/idf_component.yml`.

## Windows PowerShell

Open an ESP-IDF PowerShell or run the installation’s `export.ps1`, then:

```powershell
python .\tools\espscreen.py doctor
python .\tools\espscreen.py test
python .\tools\espscreen.py build --board hosyond-esp32-35-st7796u
python .\tools\espscreen.py flash --board hosyond-esp32-35-st7796u --port COM5
python .\tools\espscreen.py monitor --board hosyond-esp32-35-st7796u --port COM5
```

`tools/bootstrap.ps1` runs the doctor, manifest validation, and generation steps after ESP-IDF is active.

## Linux, macOS, or WSL

```bash
. "$IDF_PATH/export.sh"
./tools/bootstrap.sh
python3 tools/espscreen.py build --board hosyond-esp32s3-35-st77922
python3 tools/espscreen.py flash --board hosyond-esp32s3-35-st77922 --port /dev/ttyACM0
```

For the Hosyond/LCDWiki ES3C35P 3.5-inch capacitive board, build all default
apps with:

```bash
python tools/espscreen.py build --board hosyond-esp32s3-35-st77922
```

Or omit apps from the firmware image at compile time:

```bash
python tools/espscreen.py build --board hosyond-esp32s3-35-st77922 \
  --apps dashboard,calculator,gpio_toolbox,recorder,paint,notes
```

The same options are available in `idf.py menuconfig` under
`EspScreenOS applications`. SD Lua app support is independently controlled by
`CONFIG_ESPSCREEN_SD_APPS`.

For Linux serial permission errors, add the user to the distribution’s serial group, then log out and back in. WSL USB access depends on the Windows USB forwarding setup.

## Board-specific build directories

The CLI executes the equivalent of:

```bash
idf.py -B build/<board> -D ESPSCREEN_BOARD=<board> set-target <esp32-or-esp32s3>
idf.py -B build/<board> -D ESPSCREEN_BOARD=<board> build
```

The target marker causes the build directory to be discarded when the MCU target changes. This prevents stale ESP32 Kconfig values from entering an ESP32-S3 build.

## Packaging binaries

After a successful build:

```bash
python tools/espscreen.py package --board hosyond-esp32-35-st7796u
```

The package contains the bootloader, partition table, OTA data, application image,
flasher arguments, generated board descriptor, release and license metadata,
these instructions, and a SHA-256 checksum file.
Keep `generated/<board>/board.json` with any binary distributed outside the source tree.

## Recovery

If automatic reset does not enter the bootloader:

1. Hold BOOT.
2. Tap RESET or reconnect USB.
3. Start the flash command.
4. Release BOOT when writing begins.

Use `idf.py erase-flash` only after preserving any required NVS data.
