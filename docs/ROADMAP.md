# Roadmap

## Milestone 0: reproducible foundation

- Pin ESP-IDF and ESP Thread Border Router SDK versions.
- Define the XIAO ESP32-C6 4 MB partition layout.
- Build a single-chip native-radio border-router image.
- Run static repository checks in CI.

## Milestone 1: first usable border router

- First-boot Wi-Fi access-point provisioning.
- Explicit **Create new Thread network** action.
- OpenThread REST API and local status page.
- Home Assistant OpenThread Border Router integration validation.
- Active dataset transfer to Home Assistant and phone.

## Milestone 2: additional routers

Status: hardware-validated on two XIAO ESP32-C6 units on 2026-08-27.

- Explicit **Join existing Thread network** action using an imported dataset.
- Safe dataset validation and redaction.
- Verify two ESP2THREAD units attach to the same partition and both advertise
  border-router service.
- Recovery behavior when one router loses power or Wi-Fi.

## Milestone 3: appliance experience

Status: release-candidate scope complete except signed web OTA and final
Matter-over-Thread device commissioning.

- Standard mDNS discovery validated with Home Assistant.
- Friendly hostname and stable local identity.
- Signed, rollback-capable local updates.
- Backup and restore workflow.
- Diagnostics suitable for support without leaking credentials.

## Release gate

No release is labelled stable until it passes the hardware matrix in
`docs/TEST_PLAN.md`, including Matter-over-Thread commissioning through Home
Assistant and a sustained coexistence test.
