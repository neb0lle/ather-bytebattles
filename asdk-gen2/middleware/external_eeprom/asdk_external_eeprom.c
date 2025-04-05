/*
	@file
	asdk_external_eeprom.c

	@path
	middleware/external_eeprom/asdk_external_eeprom.c

	@Created on
	Nov 27, 2023

	@Author
	siddharth.das

	@Copyright
	Copyright (c) Ather Energy Pvt Ltd.  All rights reserved.

	@brief
	This file implements the External EEPROM module for Ather SDK (asdk)

*/

/*==============================================================================

						   INCLUDE FILES

==============================================================================*/

#include "asdk_external_eeprom.h"

/*==============================================================================

					  LOCAL DEFINITIONS AND TYPES : MACROS

==============================================================================*/

/*==============================================================================

					LOCAL DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

/*!
 * @brief An data structure for EEPROM read/write management.
 *
 * Implements : eeprom_manager_t
 */
typedef struct {
	uint32_t address;		  /* Address where data will be read from or written to external EEPROM */
	uint8_t *buffer;		  /* Pointer to the buffer where data will be read from or written to external EEPROM */
	uint32_t bytes_remaining; /* Number of bytes remaining for read or write */
	uint32_t length;		  /* Length of the data to be read or write */
	uint32_t index;			  /* Index value used for tracking purposes */
	bool is_op_pending;		  /* Flag indicating whether a read or write operation is pending or not */
	bool is_cb_pending;		  /* Flag indicating whether a callback is issued or not */
} eeprom_manager_t;

/*!
 * @brief An data structure for External EEPROM operation.
 *
 * Implements : ext_eeprom_t
 */
typedef struct {
	asdk_external_eeprom_config_t eeprom_config; /* Config from user */
	eeprom_manager_t eeprom_write_mgr;			 /* Write manager */
	eeprom_manager_t eeprom_read_mgr;			 /* Read manager */
	bool read_complete;							 /* Flag indicating whether read operation is complete or not */
	bool write_complete;						 /* Flag indicating whether write operation is complete or not */
} ext_eeprom_t;

/* Static global variable to store External EEPROM configurations and manage read/write operations */
static ext_eeprom_t g_ext_eeprom[ASDK_EXTERNAL_EEPROM_MAX];

/*==============================================================================

							LOCAL FUNCTION PROTOTYPES

==============================================================================*/

static asdk_errorcode_t __asdk_external_eeprom_page_write(uint8_t eeprom_index, uint32_t start_addr, void *buff,
														  uint16_t wr_len);
static asdk_errorcode_t __asdk_external_eeprom_page_read(uint8_t eeprom_index, uint32_t start_addr, void *buff,
														 uint16_t rd_len);
static asdk_errorcode_t __asdk_i2c_eeprom_init(asdk_i2c_config_t *i2c_config);
static void __asdk_i2c_eeprom_callback(asdk_i2c_num_t i2c_no, uint8_t *data, uint32_t data_size,
									   asdk_i2c_status_t status);

/*==============================================================================

						LOCAL AND EXTERNAL DEFINITIONS

==============================================================================*/

/*==============================================================================

							LOCAL FUNCTION DEFINITIONS

==============================================================================*/

asdk_errorcode_t __asdk_i2c_eeprom_init(asdk_i2c_config_t *i2c_config)
{
	/* Updating necessary I2C configuration */
	i2c_config->i2c_mode = ASDK_I2C_MODE_MASTER;
	i2c_config->interrupt_config.use_interrupt = true;

	if ((0 == i2c_config->interrupt_config.priority) || (1 == i2c_config->interrupt_config.priority))
		i2c_config->interrupt_config.priority = 2;

	return asdk_i2c_init(i2c_config);
}

