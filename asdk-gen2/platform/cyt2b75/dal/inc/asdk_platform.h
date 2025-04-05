#ifndef ASDK_PLATFORM_H
#define ASDK_PLATFORM_H

/* MCU SDK includes required for application */

#include "cy_device_headers.h"
#include "asdk_mcu_pins.h"
#include "asdk_system.h"

#define ASDK_CYT2B75_PIN_MAX MCU_PIN_MAX /*!< This microcontroller has 100 pins. Refer @ref MCU_PIN_MAX */

#define ASDK_ENTER_CRITICAL_SECTION() asdk_sys_disable_interrupts();

#define ASDK_EXIT_CRITICAL_SECTION()  asdk_sys_enable_interrupts();

/*!
 * @brief An enumerator to represent CAN channels.
 *
 * The CAN peripheral is abstracted based on channel number regardless of the
 * number of available CAN modules or their underlying channels. Refer the TRM
 * of the microcontroller for more information.
 * 
 * @note
 * * The TRM lists combination of CAN peripheral clock with total time-quanta.
 * From the table, we picked clock of 40MHz because it covers the entire range of
 * baudrate within the specified time-quanta limits.
 * * As consequence of setting clock to 40MHz the pre-scaler can be adjusted
 * to maintain a constant time quanta. Hence the DAL of CYT2B75 platform
 * adjusts the pre-scaler to maintain a constant time quanta of 40 for
 * all baudrates.
 * 
 */
typedef enum
{
    ASDK_CAN_MODULE_CAN_CH_0 = 0, /*!< Use CANFD0 Channel-0 as CAN Channel #0 */
    ASDK_CAN_MODULE_CAN_CH_1,     /*!< Use CANFD0 Channel-1 as CAN Channel #1 */
    ASDK_CAN_MODULE_CAN_CH_2,     /*!< Use CANFD0 Channel-2 as CAN Channel #2 */
    ASDK_CAN_MODULE_CAN_CH_3,     /*!< Use CANFD1 Channel-0 as CAN Channel #3 */
    ASDK_CAN_MODULE_CAN_CH_4,     /*!< Use CANFD1 Channel-1 as CAN Channel #4 */
    ASDK_CAN_MODULE_CAN_CH_5,     /*!< Use CANFD1 Channel-2 as CAN Channel #5 */
    ASDK_CAN_MODULE_CAN_CH_MAX,   /*!< Total number of CAN channels: 6*/
    ASDK_CAN_MODULE_NOT_DEFINED = ASDK_CAN_MODULE_CAN_CH_MAX, /*!< Channels beyond @ref ASDK_CAN_MODULE_CAN_CH_MAX is undefined. */
} asdk_can_channel_t;

/*!
 * @brief An enumerator to represent Timer channels. The Timer peripheral is 
 * abstracted based on channel number regardless of the number of available 
 * Timer modules or their underlying channels. Refer the TRM of the 
 * microcontroller for more information.
 * 
 * @note
 * Following groups of timers are available in this microcontrollers:
 * * Group 0 has 63 timers of 16-bit type.
 * * Group 1 has 12 timers of 16-bit type.
 * * Group 2 has 4 timers of 32-bit type.
 */
