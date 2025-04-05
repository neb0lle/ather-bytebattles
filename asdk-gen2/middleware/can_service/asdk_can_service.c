/*
    @file
    asdk_can_service.c

    @path
    platform/cyt2b75/dal/src/asdk_can_service.c

    @Created on
    Oct 17, 2023

    @Author
    ajmeri.j

    @Copyright
    Copyright (c) Ather Energy Pvt Ltd.  All rights reserved.

    @brief
    This file implements the CAN_SERVICE module for Ather SDK (asdk)

*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

/* standard includes ************************* */

#include <string.h>

/* asdk includes ***************************** */

#include "asdk_error.h"

/* middleware includes *********************** */

#include "asdk_can_service.h"

/* lib includes ****************************** */
#include "ring_buffer.h"

/* dal includes ****************************** */

#include "asdk_can.h"
#include "asdk_platform.h"

/* sdk includes ****************************** */

/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : MACROS

==============================================================================*/

#define MAX_POOL_SIZE 40
#define MAX_STD_CANID 0x7FF

/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

/*==============================================================================

                    LOCAL DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/


/*==============================================================================

                            LOCAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

                        LOCAL AND EXTERNAL DEFINITIONS

==============================================================================*/

/* volatile global variables ***************** */

/* global variables ************************** */

/* static variables ************************** */
static asdk_can_frame_t can_message_tx[ASDK_CAN_MODULE_CAN_CH_MAX][MAX_POOL_SIZE] = {0};
static asdk_can_frame_t can_message_rx[ASDK_CAN_MODULE_CAN_CH_MAX][MAX_POOL_SIZE] = {0};

static ring_buffer_t can_tx_buffer[ASDK_CAN_MODULE_CAN_CH_MAX] = {0};
static ring_buffer_t can_rx_buffer[ASDK_CAN_MODULE_CAN_CH_MAX] = {0};

static asdk_can_callback_t service_user_callback = NULL;

/*==============================================================================

                            LOCAL FUNCTION DEFINITIONS

==============================================================================*/

/* static functions ************************** */
void __asdk_can_service_callback_handler(uint8_t can_ch, asdk_can_event_t event, asdk_can_message_t *message)
{
    asdk_can_frame_t can_data;
    switch (event)
    {
    /* service rx event */
    case ASDK_CAN_RX_EVENT:
        ASDK_ENTER_CRITICAL_SECTION()
        can_data.can_id = message->can_id;
        can_data.dlc = message->dlc;
        memcpy(can_data.message, message->message, message->dlc);
        ASDK_EXIT_CRITICAL_SECTION()

        ring_buffer_write(&(can_rx_buffer[can_ch]), (uint8_t *)&can_data, 1);
        break;

    // propogate transmit complete event
    case ASDK_CAN_TX_COMPLETE_EVENT:
        if (NULL != service_user_callback)
        {
            service_user_callback(can_ch, event, message);
        }
        break;

    // propogate error events to application
    case ASDK_CAN_TX_ERROR_EVENT:
    case ASDK_CAN_RX_ERROR_EVENT:
    case ASDK_CAN_ERROR_EVENT:
    case ASDK_CAN_BUS_OFF_EVENT:
    case ASDK_CAN_SLEEP_EVENT:
    case ASDK_CAN_WAKE_UP_EVENT:
        if (NULL != service_user_callback)
        {
            service_user_callback(can_ch, event, NULL);
        }
        break;

    default:
        break;
    }
}

asdk_errorcode_t asdk_can_service_init(uint8_t can_ch, asdk_can_config_t can_config)
{
    asdk_errorcode_t service_init_status = ASDK_MW_CAN_SERVICE_SUCCESS;

    /* initialize CAN driver */

    service_init_status = asdk_can_init(can_ch, &can_config);

    if (service_init_status != ASDK_CAN_SUCCESS)
    {
        return service_init_status;
    }

    /* initialize circular buffers for a given channel */

    can_tx_buffer[can_ch].buffer = (uint8_t *)&(can_message_tx[can_ch]);
    can_tx_buffer[can_ch].total_capacity = sizeof(can_message_tx[can_ch]);
    can_tx_buffer[can_ch].block_size = sizeof(asdk_can_frame_t);
    can_tx_buffer[can_ch].enable_overwrite = false;
    can_tx_buffer[can_ch].event_callback = (ring_buffer_event_callback_t)NULL;

    ring_buffer_init(&(can_tx_buffer[can_ch]));

    can_rx_buffer[can_ch].buffer = (uint8_t *)&(can_message_rx[can_ch]);
    can_rx_buffer[can_ch].total_capacity = sizeof(can_message_rx[can_ch]);
    can_rx_buffer[can_ch].block_size = sizeof(asdk_can_frame_t);
    can_rx_buffer[can_ch].enable_overwrite = false;
    can_rx_buffer[can_ch].event_callback = (ring_buffer_event_callback_t)NULL;

    ring_buffer_init(&(can_rx_buffer[can_ch]));

    return service_init_status;
}

