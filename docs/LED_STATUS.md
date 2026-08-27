# Status LED

The Seeed Studio XIAO ESP32-C6 user LED is connected to GPIO15 and is active
low. ESP2THREAD drives it from a low-priority FreeRTOS task. The task observes
network state only; it never starts, stops or reconfigures Wi-Fi or OpenThread.

| LED pattern | Meaning |
| --- | --- |
| One short pulse every two seconds | Waiting for first Wi-Fi setup |
| Two short pulses every two seconds | Saved Wi-Fi is disconnected or reconnecting |
| Fast blink | Wi-Fi has an IP address and the Thread partition is forming |
| Solid on | Border router is ready as a Thread child, router or leader |

The LED can remain in the fast-blink state for tens of seconds after boot. The
first network test returned to `leader` 44 seconds after a reboot. With the LED
firmware installed, the hardware sequence reached Wi-Fi/IP at 17 seconds and
the solid ready indication at 53 seconds. Weak 2.4 GHz signal or interference
can extend this period.

## Implementation constraints

- GPIO15 and active-low behavior are specific to the XIAO ESP32-C6 target.
- Pattern timing is non-blocking and does not run in a Wi-Fi or OpenThread event
  callback.
- Event callbacks only update the cached Wi-Fi/IP state.
- Thread role is read under the ESP-IDF OpenThread lock.
- No credential, dataset or network-key value is read or logged.
