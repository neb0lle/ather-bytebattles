#include "debug_uart.h"
#include "ring_buffer.h"

#define PRINT_BUFFER_SIZE 64

typedef enum {
  DEBUG_UART_INIT,
  DEBUG_UART_SEND,
  DEBUG_UART_WAIT_FOR_TX_EVT,
} debug_uart_state_t;

static uint8_t _debug_uart_char_buffer[4096] = {0};
static asdk_errorcode_t sm_iter_err = ASDK_SUCCESS;
static uint32_t buff_len = 0;
static uint32_t read_len = 0;

static asdk_uart_config_t _debug_uart = {
    .uart_no = ASDK_UART_0, /*!< UART no. indicates the UART module no. of the ECU */
    .uart_tx_mcu_pin_no = MCU_PIN_3, /*!< UART TX mcu pin no */
    .uart_rx_mcu_pin_no = MCU_PIN_2, /*!< UART RX mcu pin no */

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
    // asdk_uart_dma_config_t uart_dma_config; /*!< UART DMA config */
};

static ring_buffer_t _debug_uart_buff = {
    .buffer = _debug_uart_char_buffer,
    .total_capacity = sizeof(_debug_uart_char_buffer),
    .used_capacity = 0,
    .block_size = 1,
    .enable_overwrite = false,
    .event_callback = NULL
};

static uint8_t _wc, _rc[PRINT_BUFFER_SIZE];
static volatile debug_uart_state_t _uart_state;
static volatile uint32_t max_buff_usage;

static void __uart_callback(asdk_uart_num_t uart_no, uint8_t *data, uint32_t data_len, asdk_uart_status_t event)
{
  switch (event)
  {
    case ASDK_UART_STATUS_TRANSMIT_COMPLETE:       /*!< Transmit complete. */
      if (uart_no == ASDK_UART_0)
      {
        if (_uart_state == DEBUG_UART_WAIT_FOR_TX_EVT)
        {
          _uart_state = DEBUG_UART_SEND;
        }
      }
      break;
    case ASDK_UART_STATUS_TRANSMIT_ERROR:          /*!< Error occurred in transmission. */
    case ASDK_UART_STATUS_RECEIVE_COMPLETE:        /*!< Receive complete */
    case ASDK_UART_STATUS_RECEIVE_OVERFLOW:        /*!< UART buffer overflow. */
    case ASDK_UART_STATUS_RECEIVE_ERR_FRAME:       /*!< Frame error, either a start or stop bit error on receive line */
    case ASDK_UART_STATUS_RECEIVE_ERR_PARITY:      /*!< Parity error on receive line */
    case ASDK_UART_STATUS_RECEIVE_BREAK_DETECT:    /*!< Break detect on receive line */
    default:
      break;
  }
}

static void __debug_uart_sm_init(void)
{
  ring_buffer_init(&_debug_uart_buff);

  _uart_state = DEBUG_UART_INIT;
}

static void __debug_uart_sm_iterate(void)
{ 
  if (max_buff_usage < _debug_uart_buff.used_capacity)
  {
    max_buff_usage = _debug_uart_buff.used_capacity;
  }

  switch (_uart_state)
  {
    case DEBUG_UART_INIT:
        sm_iter_err = asdk_uart_init(&_debug_uart);
        ASDK_DEV_ERROR_ASSERT(sm_iter_err, ASDK_UART_STATUS_SUCCESS);
        sm_iter_err = asdk_uart_install_callback(ASDK_UART_0, __uart_callback);
        ASDK_DEV_ERROR_ASSERT(sm_iter_err, ASDK_UART_STATUS_SUCCESS);
        _uart_state = DEBUG_UART_SEND;
        break;

    case DEBUG_UART_SEND:
      buff_len = _debug_uart_buff.used_capacity;
      
      if (buff_len == 0)
      {
        break;
      }

      if (buff_len >= PRINT_BUFFER_SIZE)
      {
        buff_len = PRINT_BUFFER_SIZE;
      }

      read_len = ring_buffer_read(&_debug_uart_buff, _rc, buff_len);

      _uart_state = DEBUG_UART_WAIT_FOR_TX_EVT;
      
      sm_iter_err = asdk_uart_write_non_blocking(ASDK_UART_0, _rc, read_len);
      ASDK_DEV_ERROR_ASSERT(sm_iter_err, ASDK_UART_STATUS_SUCCESS);

      break;

    case DEBUG_UART_WAIT_FOR_TX_EVT:
    default:
      break; 
  }
}

void _putchar(char character)
{
  // send char to console etc.
  _wc = character;
  ring_buffer_write(&_debug_uart_buff, &_wc, 1);
}

void debug_uart_init()
{
  __debug_uart_sm_init();
}

void debug_uart_iteration()
{
  __debug_uart_sm_iterate();
}

uint32_t debug_uart_get_max_usage(void)
{
  return max_buff_usage;
}
