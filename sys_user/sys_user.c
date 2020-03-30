
#include "bsp.h"
//#include "timer.h"
#include "global.h" 

unsigned char systime[6]={19,9,29,9,20,47},time_hav_ch=0; //20 (19年 9月 29日 9点 20分 47秒)
unsigned int systime32;

unsigned int runningtime;
unsigned int work_i100ms;
unsigned int testtime;
uint8_t time_cnt;
//int worktime_sys;

void NW_timer_100ms(void);
void NW_timer_1s(void);
//	ts = OS_TS_GET() - ts; //计算消息从发布到被接收的时间差
//	cpu_clk_freq = BSP_CPU_ClkFreq();                 //获取CPU时钟，时间戳是以该时钟计数

int tick_time=0;
int tick_time_h=0;
extern OS_TCB   ethernetif_input_TCB;															//任务控制块

void OSTaskStk_Check()
{
	CPU_STK_SIZE p_free,p_used;
	OS_ERR      err;
	
	OSTaskStkChk (&ethernetif_input_TCB,                  //目标任务控制块的指针
                    &p_free,                 //返回空闲堆栈大小
                    &p_used,                 //返回已用堆栈大小
                    &err);                  //返回错误类型	
	printf("\r\nethernetif_input p_free=%d p_used=%d",p_free,p_used); //实测最高88
}



int EncodeDate(u8 *in,uint32_t *Date) //把年月日转换为天数,从 0年 3月1日 为1 小于  0年3月1日数据异常,计算 (153 * m + 2) / 5 为总月数经过的天数,不会算到2月分的值
{
  int c, y;
//	u16 y;
	u8 m,d;
  
	y=2000+in[0];
	m=in[1];
	d=in[2];
	
  if (m > 2) m -= 3;  //3月-2月  对应 0-11;
	else
	{
		m += 9;
		y -= 1;
	}

	c = y / 100;
	
	y = y - 100 * c;

	*Date= ((146097 * c) / 4 +							//每400年 146097天
					   (1461 * y) / 4 +						//非400的整数, 每4年1461天,余数不要
					   (153 * m + 2) / 5 + d ); 
	
	return 0;
}

int DecodeDate(u8 * out,u32 Date) //把天数期转为 年与日
{
  u32 ly, ld, lm, j;

  j = (Date ) * 4  - 1; 				//
  ly = j / 146097;      				//
  j = j - 146097 * ly;
  ld = j / 4;
  j =(ld * 4 + 3) / 1461;  			//
  ld = (ld * 4 + 7 - 1461*j) / 4;
  lm =(5 * ld-3) / 153;
  ld = (5 * ld +2 - 153*lm) / 5;
  ly = 100 * ly + j;
  if (lm < 10) 
   lm += 3;
  else
    {
      lm -= 9;
      ly++;
    }
		
  out[2] = ld;
  out[1] = lm;
	if(ly>=2000) out[0] = ly-2000;
	else
	{
		out[0] = 0;
		return -1;
	}
	return 0;
}

	// 693900  
	// 1900-1-1 为2,   
	// 2000-01-01 36526  
	// 2019-01-01 43466
	// 2019-03-01 43525
	// 2019-09-29 43737 + 693900= 737637

int char_time_to_int_time(uint8_t *in,uint32_t *sec) //从2000-1-1 0:0:0 分开始算
{
	uint32_t da;
	
//	if((in[0]<y_min)||(in[0]>y_max)) return -1;
	
	if(EncodeDate(in,&da) !=0)
	{
		return -1;
	}
	
	if(da<737637)
	{
		printf("系统时间错误");
		return -1;
	}
	
//	if(da<737700)
//	{
//		printf("\r\n还未校时");
//		return -1;
//	}
	
	*sec=(da-737637)* 86400 + in[3]*3600+in[4]*60+in[5];  //136年溢出, 到2155年

	return 0;
}

