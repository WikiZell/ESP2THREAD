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

The device hostname is `esp2thread.local`. If local-name resolution is not
available, find its DHCP address in the router. Keep the password out of serial
logs, screenshots, issue reports and repository files.

## 2. Choose the Thread role

After Wi-Fi connects, open `http://esp2thread.local/setup.html` (or use the
DHCP address).

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

Open `http://esp2thread.local/node/state`. A fresh single-router network first
reports `detached` and then changes to `leader`. The tested unit recovered its
Wi-Fi connection and leader role 44 seconds after a controlled reboot. Weak
2.4 GHz signal can extend this time; move the board closer to the access point
before diagnosing firmware or Thread problems.

Do not share the response from `/node/dataset/active`. It contains the Thread
network key and commissioner credential.

## 4. Add to Home Assistant

1. Open **Settings > Devices & services**.
2. Add **OpenThread Border Router**.
3. Enter `http://esp2thread.local:80`. Use the DHCP address if `.local` name
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

## Current limitations

- The Create/Join workflow and existing-network upgrade path passed on the
  first hardware unit. A second identical board is still required to prove the
  complete multi-router handoff and failover behavior.
- Dataset copy currently relies on the local unauthenticated Espressif REST
  service. Use ESP2THREAD only on a trusted home LAN; authenticated management
  is required before a stable release.
- The ESP32-C6 shares one RF path between Wi-Fi and Thread. Placement and 2.4 GHz
  interference affect formation time and throughput.
- Matter-over-Thread commissioning and multi-router failover have not passed
  hardware testing yet.
