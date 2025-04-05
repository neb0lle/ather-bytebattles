/* Platform dependent includes */
#include "asdk_platform.h"

/* ASDK includes */
#include "asdk_error.h"

/* Application specific includes */
#include "app_can.h"
#include "can_process.h"
#include <string.h>

/* Debug Print includes */
#include "debug_print.h"

#define CAN_TX_PIN MCU_PIN_4
#define CAN_RX_PIN MCU_PIN_5

volatile uint32_t can_error_count = 0;
volatile uint32_t can_busoff_count = 0;
volatile bool can_bus_off = false;

volatile bool horn_state = false;
volatile bool brake_state = false;
volatile uint8_t indicator_state = 0;
volatile uint8_t riding_mode = 0;
volatile int8_t throttle = 0;
volatile uint8_t vehicle_speed = 0;
volatile bool start_button = 0;
volatile bool prev_start_button = 0;
volatile bool sidestand_engaged = false;

volatile uint8_t riding_mode_r = 0;
volatile uint8_t vehicle_speed_r = 0;
volatile int16_t roll = 0;
volatile int16_t pitch = 0;
volatile int16_t yaw = 0;

uint8_t tx_buffer[8] = {0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA};
uint8_t tx306_buffer[8] = {0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA};
uint8_t rx_buffer[8] = {0};

uint32_t rx_accept_can_ids[] = {0x300, 0x301};
uint8_t can_ids_length =
    sizeof(rx_accept_can_ids) / sizeof(rx_accept_can_ids[0]);

asdk_can_message_t rx_msg = {
    .message = rx_buffer,
};

asdk_can_message_t tx_msg = {
    .can_id = 0x400,
    .dlc = 8,
    .message = tx_buffer,
};

uint8_t can_data[8] = {0};
asdk_can_message_t msg = {.message = can_data};

bool msg_received = false;
bool tx_status_busy = 0;
asdk_errorcode_t can_write_status = ASDK_CAN_SUCCESS;

volatile uint16_t tx_can_id = 0;

void __service_callback(uint8_t VEHICLE_CAN, asdk_can_event_t event,
                        asdk_can_message_t *can_message) {
    switch (event) {
    case ASDK_CAN_TX_COMPLETE_EVENT:
        tx_can_id = can_message->can_id;
        break;

    case ASDK_CAN_RX_EVENT:
        msg_received = true;
        rx_msg.can_id = can_message->can_id;
        rx_msg.dlc = can_message->dlc;
        memcpy(rx_msg.message, can_message->message, can_message->dlc);

        if (rx_msg.can_id == 0x300) {
            horn_state = rx_msg.message[0];
            brake_state = rx_msg.message[1];
            indicator_state = rx_msg.message[2];
            throttle = rx_msg.message[3];
            sidestand_engaged = rx_msg.message[4];
            start_button = rx_msg.message[5];
        }
        if (rx_msg.can_id == 0x301) {
            riding_mode_r = rx_msg.message[0];
            vehicle_speed_r = rx_msg.message[1];
            roll = rx_msg.message[2];
            pitch = rx_msg.message[4];
            yaw = rx_msg.message[6];
        }

        break;

    case ASDK_CAN_ERROR_EVENT:
        can_error_count++;
        break;

    case ASDK_CAN_BUS_OFF_EVENT:
        can_bus_off = true;
        can_busoff_count++;
        break;

    default:
        break;
    }
}

void app_can_init() {
    asdk_errorcode_t can_status = ASDK_MW_CAN_SERVICE_SUCCESS;

    asdk_can_config_t can_cfg = {
        .mcu_pins =
            {
                CAN_TX_PIN,
                CAN_RX_PIN,
            },

        .hw_filter = {.rx_fifo_acceptance_filter =
                          {
                              .can_ids = rx_accept_can_ids,
                              .length = can_ids_length,
                          }},

        .controller_settings =
            {
                .mode = ASDK_CAN_MODE_STANDARD,
                .max_dlc = ASDK_CAN_DLC_8,
                .can_id_type = ASDK_CAN_ID_STANDARD,
                .bitrate_config.can =
                    {
                        .baudrate = ASDK_CAN_BAUDRATE_500K,
                        .bit_time =
                            {
                                .prop_segment = 29,
                                .phase_segment1 = 5,
                                .phase_segment2 = 5,
                                .sync_jump_width = 5,
                            },
                    },
                .interrupt_config =
                    {
                        .intr_num = ASDK_EXTI_INTR_CPU_4,
                        .use_interrupt = true,
                        .priority = 3,
                    },
            },
    };

    can_status = asdk_can_service_init(VEHICLE_CAN, can_cfg);
    ASDK_DEV_ERROR_ASSERT(can_status, ASDK_CAN_SUCCESS);

    can_status = asdk_can_service_install_callback(__service_callback);
    ASDK_DEV_ERROR_ASSERT(can_status, ASDK_CAN_SUCCESS);
}

void app_can_deinit() {
    asdk_errorcode_t can_deinit_status = asdk_can_deinit(VEHICLE_CAN);
    ASDK_DEV_ERROR_ASSERT(ASDK_CAN_SUCCESS, can_deinit_status);
}

void app_can_iteration() {
    if (can_bus_off) {
        app_can_deinit();
        app_can_init();
        can_bus_off = false;
    }

    if (msg_received) {
        msg_received = false;

        process_horn_state();
        process_brake_state();
        process_indicator_state();
        process_hold_state();

        if (start_button && !prev_start_button) {
            handle_start_button_press();
        }
        prev_start_button = start_button;
        update_vehicle_speed();
    }

    asdk_can_is_tx_busy(VEHICLE_CAN, 0, &tx_status_busy);

    if (!tx_status_busy) {
        can_write_status = asdk_can_service_send(VEHICLE_CAN, &tx_msg);
        ASDK_DEV_ERROR_ASSERT(ASDK_MW_CAN_SERVICE_SUCCESS, can_write_status);
    }
}

void app_can_send(uint32_t can_id, uint8_t *data, uint8_t data_length) {
    msg.can_id = can_id;
    msg.dlc = data_length;
    memcpy(msg.message, data, data_length);

    can_write_status = asdk_can_service_send(VEHICLE_CAN, &msg);
    ASDK_DEV_ERROR_ASSERT(ASDK_MW_CAN_SERVICE_SUCCESS, can_write_status);
}