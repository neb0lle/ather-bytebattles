/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2014; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                         BOARD SUPPORT PACKAGE
*
*                                             Delfino 28335
*                                                on the
*                                             TMDSDOCK28335
*                                            Experimenter Kit
*
* Filename      : bsp.c
* Version       : V1.00
* Programmer(s) : JFT
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define    BSP_MODULE
#include  <bsp.h>
#include  <bsp_os.h>

#include  <os.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  BSP_Int_Init(void);

/*
*********************************************************************************************************
*                                             REGISTERS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            REGISTER BITS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/

#if ((CPU_CFG_TS_TMR_EN          != DEF_ENABLED) && \
     (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED) && \
     (OS_PROBE_HOOKS_EN          == 1))
#error  "CPU_CFG_TS_EN                  illegally #define'd in 'cpu.h'"
#error  "                              [MUST be  DEF_ENABLED] when    "
#error  "                               using uC/Probe COM modules    "
#endif


/*
*********************************************************************************************************
*                                               BSP_Init()
*
* Description : Initialize the Board Support Package (BSP).
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) This function SHOULD be called before any other BSP function is called.
*********************************************************************************************************
*/

void  BSP_Init (void)
{
                                                                /* Allow access to protected registers and regions.     */
    asm(" EALLOW");
                                                                /* Disable interrupts.                                  */
    CPU_IntDis();
                                                                /* Debugger can access registers and memory.            */
    asm(" CLRC DBGM");
                                                                /* Configure interrupts.                                */
    BSP_Int_Init();

    CPU_IntEn();
}


/*
*********************************************************************************************************
*                                            BSP_Tick_Init()
*
* Description : Initialize all the peripherals that required OS Tick services (OS initialized)
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void BSP_Tick_Init (void)
{
#if (!defined(OS_CFG_DYN_TICK_EN) || (OS_CFG_DYN_TICK_EN != DEF_ENABLED))
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
#endif


#if (defined(OS_CFG_DYN_TICK_EN) && (OS_CFG_DYN_TICK_EN == DEF_ENABLED))
    BSP_OS_Dyn_TickInit();                                      /* Init uC/OS dynamic time source.                      */
#else
    cpu_clk_freq = BSP_CPU_ClkFreq();                           /* Determine TIMER2 reference freq.                     */

#if (OS_VERSION >= 30000u)
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        /* Determine nbr TIMER2 increments.                     */
#else
    cnts = cpu_clk_freq / (CPU_INT32U)OS_TICKS_PER_SEC;         /* Determine nbr TIMER2 increments.                     */
#endif
    BSP_OS_TIMER2_TickInit(cnts);                               /* Init uC/OS periodic time src (TIMER2).               */
#endif
}


/*
*********************************************************************************************************
*                                            BSP_CPU_ClkFreq()
*
* Description : Read CPU registers to determine the CPU clock frequency of the chip.
*
* Argument(s) : none.
*
* Return(s)   : The CPU clock frequency, in Hz.
*
* Caller(s)   : Application, BSP_Tick_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT32U  BSP_CPU_ClkFreq (void)
{
  return ((CPU_INT32U)120000000UL);                             /* The board runs the CPU at 150 MHz.                   */
}


/*
*********************************************************************************************************
*                                             BSP_Int_Init()
*
* Description : Initializes the default vector table and moves it to PIE Vector RAM.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_Int_Init (void)
{
    CPU_DATA  intid;

                                                                /* Enable PIE. */
    BSP_PIE_CTRL |= BSP_PIE_CTRL_ENPIE;
                                                                /* Build vector table in PIE RAM.                       */
                                                                /*   Set Dummy Handler for all interrupts.              */
    //Disabling the default Int handler setting for all CPU interrupts.
    // for (intid = 1; intid < BSP_INT_SRC_NBR; ++intid) {
    //     BSP_IntVectSet(intid, BSP_IntHandlerDummy);
    // }
                                                                /*   Set TIMER2 (Tick) Interrupt handler.               */
    BSP_IntVectSet(BSP_INT_ID_INT14  , OS_CPU_IntHandler);
                                                                /*   Set RTOSINT (Context Switch) Interrupt handler.    */
    BSP_IntVectSet(BSP_INT_ID_RTOSINT, OS_CPU_RTOSINT_Handler);
}
