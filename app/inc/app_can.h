#ifndef APP_CAN_H
#define APP_CAN_H

#include "asdk_platform.h"
#include "asdk_can_service.h"

#define VEHICLE_CAN ASDK_CAN_MODULE_CAN_CH_1

void app_can_init();
void app_can_deinit();
void app_can_iteration();
void app_can_send(uint32_t can_id, uint8_t *data, uint8_t data_length);

#endif // APP_CAN_H
