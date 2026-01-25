#include "tasks_stats.h"
#include "gps_nmea.h"
#include "scheduler.h"
#include <stdio.h>

/* ============================================================================
 * STATISTICS & MONITORING TASKS
 * ============================================================================ */

/**
 * System statistics task - periodic reporting
 * 
 * Periodically prints:
 * - GPS position and status
 * - All active tasks and their performance
 * - Scheduler statistics
 * 
 * Priority: LOW - Not time-critical, only for monitoring/debugging
 * Interval: 10s - Periodic reporting doesn't need to be frequent
 */
uint32_t task_statistics(task_handle_t* task) {
    (void)task;
    
    /* Print GPS status if we have a fix */
    if (GPS_IsFixed()) {
        gps_position_t pos = GPS_GetPosition();
        printf("\n┌─ GPS Status ─────────────────────────┐\n");
        printf("│ Latitude:  %.6f°\n", pos.latitude);
        printf("│ Longitude: %.6f°\n", pos.longitude);
        printf("│ Altitude:  %.1f m\n", pos.altitude);
        printf("│ Satellites: %u\n", pos.num_satellites);
        printf("└───────────────────────────────────────┘\n\n");
    }
    
    /* Print all tasks and scheduler statistics */
    SCHED_PrintAllTasks();
    
    return 10000;  /* Run every 10 seconds */
}

/* ============================================================================
 * PLACEHOLDER TASKS (for future functionality)
 * ============================================================================ */

/**
 * Logging task - save data to EEPROM/Flash
 * TODO: Implement data logging when storage is integrated
 */
// uint32_t task_logging(task_handle_t* task) {
//     // log_gps_data();
//     // log_system_metrics();
//     return 60000;  // Every minute
// }

/**
 * Debug task - for development only
 * TODO: Remove from production builds
 */
// uint32_t task_debug(task_handle_t* task) {
//     // debug_print_memory_stats();
//     // debug_check_stack_usage();
//     return 30000;  // Every 30 seconds
// }
