/*
 * SPDX-FileCopyrightText: 2026 WikiZell
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"

/** Start the non-blocking XIAO ESP32-C6 status LED task. */
esp_err_t esp2thread_status_led_start(void);
