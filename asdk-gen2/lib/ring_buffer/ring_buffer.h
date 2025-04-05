/**
	Disclaimer:

	The information/code provided by Ather Energy Private Limited (“Ather”) in
	this file is for informational purposes only. All information/code in this
	file is provided in good faith.  However, Ather makes no representation or
	warranty of any kind, express or implied, regarding the accuracy, adequacy,
	validity, reliability, availability or completeness of any information/code
	in the file. Under no circumstances shall Ather have any liability to you
	for any loss or damage of any kind incurred as a result of reliance on any
	information/code provided in the file. Your reliance on any Ather’s
	information/code in the file is solely at your own risk.

	@file
	ring_buffer.h

	@path
	lib/ring_buffer/ring_buffer.h

	@Created on
	Dec 17, 2024

	@Author
	Ather Energy Pvt. Ltd.

	@Copyright
	Copyright (c) Ather Energy Pvt Ltd.  All rights reserved.

  @brief
  This file contains all the enumuration, structure types and functions in
  ring buffer.

  @example
  How to use the ring buffer module:
  @code

  // Define the ring buffer event callback function
  void ring_buffer_event_callback(ring_buffer *buffer_p,
  ring_buffer_event_t event, void *param)
  {
    uint32_t num_bytes_overwritten;
    if (event == RING_BUFFER_EVENT_FULL)
    {
        // Add your code for Buffer full event
    }
    else if (event == RING_BUFFER_EVENT_OVERWRITE)
    {
      num_bytes_overwritten = (uint32_t *)param;
        // Add your code for  Buffer overwrite event
    }
  }

  // Define the buffer
  uint8_t buffer[10] = {0};

  // Define the buffer structure
  ring_buffer_t ring_buffer = {
      .buffer = buffer,
      .total_capacity = sizeof(buffer),
      .block_size = 1,
      .write_index = 0,
      .read_index = 0,
      .used_capacity = 0,
      .enable_overwrite = false,
      .event_callback = ring_buffer_event_callback,
  };

  ring_buffer_error_t error;

  uint8_t wr_data[5] = {1,2,3,4,5};
  uint8_t rd_data[5] = {0};

  // Initialize the ring buffer
  error = ring_buffer_init(&ring_buffer);

  if (error != RING_BUFFER_SUCCESS)
  {
    // Handle the error
  }

  // Write 5 bytes into the buffer
  uint8_t num_of_blocks = ring_buffer_write(&ring_buffer, wr_data, 5);

  // Read 3 bytes from the buffer
  num_of_blocks = ring_buffer_read(&ring_buffer, rd_data, 3);

  @endcode
*/
#ifndef RING_BUFFER_H
#define RING_BUFFER_H

/*==============================================================================

                                  INCLUDE FILES

==============================================================================*/

/* standard includes ************************* */
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/*==============================================================================

                          DEFINITIONS AND TYPES : MACROS

==============================================================================*/

/*==============================================================================

                          DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

/** @defgroup ring_buffer_enum_group Enumerations
 *  Lists all the enumerations used by the ring buffer module.
 *  @{
 */

/*!
 * @brief An enumerator to represent the error codes representing possible
          errors during ring buffer operations.
 */
typedef enum {
  RING_BUFFER_SUCCESS,
  RING_BUFFER_ERROR_INVALID_PTR,
  RING_BUFFER_ERROR_INVALID_LEN,

  RING_BUFFER_ERROR_MAX = RING_BUFFER_ERROR_INVALID_LEN,
} ring_buffer_error_t;

/*!
 * @brief An enumerator to represent the possible events.
 */
typedef enum {
  RING_BUFFER_EVENT_FULL,
  RING_BUFFER_EVENT_OVERWRITE,

  RING_BUFFER_EVENT_MAX = RING_BUFFER_EVENT_OVERWRITE,
} ring_buffer_event_t;

/** @} */ // end of ring_buffer_enum_group

/*==============================================================================

                              CALLBACK FUNCTION TYPES

==============================================================================*/

/** @defgroup ring_buffer_cb_group Callback function type
 *  Lists the callback functions from the ring buffer module.
 *  @{
 */

typedef struct ring_buffer_t ring_buffer;

