/* Platform dependent includes */
#include "asdk_platform.h"

/* ASDK includes */
#include "asdk_error.h"

/* Application specific includes */
#include <stdbool.h>
#include "app_gpio.h"
#include "gpio_cfg.h"
#include "app_can.h"

/* Debug Print includes */
#include "debug_print.h"

volatile bool button_pressed = false;

static void ir_sensor_iteration(void);
static void rain_sensor_iteration(void);

/* Interrupt callback funtion for GPIO */
static void gpio_callback(asdk_mcu_pin_t mcu_pin, uint32_t pin_state)
{
    switch (mcu_pin)
    {
    case MCU_PIN_29:
        button_pressed = true;
        break;

    default:
        break;
    }
}

/* Will be called from asdk_app_init() */
void app_gpio_init()
{
    asdk_errorcode_t status = ASDK_GPIO_SUCCESS;

    /* Initialize output pins */

    for (uint8_t i=0; i<gpio_output_config_size; i++)
    {
        status = asdk_gpio_init(&gpio_output_config[i]);
        ASDK_DEV_ERROR_ASSERT(status, ASDK_GPIO_SUCCESS);
    }

    /* Initialize input pins */

    for (uint8_t i=0; i<gpio_input_config_size; i++)
    {
        status = asdk_gpio_init(&gpio_input_config[i]);
        ASDK_DEV_ERROR_ASSERT(status, ASDK_GPIO_SUCCESS);
    }

    /* Initialize gpio peripheral ISR callback */

    status = asdk_gpio_install_callback(gpio_callback);
    ASDK_DEV_ERROR_ASSERT(status, ASDK_GPIO_SUCCESS);
}

/* Will be called from asdk_app_loop() */
void app_gpio_iteration()
{
    // DEBUG_PRINTF("Iterating GPIO\r\n" );
    asdk_gpio_output_toggle(USER_LED_1);

    if (button_pressed)
    {
        // DEBUG_PRINTF("Button pressed\r\n" );

        button_pressed = false;

        asdk_gpio_output_toggle(USER_LED_2);
    }    

    ir_sensor_iteration();

    rain_sensor_iteration();
}

bool app_gpio_get_pin_state(asdk_mcu_pin_t pin)
{
    asdk_errorcode_t status = ASDK_GPIO_SUCCESS;
    asdk_gpio_state_t pin_state = ASDK_GPIO_STATE_INVALID;

    status = asdk_gpio_get_input_state(pin, &pin_state);
    ASDK_DEV_ERROR_ASSERT(status, ASDK_GPIO_SUCCESS);

    if (pin_state == ASDK_GPIO_STATE_HIGH)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void app_gpio_set_pin_state(asdk_mcu_pin_t pin, bool state)
{
    asdk_errorcode_t status = ASDK_GPIO_SUCCESS;

    if (state)
    {
        status = asdk_gpio_output_set(pin);
    }
    else
    {
        status = asdk_gpio_output_clear(pin);
    }

    ASDK_DEV_ERROR_ASSERT(status, ASDK_GPIO_SUCCESS);
}

void app_gpio_toggle(asdk_mcu_pin_t pin)
{
    asdk_errorcode_t status = asdk_gpio_output_toggle(pin);
    ASDK_DEV_ERROR_ASSERT(status, ASDK_GPIO_SUCCESS);
}

static void ir_sensor_iteration(void)
{
    /* IR Sensing */
    if (app_gpio_get_pin_state(IR1_SENSE) == false) {
        /* IR detected */
    } else {
        /* IR not detected */
    }
}

static void rain_sensor_iteration(void)
{
    /* Rain Sensing */
    if (app_gpio_get_pin_state(RAIN1_SENSE) == true) {

    } else {

    }
}
