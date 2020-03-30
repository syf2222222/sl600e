
#include "bsp.h"
#include "global.h"

unsigned char power_flag_DTU,power_flag_485,power_flag_12V,power_flag_cam1,power_flag_ETH,dm9k_init_flag,power_flag_sys,power_on_delay_485;
uint16_t power_delay_hk;

//����͹��ĵķ�����PQ7600 �˳�������ģʽ�������ڣ�����������ڣ�˫
//���ڣ�������AT+CTEST=0��Ȼ������dtr���������͹��ģ�����ƻ�ֹͣ
//��˸��ע��Ҫ�ε�USB��
//? �˳��͹��ĵķ���������dtr Ȼ��ͨ�����ڷ������AT+CTEST=1�������
//�ָ�������

void power_set_DTU(uint8_t flag)
{	
	OS_ERR      err;
	if(flag>2)
	{
		printf("\r\nDTU��Դ���ò����쳣");
	}
	if(flag==0)	//�ر�
	{
		PWR_W_DTU(0);
	}
	if(flag==1)	//��
	{
		if(power_flag_DTU==2)	//�˳��͹���
		{
			DTU_W_DTR(1);//����DTR
			OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err ); 
			HAL_UART_Transmit_IT(&huart5, (unsigned char *)"AT+CTEST=1\r\n",strlen("AT+CTEST=1\r\n"));
		}
		else		//��DTU
		{
			PWR_W_DTU(1);
			DTU_W_DTR(1);	
		}
	}
	if(flag==2)	//����͹���
	{
		if(power_flag_DTU==0)	PWR_W_DTU(1);//ԭ��Ϊ�ر�
		HAL_UART_Transmit_IT(&huart5, (unsigned char *)"AT+CTEST=0\r\n",strlen("AT+CTEST=0\r\n"));
		DTU_W_DTR(0);//����DTR
//		OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err ); 
		HAL_UART_Transmit_IT(&huart5, (unsigned char *)"AT+CTEST=0\r\n",strlen("AT+CTEST=0\r\n"));
		DTU_W_DTR(0);//����DTR
	}
	
	power_flag_DTU=flag;
}

void DTU_reset(void)
{
	OS_ERR      err;
	
	power_set_DTU(0);
	OSTimeDly ( 3000, OS_OPT_TIME_DLY, & err ); 
	power_set_DTU(1);
}

void power_set_12V(uint8_t flag)
{
	power_flag_12V=flag;
	PWR_W_12V(power_flag_12V);
}

void power_set_485(uint8_t flag) //485��Ҫ 12V����
{
	if(flag)
	{
		if((power_flag_485==0)||(power_flag_12V==0))
		{
			power_set_12V(1);
			power_on_delay_485=80;
		}
	}
	
	PWR_W_485(flag);
	PWR_W_12V3(flag);
	power_flag_485=flag;

}


//void mode_4G_init(void)
//{
////	RUN_LED1_ON();
//	PWR_W_DTU(1);
////	Wake_up_4G();
//}
void dm9k_init(unsigned char *MAC_ADD);
extern unsigned char	mac_addr[6];

void power_set_ETH(uint8_t flag) //ETH����cam1
{
	power_flag_ETH=flag;
	PWR_W_ETH(power_flag_ETH);
}
extern uint8_t http_linkd;

void close_hk(void);
void open_hk(void);
u16 cam_on_time_detect;

void power_set_cam1(uint8_t flag)	//cam1 ����ͬʱ����ETH
{	
	if(power_flag_cam1 != flag)
	{
		power_flag_cam1=flag;
		power_set_ETH(power_flag_cam1);
		PWR_W_CAM1(power_flag_cam1);
		
		if(power_flag_cam1 == 0)	//��
		{
			printf("\r\ncamera power off");
			close_hk();
			http_linkd=0;
			dm9k_init_flag=0;
		}
		else
		{
			printf("\r\ncamera power on");
			dm9k_init(mac_addr);  //����������ʱ�������л�
			power_delay_hk=hk_power_on_time;
			open_hk();
			cam_on_time_detect=0;
		}
		
	}
	else //�ظ�����һ��
	{
		power_set_ETH(power_flag_cam1);
		PWR_W_CAM1(power_flag_cam1);	
	}

//	if((flag)&&(power_flag_cam1==0))
//	{
//		power_flag_cam1=flag;	
//		printf("\r\ncamera power on");
//		power_set_ETH(flag);//�ȿ�
//		PWR_W_CAM1(flag);	
////		printf("\r\ncamera starts in %d seconds",hk_power_on_time/10);
//		dm9k_init(mac_addr);  //����������ʱ�������л�
//		power_delay_hk=hk_power_on_time;
//		open_hk();
//		cam_on_time_detect=0;
//	}
//	else
//	if((flag==0)&&(power_flag_cam1)) 
//	{
//		power_flag_cam1=flag;	
//		printf("\r\ncamera power off");
//		
//		close_hk();
//		power_set_ETH(flag);//���
//		PWR_W_CAM1(flag);
//	}
//	else	//��ֹ©����������ʵ��״̬���־һ��
//	{
//		power_flag_cam1=flag;	
//		power_set_ETH(flag);//���
//		PWR_W_CAM1(flag);
//	}
//		
//	if(power_flag_cam1==0)
//	{
//		http_linkd=0;
//		dm9k_init_flag=0;
//	}
}

void all_power_down(void)
{
	MCU_OUT1_W(0);	//δʹ��
	MCU_OUT2_W(0);	//δʹ��
	PWR_W_CAM2(0);	//δʹ��
	PWR_W_12V1(0);	//δʹ��
	PWR_W_12V2(0);	//δʹ��
	
	RUN_LED1(0);
	power_set_12V(0);
	power_set_485(0);
	power_set_cam1(0);
	power_set_DTU(0);
	power_flag_sys=0;
}

void low_power(void)
{
	MCU_OUT1_W(0);	//δʹ��
	MCU_OUT2_W(0);	//δʹ��
	PWR_W_CAM2(0);	//δʹ��
	PWR_W_12V1(0);	//δʹ��
	PWR_W_12V2(0);	//δʹ��
	
	RUN_LED1(0);
	power_set_12V(0);
	power_set_485(0);
	power_set_cam1(0);
	power_set_DTU(2);		//�͹���ģʽ
	power_flag_sys=0;
}

void power_on(void)
{
//	MCU_OUT1_W(0);	//δʹ��
//	MCU_OUT2_W(0);	//δʹ��
//	PWR_W_CAM2(0);	//δʹ��
//	PWR_W_12V1(0);	//δʹ��
//	PWR_W_12V2(0);	//δʹ��
	
	RUN_LED1(1);
	power_set_12V(1);
	power_set_485(1);
//	power_set_cam1(0);
	power_set_DTU(1);		//��
	power_flag_sys=1;
}

void power_init(void)
{
	MCU_OUT1_W(0);	//δʹ��
	MCU_OUT2_W(0);	//δʹ��
	PWR_W_CAM2(0);	//δʹ��
	PWR_W_12V1(0);	//δʹ��
	PWR_W_12V2(0);	//δʹ��
	
	RUN_LED1(1);
	power_set_12V(1);	//12V + 485 Լ13mA
	power_set_485(1);
	power_set_cam1(0);
//	power_set_DTU(1);
	DTU_reset();
	power_flag_sys=1;
}

