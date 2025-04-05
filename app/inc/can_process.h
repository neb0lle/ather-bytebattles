#ifndef CAN_PROCESS_H
#define CAN_PROCESS_H

#ifdef __cplusplus
extern "C" {
#endif

void process_horn_state(void);
void process_brake_state(void);
void process_indicator_state(void);
void handle_start_button_press(void);
void update_vehicle_speed(void);

#ifdef __cplusplus
}
#endif

#endif // CAN_PROCESS_H
