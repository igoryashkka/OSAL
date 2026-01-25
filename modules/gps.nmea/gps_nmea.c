#include "gps_nmea.h"
#include "platform_uart.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

/* Global GPS context */
static gps_context_t g_gps = {0};

/* ============================================================================
 * UTILITY FUNCTIONS
 * ============================================================================ */

/**
 * Calculate checksum for NMEA sentence (XOR of all bytes between $ and *)
 */
static uint8_t gps_calculate_checksum(const char* sentence) {
    uint8_t checksum = 0;
    
    /* Skip '$' */
    if (*sentence == '$') sentence++;
    
    /* XOR all bytes until '*' */
    while (*sentence && *sentence != '*') {
        checksum ^= (uint8_t)*sentence;
        sentence++;
    }
    
    return checksum;
}

/**
 * Verify NMEA checksum
 */
static bool gps_verify_checksum(const char* sentence) {
    const char* asterisk = strchr(sentence, '*');
    if (!asterisk) return false;
    
    uint8_t expected = gps_calculate_checksum(sentence);
    uint8_t received = (uint8_t)strtol(asterisk + 1, NULL, 16);
    
    return expected == received;
}

/**
 * Extract field from comma-separated NMEA sentence
 */
static const char* gps_get_field(const char* sentence, int field_num) {
    static char field_buffer[64];
    int field_count = 0;
    const char* start = sentence;
    
    if (*start == '$') start++;  /* Skip header */
    
    while (field_count < field_num && *start) {
        if (*start == ',') field_count++;
        start++;
    }
    
    if (field_count != field_num) return "";
    
    int len = 0;
    while (len < sizeof(field_buffer) - 1 && *start && *start != ',' && *start != '*') {
        field_buffer[len++] = *start++;
    }
    field_buffer[len] = '\0';
    
    return field_buffer;
}

/**
 * Convert NMEA coordinate (ddmm.mmmm) to decimal degrees
 */
static double gps_parse_coordinate(const char* coord_str, char direction) {
    if (!coord_str || strlen(coord_str) < 5) return 0.0;
    
    /* Find decimal point */
    const char* decimal = strchr(coord_str, '.');
    if (!decimal) return 0.0;
    
    int decimal_pos = decimal - coord_str;
    int degrees_len = decimal_pos - 2;
    
    if (degrees_len < 2) return 0.0;
    
    /* Extract degrees */
    char deg_str[10] = {0};
    strncpy(deg_str, coord_str, degrees_len);
    double degrees = atof(deg_str);
    
    /* Extract minutes */
    char min_str[10] = {0};
    strncpy(min_str, coord_str + degrees_len, decimal_pos - degrees_len);
    strcat(min_str, decimal);
    double minutes = atof(min_str);
    
    /* Convert to decimal degrees */
    double result = degrees + (minutes / 60.0);
    
    /* Apply sign based on direction */
    if (direction == 'S' || direction == 'W') {
        result = -result;
    }
    
    return result;
}

/**
 * Convert knots to m/s
 */
static float gps_knots_to_mps(double knots) {
    return (float)(knots * 0.51444);
}

/* ============================================================================
 * NMEA SENTENCE PARSERS
 * ============================================================================ */

/**
 * Parse RMC (Recommended Minimum Navigation Information)
 * $GPRMC,123519,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A
 */
