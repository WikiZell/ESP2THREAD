# Test plan

Documentation and tests are part of every firmware change. A change is not
complete until its behavior, test evidence and user instructions agree.

## Automated checks

Every push must check:

- clean ESP-IDF build for `esp32c6`
- firmware and partition images fit the detected 4 MB flash
- no real credentials or operational datasets are present in tracked files
- configuration selects native IEEE 802.15.4 radio and software coexistence
- OpenThread REST API routes required by Home Assistant are compiled in
- Markdown links and repository structure are valid

## Bench tests

### Provisioning

- Fresh flash exposes the setup access point.
- Wrong Wi-Fi credentials do not destroy an existing Thread dataset.
- Correct credentials survive reboot.
- Credentials are absent from normal serial logs.

### Thread network creation

- A network is created only after an explicit action.
- Network name, channel and dataset persist after reboot.
- Factory reset requires a deliberate action and clears both Wi-Fi and Thread
  state.

### Home Assistant

- Home Assistant reaches the REST API through the ESP's Wi-Fi address.
- OpenThread Border Router integration setup succeeds.
- Thread integration reads the active dataset and marks it preferred.
- Credentials transfer to the Home Assistant companion app succeeds.
- Standard mDNS discovery is observed and its UI behavior is documented.

### Matter-over-Thread

- Commission one supported Matter-over-Thread device.
- Control it from Home Assistant and record command latency.
- Reboot the ESP and verify automatic recovery.
- Reboot Home Assistant and verify automatic recovery.
- Run a sustained connectivity test while ordinary Wi-Fi traffic is present.

### Multiple routers

- Export the first router's dataset without logging it.
- Import it into a second router.
- Confirm both attach to the same Thread partition and do not create a split
  network.
- Remove each router in turn and verify the remaining border path.

## Test evidence

Each hardware-tested release records firmware commit, board revision, ESP chip
revision, flash size, Home Assistant version, Matter Server version, test
device, result and any known limitation. Real network credentials are never
recorded.

