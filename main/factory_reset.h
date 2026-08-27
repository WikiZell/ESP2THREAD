/*
 * SPDX-FileCopyrightText: 2026 WikiZell
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>

#include "esp_err.h"

/** Start the physical BOOT-button factory-reset monitor. */
esp_err_t esp2thread_factory_reset_start(void);

/** True while an eight-second reset hold is being counted. */
bool esp2thread_factory_reset_pending(void);