typedef enum
{
    // Group 0, 63 timers
    ASDK_TIMER_MODULE_CH_0 = 0, /*!< 16-bit Timer, Use Group0 Channel-0 as Timer Channel #0 */
    ASDK_TIMER_MODULE_CH_1,     /*!< 16-bit Timer, Use Group0 Channel-1 as Timer Channel #1 */
    ASDK_TIMER_MODULE_CH_2,     /*!< 16-bit Timer, Use Group0 Channel-2 as Timer Channel #2 */
    ASDK_TIMER_MODULE_CH_3,     /*!< 16-bit Timer, Use Group0 Channel-3 as Timer Channel #3 */
    ASDK_TIMER_MODULE_CH_4,     /*!< 16-bit Timer, Use Group0 Channel-4 as Timer Channel #4 */
    ASDK_TIMER_MODULE_CH_5,     /*!< 16-bit Timer, Use Group0 Channel-5 as Timer Channel #5 */
    ASDK_TIMER_MODULE_CH_6,     /*!< 16-bit Timer, Use Group0 Channel-6 as Timer Channel #6 */
    ASDK_TIMER_MODULE_CH_7,     /*!< 16-bit Timer, Use Group0 Channel-7 as Timer Channel #7 */
    ASDK_TIMER_MODULE_CH_8,     /*!< 16-bit Timer, Use Group0 Channel-8 as Timer Channel #8 */
    ASDK_TIMER_MODULE_CH_9,     /*!< 16-bit Timer, Use Group0 Channel-9 as Timer Channel #9 */
    ASDK_TIMER_MODULE_CH_10,    /*!< 16-bit Timer, Use Group0 Channel-10 as Timer Channel #10 */
    ASDK_TIMER_MODULE_CH_11,    /*!< 16-bit Timer, Use Group0 Channel-11 as Timer Channel #11 */
    ASDK_TIMER_MODULE_CH_12,    /*!< 16-bit Timer, Use Group0 Channel-12 as Timer Channel #12 */
    ASDK_TIMER_MODULE_CH_13,    /*!< 16-bit Timer, Use Group0 Channel-13 as Timer Channel #13 */
    ASDK_TIMER_MODULE_CH_14,    /*!< 16-bit Timer, Use Group0 Channel-14 as Timer Channel #14 */
    ASDK_TIMER_MODULE_CH_15,    /*!< 16-bit Timer, Use Group0 Channel-15 as Timer Channel #15 */
    ASDK_TIMER_MODULE_CH_16,    /*!< 16-bit Timer, Use Group0 Channel-16 as Timer Channel #16 */
    ASDK_TIMER_MODULE_CH_17,    /*!< 16-bit Timer, Use Group0 Channel-17 as Timer Channel #17 */
    ASDK_TIMER_MODULE_CH_18,    /*!< 16-bit Timer, Use Group0 Channel-18 as Timer Channel #18 */
    ASDK_TIMER_MODULE_CH_19,    /*!< 16-bit Timer, Use Group0 Channel-19 as Timer Channel #19 */
    ASDK_TIMER_MODULE_CH_20,    /*!< 16-bit Timer, Use Group0 Channel-20 as Timer Channel #20 */
    ASDK_TIMER_MODULE_CH_21,    /*!< 16-bit Timer, Use Group0 Channel-21 as Timer Channel #21 */
    ASDK_TIMER_MODULE_CH_22,    /*!< 16-bit Timer, Use Group0 Channel-22 as Timer Channel #22 */
    ASDK_TIMER_MODULE_CH_23,    /*!< 16-bit Timer, Use Group0 Channel-23 as Timer Channel #23 */
    ASDK_TIMER_MODULE_CH_24,    /*!< 16-bit Timer, Use Group0 Channel-24 as Timer Channel #24 */
    ASDK_TIMER_MODULE_CH_25,    /*!< 16-bit Timer, Use Group0 Channel-25 as Timer Channel #25 */
    ASDK_TIMER_MODULE_CH_26,    /*!< 16-bit Timer, Use Group0 Channel-26 as Timer Channel #26 */
    ASDK_TIMER_MODULE_CH_27,    /*!< 16-bit Timer, Use Group0 Channel-27 as Timer Channel #27 */
    ASDK_TIMER_MODULE_CH_28,    /*!< 16-bit Timer, Use Group0 Channel-28 as Timer Channel #28 */
    ASDK_TIMER_MODULE_CH_29,    /*!< 16-bit Timer, Use Group0 Channel-29 as Timer Channel #29 */
    ASDK_TIMER_MODULE_CH_30,    /*!< 16-bit Timer, Use Group0 Channel-30 as Timer Channel #30 */
    ASDK_TIMER_MODULE_CH_31,    /*!< 16-bit Timer, Use Group0 Channel-31 as Timer Channel #31 */
    ASDK_TIMER_MODULE_CH_32,    /*!< 16-bit Timer, Use Group0 Channel-32 as Timer Channel #32 */
    ASDK_TIMER_MODULE_CH_33,    /*!< 16-bit Timer, Use Group0 Channel-33 as Timer Channel #33 */
    ASDK_TIMER_MODULE_CH_34,    /*!< 16-bit Timer, Use Group0 Channel-34 as Timer Channel #34 */
    ASDK_TIMER_MODULE_CH_35,    /*!< 16-bit Timer, Use Group0 Channel-35 as Timer Channel #35 */
    ASDK_TIMER_MODULE_CH_36,    /*!< 16-bit Timer, Use Group0 Channel-36 as Timer Channel #36 */
    ASDK_TIMER_MODULE_CH_37,    /*!< 16-bit Timer, Use Group0 Channel-37 as Timer Channel #37 */
    ASDK_TIMER_MODULE_CH_38,    /*!< 16-bit Timer, Use Group0 Channel-38 as Timer Channel #38 */
    ASDK_TIMER_MODULE_CH_39,    /*!< 16-bit Timer, Use Group0 Channel-39 as Timer Channel #39 */
    ASDK_TIMER_MODULE_CH_40,    /*!< 16-bit Timer, Use Group0 Channel-40 as Timer Channel #40 */
    ASDK_TIMER_MODULE_CH_41,    /*!< 16-bit Timer, Use Group0 Channel-41 as Timer Channel #41 */
    ASDK_TIMER_MODULE_CH_42,    /*!< 16-bit Timer, Use Group0 Channel-42 as Timer Channel #42 */
    ASDK_TIMER_MODULE_CH_43,    /*!< 16-bit Timer, Use Group0 Channel-43 as Timer Channel #43 */
    ASDK_TIMER_MODULE_CH_44,    /*!< 16-bit Timer, Use Group0 Channel-44 as Timer Channel #44 */
    ASDK_TIMER_MODULE_CH_45,    /*!< 16-bit Timer, Use Group0 Channel-45 as Timer Channel #45 */
    ASDK_TIMER_MODULE_CH_46,    /*!< 16-bit Timer, Use Group0 Channel-46 as Timer Channel #46 */
    ASDK_TIMER_MODULE_CH_47,    /*!< 16-bit Timer, Use Group0 Channel-47 as Timer Channel #47 */
    ASDK_TIMER_MODULE_CH_48,    /*!< 16-bit Timer, Use Group0 Channel-48 as Timer Channel #48 */
    ASDK_TIMER_MODULE_CH_49,    /*!< 16-bit Timer, Use Group0 Channel-49 as Timer Channel #49 */
    ASDK_TIMER_MODULE_CH_50,    /*!< 16-bit Timer, Use Group0 Channel-50 as Timer Channel #50 */
    ASDK_TIMER_MODULE_CH_51,    /*!< 16-bit Timer, Use Group0 Channel-51 as Timer Channel #51 */
    ASDK_TIMER_MODULE_CH_52,    /*!< 16-bit Timer, Use Group0 Channel-52 as Timer Channel #52 */
    ASDK_TIMER_MODULE_CH_53,    /*!< 16-bit Timer, Use Group0 Channel-53 as Timer Channel #53 */
    ASDK_TIMER_MODULE_CH_54,    /*!< 16-bit Timer, Use Group0 Channel-54 as Timer Channel #54 */
    ASDK_TIMER_MODULE_CH_55,    /*!< 16-bit Timer, Use Group0 Channel-55 as Timer Channel #55 */
    ASDK_TIMER_MODULE_CH_56,    /*!< 16-bit Timer, Use Group0 Channel-56 as Timer Channel #56 */
    ASDK_TIMER_MODULE_CH_57,    /*!< 16-bit Timer, Use Group0 Channel-57 as Timer Channel #57 */
    ASDK_TIMER_MODULE_CH_58,    /*!< 16-bit Timer, Use Group0 Channel-58 as Timer Channel #58 */
    ASDK_TIMER_MODULE_CH_59,    /*!< 16-bit Timer, Use Group0 Channel-59 as Timer Channel #59 */
    ASDK_TIMER_MODULE_CH_60,    /*!< 16-bit Timer, Use Group0 Channel-60 as Timer Channel #60 */
    ASDK_TIMER_MODULE_CH_61,    /*!< 16-bit Timer, Use Group0 Channel-61 as Timer Channel #61 */
    ASDK_TIMER_MODULE_CH_62,    /*!< 16-bit Timer, Use Group0 Channel-62 as Timer Channel #62 */

    // Group 1, 12 timers
    ASDK_TIMER_MODULE_CH_63, /*!< 16-bit Timer, Use Group1 Channel-0 as Timer Channel #63 */
    ASDK_TIMER_MODULE_CH_64, /*!< 16-bit Timer, Use Group1 Channel-1 as Timer Channel #64 */
    ASDK_TIMER_MODULE_CH_65, /*!< 16-bit Timer, Use Group1 Channel-2 as Timer Channel #65 */
    ASDK_TIMER_MODULE_CH_66, /*!< 16-bit Timer, Use Group1 Channel-3 as Timer Channel #66 */
    ASDK_TIMER_MODULE_CH_67, /*!< 16-bit Timer, Use Group1 Channel-4 as Timer Channel #67 */
    ASDK_TIMER_MODULE_CH_68, /*!< 16-bit Timer, Use Group1 Channel-5 as Timer Channel #68 */
    ASDK_TIMER_MODULE_CH_69, /*!< 16-bit Timer, Use Group1 Channel-6 as Timer Channel #69 */
    ASDK_TIMER_MODULE_CH_70, /*!< 16-bit Timer, Use Group1 Channel-7 as Timer Channel #70 */
    ASDK_TIMER_MODULE_CH_71, /*!< 16-bit Timer, Use Group1 Channel-8 as Timer Channel #71 */
    ASDK_TIMER_MODULE_CH_72, /*!< 16-bit Timer, Use Group1 Channel-9 as Timer Channel #72 */
    ASDK_TIMER_MODULE_CH_73, /*!< 16-bit Timer, Use Group1 Channel-10 as Timer Channel #73 */
    ASDK_TIMER_MODULE_CH_74, /*!< 16-bit Timer, Use Group1 Channel-11 as Timer Channel #74 */

    // Group 2, 4 timers
    ASDK_TIMER_MODULE_CH_75, /*!< 32-bit Timer, Use Group2 Channel-0 as Timer Channel #75 */
    ASDK_TIMER_MODULE_CH_76, /*!< 32-bit Timer, Use Group2 Channel-1 as Timer Channel #76 */
    ASDK_TIMER_MODULE_CH_77, /*!< 32-bit Timer, Use Group2 Channel-2 as Timer Channel #77 */
    ASDK_TIMER_MODULE_CH_78, /*!< 32-bit Timer, Use Group2 Channel-3 as Timer Channel #78 */

    ASDK_TIMER_MODULE_CH_MAX /*!< Total number of timer channels. */
} asdk_timer_channel_t;

