

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
OS_TCB   AppTasksysT_TCB;															//������ƿ�
CPU_STK  AppTasksysT_Stk [ AppTasksysT_SIZE ];	//�����ջ
void  AppTasksysT  ( void * p_arg );


void ATSTaskStart (void *p_arg)
{
	OS_ERR      err;
	#if 1	
						 
		/* ���� ���� */
    OSTaskCreate((OS_TCB     *)&AppTasksysT_TCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"AppTasksysT_Name",                             //��������
                 (OS_TASK_PTR ) AppTasksysT,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) AppTasksysT_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTasksysT_Stk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) AppTasksysT_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) AppTasksysT_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������
#endif	
		
}

