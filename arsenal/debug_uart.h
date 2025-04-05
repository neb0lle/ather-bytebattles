#ifndef _DEBUG_UART_H_
#define _DEBUG_UART_H_

#include "asdk_uart.h"

void debug_uart_init(void);
void debug_uart_iteration(void);
uint32_t debug_uart_get_max_usage(void);

#endif // _DEBUG_UART_H_
