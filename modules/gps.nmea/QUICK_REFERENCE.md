# GPS NMEA Module - Quick Reference

## 🚀 Quick Start

### 1. Initialize
```c
#include "gps_nmea.h"

int main(void) {
    GPS_Init(9600);  // 9600 baud for NEO-6M
    // ...
}
```

### 2. Feed Data (Polling)
```c
while (1) {
    uint8_t buf[32];
    int n = GPS_Read(buf, sizeof(buf));
    for (int i = 0; i < n; i++) {
        GPS_ProcessByte(buf[i]);
    }
}
```

### 3. Or Feed Data (ISR)
```c
void UART_RxISR(uint8_t byte) {
    GPS_ProcessByte(byte);
}
```

### 4. Use Data
```c
if (GPS_IsFixed()) {
    gps_position_t pos = GPS_GetPosition();
    printf("Lat: %.6f°, Lon: %.6f°\n", pos.latitude, pos.longitude);
}
```

## 📡 Data Structures

### Position
```c
gps_position_t {
    .latitude        // degrees (e.g., 48.1339)
    .longitude       // degrees (e.g., 11.5820)
    .altitude        // meters
    .speed_knots     // nautical miles/hour
    .course          // degrees (0-359)
    .num_satellites  // count
    .hdop            // dilution x10 (e.g., 15 = 1.5)
    .fix_type        // FIX_GPS, FIX_DGPS, FIX_RTK, etc.
}
```

### Date/Time
```c
gps_datetime_t {
    .year   // e.g., 2026
    .month  // 1-12
    .day    // 1-31
    .hour   // 0-23 (UTC)
    .minute // 0-59
    .second // 0-59
}
```

### Statistics
```c
gps_stats_t {
    .total_sentences    // All received
    .valid_sentences    // Checksum OK
    .invalid_sentences  // Checksum failed
    .gga_count          // GGA messages
    .rmc_count          // RMC messages
    .gsa_count          // GSA messages
    .gsv_count          // GSV messages
    .checksum_errors
    .parse_errors
    .valid_fixes        // Position updates
}
```

## 🎯 API Functions

### Core
| Function | Purpose |
|----------|---------|
| `GPS_Init(baud)` | Initialize with baud rate |
| `GPS_ProcessByte(byte)` | Feed single byte to parser |
| `GPS_ProcessSentence(str)` | Feed complete NMEA sentence |
| `GPS_Read(buf, len)` | Read from Platform_UART |

### Data Getters
| Function | Returns |
|----------|---------|
| `GPS_GetPosition()` | Current position/speed/course |
| `GPS_GetDateTime()` | Current date and time |
| `GPS_GetStats()` | Statistics counters |
| `GPS_IsFixed()` | Has valid position? |
| `GPS_GetSatelliteCount()` | Number of satellites |
| `GPS_GetSatellite(i)` | Satellite by index |

### Statistics
| Function | Returns |
|----------|---------|
| `GPS_GetFixRate()` | % with valid fixes |
| `GPS_GetValidRate()` | % with valid checksum |
| `GPS_ResetStats()` | Clear all counters |
| `GPS_PrintStats()` | Formatted report to stdout |

### Utilities
| Function | Returns |
|----------|---------|
| `GPS_GetFixTypeStr(type)` | "GPS", "DGPS", "RTK", etc. |

## 📊 Fix Types
```
FIX_INVALID    = 0   No fix
FIX_GPS        = 1   2D/3D GPS
FIX_DGPS       = 2   Differential
FIX_PPS        = 3   Precise Positioning Service
FIX_RTK        = 4   Real-Time Kinematic
FIX_FLOAT_RTK  = 5   Float RTK
FIX_ESTIMATED  = 6   Estimated
FIX_MANUAL     = 7   Manual input
FIX_SIMULATION = 8   Simulation mode
```

## 📌 Common Tasks

### Check if GPS has a fix
```c
if (GPS_IsFixed()) {
    // We have a position
}
```

### Get current position
```c
gps_position_t pos = GPS_GetPosition();
printf("Position: %.6f°N, %.6f°E\n", pos.latitude, pos.longitude);
printf("Altitude: %.1f m\n", pos.altitude);
```

