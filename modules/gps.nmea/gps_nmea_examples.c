/**
 * GPS NMEA Module - Example Usage
 * 
 * This demonstrates how to use the GPS NMEA parser module with the NEO-6M GPS receiver.
 * The module uses Platform_UART API for MCU-agnostic UART communication.
 */

#include "gps_nmea.h"
#include "platform_uart.h"
#include <stdio.h>

/* ============================================================================
 * Example: Main application loop with GPS parsing
 * ============================================================================ */

void gps_app_example(void) {
    /* Initialize GPS (9600 baud is standard for NEO-6M) */
    GPS_Init(9600);
    
    /* Main loop - read from UART and process bytes */
    while (1) {
        uint8_t rx_buffer[32];
        int bytes_read = GPS_Read(rx_buffer, sizeof(rx_buffer));
        
        /* Process each received byte through the NMEA parser */
        for (int i = 0; i < bytes_read; i++) {
            GPS_ProcessByte(rx_buffer[i]);
        }
        
        /* Periodically log statistics */
        static int loop_count = 0;
        if (++loop_count >= 10000) {
            GPS_PrintStats();
            loop_count = 0;
        }
    }
}

/* ============================================================================
 * Example: Interrupt-driven GPS processing (UART RX ISR)
 * ============================================================================ */

/* This would be called from your UART interrupt handler */
void UART_RxISR_Handler(uint8_t byte) {
    GPS_ProcessByte(byte);
}

/* ============================================================================
 * Example: Querying GPS data
 * ============================================================================ */

void gps_query_example(void) {
    /* Get current position */
    gps_position_t pos = GPS_GetPosition();
    
    printf("Current Position:\n");
    printf("  Latitude:  %.6f°\n", pos.latitude);
    printf("  Longitude: %.6f°\n", pos.longitude);
    printf("  Altitude:  %.1f m\n", pos.altitude);
    printf("  Satellites: %u\n", pos.num_satellites);
    printf("  Fix Type: %s\n", GPS_GetFixTypeStr(pos.fix_type));
    
    if (GPS_IsFixed()) {
        printf("  Speed: %.1f knots\n", pos.speed_knots);
        printf("  Course: %.1f°\n", pos.course);
        printf("  HDOP: %.1f\n", pos.hdop / 10.0f);
    }
    
    /* Get date/time */
    gps_datetime_t dt = GPS_GetDateTime();
    printf("\nDate/Time: %04u-%02u-%02u %02u:%02u:%02u\n",
           dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
    
    /* Get satellite details */
    printf("\nTracked Satellites:\n");
    for (int i = 0; i < GPS_GetSatelliteCount(); i++) {
        gps_satellite_t sat = GPS_GetSatellite(i);
        if (sat.prn > 0) {
            printf("  PRN %2u: Elev %3u° Azim %3u° SNR %2u dB-Hz\n",
                   sat.prn, sat.elevation, sat.azimuth, sat.snr);
        }
    }
}

/* ============================================================================
 * Example: Statistics monitoring
 * ============================================================================ */

void gps_monitor_example(void) {
    gps_stats_t stats = GPS_GetStats();
    
    printf("\nGPS Statistics:\n");
    printf("  Total Sentences: %u\n", stats.total_sentences);
    printf("  Valid Rate: %.1f%%\n", GPS_GetValidRate());
    printf("  Fix Rate: %.1f%%\n", GPS_GetFixRate());
    printf("  Checksum Errors: %u\n", stats.checksum_errors);
    printf("  Parse Errors: %u\n", stats.parse_errors);
    printf("  Sentence Types:\n");
    printf("    RMC: %u\n", stats.rmc_count);
    printf("    GGA: %u\n", stats.gga_count);
    printf("    GSA: %u\n", stats.gsa_count);
    printf("    GSV: %u\n", stats.gsv_count);
}

/* ============================================================================
 * Example: Integrating into main.c
 * ============================================================================ */

#if 0  /* Pseudocode for integration into app/main.c */

#include "app.h"
#include "gps_nmea.h"
#include "module_blink.h"

int main(void) {
    /* Initialize platform */
    Platform_Init();
    
    /* Initialize GPS module (9600 baud for NEO-6M) */
    GPS_Init(9600);
    
    /* Blink LED to show system running */
    BLINK_Init();
    
    /* Main application loop */
    while (1) {
        /* Read and parse GPS data */
        uint8_t rx_buffer[32];
        int bytes_read = GPS_Read(rx_buffer, sizeof(rx_buffer));
        
        for (int i = 0; i < bytes_read; i++) {
            GPS_ProcessByte(rx_buffer[i]);
        }
        
        /* Check GPS fix status every 1000 iterations */
        static int count = 0;
        if (++count >= 1000) {
            count = 0;
            
            if (GPS_IsFixed()) {
                BLINK_SetRate(100);  /* Fast blink when fixed */
                gps_position_t pos = GPS_GetPosition();
                printf("Fixed! Lat: %.6f Lon: %.6f\n", pos.latitude, pos.longitude);
            } else {
                BLINK_SetRate(500);  /* Slow blink when searching */
                printf("Searching for fix...\n");
            }
        }
    }
    
    return 0;
}

#endif

/* ============================================================================
 * NEO-6M Configuration Notes
 * ============================================================================ */

/*
 * Default Settings:
 * - Baud Rate: 9600
 * - Data Bits: 8
 * - Stop Bits: 1
 * - Parity: None
 * - Flow Control: None
 * - Default Update Rate: 1 Hz
 * - Default Sentences: GGA, RMC, GSV, GSA
 * 
 * Key Features:
 * - Concurrent reception of GPS, SBAS, and QZSS
 * - Up to 22 acquisition channels, 12 parallel tracking channels
 * - Cold start: ~45 seconds
 * - Warm start: ~5 seconds
 * - Hot start: ~1 second
 * - Typical accuracy: <2.5m (without DGPS)
 * - Velocity accuracy: 0.05 m/s
 * 
 * NMEA Sentences Commonly Output:
 * - GGA: Global Positioning System Fix Data (position, altitude, fix quality)
 * - RMC: Recommended Minimum Navigation Information (position, speed, course, date/time)
 * - GSA: GPS DOP and Active Satellites (fix type, HDOP, VDOP, PDOP)
 * - GSV: GPS Satellites in View (satellite signal strengths)
 * 
 * Typical Sentence Output Sequence:
 * $GPGGA,...  <- Position and altitude
 * $GPGSA,...  <- Fix type and DOP values
 * $GPGSV,...  <- First satellite set
 * $GPGSV,...  <- Second satellite set
 * $GPRMC,...  <- Position, speed, course, date/time
 * 
 * Connection Diagram (NEO-6M to MCU):
 * NEO-6M RX (pin 0)  <-- MCU TX (UART TX)
 * NEO-6M TX (pin 1)  --> MCU RX (UART RX)
 * NEO-6M VCC (pin 4) <-- +3.3V or 5V
 * NEO-6M GND (pin 5) --- GND
 */
