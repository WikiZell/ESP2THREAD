# Connecting Matter, Thread and IKEA devices

ESP2THREAD routes **Thread** traffic and exposes the Thread network to Home
Assistant. Matter is the application protocol Home Assistant uses to commission
and control compatible devices.

## Matter-over-Thread device

1. Confirm the device packaging or label explicitly shows both **Matter** and
   **Thread**. A Matter QR code alone does not prove Thread; Matter can also use
   Wi-Fi or Ethernet.
2. Confirm ESP2THREAD is attached, Home Assistant's Thread network is preferred,
   and the phone running the Companion app has those Thread credentials.
3. Factory-reset the end device according to its manufacturer instructions.
4. In the Home Assistant Companion app, open **Settings > Devices & services**,
   select **Add device**, then **Add Matter device**.
5. Scan the Matter QR code or enter the numeric setup code.
6. Keep the phone close to the new device during Bluetooth commissioning. The
   phone supplies the Thread credentials; ESP2THREAD then provides the routed
   Thread path.
7. Name the device, assign its area and test commands while both routers are
   powered. Then repeat a command with each border router powered off in turn.

Do not publish a Matter setup code while the device is commissionable.

## IKEA compatibility

IKEA uses several different technologies that are not interchangeable:

| Label or product type | How it connects to Home Assistant |
| --- | --- |
| IKEA device labelled **Matter + Thread** | Commission with **Add Matter device**; ESP2THREAD provides Thread routing |
| IKEA Matter bridge such as DIRIGERA | Add the bridge through Home Assistant's Matter flow; the bridge itself uses the LAN and may bridge Zigbee accessories |
| IKEA Zigbee bulb, plug, sensor or remote | Pair with a Zigbee coordinator using ZHA or Zigbee2MQTT; it cannot join ESP2THREAD |
| IKEA Wi-Fi device | Use its supported LAN/cloud integration; it does not use Thread |

Many TRADFRI-era products and common IKEA remotes/sensors are Zigbee. Do not
factory-reset an existing working IKEA device until its exact model and radio
technology are confirmed. ESP32-C6 contains an IEEE 802.15.4 radio that can run
Thread, but this firmware cannot operate Zigbee and Thread concurrently and is
not a Zigbee coordinator or Zigbee repeater.

## Troubleshooting Matter commissioning

- Move the device, phone and one ESP2THREAD within a few metres for the first
  commission.
- Enable Bluetooth and local-network permission for the Companion app.
- Confirm the phone has the same preferred Thread dataset as Home Assistant.
- Remove a failed partial Matter entry before retrying the device's documented
  factory-reset procedure.
- Avoid 2.4 GHz Wi-Fi congestion where practical; ESP32-C6 shares one RF path
  between Wi-Fi and Thread.
- A Zigbee-only device will never appear in Matter commissioning, regardless of
  proximity or reset attempts.

