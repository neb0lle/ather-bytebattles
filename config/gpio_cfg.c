#include "app_gpio.h"

#include "gpio_cfg.h"
#include "defaults.h"

/* ASDK User Action: Add new output pins here */

asdk_gpio_config_t gpio_output_config[] = {
    GPIO_OUTPUT_CONFIG(USER_LED_1),
    GPIO_OUTPUT_CONFIG(USER_LED_2),
    GPIO_OUTPUT_CONFIG(ULTRASONIC_TRIG1),
};

/* ASDK User Action: Add new input pins here */

asdk_gpio_config_t gpio_input_config[] = {
    GPIO_INPUT_CONFIG(USER_BUTTON),
    GPIO_INPUT_CONFIG(IR1_SENSE),
    GPIO_INPUT_CONFIG(ULTRASONIC_ECHO1),
    GPIO_INPUT_CONFIG(RAIN1_SENSE),
    // GPIO_INPUT_CONFIG_WITH_INTERRUPT(USER_BUTTON),
};

/* ******** CAUTION! Do not edit below code ******** */

const uint8_t gpio_output_config_size = sizeof(gpio_output_config) / sizeof(gpio_output_config[0]);
const uint8_t gpio_input_config_size = sizeof(gpio_input_config) / sizeof(gpio_input_config[0]);
