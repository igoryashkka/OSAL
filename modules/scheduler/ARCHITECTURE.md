# Task Scheduler Architecture

## System Overview

```
┌─────────────────────────────────────────────────────────────────┐
│                     Application (app/main.c)                    │
│                                                                 │
│  main() {                                                       │
│    SCHED_Init();                                                │
│    SCHED_CreateTask("GPS", task_gps, HIGH, 100, NULL);          │
│    SCHED_CreateTask("LED", task_led, NORMAL, 500, NULL);        │
│    SCHED_CreateTask("BAT", task_bat, NORMAL, 5000, NULL);       │
│    SCHED_StartTask(...);                                        │
│    SCHED_Run();  ← Main scheduler loop                          │
│  }                                                              │
└────────────────────────┬────────────────────────────────────────┘
                         │
                         ↓
┌─────────────────────────────────────────────────────────────────┐
│                  Scheduler Module (scheduler.c)                 │
│                                                                 │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ Task Management                                          │  │
│  │ ├─ Create/Destroy tasks                                │  │
│  │ ├─ Find task by name                                   │  │
│  │ └─ Task state transitions                              │  │
│  └──────────────────────────────────────────────────────────┘  │
│                                                                 │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ Task Scheduling (Main Loop)                            │  │
│  │ ├─ Find next ready task (priority-based)               │  │
│  │ ├─ Execute task function                               │  │
│  │ ├─ Collect task statistics                             │  │
│  │ └─ Schedule next run based on return value             │  │
│  └──────────────────────────────────────────────────────────┘  │
│                                                                 │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ Task Table (Fixed 16 tasks max)                        │  │
│  │                                                         │  │
│  │  [0] GPS_Process  {HIGH, 100ms, running, stats}       │  │
│  │  [1] LED_Blink    {NORMAL, 500ms, waiting, stats}     │  │
│  │  [2] Battery      {NORMAL, 5000ms, waiting, stats}    │  │
│  │  [3] Statistics   {LOW, 10000ms, stopped, stats}      │  │
│  │  ...                                                   │  │
│  └──────────────────────────────────────────────────────────┘  │
│                                                                 │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ System Time (SysTick based)                            │  │
│  │ ├─ g_system_time_ms (incremented by ISR)               │  │
│  │ ├─ SCHED_GetSystemTime()                               │  │
│  │ └─ SCHED_SysTick_Handler()                             │  │
│  └──────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
                         │
                         ↓
┌─────────────────────────────────────────────────────────────────┐
│                  Application Tasks (Custom Code)                │
│                                                                 │
│  uint32_t task_gps(task_handle_t* task) {                      │
│    GPS_Read(); GPS_ProcessByte();                              │
│    return 100;  /* Next run in 100ms */                        │
│  }                                                              │
│                                                                 │
│  uint32_t task_led(task_handle_t* task) {                      │
│    LED_Toggle();                                                │
│    return 500;  /* Next run in 500ms */                        │
│  }                                                              │
└─────────────────────────────────────────────────────────────────┘
                         │
                         ↓
┌─────────────────────────────────────────────────────────────────┐
│              Platform APIs (Platform Layer)                     │
│                                                                 │
│  GPS_Read() ─→ Platform_UART_Read()                           │
│  LED_Toggle() ─→ Platform_GPIO_...()                          │
│  ADC_Read() ─→ Platform_ADC_...()                             │
└─────────────────────────────────────────────────────────────────┘
                         │
                         ↓
┌─────────────────────────────────────────────────────────────────┐
│                   STM32 Hardware                                │
│                                                                 │
│  SysTick (1ms interrupts)                                      │
│  UART1/2/3 (GPS, Ethernet, Radio)                             │
│  GPIO (LED, Button)                                            │
│  ADC (Battery voltage)                                         │
└─────────────────────────────────────────────────────────────────┘
```

## Task State Machine

```
┌─────────────┐
│   STOPPED   │  Task not running
└──────┬──────┘
       │ StartTask()
       ↓
┌─────────────────┐
│     READY       │  Task ready to run immediately
│ next_run <= now │
└────────┬────────┘
         │ Scheduler picks task (priority)
         ↓
┌──────────────┐
│   RUNNING    │  Task function executing
└────┬─────────┘
     │ Task returns interval
     ↓
┌──────────────────────┐
│   WAITING            │  Task waiting for scheduled time
│ next_run > now       │
└────┬─────────────────┘
     │ DelayTask() / Interval expired
     ↓
    READY ──→ RUNNING ──→ WAITING (repeat)

Alternative paths:
READY ──(StopTask)──→ STOPPED
WAITING ──(PauseTask)──→ (same state, but won't execute)
WAITING ──(ResumeTask)──→ READY
```

## Scheduling Example (Timeline)

```
Time    Action
────────────────────────────────────────────────────────────────

0ms     BOOT: SCHED_Init()
        Create: GPS (100ms, HIGH), LED (500ms, NORMAL), 
                BAT (5000ms, NORMAL)
        Start all tasks
        GPS.next_run=0, LED.next_run=0, BAT.next_run=0

1ms     SysTick: g_system_time_ms = 1
        SCHED_Run finds GPS ready (priority HIGH)
        Execute task_gps() → returns 100
        GPS.next_run = 1+100 = 101ms

2ms     SysTick: g_system_time_ms = 2
        SCHED_Run finds LED ready (no higher priority task)
        Execute task_led() → returns 500
        LED.next_run = 2+500 = 502ms

...

500ms   SysTick: g_system_time_ms = 500
        No other task ready (BAT next at 5000)
        GPS ready again (101+100=201, expired)
        Execute task_gps() → returns 100
        GPS.next_run = 500+100 = 600ms

502ms   SysTick: g_system_time_ms = 502
        LED ready (502 >= 502)
        Execute task_led() → returns 500
        LED.next_run = 502+500 = 1002ms

...

5000ms  SysTick: g_system_time_ms = 5000
        BAT ready (5000 >= 5000)
        GPS also ready, but BAT at 5000 doesn't have priority
        Actually GPS (HIGH) ready at 600+100=700 expired
        Execute task_gps() → returns 100
        GPS.next_run = 5000+100 = 5100ms

5000ms  Next cycle: BAT ready (5000 >= 5000)
        Execute task_battery() → returns 5000
        BAT.next_run = 5000+5000 = 10000ms

...
```

