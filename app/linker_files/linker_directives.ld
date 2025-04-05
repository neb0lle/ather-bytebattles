/* __DISCLAIMER_START__                                                      */
/******************************************************************************
* (c) 2014-2017, Cypress Semiconductor Corporation
* or a subsidiary of Cypress Semiconductor Corporation. All rights
* reserved.
*
* This software, including source code, documentation and related
* materials ( "Software" ), is owned by Cypress Semiconductor
* Corporation or one of its subsidiaries ( "Cypress" ) and is protected by
* and subject to worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ( "EULA" ).
*
* If no EULA applies, Cypress hereby grants you a personal, nonexclusive,
* non-transferable license to copy, modify, and compile the
* Software source code solely for use in connection with Cypress' s
* integrated circuit products. Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO
* WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING,
* BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE. Cypress reserves the right to make
* changes to the Software without notice. Cypress does not assume any
* liability arising out of the application or use of the Software or any
* product or circuit described in the Software. Cypress does not
* authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ( "High Risk Product" ). By
* including Cypress' s product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
******************************************************************************/
/* __DISCLAIMER_END__                                                        */
/*****************************************************************************/


/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   This linker file needs to be preprocessed by GCC because LD does not support preprocessing.
   CMake build environment will take care of this!
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/



/* Check that necessary definitions are available */

#if (!defined(_LINK_flash_)) && (!defined(_LINK_sram_))
  #error "Link location not defined or not supported!"
#endif

#if (!defined(_CORE_cm0plus_)) && (!defined(_CORE_cm4_))
  #error "Target core not defined or not supported!"
#endif

#if (!defined(CY_MCU_rev_a)) && (!defined(CY_MCU_rev_b)) && (!defined(CY_MCU_rev_c)) && (!defined(CY_MCU_rev_d))
  #error "MCU revision is not defined or not supported!"
#endif


/*=======================================================================================
// Description of special symbols
//=======================================================================================
// sram_private_for_srom:
//     The SROM code needs an area for private data during system call processing. It will
//     not be accessible by user software in protection state NORMAL and beyond.
//     So it needs to be excluded from the total SRAM available to the user. The location of
//     this RAM depends on the MCU revision.
//
// sram_used_by_boot_base:
//     Cypress (ROM and Flash) boot firmware needs an area in SRAM (e.g. for stack). This may cause issues in case of
//     RAM build configurations when code/const sections are loaded there by the debugger, but may get overwritten
//     if reset is triggered by the debug script later on. So in case of RAM build configurations the linker should
//     check if the area of ROM type sections overlaps the boot area.
//
// sram_can_lin_boot_reserve:
//     Cypress CAN/LIN Bootloader is entered only if several conditions apply (refer Architecture TRM, "Flash Boot" chapter).
//     It will then use an area of SRAM for its data.
//     This may cause issues in case of RAM build configurations when code/const sections are loaded there by the debugger,
//     but may get overwritten if reset is triggered by the debug script later on. Therefore, this area shall be reserved.
//
// ecc_init_width:
//     Defines the most restrictive native ECC width (in bytes) of all "normal" memories (SRAM, DTCM, ITCM) in any
//     Traveo II derivate in order to keep the startup code dealing with ECC initialization generic
*/


/*=======================================================================================
// Following symbol and memory area definitions are device specific
//=======================================================================================*/


  /* Constants */
  #define code_flash_total_size        1088K
  #define sram_total_size              128K
  #define sram_base                    0x08000000
  #define sram_used_by_boot_base       (sram_base + sram_total_size - 6K)
  #define sram_private_for_srom        2K
  #define cm0plus_intvec_alignment     256   /* CPU specific vector table alignment requirement */
  #define cm4_intvec_alignment         1024  /* CPU specific vector table alignment requirement */
  #define ecc_init_width               8

#if defined(_LINK_sram_)
  #define sram_can_lin_boot_reserve    512
#else
  #define sram_can_lin_boot_reserve    0
#endif

#if defined(CY_MCU_rev_a) || defined(CY_MCU_rev_b) || defined(CY_MCU_rev_c)
  #define sram_user_start_offset       (0                     + sram_can_lin_boot_reserve)
  #define sram_user_end_offset         (sram_total_size       - sram_private_for_srom)
#else
  #define sram_user_start_offset       (sram_private_for_srom + sram_can_lin_boot_reserve)
  #define sram_user_end_offset         (sram_total_size       - 0)