static void gps_parse_rmc(const char* sentence) {
    g_gps.stats.rmc_count++;
    
    /* Time: field 1 */
    const char* time_str = gps_get_field(sentence, 1);
    if (strlen(time_str) >= 6) {
        g_gps.datetime.hour = (time_str[0] - '0') * 10 + (time_str[1] - '0');
        g_gps.datetime.minute = (time_str[2] - '0') * 10 + (time_str[3] - '0');
        g_gps.datetime.second = (time_str[4] - '0') * 10 + (time_str[5] - '0');
    }
    
    /* Status: field 2 (A=valid, V=invalid) */
    const char* status = gps_get_field(sentence, 2);
    if (*status != 'A') return;  /* Invalid fix */
    
    /* Latitude: field 3 */
    const char* lat = gps_get_field(sentence, 3);
    const char* lat_dir = gps_get_field(sentence, 4);
    if (lat[0]) g_gps.position.latitude = gps_parse_coordinate(lat, lat_dir[0]);
    
    /* Longitude: field 5 */
    const char* lon = gps_get_field(sentence, 5);
    const char* lon_dir = gps_get_field(sentence, 6);
    if (lon[0]) g_gps.position.longitude = gps_parse_coordinate(lon, lon_dir[0]);
    
    /* Speed: field 7 (knots) */
    const char* speed = gps_get_field(sentence, 7);
    if (speed[0]) g_gps.position.speed_knots = atof(speed);
    
    /* Course: field 8 */
    const char* course = gps_get_field(sentence, 8);
    if (course[0]) g_gps.position.course = atof(course);
    
    /* Date: field 9 (ddmmyy) */
    const char* date_str = gps_get_field(sentence, 9);
    if (strlen(date_str) >= 6) {
        g_gps.datetime.day = (date_str[0] - '0') * 10 + (date_str[1] - '0');
        g_gps.datetime.month = (date_str[2] - '0') * 10 + (date_str[3] - '0');
        g_gps.datetime.year = 2000 + (date_str[4] - '0') * 10 + (date_str[5] - '0');
    }
    
    g_gps.stats.valid_fixes++;
}

/**
 * Parse GGA (Global Positioning System Fix Data)
 * $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
 */
static void gps_parse_gga(const char* sentence) {
    g_gps.stats.gga_count++;
    
    /* Time: field 1 */
    const char* time_str = gps_get_field(sentence, 1);
    if (strlen(time_str) >= 6) {
        g_gps.datetime.hour = (time_str[0] - '0') * 10 + (time_str[1] - '0');
        g_gps.datetime.minute = (time_str[2] - '0') * 10 + (time_str[3] - '0');
        g_gps.datetime.second = (time_str[4] - '0') * 10 + (time_str[5] - '0');
    }
    
    /* Latitude: field 2 */
    const char* lat = gps_get_field(sentence, 2);
    const char* lat_dir = gps_get_field(sentence, 3);
    if (lat[0]) g_gps.position.latitude = gps_parse_coordinate(lat, lat_dir[0]);
    
    /* Longitude: field 4 */
    const char* lon = gps_get_field(sentence, 4);
    const char* lon_dir = gps_get_field(sentence, 5);
    if (lon[0]) g_gps.position.longitude = gps_parse_coordinate(lon, lon_dir[0]);
    
    /* Fix Quality: field 6 */
    const char* fix_type = gps_get_field(sentence, 6);
    if (fix_type[0]) {
        g_gps.position.fix_type = (gps_fix_type_t)atoi(fix_type);
        if (g_gps.position.fix_type > 0) {
            g_gps.stats.valid_fixes++;
        }
    }
    
    /* Number of Satellites: field 7 */
    const char* num_sats = gps_get_field(sentence, 7);
    if (num_sats[0]) g_gps.position.num_satellites = (uint8_t)atoi(num_sats);
    
    /* HDOP: field 8 */
    const char* hdop = gps_get_field(sentence, 8);
    if (hdop[0]) g_gps.position.hdop = (uint8_t)(atof(hdop) * 10);
    
    /* Altitude: field 9 */
    const char* alt = gps_get_field(sentence, 9);
    if (alt[0]) g_gps.position.altitude = atof(alt);
}

/**
 * Parse GSA (GPS DOP and active satellites)
 * $GPGSA,A,3,04,05,,09,12,,,24,,,,,2.5,1.3,2.1*30
 */
static void gps_parse_gsa(const char* sentence) {
    g_gps.stats.gsa_count++;
    
    /* Mode: field 1 (A=auto, M=manual) */
    const char* mode = gps_get_field(sentence, 1);
    
    /* Fix Type: field 2 (1=no fix, 2=2D, 3=3D) */
    const char* fix_type = gps_get_field(sentence, 2);
    if (fix_type[0] == '3') {
        g_gps.position.fix_type = FIX_GPS;
    } else if (fix_type[0] == '2') {
        g_gps.position.fix_type = FIX_GPS;
    }
    
    /* HDOP: field 16 */
    const char* hdop = gps_get_field(sentence, 16);
    if (hdop[0]) g_gps.position.hdop = (uint8_t)(atof(hdop) * 10);
}

/**
 * Parse GSV (GPS Satellites in view)
 * $GPGSV,2,1,08,01,40,083,46,02,17,308,41,12,07,344,39,14,22,228,45*75
 */