int int_time_to_char_time(uint8_t *out,uint32_t sec) //从2000-1-1 0:0:0 分开始算
{
	int da;
		
	da=sec/86400+737637;
	
	if(DecodeDate(out,da)!=0)
	{
		return -1;
	}
	
	out[3]=sec%86400/3600;
	out[4]=sec%3600/60;
	out[5]=sec%60;
	
	return 0;
}
void Runing_clock(void);



void save_systime(void)
{
	uint8_t buf[6];
	runningtime_buf[0]=(runningtime>>24);
	runningtime_buf[1]=(runningtime>>16);
	runningtime_buf[2]=(runningtime>>8);
	runningtime_buf[3]=(runningtime>>0);
	
	int_time_to_char_time(buf,runningtime);
	printf("\r\n保存时间:20%02d-%02d-%02d,%02d:%02d:%02d",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);
	flash_save(runningtime_buf,runningtime_buf,4);
	
//	printf("r\ntime_saved_buf=%08x",time_saved_buf);
}


extern uint8_t STM32_reseting;
uint8_t STM32_sleeping,STM32_rest_delay;

extern uint8_t STM32_rest_wait,HK_Task_free,NW_Tast_free,DATA_Tast_free;

void Fixed_reset_STM32(void)//reset_STM32
{
	if(STM32_reseting==0)
	{
		printf("\r\n准备复位系统.");
		STM32_rest_wait=0;
		STM32_reseting=1;
	}
	else
	{
		printf("\r\n正在等待结束任务.");
	}
//	HAL_NVIC_SystemReset();
}

void reset_polling(void)
{
	if(STM32_rest_delay)	//延时复位,到时间立即复位
	{
		STM32_rest_delay--;
		if(STM32_rest_delay==0)
		{
			save_systime();
			printf("\r\n立即进行手动复位");
			HAL_NVIC_SystemReset();
		}
	}
	
	if(STM32_reseting)	//定时复位,等任务完成才复位
	{
		if(((HK_Task_free==1)&&(PH_sram_buf==bempty))||(STM32_rest_wait>=180))
		{
			printf("HK_Task_free=%d,STM32_rest_wait=%d",HK_Task_free,STM32_rest_wait);
			save_systime();
			printf("\r\n立即进行定时复位");
			HAL_NVIC_SystemReset();
		}
		STM32_rest_wait++;
	}
}
void SRAM_data_int(void);

uint8_t time_fine_tuning;
void sys_clock(void)
{
	char datmax=30;
	if(++systime[5]>=60)
	{
		systime[5]=0;		
		
		if(++systime[4]>=60)
		{
			systime[4]=0;
			if(++systime[3]>=24)
			{
				systime[3]=0;
				if(systime[1]==2) //2月
				{
					if(((systime[0]%100!=0)&&(systime[0]%4==0))||(systime[0]%400==0)) datmax=29;//润年
					else datmax=28;//非润年
				}
				else if((systime[1]==4)||(systime[1]==6)||(systime[1]==9)||(systime[1]==11)) datmax=30;// 小月 (systime[1]==2)||
				else datmax=31;

				if(++systime[2]>datmax) //日超出最大值,从1开始循环
				{
					systime[2]=1;
					if(++systime[1]>12) //月超出最大值,从1开始循环
					{
						systime[1]=1;
						systime[0]++; //年 +1
					}
				}
			}
		}
	}
}
extern IWDG_HandleTypeDef hiwdg;
void udelay_no_inv(int cnt)
{
	while(cnt--);
}
uint32_t onlin_time;
extern void SystemClock_Config(void);
extern void MX_GPIO_Init(void);
void all_power_down(void);
void low_power(void);
void power_on(void);
//void sent_HeartBeat(void);

void EXTI_SET(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	  /*Configure GPIO pin : PA6 */
//  GPIO_InitStruct.Pin = GPIO_PIN_6;
//  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		
	  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	
	GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	
//	/*Configure GPIO pins : PB0 PB8 PB9 */
//  GPIO_InitStruct.Pin = GPIO_PIN_1;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//	
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); //PB1=0; 使能RTS接收
}

