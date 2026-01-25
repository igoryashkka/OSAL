# Application Tasks

This folder contains all task definitions organized by function.

## Task Categories

### Peripheral Tasks (`tasks_peripheral.c/h`)
- `task_led_blink()` - LED heartbeat indicator (500ms, NORMAL priority)
- `task_battery_monitor()` - Battery voltage monitoring (5s, NORMAL priority)

### Communication Tasks (`tasks_comms.c/h`)
- `task_gps_process()` - GPS NMEA parsing and position updates (100ms, HIGH priority)
- Placeholders: ethernet, radio tasks

### Statistics & Monitoring (`tasks_stats.c/h`)
- `task_statistics()` - System statistics and GPS status reporting (10s, LOW priority)
- Placeholders: logging, debug tasks

## Adding New Tasks

1. Create task function in appropriate file (or new file)
2. Add task declaration in corresponding header
3. Call `SCHED_CreateTask()` in `app_init_tasks()` (application.c)
4. Document task purpose, priority, and interval

## Task Priorities
- CRITICAL (3) - Real-time critical tasks
- HIGH (2) - GPS, Ethernet, important sensors
- NORMAL (1) - General housekeeping
- LOW (0) - Monitoring, logging, statistics

## Task Return Value
Tasks return the next execution interval in milliseconds.
Return 0 to execute on next scheduler tick.