/*!
 * @brief An enumerator to represent PWM channels.
 *
 * The PWM peripheral is abstracted based on channel number regardless of the
 * number of available PWM modules or their underlying channels. Refer the TRM
 * of the microcontroller for more information.
 * 
 * @note
 *  Following groups of timers are available as PWM in this microcontrollers:
 * * Group 0 has 63 timers of 16-bit type.
 * * Group 1 has 12 timers of 16-bit type. Dedicated for Motor Control.
 * * Group 2 has 4 timers of 32-bit type.
 */
typedef enum
{
    // Group 0, 63 PWM Channels
    ASDK_PWM_MODULE_CH_0 = 0, /*!< 16-bit Timer, Use Group0 Channel-0 as PWM Channel #0 */
    ASDK_PWM_MODULE_CH_1,     /*!< 16-bit Timer, Use Group0 Channel-1 as PWM Channel #1 */
    ASDK_PWM_MODULE_CH_2,     /*!< 16-bit Timer, Use Group0 Channel-2 as PWM Channel #2 */
    ASDK_PWM_MODULE_CH_3,     /*!< 16-bit Timer, Use Group0 Channel-3 as PWM Channel #3 */
    ASDK_PWM_MODULE_CH_4,     /*!< 16-bit Timer, Use Group0 Channel-4 as PWM Channel #4 */
    ASDK_PWM_MODULE_CH_5,     /*!< 16-bit Timer, Use Group0 Channel-5 as PWM Channel #5 */
    ASDK_PWM_MODULE_CH_6,     /*!< 16-bit Timer, Use Group0 Channel-6 as PWM Channel #6 */
    ASDK_PWM_MODULE_CH_7,     /*!< 16-bit Timer, Use Group0 Channel-7 as PWM Channel #7 */
    ASDK_PWM_MODULE_CH_8,     /*!< 16-bit Timer, Use Group0 Channel-8 as PWM Channel #8 */
    ASDK_PWM_MODULE_CH_9,     /*!< 16-bit Timer, Use Group0 Channel-9 as PWM Channel #9 */
    ASDK_PWM_MODULE_CH_10,    /*!< 16-bit Timer, Use Group0 Channel-10 as PWM Channel #10 */
    ASDK_PWM_MODULE_CH_11,    /*!< 16-bit Timer, Use Group0 Channel-11 as PWM Channel #11 */
    ASDK_PWM_MODULE_CH_12,    /*!< 16-bit Timer, Use Group0 Channel-12 as PWM Channel #12 */
    ASDK_PWM_MODULE_CH_13,    /*!< 16-bit Timer, Use Group0 Channel-13 as PWM Channel #13 */
    ASDK_PWM_MODULE_CH_14,    /*!< 16-bit Timer, Use Group0 Channel-14 as PWM Channel #14 */
    ASDK_PWM_MODULE_CH_15,    /*!< 16-bit Timer, Use Group0 Channel-15 as PWM Channel #15 */
    ASDK_PWM_MODULE_CH_16,    /*!< 16-bit Timer, Use Group0 Channel-16 as PWM Channel #16 */
    ASDK_PWM_MODULE_CH_17,    /*!< 16-bit Timer, Use Group0 Channel-17 as PWM Channel #17 */
    ASDK_PWM_MODULE_CH_18,    /*!< 16-bit Timer, Use Group0 Channel-18 as PWM Channel #18 */
    ASDK_PWM_MODULE_CH_19,    /*!< 16-bit Timer, Use Group0 Channel-19 as PWM Channel #19 */
    ASDK_PWM_MODULE_CH_20,    /*!< 16-bit Timer, Use Group0 Channel-20 as PWM Channel #20 */
    ASDK_PWM_MODULE_CH_21,    /*!< 16-bit Timer, Use Group0 Channel-21 as PWM Channel #21 */
    ASDK_PWM_MODULE_CH_22,    /*!< 16-bit Timer, Use Group0 Channel-22 as PWM Channel #22 */
    ASDK_PWM_MODULE_CH_23,    /*!< 16-bit Timer, Use Group0 Channel-23 as PWM Channel #23 */
    ASDK_PWM_MODULE_CH_24,    /*!< 16-bit Timer, Use Group0 Channel-24 as PWM Channel #24 */
    ASDK_PWM_MODULE_CH_25,    /*!< 16-bit Timer, Use Group0 Channel-25 as PWM Channel #25 */
    ASDK_PWM_MODULE_CH_26,    /*!< 16-bit Timer, Use Group0 Channel-26 as PWM Channel #26 */
    ASDK_PWM_MODULE_CH_27,    /*!< 16-bit Timer, Use Group0 Channel-27 as PWM Channel #27 */
    ASDK_PWM_MODULE_CH_28,    /*!< 16-bit Timer, Use Group0 Channel-28 as PWM Channel #28 */
    ASDK_PWM_MODULE_CH_29,    /*!< 16-bit Timer, Use Group0 Channel-29 as PWM Channel #29 */
    ASDK_PWM_MODULE_CH_30,    /*!< 16-bit Timer, Use Group0 Channel-30 as PWM Channel #30 */
    ASDK_PWM_MODULE_CH_31,    /*!< 16-bit Timer, Use Group0 Channel-31 as PWM Channel #31 */
    ASDK_PWM_MODULE_CH_32,    /*!< 16-bit Timer, Use Group0 Channel-32 as PWM Channel #32 */
    ASDK_PWM_MODULE_CH_33,    /*!< 16-bit Timer, Use Group0 Channel-33 as PWM Channel #33 */
    ASDK_PWM_MODULE_CH_34,    /*!< 16-bit Timer, Use Group0 Channel-34 as PWM Channel #34 */
    ASDK_PWM_MODULE_CH_35,    /*!< 16-bit Timer, Use Group0 Channel-35 as PWM Channel #35 */
    ASDK_PWM_MODULE_CH_36,    /*!< 16-bit Timer, Use Group0 Channel-36 as PWM Channel #36 */
    ASDK_PWM_MODULE_CH_37,    /*!< 16-bit Timer, Use Group0 Channel-37 as PWM Channel #37 */
    ASDK_PWM_MODULE_CH_38,    /*!< 16-bit Timer, Use Group0 Channel-38 as PWM Channel #38 */
    ASDK_PWM_MODULE_CH_39,    /*!< 16-bit Timer, Use Group0 Channel-39 as PWM Channel #39 */
    ASDK_PWM_MODULE_CH_40,    /*!< 16-bit Timer, Use Group0 Channel-40 as PWM Channel #40 */
    ASDK_PWM_MODULE_CH_41,    /*!< 16-bit Timer, Use Group0 Channel-41 as PWM Channel #41 */
    ASDK_PWM_MODULE_CH_42,    /*!< 16-bit Timer, Use Group0 Channel-42 as PWM Channel #42 */
    ASDK_PWM_MODULE_CH_43,    /*!< 16-bit Timer, Use Group0 Channel-43 as PWM Channel #43 */
    ASDK_PWM_MODULE_CH_44,    /*!< 16-bit Timer, Use Group0 Channel-44 as PWM Channel #44 */
    ASDK_PWM_MODULE_CH_45,    /*!< 16-bit Timer, Use Group0 Channel-45 as PWM Channel #45 */
    ASDK_PWM_MODULE_CH_46,    /*!< 16-bit Timer, Use Group0 Channel-46 as PWM Channel #46 */
    ASDK_PWM_MODULE_CH_47,    /*!< 16-bit Timer, Use Group0 Channel-47 as PWM Channel #47 */
    ASDK_PWM_MODULE_CH_48,    /*!< 16-bit Timer, Use Group0 Channel-48 as PWM Channel #48 */
    ASDK_PWM_MODULE_CH_49,    /*!< 16-bit Timer, Use Group0 Channel-49 as PWM Channel #49 */
    ASDK_PWM_MODULE_CH_50,    /*!< 16-bit Timer, Use Group0 Channel-50 as PWM Channel #50 */
    ASDK_PWM_MODULE_CH_51,    /*!< 16-bit Timer, Use Group0 Channel-51 as PWM Channel #51 */
    ASDK_PWM_MODULE_CH_52,    /*!< 16-bit Timer, Use Group0 Channel-52 as PWM Channel #52 */
    ASDK_PWM_MODULE_CH_53,    /*!< 16-bit Timer, Use Group0 Channel-53 as PWM Channel #53 */
    ASDK_PWM_MODULE_CH_54,    /*!< 16-bit Timer, Use Group0 Channel-54 as PWM Channel #54 */
    ASDK_PWM_MODULE_CH_55,    /*!< 16-bit Timer, Use Group0 Channel-55 as PWM Channel #55 */
    ASDK_PWM_MODULE_CH_56,    /*!< 16-bit Timer, Use Group0 Channel-56 as PWM Channel #56 */
    ASDK_PWM_MODULE_CH_57,    /*!< 16-bit Timer, Use Group0 Channel-57 as PWM Channel #57 */
    ASDK_PWM_MODULE_CH_58,    /*!< 16-bit Timer, Use Group0 Channel-58 as PWM Channel #58 */
    ASDK_PWM_MODULE_CH_59,    /*!< 16-bit Timer, Use Group0 Channel-59 as PWM Channel #59 */
    ASDK_PWM_MODULE_CH_60,    /*!< 16-bit Timer, Use Group0 Channel-60 as PWM Channel #60 */
    ASDK_PWM_MODULE_CH_61,    /*!< 16-bit Timer, Use Group0 Channel-61 as PWM Channel #61 */
    ASDK_PWM_MODULE_CH_62,    /*!< 16-bit Timer, Use Group0 Channel-62 as PWM Channel #62 */

    // Group 1, 12 timers
    ASDK_PWM_MODULE_CH_63, /*!< 16-bit Timer, Use Group1 Channel-0 as PWM Channel #63 */
    ASDK_PWM_MODULE_CH_64, /*!< 16-bit Timer, Use Group1 Channel-1 as PWM Channel #64 */
    ASDK_PWM_MODULE_CH_65, /*!< 16-bit Timer, Use Group1 Channel-2 as PWM Channel #65 */
    ASDK_PWM_MODULE_CH_66, /*!< 16-bit Timer, Use Group1 Channel-3 as PWM Channel #66 */
    ASDK_PWM_MODULE_CH_67, /*!< 16-bit Timer, Use Group1 Channel-4 as PWM Channel #67 */
    ASDK_PWM_MODULE_CH_68, /*!< 16-bit Timer, Use Group1 Channel-5 as PWM Channel #68 */
    ASDK_PWM_MODULE_CH_69, /*!< 16-bit Timer, Use Group1 Channel-6 as PWM Channel #69 */
    ASDK_PWM_MODULE_CH_70, /*!< 16-bit Timer, Use Group1 Channel-7 as PWM Channel #70 */
    ASDK_PWM_MODULE_CH_71, /*!< 16-bit Timer, Use Group1 Channel-8 as PWM Channel #71 */
    ASDK_PWM_MODULE_CH_72, /*!< 16-bit Timer, Use Group1 Channel-9 as PWM Channel #72 */
    ASDK_PWM_MODULE_CH_73, /*!< 16-bit Timer, Use Group1 Channel-10 as PWM Channel #73 */
    ASDK_PWM_MODULE_CH_74, /*!< 16-bit Timer, Use Group1 Channel-11 as PWM Channel #74 */

    // Group 2, 4 timers
    ASDK_PWM_MODULE_CH_75, /*!< 32-bit Timer, Use Group2 Channel-0 as PWM Channel #75 */
    ASDK_PWM_MODULE_CH_76, /*!< 32-bit Timer, Use Group2 Channel-1 as PWM Channel #76 */
    ASDK_PWM_MODULE_CH_77, /*!< 32-bit Timer, Use Group2 Channel-2 as PWM Channel #77 */
    ASDK_PWM_MODULE_CH_78, /*!< 32-bit Timer, Use Group2 Channel-3 as PWM Channel #78 */

    ASDK_PWM_MODULE_CH_MAX /*!< Total number of PWM channels. */
}asdk_pwm_channel_t;

