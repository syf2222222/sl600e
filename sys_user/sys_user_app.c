

//#include "bsp.h"
#include  <os.h>

#include <stdio.h>
#include <string.h>

#include "stdint.h"
#include "bsp.h"

uint8_t STM32_reseting=0;
uint8_t STM32_rest_wait;


void wakeup_dev(void)
{
}
void dev_sleep(void)
{
}

#define  AppTasksysT_SIZE                     150
#define  AppTasksysT_PRIO                      9
OS_TCB   AppTasksysT_TCB;															//任务控制块
CPU_STK  AppTasksysT_Stk [ AppTasksysT_SIZE ];	//任务堆栈
void  AppTasksysT  ( void * p_arg );


void ATSTaskStart (void *p_arg)
{
	OS_ERR      err;
	#if 1	
						 
		/* 创建 任务 */
    OSTaskCreate((OS_TCB     *)&AppTasksysT_TCB,                             //任务控制块地址
                 (CPU_CHAR   *)"AppTasksysT_Name",                             //任务名称
                 (OS_TASK_PTR ) AppTasksysT,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) AppTasksysT_PRIO,                         //任务的优先级
                 (CPU_STK    *)&AppTasksysT_Stk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) AppTasksysT_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) AppTasksysT_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型
#endif	
		
}

