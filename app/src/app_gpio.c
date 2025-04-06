/* Platform dependent includes */
#include "asdk_platform.h"

/* ASDK includes */
#include "asdk_error.h"

/* Application specific includes */
#include "app_can.h"
#include "app_gpio.h"
#include "gpio_cfg.h"
#include <stdbool.h>
#include <stdint.h>

/* Debug Print includes */
#include "debug_print.h"

static uint32_t current_time;
static uint32_t last_trigger_time;
static bool trigger_state;
static bool echo_captured = true;
static uint32_t echo_start = 0;
static uint32_t echo_end = 0;
static uint32_t measured_distance_cm = 0;
static uint32_t prev_distance_cm = (uint32_t)-1;
static const uint32_t pothole_threshold_cm =
    10000; // Adjust this based on sensitivity

volatile bool button_pressed = false;

static void ir_sensor_iteration(void);
static void rain_sensor_iteration(void);
static void light_sensor_iteration(void);
static void ultrasonic_sensor_iteration(void);
static void obs_ultrasonic_sensor_iteration(void);

volatile bool temp1;
volatile bool temp2;
volatile bool rain_temp;

static bool raining = false;
extern volatile uint8_t indicator_state;

uint8_t tx_buffer1[8] = {0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA};
uint8_t tx_buffer2[8] = {0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA};

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
    ultrasonic_sensor_iteration();
    obs_ultrasonic_sensor_iteration();
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
        tx_buffer1[1] = (uint8_t)-30;
    } else if (temp1 && !temp2) {
        tx_buffer1[1] = (uint8_t)30;
    }
    app_can_send(0x305, tx_buffer1, 2);
}

static void light_sensor_iteration(void) {}

static uint32_t hazard_timer = 0;
static bool hazard_on = false;
static bool hazard_active = false;

void handle_rain() {
    if (indicator_state != 0) {
        // Indicators are on, so do not enable hazard lights.
        return;
    }

    tx_buffer1[0] = 0x04; // Command byte for indicators

    if (raining) {
        hazard_active = true;
        hazard_timer++; // Increment every 100ms iteration

        if (hazard_timer >= 3) {
            hazard_timer = 0;
            hazard_on = !hazard_on; // Toggle hazard state every 300ms

            if (hazard_on) {
                // Turn both indicators ON in sequence with minimal delay
                tx_buffer1[1] = 0x01; // Left indicator ON
                app_can_send(0x305, tx_buffer1, 2);

                tx_buffer1[1] = 0x02; // Right indicator ON
                app_can_send(0x305, tx_buffer1, 2);
            } else {
                // Turn both indicators OFF
                tx_buffer1[1] = 0x00; // Disable both indicators
                app_can_send(0x305, tx_buffer1, 2);
            }
        }
    } else {
        if (hazard_active) {
            // Make sure hazard lights are off when rain stops
            hazard_active = false;
            hazard_timer = 0;
            tx_buffer1[1] = 0x00; // Disable both indicators
            app_can_send(0x305, tx_buffer1, 2);
        }
    }
}
static void rain_sensor_iteration(void) {

    rain_temp = app_gpio_get_pin_state(RAIN1_SENSE);
    /* Rain Sensing */
    if (app_gpio_get_pin_state(RAIN1_SENSE) == false) {
        raining = true;
    } else {
        raining = false;
    }

    handle_rain();
}

static void ultrasonic_sensor_iteration(void) {
    current_time = asdk_sys_get_time_ms();

    // Trigger every 100ms
    if ((current_time - last_trigger_time) >= 100) {
        if (!trigger_state) {
            app_gpio_set_pin_state(ULTRASONIC_TRIG1, true); // Send 10us pulse
            trigger_state = true;
        } else {
            app_gpio_set_pin_state(ULTRASONIC_TRIG1, false);
            trigger_state = false;
            last_trigger_time = current_time;
        }
    }

    // Read echo pin state
    bool echo_state = app_gpio_get_pin_state(ULTRASONIC_ECHO1);

    if (echo_state && echo_captured) {
        // Rising edge
        echo_start = asdk_sys_get_time_ms();
        echo_captured = false;
    } else if (!echo_state && !echo_captured) {
        // Falling edge
        echo_end = asdk_sys_get_time_ms();
        uint32_t duration_ms = echo_end - echo_start;

        // Convert to cm
        measured_distance_cm = (duration_ms * 34300) / (2 * 1000); // cm

        // Pothole detection
        if (prev_distance_cm != (uint32_t)-1) {
            uint32_t diff = (measured_distance_cm > prev_distance_cm)
                                ? (measured_distance_cm - prev_distance_cm)
                                : (prev_distance_cm - measured_distance_cm);

            if (diff >= pothole_threshold_cm) {
                tx_buffer2[0] = 0x02;
                tx_buffer2[1] = 0x01;
                app_can_send(0x305, tx_buffer2, 2);
                tx_buffer2[1] = 0x00;
                app_can_send(0x305, tx_buffer2, 2);
            }
        }

        prev_distance_cm = measured_distance_cm;
        echo_captured = true;
    }
}

static void obs_ultrasonic_sensor_iteration(void) {
    static bool already_honked = false;
    current_time = asdk_sys_get_time_ms();

    // Trigger every 100ms
    if ((current_time - last_trigger_time) >= 100) {
        if (!trigger_state) {
            app_gpio_set_pin_state(ULTRASONIC_TRIG2, true); // Send 10us pulse
            trigger_state = true;
        } else {
            app_gpio_set_pin_state(ULTRASONIC_TRIG2, false);
            trigger_state = false;
            last_trigger_time = current_time;
        }
    }

    // Read echo pin state
    bool echo_state = app_gpio_get_pin_state(ULTRASONIC_ECHO2);

    if (echo_state && echo_captured) {
        // Rising edge
        echo_start = asdk_sys_get_time_ms();
        echo_captured = false;
    } else if (!echo_state && !echo_captured) {
        // Falling edge
        echo_end = asdk_sys_get_time_ms();
        uint32_t duration_ms = echo_end - echo_start;

        // Convert to cm
        measured_distance_cm =
            (duration_ms * 34300) /
            (2 * 1000); // cm
                        // static bool already_honked = false;

        if (measured_distance_cm < 100 &&
            !already_honked) {
            tx_buffer1[0] = 0x01;
            tx_buffer1[1] = 0x01;
            app_can_send(0x305, tx_buffer1, 2);
            tx_buffer1[1] = 0x00;
            app_can_send(0x305, tx_buffer1, 2);
            tx_buffer1[1] = 0x01;
            app_can_send(0x305, tx_buffer1, 2);
            tx_buffer1[1] = 0x00;
            app_can_send(0x305, tx_buffer1, 2);
            already_honked = true;
        } else if (measured_distance_cm >= 100) {
            already_honked = false;
        }

        echo_captured = true;
    }
}
