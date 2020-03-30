//#include "bsp.h"
#include  <os.h>

#include <stdio.h>
#include <string.h>
#include "stdint.h"


#define  AppTaskADC_SIZE                     512
#define  AppTaskADC_PRIO                         9
OS_TCB   AppTaskADC_TCB;															//任务控制块
CPU_STK  AppTaskADC_Stk [ AppTaskADC_SIZE ];	//任务堆栈
void  AppTaskADC  ( void * p_arg );

#define  AppTaskTS_SIZE                     512
#define  AppTaskTS_PRIO                         9
OS_TCB   AppTaskTS_TCB;															//任务控制块
CPU_STK  AppTaskTS_Stk [ AppTaskTS_SIZE ];	//任务堆栈
void  AppTaskTS  ( void * p_arg );

	
void SenSTaskStart (void *p_arg)
{
	OS_ERR      err;

#if 1	
						 
		/* 创建 任务 */
    OSTaskCreate((OS_TCB     *)&AppTaskTS_TCB,                             //任务控制块地址
                 (CPU_CHAR   *)"AppTaskTS_Name",                             //任务名称
                 (OS_TASK_PTR ) AppTaskTS,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     )	AppTaskTS_PRIO,                         //任务的优先级
                 (CPU_STK    *)&AppTaskTS_Stk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) AppTaskTS_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) AppTaskTS_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型
#endif


}