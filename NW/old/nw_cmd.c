
#include "bsp.h"
#include "nw.h"

void reset_STM32(void)
{
	printf("\r\n׼����λϵͳ.");
	HAL_NVIC_SystemReset();
}
void wakeup_dev(void)
{
}
void dev_sleep(void)
{
}


void check_function(void)
{
	char i;
		for(i=0;i<strlen((char *)function_buf);i++)
		{
			if(function_buf[i]==0x22)//	��������������Ǽ�⹦��
			{
			}
			if(function_buf[i]==0x24)//	��Ե��й©������⹦��
			{
			}
			if(function_buf[i]==0x25)//	�������ݼ�⹦��
			{
			}
			if(function_buf[i]==0x26)//	�����¶ȡ��������ݼ�⹦��
			{
			}
			if(function_buf[i]==0x27)//	���������ݼ�⹦��
			{
			}
			if(function_buf[i]==0x28)//	���߲���Ǽ�⹦��
			{
			}
			if(function_buf[i]==0x29)//	�趯���Ƶ�ʼ�⹦��
			{
			}
			if(function_buf[i]==0x2A)//	������б���ݼ�⹦��
			{
			}
			if(function_buf[i]==0x2B)//	����΢�������ݼ�⹦��
			{
			}
			if(function_buf[i]==0x2C)//	�ۺϷ�������
			{
			}
			if(function_buf[i]==0x2D)//	ɽ�𱨾�����
			{
			}
			if(function_buf[i]==0x2E)//	����趯��������
			{
			}
			if(function_buf[i]==0x30)//	�豸�����Լ칦��
			{
			}
			if(function_buf[i]==0x32)//	΢���񶯶�̬���ݼ�⹦��
			{
			}
			if(function_buf[i]==0x36)//	�趯��̬���ݼ�⹦��
			{
			}
			if(function_buf[i]==0x41)//	�ۻ����ݼ�⹦��
			{
			}
			if(function_buf[i]==0x42)//	���߻������ݼ�⹦��
			{
			}
			if(function_buf[i]==0x43)//	�����¶����ݼ�⹦��
			{
			}
			if(function_buf[i]==0x44)//	���»���ӵػ������ݼ�⹦��
			{
			}
			if(function_buf[i]==0x45)//	���¹��϶�λ���ݼ�⹦��
			{
			}
			if(function_buf[i]==0x46)//	���¹��Ͼַ����ݼ�⹦��
			{
			}
			if(function_buf[i]==0x47)//	���¾ַ���ͼ���ݴ��书��
			{
			}
			if(function_buf[i]==0x73)//	�ļ����书��
			{
			}
			if(function_buf[i]==0x84)//	ͼ���⹦��
			{
			}
			if(function_buf[i]==0x85)//	��Ƶ��⹦��
			{
			}
		}
}
