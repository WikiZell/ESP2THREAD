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
is eligible for hardware testing. The first build fits, but only has about
50 KiB of application-slot headroom. Reducing that footprint is tracked before
stable release.

## Flashing safety

Do not erase or flash a board until its complete 4 MB flash has been backed up.
Backups can contain Wi-Fi and Thread credentials, so keep them outside Git and
never attach them to a public issue.

The connected test board uses native USB on `COM3`; ports vary by system.
Hardware-tested flash commands will be added after the backup and first-boot
recovery path are verified.

## Upstream source

The ESP Thread Border Router SDK is pinned under
`third_party/esp-thread-br`. Update it only in a dedicated change followed by a
clean firmware build and the complete Home Assistant REST compatibility tests.
