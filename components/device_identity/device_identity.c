/*
 * SPDX-FileCopyrightText: 2026 WikiZell
 * SPDX-License-Identifier: Apache-2.0
 */

#include "device_identity.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "esp_check.h"
#include "esp_mac.h"
#include "mdns.h"

#define HOSTNAME_LENGTH 32
#define INSTANCE_NAME_LENGTH 64

static const char *TAG = "device_identity";
static char s_hostname[HOSTNAME_LENGTH];
static char s_instance_name[INSTANCE_NAME_LENGTH];
static bool s_initialized;

esp_err_t esp2thread_device_identity_init(void)
{
    uint8_t mac[6];
    ESP_RETURN_ON_ERROR(esp_read_mac(mac, ESP_MAC_BASE), TAG, "Unable to read base MAC address");

    snprintf(s_hostname, sizeof(s_hostname), "esp2thread-%02x%02x%02x", mac[3], mac[4], mac[5]);
    snprintf(s_instance_name, sizeof(s_instance_name), "ESP2THREAD %02X%02X%02X", mac[3], mac[4], mac[5]);
    s_initialized = true;
    return ESP_OK;
}

esp_err_t esp2thread_device_identity_apply_mdns(void)
{
    ESP_RETURN_ON_FALSE(s_initialized, ESP_ERR_INVALID_STATE, TAG, "Device identity is not initialized");
    ESP_RETURN_ON_ERROR(mdns_hostname_set(s_hostname), TAG, "Unable to set mDNS hostname");
    return mdns_instance_name_set(s_instance_name);
}

esp_err_t esp2thread_device_identity_apply_netif(esp_netif_t *netif)
{
    ESP_RETURN_ON_FALSE(s_initialized, ESP_ERR_INVALID_STATE, TAG, "Device identity is not initialized");
    ESP_RETURN_ON_FALSE(netif != NULL, ESP_ERR_INVALID_ARG, TAG, "Backbone netif is unavailable");
    return esp_netif_set_hostname(netif, s_hostname);
}

const char *esp2thread_device_hostname(void)
{
    return s_initialized ? s_hostname : NULL;
}

const char *esp2thread_device_instance_name(void)
{
    return s_initialized ? s_instance_name : NULL;
}
