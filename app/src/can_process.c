/* CAN Process Handling */
#include "app_can.h"
#include <string.h>

extern volatile bool horn_state;
extern volatile bool brake_state;
extern volatile uint8_t indicator_state;
extern volatile uint8_t riding_mode; // module to vehicle 0
extern volatile int8_t throttle;
extern volatile uint8_t vehicle_speed;
extern volatile bool sidestand_engaged;
extern volatile bool start_button;
extern volatile bool prev_start_button;

extern volatile uint8_t riding_mode_r; // reading from vehicle to module
extern volatile uint8_t vehicle_speed_r;
extern volatile int16_t roll;
extern volatile int16_t pitch;
extern volatile int16_t yaw;

static uint8_t tx_buffer[8];
static uint8_t tx306_buffer[8];

void process_horn_state() {
    tx_buffer[0] = 0x01;
    tx_buffer[1] = horn_state ? 0x01 : 0x00;
    app_can_send(0x305, tx_buffer, 2);
}

void process_brake_state() {
    tx_buffer[0] = 0x03;
    tx_buffer[1] = brake_state ? 0x02 : 0x01;
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
            return; // Invalid, do nothing
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
    if (riding_mode == 0) {
        static bool next_is_reverse = true;

        if (next_is_reverse)
            riding_mode = 2; // Neutral -> Reverse
        else
            riding_mode = 1; // Neutral -> Forward

        next_is_reverse = !next_is_reverse; // Toggle for next time
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
    static bool hold_active = false;
    static bool manual_brake_requested = false;
    static uint8_t previous_riding_mode = 1;

    // Start hold sequence only in Forward or Reverse
    if ((riding_mode == 1 || riding_mode == 2) && brake_state && !hold_active) {
        if (pitch > 10 || pitch < -10) {
            vehicle_speed = 0;
            manual_brake_requested = true;
            previous_riding_mode = riding_mode;
        }
    }

    // Once vehicle is stopped with brake
    if (manual_brake_requested && vehicle_speed == 0) {
        if (pitch > 10) {
            riding_mode = 3; // HoldUp
        } else if (pitch < -10) {
            riding_mode = 4; // HoldDown
        }

        hold_active = true;
        manual_brake_requested = false;
    }

    // Exit only on throttle input
    if (hold_active && throttle > 0) {
        if (previous_riding_mode == 1 || previous_riding_mode == 2) {
            riding_mode = previous_riding_mode;
        } else {
            riding_mode = 0; // Fallback to Neutral
        }

        hold_active = false;
    }

    // Failsafe: If riding mode is Hold but came from Neutral, revert
    if ((riding_mode == 3 || riding_mode == 4) && previous_riding_mode == 0) {
        riding_mode = 0;
        hold_active = false;
        manual_brake_requested = false;
    }

    // Send updated state
    tx306_buffer[0] = riding_mode;
    tx306_buffer[1] = vehicle_speed;
    app_can_send(0x306, tx306_buffer, 2);
}
