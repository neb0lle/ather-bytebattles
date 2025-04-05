#ifndef ASDK_APP_H
#define ASDK_APP_H

#include "systick/cy_systick.h"

#define ASDK_DELAY(DELAY_MS) (Cy_SysTick_DelayInUs(DELAY_MS * 1000U));

void asdk_app_init();

#endif