/*!
 * @brief An enumerator to represent number of I2C modules.
 *
 * @note: Maximum of 8 I2C modules are supported.
 */
typedef enum
{
    ASDK_I2C_0 = 0, /*!< Use SCB0 as I2C #0 */
    ASDK_I2C_1,     /*!< Use SCB1 as I2C #1 */
    ASDK_I2C_2,     /*!< Use SCB2 as I2C #2 */
    ASDK_I2C_3,     /*!< Use SCB3 as I2C #3 */
    ASDK_I2C_4,     /*!< Use SCB4 as I2C #4 */
    ASDK_I2C_5,     /*!< Use SCB5 as I2C #5 */
    ASDK_I2C_6,     /*!< Use SCB6 as I2C #6 */
    ASDK_I2C_7,     /*!< Use SCB7 as I2C #7 */
    ASDK_I2C_MAX,   /*!< Max number of I2C channels */
    ASDK_I2C_UNDEFINED = ASDK_I2C_MAX, /*!< Values beyond @ref ASDK_I2C_MAX is undefined. */
}asdk_i2c_num_t;

/*!
 * @brief An enumerator to represent number of SPI modules.
 *
 */
typedef enum
{
    ASDK_SPI_0 = 0, /*!< Use SCB0 as SPI #0 */
    ASDK_SPI_1,     /*!< Use SCB1 as SPI #1 */
    ASDK_SPI_2,     /*!< Use SCB2 as SPI #2 */
    ASDK_SPI_3,     /*!< Use SCB3 as SPI #3 */
    ASDK_SPI_4,     /*!< Use SCB4 as SPI #4 */
    ASDK_SPI_5,     /*!< Use SCB5 as SPI #5 */
    ASDK_SPI_6,     /*!< Use SCB6 as SPI #6 */
    ASDK_SPI_7,     /*!< Use SCB6 as SPI #7 */
    ASDK_SPI_MAX,   /*!< Max number of SPI channels */
    ASDK_SPI_UNDEFINED = ASDK_SPI_MAX,  /*!< Values beyond @ref ASDK_SPI_MAX is undefined. */
} asdk_spi_num_t;

