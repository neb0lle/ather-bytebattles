/*
    @file
    asdk_flash.h

    @path
    asdk-gen2/platform/cyt2b75/inc/asdk_flash.h

    @Created on
    Sep 5, 2023

    @Author
    gautam.sagar

    @Copyright
    Copyright (c) Ather Energy Pvt Ltd. All rights reserved.

    @brief
    This file prototypes the Flash Module for Ather SDK(asdk).
*/

#ifndef ASDK_FLASH_H
#define ASDK_FLASH_H

/*==============================================================================

                               INCLUDE FILES

==============================================================================*/

#include <stdint.h>
#include <stdbool.h>
#include "asdk_error.h"
#include "asdk_platform.h"

/*==============================================================================

                      DEFINITIONS AND TYPES : MACROS

==============================================================================*/

/*==============================================================================

                      DEFINITIONS AND TYPES : ENUMS

==============================================================================*/
/*!
 * @brief Flash erase type
 */
typedef enum
{
    ASDK_FLASH_ERASETYPE_ENTIRE_FLASH = 0,
    ASDK_FLASH_ERASETYPE_SECTOR_WISE,
    ASDK_FLASH_ERASETYPE_MAX,
    ASDK_FLASH_ERASETYPE_INVALID = ASDK_FLASH_ERASETYPE_MAX,

} asdk_flash_erase_type_t;

/*!
 * @brief Which bank mode the flash should be used Single/Dual bank mode
 */
typedef enum
{
    ASDK_FLASH_BANKTYPE_SINGLE_BANK = 0,
    ASDK_FLASH_BANKTYPE_DUAL_BANK,
    ASDK_FLASH_BANKTYPE_MAX,
    ASDK_FLASH_BANKTYPE_INVALID = ASDK_FLASH_BANKTYPE_MAX,

} asdk_flash_bank_type_t;

/*!
 * @brief Tells if in dual bank mode which Bank to erase.
 */
typedef enum
{
    ASDK_FLASH_BANK_A = 0,
    ASDK_FLASH_BANK_B,
    ASDK_FLASH_BANK_MAX,
    ASDK_FLASH_BANK_INVALID = ASDK_FLASH_BANK_MAX,

} asdk_flash_dual_bank_t;

/*!
 * @brief Which flash should be initialized code flash/work flash/both
 */
typedef enum
{
    ASDK_FLASH_INIT_FLASHTYPE_BOTH_CODE_FLASH_DATA_FLASH = 0,
    ASDK_FLASH_INIT_FLASHTYPE_CODE_FLASH,
    ASDK_FLASH_INIT_FLASHTYPE_DATA_FLASH,
    ASDK_FLASH_INIT_FLASHTYPE_MAX,
    ASDK_FLASH_INIT_FLASHTYPE_INVALID = ASDK_FLASH_INIT_FLASHTYPE_MAX,

} asdk_flash_type_t;

/*!
 * @brief Flash operation type
 *
 * @note:The DMA support has not been added in the current version
 */
typedef enum
{
    ASDK_FLASH_OPERATION_NON_BLOCKING_MODE = 0,
    ASDK_FLASH_OPERATION_BLOCKING_MODE,
    ASDK_FLASH_OPERATION_MAX,
    ASDK_FLASH_OPERATION_INVALID = ASDK_FLASH_OPERATION_MAX,
} asdk_flash_operation_mode_t;

/*!
 * @brief An data structure to represent watchdog interrupt.
 */
typedef struct
{
    bool enable;      /*!< Enable interrupt */
    uint8_t priority; /*!< Interrupt priority */
    asdk_exti_interrupt_num_t intr_num;  /*Interrupt number of the particular external interrupt*/
} asdk_flash_interrupt_t;
/*==============================================================================

                    LOCAL DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/
/*!
 * @brief The initialization configuration structure to be passed during intialization
 *
 * @note:For cyt expects operation mode in the init otherwise it would be ignored
 */
typedef struct
{
    asdk_flash_type_t flash_type;                     /*Initialize the code flash/data flash/both*/
    asdk_flash_operation_mode_t flash_operation_mode; /*Initialize the flash in the polling mode or interrupt mode*/
    asdk_flash_interrupt_t flash_interrupt_config;
} asdk_flash_config_t;

/*!
 * @brief This configuration structure implements the flash read/write config structure
 */
typedef struct
{
    uint32_t size_in_bytes;                           /* Size of variable to be read or write*/
    uint32_t source_addr;                             /*Source address of the varaible to be read/write*/
    uint32_t destination_addr;                        /*Destination address for the varaible to be read / write*/
} asdk_flash_operation_config_t;


