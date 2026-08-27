/*
 * SPDX-FileCopyrightText: 2026 WikiZell
 * SPDX-License-Identifier: Apache-2.0
 */

#include "factory_reset.h"

#include <stdbool.h>
#include <stdint.h>

#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#define FACTORY_RESET_GPIO GPIO_NUM_9
#define FACTORY_RESET_ACTIVE_LEVEL 0
#define FACTORY_RESET_HOLD_MS 8000
#define FACTORY_RESET_SAMPLE_MS 50
#define FACTORY_RESET_LED_GPIO GPIO_NUM_15
#define FACTORY_RESET_LED_ON_LEVEL 0
#define FACTORY_RESET_LED_OFF_LEVEL 1
#define FACTORY_RESET_RTC_MAGIC 0x45535032U

static const char *TAG = "factory_reset";
static volatile bool s_reset_pending;
static RTC_NOINIT_ATTR uint32_t s_reset_rtc_magic;
static RTC_NOINIT_ATTR uint32_t s_reset_rtc_magic_inverse;

static void reset_led_set(bool on)
{
    gpio_set_level(FACTORY_RESET_LED_GPIO, on ? FACTORY_RESET_LED_ON_LEVEL : FACTORY_RESET_LED_OFF_LEVEL);
}

bool esp2thread_factory_reset_pending(void)
{
    return s_reset_pending;
}

static void factory_reset_task(void *context)
{
    (void)context;

    while (true) {
        if (gpio_get_level(FACTORY_RESET_GPIO) != FACTORY_RESET_ACTIVE_LEVEL) {
            vTaskDelay(pdMS_TO_TICKS(FACTORY_RESET_SAMPLE_MS));
            continue;
        }

        s_reset_pending = true;
        ESP_LOGW(TAG, "BOOT pressed; keep holding for 8 seconds to erase local configuration");

        unsigned elapsed_ms = 0;
        for (; elapsed_ms < FACTORY_RESET_HOLD_MS; elapsed_ms += FACTORY_RESET_SAMPLE_MS) {
            reset_led_set((elapsed_ms / 200U) % 2U == 0U);
            vTaskDelay(pdMS_TO_TICKS(FACTORY_RESET_SAMPLE_MS));
            if (gpio_get_level(FACTORY_RESET_GPIO) != FACTORY_RESET_ACTIVE_LEVEL) {
                break;
            }
        }

        if (elapsed_ms < FACTORY_RESET_HOLD_MS) {
            reset_led_set(false);
            s_reset_pending = false;
            ESP_LOGI(TAG, "Factory reset cancelled");
            continue;
        }

        ESP_LOGW(TAG, "Factory reset confirmed; restarting into safe erase mode");
        s_reset_rtc_magic = FACTORY_RESET_RTC_MAGIC;
        s_reset_rtc_magic_inverse = ~FACTORY_RESET_RTC_MAGIC;
        reset_led_set(true);
        vTaskDelay(pdMS_TO_TICKS(250));
        esp_restart();
    }
}

esp_err_t esp2thread_factory_reset_start(void)
{
    if (s_reset_rtc_magic == FACTORY_RESET_RTC_MAGIC &&
        s_reset_rtc_magic_inverse == ~FACTORY_RESET_RTC_MAGIC) {
        s_reset_rtc_magic = 0;
        s_reset_rtc_magic_inverse = 0;
        ESP_LOGW(TAG, "Erasing Wi-Fi and OpenThread configuration");
        ESP_RETURN_ON_ERROR(nvs_flash_deinit(), TAG, "Unable to close NVS");
        ESP_RETURN_ON_ERROR(nvs_flash_erase(), TAG, "Unable to erase NVS");
        ESP_LOGI(TAG, "Factory reset complete; restarting");
        esp_restart();
        return ESP_OK;
    }

    const gpio_config_t button_config = {
        .pin_bit_mask = 1ULL << FACTORY_RESET_GPIO,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_RETURN_ON_ERROR(gpio_config(&button_config), TAG, "Unable to configure BOOT button");

    const gpio_config_t led_config = {
        .pin_bit_mask = 1ULL << FACTORY_RESET_LED_GPIO,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_RETURN_ON_ERROR(gpio_config(&led_config), TAG, "Unable to configure reset indicator");
    reset_led_set(false);

    if (xTaskCreate(factory_reset_task, "factory_reset", 3072, NULL, 2, NULL) != pdPASS) {
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}
