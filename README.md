# ESP2THREAD

ESP2THREAD turns a Seeed Studio XIAO ESP32-C6 into a Wi-Fi-connected
OpenThread Border Router for Home Assistant. The intended experience is a
small network appliance: provision Wi-Fi, create the first Thread network,
add the router to Home Assistant, then copy the same Thread dataset to
additional ESP2THREAD units.

Each unit derives a unique local name from its factory MAC address, such as
`esp2thread-8f9ee4.local`, so several routers can share one LAN without name
collisions.

> [!IMPORTANT]
> The ESP32-C6 has one shared 2.4 GHz RF path. Wi-Fi and IEEE 802.15.4 cannot
> receive simultaneously. Espressif supports this single-chip configuration,
> but recommends a separate Wi-Fi host and Thread radio for production-grade
> performance. ESP2THREAD will measure this limitation during hardware tests
> and will not describe the single-chip build as equivalent to a two-radio
> commercial border router.

## Target hardware

- Seeed Studio XIAO ESP32-C6
- ESP32-C6FH4, revision 0.2 tested
- 4 MB flash
- Native USB Serial/JTAG
- 2.4 GHz Wi-Fi 6 and IEEE 802.15.4 Thread radio

## Target setup flow

1. Flash the same ESP2THREAD image to every supported board.
2. On first boot, connect to the temporary ESP2THREAD setup access point.
3. Select the home Wi-Fi network and store its credentials locally.
4. On the first unit, create a new Thread network.
5. Add its local URL to Home Assistant's OpenThread Border Router integration.
6. Mark that Thread network preferred and send its credentials to Home
   Assistant and the Home Assistant companion app.
7. On additional units, import the existing active operational dataset rather
   than creating another mesh.

## Project status

The first native-radio firmware builds and boots successfully on the 4 MB XIAO
ESP32-C6. Wi-Fi provisioning, Thread network creation, reboot recovery, the
OpenThread REST API, MeshCoP advertisement and Home Assistant dataset import
have passed hardware tests. Home Assistant marks the imported network
preferred. A first production-footprint pass also passed on-device regression
testing and leaves about 94 KiB free in each OTA slot. Explicit Create/Join
selection, secure-body dataset import, a second-router join, two loaded Home
Assistant OTBR entries and bidirectional border-router failover now pass on two
physical units. Home Assistant Matter commissioning with end devices has now
also been confirmed, completing the hardware gates for the first stable release.

Follow the [hardware-tested setup guide](docs/SETUP.md), and see the
[roadmap](docs/ROADMAP.md), [test plan](docs/TEST_PLAN.md) and
[current results](docs/TEST_RESULTS.md).

For day-to-day use, follow the dedicated [flashing and update guide](docs/UPDATING.md),
[Home Assistant guide](docs/HOME_ASSISTANT.md), and
[device and IKEA compatibility guide](docs/DEVICES.md).

The onboard LED provides local status without a serial console. See the
[status LED reference](docs/LED_STATUS.md) for its patterns.

Holding the physical BOOT button for eight continuous seconds requests a
factory reset. The firmware restarts into a safe erase step and removes both
Wi-Fi and OpenThread configuration; a short press cancels without changing
stored state.

## Foundations

ESP2THREAD is based on:

- ESP-IDF v5.5.4
- Espressif ESP Thread Border Router SDK
- OpenThread
- The OpenThread REST API used by Home Assistant

Upstream source is pinned as a Git submodule so builds remain reproducible.

## Security

Wi-Fi passwords and Thread operational datasets are credentials. They must
never be committed to this repository, printed in test logs, or included in
bug reports. See [SECURITY.md](SECURITY.md).

## License

ESP2THREAD project code and documentation are licensed under Apache-2.0.
Third-party submodules retain their own licenses.
