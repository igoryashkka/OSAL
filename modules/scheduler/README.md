# Task Scheduler Module

Lightweight, cooperative multitasking scheduler for embedded systems. No RTOS required - perfect for STM32 applications with multiple periodic tasks.

## Features

- **Cooperative Multitasking**: Non-preemptive, deterministic scheduling
- **Priority-Based**: 4 priority levels (LOW, NORMAL, HIGH, CRITICAL)
- **Flexible Timing**: Millisecond-precision with SysTick integration
- **Task Management**: Create, start, stop, pause, resume tasks dynamically
- **Statistics**: Track task execution time, run counts, errors
- **Zero Dynamic Allocation**: Fixed-size task table (up to 16 tasks)
- **Simple API**: Easy to use from application code
- **Memory Efficient**: Minimal overhead

## Core Concepts

### Cooperative Multitasking
Tasks voluntarily yield control. Each task:
1. Runs to completion (no preemption)
2. Returns how long to wait before next execution
3. Yields control to scheduler
4. Scheduler picks next ready task based on priority

### Task Execution Flow
```
Task 1: GPU Read ─┐
                 ├─→ [Scheduler] ─→ Task 2: Process ─┐
Task 3: Monitor ─┘                                   ├─→ [Scheduler] ─→ ...
                                                     ┘
```

### Task States
- **STOPPED**: Task not running
- **READY**: Task ready to execute immediately
- **RUNNING**: Task currently executing
- **WAITING**: Task waiting for timeout
- **ERROR**: Task encountered error

## Task Definition

### Function Signature
```c
uint32_t my_task(task_handle_t* task) {
    // Task code here
    
    // Return next interval in milliseconds
    return 500;  // Run again in 500ms
}
```

The task function:
- Receives task handle as parameter
- Returns next interval (0 = ASAP, N = wait N ms)
- Must complete quickly (yields to other tasks)
- Should be deterministic and non-blocking

## API Reference

### Initialization
```c
SCHED_Init();                    // Initialize scheduler
SCHED_Run();                     // Enter main scheduler loop
SCHED_Stop();                    // Stop scheduler
```

### Task Creation
```c
task_handle_t* task = SCHED_CreateTask(
    "GPS_Process",               // Task name
    task_gps_func,               // Task function
    TASK_PRIORITY_HIGH,          // Priority
    100,                         // Interval (ms)
    NULL                         // User data
);

// Simple version (default priority, custom interval)
task_handle_t* task = SCHED_CreateTask_Default("MyTask", my_func);
```

### Task Control
```c
SCHED_StartTask(task);           // Start task
SCHED_StopTask(task);            // Stop task
SCHED_PauseTask(task);           // Pause (keep state)
SCHED_ResumeTask(task);          // Resume
SCHED_DelayTask(task, 1000);     // Wait 1 second before next run
SCHED_Yield(task);               // Voluntarily yield (run ASAP next time)
```

### Task Queries
```c
task_state_t state = SCHED_GetTaskState(task);
uint32_t runs = SCHED_GetTaskRunCount(task);
uint32_t avg_us = SCHED_GetTaskAverageRuntime(task);
task_stats_t stats = SCHED_GetTaskStats(task);
uint32_t elapsed = SCHED_GetElapsedTime(task);
```

### Utilities
```c
task_handle_t* task = SCHED_FindTask("GPS_Process");
uint8_t count = SCHED_GetTaskCount();
uint32_t time_ms = SCHED_GetSystemTime();
uint32_t uptime = SCHED_GetUptime();
void* data = SCHED_GetTaskUserData(task);
```

### Statistics
```c
SCHED_PrintStats();              // Overall scheduler stats
SCHED_PrintAllTasks();           // All tasks info
SCHED_PrintTaskInfo(task);       // Specific task info
scheduler_stats_t s = SCHED_GetStats();
```

## Usage Examples

### Simple Application with Multiple Tasks

```c
#include "scheduler.h"
#include "gps_nmea.h"

// Define task functions
uint32_t task_led(task_handle_t* task) {
    LED_Toggle();
    return 500;  // Run every 500ms
}

uint32_t task_gps(task_handle_t* task) {
    uint8_t buf[32];
    int n = GPS_Read(buf, sizeof(buf));
    for (int i = 0; i < n; i++) GPS_ProcessByte(buf[i]);
    return 100;  // Run every 100ms
}

uint32_t task_battery(task_handle_t* task) {
    uint16_t mv = ADC_ReadBattery();
    if (mv < 3000) alert_low_battery();
    return 5000;  // Run every 5 seconds
}

int main(void) {
    // Initialize
    Platform_Init();
    SCHED_Init();
    GPS_Init(9600);
    
    // Create tasks
    SCHED_CreateTask("LED", task_led, TASK_PRIORITY_NORMAL, 500, NULL);
    SCHED_CreateTask("GPS", task_gps, TASK_PRIORITY_HIGH, 100, NULL);
    SCHED_CreateTask("BAT", task_battery, TASK_PRIORITY_NORMAL, 5000, NULL);
    
    // Start tasks
    SCHED_StartTask(SCHED_FindTask("LED"));
    SCHED_StartTask(SCHED_FindTask("GPS"));
    SCHED_StartTask(SCHED_FindTask("BAT"));
    
    // Run scheduler (never returns)
    SCHED_Run();
    
    return 0;
}
```

### Task with User Data

