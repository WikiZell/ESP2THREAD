# Installing and updating firmware

Release downloads never contain NVS, Wi-Fi credentials or a Thread dataset.
Only use firmware published by the WikiZell ESP2THREAD repository.

## First installation

The release `factory.bin` is for a new unit or an intentional complete reset.
Writing it at address `0x0` installs the bootloader, partition table, firmware,
web interface and blank configuration. It is not the update method for a
configured router because it clears NVS.

## Configuration-preserving USB update

The supported update path for this release is USB. It writes the release
components around the NVS partition at `0x9000` and therefore preserves Wi-Fi
and Thread configuration.

1. Download and extract the release bundle.
2. Connect one XIAO ESP32-C6 by USB and identify its serial port.
3. Install Espressif `esptool` 4.12 or use an ESP-IDF 5.5.4 environment.
4. From the extracted bundle, run the command shown in that release's notes.
   It writes only `0x0`, `0x8000`, `0xf000`, `0x20000` and `0x3a0000`.
5. Never erase or write the NVS range `0x9000` through `0xefff` during an
   ordinary update.
6. Wait for the unit's unique `.local` URL and attached Thread role to return
   before updating another router.

Keeping one router online while updating the other preserves a border path for
Thread devices.

## Why web OTA is not enabled yet

Home Assistant requires the standard unauthenticated OTBR REST API on the local
LAN. Adding an unauthenticated firmware upload beside that API would create an
unacceptable takeover path. The dual OTA slots are reserved for a future
physically authorized, signed and rollback-tested updater. Secure boot or eFuse
changes are not required for this release and must not be applied casually.

