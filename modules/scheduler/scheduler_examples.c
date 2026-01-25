/**
 * Task Scheduler - Application Integration Examples
 * 
 * Shows how to use the scheduler to manage multiple periodic tasks.
 */

#include "scheduler.h"
#include "gps_nmea.h"
#include "module_blink.h"
#include <stdio.h>

/* ============================================================================
 * TASK DEFINITIONS
 * ============================================================================ */

/**
 * LED blink task - runs every 500ms
 */
static uint32_t task_led_blink(task_handle_t* task) {
    (void)task;
    
    /* Toggle LED */
    blink_once();
    
    /* Run again in 500ms */
    return 500;
}

/**
 * GPS data processing task - runs every 100ms
 * Reads available UART data and parses GPS sentences
 */
static uint32_t task_gps_process(task_handle_t* task) {
    (void)task;
    
    /* Read any available GPS data */
    uint8_t buffer[32];
    int bytes = GPS_Read(buffer, sizeof(buffer));
    
    /* Process each byte */
    for (int i = 0; i < bytes; i++) {
        GPS_ProcessByte(buffer[i]);
    }
    
    /* Check if we have a fix */
    if (GPS_IsFixed()) {
        gps_position_t pos = GPS_GetPosition();
        // Use position data...
        // printf("GPS: %.6f, %.6f\n", pos.latitude, pos.longitude);
    }
    
    /* Run again in 100ms */
    return 100;
}

/**
 * Battery monitoring task - runs every 5 seconds
 */
static uint32_t task_battery_monitor(task_handle_t* task) {
    (void)task;
    
    // Read battery ADC
    // uint16_t battery_mv = ADC_ReadBattery();
    // Check battery level and take action if needed
    
    /* Run again in 5000ms */
    return 5000;
}

/**
 * Statistics/logging task - runs every 10 seconds
 */
static uint32_t task_stats_report(task_handle_t* task) {
    (void)task;
    
    /* Print GPS statistics */
    GPS_PrintStats();
    
    /* Print scheduler statistics */
    SCHED_PrintAllTasks();
    
    /* Run again in 10000ms */
    return 10000;
}

/**
 * Ethernet/Network task - runs every 200ms when enabled
 */
static uint32_t task_ethernet(task_handle_t* task) {
    (void)task;
    
    // Process network packets
    // eth_process_packets();
    
    return 200;
}

/* ============================================================================
 * APPLICATION INITIALIZATION
 * ============================================================================ */

void app_init_tasks(void) {
    printf("Initializing Task Scheduler...\n");
    
    /* Initialize scheduler */
    SCHED_Init();
    
    /* Create tasks */
    task_handle_t* t_led = SCHED_CreateTask(
        "LED_Blink",
        task_led_blink,
        TASK_PRIORITY_NORMAL,
        500,        /* Run every 500ms */
        NULL
    );
    
    task_handle_t* t_gps = SCHED_CreateTask(
        "GPS_Process",
        task_gps_process,
        TASK_PRIORITY_HIGH,  /* GPS is important */
        100,        /* Run every 100ms */
        NULL
    );
    
    task_handle_t* t_bat = SCHED_CreateTask(
        "Battery_Monitor",
        task_battery_monitor,
        TASK_PRIORITY_NORMAL,
        5000,       /* Run every 5 seconds */
        NULL
    );
    
    task_handle_t* t_stats = SCHED_CreateTask(
        "Stats_Report",
        task_stats_report,
        TASK_PRIORITY_LOW,
        10000,      /* Run every 10 seconds */
        NULL
    );
    
    /* Optional: create ethernet task but don't start it yet */
    task_handle_t* t_eth = SCHED_CreateTask(
        "Ethernet",
        task_ethernet,
        TASK_PRIORITY_NORMAL,
        200,
        NULL
    );
    
    /* Start tasks */
    SCHED_StartTask(t_led);
    SCHED_StartTask(t_gps);
    SCHED_StartTask(t_bat);
    SCHED_StartTask(t_stats);
    /* Don't start ethernet yet */
    
    printf("Tasks created:\n");
    printf("  - LED_Blink (500ms)\n");
    printf("  - GPS_Process (100ms, HIGH priority)\n");
    printf("  - Battery_Monitor (5s)\n");
    printf("  - Stats_Report (10s, LOW priority)\n");
    printf("  - Ethernet (200ms, disabled)\n");
    printf("\n");
}

