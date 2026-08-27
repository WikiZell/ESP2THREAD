/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-FileCopyrightText: 2026 WikiZell
 * SPDX-License-Identifier: CC0-1.0
 */

#include "border_router_launch.h"

#include <string.h>

#include "esp_check.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_openthread.h"
#include "esp_openthread_border_router.h"
#include "esp_openthread_lock.h"
#include "esp_ot_cli_extension.h"
#include "esp_ot_rcp_update.h"
#include "esp_rcp_update.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#if CONFIG_OPENTHREAD_BR_SOFTAP_SETUP
#include "esp_br_wifi_config.h"
#endif
#include "openthread/cli.h"
#include "openthread/dataset.h"
#include "openthread/error.h"
#include "openthread/ip6.h"
#include "protocol_examples_common.h"
#include "ot_examples_common.h"

#if CONFIG_OPENTHREAD_CLI_WIFI
#include "esp_ot_wifi_cmd.h"
#endif

#if !CONFIG_EXAMPLE_CONNECT_WIFI && !CONFIG_EXAMPLE_CONNECT_ETHERNET
#error No backbone netif configured
#endif

#define TAG "esp2thread_br"

#if CONFIG_EXAMPLE_CONNECT_WIFI && CONFIG_OPENTHREAD_BR_AUTO_START
static bool wifi_config_save_and_connect(const char *ssid, const char *password)
{
    if (esp_ot_wifi_connect(ssid, password) != ESP_OK) {
        return false;
    }

    if (esp_ot_wifi_config_set_ssid(ssid) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to save Wi-Fi SSID");
        return false;
    }

    const char *stored_password = password != NULL ? password : "";
    if (esp_ot_wifi_config_set_password(stored_password) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to save Wi-Fi password");
        return false;
    }
    return true;
}
#endif

#if CONFIG_OPENTHREAD_BR_AUTO_START
static void ot_br_init(void *context)
{
    (void)context;

#if CONFIG_EXAMPLE_CONNECT_WIFI
    char wifi_ssid[32] = "";
    char wifi_password[64] = "";
    bool has_saved_wifi = false;

    if (esp_ot_wifi_config_get_ssid(wifi_ssid) == ESP_OK) {
        esp_ot_wifi_config_get_password(wifi_password);
        has_saved_wifi = true;
    }

    if (!(has_saved_wifi && esp_ot_wifi_connect(wifi_ssid, wifi_password) == ESP_OK)) {
#if CONFIG_OPENTHREAD_BR_SOFTAP_SETUP
        ESP_ERROR_CHECK(esp_br_wifi_config_start());
        ESP_ERROR_CHECK(esp_br_wifi_config_get_configured_wifi(
            wifi_ssid, sizeof(wifi_ssid), wifi_password, sizeof(wifi_password), 0));
        ESP_ERROR_CHECK(esp_br_wifi_config_stop());
#else
        strlcpy(wifi_ssid, CONFIG_EXAMPLE_WIFI_SSID, sizeof(wifi_ssid));
        strlcpy(wifi_password, CONFIG_EXAMPLE_WIFI_PASSWORD, sizeof(wifi_password));
#endif

        if (!wifi_config_save_and_connect(wifi_ssid, wifi_password)) {
            ESP_LOGE(TAG, "Wi-Fi connection failed; rebooting to retry");
            esp_restart();
        }
    }
#elif CONFIG_EXAMPLE_CONNECT_ETHERNET
    ESP_ERROR_CHECK(example_ethernet_connect());
#endif

    esp_openthread_lock_acquire(portMAX_DELAY);
    esp_openthread_set_backbone_netif(get_example_netif());
    ESP_ERROR_CHECK(esp_openthread_border_router_init());
#if CONFIG_EXAMPLE_CONNECT_WIFI
    esp_ot_wifi_border_router_init_flag_set(true);
#endif

    otOperationalDatasetTlvs dataset;
    const otError dataset_error = otDatasetGetActiveTlvs(esp_openthread_get_instance(), &dataset);
    if (dataset_error == OT_ERROR_NONE) {
        ESP_ERROR_CHECK(esp_openthread_auto_start(&dataset));
        ESP_LOGI(TAG, "Started saved Thread network");
    } else if (dataset_error == OT_ERROR_NOT_FOUND || dataset_error == OT_ERROR_INVALID_STATE) {
        /*
         * A fresh board stays uncommissioned. The local setup page or Home
         * Assistant must explicitly create/import a dataset before Thread is
         * enabled. This prevents additional boards from forming split meshes.
         */
        ESP_ERROR_CHECK(otIp6SetEnabled(esp_openthread_get_instance(), true) == OT_ERROR_NONE ? ESP_OK : ESP_FAIL);
        ESP_LOGI(TAG, "No Thread dataset; waiting for explicit Create or Join selection");
    } else {
        ESP_LOGE(TAG, "Unable to read saved Thread dataset: %s", otThreadErrorToString(dataset_error));
    }
    esp_openthread_lock_release();

    vTaskDelete(NULL);
}
#endif

void launch_openthread_border_router(const esp_openthread_config_t *config,
                                     const esp_rcp_update_config_t *update_config)
{
#if CONFIG_OPENTHREAD_CLI
    ot_console_start();
#endif

#if CONFIG_AUTO_UPDATE_RCP
    ESP_ERROR_CHECK(esp_rcp_update_init(update_config));
    esp_ot_register_rcp_handler();
#else
    (void)update_config;
#endif

    ESP_ERROR_CHECK(esp_openthread_start(config));
#if CONFIG_AUTO_UPDATE_RCP
    esp_ot_update_rcp_if_different();
#endif
#if CONFIG_OPENTHREAD_CLI_ESP_EXTENSION
    esp_cli_custom_command_init();
#endif
    ot_register_external_commands();
#if CONFIG_OPENTHREAD_BR_AUTO_START
    if (xTaskCreate(ot_br_init, "ot_br_init", 6144, NULL, 4, NULL) != pdPASS) {
        ESP_LOGE(TAG, "Unable to start border-router initialization task");
    }
#endif
}
