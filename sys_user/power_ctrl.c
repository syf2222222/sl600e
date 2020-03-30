
#include "bsp.h"
#include "global.h"

unsigned char power_flag_DTU,power_flag_485,power_flag_12V,power_flag_cam1,power_flag_ETH,dm9k_init_flag,power_flag_sys,power_on_delay_485;
uint16_t power_delay_hk;

//进入低功耗的方法：PQ7600 退出到命令模式（单串口）或利用命令串口（双
//串口），发送AT+CTEST=0，然后拉高dtr。如果进入低功耗，网络灯会停止
//闪烁。注意要拔掉USB。
//? 退出低功耗的方法：拉低dtr 然后通过串口发送命令：AT+CTEST=1。网络灯
//恢复工作。

void power_set_DTU(uint8_t flag)
{	
	OS_ERR      err;
	if(flag>2)
	{
		printf("\r\nDTU电源设置参数异常");
	}
	if(flag==0)	//关闭
	{
		PWR_W_DTU(0);
	}
	if(flag==1)	//开
	{
		if(power_flag_DTU==2)	//退出低功耗
		{
			DTU_W_DTR(1);//拉低DTR
			OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err ); 
			HAL_UART_Transmit_IT(&huart5, (unsigned char *)"AT+CTEST=1\r\n",strlen("AT+CTEST=1\r\n"));
		}
		else		//打开DTU
		{
			PWR_W_DTU(1);
			DTU_W_DTR(1);	
		}
	}
	if(flag==2)	//进入低功耗
	{
		if(power_flag_DTU==0)	PWR_W_DTU(1);//原来为关闭
		HAL_UART_Transmit_IT(&huart5, (unsigned char *)"AT+CTEST=0\r\n",strlen("AT+CTEST=0\r\n"));
		DTU_W_DTR(0);//拉高DTR
//		OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err ); 
		HAL_UART_Transmit_IT(&huart5, (unsigned char *)"AT+CTEST=0\r\n",strlen("AT+CTEST=0\r\n"));
		DTU_W_DTR(0);//拉高DTR
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

void power_set_485(uint8_t flag) //485需要 12V开关
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

void power_set_ETH(uint8_t flag) //ETH跟随cam1
{
	power_flag_ETH=flag;
	PWR_W_ETH(power_flag_ETH);
}
extern uint8_t http_linkd;

void close_hk(void);
void open_hk(void);
u16 cam_on_time_detect;

void power_set_cam1(uint8_t flag)	//cam1 开关同时控制ETH
{	
	if(power_flag_cam1 != flag)
	{
		power_flag_cam1=flag;
		power_set_ETH(power_flag_cam1);
		PWR_W_CAM1(power_flag_cam1);
		
		if(power_flag_cam1 == 0)	//关
		{
			printf("\r\ncamera power off");
			close_hk();
			http_linkd=0;
			dm9k_init_flag=0;
		}
		else
		{
			printf("\r\ncamera power on");
			dm9k_init(mac_addr);  //这里面有延时，任务切换
			power_delay_hk=hk_power_on_time;
			open_hk();
			cam_on_time_detect=0;
		}
		
	}
	else //重复操作一次
	{
		power_set_ETH(power_flag_cam1);
		PWR_W_CAM1(power_flag_cam1);	
	}

//	if((flag)&&(power_flag_cam1==0))
//	{
//		power_flag_cam1=flag;	
//		printf("\r\ncamera power on");
//		power_set_ETH(flag);//先开
//		PWR_W_CAM1(flag);	
////		printf("\r\ncamera starts in %d seconds",hk_power_on_time/10);
//		dm9k_init(mac_addr);  //这里面有延时，任务切换
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
//		power_set_ETH(flag);//后关
//		PWR_W_CAM1(flag);
//	}
//	else	//防止漏操作，保持实际状态与标志一致
//	{
//		power_flag_cam1=flag;	
//		power_set_ETH(flag);//后关
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
	MCU_OUT1_W(0);	//未使用
	MCU_OUT2_W(0);	//未使用
	PWR_W_CAM2(0);	//未使用
	PWR_W_12V1(0);	//未使用
	PWR_W_12V2(0);	//未使用
	
	RUN_LED1(0);
	power_set_12V(0);
	power_set_485(0);
	power_set_cam1(0);
	power_set_DTU(0);
	power_flag_sys=0;
}

void low_power(void)
{
	MCU_OUT1_W(0);	//未使用
	MCU_OUT2_W(0);	//未使用
	PWR_W_CAM2(0);	//未使用
	PWR_W_12V1(0);	//未使用
	PWR_W_12V2(0);	//未使用
	
	RUN_LED1(0);
	power_set_12V(0);
	power_set_485(0);
	power_set_cam1(0);
	power_set_DTU(2);		//低功耗模式
	power_flag_sys=0;
}

void power_on(void)
{
//	MCU_OUT1_W(0);	//未使用
//	MCU_OUT2_W(0);	//未使用
//	PWR_W_CAM2(0);	//未使用
//	PWR_W_12V1(0);	//未使用
//	PWR_W_12V2(0);	//未使用
	
	RUN_LED1(1);
	power_set_12V(1);
	power_set_485(1);
//	power_set_cam1(0);
	power_set_DTU(1);		//打开
	power_flag_sys=1;
}

void power_init(void)
{
	MCU_OUT1_W(0);	//未使用
	MCU_OUT2_W(0);	//未使用
	PWR_W_CAM2(0);	//未使用
	PWR_W_12V1(0);	//未使用
	PWR_W_12V2(0);	//未使用
	
	RUN_LED1(1);
	power_set_12V(1);	//12V + 485 约13mA
	power_set_485(1);
	power_set_cam1(0);
//	power_set_DTU(1);
	DTU_reset();
	power_flag_sys=1;
}