/* ============================================================================
 * MAIN APPLICATION ENTRY POINT
 * ============================================================================ */

/**
 * Simple example: Run scheduler in main loop
 */
int main_scheduler_example(void) {
    /* Platform initialization */
    // init_system();
    
    /* Task initialization */
    app_init_tasks();
    
    /* Initialize GPS */
    GPS_Init(9600);
    
    printf("Starting scheduler...\n\n");
    
    /* Run main scheduler loop (never returns) */
    SCHED_Run();
    
    return 0;  /* Never reaches here */
}

/* ============================================================================
 * ALTERNATIVE: RUN SCHEDULER IN A MAIN LOOP
 * (If you need more control over main loop)
 * ============================================================================ */

int main_with_custom_loop(void) {
    /* Platform initialization */
    // init_system();
    
    /* Task initialization */
    app_init_tasks();
    
    /* Get task handles for manual control */
    task_handle_t* gps_task = SCHED_FindTask("GPS_Process");
    task_handle_t* stats_task = SCHED_FindTask("Stats_Report");
    
    printf("Starting custom event loop...\n\n");
    
    /* Custom event loop */
    while (1) {
        /* Manually run one scheduler cycle */
        // SCHED_SingleCycle();  // Not implemented - use SCHED_Run instead
        
        /* Or handle other application-specific events */
        // if (some_event_occurred) {
        //     SCHED_PauseTask(stats_task);
        // }
        
        __asm("nop");  /* Yield to other threads if using RTOS */
    }
    
    return 0;
}

/* ============================================================================
 * RUNTIME TASK CONTROL EXAMPLES
 * ============================================================================ */

/**
 * Example: Enable Ethernet dynamically
 */
void app_enable_ethernet(void) {
    task_handle_t* eth_task = SCHED_FindTask("Ethernet");
    if (eth_task) {
        SCHED_StartTask(eth_task);
        printf("Ethernet task enabled\n");
    }
}

/**
 * Example: Disable Ethernet
 */
void app_disable_ethernet(void) {
    task_handle_t* eth_task = SCHED_FindTask("Ethernet");
    if (eth_task) {
        SCHED_StopTask(eth_task);
        printf("Ethernet task disabled\n");
    }
}

/**
 * Example: Pause reporting temporarily
 */
void app_pause_reporting(void) {
    task_handle_t* stats_task = SCHED_FindTask("Stats_Report");
    if (stats_task) {
        SCHED_PauseTask(stats_task);
        printf("Reporting paused\n");
    }
}

/**
 * Example: Resume reporting
 */
void app_resume_reporting(void) {
    task_handle_t* stats_task = SCHED_FindTask("Stats_Report");
    if (stats_task) {
        SCHED_ResumeTask(stats_task);
        printf("Reporting resumed\n");
    }
}

/**
 * Example: Change GPS polling interval
 */
void app_set_gps_interval(uint32_t ms) {
    task_handle_t* gps_task = SCHED_FindTask("GPS_Process");
    if (gps_task) {
        SCHED_SetTaskInterval(gps_task, ms);
        printf("GPS interval set to %u ms\n", ms);
    }
}

/**
 * Example: Get GPS task statistics
 */
void app_print_gps_stats(void) {
    task_handle_t* gps_task = SCHED_FindTask("GPS_Process");
    if (gps_task) {
        SCHED_PrintTaskInfo(gps_task);
    }
}
