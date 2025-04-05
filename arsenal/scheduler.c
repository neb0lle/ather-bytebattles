#include "scheduler.h"
#include "asdk_timer.h"

static scheduler_t *scheduler_config_p;
static uint8_t scheduler_config_size = 0;
static uint64_t current_tick = 0;

volatile uint64_t tick_ms = 0;

static void timer_callback(asdk_timer_event_t);

static asdk_timer_t scheduler_timer_config = 
{
    .type = ASDK_TIMER_TYPE_PERIODIC,
    .mode = 
    {
        .type = ASDK_TIMER_MODE_TIMER,          // Timer mode 
        .config.timer = 
        {
            .callback = &timer_callback,
            .timer_period = 1000,               // Setting count for 1ms
        }
    },
    .direction = ASDK_TIMER_COUNT_DIRECTION_UP,
    .interrupt = 
    {
        .intr_num = ASDK_EXTI_INTR_CPU_4,       // Chosing external interrupt 4
        .enable = true,
        .priority = 3
    },
    .counter_clock = 
    {
        .frequency = 1000000,                   // choosing a clock of 1MHz
        .prescaler = ASDK_CLOCK_PRESCALER_1     // 1MHz / 1 = 1MHz
    },
};

static void timer_callback(asdk_timer_event_t timer_event)
{
    switch (timer_event)
    {
    case ASDK_TIMER_TERMINAL_COUNT_EVENT:
        tick_ms++;
        break;

    default:
        break;
    }
}

void scheduler_init(scheduler_t *scheduler_config, uint8_t size)
{
    asdk_errorcode_t status;

    scheduler_config_p = scheduler_config;
    scheduler_config_size = size;

    /* Initializing the timer channel 75 as periodic */
    status = asdk_timer_init(ASDK_TIMER_MODULE_CH_76, &scheduler_timer_config);
    ASDK_DEV_ERROR_ASSERT(status, ASDK_TIMER_SUCCESS);

    status = asdk_timer_start(ASDK_TIMER_MODULE_CH_76);
    ASDK_DEV_ERROR_ASSERT(status, ASDK_TIMER_SUCCESS);
}

void scheduler_iteration(void)
{
    current_tick = tick_ms;

    for(uint8_t i=0; i < scheduler_config_size; i++)
    {
        if(scheduler_config_p[i].periodicty == 0)
        {
            (*scheduler_config_p[i].task_fn)();
        }
        else if((current_tick - scheduler_config_p[i].last_tick) >= scheduler_config_p[i].periodicty)
        {
            (*scheduler_config_p[i].task_fn)();
            scheduler_config_p[i].last_tick = current_tick;
        }
    }
}
