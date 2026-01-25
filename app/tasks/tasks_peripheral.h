#pragma once
#include "scheduler.h"

/**
 * Task definitions for peripheral monitoring
 */

uint32_t task_led_blink(task_handle_t* task);
uint32_t task_battery_monitor(task_handle_t* task);