void HAL_NVIC_DisableIRQ(IRQn_Type IRQn);
HAL_StatusTypeDef HAL_UART_DeInit(UART_HandleTypeDef *huart);

uint8_t EXTI9_5_flag,EXTI15_10_flag;
extern RTC_HandleTypeDef hrtc;

HAL_StatusTypeDef HAL_RTC_SetTime(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTime, uint32_t Format);
HAL_StatusTypeDef HAL_RTC_GetTime(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTime, uint32_t Format);
HAL_StatusTypeDef HAL_RTC_SetDate(RTC_HandleTypeDef *hrtc, RTC_DateTypeDef *sDate, uint32_t Format);
HAL_StatusTypeDef HAL_RTC_GetDate(RTC_HandleTypeDef *hrtc, RTC_DateTypeDef *sDate, uint32_t Format);

RTC_TimeTypeDef	rtcTime;
RTC_DateTypeDef rtcDate;

//#define RTC_FORMAT_BIN                      0x00000000u
//#define RTC_FORMAT_BCD                      0x00000001u
//RTC_HOURFORMAT_24
//我告诉你，STM的RTC一直有个很奇怪的问题，一定要先读time，再读date，不然time是不对的。 这么多年一直都没有改。
uint32_t hav_sleepT=0,sleep_time;
extern uint8_t wr_sleep;
extern uint32_t next_HBT;

void sys_time_chang(void)
{
		rtcDate.Year=systime[0];
		rtcDate.Month=systime[1];
		rtcDate.Date=systime[2];
		rtcTime.Hours=systime[3];
		rtcTime.TimeFormat=RTC_HOURFORMAT_24;
		rtcTime.Minutes=systime[4];	
		rtcTime.Seconds=systime[5];	
		
		HAL_RTC_SetTime(&hrtc, &rtcTime, RTC_FORMAT_BIN);
		HAL_RTC_SetDate(&hrtc, &rtcDate, RTC_FORMAT_BIN);
}

void sys_time_check(void)
{
	
	u32 buf1,buf2;
	HAL_RTC_GetTime(&hrtc, &rtcTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &rtcDate, RTC_FORMAT_BIN);
	
	buf1=rtcTime.Hours*3600+rtcTime.Minutes*60+rtcTime.Seconds;
	buf2=systime[3]*3600+systime[4]*60+systime[5];
	if((rtcDate.Year!=systime[0])||(rtcDate.Month!=systime[1])||(rtcDate.Date!=systime[2])
		||(rtcTime.Hours!=systime[3])||(rtcTime.Minutes!=systime[4])||(rtcTime.Seconds>(systime[5]+15))||((rtcTime.Seconds+15)<systime[5]))		//误差超过10s
	{
//		if(Contact_state==1)
		{
			sys_time_chang();
			printf("\r\n更新RTC时钟");
		}
	}
	else if(buf1!=buf2)
	{
		systime[5]=rtcTime.Seconds;
		printf("\r\n下载RTC时钟");
	}
}

void sys_time_get(void)
{
	HAL_RTC_GetTime(&hrtc, &rtcTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &rtcDate, RTC_FORMAT_BIN);
	systime[0]=rtcDate.Year;
	systime[1]=rtcDate.Month;
	systime[2]=rtcDate.Date;
	systime[3]=rtcTime.Hours;
	systime[4]=rtcTime.Minutes;	
	systime[5]=rtcTime.Seconds;	
}
void sent_Sleep(void);

uint8_t sleepdelay=0;