/*!
 * @brief An enumerator to represent number of UART modules.
 *
 * @note: Maximum of 8 UART modules are supported.
 */
typedef enum
{
    ASDK_UART_0 = 0,    /*!< Use SCB0 as UART #0 */
    ASDK_UART_1,        /*!< Use SCB1 as UART #1 */
    ASDK_UART_2,        /*!< Use SCB2 as UART #2 */
    ASDK_UART_3,        /*!< Use SCB3 as UART #3 */
    ASDK_UART_4,        /*!< Use SCB4 as UART #4 */
    ASDK_UART_5,        /*!< Use SCB5 as UART #5 */
    ASDK_UART_6,        /*!< Use SCB6 as UART #6 */
    ASDK_UART_7,        /*!< Use SCB7 as UART #7 */
    ASDK_UART_MAX,      /*!< Max number of UART channels */
    ASDK_UART_UNDEFINED = ASDK_UART_MAX, /*!< Values beyond @ref ASDK_UART_MAX is undefined. */
}asdk_uart_num_t;

/*!
 * @brief An enumerator to represent the External Interrupt in Traveo
 *
 * @note: Maximum of 8 CPU_INTERRUPTS are supported(for M4 core)
 * @note: Maximum of 6 CPU_INTERRUPTS are supported(for m0plus  core)
 * @note: Please avoid using (ASDK_EXTI_INTR_CPU_0 & ASDK_EXTI_INTR_CPU_1) for peripherals as they are reserved for IPC calls
 * 
 */
typedef enum
{
    ASDK_EXTI_INTR_CPU_0 = 0,    /*!< Use CPU_INTR_0 as INTR #0 */
    ASDK_EXTI_INTR_CPU_1,        /*!< Use CPU_INTR_1 as INTR #1 */
    ASDK_EXTI_INTR_CPU_2,        /*!< Use CPU_INTR_2 as INTR #2 */
    ASDK_EXTI_INTR_CPU_3,        /*!< Use CPU_INTR_3 as INTR #3 */
    ASDK_EXTI_INTR_CPU_4,        /*!< Use CPU_INTR_4 as INTR #4 */
    ASDK_EXTI_INTR_CPU_5,        /*!< Use CPU_INTR_5 as INTR #5 */
    ASDK_EXTI_INTR_CPU_6,        /*!< Use CPU_INTR_6 as INTR #6 */
    ASDK_EXTI_INTR_CPU_7,        /*!< Use CPU_INTR_7 as INTR #7 */
    ASDK_EXTI_INTR_MAX,          /*!< Max number of External Interrupts */
    ASDK_EXTI_INTR_UNDEFINED = ASDK_EXTI_INTR_MAX, /*!< Values beyond @ref ASDK_EXTI_INTR_MAX is undefined. */
}asdk_exti_interrupt_num_t;

/*!
 * @brief Flash memory is abstracted based on the Base Addresses of the Sectors
 * Macros defined below gives the base addresses of all the code flasha and work flash sectors of CYT.
 * @note Please refer the following base addresses macro for using the flash
 * The non-blocking APIs are not supported in the current asdk version of CYT
 * The timeout values passed as parameter in the blocking Flash APIs are ignored since the timeout has been handled in the SDK of CYT.
 * For reading from code flash direct address de-referencing can be used.
 * For reading work flash recommended to use the asdk_flash_read_blocking() api.
 */

/*Work Flash Memory Partitioning*/
#define WORKFLASH_LARGE_START_ADDRESS           0x14000000
#define WORKFLASH_LARGE_END_ADDRESS             0x14011fff
#define WORKFLASH_SMALL_START_ADDRESS           0x14012000
#define WORKFLASH_SMALL_END_ADDRESS             0x14017FFF

/*Code Flash Memory Partitioning*/
#define CODE_LARGE_START_ADDR                   0x10000000
#define CODE_LARGE_END_ADDR                     0x100EFFFF
#define CODE_SMALL_START_ADDR                   0x100F0000
#define CODE_SMALL_END_ADDR                     0x1010FFFF

/*Base addresses of the Code Flash Large Sectors*/
#define CODE_FLASH_LS_0_BASE_ADDRESS   0x10000000 
#define CODE_FLASH_LS_1_BASE_ADDRESS   0x10008000 
#define CODE_FLASH_LS_2_BASE_ADDRESS   0x10010000 
#define CODE_FLASH_LS_3_BASE_ADDRESS   0x10018000 
#define CODE_FLASH_LS_4_BASE_ADDRESS   0x10020000 
#define CODE_FLASH_LS_5_BASE_ADDRESS   0x10028000 
#define CODE_FLASH_LS_6_BASE_ADDRESS   0x10030000 
#define CODE_FLASH_LS_7_BASE_ADDRESS   0x10038000 
#define CODE_FLASH_LS_8_BASE_ADDRESS   0x10040000 
#define CODE_FLASH_LS_9_BASE_ADDRESS   0x10048000 
#define CODE_FLASH_LS_10_BASE_ADDRESS   0x10050000 
#define CODE_FLASH_LS_11_BASE_ADDRESS   0x10058000 
#define CODE_FLASH_LS_12_BASE_ADDRESS   0x10060000 
#define CODE_FLASH_LS_13_BASE_ADDRESS   0x10068000 
#define CODE_FLASH_LS_14_BASE_ADDRESS   0x10070000 
#define CODE_FLASH_LS_15_BASE_ADDRESS   0x10078000 
#define CODE_FLASH_LS_16_BASE_ADDRESS   0x10080000 
#define CODE_FLASH_LS_17_BASE_ADDRESS   0x10088000 
#define CODE_FLASH_LS_18_BASE_ADDRESS   0x10090000 
#define CODE_FLASH_LS_19_BASE_ADDRESS   0x10098000 
#define CODE_FLASH_LS_20_BASE_ADDRESS   0x100a0000 
#define CODE_FLASH_LS_21_BASE_ADDRESS   0x100a8000 
#define CODE_FLASH_LS_22_BASE_ADDRESS   0x100b0000 
#define CODE_FLASH_LS_23_BASE_ADDRESS   0x100b8000 
#define CODE_FLASH_LS_24_BASE_ADDRESS   0x100c0000 
#define CODE_FLASH_LS_25_BASE_ADDRESS   0x100c8000 
#define CODE_FLASH_LS_26_BASE_ADDRESS   0x100d0000 
#define CODE_FLASH_LS_27_BASE_ADDRESS   0x100d8000 
#define CODE_FLASH_LS_28_BASE_ADDRESS   0x100e0000 
#define CODE_FLASH_LS_29_BASE_ADDRESS   0x100e8000 

