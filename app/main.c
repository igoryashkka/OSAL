#include "init/application.h"
#include "tasks/tasks_peripheral.h"
#include "tasks/tasks_comms.h"
#include "tasks/tasks_stats.h"
#include "scheduler.h"
#include <stdio.h>

/**
 * Main application entry point
 * 
 * Program flow:
 * 1. Initialize all platform subsystems and modules
 * 2. Create and start all application tasks
 * 3. Run the task scheduler forever
 * 
 * The actual task definitions are organized in separate files:
 * - tasks_peripheral.c - LED, battery monitoring
 * - tasks_comms.c - GPS, Ethernet, Radio
 * - tasks_stats.c - Statistics, logging, monitoring
 * 
 * Application initialization is in application.c
 */
int main(void) {
    /* Initialize all subsystems */
    app_init();
    
    /* Create and start application tasks */
    app_start_tasks();
    
    /* Run the task scheduler - never returns */
    SCHED_Run();
    
    return 0;
}
