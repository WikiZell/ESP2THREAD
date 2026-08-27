# Setup guide

This guide describes the first-router and additional-router flow for the Seeed
Studio XIAO ESP32-C6. The same firmware image is used on every unit. A fresh
board never creates a Thread network until the user explicitly chooses
**Create** or **Join**.

## 1. Provision Wi-Fi

1. Power the flashed XIAO ESP32-C6.
2. Join the unique open setup network named `ESP-ThreadBR-XXXX`.
3. Open `http://192.168.4.1`.
4. Select the home 2.4 GHz Wi-Fi network, enter its password and save.
5. Wait for the setup network to disappear.

Each device has a unique hostname derived from its factory MAC address, for
example `esp2thread-8f9ee4.local`. The last six characters distinguish units
and remain stable across firmware updates and factory resets. If local-name
resolution is unavailable, find the matching hostname or MAC address in the
router's DHCP clients. Keep the password out of serial logs, screenshots,
issue reports and repository files.

## 2. Choose the Thread role

After Wi-Fi connects, open `http://esp2thread-xxxxxx.local/setup.html`, replacing
`xxxxxx` with that unit's six-character suffix (or use its DHCP address).

For the first ESP2THREAD:

1. Keep or change the proposed network name.
2. Select **Create new Thread network**.
3. The page generates unique network credentials locally and submits them in a
   POST body; credentials are not placed in the URL.
4. Wait until the dashboard opens and reports `leader`.

For each additional ESP2THREAD:

1. On the already configured first router, open `/setup.html` and select
   **Copy dataset for another router**.
   If the browser blocks automatic clipboard access on local HTTP, copy the
   selected value from the manual field that appears.
2. On the additional router, open its `/setup.html` page.
3. Paste the dataset into **Join / extend a network** and select **Join existing
   Thread network**.
4. Wait for `child` or `router`. The additional unit must never be told to
   create another network.

The active operational dataset is a credential. Keep it out of screenshots,
messages, logs and issue reports, and clear the clipboard after the additional
router is configured.

## 3. Wait for Thread attachment

Open `http://esp2thread-xxxxxx.local/node/state`. A fresh single-router network first
reports `detached` and then changes to `leader`. The tested unit recovered its
Wi-Fi connection and leader role 44 seconds after a controlled reboot. Weak
2.4 GHz signal can extend this time; move the board closer to the access point
before diagnosing firmware or Thread problems.

Do not share the response from `/node/dataset/active`. It contains the Thread
network key and commissioner credential.

## 4. Add to Home Assistant

1. Open **Settings > Devices & services**.
2. Add **OpenThread Border Router**.
3. Enter `http://esp2thread-xxxxxx.local:80`. Use the DHCP address if `.local` name
   resolution is unavailable from Home Assistant.
4. Confirm that both **OpenThread Border Router** and **Thread** show as loaded.
5. In the Thread integration, confirm the ESP-created network is preferred.

Repeat the OpenThread Border Router integration step for an additional
ESP2THREAD using that unit's own local URL. Both routers must expose the same
Thread network name and Extended PAN ID because they share one active dataset.

The tested Home Assistant 2026.8.3 installation accepted the REST URL, imported
the active dataset from the ESP and marked it preferred. Standard MeshCoP mDNS
advertising was visible, but automatic creation of the OpenThread Border Router
entry was not observed; the manual URL remains the tested fallback.

## Factory reset

Keep the unit powered and hold its physical **BOOT** button continuously for
eight seconds. The onboard LED blinks rapidly during the countdown. Releasing
the button early cancels the operation. After a complete hold, the unit
restarts, erases the NVS partition before network services start, restarts
again, and returns to Wi-Fi setup.

This deliberately removes both saved Wi-Fi credentials and the active Thread
dataset. Export the dataset from another working border router first if this
is the only copy needed to add replacement routers. Do not hold BOOT while
applying power; GPIO9 is also the ESP32-C6 ROM download-mode strap. Start the
unit normally, then hold BOOT after it is running.

## Current limitations

- The Create/Join workflow, second-router dataset handoff and failover passed
  on two physical XIAO ESP32-C6 units.
- Dataset copy currently relies on the local unauthenticated Espressif REST
  service. Use ESP2THREAD only on a trusted home LAN; authenticated management
  is the compatibility boundary for this release. Web firmware upload remains
  disabled until it can require physical authorization and signed images.
- The ESP32-C6 shares one RF path between Wi-Fi and Thread. Placement and 2.4 GHz
  interference affect formation time and throughput.
- Matter-over-Thread commissioning remains pending until a confirmed compatible
  end device and its Matter setup code are available.