void __asdk_i2c_eeprom_callback(asdk_i2c_num_t i2c_no, uint8_t *data, uint32_t data_size, asdk_i2c_status_t status)
{
	static uint8_t eeprom_index = 0;

	for (uint8_t i = 0; i < ASDK_EXTERNAL_EEPROM_MAX; i++) {
		if (g_ext_eeprom[i].eeprom_config.protocol == ASDK_EXTERNAL_EEPROM_PROTOCOL_I2C) {
			if (g_ext_eeprom[i].eeprom_config.protocol_config.i2c_config.i2c_no == i2c_no) {
				eeprom_index = i;
				break;
			}
		}
	}

	if (ASDK_I2C_STATUS_WR_COMPLETE == status) {
		g_ext_eeprom[eeprom_index].write_complete = true;

		if (g_ext_eeprom[eeprom_index].eeprom_write_mgr.is_cb_pending == true) {
			g_ext_eeprom[eeprom_index].eeprom_config.eeprom_user_callback(ASDK_EXTERNAL_EEPROM_STATUS_WRITE_COMPLETE);
			g_ext_eeprom[eeprom_index].eeprom_write_mgr.is_cb_pending = false;
		}
	}

	if (ASDK_I2C_STATUS_RD_COMPLETE == status) {
		g_ext_eeprom[eeprom_index].read_complete = true;

		if (g_ext_eeprom[eeprom_index].eeprom_read_mgr.is_cb_pending == true) {
			g_ext_eeprom[eeprom_index].eeprom_config.eeprom_user_callback(ASDK_EXTERNAL_EEPROM_STATUS_READ_COMPLETE);
			g_ext_eeprom[eeprom_index].eeprom_read_mgr.is_cb_pending = false;
		}
	}
}

/*!
 * @todo Add support for SPI protocol.
 * @note If called continously, blocked for write cycle time of EEPROM.
 */
static asdk_errorcode_t __asdk_external_eeprom_page_write(uint8_t eeprom_index, uint32_t start_addr, void *buff,
														  uint16_t wr_len)
{
	asdk_errorcode_t ret_val = ASDK_MW_EXTERNAL_EEPROM_STATUS_SUCCESS;

	if ((false == g_ext_eeprom[eeprom_index].read_complete) || (false == g_ext_eeprom[eeprom_index].write_complete))
		return ASDK_MW_EXTERNAL_EEPROM_ERROR_BUSY;

	g_ext_eeprom[eeprom_index].write_complete = false;

	/* Copying EEPROM write address, MSB first */
	for (uint8_t i = 0; i < g_ext_eeprom[eeprom_index].eeprom_config.addr_width; i++)
		if (g_ext_eeprom[eeprom_index].eeprom_config.is_big_endian == true)
			g_ext_eeprom[eeprom_index].eeprom_config.tx_buffer[i] = (start_addr >> (8 * i)) & 0xFF;
		else
			g_ext_eeprom[eeprom_index]
				.eeprom_config.tx_buffer[g_ext_eeprom[eeprom_index].eeprom_config.addr_width - i - 1] =
				(start_addr >> (8 * i)) & 0xFF;

	/* Copying data from application buffer to static global buffer */
	memcpy(&g_ext_eeprom[eeprom_index].eeprom_config.tx_buffer[g_ext_eeprom[eeprom_index].eeprom_config.addr_width],
		   buff, wr_len);

	if (ASDK_EXTERNAL_EEPROM_PROTOCOL_I2C == g_ext_eeprom[eeprom_index].eeprom_config.protocol) {
		asdk_i2c_config_t *p_i2c_config = &g_ext_eeprom[eeprom_index].eeprom_config.protocol_config.i2c_config;
		asdk_i2c_status_t i2c_status = ASDK_I2C_STATUS_UNDEFINED;

		do {
			ret_val = asdk_i2c_master_write_non_blocking(p_i2c_config->i2c_no, p_i2c_config->slave_Address,
														 g_ext_eeprom[eeprom_index].eeprom_config.tx_buffer,
														 g_ext_eeprom[eeprom_index].eeprom_config.addr_width + wr_len);
			ASDK_DEV_ERROR_RETURN(ret_val, ASDK_I2C_STATUS_SUCCESS);
			asdk_i2c_get_transfer_status(g_ext_eeprom[eeprom_index].eeprom_config.protocol_config.i2c_config.i2c_no,
										 &i2c_status);
		} while (i2c_status == ASDK_I2C_STATUS_MASTER_ADDR_NACK);

		ret_val = ASDK_MW_EXTERNAL_EEPROM_STATUS_SUCCESS;
	} else {
		ret_val = ASDK_MW_EXTERNAL_EEPROM_ERROR_FEATURE_NOT_IMPLEMENTED;
	}

	return ret_val;
}

