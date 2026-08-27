# Home Assistant setup

This guide adds one or more ESP2THREAD border routers to Home Assistant. Every
router must use the same active Thread dataset. Do not create a second Thread
network for an additional router.

## Before starting

- Home Assistant and every ESP2THREAD must be on the same trusted home LAN.
- The Home Assistant **Thread**, **Matter** and **OpenThread Border Router**
  integrations must be installed.
- Keep operational datasets, network keys and PSKc out of screenshots, logs,
  issue reports and chat messages.
- Do not expose ESP2THREAD port 80 to the internet.

## Add the first router

1. Flash ESP2THREAD and complete its temporary Wi-Fi setup portal.
2. Open `http://esp2thread-xxxxxx.local/setup.html`, replacing `xxxxxx` with
   the unit's suffix.
3. Select **Create new Thread network** only when no Thread network exists.
4. Wait until the page reports `leader`, `router` or `child` and the onboard
   LED stays solid.
5. In Home Assistant, open **Settings > Devices & services > Add integration**.
6. Select **OpenThread Border Router**.
7. Enter `http://esp2thread-xxxxxx.local:80`. A reserved DHCP address may be
   used when `.local` resolution is unavailable.
8. Open the **Thread** integration. Confirm the imported ESP2THREAD network is
   listed and select **Make preferred network** when necessary.
9. In the Home Assistant Companion app, open its Thread settings and send or
   synchronize the preferred Thread credentials to the phone. Exact menu names
   vary by Android/iOS release.

Home Assistant does not need to restart for this process.

## Add another ESP2THREAD router

1. Flash the same firmware and complete Wi-Fi setup on the additional unit.
2. On an already configured router, open `/setup.html` and use **Copy dataset
   for another router**. Treat the result like a password.
3. On the additional router, open `/setup.html`, select **Join/extend an
   existing Thread network**, paste the dataset and choose **Join**.
4. Wait for an attached role and a solid LED.
5. Add another **OpenThread Border Router** integration using that unit's
   unique URL.

Both integration entries may have the same Home Assistant title. Their URLs
and unique `esp2thread-xxxxxx.local` hostnames distinguish them. Additional
routers provide alternate border paths and Thread radio coverage; they do not
create copies of Matter devices.

## Verify the installation

- Each ESP2THREAD `/node/state` endpoint reports `leader`, `router` or `child`.
- Home Assistant shows every OpenThread Border Router entry as loaded.
- The Thread integration contains one preferred network, not one network per
  router.
- Powering off either router eventually leaves the other attached. Leader
  election can take roughly one minute.

Automatic discovery depends on the Home Assistant release and local mDNS
handling. The manual URL flow is the supported fallback.

