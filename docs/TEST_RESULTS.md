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
| Production footprint configuration | Pass | unused iPerf, mDNS-console, SRP-client and DNS-client features disabled; image reduced from `0x1b4300` to `0x1a8e80` and OTA-slot headroom increased from `0xbd00` to `0x17180` bytes |
| Footprint firmware hardware regression | Pass | flash hashes verified on `COM3` without writing the NVS partition; saved Wi-Fi identity returned immediately and Thread progressed from `detached` to `leader` after 15 seconds of polling |
| Home Assistant regression after footprint build | Pass | Home Assistant 2026.8.3 continued to report the OpenThread Border Router, Thread and Matter config entries as `loaded` |
| Explicit Create/Join foundation | Pass | a fresh-board code path no longer calls automatic dataset creation; the hardware-served setup page offers Create and full-dataset Join, with dataset import in an HTTP body rather than a URL |
| Existing-network upgrade path | Pass | Create/Join firmware flashed without writing NVS, preserved the commissioned dataset and returned the test unit to `leader`; the normal root URL remained on the dashboard |
| Setup-page browser QA | Pass | hardware-served page identity, meaningful DOM, Create/Join controls, invalid-dataset rejection and desktop layout passed; no browser console warning or error was observed |
| Multi-unit identity build | Pass | base-MAC-derived mDNS instance, hostname and station-netif hostname compile in the production image |
| Unique-name hardware regression | Pass | existing unit preserved NVS, resolved at `esp2thread-8f9ee4.local`, served HTTP 200 and returned from `detached` to `leader` |
| Unique-name browser QA | Pass | dashboard and configured-router setup page loaded through the new `.local` name with zero browser warnings or errors; dataset-copy control was not activated |
| Factory-reset safety path | Partial pass | normal boot with GPIO9 released preserved Wi-Fi and Thread state; destructive eight-second erase and short-press cancellation intentionally remain untested on the commissioned unit |
| Home Assistant identity regression | Pass | OpenThread Border Router, Thread and Matter config entries all remained `loaded` after the hostname firmware update |
| Second-unit hardware identification | Pass | COM4 is a second XIAO ESP32-C6FH4 revision 0.2 with 4 MB flash and base-MAC suffix `8e88ec`; its original 4 MB flash was privately backed up and SHA-256 verified before installation |
| Fresh second-unit provisioning | Pass | clean NVS boot produced unique `ESP-ThreadBR-88ED`, branded setup portal at `192.168.4.1`, one-pulse LED status and stable LAN hostname `esp2thread-8e88ec.local` |
| Additional-router dataset join | Pass | dataset moved directly in memory from `esp2thread-8f9ee4` to `esp2thread-8e88ec` without log, URL or file exposure; exact dataset equality was checked privately and the second unit attached as `child` |
| Two-router Home Assistant integration | Pass | Home Assistant 2026.8.3 reports two OpenThread Border Router entries, Thread and Matter as `loaded` |
| Preferred-network UI validation | Pass | Home Assistant Thread page shows one preferred `ESP-BR-9EE4` network containing both uniquely named ESP2THREAD border routers; no dataset or key material was captured |
| Bidirectional border-router failover | Pass | with COM3 offline, COM4 progressed through `detached` to `leader`; with COM4 offline, COM3 remained attached and became `leader` after about 50 seconds; both restored as `leader` plus `child` |
| Release-candidate automated gates | Pass | 13 repository/security/documentation tests, link checks, whitespace checks and full ESP-IDF 5.5.4 build pass for version `0.1.0-rc.1` |
| Sequential release-candidate update | Pass | COM3 and COM4 were updated one at a time without writing NVS; both preserved Wi-Fi and dataset, report version `0.1.0-rc.1`, and restored as `leader` plus `router` |
| Release-candidate browser QA | Pass | branded About page, embedded version, redacted-diagnostics control and dashboard without displayed PSKc passed; zero browser warning/error messages |
| Matter device commissioning | Pass (user confirmed) | end devices paired successfully through Home Assistant after the preferred two-router Thread network was synchronized; exact product models and setup codes were not recorded |
| Stable `v0.1.0` hardware regression | Pass | full ESP-IDF build and sequential verified flashes completed without writing NVS; both routers report `0.1.0` and restored as `leader` plus `router` |
| Published flashing-guide validation | Pass | all `v0.1.0` release asset checksums passed; documented component offsets reconstructed the exact published 4 MB factory image with esptool 4.12.0 without accessing either router |

The application currently uses about 95% of each OTA slot. After multi-unit
identity and recovery support, `esp2thread.bin` is `0x1a9b10` bytes and leaves
`0x164f0` bytes (about 89 KiB) in each 1792 KiB OTA slot. New features must
continue to pass the partition-size gate.

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
