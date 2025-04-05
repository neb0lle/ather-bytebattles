/*
    @file
    asdk_system.c

    @path
    platform/cyt2b75/dal/src/asdk_system.c

    @Created on
    Sep 11, 2023

    @Author
    ajmeri.j

    @Copyright
    Copyright (c) Ather Energy Pvt Ltd.  All rights reserved.

    @brief
    This file implements the SYSTEM module for Ather SDK (asdk)

*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

/* standard includes ************************* */

/* asdk includes ***************************** */

#include "asdk_platform.h"

/* dal includes ****************************** */

#include "asdk_system.h"
#include "asdk_clock.h"
#include "asdk_timer.h"

/* sdk includes ****************************** */

#include "sysreset/cy_sysreset.h"

/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : MACROS

==============================================================================*/

/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

/*==============================================================================

                    LOCAL DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

/*==============================================================================

                            LOCAL FUNCTION PROTOTYPES

==============================================================================*/

#ifdef SYS_TIMER
static void __sys_timer_callback(asdk_timer_event_t timer_event);
#endif

/*==============================================================================

                        LOCAL AND EXTERNAL DEFINITIONS

==============================================================================*/

#ifdef USE_RTOS
extern uint64_t asdk_os_get_time_in_ms(void);
#endif

/* volatile global variables ***************** */

/* global variables ************************** */

/* static variables ************************** */

#ifdef SYS_TIMER
static asdk_timer_t sys_timer_config = {
    .type = ASDK_TIMER_TYPE_PERIODIC,
    .mode = {
        .type = ASDK_TIMER_MODE_TIMER,
        .config.timer = {
            .timer_period = 1000 /* ms */,
            .callback = __sys_timer_callback,
        },
    },
    .direction = ASDK_TIMER_COUNT_DIRECTION_UP,
    .interrupt = {
        .intr_num = ASDK_EXTI_INTR_CPU_4,
        .enable = true,
        .priority = 3,
    },
    .counter_clock = {
        .frequency = 1000000,
        .prescaler = ASDK_CLOCK_PRESCALER_1,
    },
};

static int64_t ms_counter_value = -1;
#endif

/*==============================================================================                                       \
                                                                                                                       \
							LOCAL FUNCTION DEFINITIONS                                                                 \
                                                                                                                       \
==============================================================================*/

/* static functions ************************** */

asdk_errorcode_t asdk_sys_init()
{
#ifdef SYS_TIMER
    asdk_timer_init(SYS_TIMER, &sys_timer_config);

    /* Making counter value to start from 0 and then starting timer */
    ms_counter_value = 0;
    asdk_timer_start(SYS_TIMER);
#endif

    return ASDK_SYS_SUCCESS;
}

void asdk_sys_sw_reset(void)
{
    NVIC_SystemReset();
}

void asdk_sys_enable_interrupts(void)
{
    __enable_irq();
}

void asdk_sys_disable_interrupts(void)
{
    __disable_irq();
}

inline uint32_t asdk_sys_get_core_clock_frequency(void)
{
    return SystemCoreClock;
}

int64_t asdk_sys_get_time_ms(void)
{
#ifdef USE_RTOS
    return asdk_os_get_time_in_ms();
#endif

#ifdef SYS_TIMER
    return ms_counter_value;
#else
    return -1;
#endif
}

asdk_sys_reset_t asdk_sys_get_reset_reason(void)
{
    asdk_sys_reset_t reason = ASDK_SYS_RESET_UNKNOWN;

    uint32_t sys_reset_reason = Cy_SysReset_GetResetReason();
    uint32_t clk_reset_reason = Cy_SysReset_GetNumHfclkResetCause();
    uint32_t cyt_reason = cyt_reason = sys_reset_reason ^ (sys_reset_reason & (sys_reset_reason - 1));

    if (clk_reset_reason)
    {
        cyt_reason = clk_reset_reason ^ (clk_reset_reason & (clk_reset_reason - 1));
    }
    else // system reset
    {
        cyt_reason = sys_reset_reason ^ (sys_reset_reason & (sys_reset_reason - 1));
    }

    switch (cyt_reason)
    {
    /* check for Low-voltage */
    case CY_SYSRESET_BODVDDD:
        reason = ASDK_SYS_RESET_LOW_VOLTAGE;
        break;

    /* check for clock loss reset */
    case CY_SYSRESET_CSV_HF:
        reason = ASDK_SYS_RESET_CLOCK_LOSS;
        break;

    /* check for Watchdog Reset */
    case CY_SYSRESET_WDT:
        reason = ASDK_SYS_RESET_WDG;
        break;

    /* Check for external pin reset */
    case CY_SYSRESET_XRES:
        reason = ASDK_SYS_RESET_EXT_PIN;
        break;

    /* check for Power on reset */
    case CY_SYSRESET_PORVDDD:
        reason = ASDK_SYS_RESET_POWERON;
        break;

    /* check for Software reset */
    case CY_SYSRESET_SOFT:
        reason = ASDK_SYS_RESET_SW;
        break;

    /* PMIC status triggered a reset. */
    case CY_SYSRESET_PMIC:
        reason = ASDK_SYS_RESET_PMIC;
        break;

    /* Reset due to over voltage detection on the external VDDD supply. */
    case CY_SYSRESET_OVDVDDD:
        reason = ASDK_SYS_RESET_OVDVDDD;
        break;

    /* Reset due to over voltage detection on the external VDDA supply.. */
    case CY_SYSRESET_OVDVDDA:
        reason = ASDK_SYS_RESET_OVDVDDA;
        break;

    /* Reset due to over voltage detection on the internal core VCCD supply. */
    case CY_SYSRESET_OVDVCCD:
        reason = ASDK_SYS_RESET_OVDVCCD;
        break;

    /* Reset due to external VDDA supply crossed the brown-out limit. */
    case CY_SYSRESET_BODVDDA:
        reason = ASDK_SYS_RESET_BODVDDA;
        break;

    /* Reset due to internal VCCD core supply crossed the brown-out limit. */
    case CY_SYSRESET_BODVCCD:
        reason = ASDK_SYS_RESET_BODVCCD;
        break;

    /* The fault logging system requested a reset from its DeepSleep logic. */
    case CY_SYSRESET_DPSLP_FAULT:
        reason = ASDK_SYS_RESET_DPSLP_FAULT;
        break;

    /* Programmable XRES triggered. */
    case CY_SYSRESET_PXRES:
        reason = ASDK_SYS_RESET_PXRES;
        break;

    /* Structural reset was asserted. */
    case CY_SYSRESET_STRUCT_XRES:
        reason = ASDK_SYS_REST_STRUCT_XRES;
        break;

    default:
        reason = ASDK_SYS_RESET_UNKNOWN;
        break;
    }


/*Clear all reset reasons so that the latest reset reason can be updated and returned to the application*/
    Cy_SysReset_ClearAllResetReasons();
    return reason;
}

#ifdef SYS_TIMER
void __sys_timer_callback(asdk_timer_event_t timer_event)
{
	switch (timer_event) {
	case ASDK_TIMER_TERMINAL_COUNT_EVENT:
        ms_counter_value++;
		break;

	default:
		break;
	}
}
#endif
