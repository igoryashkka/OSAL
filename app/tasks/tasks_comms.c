#include "tasks_comms.h"
#include "gps_nmea.h"
#include <stdio.h>

/* ============================================================================
 * COMMUNICATION TASKS
 * ============================================================================ */

/**
 * GPS processing task - reads and parses NMEA sentences
 * 
 * Continuously reads data from the GPS module via UART and processes
 * incoming NMEA sentences. Updates GPS position, time, and satellite data.
 * 
 * Priority: HIGH - GPS data is time-critical and must be processed quickly
 * Interval: 100ms - Frequent polling for responsive position updates
 */
uint32_t task_gps_process(task_handle_t* task) {
    (void)task;
    
    /* Read available data from GPS UART */
    uint8_t buffer[32];
    int bytes = GPS_Read(buffer, sizeof(buffer));
    
    /* Process each received byte through NMEA parser */
    for (int i = 0; i < bytes; i++) {
        GPS_ProcessByte(buffer[i]);
    }
    
    return 100;  /* Run every 100ms - frequent polling for GPS */
}

/* ============================================================================
 * PLACEHOLDER TASKS (for future functionality)
 * ============================================================================ */

/**
 * Ethernet task - for network communication
 * TODO: Implement when Ethernet is integrated
 */
// uint32_t task_ethernet(task_handle_t* task) {
//     // eth_process_packets();
//     return 200;
// }

/**
 * Radio task - for wireless communication
 * TODO: Implement when radio module is integrated
 */
// uint32_t task_radio(task_handle_t* task) {
//     // radio_process_messages();
//     return 150;
// }