void stop_test(void)
{
	uint32_t sleepT;
//	OS_ERR      err;
	
//	printf("%d,",onlin_time);
	
//	if(STM32_sleeping==0)
//	{
//		sleep_time=0;
//		onlin_time++;
//	}
//	if((SLT[0]==0)&&(wr_sleep==0))
//	{
//		STM32_sleeping=0;
//		return;
//	}
//	
//	if((onlin_time>=(OLT[0]*60))||(wr_sleep))//&&(HK_Task_free==1)&&(NW_Tast_free==1)
//	{
//		if(STM32_sleeping==0)
//		{
//			STM32_sleeping=1;
//			sleep_time=SLT[0];
//			onlin_time=0; //如果睡眠需要采集数据则不清零
//			printf("\r\n在线时长到");
//		}
//	}
	return;

	if(STM32_reseting) return;
	if(sleepdelay)
	{		
		sleepdelay--;
	}	
//	if(sleepdelay>1) 
//	{
//		sleepdelay--;
////		return;
//	}
	
	STM32_sleeping=1;
		
	if(((HK_Task_free==1)&&(NW_Tast_free==1)&&(DATA_Tast_free==1)&&(sleepdelay==0))||(wr_sleep))	//睡眠时间到,((HK_Task_free==1)||(STM32_rest_wait>=180)) sleep_time&&
	{
		if(wr_sleep) sleepdelay=0;
		if(next_HBT>systime32+3) sleepT=next_HBT-(systime32+1)-1; //睡眠时长
		else
		{
			return;
		}
		if((sleepT>60*HBT[0])||(sleepT>600)) 
		{
			printf("\r\n休眠时间计算错误%d",sleepT);
			return;
		}
		STM32_sleeping=2;
		printf("\r\n进入休眠:20%d-%d-%d %d:%d:%d\t%d",systime[0],systime[1],systime[2],systime[3],systime[4],systime[5],sleepT);	
		HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, sleepT, RTC_WAKEUPCLOCK_CK_SPRE_16BITS); //(9+1)s,设置唤醒时间
		
		low_power();	
		udelay_no_inv(1000000);

		HAL_UART_DeInit(&huart3);	//关串口
		HAL_UART_DeInit(&huart1);	//关串口

//		EXTI9_5_flag=0;	
//		EXTI15_10_flag=0;
		EXTI_SET();								//开外部中断,会进一次 EXTI9_5中断?
		EXTI9_5_flag=0;
		EXTI15_10_flag=0;
		udelay_no_inv(1000000);
///////////////////////////////////////////////////////////////////////////////////////////////////////////		
		if((sleepdelay==0)&&(EXTI9_5_flag==0)&&(EXTI15_10_flag==0))
		{
			HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI); //停止模式
			SystemClock_Config();		//重设时钟
		}
		else STM32_sleeping=1;
///////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);	//停止闹钟
		HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);	//关闭外部中断
		HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);	//关闭外部中断
		HAL_UART_Init(&huart3);						//打开串口
		HAL_UART_Init(&huart1);						//打开串口

		tick_time_h=tick_time;
		power_on();
		sys_time_get();	 //睡眠后更新系统时间
		
		if(STM32_sleeping==1)
		{
			printf("\r\n未准备好,打断,%d,%d/%d",sleepdelay,EXTI9_5_flag,EXTI15_10_flag);
			
			if((EXTI9_5_flag>=5)||(EXTI15_10_flag>=5))	sleepdelay=6;
		}
		else 
		{
			if(EXTI9_5_flag>EXTI15_10_flag)	//南网信号唤醒
			{
				sleepdelay=6;
				sleep_time=0;
				STM32_sleeping=0;
				printf("\r\n南网唤醒:20%d-%d-%d %d:%d:%d\t%d/%d",rtcDate.Year,rtcDate.Month,rtcDate.Date,rtcTime.Hours,rtcTime.Minutes,rtcTime.Seconds,EXTI9_5_flag,EXTI15_10_flag);
			}
			else if(EXTI15_10_flag)	//工作台唤醒
			{
				sleepdelay=6;
				sleep_time=0;
				STM32_sleeping=0;
				printf("\r\n工作唤醒:20%d-%d-%d %d:%d:%d\t%d/%d",rtcDate.Year,rtcDate.Month,rtcDate.Date,rtcTime.Hours,rtcTime.Minutes,rtcTime.Seconds,EXTI9_5_flag,EXTI15_10_flag);
			}
			else	//定时唤醒
			{
				sleepdelay=2;
				sleep_time--;
				if(sleep_time==0) 
				{
					STM32_sleeping=0;
					printf("\r\n休眠结束:20%d-%d-%d %d:%d:%d",rtcDate.Year,rtcDate.Month,rtcDate.Date,rtcTime.Hours,rtcTime.Minutes,rtcTime.Seconds);
				}
				else
				{
					printf("\r\n定时唤醒:20%d-%d-%d %d:%d:%d",rtcDate.Year,rtcDate.Month,rtcDate.Date,rtcTime.Hours,rtcTime.Minutes,rtcTime.Seconds);
				}
			}
		}
			
		STM32_sleeping=1;		
		wr_sleep=0;
		hav_sleepT=60;
		NW_Tast_free=0;
	}
	else
	{
//		(HK_Task_free==1)&&(NW_Tast_free==1)&&(DATA_Tast_free==1)&&(sleepdelay==0)
		
//		printf("\r\nHK_Task_free=%d,NW_Tast_free=%d,DATA_Tast_free=%d,sleepdelay=%d,",HK_Task_free,NW_Tast_free,DATA_Tast_free,sleepdelay);
	}
}
//uint8_t bufaa[6];