/*!
 * @todo Add support for SPI protocol.
 */
static asdk_errorcode_t __asdk_external_eeprom_page_read(uint8_t eeprom_index, uint32_t start_addr, void *buff,
														 uint16_t rd_len)
{
	asdk_errorcode_t ret_val = ASDK_MW_EXTERNAL_EEPROM_STATUS_SUCCESS;
	uint8_t read_addr[g_ext_eeprom[eeprom_index].eeprom_config.addr_width];

	if ((false == g_ext_eeprom[eeprom_index].read_complete) || (false == g_ext_eeprom[eeprom_index].write_complete))
		return ASDK_MW_EXTERNAL_EEPROM_ERROR_BUSY;

	g_ext_eeprom[eeprom_index].write_complete = false;
	g_ext_eeprom[eeprom_index].read_complete = false;

	/* Copying EEPROM read address, MSB first */
	for (uint8_t i = 0; i < g_ext_eeprom[eeprom_index].eeprom_config.addr_width; i++)
		if (g_ext_eeprom[eeprom_index].eeprom_config.is_big_endian == true)
			read_addr[i] = (start_addr >> (8 * i)) & 0xFF;
		else
			read_addr[g_ext_eeprom[eeprom_index].eeprom_config.addr_width - i - 1] = (start_addr >> (8 * i)) & 0xFF;

	if (ASDK_EXTERNAL_EEPROM_PROTOCOL_I2C == g_ext_eeprom[eeprom_index].eeprom_config.protocol) {
		asdk_i2c_config_t *p_i2c_config = &g_ext_eeprom[eeprom_index].eeprom_config.protocol_config.i2c_config;
		asdk_i2c_status_t i2c_status = ASDK_I2C_STATUS_UNDEFINED;

		ret_val = asdk_i2c_master_write_non_blocking(p_i2c_config->i2c_no, p_i2c_config->slave_Address, read_addr,
													 g_ext_eeprom[eeprom_index].eeprom_config.addr_width);
		ASDK_DEV_ERROR_RETURN(ret_val, ASDK_I2C_STATUS_SUCCESS);

		while (g_ext_eeprom[eeprom_index].write_complete == false) {
			asdk_i2c_get_transfer_status(p_i2c_config->i2c_no, &i2c_status);
			if (ASDK_I2C_STATUS_MASTER_ADDR_NACK == i2c_status) {
				ret_val =
					asdk_i2c_master_write_non_blocking(p_i2c_config->i2c_no, p_i2c_config->slave_Address, read_addr,
													   g_ext_eeprom[eeprom_index].eeprom_config.addr_width);
				ASDK_DEV_ERROR_RETURN(ret_val, ASDK_I2C_STATUS_SUCCESS);
			}
		}

		ret_val = asdk_i2c_master_read_non_blocking(p_i2c_config->i2c_no, p_i2c_config->slave_Address, buff, rd_len);
		ASDK_DEV_ERROR_RETURN(ret_val, ASDK_I2C_STATUS_SUCCESS);

		ret_val = ASDK_MW_EXTERNAL_EEPROM_STATUS_SUCCESS;

	} else {
		ret_val = ASDK_MW_EXTERNAL_EEPROM_ERROR_FEATURE_NOT_IMPLEMENTED;
	}

	return ret_val;
}

/*!
 * @todo Add support for SPI protocol.
 */
