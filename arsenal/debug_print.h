#ifndef _DEBUG_PRINT_H_
#define _DEBUG_PRINT_H_

#include "asdk_system.h"
#include "printf.h"

#define DEBUG_PRINTF(fmt, ...) \
    printf("%lld: %s, %d: " fmt, asdk_sys_get_time_ms(), __func__, __LINE__, ##__VA_ARGS__)

#endif