### Get current time
```c
gps_datetime_t dt = GPS_GetDateTime();
printf("Time: %02d:%02d:%02d UTC\n", dt.hour, dt.minute, dt.second);
printf("Date: %04d-%02d-%02d\n", dt.year, dt.month, dt.day);
```

### Get satellite information
```c
printf("Satellites: %u\n", GPS_GetSatelliteCount());
for (int i = 0; i < GPS_GetSatelliteCount(); i++) {
    gps_satellite_t sat = GPS_GetSatellite(i);
    printf("  PRN %u: elev=%u°, azim=%u°, snr=%u dB\n",
           sat.prn, sat.elevation, sat.azimuth, sat.snr);
}
```

### Monitor statistics
```c
gps_stats_t stats = GPS_GetStats();
printf("Sentences: %u (%u valid, %u errors)\n",
       stats.total_sentences, stats.valid_sentences, stats.invalid_sentences);
printf("Fixes: %u (%.1f%% fix rate)\n", stats.valid_fixes, GPS_GetFixRate());
printf("Parse Errors: %u\n", stats.parse_errors);
```

### Print full report
```c
GPS_PrintStats();  // Comprehensive formatted output
```

## 🔧 Platform Integration

### Polling Method (Simple)
```c
int main(void) {
    GPS_Init(9600);
    while (1) {
        uint8_t buf[32];
        int n = GPS_Read(buf, sizeof(buf));
        for (int i = 0; i < n; i++) GPS_ProcessByte(buf[i]);
        // application logic...
    }
}
```

### Interrupt Method (Recommended)
```c
// In UART RX interrupt handler
void USART_IRQ_Handler(void) {
    uint8_t byte = USART_GetByte();
    GPS_ProcessByte(byte);
}

int main(void) {
    GPS_Init(9600);
    // Enable UART RX interrupt
    while (1) {
        // application logic (no blocking on UART reads)
    }
}
```

## 📝 NMEA Sentence Examples

### RMC (Recommended Minimum)
```
$GPRMC,123519,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A
        ^^^^^^  time (UTC)
                ^^^^^^^^^ latitude (ddmm.mmmm)
                        ^ N/S
                         ^^^^^^^^^ longitude (dddmm.mmmm)
                                 ^ E/W
                                   ^^^^^ speed (knots)
                                         ^^^^^ course (degrees)
                                               ^^^^^^ date (ddmmyy)
                                                      ^^^^^ mag variation
                                                            ^ E/W
```

### GGA (Fix Data)
```
$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
        ^^^^^^  time
                ^^^^^^^^^ latitude
                        ^ N/S
                         ^^^^^^^^^ longitude
                                 ^ E/W
                                   ^ fix quality (0=invalid, 1=GPS, 2=DGPS, etc.)
                                     ^^ number of satellites
                                       ^^^ HDOP
                                           ^^^^^ altitude (meters)
```

## ⚙️ Typical NMEA Output Sequence
```
$GPGGA,...   <- Position and altitude
$GPGSA,...   <- Fix type and DOP
$GPGSV,...   <- Satellite 1-4
$GPGSV,...   <- Satellite 5-8
$GPRMC,...   <- Position, speed, course, date/time
(repeat every second at 1 Hz update rate)
```

## 🔌 Hardware Wiring
```
NEO-6M      STM32
─────────────────
TX   1  ──→ RX (UART)
RX   0  ←── TX (UART)
GND  5  ──── GND
VCC  4  ──→ 3.3V (or 5V)
```

## 💾 Memory Usage
- **Context struct**: ~1 KB
- **RX buffer**: 256 bytes
- **Total**: ~1.5 KB RAM
- **Code**: ~15 KB

## 📋 Checklist for Integration
- [ ] Add to CMakeLists.txt
- [ ] Implement `Platform_UART_Init()` for your MCU
- [ ] Implement `Platform_UART_Read()` for your MCU
- [ ] Call `GPS_Init(9600)` in main
- [ ] Feed UART data with `GPS_ProcessByte()`
- [ ] Check `GPS_IsFixed()` before using position
- [ ] Use `GPS_PrintStats()` for debugging

## 📚 Documentation Files
- `README.md` - Full API documentation
- `IMPLEMENTATION.md` - Architecture and design
- `gps_nmea_examples.c` - Complete code examples
- `UART_IMPLEMENTATION_GUIDE.md` - MCU-specific UART setup

---
**For detailed information, see the module README and documentation files!**