asdk_errorcode_t asdk_external_eeprom_init(uint8_t eeprom_index, asdk_external_eeprom_config_t *eeprom_config)
{
	asdk_errorcode_t ret_val = ASDK_ERROR;

	/* validate configuration parameters */
	if (0 == eeprom_config->memory_size_bytes)
		return ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_SIZE;

	if (0 == eeprom_config->page_size_bytes)
		return ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_PAGE_SIZE;

	if (0 == eeprom_config->addr_width)
		return ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_ADDRESS_WIDTH;

	if (NULL == eeprom_config->tx_buffer)
		return ASDK_MW_EXTERNAL_EEPROM_ERROR_NULL_PTR;

	if ((eeprom_config->page_size_bytes + eeprom_config->addr_width) > eeprom_config->tx_buffer_size)
		return ASDK_MW_EXTERNAL_EEPROM_ERROR_LESS_BUFFER_SIZE;

	if (eeprom_index >= ASDK_EXTERNAL_EEPROM_MAX)
		return ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_INDEX;

	/* Init peripheral respective to protocol configured by user */
	switch (eeprom_config->protocol) {
	case ASDK_EXTERNAL_EEPROM_PROTOCOL_I2C: {
		ret_val = __asdk_i2c_eeprom_init(&eeprom_config->protocol_config.i2c_config);
		if (ASDK_I2C_STATUS_SUCCESS != ret_val)
			return ret_val;

		asdk_i2c_install_callback(eeprom_config->protocol_config.i2c_config.i2c_no,
								  (asdk_i2c_callback_fun_t)__asdk_i2c_eeprom_callback);

		ret_val = ASDK_MW_EXTERNAL_EEPROM_STATUS_SUCCESS;
	} break;

	case ASDK_EXTERNAL_EEPROM_PROTOCOL_SPI:
		return ASDK_MW_EXTERNAL_EEPROM_ERROR_FEATURE_NOT_IMPLEMENTED; //todo: add support for SPI protocol
		break;

	default:
		return ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_PROTOCOL;
		break;
	}

	/* Copy EEPROM configuration to global variable */
	memcpy(&g_ext_eeprom[eeprom_index].eeprom_config, eeprom_config, sizeof(asdk_external_eeprom_config_t));

	g_ext_eeprom[eeprom_index].read_complete = g_ext_eeprom[eeprom_index].write_complete = true;

	return ret_val;
}

asdk_errorcode_t asdk_external_eeprom_non_blocking_read(uint8_t eeprom_index, uint32_t eeprom_addr, void *buff,
														uint32_t length)
{
	if ((g_ext_eeprom[eeprom_index].eeprom_config.memory_size_bytes - 1) <= eeprom_addr)
		return ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_ADDRESS;

	if (g_ext_eeprom[eeprom_index].eeprom_config.memory_size_bytes < (eeprom_addr + length))
		return ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_LENGTH;

	if (NULL == buff)
		return ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_BUFFER;

	if (g_ext_eeprom[eeprom_index].eeprom_read_mgr.is_op_pending == true)
		return ASDK_MW_EXTERNAL_EEPROM_ERROR_BUSY;

	if (eeprom_index >= ASDK_EXTERNAL_EEPROM_MAX)
		return ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_INDEX;

	g_ext_eeprom[eeprom_index].eeprom_read_mgr.address = eeprom_addr;
	g_ext_eeprom[eeprom_index].eeprom_read_mgr.buffer = buff;
	g_ext_eeprom[eeprom_index].eeprom_read_mgr.length =
		g_ext_eeprom[eeprom_index].eeprom_config.page_size_bytes -
		(g_ext_eeprom[eeprom_index].eeprom_read_mgr.address % g_ext_eeprom[eeprom_index].eeprom_config.page_size_bytes);

	/* Check if read length is less than data length to be read */
	if (length < g_ext_eeprom[eeprom_index].eeprom_read_mgr.length)
		g_ext_eeprom[eeprom_index].eeprom_read_mgr.length = length;

	g_ext_eeprom[eeprom_index].eeprom_read_mgr.bytes_remaining =
		length - g_ext_eeprom[eeprom_index].eeprom_read_mgr.length;
	g_ext_eeprom[eeprom_index].eeprom_read_mgr.is_op_pending = true;

	return ASDK_MW_EXTERNAL_EEPROM_STATUS_SUCCESS;
}

