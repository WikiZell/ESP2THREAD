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
| Repository documentation tests | Pass | 7 tests passed locally after the branded portal change |
| Dependency resolution | Pass | ESP-IDF resolved and locked nine dependencies |
| Native ESP32-C6 build | Pass | `esp2thread.bin` generated successfully |
| Partition layout | Pass | final partition ends at `0x400000` on 4 MB flash |
| Application fit | Pass with warning | LED-enabled image `0x1b4300`, 1792 KiB slot, `0xbd00` bytes free |
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
| Home Wi-Fi provisioning | Pass | credentials saved privately; device obtained DHCP address and became reachable as `esp2thread.local` |
| REST compatibility | Pass | node, state, network, border-agent, diagnostics and active-dataset routes returned HTTP 200; dataset content stayed redacted in test output |
| Thread network creation | Pass | first unit created a random active dataset and formed a single-router partition as `leader` |
| Reboot persistence | Pass | saved Wi-Fi and Thread state survived a controlled reboot; REST became reachable after 11 seconds and Thread returned to `leader` after 44 seconds |
| MeshCoP mDNS advertisement | Pass | `_meshcop._udp.local` advertised `esp2thread`, the LAN addresses, Border Agent port and standard TXT fields |
| Home Assistant REST integration | Pass | Home Assistant 2026.8.3 loaded the `otbr` entry using the local REST URL |
| Home Assistant Thread import | Pass | Thread integration loaded one dataset sourced from `otbr` and marked it preferred; key material was not printed |
| Automatic Home Assistant offer | Inconclusive | standard mDNS was visible, but automatic OTBR entry creation was not observed; manual URL setup passed |
| Onboard status LED | Pass | GPIO15 active-low driver reported Wi-Fi disconnected at boot, Thread forming after IP at 17 seconds, and border-router ready at 53 seconds; REST remained `leader` and both Home Assistant integrations stayed loaded |
| Matter-over-Thread commissioning | Pending | requires a supported test device |

The application currently uses 97% of each OTA slot. This is acceptable for
the first hardware experiment but not for a stable release; footprint reduction
is an explicit release requirement.

The complete pre-flash backup is intentionally excluded from Git. A full flash
image may contain credentials and must never be attached to an issue or release.

The home Wi-Fi password was submitted privately through the local portal and
was never printed or committed. Thread dataset retrieval was verified only by
HTTP status, size and Home Assistant metadata; network keys and PSKc remained
redacted.

The test board initially measured approximately -90 dBm Wi-Fi signal and needed
one reconnect attempt after reboot. It subsequently reached the REST API and
held `leader` state. Better placement and longer coexistence testing remain
required before release.
