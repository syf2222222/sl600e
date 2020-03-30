



#include "global.h"
#include "nw.h"
#include "sram.h"
#include "sys_user.h"

int int_time_to_char_time(uint8_t *out,uint32_t sec);
void Fixed_reset_STM32(void);
int gain_qx_data(void);
int get_a_data_ll(void);	//1s一次
int gain_ll_data(void);


extern uint32_t LastTxT;
int get_a_data_ll(void);
int gain_qx_data(void);	
int gain_ll_data(void);

extern uint8_t STM32_sleeping;

uint32_t lastRSTDA=0;
uint32_t LastHBT=0,next_HBT;  //LastGDT=0,LastOLT=0,
uint32_t LastGDT=0; //分钟

uint8_t data_tast_step,DATA_Tast_free;
extern uint8_t HK_Task_free,NW_Tast_free,function84_flag,function22_flag,function25_flag;;


extern uint32_t systime32;

void Runing_clock(void)
{
	unsigned char i;
	uint8_t *star_add;
	OS_ERR      err;
	uint8_t wort_time[6],aaa;
	u16 time_min;
	
	runningtime++;
	int_time_to_char_time(wort_time,runningtime);
	time_min=wort_time[3]*60+wort_time[4];
	
	if(wort_time[5]==0)
	{
		if(lastRSTDA==0) lastRSTDA=runningtime;
		if(((RST[0]==0)||(wort_time[2]==RST[0]))&&(wort_time[3]==RST[1])&&(wort_time[4]==RST[2])&&(work_i100ms>=100)) //日 时 分
		{		
			printf("\r\n系统定时复位时间");
			Fixed_reset_STM32();
			lastRSTDA=runningtime;
			
			STM32_sleeping=0;
		}
	}
////////////////////////////////////////////////////////////////////////////
	if(runningtime>=next_HBT) //如果某次数据错误可能导致心跳严重滞后甚至没有
	{
		senttask_Asim |= data_flag05;
		if(runningtime>=(next_HBT+5)) LastHBT	=	runningtime;
		else LastHBT	=	next_HBT;
		NW_Tast_free=0;
		next_HBT =LastHBT + 60*HBT[0];
	}
	if(LastTxT>=LastHBT+5)
	{
//		LastHBT=LastTxT;
		next_HBT = 60*HBT[0]+LastTxT;
	}
	if(next_HBT>(systime32+60*HBT[0])) //异常恢复
	{
		next_HBT=systime32+60*HBT[0];
	}
		
////////////////////////////////////////////////////////////////////////////	
	if((wort_time[5]==2)&&((function22_flag)||(function25_flag)))
	{
		if(GDT[0]>=6) aaa=3;
		else if(GDT[0]>=4) aaa=2;
		else  aaa=1;
		if((wort_time[4]%aaa)==0)  //1分钟一次？
		{
//			ll_poll_time=0;
			datagettask_Asim |= gat_a_ll_req;
			DATA_Tast_free=0;
		}
		
		if(((time_min-LastGDT)>=GDT[0])||((time_min<LastGDT)&&((time_min+1440-LastGDT)>=GDT[0]))) //采集时间//*60/testdvi
		{
			datagettask_Asim |= gat_data_time;
			LastGDT=time_min;	//精确到分钟
			DATA_Tast_free=0;
		}
	}
////////////////////////////////////////////////////////////////////////////	
	if((wort_time[5]==2)&&(function84_flag)) //
	{		
		for(i=0;i<aut_point_group1[0];i++)
		{
//			if((aut_get_point1[i].h * 60 + aut_get_point1[i].m)==time_min)
			
			if((wort_time[3]==(aut_get_point1[i].h))&&(wort_time[4]==(aut_get_point1[i].m)))
			{
				printf("\r\ntake a photo in %d",aut_get_point1[i].p);
//				printf("\r\nsystime[3]=%d,systime[4]=%d,runningtime=%d,systime32=%d",systime[3],systime[4],runningtime,systime32);
//				printf("\r\nwort_time[3]=%d,wort_time[4]=%d,h=%d,m=%d",wort_time[3],wort_time[4],aut_get_point1[i].h,aut_get_point1[i].m);

				if(HK_post_cnt>=HK_post_max) HK_post_cnt=0;
				star_add=&HK_post_buf[HK_post_cnt];
				HK_post_buf[HK_post_cnt++]=0x83; //拍照指令
				HK_post_buf[HK_post_cnt++]=aut_get_point1[i].p;	//预置位
				OSQPost(&hktask_Asim[0],star_add,2,OS_OPT_POST_FIFO,&err);
				HK_Task_free=0;
				if(power_flag_cam1==0) power_set_cam1(1); //打开球机电源
			}
		}
////////////////////////////////////////////////////////////////////////////
	}
	
}
	
void GDATA_Task(void * p_arg )
{
	data_tast_step=0;
	switch(data_tast_step)
	{
		case 0:
			if((datagettask_Asim & gat_a_ll_req)||(datagettask_Asim & gat_data_time))
			{
				DATA_Tast_free=0;
				if((power_on_delay_485==0)&&(power_flag_485)) //默认开机就开 power
				{
					if(datagettask_Asim & gat_a_ll_req)
					{
						get_a_data_ll();
						datagettask_Asim &= (~gat_a_ll_req);
					}
					else if(datagettask_Asim & gat_data_time)
					{
						if(gain_ll_data()==0) senttask_Asim |= data_flag22;
						if(gain_qx_data()==0) senttask_Asim |= data_flag25;
						
						datagettask_Asim &= (~gat_data_time);
					}
				}
				else //定时采集不因定时休眠和定时复位停止
				{
//					power_set_485(1);
				}
			}
			else
			{
				DATA_Tast_free=1;
			}
			
			break;
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		default:data_tast_step=0;break;
	}
	
}








#if 0
void NWTaskStart (void *p_arg)
{
	OS_ERR      err;
	(void)p_arg;

	

//		多值信号量控制块指针		//多值信号量名称 //资源数目或事件是否发生标志 //返回错误类型
	OSSemCreate (&read_RSSI, "read_RSSI",  0, &err); //3 		
//	OSMutexCreate (&sd_sem_lock, //互斥信号量指针
//                 "dm9000_sem_lock",  //取信号量的名称
//                  &err);   //返回错误类型

#if 1
						 
		/* 创建  开机联络 任务 */
    OSTaskCreate((OS_TCB     *)&NW_REV_Task_TCB,                             //任务控制块地址
                 (CPU_CHAR   *)"NW_REV_Task_Name",                             //任务名称
                 (OS_TASK_PTR ) NW_REV_Task,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) NW_REV_Task_PRIO,                         //任务的优先级
                 (CPU_STK    *)&NW_REV_Task_Stk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) NW_REV_Task_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) NW_REV_Task_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型
#endif	
								 
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
#endif

