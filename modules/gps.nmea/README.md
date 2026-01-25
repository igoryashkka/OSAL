# GPS NMEA Module

Complete NMEA 0183 protocol parser for **NEO-6M** and compatible GPS receivers. Provides position tracking, date/time, satellite information, and comprehensive statistics.

## Features

### Data Tracking
- **Position**: Latitude, Longitude, Altitude (WGS84)
- **Motion**: Speed (knots), Course/Heading
- **Date/Time**: UTC with hour, minute, second, day, month, year
- **Satellites**: Count, signal strength (SNR), elevation, azimuth
- **Quality Metrics**: Fix type, HDOP (horizontal dilution of precision)

### Statistics
- Sentence validation and checksum verification
- Per-sentence-type counters (GGA, RMC, GSA, GSV)
- Valid/invalid sentence tracking
- Parse error and checksum error logging
- Fix rate and validity rate calculations

### Supported NMEA Sentences
- **GGA**: Global Positioning System Fix Data
- **RMC**: Recommended Minimum Navigation Information
- **GSA**: GPS DOP and Active Satellites
- **GSV**: GPS Satellites in View

## Architecture

Uses **Platform_UART** API for MCU-agnostic communication - no direct HAL dependencies.

### Platform API Integration
```c
/* Implemented by platform layer */
void Platform_UART_Init(uint32_t baud);      /* Initialize UART (9600 for NEO-6M) */
int  Platform_UART_Read(uint8_t* buf, int len); /* Non-blocking read */
int  Platform_UART_Write(const uint8_t* data, int len); /* Write data */
```

## API Reference

### Initialization
```c
void GPS_Init(uint32_t uart_baud);  /* 9600 for NEO-6M */
```

### Data Processing
```c
void GPS_ProcessByte(uint8_t byte);              /* Call from UART RX ISR or polling loop */
void GPS_ProcessSentence(const char* sentence);  /* Direct sentence processing (testing) */
int  GPS_Read(uint8_t* buffer, int max_len);    /* Read from UART */
```

### Data Getters
```c
gps_position_t GPS_GetPosition(void);    /* Current position, speed, course */
gps_datetime_t GPS_GetDateTime(void);    /* Current date/time */
gps_stats_t    GPS_GetStats(void);       /* Statistics counters */
bool           GPS_IsFixed(void);        /* Has valid fix? */
uint8_t        GPS_GetSatelliteCount(void);
gps_satellite_t GPS_GetSatellite(uint8_t index);
```

### Statistics
```c
void  GPS_ResetStats(void);              /* Clear counters */
float GPS_GetFixRate(void);              /* % of valid fixes */
float GPS_GetValidRate(void);            /* % of valid sentences */
void  GPS_PrintStats(void);              /* Print formatted report */
```

### Utilities
```c
const char* GPS_GetFixTypeStr(gps_fix_type_t type);  /* Human-readable fix type */
```

## Data Structures

### Position
```c
typedef struct {
    double latitude;        /* degrees */
    double longitude;       /* degrees */
    double altitude;        /* meters above MSL */
    double speed_knots;     /* speed in knots */
    double course;          /* track/course in degrees */
    uint8_t num_satellites; /* number of satellites in use */
    uint8_t hdop;           /* HDOP x 10 (e.g., 15 = 1.5) */
    gps_fix_type_t fix_type;/* GPS fix quality */
} gps_position_t;
```

### Date/Time
```c
typedef struct {
    uint8_t hour, minute, second;
    uint16_t millisecond;
    uint8_t day, month;
    uint16_t year;
} gps_datetime_t;
```

### Statistics
```c
typedef struct {
    uint32_t total_sentences;    /* All received */
    uint32_t valid_sentences;    /* Checksum OK */
    uint32_t invalid_sentences;  /* Checksum failed */
    uint32_t gga_count;          /* GGA messages */
    uint32_t rmc_count;          /* RMC messages */
    uint32_t gsa_count;          /* GSA messages */
    uint32_t gsv_count;          /* GSV messages */
    uint32_t checksum_errors;
    uint32_t parse_errors;
    uint32_t valid_fixes;        /* Messages with valid position */
} gps_stats_t;
```