asdk_errorcode_t asdk_external_eeprom_blocking_read(uint8_t eeprom_index, uint32_t eeprom_addr, void *buff,
													uint32_t length, uint16_t wait_time_ms)
{
	asdk_errorcode_t ret_val = ASDK_ERROR;
	eeprom_manager_t eeprom_read_mgr = { 0 };
	int32_t start_time_ms = asdk_sys_get_time_ms();

	if ((g_ext_eeprom[eeprom_index].eeprom_config.memory_size_bytes - 1) <= eeprom_addr)
		return ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_ADDRESS;

	if (g_ext_eeprom[eeprom_index].eeprom_config.memory_size_bytes < (eeprom_addr + length))
		return ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_LENGTH;

	if (NULL == buff)
		return ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_BUFFER;

	if (start_time_ms < 0)
		return ASDK_SYS_ERROR_TIMER_INIT_FAILED;

	if (eeprom_index >= ASDK_EXTERNAL_EEPROM_MAX)
		return ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_INDEX;

	eeprom_read_mgr.address = eeprom_addr;
	eeprom_read_mgr.buffer = buff;
	eeprom_read_mgr.length = g_ext_eeprom[eeprom_index].eeprom_config.page_size_bytes -
							 (eeprom_read_mgr.address % g_ext_eeprom[eeprom_index].eeprom_config.page_size_bytes);

	/* Check if read length is less than data length to be read */
	if (eeprom_read_mgr.length > length)
		eeprom_read_mgr.length = length;

	eeprom_read_mgr.bytes_remaining = length - eeprom_read_mgr.length;
	eeprom_read_mgr.is_op_pending = true;

	while (eeprom_read_mgr.is_op_pending == true) {
		ret_val =
			__asdk_external_eeprom_page_read(eeprom_index, eeprom_read_mgr.address,
											 (eeprom_read_mgr.buffer + eeprom_read_mgr.index), eeprom_read_mgr.length);
		ASDK_DEV_ERROR_RETURN(ret_val, ASDK_MW_EXTERNAL_EEPROM_STATUS_SUCCESS);

		while ((g_ext_eeprom[eeprom_index].read_complete != true) &&
			   (asdk_sys_get_time_ms() < start_time_ms + wait_time_ms))
			;

		if (asdk_sys_get_time_ms() > start_time_ms + wait_time_ms)
			return ASDK_MW_EXTERNAL_EEPROM_ERROR_TIMEOUT;

		eeprom_read_mgr.index += eeprom_read_mgr.length;
		eeprom_read_mgr.address += eeprom_read_mgr.length;
		eeprom_read_mgr.length =
			(eeprom_read_mgr.bytes_remaining >= g_ext_eeprom[eeprom_index].eeprom_config.page_size_bytes)
				? g_ext_eeprom[eeprom_index].eeprom_config.page_size_bytes
				: eeprom_read_mgr.bytes_remaining;
		eeprom_read_mgr.bytes_remaining -= eeprom_read_mgr.length;

		if ((eeprom_read_mgr.bytes_remaining == 0) && (eeprom_read_mgr.length == 0))
			eeprom_read_mgr.is_op_pending = false;
	}

	return ret_val;
}

asdk_errorcode_t asdk_external_eeprom_non_blocking_write(uint8_t eeprom_index, uint32_t eeprom_addr, void *buff,
														 uint32_t length)
{
	if ((g_ext_eeprom[eeprom_index].eeprom_config.memory_size_bytes - 1) <= eeprom_addr)
		return ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_ADDRESS;

	if (g_ext_eeprom[eeprom_index].eeprom_config.memory_size_bytes < (eeprom_addr + length))
		return ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_LENGTH;

	if (NULL == buff)
		return ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_BUFFER;

	if (true == g_ext_eeprom[eeprom_index].eeprom_write_mgr.is_op_pending)
		return ASDK_MW_EXTERNAL_EEPROM_ERROR_BUSY;

	if (eeprom_index >= ASDK_EXTERNAL_EEPROM_MAX)
		return ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_INDEX;

	g_ext_eeprom[eeprom_index].eeprom_write_mgr.address = eeprom_addr;
	g_ext_eeprom[eeprom_index].eeprom_write_mgr.buffer = buff;
	g_ext_eeprom[eeprom_index].eeprom_write_mgr.length = g_ext_eeprom[eeprom_index].eeprom_config.page_size_bytes -
														 (g_ext_eeprom[eeprom_index].eeprom_write_mgr.address %
														  g_ext_eeprom[eeprom_index].eeprom_config.page_size_bytes);

	/* Check if write length is less than data length to be written */
	if (length < g_ext_eeprom[eeprom_index].eeprom_write_mgr.length)
		g_ext_eeprom[eeprom_index].eeprom_write_mgr.length = length;

	g_ext_eeprom[eeprom_index].eeprom_write_mgr.bytes_remaining =
		length - g_ext_eeprom[eeprom_index].eeprom_write_mgr.length;
	g_ext_eeprom[eeprom_index].eeprom_write_mgr.is_op_pending = true;

	return ASDK_MW_EXTERNAL_EEPROM_STATUS_SUCCESS;
}

