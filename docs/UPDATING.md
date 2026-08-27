# Flashing and updating ESP2THREAD

This guide applies to the Seeed Studio XIAO ESP32-C6 with 4 MB flash. Release
downloads never contain NVS, Wi-Fi credentials or a Thread dataset. Only use
firmware from the official [ESP2THREAD releases](https://github.com/WikiZell/ESP2THREAD/releases).

## Before flashing

You need:

- a data-capable USB-C cable;
- Python 3 installed on Windows, macOS or Linux;
- `esptool` 4.12.0;
- the serial port assigned to the XIAO ESP32-C6.

Install the tested esptool version:

```text
python -m pip install "esptool==4.12.0"
```

On Windows, `py` can replace `python` if that is how Python is installed. Find
the port in Device Manager under **Ports (COM & LPT)**, for example `COM3`. On
Linux it is commonly `/dev/ttyACM0`; on macOS it is commonly a
`/dev/cu.usbmodem...` device.

If esptool reports that it cannot connect:

1. Disconnect USB power.
2. Hold the XIAO **BOOT** button.
3. Reconnect USB while continuing to hold **BOOT**.
4. Release **BOOT** after the computer detects the serial device.
5. Run the flash command again.

This is download mode, not the firmware's eight-second factory-reset action.

## Verify the download

Download `SHA256SUMS.txt` with the required firmware files. On Linux, verify
all files in the download folder with:

```text
sha256sum -c SHA256SUMS.txt
```

On macOS use:

```text
shasum -a 256 -c SHA256SUMS.txt
```

On Windows PowerShell, this example verifies the factory image and returns
`True` only when it matches:

```powershell
$line = (Select-String -Path .\SHA256SUMS.txt -Pattern ' ESP2THREAD-factory\.bin$').Line
$expected = ($line -split '\s+')[0].ToLower()
$actual = (Get-FileHash .\ESP2THREAD-factory.bin -Algorithm SHA256).Hash.ToLower()
$actual -eq $expected
```

Stop if a checksum does not match. Download that file again from the official
release.

## Fresh installation or complete reset

Use this method for a new board or when intentionally removing all existing
configuration. Download:

- `ESP2THREAD-factory.bin`
- `SHA256SUMS.txt`

The factory image installs the bootloader, partition table, application and web
interface. It also clears saved Wi-Fi and Thread configuration. From the
download folder, replace `COM3` with the actual port and run this single-line
command:

```text
python -m esptool --chip esp32c6 --port COM3 --baud 460800 write_flash --flash_mode dio --flash_freq 80m --flash_size 4MB 0x0 ESP2THREAD-factory.bin
```

After esptool reports **Hash of data verified**, disconnect and reconnect USB
or press reset. The onboard LED gives one short pulse every two seconds and the
unique `ESP-ThreadBR-XXXX` Wi-Fi setup network appears. Continue with the
[setup guide](SETUP.md).

## Update while preserving configuration

Use this method for an ESP2THREAD router already connected to Wi-Fi and a
Thread network. Do not use the factory image: it would clear NVS and remove the
saved Wi-Fi credentials and active Thread dataset.

Download these release assets into one empty folder:

- `bootloader.bin`
- `partition-table.bin`
- `ota-data-initial.bin`
- `esp2thread.bin`
- `web-storage.bin`
- `SHA256SUMS.txt`

Verify them, replace `COM3` with the actual serial port, then run:

```text
python -m esptool --chip esp32c6 --port COM3 --baud 460800 write_flash --flash_mode dio --flash_freq 80m --flash_size 4MB 0x0 bootloader.bin 0x8000 partition-table.bin 0xf000 ota-data-initial.bin 0x20000 esp2thread.bin 0x3a0000 web-storage.bin
```

This command writes around the NVS partition at `0x9000` through `0xefff`.
Never add `erase_flash`, never write the factory image at `0x0`, and never write
anything into that NVS range during a configuration-preserving update.

For two or more routers, update only one at a time:

1. Keep the other router powered and attached.
2. Flash the first router with the preserving command.
3. Wait for its solid LED and open
   `http://esp2thread-xxxxxx.local/node/state`.
4. Continue only when it reports `leader`, `router` or `child`.
5. Repeat for the next router.

This keeps one border path available for Thread devices. A full-flash backup of
a configured unit can contain Wi-Fi and Thread secrets; keep any backup private
and never upload it to GitHub or an issue report.

## Verify after flashing

1. Open `http://esp2thread-xxxxxx.local/about.html` and confirm the expected
   application version.
2. Confirm `/node/state` reports `leader`, `router` or `child`.
3. Confirm Home Assistant still shows the OpenThread Border Router entry as
   loaded.
4. Test one paired Matter device from Home Assistant.
5. With multiple routers, verify every router returned before disconnecting the
   USB cables or changing their placement.

No Home Assistant restart is required.

## Why web OTA is not enabled

Home Assistant requires the standard unauthenticated OTBR REST API on the local
LAN. Adding an unauthenticated firmware upload beside that API would create an
unacceptable takeover path. The dual OTA slots are reserved for a future
physically authorized, signed and rollback-tested updater. Secure boot or eFuse
changes are not required for this release and must not be applied casually.