### Fix Types
```c
typedef enum {
    FIX_INVALID = 0,
    FIX_GPS = 1,          /* 2D/3D GPS fix */
    FIX_DGPS = 2,         /* Differential GPS */
    FIX_PPS = 3,          /* PPS fix */
    FIX_RTK = 4,          /* Real-time kinematic */
    FIX_FLOAT_RTK = 5,    /* Float RTK */
    FIX_ESTIMATED = 6,    /* Estimated */
    FIX_MANUAL = 7,       /* Manual */
    FIX_SIMULATION = 8    /* Simulation */
} gps_fix_type_t;
```

## Usage Example

### Polling Mode (main loop)
```c
int main(void) {
    GPS_Init(9600);
    
    while (1) {
        uint8_t buffer[32];
        int n = GPS_Read(buffer, sizeof(buffer));
        
        for (int i = 0; i < n; i++) {
            GPS_ProcessByte(buffer[i]);
        }
        
        if (GPS_IsFixed()) {
            gps_position_t pos = GPS_GetPosition();
            printf("Lat: %.6f, Lon: %.6f, Alt: %.1f m\n",
                   pos.latitude, pos.longitude, pos.altitude);
        }
    }
}
```

### Interrupt Mode (UART RX ISR)
```c
/* In your UART RX interrupt handler */
void USART1_IRQHandler(void) {
    if (LL_USART_IsActiveFlag_RXNE(USART1)) {
        uint8_t byte = LL_USART_ReceiveData8(USART1);
        GPS_ProcessByte(byte);  /* Direct byte processing */
    }
}
```

## NEO-6M Module Specifications

| Parameter | Value |
|-----------|-------|
| Default Baud | 9600 |
| Data Bits | 8 |
| Stop Bits | 1 |
| Parity | None |
| Update Rate | 1 Hz (adjustable) |
| Cold Start | ~45 seconds |
| Warm Start | ~5 seconds |
| Hot Start | ~1 second |
| Accuracy | < 2.5 m (without DGPS) |
| Velocity Accuracy | 0.05 m/s |
| Tracking Channels | 12 parallel |
| Acquisition Channels | 22 |

## Wiring (NEO-6M to STM32)

```
NEO-6M          STM32
------          -----
TX (pin 1)  --> RX (UART2 or UART1)
RX (pin 0)  <-- TX (UART2 or UART1)
VCC (pin 4) <-- 3.3V or 5V
GND (pin 5) --- GND
```

## Implementation Notes

1. **Thread-Safe**: Single-threaded design; use mutex if calling from multiple threads
2. **Memory**: ~1KB for context structure, ~256 bytes for RX buffer
3. **Checksum**: All sentences are verified with XOR checksum
4. **Overflow**: Old satellite data is retained; only updated on new GSV messages
5. **Coordinate Format**: Input as NMEA (ddmm.mmmm), output as decimal degrees
6. **Units**: Internal meters/degrees/seconds; knots also available

## Troubleshooting

### No Data Received
- Check UART baud rate (default: 9600)
- Verify TX/RX pins are connected
- Check voltage levels (3.3V logic)
- Add pull-up resistor on TX line if needed

### All Checksum Errors
- Verify UART data line is not corrupted
- Check for noise on communication lines
- Reduce baud rate if needed
- Verify data format matches NMEA 0183

### No Fix
- GPS needs clear sky view (works indoors with difficulty)
- Wait for cold start (~45s first time)
- Check antenna connection
- Verify power supply voltage and current

## See Also
- `gps_nmea_examples.c` - Complete usage examples
- NEO-6M datasheet for detailed NMEA sentence formats
- NMEA 0183 specification for standard definitions
