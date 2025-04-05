/*
   @file
   ring_buffer.c

   @path
   lib/ring_buffer/ring_buffer.c

   @Created on
   Dec 17, 2024

   @Author
   Ather Energy Pvt Ltd.

   @Copyright
   Copyright (c) Ather Energy Pvt Ltd.  All rights reserved.

   @brief
   This file implements the functions in the ring buffer module.
*/

/*==============================================================================

z                                INCLUDE FILES

==============================================================================*/

/* standard includes ************************* */
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/* asdk includes ************************* */
#include "asdk_platform.h"

/* rin buffer includes ************************* */
#include "ring_buffer.h"

#define CALCULATE_SEGMENT_LENGTH(buff_idx, num_of_bytes, buff_size)            \
  ((buff_idx + num_of_bytes) < buff_size) ? num_of_bytes                       \
                                          : (buff_size - buff_idx)

#define EXIT_CRITICAL_SECTION_AND_RETURN(num_of_blocks)                        \
  {                                                                            \
    ASDK_EXIT_CRITICAL_SECTION()                                               \
    return num_of_blocks;                                                      \
  }

/*==============================================================================

                            LOCAL FUNCTION DECLARATIONS

==============================================================================*/

/*==============================================================================

                            GLOBAL FUNCTION DEFINITIONS

==============================================================================*/
/*******************************************************************************
 * Function ring_buffer_init
 ****************************************************************************/
/**
 *
 * Initialization function of ring buffer.
 *
 *******************************************************************************/
ring_buffer_error_t ring_buffer_init(ring_buffer_t *buffer_p) {
  if (buffer_p == NULL) {
    return RING_BUFFER_ERROR_INVALID_PTR;
  }

  if (buffer_p->buffer == NULL) {
    return RING_BUFFER_ERROR_INVALID_PTR;
  }

  if (buffer_p->total_capacity == 0) {
    return RING_BUFFER_ERROR_INVALID_LEN;
  }

  if (buffer_p->block_size == 0) {
    return RING_BUFFER_ERROR_INVALID_LEN;
  }

  if ((buffer_p->total_capacity % buffer_p->block_size) != 0) {
    return RING_BUFFER_ERROR_INVALID_LEN;
  }

  ASDK_ENTER_CRITICAL_SECTION()

  buffer_p->read_index = 0;
  buffer_p->write_index = 0;
  buffer_p->used_capacity = 0;

  ASDK_EXIT_CRITICAL_SECTION()

  return RING_BUFFER_SUCCESS;
}

/*******************************************************************************
 * Function ring_buffer_write
 ****************************************************************************/
/**
 *
 * Write to ring buffer.
 *
 *******************************************************************************/
