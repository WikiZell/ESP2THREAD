/*
 * SPDX-FileCopyrightText: 2026 WikiZell
 * SPDX-License-Identifier: Apache-2.0
 */

#include "status_led.h"

#include <stdbool.h>
#include <stddef.h>

#include "driver/gpio.h"
#include "esp_check.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_openthread.h"
#include "esp_openthread_lock.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "openthread/dataset.h"
#include "openthread/instance.h"
#include "openthread/thread.h"

#define STATUS_LED_GPIO GPIO_NUM_15
#define STATUS_LED_ON_LEVEL 0
#define STATUS_LED_OFF_LEVEL 1
#define STATUS_LED_TICK_MS 100
#define STATUS_LED_CYCLE_TICKS 20

static const char *TAG = "status_led";
static volatile bool s_wifi_has_ip;

typedef enum {
    LED_STATUS_WAITING_SETUP,
    LED_STATUS_WIFI_DISCONNECTED,
    LED_STATUS_THREAD_SELECTION,
    LED_STATUS_THREAD_FORMING,
    LED_STATUS_READY,
} led_status_t;

static void set_led(bool on)
{
    gpio_set_level(STATUS_LED_GPIO, on ? STATUS_LED_ON_LEVEL : STATUS_LED_OFF_LEVEL);
}

static bool wifi_is_configured(void)
{
    nvs_handle_t handle;
    if (nvs_open("wifi_config", NVS_READONLY, &handle) != ESP_OK) {
        return false;
    }

    size_t ssid_length = 0;
    const bool configured = nvs_get_str(handle, "ssid", NULL, &ssid_length) == ESP_OK && ssid_length > 1;
    nvs_close(handle);
    return configured;
}

static bool thread_is_ready(void)
{
    otInstance *instance = esp_openthread_get_instance();
    if (instance == NULL) {
        return false;
    }

    esp_openthread_lock_acquire(portMAX_DELAY);
    const otDeviceRole role = otThreadGetDeviceRole(instance);
    esp_openthread_lock_release();

    return role == OT_DEVICE_ROLE_CHILD || role == OT_DEVICE_ROLE_ROUTER || role == OT_DEVICE_ROLE_LEADER;
}

static bool thread_has_dataset(void)
{
    otInstance *instance = esp_openthread_get_instance();
    if (instance == NULL) {
        return true;
    }

    otOperationalDatasetTlvs dataset;
    esp_openthread_lock_acquire(portMAX_DELAY);
    const bool present = otDatasetGetActiveTlvs(instance, &dataset) == OT_ERROR_NONE;
    esp_openthread_lock_release();
    return present;
}

static led_status_t get_status(bool wifi_configured)
{
    if (!wifi_configured) {
        return LED_STATUS_WAITING_SETUP;
    }
    if (!s_wifi_has_ip) {
        return LED_STATUS_WIFI_DISCONNECTED;
    }
    if (!thread_has_dataset()) {
        return LED_STATUS_THREAD_SELECTION;
    }
    return thread_is_ready() ? LED_STATUS_READY : LED_STATUS_THREAD_FORMING;
}

static const char *status_name(led_status_t status)
{
    switch (status) {
    case LED_STATUS_WAITING_SETUP:
        return "waiting for Wi-Fi setup";
    case LED_STATUS_WIFI_DISCONNECTED:
        return "Wi-Fi disconnected";
    case LED_STATUS_THREAD_SELECTION:
        return "waiting for Create or Join selection";
    case LED_STATUS_THREAD_FORMING:
        return "Thread forming";
    case LED_STATUS_READY:
        return "border router ready";
    default:
        return "unknown";
    }
}

static bool led_on_for_pattern(led_status_t status, unsigned phase)
{
    switch (status) {
    case LED_STATUS_WAITING_SETUP:
        return phase == 0;
    case LED_STATUS_WIFI_DISCONNECTED:
        return phase == 0 || phase == 2;
    case LED_STATUS_THREAD_SELECTION:
        return phase == 0 || phase == 2 || phase == 4;
    case LED_STATUS_THREAD_FORMING:
        return (phase / 2) % 2 == 0;
    case LED_STATUS_READY:
        return true;
    default:
        return false;
    }
}

static void status_led_task(void *context)
{
    (void)context;
    unsigned phase = 0;
    led_status_t previous = (led_status_t)-1;
    bool wifi_configured = wifi_is_configured();

    while (true) {
        // During first setup, retry the read-only NVS check once per LED cycle.
        // Once configured it remains cached until reboot, avoiding repeated
        // flash reads during normal operation.
        if (!wifi_configured && phase == 0) {
            wifi_configured = wifi_is_configured();
        }

        const led_status_t status = get_status(wifi_configured);
        if (status != previous) {
            ESP_LOGI(TAG, "Status: %s", status_name(status));
            previous = status;
            phase = 0;
        }

        set_led(led_on_for_pattern(status, phase));
        phase = (phase + 1) % STATUS_LED_CYCLE_TICKS;
        vTaskDelay(pdMS_TO_TICKS(STATUS_LED_TICK_MS));
    }
}

static void network_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    (void)arg;
    (void)event_data;

    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        s_wifi_has_ip = true;
    } else if ((event_base == IP_EVENT && event_id == IP_EVENT_STA_LOST_IP) ||
               (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)) {
        s_wifi_has_ip = false;
    }
}

esp_err_t esp2thread_status_led_start(void)
{
    const gpio_config_t config = {
        .pin_bit_mask = 1ULL << STATUS_LED_GPIO,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_RETURN_ON_ERROR(gpio_config(&config), TAG, "Unable to configure status LED");
    set_led(false);

    ESP_RETURN_ON_ERROR(
        esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, network_event_handler, NULL),
        TAG,
        "Unable to register IP event handler");
    ESP_RETURN_ON_ERROR(
        esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, network_event_handler, NULL),
        TAG,
        "Unable to register Wi-Fi event handler");

    if (xTaskCreate(status_led_task, "status_led", 3072, NULL, 3, NULL) != pdPASS) {
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}
