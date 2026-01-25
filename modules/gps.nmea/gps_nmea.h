#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * NEO-6M GPS NMEA Protocol Module
 * 
 * Parses NMEA 0183 sentences from NEO-6M module and maintains statistics.
 * Common sentences: RMC, GGA, GSA, GSV
 */

/* GPS Fix Quality */
typedef enum {
    FIX_INVALID = 0,
    FIX_GPS = 1,
    FIX_DGPS = 2,
    FIX_PPS = 3,
    FIX_RTK = 4,
    FIX_FLOAT_RTK = 5,
    FIX_ESTIMATED = 6,
    FIX_MANUAL = 7,
    FIX_SIMULATION = 8
} gps_fix_type_t;

/* GPS Position Data */
typedef struct {
    double latitude;        /* degrees */
    double longitude;       /* degrees */
    double altitude;        /* meters above MSL */
    double speed_knots;     /* speed in knots */
    double course;          /* track/course in degrees */
    uint8_t num_satellites; /* number of satellites in use */
    uint8_t hdop;           /* horizontal dilution of precision (scaled x10) */
    gps_fix_type_t fix_type;/* GPS fix quality */
} gps_position_t;

/* GPS Time/Date */
typedef struct {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint16_t millisecond;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} gps_datetime_t;

/* GPS Statistics */
typedef struct {
    uint32_t total_sentences;      /* total sentences received */
    uint32_t valid_sentences;      /* valid checksum sentences */
    uint32_t invalid_sentences;    /* invalid checksum */
    uint32_t gga_count;            /* GGA sentence count */
    uint32_t rmc_count;            /* RMC sentence count */
    uint32_t gsa_count;            /* GSA sentence count */
    uint32_t gsv_count;            /* GSV sentence count */
    uint32_t checksum_errors;      /* checksum failures */
    uint32_t parse_errors;         /* parsing failures */
    uint32_t valid_fixes;          /* fixes with valid position */
} gps_stats_t;

/* Satellite Info */
typedef struct {
    uint8_t prn;                   /* satellite PRN number */
    uint8_t elevation;             /* elevation angle 0-90 degrees */
    uint16_t azimuth;              /* azimuth 0-359 degrees */
    uint8_t snr;                   /* signal-to-noise ratio dB-Hz */
} gps_satellite_t;

/* Main GPS context */
typedef struct {
    gps_position_t position;
    gps_datetime_t datetime;
    gps_stats_t stats;
    gps_satellite_t satellites[12]; /* NEO-6M can track up to 12 */
    uint8_t num_satellites_tracked;
    
    /* Internal buffer for NMEA sentence parsing */
    uint8_t rx_buffer[256];
    uint16_t rx_index;
    bool sentence_complete;
} gps_context_t;

/* === Initialization === */
void GPS_Init(uint32_t uart_baud);

/* === Data Processing === */
void GPS_ProcessByte(uint8_t byte);
void GPS_ProcessSentence(const char* sentence);
int  GPS_Read(uint8_t* buffer, int max_len);

/* === Data Getters === */
gps_position_t GPS_GetPosition(void);
gps_datetime_t GPS_GetDateTime(void);
gps_stats_t GPS_GetStats(void);
bool GPS_IsFixed(void);
uint8_t GPS_GetSatelliteCount(void);
gps_satellite_t GPS_GetSatellite(uint8_t index);

/* === Statistics === */
void GPS_ResetStats(void);
float GPS_GetFixRate(void);          /* percentage of valid fixes */
float GPS_GetValidRate(void);        /* percentage of valid sentences */

/* === Utility === */
const char* GPS_GetFixTypeStr(gps_fix_type_t type);
void GPS_PrintStats(void);

#ifdef __cplusplus
}
#endif
