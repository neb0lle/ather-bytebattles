#ifndef DEFAULTS_H
#define DEFAULTS_H

#define GPIO_OUTPUT_CONFIG(pin) {\
    .mcu_pin = pin, \
    .gpio_mode = ASDK_GPIO_MODE_OUTPUT, \
    .gpio_init_state = ASDK_GPIO_STATE_LOW, \
    .gpio_pull = ASDK_GPIO_PUSH_PULL \
}

#define GPIO_INPUT_CONFIG(pin) {\
    .mcu_pin = pin, \
    .gpio_mode = ASDK_GPIO_MODE_INPUT, \
    .gpio_pull = ASDK_GPIO_HIGH_Z, \
    .interrupt_config = {0} \
}

#define GPIO_INPUT_CONFIG_WITH_INTERRUPT(pin) {\
    .mcu_pin = pin, \
    .gpio_mode = ASDK_GPIO_MODE_INPUT, \
    .gpio_pull = ASDK_GPIO_HIGH_Z, \
    .interrupt_config = {   \
        .intr_num = ASDK_EXTI_INTR_CPU_4, \
        .type = ASDK_GPIO_INTERRUPT_FALLING_EDGE, \
        .priority = 3, \
    } \
}

#define COLOR_SENSOR_INPUT(pin) {\
    .mcu_pin = pin, \
    .gpio_mode = ASDK_GPIO_MODE_INPUT, \
    .gpio_pull = ASDK_GPIO_HIGH_Z, \
    .interrupt_config = {   \
        .intr_num = ASDK_EXTI_INTR_CPU_4, \
        .type = ASDK_GPIO_INTERRUPT_BOTH_EDGES, \
        .priority = 3, \
    } \
}

#endif /* DEFAULTS_H */
