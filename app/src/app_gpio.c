/* Platform dependent includes */
#include "asdk_platform.h"

/* ASDK includes */
#include "asdk_error.h"

/* Application specific includes */
#include "app_can.h"
#include "app_gpio.h"
#include "gpio_cfg.h"
#include <stdbool.h>

/* Debug Print includes */
#include "debug_print.h"

volatile bool button_pressed = false;

static void ir_sensor_iteration(void);
static void rain_sensor_iteration(void);
static void light_sensor_iteration(void);
volatile bool temp1;
volatile bool temp2;
int8_t tx_buffer1[8] = {0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA};

/* Interrupt callback function for GPIO */
static void gpio_callback(asdk_mcu_pin_t mcu_pin, uint32_t pin_state) {
    switch (mcu_pin) {
    case MCU_PIN_29:
        button_pressed = true;
        break;

    default:
        break;
    }
}

/* Will be called from asdk_app_init() */
void app_gpio_init() {
    asdk_errorcode_t status = ASDK_GPIO_SUCCESS;

    /* Initialize output pins */

    for (uint8_t i = 0; i < gpio_output_config_size; i++) {
        status = asdk_gpio_init(&gpio_output_config[i]);
        ASDK_DEV_ERROR_ASSERT(status, ASDK_GPIO_SUCCESS);
    }

    /* Initialize input pins */

    for (uint8_t i = 0; i < gpio_input_config_size; i++) {
        status = asdk_gpio_init(&gpio_input_config[i]);
        ASDK_DEV_ERROR_ASSERT(status, ASDK_GPIO_SUCCESS);
    }

    /* Initialize gpio peripheral ISR callback */

    status = asdk_gpio_install_callback(gpio_callback);
    ASDK_DEV_ERROR_ASSERT(status, ASDK_GPIO_SUCCESS);
}

/* Will be called from asdk_app_loop() */
void app_gpio_iteration() {
    // DEBUG_PRINTF("Iterating GPIO\r\n" );
    asdk_gpio_output_toggle(USER_LED_1);

    if (button_pressed) {
        // DEBUG_PRINTF("Button pressed\r\n" );

        button_pressed = false;

        asdk_gpio_output_toggle(USER_LED_2);
    }

    ir_sensor_iteration();
    light_sensor_iteration();
    rain_sensor_iteration();
}

bool app_gpio_get_pin_state(asdk_mcu_pin_t pin) {
    asdk_errorcode_t status = ASDK_GPIO_SUCCESS;
    asdk_gpio_state_t pin_state = ASDK_GPIO_STATE_INVALID;

    status = asdk_gpio_get_input_state(pin, &pin_state);
    ASDK_DEV_ERROR_ASSERT(status, ASDK_GPIO_SUCCESS);

    if (pin_state == ASDK_GPIO_STATE_HIGH) {
        return true;
    } else {
        return false;
    }
}

void app_gpio_set_pin_state(asdk_mcu_pin_t pin, bool state) {
    asdk_errorcode_t status = ASDK_GPIO_SUCCESS;

    if (state) {
        status = asdk_gpio_output_set(pin);
    } else {
        status = asdk_gpio_output_clear(pin);
    }

    ASDK_DEV_ERROR_ASSERT(status, ASDK_GPIO_SUCCESS);
}

void app_gpio_toggle(asdk_mcu_pin_t pin) {
    asdk_errorcode_t status = asdk_gpio_output_toggle(pin);
    ASDK_DEV_ERROR_ASSERT(status, ASDK_GPIO_SUCCESS);
}

static void ir_sensor_iteration(void) {
    /* IR Sensing */

    bool read_IR1 = app_gpio_get_pin_state(IR1_SENSE);
    bool read_IR2 = app_gpio_get_pin_state(IR2_SENSE);

    temp1 = read_IR1;
    temp2 = read_IR2;

    tx_buffer1[0] = 0x05;
    tx_buffer1[1] = 0x00;
    if (!temp1 && temp2) {
        tx_buffer1[1] = -30;
    } else if (temp1 && !temp2) {
        tx_buffer1[1] = 30;
    }
    app_can_send(0x305, tx_buffer1, 2);
}

static void light_sensor_iteration(void) {
}

static void rain_sensor_iteration(void)
{

    /* Rain Sensing */
    if (app_gpio_get_pin_state(RAIN1_SENSE) == true) {
        raining = true;
    } else {
        raining=false;
    }

    handle_rain();
}

void handle_rain(){
    if(indicator_state != 0){
        // Indicators are on, so do not enable hazard lights.
        return;
    }

    tx_buffer1[0]=0x04;
    if(raining){
        // Turn on hazard lights (assuming 0x01 and 0x02 are left/right respectively)
        tx_buffer1[1]=0x01;
        app_can_send(0x305,tx_buffer1,2);
        tx_buffer1[1]=0x02;
    }
    else{
        // Turn off hazard lights
        tx_buffer1[1]=0x00;
    }
    app_can_send(0x305,tx_buffer1,2);
}