#endif

  /*===================================================================================
  //                              USER CONFIGURATION
  //===================================================================================*/

  /* Specify how much SRAM and CODE_FLASH shall be used by CM0+ application,
   * the rest of the memory will be automatically assigned to CM4 application */
  #define cm0plus_sram_reserve         (64K - sram_user_start_offset)
  #define cm0plus_code_flash_reserve   512K

  /*-----------------------------------------------------------------------------------*/

  /* Specify stack and heap sizes for CM0+ and CM4 application independently */
  #define cm0plus_heap_reserve         512
  #define cm0plus_stack_reserve        1K
  #define cm4_heap_reserve             512
  #define cm4_stack_reserve            1K

  /*===================================================================================*/


  /* Calculation of SRAM distribution among all cores */
  #define cm0plus_sram_offset          sram_user_start_offset
  #define cm4_sram_offset              (cm0plus_sram_offset  + cm0plus_sram_reserve)
  #define cm4_sram_reserve             (sram_user_end_offset - cm4_sram_offset)

  /* Calculation of CODE_FLASH distribution among all cores */
  #define cm0plus_code_flash_offset    0
  #define cm4_code_flash_offset        (cm0plus_code_flash_offset + cm0plus_code_flash_reserve)
  #define cm4_code_flash_reserve       (code_flash_total_size     - cm4_code_flash_offset)


  /* Map core dependent definition to generic name for memory and section definition */
#if defined(_CORE_cm0plus_)
  #define heap_reserve                 cm0plus_heap_reserve
  #define stack_reserve                (cm0plus_stack_reserve + (ecc_init_width - 1)) & (~((ecc_init_width - 1))) /* Ensure that stack size is an integer multiple of ECC init width (round up) */
  #define sram_offset                  cm0plus_sram_offset
  #define sram_reserve                 cm0plus_sram_reserve
  #define code_flash_offset            cm0plus_code_flash_offset
  #define code_flash_reserve           cm0plus_code_flash_reserve
  #define intvec_alignment             cm0plus_intvec_alignment

#else /* CM4 */
  #define heap_reserve                 cm4_heap_reserve
  #define stack_reserve                (cm4_stack_reserve + (ecc_init_width - 1)) & (~((ecc_init_width - 1))) /* Ensure that stack size is an integer multiple of ECC init width (round up) */
  #define sram_offset                  cm4_sram_offset
  #define sram_reserve                 cm4_sram_reserve
  #define code_flash_offset            cm4_code_flash_offset
  #define code_flash_reserve           cm4_code_flash_reserve
  #define intvec_alignment             cm4_intvec_alignment

#endif



MEMORY
{
    SRAM         (rwx)   : ORIGIN = sram_base  + sram_offset,       LENGTH = sram_reserve
    CODE_FLASH   (rx)    : ORIGIN = 0x10000000 + code_flash_offset, LENGTH = code_flash_reserve
    WORK_FLASH           : ORIGIN = 0x14000000,                     LENGTH = 0x00018000         /*   96 KB */
    SFLASH               : ORIGIN = 0x17000000,                     LENGTH = 0x00008000         /*   32 KB */
    SFLASH_ALT           : ORIGIN = 0x17800000,                     LENGTH = 0x00008000         /*   32 KB */
}



