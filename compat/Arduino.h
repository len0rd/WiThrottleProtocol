#pragma once
/**
 * Minimal Arduino.h shim for ESP-IDF.
 * Provides just enough surface area for WiThrottleProtocol to compile.
 */
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "WString.h"
#include "Print.h" // defines both Print and Stream

// Arduino type alias
using boolean = bool;

// millis() — milliseconds since boot
inline uint32_t millis()
{
    return (uint32_t) (esp_timer_get_time() / 1000ULL);
}

// delay() — blocking delay in milliseconds
inline void delay(uint32_t ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}
