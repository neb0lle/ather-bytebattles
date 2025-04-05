/* Platform dependent includes */
#include "asdk_platform.h"

/* ASDK includes */
#include "asdk_error.h"
#include "asdk_can.h"
#include "asdk_timer.h"

/* Application specific includes */
#include <string.h>
#include "app_can.h"
#include "app_gpio.h"
#include "colour.h"
#include "gpio_cfg.h"

static colour_sensor_filter_t current_filter = NO_FILTER;
static uint64_t first_edge_time = 0, second_edge_time = 0, _pulse_duration_in_us = 0;
static volatile bool first_edge_completed_flag = false;

asdk_timer_t colour_pulse_timer = {
    .type = ASDK_TIMER_TYPE_PERIODIC,
    .mode = {
        .type = ASDK_TIMER_MODE_TIMER,
        .config =  {
            .timer = {
                .timer_period = 0xFFFFFFFF,
            },
        },
    },
    .direction = ASDK_TIMER_COUNT_DIRECTION_UP,
    .interrupt = {
        .enable = false,
        .priority = 3,
        .intr_num = ASDK_EXTI_INTR_CPU_4,
    },
    .counter_clock = {
        .frequency = 1000000,
        .prescaler = ASDK_CLOCK_PRESCALER_1,
    }
};

void set_colour_sensor_filter(colour_sensor_filter_t filter);

void colour_sensor_callback(uint32_t pin_state)
{
    if(pin_state) // Rising edge
    {
        asdk_timer_get_counter_in_us(ASDK_TIMER_MODULE_CH_77, colour_pulse_timer.counter_clock, &first_edge_time);
        first_edge_completed_flag = true;
    }
    else if(pin_state == ASDK_GPIO_STATE_LOW && first_edge_completed_flag) // Falling edge
    {
        asdk_timer_get_counter_in_us(ASDK_TIMER_MODULE_CH_77, colour_pulse_timer.counter_clock, &second_edge_time);
        _pulse_duration_in_us = second_edge_time - first_edge_time;
    }
}

void colour_sensor_init(void)
{
    asdk_errorcode_t timer_status = ASDK_TIMER_SUCCESS;

    timer_status = asdk_timer_init(ASDK_TIMER_MODULE_CH_77, &colour_pulse_timer);
    ASDK_DEV_ERROR_ASSERT(timer_status, ASDK_TIMER_SUCCESS);

    timer_status = asdk_timer_start(ASDK_TIMER_MODULE_CH_77);
    ASDK_DEV_ERROR_ASSERT(timer_status, ASDK_TIMER_SUCCESS);

    set_colour_sensor_filter(NO_FILTER);
}

colour_sensor_filter_t get_current_filter()
{
    return current_filter;
}


void set_colour_sensor_filter(colour_sensor_filter_t filter)
{
    current_filter = filter;
    first_edge_completed_flag = false;
    first_edge_time = 0;
    second_edge_time = 0;
    _pulse_duration_in_us = 0;
    switch(filter)
    {
        case NO_FILTER:
            asdk_gpio_output_set(COLOUR_SENSOR_S2);
            asdk_gpio_output_clear(COLOUR_SENSOR_S3);
            break;
        case RED_FILTER:
            asdk_gpio_output_clear(COLOUR_SENSOR_S2);
            asdk_gpio_output_clear(COLOUR_SENSOR_S3);
            break;
        case GREEN_FILTER:
            asdk_gpio_output_set(COLOUR_SENSOR_S2);
            asdk_gpio_output_set(COLOUR_SENSOR_S3);
            break;
        case BLUE_FILTER:
            asdk_gpio_output_clear(COLOUR_SENSOR_S2);
            asdk_gpio_output_set(COLOUR_SENSOR_S3);
            break;
        default:
            break;
    }
}

void get_colour_sensor_reading(colour_components_t *components)
{
    asdk_sys_disable_interrupts();
    switch(current_filter)
    {
        case NO_FILTER:
        {
            components->clear = _pulse_duration_in_us;
            break;
        }
        case RED_FILTER:
        {
            components->red = _pulse_duration_in_us;
            break;
        }
        case GREEN_FILTER:
        {
            components->green = _pulse_duration_in_us;
            break;
        }
        case BLUE_FILTER:
        {
            components->blue = _pulse_duration_in_us;
            break;
        }
        default:
            break;
    }
    asdk_sys_enable_interrupts();
}

void colour_sensor_iteration(void)
{
    asdk_sys_disable_interrupts();
    current_filter = (current_filter + 1) % MAX_FILTER;
    set_colour_sensor_filter(current_filter);
    asdk_sys_enable_interrupts();
}
