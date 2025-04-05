/*
    @file
    asdk_can_service.h

    @path
    inc/asdk_can_service.h

    @Created on
    Oct 17, 2023

    @Author
    ajmeri.j

    @Copyright
    Copyright (c) Ather Energy Pvt Ltd.  All rights reserved.

    @brief
    This file prototypes the CAN_SERVICE DAL module of asdk ( Ather SDK )
*/

#ifndef ASDK_CAN_SERVICE_H
#define ASDK_CAN_SERVICE_H

/*==============================================================================

                               INCLUDE FILES

==============================================================================*/

/* standard includes ************************* */

#include <stdint.h>
#include <stdbool.h>

/* asdk includes ***************************** */

#include "asdk_error.h"
#include "ring_buffer.h"

/* dal includes ****************************** */

#include "asdk_can.h"

/* sdk includes ****************************** */

/*==============================================================================

                      DEFINITIONS AND TYPES : MACROS

==============================================================================*/

/*==============================================================================

                      DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

/*==============================================================================

                   DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

typedef struct {
    uint32_t can_id;  /*!< The CAN message identifier (CAN ID). */
    uint8_t dlc;      /*!< Length of the message. */
    uint8_t message[8]; /*!< A pointer to a buffer for holding data. */
} asdk_can_frame_t;

/*==============================================================================

                           EXTERNAL DECLARATIONS

==============================================================================*/

/*==============================================================================

                           FUNCTION PROTOTYPES

==============================================================================*/

asdk_errorcode_t asdk_can_service_init(uint8_t can_ch, asdk_can_config_t can_config);
asdk_errorcode_t asdk_can_service_install_callback(asdk_can_callback_t user_callback);
asdk_errorcode_t asdk_can_service_send(uint8_t can_ch, asdk_can_message_t *msg);
asdk_errorcode_t asdk_can_service_send_iteration(uint8_t can_ch);
asdk_errorcode_t asdk_can_service_receive_iteration(uint8_t can_ch);

#endif /* ASDK_CAN_SERVICE_H */