asdk_errorcode_t asdk_can_service_install_callback(asdk_can_callback_t user_callback)
{
    asdk_errorcode_t assign_cb_status = ASDK_MW_CAN_SERVICE_SUCCESS;

    /* assign CAN service user callback handler */

    if (NULL != user_callback)
    {
        service_user_callback = user_callback;
    }
    else
    {
        return ASDK_MW_CAN_SERVICE_ERROR_NULL_PTR;
    }

    /* assign service callback handler */

    // the callback is made from interrupt context if interrupt is enabled
    // otherwise it is made from iteration context if interrupt is disabled
    assign_cb_status = asdk_can_install_callback(&__asdk_can_service_callback_handler);

    if (assign_cb_status != ASDK_CAN_SUCCESS)
    {
        return assign_cb_status;
    }

    return assign_cb_status;
}

asdk_errorcode_t asdk_can_service_send(uint8_t can_ch, asdk_can_message_t *msg)
{
    asdk_errorcode_t service_send_status = ASDK_MW_CAN_SERVICE_SUCCESS;
    uint32_t num_blocks;
    asdk_can_frame_t can_data;

    if (ASDK_CAN_MODULE_CAN_CH_MAX <= can_ch)
    {
        return ASDK_CAN_ERROR_INVALID_CHANNEL;
    }

    /* push packet to transmit  queue */
    ASDK_ENTER_CRITICAL_SECTION()
    can_data.can_id = msg->can_id;
    can_data.dlc = msg->dlc;
    memcpy(can_data.message, msg->message, msg->dlc);
    ASDK_EXIT_CRITICAL_SECTION()

    num_blocks = ring_buffer_write(&(can_tx_buffer[can_ch]), (uint8_t *)&can_data, 1);

    if (num_blocks == 0) {
        return ASDK_MW_CAN_SERVICE_TX_QUEUE_FULL;
    }

    return service_send_status;
}

asdk_errorcode_t asdk_can_service_send_iteration(uint8_t can_ch)
{
    asdk_errorcode_t service_send_iteration_status = ASDK_MW_CAN_SERVICE_SUCCESS;
    bool tx_status_busy = false;
    asdk_can_frame_t can_data;
    asdk_can_message_t tx_msg = {0};
    uint32_t num_blocks = 0;

    service_send_iteration_status = asdk_can_is_tx_busy(can_ch, 0, &tx_status_busy);

    if (ASDK_CAN_SUCCESS != service_send_iteration_status)
    {
        return service_send_iteration_status;
    }
    else if (tx_status_busy)
    {
        return ASDK_MW_CAN_SERVICE_TX_BUSY;
    }
    else
    {
        /* get data from transmit queue and send it on can bus */
        num_blocks = ring_buffer_read(&(can_tx_buffer[can_ch]), (uint8_t *)&can_data, 1);

        if (num_blocks == 0) {
            return ASDK_MW_CAN_SERVICE_TX_QUEUE_EMPTY;
        }

        tx_msg.can_id = can_data.can_id;
        tx_msg.dlc = can_data.dlc;
        tx_msg.message = can_data.message;

        service_send_iteration_status = asdk_can_write(can_ch, 0, &tx_msg);
    }

    return service_send_iteration_status;
}

asdk_errorcode_t asdk_can_service_receive_iteration(uint8_t can_ch)
{
    asdk_errorcode_t service_receive_iteration_status = ASDK_MW_CAN_SERVICE_SUCCESS;
    asdk_can_frame_t rx_frame;
    uint32_t num_blocks = 0;
    asdk_can_message_t rx_msg = {0};

    if (ASDK_CAN_MODULE_CAN_CH_MAX <= can_ch)
    {
        return ASDK_CAN_ERROR_INVALID_CHANNEL;
    }

    /* get data from receive pending queue */
    num_blocks = ring_buffer_read(&(can_rx_buffer[can_ch]), (uint8_t *)&rx_frame, 1);

    if (num_blocks == 0) {
        return ASDK_MW_CAN_SERVICE_RX_QUEUE_EMPTY;
    }

    /* callback to user with received message */
    if (NULL != service_user_callback)
    {
        rx_msg.can_id = rx_frame.can_id;
        rx_msg.dlc = rx_frame.dlc;
        rx_msg.message = rx_frame.message;

        service_user_callback(can_ch, ASDK_CAN_RX_EVENT, &rx_msg);
    }

    return service_receive_iteration_status;
}
