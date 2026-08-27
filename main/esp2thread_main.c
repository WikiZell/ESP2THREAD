/*
 * SPDX-FileCopyrightText: 2026 WikiZell
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"

#include "border_router_launch.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_openthread.h"
#include "esp_openthread_netif_glue.h"
#include "esp_openthread_types.h"
#include "esp_ot_config.h"
#include "esp_spiffs.h"
#include "esp_vfs_eventfd.h"
#include "esp_br_web.h"
#include "mdns.h"
#include "nvs_flash.h"

#define TAG "esp2thread"
#define ESP2THREAD_HOSTNAME "esp2thread"
#define WEB_MOUNT_POINT "/spiffs"

static esp_err_t init_nvs(void)
{
    esp_err_t error = nvs_flash_init();
    if (error == ESP_ERR_NVS_NO_FREE_PAGES || error == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_RETURN_ON_ERROR(nvs_flash_erase(), TAG, "Unable to recover NVS");
        error = nvs_flash_init();
    }
    return error;
}

static esp_err_t init_web_storage(void)
{
    const esp_vfs_spiffs_conf_t config = {
        .base_path = WEB_MOUNT_POINT,
        .partition_label = "web_storage",
        .max_files = 10,
        .format_if_mount_failed = false,
    };
    return esp_vfs_spiffs_register(&config);
}

void app_main(void)
{
    const esp_vfs_eventfd_config_t eventfd_config = {
        // netif, task queue, border router and native radio driver
        .max_fds = 4,
    };
    static const esp_openthread_config_t openthread_config = {
        .netif_config = ESP_NETIF_DEFAULT_OPENTHREAD(),
        .platform_config = {
            .radio_config = ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG(),
            .host_config = ESP_OPENTHREAD_DEFAULT_HOST_CONFIG(),
            .port_config = ESP_OPENTHREAD_DEFAULT_PORT_CONFIG(),
        },
    };

    ESP_ERROR_CHECK(esp_vfs_eventfd_register(&eventfd_config));
    ESP_ERROR_CHECK(init_nvs());
    ESP_ERROR_CHECK(init_web_storage());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(mdns_init());
    ESP_ERROR_CHECK(mdns_hostname_set(ESP2THREAD_HOSTNAME));

    ESP_LOGI(TAG, "Starting ESP2THREAD single-chip border router");
    esp_br_web_start(WEB_MOUNT_POINT);
    launch_openthread_border_router(&openthread_config, NULL);
}
