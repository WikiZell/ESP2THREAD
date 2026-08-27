# ESP2THREAD

ESP2THREAD turns a Seeed Studio XIAO ESP32-C6 into a Wi-Fi-connected
OpenThread Border Router for Home Assistant. The intended experience is a
small network appliance: provision Wi-Fi, create the first Thread network,
add the router to Home Assistant, then copy the same Thread dataset to
additional ESP2THREAD units.

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

## Planned setup flow

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

The first native-radio firmware foundation builds and boots successfully on
the 4 MB XIAO ESP32-C6. The branded first-boot setup network, portal and Wi-Fi
scanner have passed hardware tests, and Espressif's OpenThread REST-compatible
web server is compiled in. Home Wi-Fi provisioning, Home Assistant discovery
and Matter commissioning have not passed yet, so no release image is published. See
[the roadmap](docs/ROADMAP.md), [test plan](docs/TEST_PLAN.md) and
[current results](docs/TEST_RESULTS.md).

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
