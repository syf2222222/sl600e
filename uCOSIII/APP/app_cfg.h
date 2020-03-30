/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                           (c) Copyright 2009-2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                      APPLICATION CONFIGURATION
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                     Micrium uC-Eval-STM32F107
*                                         Evaluation Board
*
* Filename      : app_cfg.h
* Version       : V1.00
* Programmer(s) : JJL
*                 EHS
*                 DC
*********************************************************************************************************
*/

#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__


//任务开始优先级
#define  APP_CFG_TASK_START_PRIO                           2u
#define  APP_HIGH_PRIORITY                                (APP_CFG_TASK_START_PRIO+1)

//任务堆栈大小
#define  APP_CFG_TASK_START_STK_SIZE                     128u
#define  APP_CFG_TASK_BLINKY_STK_SIZE                    128u
#define     TASK_STK_SIZE                                256u

//任务堆栈限度
#define  APP_CFG_TASK_START_STK_SIZE_PCT_FULL             90u
#define  APP_CFG_TASK_START_STK_SIZE_LIMIT       (APP_CFG_TASK_START_STK_SIZE     * (100u - APP_CFG_TASK_START_STK_SIZE_PCT_FULL))    / 100u

#define  APP_CFG_TASK_BLINKY_STK_SIZE_LIMIT      (APP_CFG_TASK_BLINKY_STK_SIZE    * (100u - APP_CFG_TASK_START_STK_SIZE_PCT_FULL))    / 100u

////追踪和调试选项
//#ifndef  TRACE_LEVEL_OFF
//#define  TRACE_LEVEL_OFF                0
//#endif

//#ifndef  TRACE_LEVEL_INFO
//#define  TRACE_LEVEL_INFO               1
//#endif

//#ifndef  TRACE_LEVEL_DBG
//#define  TRACE_LEVEL_DBG                2
//#endif

//#define  APP_CFG_TRACE_LEVEL             TRACE_LEVEL_OFF
//#define  APP_CFG_TRACE                   printf

//#define  BSP_CFG_TRACE_LEVEL             TRACE_LEVEL_OFF
//#define  BSP_CFG_TRACE                   printf

//#define  APP_TRACE_INFO(x)               ((APP_CFG_TRACE_LEVEL >= TRACE_LEVEL_INFO)  ? (void)(APP_CFG_TRACE x) : (void)0)
//#define  APP_TRACE_DBG(x)                ((APP_CFG_TRACE_LEVEL >= TRACE_LEVEL_DBG)   ? (void)(APP_CFG_TRACE x) : (void)0)

//#define  BSP_TRACE_INFO(x)               ((BSP_CFG_TRACE_LEVEL  >= TRACE_LEVEL_INFO) ? (void)(BSP_CFG_TRACE x) : (void)0)
//#define  BSP_TRACE_DBG(x)                ((BSP_CFG_TRACE_LEVEL  >= TRACE_LEVEL_DBG)  ? (void)(BSP_CFG_TRACE x) : (void)0)
	
//#define   WORD    unsigned short
//#define   BYTE    unsigned char
//#define   DWORD   unsigned long
//#define   UINT    unsigned int
//#define   bool    unsigned char
//	#define   false 0
//#define   true  1



/*
*********************************************************************************************************
*                                       MODULE ENABLE / DISABLE
*********************************************************************************************************
*/

#define  APP_CFG_SERIAL_EN                          DEF_DISABLED          // Modified by fire （原是 DEF_ENABLED）

/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/

#define  APP_TASK_START_PRIO                        2            //任务优先级

//#define  APP_TASK_LED1_PRIO                         3
//#define  APP_TASK_LED2_PRIO                         3
//#define  APP_TASK_LED3_PRIO                         3


/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/

#define  APP_TASK_START_STK_SIZE                    128          //任务堆栈空间（单位：sizeof(CPU_STK)）

//#define  APP_TASK_LED1_STK_SIZE                     512   
//#define  APP_TASK_LED2_STK_SIZE                     512
//#define  APP_TASK_LED3_STK_SIZE                     512


/*
*********************************************************************************************************
*                                    BSP CONFIGURATION: RS-232
*********************************************************************************************************
*/

#define  BSP_CFG_SER_COMM_SEL             			BSP_SER_COMM_UART_02
#define  BSP_CFG_TS_TMR_SEL                         2


/*
*********************************************************************************************************
*                                     TRACE / DEBUG CONFIGURATION
*********************************************************************************************************
*/
#if 0
#define  TRACE_LEVEL_OFF                            0
#define  TRACE_LEVEL_INFO                           1
#define  TRACE_LEVEL_DEBUG                          2
#endif

#define  APP_TRACE_LEVEL                            TRACE_LEVEL_INFO
#define  APP_TRACE                                  BSP_Ser_Printf

#define  APP_TRACE_INFO(x)            ((APP_TRACE_LEVEL >= TRACE_LEVEL_INFO)  ? (void)(APP_TRACE x) : (void)0)
#define  APP_TRACE_DEBUG(x)           ((APP_TRACE_LEVEL >= TRACE_LEVEL_DEBUG) ? (void)(APP_TRACE x) : (void)0)


#endif