```c
// Task structure
typedef struct {
    uint16_t threshold;
    uint16_t last_value;
} sensor_context_t;

// Task function
uint32_t task_sensor(task_handle_t* task) {
    sensor_context_t* ctx = (sensor_context_t*)SCHED_GetTaskUserData(task);
    
    uint16_t value = read_sensor();
    if (value > ctx->threshold) {
        handle_alert(value);
    }
    ctx->last_value = value;
    
    return 1000;  // Read every second
}

// Usage
int main(void) {
    SCHED_Init();
    
    sensor_context_t sensor_data = {.threshold = 512};
    
    SCHED_CreateTask(
        "Sensor",
        task_sensor,
        TASK_PRIORITY_NORMAL,
        1000,
        &sensor_data  // User data
    );
    
    SCHED_Run();
}
```

### Dynamic Task Control

```c
// Runtime task enable/disable
void on_button_press(void) {
    task_handle_t* eth = SCHED_FindTask("Ethernet");
    if (SCHED_GetTaskState(eth) == TASK_STOPPED) {
        SCHED_StartTask(eth);
    } else {
        SCHED_StopTask(eth);
    }
}

// Change task timing
void set_gps_fast_mode(void) {
    task_handle_t* gps = SCHED_FindTask("GPS_Process");
    SCHED_SetTaskInterval(gps, 50);  // 50ms instead of 100ms
}

// Pause non-critical tasks to save power
void enter_low_power(void) {
    SCHED_PauseTask(SCHED_FindTask("Stats_Report"));
    SCHED_PauseTask(SCHED_FindTask("Ethernet"));
}
```

### Monitoring

```c
// Print task information
void debug_tasks(void) {
    SCHED_PrintAllTasks();
    
    // Or specific task
    task_handle_t* gps = SCHED_FindTask("GPS_Process");
    SCHED_PrintTaskInfo(gps);
}

// Sample output:
// === SCHEDULER TASKS (3 active) ===
// 
// [0] LED_Blink
//     State: WAITING | Priority: 1 | Interval: 500 ms
//     Runs: 24 | Avg: 45 μs | Max: 120 μs
// 
// [1] GPS_Process
//     State: WAITING | Priority: 2 | Interval: 100 ms
//     Runs: 120 | Avg: 850 μs | Max: 2100 μs
// 
// [2] Battery_Monitor
//     State: WAITING | Priority: 1 | Interval: 5000 ms
//     Runs: 4 | Avg: 200 μs | Max: 450 μs
```

## Priority Levels

```c
typedef enum {
    TASK_PRIORITY_LOW = 0,        // Non-critical (reporting, logging)
    TASK_PRIORITY_NORMAL = 1,     // Default (most tasks)
    TASK_PRIORITY_HIGH = 2,       // Important (sensor processing, GPS)
    TASK_PRIORITY_CRITICAL = 3    // Essential (safety, control)
} task_priority_t;
```

**Scheduling Rule**: Higher priority tasks run before lower priority tasks. Among same priority, oldest task (FIFO) runs first.

## Integration with SysTick

The scheduler requires millisecond-accurate system time. It uses SysTick for this:

### Platform Integration
```c
// In your STM32 platform layer:

void SysTick_Handler(void) {
    SCHED_SysTick_Handler();  // Increments system time
    // Other SysTick processing...
}

// During platform init:
void Platform_Init(void) {
    // ... other init ...
    SCHED_SysTick_Init();  // Setup 1ms SysTick
}
```

### Manual Time Update (if not using SysTick)
```c
// If you update time manually every millisecond:
void timer_interrupt(void) {
    SCHED_SysTick_Handler();  // Call this from your 1ms timer
}
```

## Guidelines

### Task Design
1. **Keep tasks short**: Long tasks block other tasks
2. **Be deterministic**: Avoid blocking operations
3. **Return proper intervals**: Scheduler depends on correct timing
4. **Handle errors gracefully**: Check return values
5. **Use user data for context**: Store task-specific state

### Performance Tips
1. **Prioritize correctly**: GPS should be higher than LED
2. **Balance intervals**: Don't make all tasks same interval (causes bunching)
3. **Monitor statistics**: Use `SCHED_PrintStats()` to find bottlenecks
4. **Profile task runtime**: Check max runtime for timing issues

### Common Mistakes
1. ❌ Blocking operations (UART_Wait, SPI_Transfer blocking)
2. ❌ Very long task functions
3. ❌ Returning wrong intervals
4. ❌ Not checking task status before using
5. ❌ Creating too many tasks (limit is 16)

## Limitations

- **Non-preemptive**: Long tasks block others. Keep tasks under 10ms.
- **16 task limit**: Compile-time constant `TASK_MAX_COUNT`
- **No ISR context**: Task functions can't be called from ISRs
- **No IPC**: No built-in inter-task communication (use shared data)
- **Fixed timing**: No dynamic priority changes during runtime

## Architecture

### Internal Structure
```
┌─ Scheduler Context ─────────────────┐
│                                     │
│ ┌─ Task Table [0..15] ────────────┐ │
│ │ [0] { name, func, state, stats }│ │
│ │ [1] { name, func, state, stats }│ │
│ │ ...                             │ │
│ └─────────────────────────────────┘ │
│                                     │
│ System Time: 12345 ms               │
│ Running: true                       │
│ Statistics: {...}                   │
└─────────────────────────────────────┘
```

## Troubleshooting

### Tasks not running
- Check if `SCHED_Run()` is called
- Verify task is started with `SCHED_StartTask()`
- Check task state with `SCHED_GetTaskState()`

### Intermittent behavior
- Check task runtime with `SCHED_PrintTaskInfo()`
- Long task might be blocking others
- Verify interval values are correct

### Wrong timing
- Check `SCHED_GetSystemTime()` increments correctly
- Verify SysTick interrupt is called every 1ms
- Check task return values (intervals)

### Memory issues
- Task table is fixed-size (16 tasks max)
- Each task ~100 bytes
- Total memory ~3KB

## See Also
- `scheduler_examples.c` - Complete application examples
- `scheduler.h` - Full API documentation