size_t ring_buffer_write(ring_buffer_t *buffer_p, void *data_p,
                         size_t num_of_blocks) {
  uint8_t *buffer_ptr;
  size_t buff_size_bytes;
  size_t buff_len;
  ring_buffer_event_callback_t buff_exc_cb;
  bool buff_ovrwr_flag;
  size_t total_num_of_bytes;
  size_t num_bytes_overwritten;
  size_t buff_wr_idx;
  bool is_full;
  size_t wrap_around_len;
  size_t remaining_space;
  bool overwrite_required;
  ring_buffer_event_t exception_type;

  /* Null pointer check, Requested write length check */
  if ((buffer_p == NULL) || (num_of_blocks == 0) || (data_p == NULL)) {
    return 0;
  }

  /* Enter critical section */
  ASDK_ENTER_CRITICAL_SECTION()

  buffer_ptr = (uint8_t *)(buffer_p->buffer);

  /* Null pointer check */
  if (buffer_ptr == NULL) {
    /* Exit critical section and return 0, as no bytes were written */
    EXIT_CRITICAL_SECTION_AND_RETURN(0)
  }

  buff_size_bytes = buffer_p->total_capacity;
  total_num_of_bytes = num_of_blocks * buffer_p->block_size;
  num_bytes_overwritten = 0;
 
  /* Can't write some value that will end up overwriting itself */
  if (total_num_of_bytes > buff_size_bytes) {
    EXIT_CRITICAL_SECTION_AND_RETURN(0)
  }

  buff_len = buffer_p->used_capacity;
  is_full = (buff_size_bytes == buff_len);
  buff_exc_cb = buffer_p->event_callback;
  buff_ovrwr_flag = buffer_p->enable_overwrite;
  num_bytes_overwritten = 0;

  /* Buffer is full and overwrite is not enabled */
  if ((is_full == true) && (buff_ovrwr_flag == false)) {
    if (buff_exc_cb != NULL) {
      buff_exc_cb((ring_buffer *)buffer_p, RING_BUFFER_EVENT_FULL,
                  &num_bytes_overwritten);
    }

    EXIT_CRITICAL_SECTION_AND_RETURN(0)
  }

  remaining_space = (buff_size_bytes - buff_len);

  /* Check if overwrite is required */
  overwrite_required = (total_num_of_bytes > remaining_space);
  if (overwrite_required) {
    if (buff_ovrwr_flag == false) {
      total_num_of_bytes = (buff_size_bytes - buff_len);
    } else {
      num_bytes_overwritten = total_num_of_bytes - remaining_space;
    }
  }

  buff_wr_idx = buffer_p->write_index;

  wrap_around_len = 0;

  /**
   * if requested write size is less than space available between write index
   * and end of buffer, then write can be performed with a single memcpy.
   *
   * if requested write size is more than space available between write index
   * and end of buffer, the write has to be performed with 2 memcpy.
   * This is being referred as wrap-around.
   * first memcpy will be till the end of the buffer, second memcpy will write
   * the remaining bytes.
   **/
  wrap_around_len = CALCULATE_SEGMENT_LENGTH(buff_wr_idx, total_num_of_bytes,
                                             buff_size_bytes);
  memcpy(&(buffer_ptr[buff_wr_idx]), data_p, wrap_around_len);

  buff_wr_idx = ((buff_wr_idx + wrap_around_len) % buff_size_bytes);
  buff_len += wrap_around_len;

  /* check for wrap around, if it exists write remaining bytes in 2nd chunk */
  if (wrap_around_len != total_num_of_bytes) {
    memcpy(&(buffer_ptr[buff_wr_idx]), data_p + wrap_around_len,
           total_num_of_bytes - wrap_around_len);

    buff_wr_idx = ((buff_wr_idx + (total_num_of_bytes - wrap_around_len)) %
                   buff_size_bytes);

    buff_len += (total_num_of_bytes - wrap_around_len);
  }

  /* Cap "used capacity" at "total capacity" */
  if (buff_len > buff_size_bytes) {
    buff_len = buff_size_bytes;
  }

  /* Update is_full flag again */
  is_full = (buff_size_bytes == buff_len);

  /* If buffer is full, either overwrite occurred or it was exactly full */
  exception_type = RING_BUFFER_EVENT_FULL;
  if (is_full) {
    /* Overwrite occurred if num_bytes_overwritten > 0 */
    if (num_bytes_overwritten > 0) {
      /* Shift read index by one from write index */
      buffer_p->read_index = buff_wr_idx;
      exception_type = RING_BUFFER_EVENT_OVERWRITE;
    }

    if (buff_exc_cb != NULL) {
      buff_exc_cb((ring_buffer *)buffer_p, exception_type,
                  &num_bytes_overwritten);
    }
  }

  /* Restore the index values and used capacity into the buffer */
  buffer_p->write_index = buff_wr_idx;
  buffer_p->used_capacity = buff_len;

  EXIT_CRITICAL_SECTION_AND_RETURN(total_num_of_bytes / buffer_p->block_size)
}

/*******************************************************************************
 * Function ring_buffer_read
 ****************************************************************************/
/**
 *
 * Read from ring buffer.
 *
 *******************************************************************************/
size_t ring_buffer_read(ring_buffer_t *buffer_p, void *data_p,
                        size_t num_of_blocks) {
  size_t wrap_around_len;
  uint8_t *buffer_ptr;
  size_t buff_rd_idx;
  size_t buff_size_bytes;
  size_t buff_len;
  size_t total_num_of_bytes;

  /* Null pointer check, Requested read length check */
  if ((buffer_p == NULL) || (num_of_blocks == 0) || (data_p == NULL)) {
    return 0;
  }

  /* Enter critical section */
  ASDK_ENTER_CRITICAL_SECTION()

  buffer_ptr = (uint8_t *)(buffer_p->buffer);

  /* Null pointer check */
  if (buffer_ptr == NULL) {
    /* Exit critical section and return 0, as no bytes were written */
    EXIT_CRITICAL_SECTION_AND_RETURN(0)
  }

  buff_len = buffer_p->used_capacity;

  /* Buffer is empty */
  if (buff_len == 0) {
    /* Exit critical section and return 0, as no bytes were written */
    EXIT_CRITICAL_SECTION_AND_RETURN(0)
  }

  total_num_of_bytes = num_of_blocks * buffer_p->block_size;

  /* read length requested is more than used capacity of the buffer */
  if (total_num_of_bytes > buff_len) {
    total_num_of_bytes = buff_len;
  }

  buff_rd_idx = buffer_p->read_index;
  buff_size_bytes = buffer_p->total_capacity;
  wrap_around_len = 0;

  /**
   * if requested read size is less than space available between read index
   * and end of buffer, then read can be performed with a single memcpy.
   *
   * if requested read size is more than space available between read index
   * and end of buffer, the read has to be performed with 2 memcpy.
   * This is being referred as wrap-around.
   * first memcpy will be till the end of the buffer, second memcpy will read
   * the remaining bytes.
   **/
  wrap_around_len = CALCULATE_SEGMENT_LENGTH(buff_rd_idx, total_num_of_bytes,
                                             buff_size_bytes);

  memcpy(data_p, &(buffer_ptr[buff_rd_idx]), wrap_around_len);

  buff_rd_idx = ((buff_rd_idx + wrap_around_len) % buff_size_bytes);

  buff_len -= wrap_around_len;
  /* check for wrap around, if exists read remaining bytes in 2nd chunk */
  if (wrap_around_len != total_num_of_bytes) {
    memcpy(data_p + wrap_around_len, &(buffer_ptr[buff_rd_idx]),
           total_num_of_bytes - wrap_around_len);

    buff_rd_idx = ((buff_rd_idx + (total_num_of_bytes - wrap_around_len)) %
                   buff_size_bytes);

    buff_len -= (total_num_of_bytes - wrap_around_len);
  }

  /* Restore the index values and used capacity into the buffer */
  buffer_p->read_index = buff_rd_idx;
  buffer_p->used_capacity = buff_len;

  EXIT_CRITICAL_SECTION_AND_RETURN(total_num_of_bytes / buffer_p->block_size)
}

