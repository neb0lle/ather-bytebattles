/* ASDK User actions starts ************************************************ */

/* ASDK User Action: add application include files */
#include "asdk_system.h"
#include "asdk_clock.h"

#include "debug_uart.h"
#include "scheduler.h"

#include "app_gpio.h"
#include "app_can.h"
#include "app_adc.h"
#include "app_rpi.h"
#include "ultrasonic.h"
#include "colour.h"

#include "asdk_app.h"

/* Debug Print includes */
#include "debug_print.h"

/* Todo: Color calibiration */

#define RED_NO_LIGHT_VALUE      328
#define RED_WHITE_LIGHT_VALUE   127 
#define GREEN_NO_LIGHT_VALUE    387
#define GREEN_WHITE_LIGHT_VALUE 142
#define BLUE_NO_LIGHT_VALUE      332
#define BLUE_WHITE_LIGHT_VALUE   109

colour_components_t colour_c;
colour_components_t colour_map;

/* ASDK User Action: Declare new task below */

static void task_always_run(void);
static void task_1ms(void);
static void task_5ms(void);
static void task_10ms(void);
static void task_100ms(void);
static void task_1000ms(void);

/* ASDK User Action: Update the scheduler with newly added task */

static scheduler_t scheduler_config[] = {
    /* Task Function               Periodicity   */
    { .task_fn = task_always_run, .periodicty = 0 },
    { .task_fn = task_1ms,         .periodicty = 1 },
    { .task_fn = task_5ms,         .periodicty = 5 },
    { .task_fn = task_10ms,       .periodicty = 10 },
    { .task_fn = task_100ms,      .periodicty = 100 },
    { .task_fn = task_1000ms,     .periodicty = 1000 },
};

static size_t scheduler_size = sizeof(scheduler_config) / sizeof(scheduler_t);

/* below functions are called by main.c */

void asdk_app_init()
{
    asdk_clock_config_t clk_cfg = {
        .clk_source = ASDK_CLOCK_SRC_INT,
        .pll = {
            .input_frequency = 8000000,
            .output_frequency = 160000000,
        },
        .xtal_trim = {0}
    };

    /* Initialize clock */
    asdk_clock_init(&clk_cfg);
    asdk_sys_init();

    /* Initialize scheduler */
    scheduler_init(scheduler_config, scheduler_size);

    /* Initialize UART for debug messages */
    debug_uart_init();

    /* ASDK User Action: Add init calls here */
    app_gpio_init();
    app_can_init();
    app_adc_init();
    app_rpi_init();
    app_adc_start_conversion();
    colour_sensor_init();

    /* Enabling the interrupt */
    asdk_sys_enable_interrupts();
}

/* ASDK User Action: Define newly added tasks below */

static void task_1ms(void)
{
    ultrasonic_iterations();
    get_colour_sensor_reading(&colour_c);
}

static void task_5ms(void)
{
    colour_sensor_iteration();

    if(get_current_filter() == BLUE_FILTER)
    {
        /* Colour detection */

        // colour_map.red = MAP(colour_c.red, RED_NO_LIGHT_VALUE, RED_WHITE_LIGHT_VALUE);
        // colour_map.green = MAP(colour_c.green, GREEN_NO_LIGHT_VALUE, GREEN_WHITE_LIGHT_VALUE);
        // colour_map.blue = MAP(colour_c.blue, BLUE_NO_LIGHT_VALUE, BLUE_WHITE_LIGHT_VALUE);

        /* Todo: Take action after detecting the right colour */
    }

}

static void task_10ms(void)
{ 
    // DEBUG_PRINTF("Max Debug Uart Buffer Usage %d\r\n", debug_uart_get_max_usage());
    app_rpi_iteration();
}

static void task_100ms(void)
{
    app_gpio_iteration();
    app_can_iteration();
    app_adc_iteration();
}

static void task_1000ms(void)
{
    app_ldr_iteration();
}

static void task_always_run(void)
{
    asdk_can_service_send_iteration(VEHICLE_CAN);
    asdk_can_service_receive_iteration(VEHICLE_CAN);
    debug_uart_iteration();
}

/* ASDK User actions ends ************************************************** */
