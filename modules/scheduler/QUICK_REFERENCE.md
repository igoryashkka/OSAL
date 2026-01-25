# Task Scheduler - Quick Reference

## Overview

Lightweight cooperative multitasking for STM32. Replace your hyper-loop with organized, priority-based tasks.

## Basic Usage

### 1. Define Task Functions
```c
uint32_t my_task(task_handle_t* task) {
    // Do task work here
    
    return 500;  // Next run in 500ms (0 = ASAP)
}
```

### 2. Initialize & Create Tasks
```c
int main(void) {
    SCHED_Init();
    
    SCHED_CreateTask("MyTask", my_task, 
                     TASK_PRIORITY_NORMAL, 500, NULL);
    
    SCHED_Run();  // Never returns
}
```

### 3. That's it!

## Common Tasks

### GPS Task (100ms, HIGH priority)
```c
uint32_t task_gps(task_handle_t* task) {
    uint8_t buf[32];
    int n = GPS_Read(buf, sizeof(buf));
    for (int i = 0; i < n; i++) GPS_ProcessByte(buf[i]);
    return 100;
}
```

### LED Task (500ms, NORMAL priority)
```c
uint32_t task_led(task_handle_t* task) {
    LED_Toggle();
    return 500;
}
```

### Battery Task (5s, NORMAL priority)
```c
uint32_t task_battery(task_handle_t* task) {
    uint16_t mv = ADC_ReadBattery();
    if (mv < 3000) Alert_LowBattery();
    return 5000;
}
```

### Statistics Task (10s, LOW priority)
```c
uint32_t task_stats(task_handle_t* task) {
    SCHED_PrintAllTasks();
    return 10000;
}
```

## Priority Levels

```
TASK_PRIORITY_CRITICAL = 3  ← Highest (safety, control)
TASK_PRIORITY_HIGH = 2      ← Important (GPS, sensors)
TASK_PRIORITY_NORMAL = 1    ← Default (most tasks)
TASK_PRIORITY_LOW = 0       ← Lowest (reporting, logging)
```

## Task Control API

| Function | Purpose |
|----------|---------|
| `SCHED_CreateTask(name, func, priority, interval, data)` | Create task |
| `SCHED_StartTask(task)` | Start task |
| `SCHED_StopTask(task)` | Stop task |
| `SCHED_PauseTask(task)` | Pause (keep state) |
| `SCHED_ResumeTask(task)` | Resume |
| `SCHED_FindTask("name")` | Find by name |
| `SCHED_DelayTask(task, ms)` | Wait N ms before next run |

## Queries

```c
SCHED_GetTaskState(task)         // Current state
SCHED_GetTaskRunCount(task)      // How many times run
SCHED_GetTaskAverageRuntime(task) // Avg execution time (μs)
SCHED_GetUptime()                // System uptime (ms)
SCHED_GetSystemTime()            // Current time (ms)
```

## Debugging

```c
SCHED_PrintAllTasks();      // Show all tasks
SCHED_PrintTaskInfo(task);  // Show specific task
SCHED_PrintStats();         // Scheduler statistics
```

## Task States

```
STOPPED  ──→ (StartTask) ──→ READY ──→ (Time to run) ──→ RUNNING
                                ↑                          │
                                └──────────────────────────┘
                                    (Return interval)
```

## Example: Complete App

```c
#include "scheduler.h"
#include "gps_nmea.h"

uint32_t task_led(task_handle_t* t) { LED_Toggle(); return 500; }
uint32_t task_gps(task_handle_t* t) {
    uint8_t buf[32];
    int n = GPS_Read(buf, sizeof(buf));
    for (int i = 0; i < n; i++) GPS_ProcessByte(buf[i]);
    return 100;
}
uint32_t task_bat(task_handle_t* t) { 
    if (ADC_ReadBattery() < 3000) alert();
    return 5000; 
}

int main(void) {
    SCHED_Init();
    GPS_Init(9600);
    
    SCHED_CreateTask("LED", task_led, TASK_PRIORITY_NORMAL, 500, NULL);
    SCHED_CreateTask("GPS", task_gps, TASK_PRIORITY_HIGH, 100, NULL);
    SCHED_CreateTask("BAT", task_bat, TASK_PRIORITY_NORMAL, 5000, NULL);
    
    SCHED_StartTask(SCHED_FindTask("LED"));
    SCHED_StartTask(SCHED_FindTask("GPS"));
    SCHED_StartTask(SCHED_FindTask("BAT"));
    
    SCHED_Run();  // Never returns
}
```

## Runtime Control

### Enable/Disable Task
```c
task_handle_t* eth = SCHED_FindTask("Ethernet");
if (button_pressed()) {
    SCHED_StartTask(eth);   // Enable
} else {
    SCHED_StopTask(eth);    // Disable
}
```

### Change Task Interval
```c
task_handle_t* gps = SCHED_FindTask("GPS_Process");
SCHED_SetTaskInterval(gps, 50);  // Change to 50ms
```

### Pause Non-Critical Tasks
```c
SCHED_PauseTask(SCHED_FindTask("Statistics"));
SCHED_PauseTask(SCHED_FindTask("Ethernet"));
SCHED_ResumeTask(SCHED_FindTask("Statistics"));
```

## Key Points

✅ **Cooperative**: Tasks yield voluntarily (deterministic)  
✅ **Priority-based**: Higher priority = runs first  
✅ **Flexible**: Pause, resume, change intervals at runtime  
✅ **Observable**: Built-in statistics and debugging  
✅ **No RTOS**: Simple, lightweight, no dependencies  
✅ **Fixed Memory**: No dynamic allocation (max 16 tasks)  

❌ **Blocking calls**: Avoid sleep, UART_Wait, blocking I/O  
❌ **Long functions**: Keep tasks under 10ms  
❌ **Task limits**: Maximum 16 concurrent tasks  

## Files

- `scheduler.h` - API definition
- `scheduler.c` - Implementation
- `scheduler_examples.c` - Complete examples
- `README.md` - Full documentation

---

**That's all you need to start!** Create tasks, set priorities, and let the scheduler manage them.
