#include "scheduler.h"
#include <string.h>
#include <stdio.h>

/* ============================================================================
 * SYSTEM TIME (SysTick)
 * ============================================================================ */

/* Global system time counter (incremented by SysTick interrupt) */
static volatile uint32_t g_system_time_ms = 0;

/**
 * Initialize SysTick for 1ms interrupts
 * Call this from platform initialization
 */
void SCHED_SysTick_Init(void) {
    /* This would be called from platform layer to setup SysTick
       For now, we assume platform already initializes it */
}

/**
 * SysTick interrupt handler - MUST be called every 1ms
 * Integrate with your STM32 HAL/LL SysTick handler
 */
void SCHED_SysTick_Handler(void) {
    g_system_time_ms++;
}

uint32_t SCHED_GetSystemTime(void) {
    return g_system_time_ms;
}

void SCHED_Delay_ms(uint32_t ms) {
    uint32_t start = g_system_time_ms;
    while ((g_system_time_ms - start) < ms);
}

/* ============================================================================
 * SCHEDULER STATE
 * ============================================================================ */

typedef struct {
    task_handle_t tasks[TASK_MAX_COUNT];
    uint8_t task_count;
    bool running;
    uint32_t start_time_ms;
    
    /* Statistics */
    uint32_t total_cycles;
    uint32_t max_cycle_us;
    uint32_t min_cycle_us;
} scheduler_context_t;

static scheduler_context_t g_scheduler = {0};

/* ============================================================================
 * HELPER FUNCTIONS
 * ============================================================================ */

/**
 * Compare task priorities for sorting
 */
static int task_priority_cmp(const void* a, const void* b) {
    const task_handle_t* ta = (const task_handle_t*)a;
    const task_handle_t* tb = (const task_handle_t*)b;
    /* Sort by priority (descending) */
    return tb->priority - ta->priority;
}

/**
 * Find task index by handle
 */
static int task_find_index(task_handle_t* task) {
    for (int i = 0; i < g_scheduler.task_count; i++) {
        if (&g_scheduler.tasks[i] == task) {
            return i;
        }
    }
    return -1;
}

/**
 * Find next ready task (highest priority, oldest)
 */
static task_handle_t* find_next_ready_task(void) {
    task_handle_t* best = NULL;
    uint32_t now = SCHED_GetSystemTime();
    
    for (int i = 0; i < g_scheduler.task_count; i++) {
        task_handle_t* task = &g_scheduler.tasks[i];
        
        /* Skip if not ready or stopped */
        if (task->state != TASK_READY && task->state != TASK_WAITING) {
            continue;
        }
        
        /* Skip if waiting for timeout */
        if (task->state == TASK_WAITING && now < task->next_run_ms) {
            continue;
        }
        
        /* Skip stopped tasks */
        if (task->state == TASK_STOPPED) {
            continue;
        }
        
        /* Check if it's time to run */
        if (now >= task->next_run_ms) {
            /* Prefer highest priority */
            if (!best || task->priority > best->priority) {
                best = task;
            }
            /* If same priority, prefer oldest (FIFO) */
            else if (task->priority == best->priority && 
                     task->next_run_ms < best->next_run_ms) {
                best = task;
            }
        }
    }
    
    return best;
}

/* ============================================================================
 * SCHEDULER CONTROL
 * ============================================================================ */

void SCHED_Init(void) {
    memset(&g_scheduler, 0, sizeof(scheduler_context_t));
    g_scheduler.running = false;
    g_scheduler.start_time_ms = SCHED_GetSystemTime();
    g_scheduler.min_cycle_us = 0xFFFFFFFF;
}

void SCHED_Stop(void) {
    g_scheduler.running = false;
}

bool SCHED_IsRunning(void) {
    return g_scheduler.running;
}

/**
 * Main scheduler loop - call this from main()
 */