static void gps_parse_gsv(const char* sentence) {
    g_gps.stats.gsv_count++;
    
    /* Message: field 1 and 2 (msg_num / total_msgs) */
    const char* msg_num_str = gps_get_field(sentence, 2);
    int msg_num = atoi(msg_num_str);
    
    /* Each GSV can have up to 4 satellite entries (fields 3-6, 7-10, etc.) */
    int base_field = 3;
    int sat_offset = 0;
    
    for (int i = 0; i < 4; i++) {
        const char* prn = gps_get_field(sentence, base_field + i * 4);
        if (!prn[0]) break;  /* No more satellites in this message */
        
        int sat_idx = (msg_num - 1) * 4 + i;
        if (sat_idx >= 12) break;  /* Max 12 satellites */
        
        g_gps.satellites[sat_idx].prn = (uint8_t)atoi(prn);
        
        const char* elev = gps_get_field(sentence, base_field + i * 4 + 1);
        if (elev[0]) g_gps.satellites[sat_idx].elevation = (uint8_t)atoi(elev);
        
        const char* azim = gps_get_field(sentence, base_field + i * 4 + 2);
        if (azim[0]) g_gps.satellites[sat_idx].azimuth = (uint16_t)atoi(azim);
        
        const char* snr = gps_get_field(sentence, base_field + i * 4 + 3);
        if (snr[0]) g_gps.satellites[sat_idx].snr = (uint8_t)atoi(snr);
    }
}

/* ============================================================================
 * MAIN PROCESSING
 * ============================================================================ */

/**
 * Process complete NMEA sentence
 */
void GPS_ProcessSentence(const char* sentence) {
    if (!sentence || strlen(sentence) < 7) {
        g_gps.stats.parse_errors++;
        return;
    }
    
    g_gps.stats.total_sentences++;
    
    /* Verify checksum */
    if (!gps_verify_checksum(sentence)) {
        g_gps.stats.checksum_errors++;
        g_gps.stats.invalid_sentences++;
        return;
    }
    
    g_gps.stats.valid_sentences++;
    
    /* Extract sentence type (3 characters after $) */
    const char* start = sentence;
    if (*start == '$') start++;
    
    char talker[3] = {0};  /* GP = GPS, GL = GLONASS, GN = Multi, etc. */
    char formatter[4] = {0};
    
    if (strlen(start) >= 6) {
        talker[0] = start[0];
        talker[1] = start[1];
        formatter[0] = start[2];
        formatter[1] = start[3];
        formatter[2] = start[4];
    }
    
    /* Route to appropriate parser */
    if (strncmp(formatter, "RMC", 3) == 0) {
        gps_parse_rmc(sentence);
    } else if (strncmp(formatter, "GGA", 3) == 0) {
        gps_parse_gga(sentence);
    } else if (strncmp(formatter, "GSA", 3) == 0) {
        gps_parse_gsa(sentence);
    } else if (strncmp(formatter, "GSV", 3) == 0) {
        gps_parse_gsv(sentence);
    }
}

/**
 * Process incoming byte from UART
 */
void GPS_ProcessByte(uint8_t byte) {
    /* Start of sentence */
    if (byte == '$') {
        g_gps.rx_index = 0;
        g_gps.sentence_complete = false;
        g_gps.rx_buffer[g_gps.rx_index++] = byte;
        return;
    }
    
    /* End of sentence */
    if (byte == '\n' || byte == '\r') {
        if (g_gps.rx_index > 0) {
            g_gps.rx_buffer[g_gps.rx_index] = '\0';
            g_gps.sentence_complete = true;
            
            /* Process if we have a complete sentence with checksum */
            if (strchr((char*)g_gps.rx_buffer, '*')) {
                GPS_ProcessSentence((char*)g_gps.rx_buffer);
            }
            
            g_gps.rx_index = 0;
        }
        return;
    }
    
    /* Accumulate sentence */
    if (g_gps.rx_index < sizeof(g_gps.rx_buffer) - 1) {
        g_gps.rx_buffer[g_gps.rx_index++] = byte;
    } else {
        /* Buffer overflow - reset */
        g_gps.rx_index = 0;
        g_gps.stats.parse_errors++;
    }
}

/**
 * Read raw data from UART
 */
int GPS_Read(uint8_t* buffer, int max_len) {
    return Platform_UART_Read(buffer, max_len);
}

/* ============================================================================
 * INITIALIZATION
 * ============================================================================ */

