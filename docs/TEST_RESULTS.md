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
| Hardware backup | Pass | complete 4,194,304-byte image saved outside Git and SHA-256 verified before flashing |
| Flash write/verification | Pass | bootloader, partitions, OTA data, application and web filesystem all passed on-device hash verification on `COM3` |
| First boot | Pass | clean ESP-IDF 5.5.4 boot with no crash or reset loop during a 20-second serial capture |
| OpenThread initialization | Pass | native-radio OpenThread interface attached successfully with host connection mode `none` |
| Wi-Fi/Thread coexistence | Pass | station plus SoftAP mode initialized with software coexistence enabled |
| Setup access point | Pass | unique `ESP-ThreadBR-9EE5` SSID started with DHCP and setup portal at `http://192.168.4.1` |
| Setup portal interaction | Pass | Windows joined the temporary AP, received `192.168.4.2`, and loaded `http://192.168.4.1` with HTTP 200 |
| ESP2THREAD portal branding | Pass | hardware-served title and heading identify ESP2THREAD; inherited M5Stack branding is absent |
| Wi-Fi scan endpoint | Pass | `/scan` returned seven AP records containing only SSID, signal strength and authentication mode fields |
| Complete management web UI image | Pass | all upstream status, network, topology, commissioning, address, tool and about pages are packaged with the branded setup page |
| Home Assistant REST integration | Pending | requires running firmware |
| mDNS discovery | Pending | requires running firmware and Home Assistant |
| Matter-over-Thread commissioning | Pending | requires a supported test device |

The application currently uses 97% of each OTA slot. This is acceptable for
the first hardware experiment but not for a stable release; footprint reduction
is an explicit release requirement.

The complete pre-flash backup is intentionally excluded from Git. A full flash
image may contain credentials and must never be attached to an issue or release.

The setup portal was tested without submitting home Wi-Fi credentials. That
submission must be performed privately by the owner; test output must not print
the password or the resulting Thread operational dataset.