//void osStaDisplay(void)
//{
//OS_TCB *ptcb = OSTCBList;
//if(ptcb)
//do{
//		printf("%s\t", ptcb->OSTCBTaskName);
//		printf(" TaskPrio:%d ? used/free:%d/%d ?usage:%%%d\r\n", 
//			ptcb->OSTCBPrio, ptcb->OSTCBStkUsed, ptcb->OSTCBStkSize * sizeof(OS_STK) - ptcb->OSTCBStkUsed, ptcb->OSTCBStkUsed * 100 / (ptcb->OSTCBStkSize * sizeof(OS_STK) ));
//		//ptcb->OSTCBCtxSwCtr
//		//ptcb->OSTCBCyclesStart
//		//ptcb->OSTCBCyclesTot
//		OSTimeDly(OS_TICKS_PER_SEC/100);
//}while(ptcb = ptcb->OSTCBNext);

//printf("\r\nCPU_UseAge:%%%d", OSCPUUsage);
// printf("\r\n\r\n\r\n");
//}


void AppTasksysT( void * p_arg )
{
	OS_ERR      err;
	
//	GET_TS();
//	CPU_TS_TmrInit
//	HAL_IWDG_Refresh(&hiwdg);
//	SRAM_data_int();
	while (DEF_TRUE)
	{
//		OSTimeDlyHMSM ( 0, 0, 0, 1200, OS_OPT_TIME_DLY, & err ); 
		OSTimeDly ( 50, OS_OPT_TIME_DLY, & err ); 
		
		if(tick_time-tick_time_h>=100)
		{
//			uwTick
			tick_time_h += 100;
			time_cnt++;
//			HAL_IWDG_Refresh(&hiwdg);//
			
			NW_timer_100ms();
			
			if(time_cnt>=10)
			{				
				time_cnt -= 10;		
//				NW_timer_1s();
//				printf("+");

				sys_clock();
				if(systime[5]==30) sys_time_check(); //因休眠后更新时间的情况,此句无意义.

				if(work_i100ms>=280)	stop_test();
				
				char_time_to_int_time(systime,&systime32);
				
//				printf("\r\n%d-%d-%d %d:%d:%d,%d",systime[0],systime[1],systime[2],systime[3],systime[4],systime[5],systime32);
//				char_time_to_int_time(systime,&systime32);
//				printf("\r\n%d-%d-%d %d:%d:%d,%d",systime[0],systime[1],systime[2],systime[3],systime[4],systime[5],systime32);
//				int_time_to_char_time(systime,systime32);
//				printf("\r\n%d-%d-%d %d:%d:%d,%d",systime[0],systime[1],systime[2],systime[3],systime[4],systime[5],systime32);
//				int_time_to_char_time(bufaa,7120922);
//				printf("\r\n%d-%d-%d %d:%d:%d",bufaa[0],bufaa[1],bufaa[2],bufaa[3],bufaa[4],bufaa[5]);
				
				reset_polling();
				
					
				if((STM32_reseting==0)&&(work_i100ms>=100))//&&((time_fine_tuning==0)||(systime32>=runningtime+1)))  ,过10s,等待拍照任务启动才能开始post
				{
					if((systime32>(runningtime+240+hav_sleepT))||((systime32+240)<runningtime)) //时间严重错乱
					{
						hav_sleepT=0;
						runningtime=systime32;
						printf("\r\n时钟错误");
						save_systime();	//整点保存系统时间
					}
					else
					{
						hav_sleepT=0;
						while((runningtime<systime32)&&(STM32_reseting==0)) //比系统时间小,直接运行到系统时间
						{
							Runing_clock(); //跑到系统时间为止
							if(runningtime%3600==0)
							{
								printf("\r\n整点报时");
								save_systime();	//整点保存系统时间
							}
						}
						if(runningtime>systime32) //比系统时间大,什么都不做,等待
						{
							
						}
					}
				}
			}
		}	
	}
}


