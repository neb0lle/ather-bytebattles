/* CAN Process Handling */
#include "app_can.h"
#include <string.h>

extern volatile bool horn_state;
extern volatile bool brake_state;
extern volatile uint8_t indicator_state;
extern volatile uint8_t riding_mode; // 0 = Neutral, 1 = Forward, 2 = Reverse, 3
                                     // = HoldUp, 4 = HoldDown
extern volatile int8_t throttle;
extern volatile uint8_t vehicle_speed;
extern volatile bool sidestand_engaged;
extern volatile bool start_button;
extern volatile bool prev_start_button;

extern volatile uint8_t riding_mode_r;
extern volatile uint8_t vehicle_speed_r;
extern volatile int16_t roll;
extern volatile int16_t pitch;
extern volatile int16_t yaw;

volatile bool hold_active = false;

static uint8_t tx_buffer[8];
static uint8_t tx306_buffer[8];

void process_horn_state() {
    tx_buffer[0] = 0x01;
    tx_buffer[1] = horn_state ? 0x01 : 0x00;
    app_can_send(0x305, tx_buffer, 2);
}

void process_brake_state() {
    tx_buffer[0] = 0x03;
    tx_buffer[1] = brake_state ? 0x02 : 0x00;
    app_can_send(0x305, tx_buffer, 2);
}

void process_indicator_state() {
    static bool indicator_on = false;
    static uint32_t indicator_timer = 0;
    static bool indicator_active = false;
    static uint8_t previous_indicator_state = 0x00;

    if (indicator_state == 0x00 && previous_indicator_state != 0x00) {
        indicator_active = true;
    }

    if (indicator_state != 0x00) {
        previous_indicator_state = indicator_state;
        indicator_active = true;
    }

    if (indicator_active) {
        indicator_timer++;
        if (indicator_timer >= 3) {
            indicator_timer = 0;
            indicator_on = !indicator_on;
        }
    }

    tx_buffer[0] = 0x04;

    if (indicator_on) {
        switch (previous_indicator_state) {
        case 0x01:
            tx_buffer[1] = 0x01; // left
            break;
        case 0x02:
            tx_buffer[1] = 0x02; // right
            break;
        default:
            return; // Invalid
        }
    } else {
        tx_buffer[1] = 0x00; // OFF
    }

    app_can_send(0x305, tx_buffer, 2);

    if (indicator_state == 0x00) {
        indicator_active = false;
    }
}

void handle_start_button_press() {
    if (hold_active)
        return; // Don't allow start mode toggle during hill hold

    if (riding_mode == 0) {
        static bool next_is_reverse = true;

        if (next_is_reverse)
            riding_mode = 2; // Neutral -> Reverse
        else
            riding_mode = 1; // Neutral -> Forward

        next_is_reverse = !next_is_reverse;
    } else {
        riding_mode = 0; // Reverse or Forward -> Neutral
    }
}

void update_vehicle_speed() {
    static bool sidestand_stop = false;

    if (sidestand_engaged && throttle == 0) {
        vehicle_speed = 0;
        riding_mode = 0;
        sidestand_stop = true;
    } else if (!sidestand_engaged || throttle > 0) {
        sidestand_stop = false;

        if (throttle > 0) {
            if (riding_mode == 1)
                vehicle_speed = 5;
            else if (riding_mode == 2)
                vehicle_speed = 3;
            else
                vehicle_speed = 0;
        } else {
            vehicle_speed = 0;
        }
    }

    if (sidestand_stop) {
        vehicle_speed = 0;
        riding_mode = 0;
    }

    tx306_buffer[0] = riding_mode;
    tx306_buffer[1] = vehicle_speed;

    app_can_send(0x306, tx306_buffer, 2);
}

void process_hold_state() {
    static uint8_t previous_riding_mode = 1;

// Thresholds for hill detection
#define INCLINE_THRESHOLD 10
#define DECLINE_THRESHOLD -10

    // Activate Hold mode only if all 3 conditions are met
    if (!hold_active &&
        (riding_mode == 1 || riding_mode == 2 || riding_mode == 5) &&
        brake_state && vehicle_speed == 0 &&
        (pitch >= INCLINE_THRESHOLD || pitch <= DECLINE_THRESHOLD)) {

        previous_riding_mode = riding_mode;

        if (pitch >= INCLINE_THRESHOLD) {
            riding_mode =
                (riding_mode == 5) ? 4 : 3; // Reverse -> HoldDown, else HoldUp
        } else if (pitch <= DECLINE_THRESHOLD) {
            riding_mode =
                (riding_mode == 5) ? 3 : 4; // Reverse -> HoldUp, else HoldDown
        }

        hold_active = true;
    }

    // Exit Hold mode only on throttle input
    if (hold_active && throttle > 0) {
        riding_mode = previous_riding_mode;
        hold_active = false;
    }

    // Failsafe: Prevent hold from Neutral
    if ((riding_mode == 3 || riding_mode == 4) && previous_riding_mode == 0) {
        riding_mode = 0;
        hold_active = false;
    }

    // Transmit updated state
    tx306_buffer[0] = riding_mode;
    tx306_buffer[1] = vehicle_speed;
    app_can_send(0x306, tx306_buffer, 2);
}
