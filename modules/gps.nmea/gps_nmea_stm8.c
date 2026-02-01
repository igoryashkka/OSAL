#include "gps_nmea.h"
#include <string.h>

/* Minimal STM8 stubs for GPS API to satisfy linking on STM8 builds */

void GPS_Init(uint32_t uart_baud) {
    (void)uart_baud;
    GPS_ResetStats();
}

void GPS_ProcessByte(uint8_t byte) {
    (void)byte;
}

void GPS_ProcessSentence(const char* sentence) {
    (void)sentence;
}

int GPS_Read(uint8_t* buffer, int max_len) {
    (void)buffer;
    (void)max_len;
    return 0;
}

gps_position_t GPS_GetPosition(void) {
    gps_position_t p;
    memset(&p, 0, sizeof(p));
    return p;
}

gps_datetime_t GPS_GetDateTime(void) {
    gps_datetime_t d;
    memset(&d, 0, sizeof(d));
    return d;
}

gps_stats_t GPS_GetStats(void) {
    gps_stats_t s;
    memset(&s, 0, sizeof(s));
    return s;
}

bool GPS_IsFixed(void) {
    return false;
}

uint8_t GPS_GetSatelliteCount(void) {
    return 0;
}

gps_satellite_t GPS_GetSatellite(uint8_t index) {
    gps_satellite_t sat;
    (void)index;
    memset(&sat, 0, sizeof(sat));
    return sat;
}

void GPS_ResetStats(void) {
}

float GPS_GetFixRate(void) {
    return 0.0f;
}

float GPS_GetValidRate(void) {
    return 0.0f;
}

const char* GPS_GetFixTypeStr(gps_fix_type_t type) {
    (void)type;
    return "NONE";
}

void GPS_PrintStats(void) {
}

/* Minimal putchar for retargeting stdio on STM8 */
int putchar(int c) {
    (void)c;
    return c;
}
