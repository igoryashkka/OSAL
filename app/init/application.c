#include "application.h"
#include "module_system.h"
#include "module_blink.h"
#include "scheduler.h"
#include "gps_nmea.h"
#include <stdio.h>

/* Forward declarations of task functions */
extern uint32_t task_led_blink(task_handle_t* task);
extern uint32_t task_gps_process(task_handle_t* task);
extern uint32_t task_battery_monitor(task_handle_t* task);
extern uint32_t task_statistics(task_handle_t* task);

/**
 * Initialize all application subsystems
 */
void app_init(void) {
    printf("╔═══════════════════════════════════════════════════╗\n");
    printf("║   OSAL Application - Initialization              ║\n");
    printf("╚═══════════════════════════════════════════════════╝\n\n");
    
    /* Platform initialization */
    printf("[1/4] Initializing platform...\n");
    init_system();
    blink_init();
    
    /* Initialize task scheduler */
    printf("[2/4] Initializing task scheduler...\n");
    SCHED_Init();
    
    /* Initialize GPS */
    printf("[3/4] Initializing GPS module...\n");
    GPS_Init(9600);
    
    printf("[4/4] All subsystems initialized!\n\n");
}

/**
 * Create and start all application tasks
 */
void app_start_tasks(void) {
    printf("Creating application tasks...\n");
    
    /* Create tasks */
    task_handle_t* t_led = SCHED_CreateTask(
        "LED_Blink",
        task_led_blink,
        TASK_PRIORITY_NORMAL,
        500,
        NULL
    );
    
    task_handle_t* t_gps = SCHED_CreateTask(
        "GPS_Process",
        task_gps_process,
        TASK_PRIORITY_HIGH,     /* GPS is important */
        100,
        NULL
    );
    
    task_handle_t* t_battery = SCHED_CreateTask(
        "Battery_Monitor",
        task_battery_monitor,
        TASK_PRIORITY_NORMAL,
        5000,
        NULL
    );
    
    task_handle_t* t_stats = SCHED_CreateTask(
        "Statistics",
        task_statistics,
        TASK_PRIORITY_LOW,
        10000,
        NULL
    );
    
    /* Start tasks */
    SCHED_StartTask(t_led);
    SCHED_StartTask(t_gps);
    SCHED_StartTask(t_battery);
    SCHED_StartTask(t_stats);
    
    printf("Tasks created and started:\n");
    printf("  ✓ LED_Blink (500ms, NORMAL priority)\n");
    printf("  ✓ GPS_Process (100ms, HIGH priority)\n");
    printf("  ✓ Battery_Monitor (5s, NORMAL priority)\n");
    printf("  ✓ Statistics (10s, LOW priority)\n");
    printf("\n");
}
