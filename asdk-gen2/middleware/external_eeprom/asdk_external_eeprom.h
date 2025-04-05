/*
	@file
	asdk_external_eeprom.h

	@path
	asdk-gen2/middleware/external_eeprom/asdk_external_eeprom.h

	@Created on
	Nov 27, 2023

	@Author
	siddharth.das

	@Copyright
	Copyright (c) Ather Energy Pvt Ltd. All rights reserved.

	@brief
	This file prototypes the External EEPROM Module for Ather SDK(asdk).
*/

#ifndef ASDK_EXTERNAL_EEPROM_H
#define ASDK_EXTERNAL_EEPROM_H

/*==============================================================================

							   INCLUDE FILES

==============================================================================*/

#include <stdio.h>
#include <string.h>
#include "asdk_error.h"
#include "asdk_i2c.h"
#include "asdk_spi.h"
#include "asdk_system.h"

/*==============================================================================

					  DEFINITIONS AND TYPES : MACROS

==============================================================================*/

#ifndef ASDK_EXTERNAL_EEPROM_MAX
#define ASDK_EXTERNAL_EEPROM_MAX 1U
#endif

/*==============================================================================

					  DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

/*!
 * @brief Protocol to be used for communicating with External EEPROM.
 *
 * Implements : asdk_external_eeprom_protocol_t
 *
 */
typedef enum {
	ASDK_EXTERNAL_EEPROM_PROTOCOL_I2C = 0,
	ASDK_EXTERNAL_EEPROM_PROTOCOL_SPI,
	ASDK_EXTERNAL_EEPROM_PROTOCOL_MAX,
	ASDK_EXTERNAL_EEPROM_PROTOCOL_UNDEFINED = ASDK_EXTERNAL_EEPROM_PROTOCOL_MAX,
} asdk_external_eeprom_protocol_t;

typedef enum {
	ASDK_EXTERNAL_EEPROM_STATUS_WRITE_COMPLETE = 0,
	ASDK_EXTERNAL_EEPROM_STATUS_READ_COMPLETE,
	ASDK_EXTERNAL_EEPROM_STATUS_MAX,
	ASDK_EXTERNAL_EEPROM_STATUS_UNDEFINED = ASDK_EXTERNAL_EEPROM_STATUS_MAX,
} asdk_external_eeprom_status_t;

/*==============================================================================

				   DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

/*!
 * @brief An union for I2C/SPI protocol configuration.
 * @note Currently only supports I2C protocol.
 *
 * Implements : asdk_external_eeprom_protocol_config_t
 */
typedef union {
	asdk_i2c_config_t i2c_config;
	asdk_spi_config_t spi_config;
} asdk_external_eeprom_protocol_config_t;

/*!
 * @brief External EEPROM callback function.
 *
 * @param [in] status Enum for callback reason to the application.
 */
typedef void (*asdk_external_eeprom_callback_fun_t)(asdk_external_eeprom_status_t status);

/*!
 * @brief An data structure for External EEPROM configuration.
 * @note Currently only supports I2C protocol.
 *
 * Implements : asdk_external_eeprom_config_t
 */
typedef struct {
	uint32_t memory_size_bytes;				  /* Memory size in bytes */
	uint16_t page_size_bytes;				  /* Page size in bytes */
	uint8_t addr_width;						  /* Address width in bytes */
	uint8_t *tx_buffer;						  /* Buffer to store EEPROM write address and page data */
	size_t tx_buffer_size;					  /* Size of buffer to store EEPROM write address and page data */
	uint8_t *rx_buffer;						  /* Bufffer to store EEPROM read address and page data */
	size_t rx_buffer_size;					  /* Size of buffer to store EEPROM read address and page data */
	uint32_t wait_time_us;					  /* Wait time for blocking APIs */
	bool is_big_endian;						  /* Endianness of the data */
	asdk_external_eeprom_protocol_t protocol; /* Protocol used by External EEPROM */
	asdk_external_eeprom_protocol_config_t protocol_config;	  /* Protocol configuration */
	asdk_external_eeprom_callback_fun_t eeprom_user_callback; /* Callback function for External EEPROM  */
} asdk_external_eeprom_config_t;

/*==============================================================================

						   EXTERNAL DECLARATIONS

==============================================================================*/

/*==============================================================================

						   FUNCTION PROTOTYPES

==============================================================================*/

/*----------------------------------------------------------------------------*/
/* Function : asdk_external_eeprom_init */
/*----------------------------------------------------------------------------*/
/*!
  @brief This function initializes the External EEPROM Module based on the config structure passed as parameter.
  @note The member temp_wr_buff in config structure, should have size greater than or equal to
		EEPROM address width and page size combined.

  @param uint8_t eeprom_index - Index of external EEPROM.
  @param asdk_external_eeprom_config_t ext_eeprom_config - External EEPROM configuration parameter.

  @return asdk_errorcode_t - Error code.
  @retval ASDK_MW_EXTERNAL_EEPROM_STATUS_SUCCESS
  @retval ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_SIZE
  @retval ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_PAGE_SIZE
*/
asdk_errorcode_t asdk_external_eeprom_init(uint8_t eeprom_index, asdk_external_eeprom_config_t *eeprom_config);