/*==============================================================================

                           EXTERNAL DECLARATIONS

==============================================================================*/

/*==============================================================================

                           FUNCTION PROTOTYPES

==============================================================================*/
typedef void (*asdk_flash_callback_fun_t)(void);

/* ************************************************************************** *
 *                          Configuration APIs                                *
 * ************************************************************************** */
/*----------------------------------------------------------------------------*/
/* Function : asdk_flash_init */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function initializes the Flash Module based on the config structure passed as parameter.

  @param [in] flash_config ASDK Flash configuration parameter.

  @return
    - @ref ASDK_FLASH_STATUS_SUCCESS
    - @ref ASDK_FLASH_STATUS_ERROR
    - @ref ASDK_FLASH_STATUS_BUSY
    - @ref ASDK_FLASH_STATUS_TIMEOUT
    - @ref ASDK_FLASH_ERROR_NULL_PTR
    - @ref ASDK_FLASH_ERROR_INIT_FAIL
    - @ref ASDK_FLASH_ERROR_DEINIT_FAIL
    - @ref ASDK_FLASH_ERROR_NOT_IMPLEMENTED
    - @ref ASDK_FLASH_ERROR_NOT_INITIALIZED
    - @ref ASDK_FLASH_ERROR_INVALID_HANDLER
    - @ref ASDK_FLASH_ERROR_INVALID_DATA_SIZE
    - @ref ASDK_FLASH_ERROR_INVALID_SECTOR_SIZE
    - @ref ASDK_FLASH_ERROR_INVALID_FLASH_ADDRESS
    - @ref ASDK_FLASH_ERROR_INVALID_DATA_ALIGNMENT
*/
asdk_errorcode_t asdk_flash_init(asdk_flash_config_t *flash_config);

/*----------------------------------------------------------------------------*/
/* Function : asdk_flash_deinit */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function de-initializes the Flash Module based on the config structure passed as parameter.

  @param [in] flash_config ASDK Flash configuration structure

  @return
    - @ref ASDK_FLASH_STATUS_SUCCESS
    - @ref ASDK_FLASH_ERROR_INVALID_FLASH_TYPE
*/
asdk_errorcode_t asdk_flash_deinit(asdk_flash_config_t *flash_config);


/*----------------------------------------------------------------------------*/
/* Function : asdk_flash_install_callback */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function installs/registers the callback function for the user callback.

  @param [in] callback_fun Callback function to the user.

  @return
    - @ref ASDK_FLASH_STATUS_SUCCESS
*/
asdk_errorcode_t asdk_flash_install_callback(asdk_flash_callback_fun_t callback_fun);

/*----------------------------------------------------------------------------*/
/* Function : asdk_flash_read_blocking */
/*----------------------------------------------------------------------------*/
/*!
  @brief
This function enables the user to read the flash memory based on the address passed in the config structure

  @param [in] flash_read_config Flash input config structure for read operation.
  @param [in] timeout_ms  Timeout value for the blocking APIs in ms

  @return
    - @ref ASDK_FLASH_STATUS_SUCCESS
    - @ref ASDK_FLASH_ERROR_INVALID_FLASH_ADDRESS
*/
asdk_errorcode_t asdk_flash_read_blocking(asdk_flash_operation_config_t *flash_read_config, uint32_t timeout_ms);

/*----------------------------------------------------------------------------*/
/* Function : asdk_flash_read_non_blocking */
/*----------------------------------------------------------------------------*/
/*!
  @brief
This function enables the user to read the flash memory based on the address passed in the config structure

  @param [in] flash_read_config Flash input config structure for read operation.

  @return
    - @ref ASDK_FLASH_STATUS_SUCCESS
    - @ref ASDK_FLASH_ERROR_INVALID_FLASH_ADDRESS
*/
asdk_errorcode_t asdk_flash_read_non_blocking(asdk_flash_operation_config_t *flash_read_config);

/*----------------------------------------------------------------------------*/
/* Function : asdk_flash_write_blocking */
/*----------------------------------------------------------------------------*/
/*!
  @brief
   This function enables the user to write to the flash memory based on the address passed in the config structure

  @param [in] flash_write_config- Flash input config structure for write operation.
  @param [in] timeout_ms  Timeout value for the blocking APIs in ms

  @return
    - @ref ASDK_FLASH_STATUS_SUCCESS
    - @ref ASDK_FLASH_ERROR_NULL_PTR
    - @ref ASDK_FLASH_ERROR_INVALID_FLASH_ADDRESS
*/
asdk_errorcode_t asdk_flash_write_blocking(asdk_flash_operation_config_t *flash_write_config, uint32_t timeout_ms);