void SCHED_Run(void) {
    g_scheduler.running = true;
    
    while (g_scheduler.running) {
        uint32_t cycle_start = SCHED_GetSystemTime();
        
        /* Find next ready task */
        task_handle_t* task = find_next_ready_task();
        
        if (!task) {
            /* No task ready - optionally sleep/wait */
            __asm("nop");  /* Avoid busy loop */
            continue;
        }
        
        /* Execute task */
        task->state = TASK_RUNNING;
        uint32_t exec_start = SCHED_GetSystemTime();
        
        uint32_t next_interval = task->func(task);
        
        uint32_t exec_time = SCHED_GetSystemTime() - exec_start;
        
        /* Update task statistics */
        task->stats.run_count++;
        task->stats.total_runtime_us += exec_time * 1000;  /* Approximate */
        task->stats.max_runtime_us = (exec_time * 1000 > task->stats.max_runtime_us) ? 
                                      (exec_time * 1000) : task->stats.max_runtime_us;
        if (task->stats.min_runtime_us == 0 || exec_time * 1000 < task->stats.min_runtime_us) {
            task->stats.min_runtime_us = exec_time * 1000;
        }
        task->stats.last_run_ms = exec_start;
        
        /* Schedule next run */
        if (next_interval == 0) {
            /* Task wants to run ASAP */
            task->next_run_ms = SCHED_GetSystemTime();
            task->state = TASK_READY;
        } else {
            /* Task specifies next interval */
            task->next_run_ms = SCHED_GetSystemTime() + next_interval;
            task->state = TASK_WAITING;
        }
        
        /* Update scheduler statistics */
        uint32_t cycle_time = SCHED_GetSystemTime() - cycle_start;
        g_scheduler.total_cycles++;
        if (cycle_time > g_scheduler.max_cycle_us) {
            g_scheduler.max_cycle_us = cycle_time * 1000;
        }
        if (cycle_time < g_scheduler.min_cycle_us) {
            g_scheduler.min_cycle_us = cycle_time * 1000;
        }
    }
}

/* ============================================================================
 * TASK MANAGEMENT
 * ============================================================================ */

task_handle_t* SCHED_CreateTask(
    const char* name,
    task_func_t func,
    task_priority_t priority,
    uint32_t interval_ms,
    void* user_data)
{
    if (g_scheduler.task_count >= TASK_MAX_COUNT) {
        return NULL;  /* Task table full */
    }
    
    task_handle_t* task = &g_scheduler.tasks[g_scheduler.task_count];
    
    task->name = name;
    task->func = func;
    task->priority = priority;
    task->interval_ms = interval_ms;
    task->user_data = user_data;
    task->state = TASK_STOPPED;
    task->next_run_ms = SCHED_GetSystemTime();
    task->creation_time_ms = SCHED_GetSystemTime();
    memset(&task->stats, 0, sizeof(task_stats_t));
    
    g_scheduler.task_count++;
    
    return task;
}

task_handle_t* SCHED_CreateTask_Default(
    const char* name,
    task_func_t func)
{
    return SCHED_CreateTask(name, func, TASK_PRIORITY_NORMAL, 0, NULL);
}

void SCHED_DestroyTask(task_handle_t* task) {
    if (!task) return;
    
    int idx = task_find_index(task);
    if (idx < 0) return;
    
    /* Shift remaining tasks */
    if (idx < g_scheduler.task_count - 1) {
        memmove(&g_scheduler.tasks[idx], 
                &g_scheduler.tasks[idx + 1],
                (g_scheduler.task_count - idx - 1) * sizeof(task_handle_t));
    }
    
    g_scheduler.task_count--;
}

task_handle_t* SCHED_FindTask(const char* name) {
    for (int i = 0; i < g_scheduler.task_count; i++) {
        if (strcmp(g_scheduler.tasks[i].name, name) == 0) {
            return &g_scheduler.tasks[i];
        }
    }
    return NULL;
}

uint8_t SCHED_GetTaskCount(void) {
    return g_scheduler.task_count;
}

/* ============================================================================
 * TASK CONTROL
 * ============================================================================ */

void SCHED_StartTask(task_handle_t* task) {
    if (task) task->state = TASK_READY;
}

void SCHED_StopTask(task_handle_t* task) {
    if (task) task->state = TASK_STOPPED;
}

void SCHED_PauseTask(task_handle_t* task) {
    if (task && task->state == TASK_READY) {
        task->state = TASK_WAITING;
    }
}

void SCHED_ResumeTask(task_handle_t* task) {
    if (task && task->state == TASK_WAITING) {
        task->state = TASK_READY;
        task->next_run_ms = SCHED_GetSystemTime();
    }
}

void SCHED_DelayTask(task_handle_t* task, uint32_t ms) {
    if (task) {
        task->state = TASK_WAITING;
        task->next_run_ms = SCHED_GetSystemTime() + ms;
    }
}

void SCHED_Yield(task_handle_t* task) {
    if (task) {
        task->state = TASK_READY;
        task->next_run_ms = SCHED_GetSystemTime();
    }
}

void SCHED_SetTaskInterval(task_handle_t* task, uint32_t ms) {
    if (task) task->interval_ms = ms;
}

void SCHED_SetTaskUserData(task_handle_t* task, void* data) {
    if (task) task->user_data = data;
}

void* SCHED_GetTaskUserData(task_handle_t* task) {
    return task ? task->user_data : NULL;
}

/* ============================================================================
 * TASK QUERIES
 * ============================================================================ */

task_state_t SCHED_GetTaskState(task_handle_t* task) {
    return task ? task->state : TASK_STOPPED;
}

