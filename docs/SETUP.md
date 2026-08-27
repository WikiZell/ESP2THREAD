# Setup guide

This guide describes the hardware-tested first-router flow for the Seeed
Studio XIAO ESP32-C6. The current development firmware automatically creates a
new random Thread network after first Wi-Fi provisioning. Role selection and
safe import for additional routers remain roadmap work.

## 1. Provision Wi-Fi

1. Power the flashed XIAO ESP32-C6.
2. Join the unique open setup network named `ESP-ThreadBR-XXXX`.
3. Open `http://192.168.4.1`.
4. Select the home 2.4 GHz Wi-Fi network, enter its password and save.
5. Wait for the setup network to disappear.

The device hostname is `esp2thread.local`. If local-name resolution is not
available, find its DHCP address in the router. Keep the password out of serial
logs, screenshots, issue reports and repository files.

## 2. Wait for Thread formation

Open `http://esp2thread.local/node/state`. A fresh single-router network first
reports `detached` and then changes to `leader`. The tested unit recovered its
Wi-Fi connection and leader role 44 seconds after a controlled reboot. Weak
2.4 GHz signal can extend this time; move the board closer to the access point
before diagnosing firmware or Thread problems.

Do not share the response from `/node/dataset/active`. It contains the Thread
network key and commissioner credential.

## 3. Add to Home Assistant

1. Open **Settings > Devices & services**.
2. Add **OpenThread Border Router**.
3. Enter `http://esp2thread.local:80`. Use the DHCP address if `.local` name
   resolution is unavailable from Home Assistant.
4. Confirm that both **OpenThread Border Router** and **Thread** show as loaded.
5. In the Thread integration, confirm the ESP-created network is preferred.

The tested Home Assistant 2026.8.3 installation accepted the REST URL, imported
the active dataset from the ESP and marked it preferred. Standard MeshCoP mDNS
advertising was visible, but automatic creation of the OpenThread Border Router
entry was not observed; the manual URL remains the tested fallback.

## Current limitations

- The development firmware creates a new Thread network automatically on the
  first provisioned unit. Do not provision a second unit yet: it would create a
  separate mesh instead of extending the first one.
- Importing the preferred Home Assistant dataset and explicit
  **Create**/**Join** role selection are not implemented yet.
- The ESP32-C6 shares one RF path between Wi-Fi and Thread. Placement and 2.4 GHz
  interference affect formation time and throughput.
- Matter-over-Thread commissioning and multi-router failover have not passed
  hardware testing yet.
