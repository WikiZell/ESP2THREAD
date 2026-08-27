# Test results

## 2026-08-27: firmware foundation

Environment:

- Seeed Studio XIAO ESP32-C6 target
- ESP32-C6FH4 revision 0.2
- detected 4 MB flash
- ESP-IDF v5.5.4
- Espressif ESP Thread Border Router SDK commit `cd0b64f`

Results:

| Test | Result | Evidence |
| --- | --- | --- |
| Repository documentation tests | Pass | 6 tests passed locally before the firmware build |
| Dependency resolution | Pass | ESP-IDF resolved and locked nine dependencies |
| Native ESP32-C6 build | Pass | `esp2thread.bin` generated successfully |
| Partition layout | Pass | final partition ends at `0x400000` on 4 MB flash |
| Application fit | Pass with warning | image `0x1b3540`, 1792 KiB slot, `0xcac0` bytes free |
| Web filesystem fit | Pass | 384 KiB SPIFFS image generated |
| Hardware backup | Pending | required before first flash |
| First boot and setup access point | Pending | board not flashed yet |
| Home Assistant REST integration | Pending | requires running firmware |
| mDNS discovery | Pending | requires running firmware and Home Assistant |
| Matter-over-Thread commissioning | Pending | requires a supported test device |

The application currently uses 97% of each OTA slot. This is acceptable for
the first hardware experiment but not for a stable release; footprint reduction
is an explicit release requirement.