const char* SCHED_GetTaskStateStr(task_state_t state) {
    switch (state) {
        case TASK_STOPPED:  return "STOPPED";
        case TASK_READY:    return "READY";
        case TASK_RUNNING:  return "RUNNING";
        case TASK_WAITING:  return "WAITING";
        case TASK_ERROR:    return "ERROR";
        default:            return "UNKNOWN";
    }
}

uint32_t SCHED_GetTaskRunCount(task_handle_t* task) {
    return task ? task->stats.run_count : 0;
}

uint32_t SCHED_GetTaskLastRunTime(task_handle_t* task) {
    return task ? task->stats.last_run_ms : 0;
}

uint32_t SCHED_GetTaskAverageRuntime(task_handle_t* task) {
    if (!task || task->stats.run_count == 0) return 0;
    return task->stats.total_runtime_us / task->stats.run_count;
}

task_stats_t SCHED_GetTaskStats(task_handle_t* task) {
    if (task) return task->stats;
    return (task_stats_t){0};
}

uint32_t SCHED_GetElapsedTime(task_handle_t* task) {
    if (!task) return 0;
    return SCHED_GetSystemTime() - task->creation_time_ms;
}

uint32_t SCHED_GetUptime(void) {
    return SCHED_GetSystemTime() - g_scheduler.start_time_ms;
}

/* ============================================================================
 * SCHEDULER STATISTICS
 * ============================================================================ */

scheduler_stats_t SCHED_GetStats(void) {
    scheduler_stats_t stats = {0};
    stats.task_count = g_scheduler.task_count;
    stats.total_runtime_us = 0;
    stats.max_cycle_time_us = g_scheduler.max_cycle_us;
    stats.min_cycle_time_us = g_scheduler.min_cycle_us;
    
    if (g_scheduler.total_cycles > 0) {
        stats.avg_cycle_time_us = stats.total_runtime_us / g_scheduler.total_cycles;
    }
    
    for (int i = 0; i < g_scheduler.task_count; i++) {
        stats.total_runtime_us += g_scheduler.tasks[i].stats.total_runtime_us;
    }
    
    return stats;
}

void SCHED_PrintTaskInfo(task_handle_t* task) {
    if (!task) return;
    
    printf("\nTask: %s\n", task->name);
    printf("  State: %s\n", SCHED_GetTaskStateStr(task->state));
    printf("  Priority: %lu\n", task->priority);
    printf("  Interval: %lu ms\n", task->interval_ms);
    printf("  Run Count: %lu\n", task->stats.run_count);
    printf("  Total Runtime: %lu μs\n", task->stats.total_runtime_us);
    printf("  Max Runtime: %lu μs\n", task->stats.max_runtime_us);
    printf("  Min Runtime: %lu μs\n", task->stats.min_runtime_us);
    printf("  Avg Runtime: %lu μs\n", 
           task->stats.run_count > 0 ? task->stats.total_runtime_us / task->stats.run_count : 0);
    printf("  Last Run: %u ms ago\n", SCHED_GetSystemTime() - task->stats.last_run_ms);
}

void SCHED_PrintAllTasks(void) {
    printf("\n=== SCHEDULER TASKS (%lu active) ===\n", g_scheduler.task_count);
    for (int i = 0; i < g_scheduler.task_count; i++) {
        printf("\n[%d] %s\n", i, g_scheduler.tasks[i].name);
        printf("    State: %s | Priority: %u | Interval: %lu ms\n",
               SCHED_GetTaskStateStr(g_scheduler.tasks[i].state),
               g_scheduler.tasks[i].priority,
               g_scheduler.tasks[i].interval_ms);
        printf("    Runs: %lu | Avg: %lu μs | Max: %lu μs\n",
               g_scheduler.tasks[i].stats.run_count,
               g_scheduler.tasks[i].stats.run_count > 0 ? 
                   g_scheduler.tasks[i].stats.total_runtime_us / g_scheduler.tasks[i].stats.run_count : 0,
               g_scheduler.tasks[i].stats.max_runtime_us);
    }
    printf("===============================\n\n");
}

void SCHED_PrintStats(void) {
    scheduler_stats_t stats = SCHED_GetStats();
    
    printf("\n=== SCHEDULER STATISTICS ===\n");
    printf("Active Tasks: %u/%u\n", stats.task_count, TASK_MAX_COUNT);
    printf("Total Cycles: %lu\n", g_scheduler.total_cycles);
    printf("Uptime: %lu ms\n", SCHED_GetUptime());
    printf("\nCycle Times:\n");
    printf("  Max: %lu μs\n", stats.max_cycle_time_us);
    printf("  Min: %lu μs\n", stats.min_cycle_time_us);
    printf("  Avg: %lu μs\n", stats.avg_cycle_time_us);
    printf("  Load: %lu%%\n", stats.scheduler_load_percent);
    printf("Total Task Runtime: %lu μs\n", stats.total_runtime_us);
    printf("=============================\n\n");
}