/*Base address of code flash small sectors*/
#define CODE_FLASH_SS_0_BASE_ADDRESS   0x100f0000 
#define CODE_FLASH_SS_1_BASE_ADDRESS   0x100f2000 
#define CODE_FLASH_SS_2_BASE_ADDRESS   0x100f4000 
#define CODE_FLASH_SS_3_BASE_ADDRESS   0x100f6000 
#define CODE_FLASH_SS_4_BASE_ADDRESS   0x100f8000 
#define CODE_FLASH_SS_5_BASE_ADDRESS   0x100fa000 
#define CODE_FLASH_SS_6_BASE_ADDRESS   0x100fc000 
#define CODE_FLASH_SS_7_BASE_ADDRESS   0x100fe000 
#define CODE_FLASH_SS_8_BASE_ADDRESS   0x10100000 
#define CODE_FLASH_SS_9_BASE_ADDRESS   0x10102000 
#define CODE_FLASH_SS_10_BASE_ADDRESS   0x10104000 
#define CODE_FLASH_SS_11_BASE_ADDRESS   0x10106000 
#define CODE_FLASH_SS_12_BASE_ADDRESS   0x10108000 
#define CODE_FLASH_SS_13_BASE_ADDRESS   0x1010a000 
#define CODE_FLASH_SS_14_BASE_ADDRESS   0x1010c000 
#define CODE_FLASH_SS_15_BASE_ADDRESS   0x1010e000 

/*Base address of Work Flash Large Sectors*/
#define WORK_FLASH_LS_0_BASE_ADDRESS   0x14012000 
#define WORK_FLASH_LS_1_BASE_ADDRESS   0x14012080 
#define WORK_FLASH_LS_2_BASE_ADDRESS   0x14012100 
#define WORK_FLASH_LS_3_BASE_ADDRESS   0x14012180 
#define WORK_FLASH_LS_4_BASE_ADDRESS   0x14012200 
#define WORK_FLASH_LS_5_BASE_ADDRESS   0x14012280 
#define WORK_FLASH_LS_6_BASE_ADDRESS   0x14012300 
#define WORK_FLASH_LS_7_BASE_ADDRESS   0x14012380 
#define WORK_FLASH_LS_8_BASE_ADDRESS   0x14012400 
#define WORK_FLASH_LS_9_BASE_ADDRESS   0x14012480 
#define WORK_FLASH_LS_10_BASE_ADDRESS   0x14012500 
#define WORK_FLASH_LS_11_BASE_ADDRESS   0x14012580 
#define WORK_FLASH_LS_12_BASE_ADDRESS   0x14012600 
#define WORK_FLASH_LS_13_BASE_ADDRESS   0x14012680 
#define WORK_FLASH_LS_14_BASE_ADDRESS   0x14012700 
#define WORK_FLASH_LS_15_BASE_ADDRESS   0x14012780 
#define WORK_FLASH_LS_16_BASE_ADDRESS   0x14012800 
#define WORK_FLASH_LS_17_BASE_ADDRESS   0x14012880 
#define WORK_FLASH_LS_18_BASE_ADDRESS   0x14012900 
#define WORK_FLASH_LS_19_BASE_ADDRESS   0x14012980 
#define WORK_FLASH_LS_20_BASE_ADDRESS   0x14012a00 
#define WORK_FLASH_LS_21_BASE_ADDRESS   0x14012a80 
#define WORK_FLASH_LS_22_BASE_ADDRESS   0x14012b00 
#define WORK_FLASH_LS_23_BASE_ADDRESS   0x14012b80 
#define WORK_FLASH_LS_24_BASE_ADDRESS   0x14012c00 
#define WORK_FLASH_LS_25_BASE_ADDRESS   0x14012c80 
#define WORK_FLASH_LS_26_BASE_ADDRESS   0x14012d00 
#define WORK_FLASH_LS_27_BASE_ADDRESS   0x14012d80 
#define WORK_FLASH_LS_28_BASE_ADDRESS   0x14012e00 
#define WORK_FLASH_LS_29_BASE_ADDRESS   0x14012e80 
#define WORK_FLASH_LS_30_BASE_ADDRESS   0x14012f00 
#define WORK_FLASH_LS_31_BASE_ADDRESS   0x14012f80 
#define WORK_FLASH_LS_32_BASE_ADDRESS   0x14013000 
#define WORK_FLASH_LS_33_BASE_ADDRESS   0x14013080 
#define WORK_FLASH_LS_34_BASE_ADDRESS   0x14013100 
#define WORK_FLASH_LS_35_BASE_ADDRESS   0x14013180 

