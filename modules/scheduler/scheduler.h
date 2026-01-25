#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Lightweight Cooperative Task Scheduler (No RTOS)
 * 
 * Simple task management for embedded systems with multiple periodic tasks.
 * - Non-preemptive (cooperative multitasking)
 * - No dynamic allocation
 * - Fixed number of tasks (up to 16)
 * - Millisecond-precision timing (requires SysTick)
 */

/* Maximum number of concurrent tasks */
#define TASK_MAX_COUNT  16

/* Task states */
typedef enum {
    TASK_STOPPED = 0,    /* Task not running */
    TASK_READY = 1,      /* Task ready to run */
    TASK_RUNNING = 2,    /* Task currently executing */
    TASK_WAITING = 3,    /* Task waiting for time or event */
    TASK_ERROR = 4       /* Task encountered error */
} task_state_t;

/* Task priority levels */
typedef enum {
    TASK_PRIORITY_LOW = 0,
    TASK_PRIORITY_NORMAL = 1,
    TASK_PRIORITY_HIGH = 2,
    TASK_PRIORITY_CRITICAL = 3
} task_priority_t;

/* Task execution result */
typedef enum {
    TASK_OK = 0,         /* Task completed successfully */
    TASK_YIELD = 1,      /* Task yielded voluntarily */
    TASK_ERROR_CODE = 2  /* Task returned error */
} task_result_t;

/* Forward declaration */
struct task_handle;
typedef struct task_handle task_handle_t;

/* Task function pointer - returns how long to wait before next run */
typedef uint32_t (*task_func_t)(task_handle_t* task);

/* Task statistics */
typedef struct {
    uint32_t run_count;           /* Number of times task executed */
    uint32_t total_runtime_us;    /* Total execution time in microseconds */
    uint32_t max_runtime_us;      /* Max single execution time */
    uint32_t min_runtime_us;      /* Min single execution time */
    uint32_t error_count;         /* Number of errors */
    uint32_t last_run_ms;         /* Timestamp of last execution */
} task_stats_t;

/* Task handle / context */
typedef struct task_handle {
    /* Configuration */
    const char* name;
    task_func_t func;
    task_priority_t priority;
    uint32_t interval_ms;         /* Target interval (0 = as fast as possible) */
    
    /* State */
    task_state_t state;
    uint32_t next_run_ms;         /* When task should run next */
    uint32_t timeout_ms;          /* Timeout if set (0 = no timeout) */
    void* user_data;              /* User-defined data pointer */
    
    /* Statistics */
    task_stats_t stats;
    
    /* Internal */
    uint32_t creation_time_ms;
} task_handle_t;

/* === Scheduler Control === */
void SCHED_Init(void);
void SCHED_Run(void);                    /* Main scheduler loop */
void SCHED_Stop(void);                   /* Stop scheduler */
bool SCHED_IsRunning(void);

/* === Task Management === */
task_handle_t* SCHED_CreateTask(
    const char* name,
    task_func_t func,
    task_priority_t priority,
    uint32_t interval_ms,
    void* user_data
);

task_handle_t* SCHED_CreateTask_Default(
    const char* name,
    task_func_t func
);

void SCHED_DestroyTask(task_handle_t* task);
task_handle_t* SCHED_FindTask(const char* name);
uint8_t SCHED_GetTaskCount(void);

/* === Task Control === */
void SCHED_StartTask(task_handle_t* task);
void SCHED_StopTask(task_handle_t* task);
void SCHED_PauseTask(task_handle_t* task);
void SCHED_ResumeTask(task_handle_t* task);

/* === Task Queries === */
task_state_t SCHED_GetTaskState(task_handle_t* task);
const char* SCHED_GetTaskStateStr(task_state_t state);
uint32_t SCHED_GetTaskRunCount(task_handle_t* task);
uint32_t SCHED_GetTaskLastRunTime(task_handle_t* task);
uint32_t SCHED_GetTaskAverageRuntime(task_handle_t* task);
task_stats_t SCHED_GetTaskStats(task_handle_t* task);

/* === Timing & Delays === */
uint32_t SCHED_GetElapsedTime(task_handle_t* task);  /* ms since task created */
uint32_t SCHED_GetUptime(void);                       /* Total uptime in ms */
void SCHED_DelayTask(task_handle_t* task, uint32_t ms);  /* Make task wait */

/* === Scheduler Statistics === */
typedef struct {
    uint8_t task_count;
    uint32_t total_runtime_us;
    uint32_t max_cycle_time_us;
    uint32_t min_cycle_time_us;
    uint32_t avg_cycle_time_us;
    uint32_t scheduler_load_percent;  /* 0-100 */
} scheduler_stats_t;

scheduler_stats_t SCHED_GetStats(void);
void SCHED_PrintStats(void);
void SCHED_PrintTaskInfo(task_handle_t* task);
void SCHED_PrintAllTasks(void);

/* === Utility === */
void SCHED_Yield(task_handle_t* task);          /* Voluntarily yield */
void SCHED_SetTaskInterval(task_handle_t* task, uint32_t ms);
void SCHED_SetTaskUserData(task_handle_t* task, void* data);
void* SCHED_GetTaskUserData(task_handle_t* task);

/* === System Time (SysTick based) === */
uint32_t SCHED_GetSystemTime(void);             /* Current time in ms */
void SCHED_Delay_ms(uint32_t ms);               /* Blocking delay (busy-wait) */

#ifdef __cplusplus
}
#endif