uint8_t STM32_reseting=0;
uint8_t STM32_rest_wait;


void wakeup_dev(void)
{
}
void dev_sleep(void)
{
}

#define  AppTasksysT_SIZE                     512
#define  AppTasksysT_PRIO                      5
OS_TCB   AppTasksysT_TCB;															//任务控制块
CPU_STK  AppTasksysT_Stk [ AppTasksysT_SIZE ];	//任务堆栈
void  AppTasksysT  ( void * p_arg );

#define  TaskDSP_SIZE                     128
#define  TaskDSP_PRIO                      5
OS_TCB   TaskDSP_TCB;															//任务控制块
CPU_STK  TaskDSP_Stk [ TaskDSP_SIZE ];	//任务堆栈
void  TaskDSP  ( void * p_arg );

void SYSTaskStart (void *p_arg)
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
								 
#if 1	
		/* 创建 任务 */
    OSTaskCreate((OS_TCB     *)&TaskDSP_TCB,                             //任务控制块地址
                 (CPU_CHAR   *)"TaskDSP_Name",                             //任务名称
                 (OS_TASK_PTR ) TaskDSP,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) TaskDSP_PRIO,                         //任务的优先级
                 (CPU_STK    *)&TaskDSP_Stk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) TaskDSP_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) TaskDSP_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型
#endif	
		
}


//extern OS_TCB		NW_SENT_Task_TCB;															//任务控制块
//extern OS_TCB   AppTasksysT_TCB;															//任务控制块
//extern OS_TCB		WR_Task_TCB;															//任务控制块
//extern OS_TCB   http_hk_thread_TCB;															//任务控制块
//extern OS_TCB   HK_Task_ch1_TCB;															//任务控制块

OS_MEM  mem;                    //声明内存管理对象
uint8_t ucArray [ 10 ] [ 4 ];   //声明内存分区大小

