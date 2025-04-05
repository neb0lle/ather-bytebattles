#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "asdk_platform.h"

typedef void (*task_t)(void);

typedef struct
{
    const task_t task_fn;
    uint64_t last_tick;
    const uint64_t periodicty;
} scheduler_t;

void scheduler_init(scheduler_t *scheduler_config, uint8_t size);
void scheduler_iteration(void);

#endif /* SCHEDULER_H */