## Memory Layout

```
RAM Usage per Scheduler:

Scheduler Context (≈150 bytes)
├─ task_count: 1 byte
├─ running: 1 byte
├─ System time: 4 bytes
└─ Statistics: remaining

Task Table (16 × 120 bytes = 1920 bytes)
├─ [0] GPS_Process
│      ├─ name ptr: 4 bytes
│      ├─ func ptr: 4 bytes
│      ├─ priority: 4 bytes
│      ├─ interval_ms: 4 bytes
│      ├─ state: 4 bytes
│      ├─ next_run_ms: 4 bytes
│      ├─ stats (run_count, runtime, etc.): 32 bytes
│      └─ user_data ptr: 4 bytes
├─ [1] LED_Blink
│      └─ (same structure)
├─ [2] Battery_Monitor
│      └─ (same structure)
└─ ...

Total: ≈2.1 KB RAM
Code: ≈8 KB Flash

Per application: scheduler + (16 × task size) = ≈2.1KB + app tasks
```

## Task Execution Flow (Main Loop)

```
SCHED_Run() {
    while (running) {
        
        // Find next ready task
        task = find_next_ready_task();
        
        if (!task) continue;  // No task ready, wait
        
        // Execute task
        task.state = RUNNING;
        uint32_t start = SCHED_GetSystemTime();
        
        uint32_t next_interval = task.func(task);  // ← Call task
        
        uint32_t elapsed = SCHED_GetSystemTime() - start;
        
        // Update statistics
        task.stats.run_count++;
        task.stats.total_runtime_us += elapsed × 1000;
        task.stats.max_runtime_us = max(task.stats.max_runtime_us, elapsed × 1000);
        task.stats.min_runtime_us = min(task.stats.min_runtime_us, elapsed × 1000);
        task.stats.last_run_ms = start;
        
        // Schedule next run
        task.next_run_ms = SCHED_GetSystemTime() + next_interval;
        task.state = (next_interval > 0) ? WAITING : READY;
    }
}
```

## Task Selection Algorithm

```
find_next_ready_task() {
    best = NULL
    now = SCHED_GetSystemTime()
    
    for each task in task_table:
        // Skip inactive tasks
        if (task.state == STOPPED) continue
        
        // Skip if not yet time to run
        if (task.next_run_ms > now) continue
        
        // Prefer highest priority
        if (!best || task.priority > best.priority)
            best = task
        
        // Same priority? Use oldest (FIFO)
        else if (task.priority == best.priority)
            if (task.next_run_ms < best.next_run_ms)
                best = task
    
    return best
}
```

**Priority+FIFO ensures**:
- Critical tasks always run first
- No starvation of lower priority tasks (unless critical tasks starve them)
- Deterministic, predictable behavior

## SysTick Integration

```
STM32 SysTick (1ms interrupt)
     ↓
SysTick_Handler()
     ↓
SCHED_SysTick_Handler()
     ↓
g_system_time_ms++  (atomic increment)
     ↓
Return to interrupted code
     ↓
Main scheduler loop checks time
```

**Synchronization**:
- SysTick increments global time every 1ms
- Scheduler reads time to determine which tasks are ready
- No blocking on time (non-blocking checks)

## Data Flow: Task Execution

```
Input Data:
  - UART from GPS: bytes
  - ADC values: analog
  - GPIO states: pins

     ↓ (Processed by tasks)

Task Processing:
  task_gps: reads UART → parses NMEA → updates g_gps state
  task_led: reads system state → toggles LED GPIO
  task_battery: reads ADC → checks threshold → triggers alert

     ↓ (Output to hardware)

Output Data:
  - GPIO to LED: PWM
  - UART commands: device control
  - Internal state: g_gps, battery_level, etc.
```

## Comparing Loop Styles

### Old Hyper-Loop (❌)
```c
int main() {
    while(1) {
        blink_once();        // Fixed 1 run per iteration
        gps_update();        // But runs at same rate
        battery_check();     // Hard to control rates
        // Can't easily add/remove/pause tasks
    }
}
```

**Problems**:
- All tasks run at same rate
- Can't prioritize GPS over LED
- Hard to pause tasks
- No statistics
- Not scalable

### With Scheduler (✅)
```c
int main() {
    SCHED_Init();
    
    SCHED_CreateTask("GPS", task_gps, HIGH, 100, NULL);
    SCHED_CreateTask("LED", task_led, NORMAL, 500, NULL);
    SCHED_CreateTask("BAT", task_bat, NORMAL, 5000, NULL);
    
    SCHED_StartTask(...);
    SCHED_Run();  // Handles everything
}
```

**Advantages**:
- Each task has own interval
- Priority-based execution
- Easy to add/remove/pause/resume
- Built-in statistics
- Scalable to many tasks
- No RTOS overhead

---

**The scheduler provides a professional application framework without the complexity of a full RTOS.**