static  void  TaskDSP ( void * p_arg )
{
	OS_ERR         err;
	CPU_TS_TMR     ts_int;
	CPU_INT16U     version;
	CPU_INT32U     cpu_clk_freq;
	CPU_SR_ALLOC();
	char * pMsg;
	OS_MSG_SIZE    msg_size;
	OS_TCB *DSP_TCB;
	
	(void)p_arg;

	version = OSVersion(&err);                          //获取uC/OS版本号
	
  cpu_clk_freq = BSP_CPU_ClkFreq();                   //获取CPU时钟，时间戳是以该时钟计数
	
	/* 创建内存管理对象 mem */
		OSMemCreate ((OS_MEM      *)&mem,             //指向内存管理对象
								 (CPU_CHAR    *)"Mem For Test",   //命名内存管理对象
								 (void        *)ucArray,          //内存分区的首地址
								 (OS_MEM_QTY   )10,               //内存分区中内存块数目
								 (OS_MEM_SIZE  )4,                //内存块的字节数目
								 (OS_ERR      *)&err);            //返回错误类型

	
	while (DEF_TRUE) {                                  //任务体
		/* 阻塞任务 */
//		OSTaskSemPend ((OS_TICK   )0,                     //无期限等待
//									 (OS_OPT    )OS_OPT_PEND_BLOCKING,  //如果信号量不可用就等待
//									 (CPU_TS   *)0,                     //获取信号量被发布的时间戳
//									 (OS_ERR   *)&err);                 //返回错误类型
		
		pMsg = OSTaskQPend ((OS_TICK        )0,                    //无期限等待
											  (OS_OPT         )OS_OPT_PEND_BLOCKING, //没有消息就阻塞任务
											  (OS_MSG_SIZE   *)&msg_size,            //返回消息长度
											  (CPU_TS        *)0,                    //返回消息被发布的时间戳
											  (OS_ERR        *)&err);                //返回错误类型
		
		
		ts_int = CPU_IntDisMeasMaxGet ();                 //获取最大关中断时间

		OS_CRITICAL_ENTER();                              //进入临界段，避免串口打印被打断

		if(*pMsg==0)
		{
			printf ( "\r\n\r\nuC/OS版本号：V%d.%02d.%02d",
             version / 10000, version % 10000 / 100, version % 100 );
    
			printf ( "\r\nCPU主频：%d MHz", cpu_clk_freq / 1000000 );  
			
			printf ( "\r\n最大中断时间：%d us", 
							ts_int / ( cpu_clk_freq / 1000000 ) ); 

			printf ( "\r\n最大锁调度器时间：%d us", 
							 OSSchedLockTimeMax / ( cpu_clk_freq / 1000000 ) );		

			printf ( "\r\n任务切换总次数：%d", OSTaskCtxSwCtr ); 	
			
			printf ( "\r\nCPU使用率：%d.%d%%",
							 OSStatTaskCPUUsage / 100, OSStatTaskCPUUsage % 100 );  
			
			printf ( "\r\nCPU最大使用率：%d.%d%%", 
							 OSStatTaskCPUUsageMax / 100, OSStatTaskCPUUsageMax % 100 );
		}
		else
		{
			extern OS_TCB		NW_SENT_Task_TCB;															//任务控制块
			extern OS_TCB   AppTasksysT_TCB;															//任务控制块
			extern OS_TCB		WR_Task_TCB;															//任务控制块
			extern OS_TCB   http_hk_thread_TCB;															//任务控制块
			extern OS_TCB   HK_Task_ch1_TCB;															//任务控制块
			
			if(*pMsg==1) DSP_TCB=&NW_SENT_Task_TCB;
			if(*pMsg==2) DSP_TCB=&AppTasksysT_TCB;
			if(*pMsg==3) DSP_TCB=&WR_Task_TCB;
			if(*pMsg==4) DSP_TCB=&http_hk_thread_TCB;
			if(*pMsg==5) DSP_TCB=&HK_Task_ch1_TCB;
			
			if(*pMsg==6) DSP_TCB=&WR_Task_TCB;
			if(*pMsg==7) DSP_TCB=&WR_Task_TCB;
			if(*pMsg==8) DSP_TCB=&WR_Task_TCB;
			if(*pMsg==9) DSP_TCB=&WR_Task_TCB;

			printf ( "\r\n任务%d的CPU使用率：    %d.%d%%", *pMsg,DSP_TCB->CPUUsage / 100, DSP_TCB->CPUUsage % 100 );  
						 
			printf ( "\r\n任务%d的CPU最大使用率：%d.%d%%", *pMsg,DSP_TCB->CPUUsageMax / 100, DSP_TCB->CPUUsageMax % 100 ); 
			
			printf ( "\r\n任务%d的堆栈使用情况： %d,%d",   *pMsg,DSP_TCB->StkUsed, DSP_TCB->StkFree ); 
		}
		
		
		
						 
		
		
		
		/* 退还内存块 */
		OSMemPut ((OS_MEM  *)&mem,                                 //指向内存管理对象
							(void    *)pMsg,                                 //内存块的首地址
							(OS_ERR  *)&err);		                             //返回错误类型
							
		OS_CRITICAL_EXIT();                               //退出临界段
		
	}
	
}