/*!
 * @note This API is blocked for I2C wirte time plus EEPROM's write cycle time.
 */
asdk_errorcode_t asdk_external_eeprom_blocking_write(uint8_t eeprom_index, uint32_t eeprom_addr, void *buff,
													 uint32_t length, uint16_t wait_time_ms)
{
	asdk_errorcode_t ret_val = ASDK_ERROR;
	eeprom_manager_t eeprom_write_mgr = { 0 };
	int32_t start_time_ms = asdk_sys_get_time_ms();

	if ((g_ext_eeprom[eeprom_index].eeprom_config.memory_size_bytes - 1) <= eeprom_addr)
		return ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_ADDRESS;

	if (g_ext_eeprom[eeprom_index].eeprom_config.memory_size_bytes < (eeprom_addr + length))
		return ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_LENGTH;

	if (NULL == buff)
		return ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_BUFFER;

	if (start_time_ms < 0)
		return ASDK_SYS_ERROR_TIMER_INIT_FAILED;

	if (eeprom_index >= ASDK_EXTERNAL_EEPROM_MAX)
		return ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_INDEX;

	eeprom_write_mgr.address = eeprom_addr;
	eeprom_write_mgr.buffer = buff;
	eeprom_write_mgr.length = g_ext_eeprom[eeprom_index].eeprom_config.page_size_bytes -
							  (eeprom_write_mgr.address % g_ext_eeprom[eeprom_index].eeprom_config.page_size_bytes);

	/* Check if write length is less than data length to be written */
	if (eeprom_write_mgr.length > length)
		eeprom_write_mgr.length = length;

	eeprom_write_mgr.bytes_remaining = length - eeprom_write_mgr.length;
	eeprom_write_mgr.is_op_pending = true;

	while (eeprom_write_mgr.is_op_pending == true) {
		ret_val = __asdk_external_eeprom_page_write(eeprom_index, eeprom_write_mgr.address,
													(eeprom_write_mgr.buffer + eeprom_write_mgr.index),
													eeprom_write_mgr.length);
		ASDK_DEV_ERROR_RETURN(ret_val, ASDK_MW_EXTERNAL_EEPROM_STATUS_SUCCESS);

		while ((g_ext_eeprom[eeprom_index].write_complete != true) &&
			   (asdk_sys_get_time_ms() < start_time_ms + wait_time_ms))
			;

		if (asdk_sys_get_time_ms() > start_time_ms + wait_time_ms)
			return ASDK_MW_EXTERNAL_EEPROM_ERROR_TIMEOUT;

		eeprom_write_mgr.index += eeprom_write_mgr.length;
		eeprom_write_mgr.address += eeprom_write_mgr.length;
		eeprom_write_mgr.length =
			(eeprom_write_mgr.bytes_remaining >= g_ext_eeprom[eeprom_index].eeprom_config.page_size_bytes)
				? g_ext_eeprom[eeprom_index].eeprom_config.page_size_bytes
				: eeprom_write_mgr.bytes_remaining;
		eeprom_write_mgr.bytes_remaining -= eeprom_write_mgr.length;

		if ((eeprom_write_mgr.bytes_remaining == 0) && (eeprom_write_mgr.length == 0))
			eeprom_write_mgr.is_op_pending = false;
	}

	return ret_val;
}

/*!
 * @note If calling frequency is more than EEPROM's write cycle time,
 * then this API shall be blocked for previous write cycle to finish.
 */
