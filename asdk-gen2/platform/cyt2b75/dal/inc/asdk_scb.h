/*
  @file
  scb.h

  @path
  /frdmkw36_demo_apps_power_manager/source/hw_i2c.c

  @Created on
  Mar 23, 2023

  @Author
  ajmeri.j

  @Copyright
  Copyright (c) Ather Energy Pvt Ltd.  All rights reserved.

  @brief
    Serial Communication Block common APIs.
*/

#ifndef SCB_H
#define SCB_H

// #include "cyt2b75cae.h"
#include "cyt2b75bae.h"
#include <stdint.h>
#include <stdbool.h>
#include "asdk_platform.h"

typedef void (*scb_user_cb_type)(void);

typedef enum {
  SCB_INDEX_0 = 0,
  SCB_INDEX_1,
  SCB_INDEX_2,
  SCB_INDEX_3,
  SCB_INDEX_4,
  SCB_INDEX_5,
  SCB_INDEX_6,
  SCB_INDEX_7,
  MAX_SCB_MODULES
} scb_index_t;

// this is default configuration, modify as per your board
#define SCB_MAX_UART_MODULES    8
#define SCB_MAX_I2C_MODULES     8
#define SCB_MAX_SPI_MODULES     8




/* SCB APIs */
void SCB_Set_ISR(uint8_t scb_index, scb_user_cb_type user_callback,asdk_exti_interrupt_num_t intr_num, uint8_t interrupt_priority);
void SCB_Clear_ISR(uint8_t scb_index);
bool SCB_Availability_Status(volatile stc_SCB_t *base);


#endif /* SCB_H */
