# GPS NMEA Module Architecture & Data Flow

## System Architecture

```
┌─────────────────────────────────────────────────────────────────────┐
│                        Your Application                             │
│                         (app/main.c)                                │
└────────────────────────┬────────────────────────────────────────────┘
                         │
                         │ Uses
                         ↓
┌─────────────────────────────────────────────────────────────────────┐
│                  GPS NMEA Module (gps_nmea.c)                       │
│                                                                     │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │ Core Functions:                                             │   │
│  │  • GPS_Init() - Initialize with baud rate                 │   │
│  │  • GPS_ProcessByte() - Feed UART data byte-by-byte        │   │
│  │  • GPS_ProcessSentence() - Direct NMEA sentence parsing    │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                                                                     │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │ NMEA Sentence Parsers:                                      │   │
│  │  • RMC - Position, speed, course, date/time               │   │
│  │  • GGA - Position, altitude, fix quality, satellites      │   │
│  │  • GSA - Fix type, HDOP/VDOP/PDOP                         │   │
│  │  • GSV - Satellite details (up to 12)                     │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                                                                     │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │ Data Structures:                                            │   │
│  │  • gps_position_t   - Position, speed, course             │   │
│  │  • gps_datetime_t   - UTC date/time                        │   │
│  │  • gps_satellite_t  - Individual satellite info            │   │
│  │  • gps_stats_t      - Error & success counters             │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                                                                     │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │ Query Functions:                                            │   │
│  │  • GPS_GetPosition()       - Current position              │   │
│  │  • GPS_GetDateTime()       - Current date/time             │   │
│  │  • GPS_GetStats()          - Statistics                    │   │
│  │  • GPS_IsFixed()           - Has valid fix?                │   │
│  │  • GPS_GetSatelliteCount() - Active satellites             │   │
│  │  • GPS_GetSatellite(i)     - Satellite details             │   │
│  └─────────────────────────────────────────────────────────────┘   │
└────────────────────────┬────────────────────────────────────────────┘
                         │
                         │ Uses
                         ↓
┌─────────────────────────────────────────────────────────────────────┐
│            Platform_UART API (platform_uart.h)                      │
│                                                                     │
│  Functions:                                                         │
│  • Platform_UART_Init(baud)      - Initialize UART                │
│  • Platform_UART_Read()          - Read data from UART            │
│  • Platform_UART_Write()         - Write data to UART             │
│                                                                     │
│  (Weak defaults in Platform_API, overridden by platform impl)     │
└────────────────────────┬────────────────────────────────────────────┘
                         │
                         │ Calls
                         ↓
┌─────────────────────────────────────────────────────────────────────┐
│       MCU-Specific UART Implementations                             │
│                                                                     │
│  platform/stm32/impl/stm32f1/   - STM32F103 implementation        │
│  platform/stm32/impl/stm32h7/   - STM32H750 implementation        │
│                                                                     │
│  Provides:                                                          │
│  • GPIO configuration for UART pins                                │
│  • UART peripheral setup                                           │
│  • RX/TX interrupt handlers (optional)                             │
│  • Circular RX buffer (optional but recommended)                   │
└────────────────────────┬────────────────────────────────────────────┘
                         │
                         │ Controls
                         ↓
┌─────────────────────────────────────────────────────────────────────┐
│                   STM32 UART Hardware                               │
│                  (USART1, USART2, UART3, etc.)                     │
└────────────────────────┬────────────────────────────────────────────┘
                         │
                         │ ↔ Serial
                         │
┌─────────────────────────────────────────────────────────────────────┐
│                    NEO-6M GPS Module                                │
│                                                                     │
│  Outputs NMEA 0183 sentences at 9600 baud:                         │
│  • $GPGGA - Global Positioning System Fix Data                     │
│  • $GPRMC - Recommended Minimum Navigation Information             │
│  • $GPGSA - GPS DOP and Active Satellites                          │
│  • $GPGSV - GPS Satellites in View                                 │
│                                                                     │
│  Update Rate: 1 Hz (default)                                        │
│  Baud Rate: 9600 (default)                                          │
└─────────────────────────────────────────────────────────────────────┘
```

