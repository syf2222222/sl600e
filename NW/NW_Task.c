


#define  NW_MODULE
#include "global.h"
#include "nw.h"
#undef NW_MODULE

#include "sram.h"
#include "sys_user.h"
#include "usart.h"
#include "bsp_uart.h"
#include "FM25V10.h"
#include "power_ctrl.h"

const unsigned char ver[2]={1,0};
	

uint32_t LastTxT;
uint8_t ip_seting;
uint8_t NW_Tast_free;
uint8_t NW_Task_step=0;

extern uint8_t DTU_c_step,DTU_c_state;
extern uint8_t Contact_step,Contact_state;
extern uint8_t Sdata_step,Sdata_state;
extern uint8_t Sphoto_step,Sphoto_state;

extern void UART_data_clear(UART_HandleTypeDef *huart);	
extern void Rev4g_polling(void * p_arg);
extern void GDATA_Task(void * p_arg );
extern void Sdata_polling(void);
extern void resent_ph(uint8_t *data_in,uint16_t data_len);
extern void Sphoto_polling(void);
extern void DTU_Check_polling(void);
extern void NW_contact_polling(void);


extern u16 hk_wait_time,power_delay_hk,cam_on_time_detect,sentdelay;
extern u16 DTU_c_time,Contact_time,Sdata_time,Sphoto_time;
void NW_timer_100ms(void)
{
	hk_wait_time++;
	work_i100ms++;
	Contact_time++;
	DTU_c_time++;
	Sdata_time++;
	Sphoto_time++;
	
	
	if(sentdelay) sentdelay--;
	if(power_delay_hk) power_delay_hk--;
	if(power_on_delay_485) power_on_delay_485--;
	
	cam_on_time_detect++;
}


void  NW_SENT_Task  ( void * p_arg )//void  NWTask_task( void * p_arg )
{
	OS_ERR      err; 
//	uint8_t j;
	(void)p_arg;	
	
	UART_data_clear(&huart3);
	
//	function_buf[0]=0x22;
//	function_buf[1]=0x25;
//	function_nub[0]=2;
		
	LastTxT=runningtime;
	
	while (DEF_TRUE)
	{	
		OSTimeDly( 100, OS_OPT_TIME_DLY, & err);
		
		Rev4g_polling(0);		//ur3监测，南网数据
		GDATA_Task(0);		//定时采集数据
		
		if((DTU_c_state==0)||(ip_seting)) DTU_Check_polling();	//ur5监测,dtu状态
		else
		{
			switch(NW_Task_step)
			{
				case 0:		//心跳任务				
					NW_contact_polling();	//开机联络，校时，心跳
					
					if((Contact_state)&&(Contact_step==0))//&&(STM32_sleeping==0)
					{
//						if((senttask_Asim &= data_flag22)||(senttask_Asim &= data_flag25))
//						{
//							Sdata_polling();		//数据上传
//							if(Sdata_step!=0) 
//							{
//								NW_Task_step=2;
//							}
//						}
					
						Sdata_polling();		//数据上传
						if(Sdata_step!=0) 
						{
							NW_Task_step=2;
						}
						else
						{
							Sphoto_polling();	//照片上传
							if(Sphoto_step!=0) 
							{
								NW_Task_step=3;
							}
						}
					}		
					if((NW_Task_step==0)&&(Contact_step==0))	//(Contact_state==1)&&
					{
//						if(NW_Tast_free==0) printf("\r\nNW_Tast_free:%d:%d:%d",systime[3],systime[4],systime[5]);	
						NW_Tast_free=1;
					}
					else
					{
						NW_Tast_free=0;
					}
					break;					
				case 2:
					Sdata_polling();		//数据上传
					if(Sdata_step==0) 
					{
						NW_Task_step=0;
					}
					break;
				case 3:
					Sphoto_polling();	//照片上传
					if(Sphoto_step==0) 
					{
						NW_Task_step=0;
					}
					break;
				default:
					NW_Task_step=0;
					break;
			}
		}
	}
}



//#define		NW_REV_Task_SIZE                     512
//#define		NW_REV_Task_PRIO                         9
//OS_TCB		NW_REV_Task_TCB;															//任务控制块
//CPU_STK		NW_REV_Task_Stk [ NW_REV_Task_SIZE ];	//任务堆栈

#define		NW_SENT_Task_SIZE                     512
#define		NW_SENT_Task_PRIO                         9
OS_TCB		NW_SENT_Task_TCB;															//任务控制块
CPU_STK		NW_SENT_Task_Stk [ NW_SENT_Task_SIZE ];	//任务堆栈




void NW_REV_Task  (void * p_arg );
void NW_SENT_Task (void *p_arg);

void NW_HK_Task_Create (void);

OS_SEM read_RSSI;
//OS_MUTEX sd_sem_lock;

void NWTaskStart (void *p_arg)
{
	OS_ERR      err;
	(void)p_arg;

	

//		多值信号量控制块指针		//多值信号量名称 //资源数目或事件是否发生标志 //返回错误类型
	OSSemCreate (&read_RSSI, "read_RSSI",  0, &err); //3 		
//	OSMutexCreate (&sd_sem_lock, //互斥信号量指针
//                 "dm9000_sem_lock",  //取信号量的名称
//                  &err);   //返回错误类型

//#if 1
//						 
//		/* 创建  开机联络 任务 */
//    OSTaskCreate((OS_TCB     *)&NW_REV_Task_TCB,                             //任务控制块地址
//                 (CPU_CHAR   *)"NW_REV_Task_Name",                             //任务名称
//                 (OS_TASK_PTR ) NW_REV_Task,                                //任务函数
//                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
//                 (OS_PRIO     ) NW_REV_Task_PRIO,                         //任务的优先级
//                 (CPU_STK    *)&NW_REV_Task_Stk[0],                          //任务堆栈的基地址
//                 (CPU_STK_SIZE) NW_REV_Task_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
//                 (CPU_STK_SIZE) NW_REV_Task_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
//                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
//                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
//                 (void       *) 0,                                          //任务扩展（0表不扩展）
//                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
//                 (OS_ERR     *)&err);                                       //返回错误类型
//#endif	
								 
#if 1
						 
		/* 创建  开机联络 任务 */
    OSTaskCreate((OS_TCB     *)&NW_SENT_Task_TCB,                             //任务控制块地址
                 (CPU_CHAR   *)"NW_SENT_Task_Name",                             //任务名称
                 (OS_TASK_PTR ) NW_SENT_Task,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) NW_SENT_Task_PRIO,                         //任务的优先级
                 (CPU_STK    *)&NW_SENT_Task_Stk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) NW_SENT_Task_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) NW_SENT_Task_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型
#endif		
							 

}