/*=======================================================================================
//
// Program layout
*/
SECTIONS
{

    /*============================================================================================================
    // ROM type sections
    //------------------------------------------------------------------------------------------------------------*/

  #if   defined(_LINK_flash_)
    #define MEMORY_FOR_ROM_TYPE_SECTIONS     CODE_FLASH
  #elif defined(_LINK_sram_)
    #define MEMORY_FOR_ROM_TYPE_SECTIONS     SRAM
  #endif

    /* Note: .intvec must be the first section in ROM in order for __cmx_vector_base_linker_symbol to be correctly calculated! */
    .intvec : ALIGN(intvec_alignment)
    {
        *(.intvec)
    } >MEMORY_FOR_ROM_TYPE_SECTIONS

    .text : ALIGN(4)
    {
        *(.text*)        
    } >MEMORY_FOR_ROM_TYPE_SECTIONS

    .rodata : ALIGN(4)
    {
        *(.rodata*)
    } >MEMORY_FOR_ROM_TYPE_SECTIONS


    /* Special GCC ARM sections */
    .ARM.extab : ALIGN(4)
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > MEMORY_FOR_ROM_TYPE_SECTIONS

    .ARM.exidx : ALIGN(4)
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > MEMORY_FOR_ROM_TYPE_SECTIONS



    /*============================================================================================================
    // RAM type sections
    //------------------------------------------------------------------------------------------------------------*/

    /* >>> SRAM <<< */


    /* In case of _LINK_sram_ the linker (LD) would set the LMA of '.data' to its VMA (because both end up in the same memory region).
     * Therefore, the '.data' section needs to be the first one of the "RAM type" sections so that the '__rom_type_sections_end_address' 
     * can be correctly calculated and startup code will not ECC initialize the RAM area occupied by the .data section (since its content
     * would have been loaded by the debugger already). In case of _LINK_flash the '.data' section can be arbitrarily placed in RAM. */


    .data : ALIGN_WITH_INPUT
    {
        . = ALIGN(ecc_init_width);
        *(.data*)
        . = ALIGN(ecc_init_width);
    } >SRAM AT>MEMORY_FOR_ROM_TYPE_SECTIONS

    .bss (NOLOAD) : ALIGN(ecc_init_width)
    {
        *(.bss*)
        *(COMMON)
        . = ALIGN(ecc_init_width);
    } >SRAM

    .heap (NOLOAD) : ALIGN(8)
    {
        __heap_start = .;
        . += ALIGN(heap_reserve, 8);
        __heap_end = .;
    } >SRAM

    .stack (NOLOAD) : ALIGN(8)
    {
        __stack_start = .;
        . += ALIGN(stack_reserve, 8);
        __stack_end = .;
    } >SRAM

    .intvec_ram (NOLOAD) : ALIGN(intvec_alignment)
    {
        *(.intvec_ram)
    } >SRAM


    /* Linker symbols for startup code */
    __data_lma   = LOADADDR(.data);
    __data_start = ADDR(.data);
    __data_end   = ADDR(.data) + SIZEOF(.data);
    __bss_start  = ADDR(.bss);
    __bss_end    = ADDR(.bss) + SIZEOF(.bss);


  #if defined(_LINK_sram_)
    /* Cypress (ROM and Flash) boot firmware needs an area in SRAM (e.g. for stack). This may cause issues in case of
     * RAM build configurations when code/const sections are loaded there by the debugger, but may get overwritten
     * if reset is triggered by the debug script later on. So in case of RAM build configurations the following
     * statement checks if the area of ROM type sections overlaps the boot area.
     * Comment the statement if the sections shall be linked there anyway.
     *
     * A simple check is sufficient because the boot area is from SRAM_MAX-6KB to SRAM_MAX-2KB and it is unlikely that the ROM sections would only
     * start in the last 2KB (which are anyway not accessible in early MCU revisions because of private SROM RAM area)
     */

    __rom_type_sections_end_address = __data_end;
    ASSERT(__rom_type_sections_end_address < sram_used_by_boot_base, "ROM type sections are overlapping the area used by Cypress boot firmware. Check comment in linker file!")
  #endif


    /*============================================================================================================
    // The start of CM4 vector table is required by CM0+ application to correctly
    // set CPUSS->CM4_VECTOR_TABLE_BASE register before releasing CM4 from reset
    // Following statements also ensure that the created symbol has the correct alignment
    //------------------------------------------------------------------------------------------------------------*/
  #ifdef _CORE_cm0plus_
      #if   defined(_LINK_flash_)
        __cm4_rom_start = ORIGIN(CODE_FLASH) - cm0plus_code_flash_offset + cm4_code_flash_offset;
      #elif defined(_LINK_sram_)
        __cm4_rom_start = ORIGIN(SRAM)       - cm0plus_sram_offset       + cm4_sram_offset;
      #endif
    __cm4_vector_base_linker_symbol = (__cm4_rom_start + cm4_intvec_alignment - 1) & ~(cm4_intvec_alignment - 1);
  #endif

    /*============================================================================================================
    // These symbols will be used by startup.c / startup_customize.h for RAM ECC initialisation
    //------------------------------------------------------------------------------------------------------------*/
  #if defined(_LINK_sram_)
    /* If "ROM type" sections are linked to SRAM they shall not be cleared by the startup code */
    __ecc_init_sram_start_address = __rom_type_sections_end_address; /* The __rom_type_sections_end_address symbol is a marker for the start of "RAM type" sections in SRAM */
  #else
    __ecc_init_sram_start_address = ORIGIN(SRAM);
  #endif
    __ecc_init_sram_end_address   = ORIGIN(SRAM) + LENGTH(SRAM) - 1;


    /*============================================================================================================
    // These special symbols mark the bounds of RAM and ROM memory.
    // They are used by the GHS MULTI debugger.
    //------------------------------------------------------------------------------------------------------------*/
    /*
    __ghs_romstart = ORIGIN(CODE_FLASH);
    __ghs_romend   = ORIGIN(SFLASH_ALT) + LENGTH(SFLASH_ALT);
    __ghs_ramstart = ORIGIN(SRAM);
    __ghs_ramend   = ORIGIN(SRAM) + LENGTH(SRAM);
    */

}