## Data Flow Diagram

```
NEO-6M Module (UART TX)
       │
       │ 9600 baud
       │ NMEA sentences
       │
       ▼
STM32 UART RX
       │
       │ [Optional] DMA/Interrupt
       │
       ▼
Platform_UART_Read()
       │
       │ Returns: uint8_t bytes
       │
       ▼
Application Loop
       │
       │ for each byte:
       │
       ├─→ GPS_ProcessByte(byte)
       │       │
       │       ├─→ Detects: $ (start)
       │       │
       │       ├─→ Accumulates: sentence data
       │       │
       │       ├─→ Detects: * (checksum marker)
       │       │
       │       ├─→ Detects: \r\n (sentence end)
       │       │
       │       ├─→ Validates: XOR checksum
       │       │
       │       └─→ Routes to: Parser
       │               │
       │               ├─→ RMC Parser (position, speed, course, date/time)
       │               │
       │               ├─→ GGA Parser (position, altitude, fix quality)
       │               │
       │               ├─→ GSA Parser (fix type, HDOP)
       │               │
       │               └─→ GSV Parser (satellite details)
       │
       │ Periodically:
       │
       └─→ GPS_GetPosition()  ─→ Returns: gps_position_t
       └─→ GPS_GetDateTime()  ─→ Returns: gps_datetime_t
       └─→ GPS_GetStats()     ─→ Returns: gps_stats_t
       └─→ GPS_IsFixed()      ─→ Returns: bool
```

## Sentence Processing Flow

```
Incoming UART Byte Stream:
$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\r\n

GPS_ProcessByte() Processing:
│
├─ Byte '$'  ─→ Start of sentence detected, initialize buffer
├─ Byte 'G'  ─→ Accumulate: buffer[0] = 'G'
├─ Byte 'P'  ─→ Accumulate: buffer[1] = 'P'
├─ ... (continue for all data bytes)
├─ Byte '*'  ─→ Checksum marker found
├─ Byte '4'  ─→ Checksum nibble 1
├─ Byte '7'  ─→ Checksum nibble 2
├─ Byte '\r' ─→ Sentence complete!
│
└─→ Validation Phase:
    │
    ├─ Extract checksum from sentence: 0x47
    ├─ Calculate checksum: XOR of all bytes between '$' and '*'
    ├─ Compare: calculated == received? YES
    │
    └─→ Parsing Phase:
        │
        ├─ Identify sentence type: "GGA"
        │
        ├─ Call: gps_parse_gga(sentence)
        │   │
        │   ├─ Extract field 1: Time "123519"
        │   ├─ Extract field 2: Latitude "4807.038"
        │   ├─ Extract field 3: Lat direction "N"
        │   ├─ Extract field 4: Longitude "01131.000"
        │   ├─ Extract field 5: Lon direction "E"
        │   ├─ Extract field 6: Fix quality "1"
        │   ├─ Extract field 7: Num satellites "08"
        │   ├─ Extract field 8: HDOP "0.9"
        │   ├─ Extract field 9: Altitude "545.4"
        │   │
        │   └─ Update global state:
        │       • g_gps.datetime.hour = 12
        │       • g_gps.datetime.minute = 35
        │       • g_gps.datetime.second = 19
        │       • g_gps.position.latitude = 48.1173°
        │       • g_gps.position.longitude = 11.5167°
        │       • g_gps.position.altitude = 545.4 m
        │       • g_gps.position.num_satellites = 8
        │       • g_gps.position.fix_type = FIX_GPS
        │       • g_gps.stats.gga_count++
        │       • g_gps.stats.valid_sentences++
        │       • g_gps.stats.total_sentences++
        │
        └─→ Data ready for: GPS_GetPosition(), GPS_GetDateTime(), etc.
```