asdk_errorcode_t asdk_external_eeprom_iteration(uint8_t eeprom_index)
{
	asdk_errorcode_t ret_val = ASDK_ERROR;

	if (eeprom_index >= ASDK_EXTERNAL_EEPROM_MAX)
		return ASDK_MW_EXTERNAL_EEPROM_ERROR_INVALID_INDEX;

	if (true == g_ext_eeprom[eeprom_index].eeprom_write_mgr.is_op_pending) {
		ret_val = __asdk_external_eeprom_page_write(
			eeprom_index, g_ext_eeprom[eeprom_index].eeprom_write_mgr.address,
			(g_ext_eeprom[eeprom_index].eeprom_write_mgr.buffer + g_ext_eeprom[eeprom_index].eeprom_write_mgr.index),
			g_ext_eeprom[eeprom_index].eeprom_write_mgr.length);
		ASDK_DEV_ERROR_RETURN(ret_val, ASDK_MW_EXTERNAL_EEPROM_STATUS_SUCCESS);

		g_ext_eeprom[eeprom_index].eeprom_write_mgr.index += g_ext_eeprom[eeprom_index].eeprom_write_mgr.length;
		g_ext_eeprom[eeprom_index].eeprom_write_mgr.address += g_ext_eeprom[eeprom_index].eeprom_write_mgr.length;
		g_ext_eeprom[eeprom_index].eeprom_write_mgr.length =
			(g_ext_eeprom[eeprom_index].eeprom_write_mgr.bytes_remaining >=
			 g_ext_eeprom[eeprom_index].eeprom_config.page_size_bytes)
				? g_ext_eeprom[eeprom_index].eeprom_config.page_size_bytes
				: g_ext_eeprom[eeprom_index].eeprom_write_mgr.bytes_remaining;
		g_ext_eeprom[eeprom_index].eeprom_write_mgr.bytes_remaining -=
			g_ext_eeprom[eeprom_index].eeprom_write_mgr.length;

		if ((0 == g_ext_eeprom[eeprom_index].eeprom_write_mgr.bytes_remaining) &&
			(0 == g_ext_eeprom[eeprom_index].eeprom_write_mgr.length)) {
			g_ext_eeprom[eeprom_index].eeprom_write_mgr.buffer = NULL;
			g_ext_eeprom[eeprom_index].eeprom_write_mgr.address = ~0;
			g_ext_eeprom[eeprom_index].eeprom_write_mgr.index = 0;
			g_ext_eeprom[eeprom_index].eeprom_write_mgr.is_op_pending = false;
			g_ext_eeprom[eeprom_index].eeprom_write_mgr.is_cb_pending = true;
		}
	} else if (g_ext_eeprom[eeprom_index].eeprom_read_mgr.is_op_pending == true) {
		ret_val = __asdk_external_eeprom_page_read(
			eeprom_index, g_ext_eeprom[eeprom_index].eeprom_read_mgr.address,
			(g_ext_eeprom[eeprom_index].eeprom_read_mgr.buffer + g_ext_eeprom[eeprom_index].eeprom_read_mgr.index),
			g_ext_eeprom[eeprom_index].eeprom_read_mgr.length);
		ASDK_DEV_ERROR_RETURN(ret_val, ASDK_MW_EXTERNAL_EEPROM_STATUS_SUCCESS);

		g_ext_eeprom[eeprom_index].eeprom_read_mgr.index += g_ext_eeprom[eeprom_index].eeprom_read_mgr.length;
		g_ext_eeprom[eeprom_index].eeprom_read_mgr.address += g_ext_eeprom[eeprom_index].eeprom_read_mgr.length;
		g_ext_eeprom[eeprom_index].eeprom_read_mgr.length =
			(g_ext_eeprom[eeprom_index].eeprom_read_mgr.bytes_remaining >=
			 g_ext_eeprom[eeprom_index].eeprom_config.page_size_bytes)
				? g_ext_eeprom[eeprom_index].eeprom_config.page_size_bytes
				: g_ext_eeprom[eeprom_index].eeprom_read_mgr.bytes_remaining;
		g_ext_eeprom[eeprom_index].eeprom_read_mgr.bytes_remaining -= g_ext_eeprom[eeprom_index].eeprom_read_mgr.length;

		if ((g_ext_eeprom[eeprom_index].eeprom_read_mgr.bytes_remaining == 0) &&
			(g_ext_eeprom[eeprom_index].eeprom_read_mgr.length == 0)) {
			g_ext_eeprom[eeprom_index].eeprom_read_mgr.buffer = NULL;
			g_ext_eeprom[eeprom_index].eeprom_read_mgr.address = ~0;
			g_ext_eeprom[eeprom_index].eeprom_read_mgr.index = 0;
			g_ext_eeprom[eeprom_index].eeprom_read_mgr.is_op_pending = false;
			g_ext_eeprom[eeprom_index].eeprom_read_mgr.is_cb_pending = true;
		}
	}

	return ret_val;
}