/*!
  @brief
  Exception callback for buffer write operation.

  @param [in] buffer_p Pointer to ring buffer.

  @param [in] exception Exception cause.

  @param [in] param Pointer to the value holding number of
              bytes overwritten.

  @return void
*/
typedef void (*ring_buffer_event_callback_t)(
    ring_buffer *buffer_p, ring_buffer_event_t exception, void *param);

/** @} */ // end of ring_buffer_cb_group

/*==============================================================================

                      DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

/** @defgroup ring_buffer_ds_group Data structures
 *  Lists all the data structures used by the ring buffer module.
 *  @{
 */

/*!
 * @brief Data structure for holding ring buffer configuration and current
 *        context
 *
 * Implements : ring_buffer_t
 */
typedef struct {
  void *buffer; /*!< Pointer to the buffer (typecast to (uint8_t *) while
                     assigning) */
  size_t total_capacity; /*!< Size of the buffer in bytes */
  size_t block_size;     /*!< Size of the block to be read/written in bytes */
  size_t write_index;   /*!< Index of the buffer at which enqueue can be done */
  size_t read_index;    /*!< Index of the buffer at which dequeue can be done */
  size_t used_capacity; /*!< Used capacity of the buffer */

  bool enable_overwrite; /*!< if true, buffer write when buffer is full will
                              overwrite into oldest element of buffer. */

  ring_buffer_event_callback_t
      event_callback; /*!< Callback to the user for buffer full/overwrite
                              event */
} ring_buffer_t;

/** @} */ // end of ring_buffer_ds_group

/*==============================================================================

                              EXTERNAL DECLARATIONS

==============================================================================*/

/*==============================================================================

                                FUNCTION PROTOTYPES

==============================================================================*/

/** @defgroup ring_buffer_fun_group Functions
 *  Lists the functions/APIs from the ring buffer module.
 *  @{
 */

/*----------------------------------------------------------------------------*/
/* Function : ring_buffer_init */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initializes and resets the ring buffer.

  @param [in] buffer_p Pointer to ring buffer.

  @return ring_buffer_error_t Error codes for the ring buffer operation
*/
ring_buffer_error_t ring_buffer_init(ring_buffer_t *buffer_p);

/*----------------------------------------------------------------------------*/
/* Function : ring_buffer_write */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Write data into the buffer.

  @param [in] buffer_p Pointer to ring buffer.

  @param [in] data_p Pointer to input data to be written into the buffer.

  @param [in] num_of_blocks Number of blocks to be written.

  @return size_t Number of Blocks written into the buffer
*/
size_t ring_buffer_write(ring_buffer_t *buffer_p, void *data_p,
                         size_t num_of_blocks);

/*----------------------------------------------------------------------------*/
/* Function : ring_buffer_read */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Read data from the buffer.

  @param [in] buffer_p Pointer to ring buffer.

  @param [in] data_p Pointer to output data to which the data read from the
  buffer will be written.

  @param [in] num_of_blocks Number of blocks to be read.

  @return size_t Number of Blocks read from the buffer
*/
size_t ring_buffer_read(ring_buffer_t *buffer_p, void *data_p,
                        size_t num_of_blocks);

/*----------------------------------------------------------------------------*/
/* Function : ring_buffer_peek */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Read data from the buffer without removing the data from the buffer.

  @param [in] buffer_p Pointer to ring buffer.

  @param [in] data_p Pointer to output data to which the data read from the
  buffer will be written.

  @param [in] num_of_blocks Number of blocks to be read.

  @return size_t Number of Blocks read from the buffer
*/
size_t ring_buffer_peek(ring_buffer_t *buffer_p, void *data_p,
                        size_t num_of_blocks);

/*----------------------------------------------------------------------------*/
/* Function : ring_buffer_is_empty */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Check if the buffer is empty.

  @param [in] buffer_p Pointer to ring buffer.

  @return bool returns true if the buffer is empty
*/
bool ring_buffer_is_empty(ring_buffer_t *buffer_p);

/*----------------------------------------------------------------------------*/
/* Function : ring_buffer_is_full */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Check if the buffer is full.

  @param [in] buffer_p Pointer to ring buffer.

  @return bool returns true if the buffer is full
*/
bool ring_buffer_is_full(ring_buffer_t *buffer_p);

/** @} */ // end of ring_buffer_fun_group

#endif /* RING_BUFFER_H */