/*----------------------------------------------------------------------------*/
/* Function : asdk_flash_write_non_blocking */
/*----------------------------------------------------------------------------*/
/*!
  @brief
   This function enables the user to write to the flash memory based on the address passed in the config structure

  @param [in] flash_write_config- Flash input config structure for write operation.
  @param [in] timeout_ms  Timeout value for the blocking APIs in ms

  @return
    - @ref ASDK_FLASH_STATUS_SUCCESS
    - @ref ASDK_FLASH_ERROR_NULL_PTR
    - @ref ASDK_FLASH_ERROR_INVALID_FLASH_ADDRESS
*/
asdk_errorcode_t asdk_flash_write_non_blocking(asdk_flash_operation_config_t *flash_write_config);

/*----------------------------------------------------------------------------*/
/* Function : asdk_flash_erase_all_sectors_blocking*/
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function erases the whole flash memory except the protected blocks.

  @param [in] Base_Sector_Address Base address of the sector to be erased.
  @param [in] timeout_ms  Timeout value for the blocking APIs in ms

  @return
    - @ref ASDK_FLASH_STATUS_SUCCESS
    - @ref ASDK_FLASH_ERROR_NULL_PTR
    - @ref ASDK_FLASH_ERROR_INVALID_FLASH_OPERATION_MODE
    - @ref ASDK_FLASH_STATUS_ERROR
*/
asdk_errorcode_t asdk_flash_erase_all_sectors_blocking(uint32_t Base_Sector_Address, uint32_t timeout_ms);
/*----------------------------------------------------------------------------*/
/* Function : asdk_flash_erase_all_sectors_non_blocking */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function erases the whole flash memory except the protected blocks.

  @param [in] Base_Sector_Address Base address of the sector to be erased.

  @return
    - @ref ASDK_FLASH_STATUS_SUCCESS
    - @ref ASDK_FLASH_ERROR_NULL_PTR
    - @ref ASDK_FLASH_ERROR_INVALID_FLASH_OPERATION_MODE
    - @ref ASDK_FLASH_STATUS_ERROR
*/
asdk_errorcode_t asdk_flash_erase_all_sectors_non_blocking(uint32_t Base_Sector_Address);

/*----------------------------------------------------------------------------*/
/* Function : asdk_flash_erase_sector_blocking */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function erases the flash memory sectorwise in blocking mode.

  @param [in] Base_Sector_Address Base address of the sector to be erased.
  @param [in] timeout_ms  Timeout value for the blocking APIs in ms
  

  @return
    - @ref ASDK_FLASH_STATUS_SUCCESS
    - @ref ASDK_FLASH_ERROR_INVALID_FLASH_ADDRESS
    - @ref ASDK_FLASH_ERROR_INVALID_FLASH_OPERATION_MODE
    - @ref ASDK_FLASH_ERROR_NULL_PTR
    - @ref ASDK_FLASH_STATUS_ERROR
*/
asdk_errorcode_t asdk_flash_erase_sector_blocking(uint32_t Base_Sector_Address, uint32_t timeout_ms);
/*----------------------------------------------------------------------------*/
/* Function : asdk_flash_erase_sector_non_blocking */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function erases the flash memory sectorwise based on the config structure passed.

  @param [in] Base_Sector_Address Base address of the sector to be erased.

  @return
    - @ref ASDK_FLASH_STATUS_SUCCESS
    - @ref ASDK_FLASH_ERROR_INVALID_FLASH_ADDRESS
    - @ref ASDK_FLASH_ERROR_INVALID_FLASH_OPERATION_MODE
    - @ref ASDK_FLASH_ERROR_NULL_PTR
    - @ref ASDK_FLASH_STATUS_ERROR
*/
asdk_errorcode_t asdk_flash_erase_sector_non_blocking(uint32_t Base_Sector_Address);
/*----------------------------------------------------------------------------*/
/* Function : asdk_flash_get_sector_size */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function erases the flash memory sectorwise based on the config structure passed.

  @param [in] address Address of which the user wants to know the sector size
  @param [out] flash_sector_size Variable in which the sector size would be given to the user.

  @return
    - @ref ASDK_FLASH_STATUS_SUCCESS
    - @ref ASDK_FLASH_ERROR_INVALID_FLASH_ADDRESS
*/
asdk_errorcode_t asdk_flash_get_sector_size(uint32_t address, uint32_t *flash_sector_size);
#endif /* ASDK_FLASH_H */