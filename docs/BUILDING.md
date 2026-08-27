# Building

The pinned toolchain is ESP-IDF v5.5.4.

Prerequisites:

- Git with submodule support
- ESP-IDF v5.5.4 and its installed Python environment
- Seeed Studio XIAO ESP32-C6 connected through native USB

No Wi-Fi password or Thread dataset belongs in `sdkconfig`, a command line,
source code or a committed file.

Clone the project with its pinned upstream source:

```powershell
git clone --recurse-submodules https://github.com/WikiZell/ESP2THREAD.git
cd ESP2THREAD
```

From an ESP-IDF v5.5.4 shell:

```powershell
idf.py set-target esp32c6
idf.py build
```

On Windows, activate the environment first if ESP-IDF is not already in the
current shell:

```powershell
. C:\path\to\esp-idf\export.ps1
idf.py build
```

The build must fit the two 1792 KiB application slots and 4 MB flash before it
is eligible for hardware testing. The hardware-tested footprint configuration
produces a `0x1a8e80`-byte application with `0x17180` bytes (about 94 KiB) free
in each application slot.

The Espressif border-router web and Wi-Fi support currently requires both
`CONFIG_OPENTHREAD_CLI` and `CONFIG_OPENTHREAD_CLI_ESP_EXTENSION`. Disabling
the OpenThread CLI while leaving the extension enabled fails at link time, so
both remain enabled. Unused features that can be disabled independently are
the OpenThread iPerf command, mDNS console commands, SRP client and DNS client.
The web API also selects commissioner and joiner support.

## Flashing safety

Do not erase or flash a board until its complete 4 MB flash has been backed up.
Backups can contain Wi-Fi and Thread credentials, so keep them outside Git and
never attach them to a public issue.

The connected test board uses native USB on `COM3`; ports vary by system. After
the full-flash backup has been verified, flash from an ESP-IDF v5.5.4 shell:

```powershell
idf.py -p COM3 flash
idf.py -p COM3 monitor
```

The first hardware flash was verified on a XIAO ESP32-C6 revision 0.2. The
bootloader, partition table, OTA metadata, application and web filesystem all
passed esptool's on-device hash verification. The first boot then completed
without a crash or reset loop and exposed the setup portal at
`http://192.168.4.1`.

To restore the board, write the private 4 MB backup at offset `0x0` using the
same detected 4 MB flash size. Keep the backup path out of shared command logs,
documentation and issue reports.

## Upstream source

The ESP Thread Border Router SDK is pinned under
`third_party/esp-thread-br`. Update it only in a dedicated change followed by a
clean firmware build and the complete Home Assistant REST compatibility tests.