void GPS_Init(uint32_t uart_baud) {
    memset(&g_gps, 0, sizeof(gps_context_t));
    Platform_UART_Init(uart_baud);
    
    /* Default fix type */
    g_gps.position.fix_type = FIX_INVALID;
}

/* ============================================================================
 * DATA GETTERS
 * ============================================================================ */

gps_position_t GPS_GetPosition(void) {
    return g_gps.position;
}

gps_datetime_t GPS_GetDateTime(void) {
    return g_gps.datetime;
}

gps_stats_t GPS_GetStats(void) {
    return g_gps.stats;
}

bool GPS_IsFixed(void) {
    return g_gps.position.fix_type > FIX_INVALID;
}

uint8_t GPS_GetSatelliteCount(void) {
    return g_gps.position.num_satellites;
}

gps_satellite_t GPS_GetSatellite(uint8_t index) {
    if (index < 12) {
        return g_gps.satellites[index];
    }
    return (gps_satellite_t){0};
}

/* ============================================================================
 * STATISTICS
 * ============================================================================ */

void GPS_ResetStats(void) {
    g_gps.stats = (gps_stats_t){0};
}

float GPS_GetFixRate(void) {
    if (g_gps.stats.total_sentences == 0) return 0.0f;
    return (float)g_gps.stats.valid_fixes * 100.0f / g_gps.stats.total_sentences;
}

float GPS_GetValidRate(void) {
    if (g_gps.stats.total_sentences == 0) return 0.0f;
    return (float)g_gps.stats.valid_sentences * 100.0f / g_gps.stats.total_sentences;
}

const char* GPS_GetFixTypeStr(gps_fix_type_t type) {
    switch (type) {
        case FIX_INVALID:      return "INVALID";
        case FIX_GPS:          return "GPS";
        case FIX_DGPS:         return "DGPS";
        case FIX_PPS:          return "PPS";
        case FIX_RTK:          return "RTK";
        case FIX_FLOAT_RTK:    return "FLOAT RTK";
        case FIX_ESTIMATED:    return "ESTIMATED";
        case FIX_MANUAL:       return "MANUAL";
        case FIX_SIMULATION:   return "SIMULATION";
        default:               return "UNKNOWN";
    }
}

void GPS_PrintStats(void) {
    printf("\n=== GPS STATISTICS ===\n");
    printf("Total Sentences:    %lu\n", g_gps.stats.total_sentences);
    printf("Valid Sentences:    %lu (%.1f%%)\n", 
           g_gps.stats.valid_sentences, GPS_GetValidRate());
    printf("Invalid Sentences:  %lu\n", g_gps.stats.invalid_sentences);
    printf("Valid Fixes:        %lu (%.1f%%)\n", 
           g_gps.stats.valid_fixes, GPS_GetFixRate());
    printf("Checksum Errors:    %lu\n", g_gps.stats.checksum_errors);
    printf("Parse Errors:       %lu\n", g_gps.stats.parse_errors);
    printf("\nSentence Types:\n");
    printf("  RMC: %lu\n", g_gps.stats.rmc_count);
    printf("  GGA: %lu\n", g_gps.stats.gga_count);
    printf("  GSA: %lu\n", g_gps.stats.gsa_count);
    printf("  GSV: %lu\n", g_gps.stats.gsv_count);
    printf("\nPosition Data:\n");
    printf("  Latitude:  %.6f°\n", g_gps.position.latitude);
    printf("  Longitude: %.6f°\n", g_gps.position.longitude);
    printf("  Altitude:  %.1f m\n", g_gps.position.altitude);
    printf("  Speed:     %.1f knots (%.2f m/s)\n", 
           g_gps.position.speed_knots, gps_knots_to_mps(g_gps.position.speed_knots));
    printf("  Course:    %.1f°\n", g_gps.position.course);
    printf("  Satellites: %u (HDOP: %.1f)\n", 
           g_gps.position.num_satellites, g_gps.position.hdop / 10.0f);
    printf("  Fix Type:  %s\n", GPS_GetFixTypeStr(g_gps.position.fix_type));
    printf("  Date/Time: %04u-%02u-%02u %02u:%02u:%02u\n",
           g_gps.datetime.year, g_gps.datetime.month, g_gps.datetime.day,
           g_gps.datetime.hour, g_gps.datetime.minute, g_gps.datetime.second);
    printf("====================\n\n");
}