/*******************************************************************************
 * Function ring_buffer_peek
 ****************************************************************************/
/**
 *
 * Read from ring buffer without removing the data from the buffer.
 *
 *******************************************************************************/
size_t ring_buffer_peek(ring_buffer_t *buffer_p, void *data_p,
                        size_t num_of_blocks) {
  size_t wrap_around_len;
  size_t temp_idx;

  uint8_t *buffer_ptr;
  size_t buff_rd_idx;
  size_t buff_size_bytes;
  size_t buff_len;
  size_t total_num_of_bytes;

  /* Null pointer check, Requested read length check */
  if ((buffer_p == NULL) || (num_of_blocks == 0) || (data_p == NULL)) {
    return 0;
  }

  /* Enter critical section */
  ASDK_ENTER_CRITICAL_SECTION()

  buffer_ptr = (uint8_t *)(buffer_p->buffer);

  /* Null pointer check */
  if (buffer_ptr == NULL) {
    /* Exit critical section and return 0, as no bytes were written */
    EXIT_CRITICAL_SECTION_AND_RETURN(0)
  }

  buff_len = buffer_p->used_capacity;

  if (buff_len == 0) {
    /* Exit critical section and return 0, as no bytes were written */
    EXIT_CRITICAL_SECTION_AND_RETURN(0)
  }

  total_num_of_bytes = num_of_blocks * buffer_p->block_size;

  /* read length requested is more than used capacity of the buffer */
  if (total_num_of_bytes > buff_len) {
    total_num_of_bytes = buff_len;
  }

  buff_size_bytes = buffer_p->total_capacity;
  temp_idx = 0;
  buff_rd_idx = buffer_p->read_index;
  wrap_around_len = 0;

  /**
   * if requested read size is less than space available between read index
   * and end of buffer, then read can be performed with a single memcpy.
   *
   * if requested read size is more than space available between read index
   * and end of buffer, the read has to be performed with 2 memcpy.
   * This is being referred as wrap-around.
   * first memcpy will be till the end of the buffer, second memcpy will read
   * the remaining bytes.
   **/
  wrap_around_len = CALCULATE_SEGMENT_LENGTH(buff_rd_idx, total_num_of_bytes,
                                             buff_size_bytes);
  memcpy(data_p, &(buffer_ptr[buff_rd_idx]), wrap_around_len);

  /* Check for wrap around, if it exists read remaining bytes in 2nd chunk */
  if (wrap_around_len != total_num_of_bytes) {
    temp_idx = ((buff_rd_idx + wrap_around_len) % buff_size_bytes);

    memcpy(data_p + wrap_around_len, &(buffer_ptr[temp_idx]),
           total_num_of_bytes - wrap_around_len);
  }

  /* Exit critical section */
  EXIT_CRITICAL_SECTION_AND_RETURN(total_num_of_bytes / buffer_p->block_size)
}

/*******************************************************************************
 * Function ring_buffer_is_empty
 ****************************************************************************/
/**
 *
 * Function to check if the ring buffer is empty.
 *
 *******************************************************************************/
bool ring_buffer_is_empty(ring_buffer_t *buffer_p) {
  return (buffer_p->used_capacity == 0);
}

/*******************************************************************************
 * Function ring_buffer_is_full
 ****************************************************************************/
/**
 *
 * Function to check if the ring buffer is full.
 *
 *******************************************************************************/
bool ring_buffer_is_full(ring_buffer_t *buffer_p) {
  return (buffer_p->used_capacity == buffer_p->total_capacity);
}