## Memory Layout

```
Stack
  ↑
  │
  │
  │
┌─────────────────────────────────┐
│  Global GPS Context (1 KB)      │
├─────────────────────────────────┤
│ gps_position_t                  │  ├─ latitude (double)
│                                 │  ├─ longitude (double)
│                                 │  ├─ altitude (double)
│                                 │  ├─ speed_knots (double)
│                                 │  ├─ course (double)
│                                 │  ├─ num_satellites (uint8)
│                                 │  ├─ hdop (uint8)
│                                 │  └─ fix_type (enum)
├─────────────────────────────────┤
│ gps_datetime_t                  │  ├─ hour, minute, second
│                                 │  └─ day, month, year
├─────────────────────────────────┤
│ gps_stats_t                     │  ├─ total_sentences (uint32)
│                                 │  ├─ valid_sentences (uint32)
│                                 │  ├─ invalid_sentences (uint32)
│                                 │  ├─ gga_count, rmc_count, ...
│                                 │  └─ error counters
├─────────────────────────────────┤
│ gps_satellite_t[12]             │  Each: PRN, elevation, azimuth, SNR
├─────────────────────────────────┤
│ RX Buffer (256 bytes)           │  Sentence assembly buffer
├─────────────────────────────────┤
│ RX Index, Flags                 │  Current buffer position, complete flag
└─────────────────────────────────┘
  ↓
Heap
```

## State Machine (Sentence Processing)

```
┌─────────────┐
│   IDLE      │  No sentence in progress
│ rx_index=0  │
└──────┬──────┘
       │ Receive byte '$'
       ↓
┌─────────────┐
│ IN_SENTENCE │  Accumulating sentence bytes
│ rx_index>0  │
└──────┬──────┘
       │ Receive data bytes
       │ (accumulate in buffer)
       │
       │ Receive '*' or '\r' or '\n'
       ↓
┌─────────────────────┐
│ VALIDATE & PARSE    │
│                     │
│ 1. Extract checksum │
│ 2. Calculate checksum
│ 3. Compare & verify │
│ 4. Parse fields     │
│ 5. Update state     │
│ 6. Update stats     │
└──────┬──────────────┘
       │
       ↓
┌─────────────┐
│   IDLE      │  Ready for next sentence
│ rx_index=0  │
└─────────────┘
```

## Integration Example Flow

```
main()
  │
  └─→ GPS_Init(9600)
        └─→ Platform_UART_Init(9600)
              └─→ Configure UART GPIO and peripheral

Loop:
  │
  ├─→ GPS_Read(buffer, 32)
  │     └─→ Platform_UART_Read()
  │           └─→ Returns: 0-32 bytes from UART
  │
  ├─→ for each byte in buffer:
  │     └─→ GPS_ProcessByte(byte)
  │           └─→ Accumulate and parse NMEA sentences
  │
  ├─→ if (GPS_IsFixed())
  │     │
  │     ├─→ gps_position_t pos = GPS_GetPosition()
  │     ├─→ gps_datetime_t dt = GPS_GetDateTime()
  │     │
  │     └─→ Use data: pos.latitude, pos.longitude, etc.
  │
  └─→ if (++stats_counter >= 10000)
        ├─→ GPS_PrintStats()  // Print diagnostics
        └─→ stats_counter = 0

Periodically:
  └─→ gps_stats_t stats = GPS_GetStats()
        ├─ printf("Valid Rate: %.1f%%\n", GPS_GetValidRate())
        └─ printf("Fix Rate: %.1f%%\n", GPS_GetFixRate())
```

---

This modular, abstraction-based architecture enables:
- **Portability**: Works across different STM32 variants
- **Testability**: Can test parsing independently of hardware
- **Maintainability**: Clear separation of concerns
- **Reliability**: Statistics tracking for debugging
