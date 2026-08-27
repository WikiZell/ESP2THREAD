# Architecture

## Objective

ESP2THREAD provides a reusable firmware image for the Seeed Studio XIAO
ESP32-C6. A first unit creates a Thread mesh and acts as a Wi-Fi Thread Border
Router. Additional units import the same Thread active operational dataset and
attach to that mesh, adding border-router paths and radio coverage.

## Home Assistant contract

Home Assistant integrates with an OpenThread Border Router through the
OpenThread REST API. ESP2THREAD will use Espressif's REST-compatible border
router server rather than a private API. The initial supported setup is:

1. ESP2THREAD and Home Assistant are on the same local network.
2. The user adds `http://<esp2thread-address>:80` to the **OpenThread Border
   Router** integration.
3. The **Thread** integration reads the active operational dataset through the
   border router service.
4. The user marks the network preferred and sends credentials to Home
   Assistant and their phone.
5. Matter commissioning uses that preferred Thread network.

Automatic discovery is a separate acceptance criterion. The firmware will
publish the standard Thread Border Agent MeshCoP service over mDNS. Testing
must prove whether the current Home Assistant release offers the device
automatically; the documented URL flow remains the compatibility fallback.

## Web interface overlay

ESP2THREAD keeps Espressif's border-router component pinned as an unmodified
submodule. A project-local `esp_ot_br_server` component compiles the pinned
upstream C sources while packaging a complete local copy of the web assets.
This permits ESP2THREAD branding and setup guidance without making the upstream
submodule dirty or dropping any management pages. Repository tests verify both
the branding and the required page set.

## Device states

The device has explicit states so first boot cannot silently create a second
Thread mesh:

- `UNPROVISIONED`: no Wi-Fi configuration; setup access point is active.
- `WIFI_READY`: Wi-Fi works but no Thread dataset is stored.
- `CREATE_NETWORK`: user explicitly requests a new random operational dataset.
- `IMPORT_NETWORK`: user supplies an existing active dataset.
- `BORDER_ROUTER`: Wi-Fi backbone and Thread network are active.
- `RECOVERY`: repeated startup failure; setup interface is restored without
  deleting the saved Thread dataset.

## Radio design

The ESP32-C6 native IEEE 802.15.4 radio runs OpenThread in full-thread-device
mode. Wi-Fi provides the backbone link. Espressif software coexistence is
enabled.

The C6 contains one shared RF path, so Wi-Fi and Thread cannot receive at the
same instant. This is the primary technical risk. Testing will cover packet
loss, Matter command latency, commissioning reliability, reconnection and
multiple-border-router behavior. A future production hardware revision may use
separate Wi-Fi and Thread chips without changing the Home Assistant API.

## Storage

The detected target has 4 MB flash. The initial partition design reserves:

- NVS for Wi-Fi and OpenThread persistent state
- two application slots for rollback-capable firmware updates
- a filesystem partition for the setup and management interface

Thread dataset export is always treated as secret material. It is displayed or
downloaded only after an explicit local user action and is never written to
ordinary logs.

## Upstream strategy

ESP-IDF v5.5.4 is the initial supported toolchain. The Espressif Thread Border
Router SDK is pinned as a submodule. Local integration code will remain small,
documented and tested so upstream security and compatibility updates can be
adopted deliberately.
