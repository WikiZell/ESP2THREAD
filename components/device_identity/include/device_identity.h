/*
 * SPDX-FileCopyrightText: 2026 WikiZell
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Build the stable, MAC-derived identity used by this ESP2THREAD unit. */
esp_err_t esp2thread_device_identity_init(void);

/** Apply the device identity to the mDNS responder after mdns_init(). */
esp_err_t esp2thread_device_identity_apply_mdns(void);

/** Apply the DHCP hostname to a Wi-Fi station netif before it connects. */
esp_err_t esp2thread_device_identity_apply_netif(esp_netif_t *netif);

/** Hostname without the .local suffix, for example esp2thread-8f9ee4. */
const char *esp2thread_device_hostname(void);

/** Human-readable mDNS instance name. */
const char *esp2thread_device_instance_name(void);

#ifdef __cplusplus
}
#endif
