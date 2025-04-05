#ifndef APP_GPIO_H
#define APP_GPIO_H

/* ASDK includes */
#include "asdk_gpio.h"
#include "asdk_mcu_pins.h"

#include "stdbool.h"

extern asdk_gpio_config_t gpio_output_config[];
extern asdk_gpio_config_t gpio_input_config[];

extern const uint8_t gpio_output_config_size;
extern const uint8_t gpio_input_config_size;

/* Application specific APIs */
void app_gpio_init();
void app_gpio_iteration();
void app_gpio_toggle(asdk_mcu_pin_t pin);
void app_gpio_set_pin_state(asdk_mcu_pin_t pin, bool state);
bool app_gpio_get_pin_state(asdk_mcu_pin_t pin);

#endif