/*----------------------------------------------------------------------------*/
/* Function : asdk_external_eeprom_non_blocking_read */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This fucntion reads data from External EEPROM in a non-blocking manner.
  @note Call asdk_external_eeprom_iteration() in a loop for completing this read operation.

  @param uint8_t eeprom_index - Index of external EEPROM.
  @param uint32_t eeprom_addr - EEPROM address to be read.
  @param uint8_t *buff - Pointer to the buffer to store the read data.
  @param uint32_t length - Number of bytes to be read.

  @return asdk_errorcode_t - Error code.
  @retval ASDK_MW_EXTERNAL_EEPROM_STATUS_SUCCESS
  @retval ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_ADDRESS
  @retval ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_LENGTH
  @retval ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_BUFFER
  @retval ASDK_MW_EXTERNAL_EEPROM_ERROR_BUSY
*/
asdk_errorcode_t asdk_external_eeprom_non_blocking_read(uint8_t eeprom_index, uint32_t eeprom_addr, void *buff,
														uint32_t length);

/*----------------------------------------------------------------------------*/
/* Function : asdk_external_eeprom_blocking_read */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This fucntion reads data from External EEPROM in a blocking manner.

  @param uint8_t eeprom_index - Index of external EEPROM.
  @param uint32_t eeprom_addr - EEPROM address to be read.
  @param uint8_t *buff - Pointer to the buffer to store the read data.
  @param uint32_t length - Number of bytes to be read.
  @param uint16_t wait_time_ms - Waiting time-period in milliseconds.

  @return asdk_errorcode_t - Error code.
  @retval ASDK_MW_EXTERNAL_EEPROM_STATUS_SUCCESS
  @retval ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_ADDRESS
  @retval ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_LENGTH
  @retval ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_BUFFER
  @retval ASDK_MW_EXTERNAL_EEPROM_ERROR_BUSY
*/
asdk_errorcode_t asdk_external_eeprom_blocking_read(uint8_t eeprom_index, uint32_t eeprom_addr, void *buff,
													uint32_t length, uint16_t wait_time_ms);

/*----------------------------------------------------------------------------*/
/* Function : asdk_external_eeprom_non_blocking_write */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function wirtes data to External EEPROM in a non-blocking manner.
  @note Call asdk_external_eeprom_iteration() in a loop for completing this write operation.

  @param uint32_t eeprom_addr - EEPROM address to be written.
  @param uint8_t *buff - Pointer to the buffer containing the data to be written.
  @param uint32_t length - Number of bytes to be written.

  @return asdk_errorcode_t - Error code.
  @retval ASDK_MW_EXTERNAL_EEPROM_STATUS_SUCCESS
  @retval ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_ADDRESS
  @retval ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_LENGTH
  @retval ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_BUFFER
  @retval ASDK_MW_EXTERNAL_EEPROM_ERROR_BUSY
*/
asdk_errorcode_t asdk_external_eeprom_non_blocking_write(uint8_t eeprom_index, uint32_t eeprom_addr, void *buff,
														 uint32_t length);

/*----------------------------------------------------------------------------*/
/* Function : asdk_external_eeprom_blocking_write */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function wirtes data to External EEPROM in a blocking manner.

  @param uint8_t eeprom_index - Index of external EEPROM.
  @param uint32_t eeprom_addr - EEPROM address to be written.
  @param uint8_t *buff - Pointer to the buffer containing the data to be written.
  @param uint32_t length - Number of bytes to be written.
  @param uint16_t wait_time_ms - Waiting time-period in milliseconds.

  @return asdk_errorcode_t - Error code.
  @retval ASDK_MW_EXTERNAL_EEPROM_STATUS_SUCCESS
  @retval ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_ADDRESS
  @retval ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_LENGTH
  @retval ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_BUFFER
  @retval ASDK_MW_EXTERNAL_EEPROM_ERROR_BUSY
*/
asdk_errorcode_t asdk_external_eeprom_blocking_write(uint8_t eeprom_index, uint32_t eeprom_addr, void *buff,
													 uint32_t length, uint16_t wait_time_ms);

/*----------------------------------------------------------------------------*/
/* Function : asdk_external_eeprom_iteration */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function writes and read data pending in the global write buffer to External EEPROM
  and from EEPROM to user read buffer.

  @param uint8_t eeprom_index - Index of external EEPROM.

  @return asdk_errorcode_t - Error code.
  @retval ASDK_MW_EXTERNAL_EEPROM_STATUS_SUCCESS
  @retval ASDK_MW_EXTERNAL_EEPROM_ERROR_BUSY
  @retval ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_ADDRESS
  @retval ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_LENGTH
  @retval ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_BUFFER
*/
asdk_errorcode_t asdk_external_eeprom_iteration(uint8_t eeprom_index);

#endif /* ASDK_EXTERNAL_EEPROM_H */
