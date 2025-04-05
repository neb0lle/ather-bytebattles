#include "gpio_cfg.h"
#include "ultrasonic.h"

static uint32_t current_time;
static uint32_t last_time_trigger;
static bool trigger_flag;

static bool ultrasonic_echo_flag_1;

static uint32_t distance_start_1;

static uint32_t distance_increment_1;

static uint32_t total_distance_1;
static uint32_t total_distance_2;
static uint32_t total_distance_3;
static uint32_t total_distance_4;

static void __ultrasonic_trigger(void);
static void __ultrasonic_echo(void);

static void __ultrasonic_echo(void)
{
    uint8_t ultrasonic_state1 = 0;
    
    ultrasonic_state1 = app_gpio_get_pin_state(ULTRASONIC_ECHO1);
    if (ultrasonic_state1) {
        if (ultrasonic_echo_flag_1) {
           distance_start_1 = asdk_sys_get_time_ms();
           ultrasonic_echo_flag_1 = false;
        }
        distance_increment_1 = asdk_sys_get_time_ms();
    } else {
        total_distance_1 = (distance_increment_1 - distance_start_1);
        /* NOTE: The distance calculation is based on the formula:
            object_distance = velocity of sound / (2*time)
            total_distance = total_distance * MILLI_TO_MICRO / object_distance;
        */
        /* Todo: Calculate distance */
        ultrasonic_echo_flag_1 = true;
    }    
}

static void __ultrasonic_trigger(void)
{
    current_time = asdk_sys_get_time_ms();
    if ((current_time - last_time_trigger) < 100)
        return;

    if (trigger_flag == 0) {
        app_gpio_set_pin_state(ULTRASONIC_TRIG1, 1);
        /* ASDK User Action: Add new ULTRASONIC_TRIG pin here */
        trigger_flag = true;
    } else {
        app_gpio_set_pin_state(ULTRASONIC_TRIG1, 0);
        /* ASDK User Action: Add new ULTRASONIC_TRIG pin here */
        trigger_flag = false;
        last_time_trigger = current_time;
    }
}

void ultrasonic_iterations(void)
{
    __ultrasonic_trigger();
    __ultrasonic_echo();
}

uint32_t ultrsonic_get_distance(asdk_mcu_pin_t mcu_pin)
{
    switch (mcu_pin)
    {
    case ULTRASONIC_ECHO1:
    return total_distance_1;

    case ULTRASONIC_ECHO2:
    return total_distance_2;

    case ULTRASONIC_ECHO3:
    return total_distance_3;

    case ULTRASONIC_ECHO4:
    return total_distance_4;
    
    default:
        break;
    }

    return 0;
}
