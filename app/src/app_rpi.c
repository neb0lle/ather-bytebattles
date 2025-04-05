/* Platform dependent includes */
#include "asdk_platform.h"

/* ASDK includes */
#include "asdk_error.h"

/* Application specific includes */
#include <string.h>
#include "app_rpi.h"

/* Debug Print includes */
#include "debug_print.h"

#define RPI_UART ASDK_UART_2

#define RPI_UART_TX_PIN MCU_PIN_61
#define RPI_UART_RX_PIN MCU_PIN_60

static asdk_uart_config_t __rpi_uart = {
    .uart_no = RPI_UART, /*!< UART no. indicates the UART module no. of the ECU */
    .uart_tx_mcu_pin_no = RPI_UART_TX_PIN, /*!< UART TX mcu pin no */
    .uart_rx_mcu_pin_no = RPI_UART_RX_PIN, /*!< UART RX mcu pin no */

    .op_mode = ASDK_UART_MODE_STANDARD, /*!< UART Operation Mode */
    .baud_rate = ASDK_UART_BAUD_RATE_115200, /*!< UART Baud Rate */
    .data_bits =ASDK_UART_DATA_BITS_8, /*!< UART Data bits count */
    .parity_mode = ASDK_UART_PARITY_NONE, /*!< UART Parity Mode */
    .stop_bits = ASDK_UART_STOP_BITS_1, /*!< UART Stop bits count, currently 1 or 2 stop bits supported */

    .interrupt_config = {
      .use_interrupt = true,
      .intr_num = ASDK_EXTI_INTR_CPU_3,
      .priority = 1,
    }, /*!< UART interrupt config */
};

static bool __rpi_data = false;
static uint8_t __rpi_data_buffer[17];

static void __rpi_uart_callback(asdk_uart_num_t uart_no, uint8_t *data, uint32_t data_len, asdk_uart_status_t event);

void app_rpi_init()
{
    asdk_errorcode_t err = ASDK_UART_STATUS_SUCCESS;

    err = asdk_uart_init(&__rpi_uart);
    ASDK_DEV_ERROR_ASSERT(ASDK_UART_STATUS_SUCCESS, err);

    err = asdk_uart_install_callback(RPI_UART, __rpi_uart_callback);
    ASDK_DEV_ERROR_ASSERT(ASDK_UART_STATUS_SUCCESS, err);

    // DEBUG_PRINTF("RPI UART initialized successfully\r\n");

    err = asdk_uart_read_non_blocking(RPI_UART, __rpi_data_buffer, sizeof(__rpi_data_buffer));
    ASDK_DEV_ERROR_ASSERT(ASDK_UART_STATUS_SUCCESS, err);
}

void app_rpi_iteration()
{
    if (__rpi_data)
    {
        // DEBUG_PRINTF("Data received from RPI: %s\r\n", __rpi_data_buffer);
        asdk_uart_read_non_blocking(RPI_UART, __rpi_data_buffer, sizeof(__rpi_data_buffer));
        __rpi_data = false;
    }
}

static void __rpi_uart_callback(asdk_uart_num_t uart_no, uint8_t *data, uint32_t data_len, asdk_uart_status_t event)
{
  switch (event)
  {
    case ASDK_UART_STATUS_RECEIVE_COMPLETE:        /*!< Receive complete */
      if (uart_no == RPI_UART)
      {
        __rpi_data = true;
      }
      break;
    case ASDK_UART_STATUS_TRANSMIT_COMPLETE:       /*!< Transmit complete. */
    case ASDK_UART_STATUS_TRANSMIT_ERROR:          /*!< Error occurred in transmission. */
    case ASDK_UART_STATUS_RECEIVE_OVERFLOW:        /*!< UART buffer overflow. */
    case ASDK_UART_STATUS_RECEIVE_ERR_FRAME:       /*!< Frame error, either a start or stop bit error on receive line */
    case ASDK_UART_STATUS_RECEIVE_ERR_PARITY:      /*!< Parity error on receive line */
    case ASDK_UART_STATUS_RECEIVE_BREAK_DETECT:    /*!< Break detect on receive line */
    default:
      break;
  }
}
