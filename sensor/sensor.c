//#include "bsp.h"
#include  <os.h>

#include <stdio.h>
#include <string.h>
#include "stdint.h"


#define  AppTaskADC_SIZE                     512
#define  AppTaskADC_PRIO                         9
OS_TCB   AppTaskADC_TCB;															//������ƿ�
CPU_STK  AppTaskADC_Stk [ AppTaskADC_SIZE ];	//�����ջ
void  AppTaskADC  ( void * p_arg );

#define  AppTaskTS_SIZE                     512
#define  AppTaskTS_PRIO                         9
OS_TCB   AppTaskTS_TCB;															//������ƿ�
CPU_STK  AppTaskTS_Stk [ AppTaskTS_SIZE ];	//�����ջ
void  AppTaskTS  ( void * p_arg );

	
void SenSTaskStart (void *p_arg)
{
	OS_ERR      err;

#if 1	
						 
		/* ���� ���� */
    OSTaskCreate((OS_TCB     *)&AppTaskTS_TCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"AppTaskTS_Name",                             //��������
                 (OS_TASK_PTR ) AppTaskTS,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     )	AppTaskTS_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskTS_Stk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) AppTaskTS_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) AppTaskTS_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������
#endif


}