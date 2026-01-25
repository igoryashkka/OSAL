#include "tasks_peripheral.h"
#include "module_blink.h"
#include <stdio.h>

/* ============================================================================
 * PERIPHERAL TASKS
 * ============================================================================ */

/**
 * LED blink task - indicates system is alive
 * 
 * Blinks the LED at a fixed interval to show the system is running.
 * This is a simple heartbeat indicator.
 */
uint32_t task_led_blink(task_handle_t* task) {
    (void)task;
    
    blink_once();
    
    return 500;  /* Run every 500ms */
}

/**
 * Battery monitoring task
 * 
 * Reads battery voltage and performs monitoring/alerting.
 * TODO: Implement actual battery ADC reading and threshold checking
 */
uint32_t task_battery_monitor(task_handle_t* task) {
    (void)task;
    
    /* TODO: Read battery ADC and monitor
     * 
     * Example implementation:
     * uint16_t battery_mv = ADC_ReadBattery();
     * if (battery_mv < CRITICAL_LEVEL) {
     *     alert_critical_battery();
     * } else if (battery_mv < LOW_LEVEL) {
     *     alert_low_battery();
     * }
     */
    
    return 5000;  /* Run every 5 seconds */
}
