/* ASDK: do not edit main file ********************************************* */

/* Platform dependent includes */
#include "asdk_platform.h"

/* ASDK app includes */
#include "asdk_app.h"
#include "asdk_system.h"
#include "can_process.h"
#include "debug_uart.h"
#include "scheduler.h"

int main() {
    // asdk application
    asdk_app_init();

    for (;;) {
        scheduler_iteration();
        debug_uart_iteration();
    }

    return 0;
}

/* ASDK: do not edit main file ********************************************* */