/*Base address of Work Flash Small Sectors*/
#define WORK_FLASH_SS_0_BASE_ADDRESS   0x14000000 
#define WORK_FLASH_SS_1_BASE_ADDRESS   0x14000800 
#define WORK_FLASH_SS_2_BASE_ADDRESS   0x14001000 
#define WORK_FLASH_SS_3_BASE_ADDRESS   0x14001800 
#define WORK_FLASH_SS_4_BASE_ADDRESS   0x14002000 
#define WORK_FLASH_SS_5_BASE_ADDRESS   0x14002800 
#define WORK_FLASH_SS_6_BASE_ADDRESS   0x14003000 
#define WORK_FLASH_SS_7_BASE_ADDRESS   0x14003800 
#define WORK_FLASH_SS_8_BASE_ADDRESS   0x14004000 
#define WORK_FLASH_SS_9_BASE_ADDRESS   0x14004800 
#define WORK_FLASH_SS_10_BASE_ADDRESS   0x14005000 
#define WORK_FLASH_SS_11_BASE_ADDRESS   0x14005800 
#define WORK_FLASH_SS_12_BASE_ADDRESS   0x14006000 
#define WORK_FLASH_SS_13_BASE_ADDRESS   0x14006800 
#define WORK_FLASH_SS_14_BASE_ADDRESS   0x14007000 
#define WORK_FLASH_SS_15_BASE_ADDRESS   0x14007800 
#define WORK_FLASH_SS_16_BASE_ADDRESS   0x14008000 
#define WORK_FLASH_SS_17_BASE_ADDRESS   0x14008800 
#define WORK_FLASH_SS_18_BASE_ADDRESS   0x14009000 
#define WORK_FLASH_SS_19_BASE_ADDRESS   0x14009800 
#define WORK_FLASH_SS_20_BASE_ADDRESS   0x1400a000 
#define WORK_FLASH_SS_21_BASE_ADDRESS   0x1400a800 
#define WORK_FLASH_SS_22_BASE_ADDRESS   0x1400b000 
#define WORK_FLASH_SS_23_BASE_ADDRESS   0x1400b800 
#define WORK_FLASH_SS_24_BASE_ADDRESS   0x1400c000 
#define WORK_FLASH_SS_25_BASE_ADDRESS   0x1400c800 
#define WORK_FLASH_SS_26_BASE_ADDRESS   0x1400d000 
#define WORK_FLASH_SS_27_BASE_ADDRESS   0x1400d800 
#define WORK_FLASH_SS_28_BASE_ADDRESS   0x1400e000 
#define WORK_FLASH_SS_29_BASE_ADDRESS   0x1400e800 
#define WORK_FLASH_SS_30_BASE_ADDRESS   0x1400f000 
#define WORK_FLASH_SS_31_BASE_ADDRESS   0x1400f800 
#define WORK_FLASH_SS_32_BASE_ADDRESS   0x14010000 
#define WORK_FLASH_SS_33_BASE_ADDRESS   0x14010800 
#define WORK_FLASH_SS_34_BASE_ADDRESS   0x14011000 
#define WORK_FLASH_SS_35_BASE_ADDRESS   0x14011800 
#define WORK_FLASH_SS_36_BASE_ADDRESS   0x14012000 
#define WORK_FLASH_SS_37_BASE_ADDRESS   0x14012800 
#define WORK_FLASH_SS_38_BASE_ADDRESS   0x14013000 
#define WORK_FLASH_SS_39_BASE_ADDRESS   0x14013800 
#define WORK_FLASH_SS_40_BASE_ADDRESS   0x14014000 
#define WORK_FLASH_SS_41_BASE_ADDRESS   0x14014800 
#define WORK_FLASH_SS_42_BASE_ADDRESS   0x14015000 
#define WORK_FLASH_SS_43_BASE_ADDRESS   0x14015800 
#define WORK_FLASH_SS_44_BASE_ADDRESS   0x14016000 
#define WORK_FLASH_SS_45_BASE_ADDRESS   0x14016800 
#define WORK_FLASH_SS_46_BASE_ADDRESS   0x14017000 
#define WORK_FLASH_SS_47_BASE_ADDRESS   0x14017800 
#define WORK_FLASH_SS_48_BASE_ADDRESS   0x14018000 
#define WORK_FLASH_SS_49_BASE_ADDRESS   0x14018800 
#define WORK_FLASH_SS_50_BASE_ADDRESS   0x14019000 
#define WORK_FLASH_SS_51_BASE_ADDRESS   0x14019800 
#define WORK_FLASH_SS_52_BASE_ADDRESS   0x1401a000 
#define WORK_FLASH_SS_53_BASE_ADDRESS   0x1401a800 
#define WORK_FLASH_SS_54_BASE_ADDRESS   0x1401b000 
#define WORK_FLASH_SS_55_BASE_ADDRESS   0x1401b800 
#define WORK_FLASH_SS_56_BASE_ADDRESS   0x1401c000 
#define WORK_FLASH_SS_57_BASE_ADDRESS   0x1401c800 
#define WORK_FLASH_SS_58_BASE_ADDRESS   0x1401d000 
#define WORK_FLASH_SS_59_BASE_ADDRESS   0x1401d800 
#define WORK_FLASH_SS_60_BASE_ADDRESS   0x1401e000 
#define WORK_FLASH_SS_61_BASE_ADDRESS   0x1401e800 
#define WORK_FLASH_SS_62_BASE_ADDRESS   0x1401f000 
#define WORK_FLASH_SS_63_BASE_ADDRESS   0x1401f800 
#define WORK_FLASH_SS_64_BASE_ADDRESS   0x14020000 
#define WORK_FLASH_SS_65_BASE_ADDRESS   0x14020800 
#define WORK_FLASH_SS_66_BASE_ADDRESS   0x14021000 
#define WORK_FLASH_SS_67_BASE_ADDRESS   0x14021800 
#define WORK_FLASH_SS_68_BASE_ADDRESS   0x14022000 
#define WORK_FLASH_SS_69_BASE_ADDRESS   0x14022800 
#define WORK_FLASH_SS_70_BASE_ADDRESS   0x14023000 
#define WORK_FLASH_SS_71_BASE_ADDRESS   0x14023800 
#define WORK_FLASH_SS_72_BASE_ADDRESS   0x14024000 
#define WORK_FLASH_SS_73_BASE_ADDRESS   0x14024800 
#define WORK_FLASH_SS_74_BASE_ADDRESS   0x14025000 
#define WORK_FLASH_SS_75_BASE_ADDRESS   0x14025800 
#define WORK_FLASH_SS_76_BASE_ADDRESS   0x14026000 
#define WORK_FLASH_SS_77_BASE_ADDRESS   0x14026800 
#define WORK_FLASH_SS_78_BASE_ADDRESS   0x14027000 
#define WORK_FLASH_SS_79_BASE_ADDRESS   0x14027800 
#define WORK_FLASH_SS_80_BASE_ADDRESS   0x14028000 
#define WORK_FLASH_SS_81_BASE_ADDRESS   0x14028800 
#define WORK_FLASH_SS_82_BASE_ADDRESS   0x14029000 
#define WORK_FLASH_SS_83_BASE_ADDRESS   0x14029800 
#define WORK_FLASH_SS_84_BASE_ADDRESS   0x1402a000 
#define WORK_FLASH_SS_85_BASE_ADDRESS   0x1402a800 
#define WORK_FLASH_SS_86_BASE_ADDRESS   0x1402b000 
#define WORK_FLASH_SS_87_BASE_ADDRESS   0x1402b800 
#define WORK_FLASH_SS_88_BASE_ADDRESS   0x1402c000 
#define WORK_FLASH_SS_89_BASE_ADDRESS   0x1402c800 
#define WORK_FLASH_SS_90_BASE_ADDRESS   0x1402d000 
#define WORK_FLASH_SS_91_BASE_ADDRESS   0x1402d800 
#define WORK_FLASH_SS_92_BASE_ADDRESS   0x1402e000 
#define WORK_FLASH_SS_93_BASE_ADDRESS   0x1402e800 
#define WORK_FLASH_SS_94_BASE_ADDRESS   0x1402f000 
#define WORK_FLASH_SS_95_BASE_ADDRESS   0x1402f800 
#define WORK_FLASH_SS_96_BASE_ADDRESS   0x14030000 
#define WORK_FLASH_SS_97_BASE_ADDRESS   0x14030800 
#define WORK_FLASH_SS_98_BASE_ADDRESS   0x14031000 
#define WORK_FLASH_SS_99_BASE_ADDRESS   0x14031800 
#define WORK_FLASH_SS_100_BASE_ADDRESS   0x14032000 
#define WORK_FLASH_SS_101_BASE_ADDRESS   0x14032800 
#define WORK_FLASH_SS_102_BASE_ADDRESS   0x14033000 
#define WORK_FLASH_SS_103_BASE_ADDRESS   0x14033800 
#define WORK_FLASH_SS_104_BASE_ADDRESS   0x14034000 
#define WORK_FLASH_SS_105_BASE_ADDRESS   0x14034800 
#define WORK_FLASH_SS_106_BASE_ADDRESS   0x14035000 
#define WORK_FLASH_SS_107_BASE_ADDRESS   0x14035800 
#define WORK_FLASH_SS_108_BASE_ADDRESS   0x14036000 
#define WORK_FLASH_SS_109_BASE_ADDRESS   0x14036800 
#define WORK_FLASH_SS_110_BASE_ADDRESS   0x14037000 
#define WORK_FLASH_SS_111_BASE_ADDRESS   0x14037800 
#define WORK_FLASH_SS_112_BASE_ADDRESS   0x14038000 
#define WORK_FLASH_SS_113_BASE_ADDRESS   0x14038800 
#define WORK_FLASH_SS_114_BASE_ADDRESS   0x14039000 
#define WORK_FLASH_SS_115_BASE_ADDRESS   0x14039800 
#define WORK_FLASH_SS_116_BASE_ADDRESS   0x1403a000 
#define WORK_FLASH_SS_117_BASE_ADDRESS   0x1403a800 
#define WORK_FLASH_SS_118_BASE_ADDRESS   0x1403b000 
#define WORK_FLASH_SS_119_BASE_ADDRESS   0x1403b800 
#define WORK_FLASH_SS_120_BASE_ADDRESS   0x1403c000 
#define WORK_FLASH_SS_121_BASE_ADDRESS   0x1403c800 
#define WORK_FLASH_SS_122_BASE_ADDRESS   0x1403d000 
#define WORK_FLASH_SS_123_BASE_ADDRESS   0x1403d800 
#define WORK_FLASH_SS_124_BASE_ADDRESS   0x1403e000 
#define WORK_FLASH_SS_125_BASE_ADDRESS   0x1403e800 
#define WORK_FLASH_SS_126_BASE_ADDRESS   0x1403f000 
#define WORK_FLASH_SS_127_BASE_ADDRESS   0x1403f800 
#define WORK_FLASH_SS_128_BASE_ADDRESS   0x14040000 
#define WORK_FLASH_SS_129_BASE_ADDRESS   0x14040800 
#define WORK_FLASH_SS_130_BASE_ADDRESS   0x14041000 
#define WORK_FLASH_SS_131_BASE_ADDRESS   0x14041800 
#define WORK_FLASH_SS_132_BASE_ADDRESS   0x14042000 
#define WORK_FLASH_SS_133_BASE_ADDRESS   0x14042800 
#define WORK_FLASH_SS_134_BASE_ADDRESS   0x14043000 
#define WORK_FLASH_SS_135_BASE_ADDRESS   0x14043800 
#define WORK_FLASH_SS_136_BASE_ADDRESS   0x14044000 
#define WORK_FLASH_SS_137_BASE_ADDRESS   0x14044800 
#define WORK_FLASH_SS_138_BASE_ADDRESS   0x14045000 
#define WORK_FLASH_SS_139_BASE_ADDRESS   0x14045800 
#define WORK_FLASH_SS_140_BASE_ADDRESS   0x14046000 
#define WORK_FLASH_SS_141_BASE_ADDRESS   0x14046800 
#define WORK_FLASH_SS_142_BASE_ADDRESS   0x14047000 
#define WORK_FLASH_SS_143_BASE_ADDRESS   0x14047800 
#define WORK_FLASH_SS_144_BASE_ADDRESS   0x14048000 
#define WORK_FLASH_SS_145_BASE_ADDRESS   0x14048800 
#define WORK_FLASH_SS_146_BASE_ADDRESS   0x14049000 
#define WORK_FLASH_SS_147_BASE_ADDRESS   0x14049800 
#define WORK_FLASH_SS_148_BASE_ADDRESS   0x1404a000 
#define WORK_FLASH_SS_149_BASE_ADDRESS   0x1404a800 
#define WORK_FLASH_SS_150_BASE_ADDRESS   0x1404b000 
#define WORK_FLASH_SS_151_BASE_ADDRESS   0x1404b800 
#define WORK_FLASH_SS_152_BASE_ADDRESS   0x1404c000 
#define WORK_FLASH_SS_153_BASE_ADDRESS   0x1404c800 
#define WORK_FLASH_SS_154_BASE_ADDRESS   0x1404d000 
#define WORK_FLASH_SS_155_BASE_ADDRESS   0x1404d800 
#define WORK_FLASH_SS_156_BASE_ADDRESS   0x1404e000 
#define WORK_FLASH_SS_157_BASE_ADDRESS   0x1404e800 
#define WORK_FLASH_SS_158_BASE_ADDRESS   0x1404f000 
#define WORK_FLASH_SS_159_BASE_ADDRESS   0x1404f800 
#define WORK_FLASH_SS_160_BASE_ADDRESS   0x14050000 
#define WORK_FLASH_SS_161_BASE_ADDRESS   0x14050800 
#define WORK_FLASH_SS_162_BASE_ADDRESS   0x14051000 
#define WORK_FLASH_SS_163_BASE_ADDRESS   0x14051800 
#define WORK_FLASH_SS_164_BASE_ADDRESS   0x14052000 
#define WORK_FLASH_SS_165_BASE_ADDRESS   0x14052800 
#define WORK_FLASH_SS_166_BASE_ADDRESS   0x14053000 
#define WORK_FLASH_SS_167_BASE_ADDRESS   0x14053800 
#define WORK_FLASH_SS_168_BASE_ADDRESS   0x14054000 
#define WORK_FLASH_SS_169_BASE_ADDRESS   0x14054800 
#define WORK_FLASH_SS_170_BASE_ADDRESS   0x14055000 
#define WORK_FLASH_SS_171_BASE_ADDRESS   0x14055800 
#define WORK_FLASH_SS_172_BASE_ADDRESS   0x14056000 
#define WORK_FLASH_SS_173_BASE_ADDRESS   0x14056800 
#define WORK_FLASH_SS_174_BASE_ADDRESS   0x14057000 
#define WORK_FLASH_SS_175_BASE_ADDRESS   0x14057800 
#define WORK_FLASH_SS_176_BASE_ADDRESS   0x14058000 
#define WORK_FLASH_SS_177_BASE_ADDRESS   0x14058800 
#define WORK_FLASH_SS_178_BASE_ADDRESS   0x14059000 
#define WORK_FLASH_SS_179_BASE_ADDRESS   0x14059800 
#define WORK_FLASH_SS_180_BASE_ADDRESS   0x1405a000 
#define WORK_FLASH_SS_181_BASE_ADDRESS   0x1405a800 
#define WORK_FLASH_SS_182_BASE_ADDRESS   0x1405b000 
#define WORK_FLASH_SS_183_BASE_ADDRESS   0x1405b800 
#define WORK_FLASH_SS_184_BASE_ADDRESS   0x1405c000 
#define WORK_FLASH_SS_185_BASE_ADDRESS   0x1405c800 
#define WORK_FLASH_SS_186_BASE_ADDRESS   0x1405d000 
#define WORK_FLASH_SS_187_BASE_ADDRESS   0x1405d800 
#define WORK_FLASH_SS_188_BASE_ADDRESS   0x1405e000 
#define WORK_FLASH_SS_189_BASE_ADDRESS   0x1405e800 
#define WORK_FLASH_SS_190_BASE_ADDRESS   0x1405f000 
#define WORK_FLASH_SS_191_BASE_ADDRESS   0x1405f800 


#endif