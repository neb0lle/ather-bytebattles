#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include "asdk_mcu_pins.h"
#include "app_gpio.h"

void ultrasonic_iterations(void);
uint32_t ultrsonic_get_distance(asdk_mcu_pin_t mcu_pin);

#endif /* ULTRASONIC_H */