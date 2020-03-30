
//#include "bsp.h"
#include  <os.h>

#include <stdio.h>
#include <string.h>

#define  NW_MODULE
#include "nw.h"
#undef NW_MODULE

#include "stdint.h"

#include "sram.h"

#include "timer.h"
#include "usart.h"

#include "FM25V10.h"

OS_MUTEX RX_4G;                         //声明互斥信号量

OS_SEM RX_4G_AT;          //标志KEY1是否被单击的多值信号量

#define Pend_4G_Mutex()		OSMutexPend ((OS_MUTEX  *)&RX_4G, (OS_TICK )0, (OS_OPT)OS_OPT_PEND_BLOCKING, (CPU_TS *)0, (OS_ERR *)&err)      
//		OSMutexPend ((OS_MUTEX  *)&RX_4G,        //申请互斥信号量 mutex
//								 (OS_TICK    )0,                       //无期限等待
//								 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //如果申请不到就堵塞任务
//								 (CPU_TS    *)0,                       //不想获得时间戳
//								 (OS_ERR    *)&err);                   //返回错误类型		

#define Post_4G_Mutex()		OSMutexPost ((OS_MUTEX  *)&RX_4G, (OS_OPT )OS_OPT_POST_NONE,(OS_ERR *)&err) 
//OSMutexPost ((OS_MUTEX  *)&RX_4G,                 //释放互斥信号量 mutex
//								 (OS_OPT     )OS_OPT_POST_NONE,       //进行任务调度
//								 (OS_ERR    *)&err);                  //返回错误类型	
								 

OS_Q NW_Msg;				//定义一个消息队列，用于按键消息传递，模拟消息邮箱

void ph_test(void);
	extern OS_SEM http_rev_sem,http_200_sem,http_401_sem,http_data_sem;

int PUT_PTZ_s(char *command);
//PAN_LEFT ：水平向左转动
//PAN_RIGHT：水平向右转动
//TILT_UP：垂直向上运动
//TILT_DOWN：垂直向下运动
//ZOOM_IN：镜头拉远
//ZOOM_OUT：镜头拉近
//UP_LEFT: 左上转向运动
//UP_RIGHT: 右上转向运动
//DOWN_LEFT: 左下转向运动
//DOWN_RIGHT: 右下转向运动
//PAN_AUTO: 自动巡航	
unsigned int LastTxT=0;
unsigned int LastGetDataT=0;
unsigned char sampled_step,sampled_data_nub=0,sampled_data_flag=0;

void sampled_data_sent_22(unsigned char step,unsigned char zbs);
void sampled_data_sent_25(unsigned char zbs);

#define testdvi	5
int check_signel(void);
int Get_Volt(void);
uint8_t GDT_OK=1,GDT_fun=0,sampled_data_time=0;

#define nw_ctl_Q_max	20
struct nw_ctl nw_ctl_Q[nw_ctl_Q_max];
uint8_t nw_ctl_Q_nub,Streaming_OK=0,ip_seting=0;
extern unsigned char STM32_rest_delay;

void check_retxtime()
{
	switch(ctlnumber)
	{
		case 	Contact:retxtime=60/testdvi;	break;	//开机联络，30s重发
		case 	CheckT:retxtime=120/testdvi;	break;	//校时，2分钟重发 	//开机联络，30s重发
		case 	HeartBeat:retxtime=*HBT*60/testdvi;	break;//	装置心跳信息	用于主站监测装置上线时间、IP地址和端口号、传输信号强度及蓄电池电压
		case 	0x84:retxtime=2;	break;//
		case 	0x86:retxtime=2;	break;//	装置心跳信息	用于主站监测装置上线时间、IP地址和端口号、传输信号强度及蓄电池电压
		case 	0x22:retxtime=2;	break;//
		case 	0x25:retxtime=2;	break;//
	}
}
unsigned char buf[50];

void NW_timer_work(void)
{
	unsigned char i;
	
	OS_ERR      err;
	
	if(STM32_rest_delay)
	{
		STM32_rest_delay--;
		if(STM32_rest_delay==0) reset_STM32();
	}
	
	if(((worktime-LastGetDataT)>=(GDT[0]*60/testdvi))&&(sampled_data_time==0))
	{
		GDT_fun=0;
		sampled_data_time=1;
		LastGetDataT=worktime;
	}
	if(ip_seting)
	{
		sprintf((char *)buf,"AT+HOST=%d.%d.%d.%d,%d\r\n",HostIP[0],HostIP[1],HostIP[2],HostIP[3],*HostPort);//HostIP[0],HostIP[1],HostIP[2],HostIP[3],*HostPort
//		printf("设置IP%s",buf);
		HAL_UART_Transmit_IT(&huart5, buf, strlen((char *)buf));
	}
	
	if(systime[5]==0) //整分钟
	{
//		printf("\r\n一分钟");
		if(((RST[0]==0)||(systime[2]==RST[0]))&&(systime[3]==RST[1])&&(systime[4]==RST[2])) //日 时 分
		{
				reset_STM32();//软件复位
		}
		for(i=0;i<aut_point_group[0];i++)
		{
			if((systime[3]==(aut_get_point[i].h))&&(systime[4]==(aut_get_point[i].m+0)))
			{
				printf("\r\n拍照时间 前端点:%d",aut_get_point[i].p);

				if(++nw_ctl_Q_nub>=nw_ctl_Q_max) nw_ctl_Q_nub=0;
				nw_ctl_Q[nw_ctl_Q_nub].cmd=0xf0;
				nw_ctl_Q[nw_ctl_Q_nub].len=0x01;
			
				nw_ctl_Q[nw_ctl_Q_nub].data[0]=aut_get_point[i].p;			
				
				
				OSQPost((OS_Q*		)&NW_Msg,		
								(void*		)&nw_ctl_Q[nw_ctl_Q_nub],
								(OS_MSG_SIZE)5,
								(OS_OPT		)OS_OPT_POST_FIFO,
								(OS_ERR*	)&err);
			}
		}
	}
}
	
void  NWTask_Contact  ( void * p_arg )//void  NWTask_task( void * p_arg )
{
	OS_ERR      err;
//	CPU_SR_ALLOC();
//	unsigned char mode=0;
	struct nw_ctl *nw_r_post;
	OS_MSG_SIZE*	 size;	
	char i;
	
	SRAM_Test();
	
//	ph_test();
	
//	PUT_PTZ_s("TILT_DOWN");
	
	(void)p_arg;	
		
	retxtime=0xff;
	function_buf[0]=0x22;
	function_buf[1]=0x25;
	function_nub=2;
	
	while (DEF_TRUE)
	{				
		nw_r_post=OSQPend((OS_Q*			)&NW_Msg,   
											(OS_TICK		)1000,		//6000,	//							
											(OS_OPT			)OS_OPT_PEND_BLOCKING,
											(OS_MSG_SIZE*	)&size,		
											(CPU_TS*		)0,
											(OS_ERR*		)&err);
		
		if(err==OS_ERR_NONE)//收到南网消息
		{				
			printf("\r\nM->S %x,%d:%d:%d",nw_r_post->cmd,systime[3],systime[4],systime[5]);

			nw_in(nw_r_post);
			LastTxT=worktime;
		}
		else //超时 (1)//	
		{
//			if((worktime-sentTime)>3600*24) //24小时校时一次
//			{
//				ctlnumber=CheckT;
//			}
			check_retxtime();
			
			
//			printf("\r\ndata_time=%d,GDT=%d,time=%d,GDT_fun=%d,GDT_OK=%d,ctlnumber=%d",sampled_data_time,GDT[0],(worktime-LastGetDataT),GDT_fun,GDT_OK,ctlnumber);

/////////////数据采集////////////////////////////////////////////////////////////////////			
			
			if((sampled_data_time)&&(ctlnumberOld<10))
			{
				if(GDT_fun<function_nub)
				{
					if((GDT_OK)||(ctlnumber<=0x20))
					{
						if(function_buf[GDT_fun]!=0)
						{
							ctlnumber=function_buf[GDT_fun++];
							GDT_OK=0;
							sampled_step=0;
							sampled_data_flag=1;
							SenTimes=5;
						}
						else
						{
							GDT_fun=0;
							sampled_data_time=0;
						}
					}
				}
				else 
				{
					GDT_fun=0;
					sampled_data_time=0;
				}
			}
/////////////////////////////////////////////////////////////////////////////////////		
			
			if(((worktime-LastTxT)>=retxtime)||(sampled_data_flag)) //重发
			{
				LastTxT=worktime;
				nw_sc.cmd=ctlnumber;
				sampled_data_flag=0;
				
				switch(ctlnumber)
				{					
					case 	Contact:			//00	//	开机联络信息	装置开机上送联络信息
						ctlnumberOld=Contact;
						nw_sc.len=sizeof(version);
//						nw_sc.data=version;
						memcpy(nw_sc.data,version,nw_sc.len);
						nw_sent(&nw_sc);
						break;
						
					case  CheckT:				//01	//	校时	该命令分请求和下发两部分，一天一次
						ctlnumberOld=CheckT;
						nw_sc.len=0;//sizeof(systime);
//						nw_sc.data=systime;
						memcpy(nw_sc.data,systime,6);
						nw_sent(&nw_sc);
						sentTime=worktime;
						break;
					
					case  HeartBeat:		//05	//	装置心跳信息	用于主站监测装置上线时间、IP地址和端口号、传输信号强度及蓄电池电压
						ctlnumberOld=HeartBeat;
						nw_sc.len=8;
						memcpy(nw_sc.data,systime,6);
						
						if(check_signel()==0) nw_sc.data[6]=signal;
						else nw_sc.data[6]=0;
					
						if(Get_Volt()==0) nw_sc.data[7]=volt;
						else nw_sc.data[7]=0;
					
//						nw_sc.data=nw_sc.data;
						nw_sent(&nw_sc);
						break;
					
					case 	0x22:			//上传采集数据
						if(SenTimes)
						{
							sampled_data_sent_22(sampled_step,sampled_data_nub);
							SenTimes--;
						}
						if(SenTimes==0)//发完,回到心跳流程
						{
							ctlnumber=ctlnumberOld;
							GDT_OK=1;
						}
						
						break;
					case 	0x25:			//上传采集数据
						if(SenTimes)
						{
							sampled_data_sent_25(sampled_data_nub);
							SenTimes--;
						}
						if(SenTimes==0)//发完,回到心跳流程
						{
							ctlnumber=ctlnumberOld;
							GDT_OK=1;
						}
						break;
					
					case  0x84:
						if(SenTimes)
						{
							nw_sent(&nw_sc);
							SenTimes--;
						}
						if(SenTimes==0)//发完,回到心跳流程
						{
							ctlnumber=ctlnumberOld;
						}
					break;
						
					case  0x86:
						if(SenTimes)
						{
							nw_sc.cmd=0x86;//数据结束
							nw_sc.len=2;			
							nw_sc.data[0]=1;//通道号	
							nw_sc.data[1]=0;//预置位号
							nw_sent(&nw_sc);
							SenTimes--;
						}
						if(SenTimes==0) //发完,回到心跳流程
						{
							ctlnumber=ctlnumberOld;
						}
					break;
					
					default:
						
					
					break;
				}
			}
		}	
	}
}

void rev_4g_data(uint8_t *rev_data,uint16_t len)
{
	unsigned int dlen,i,rlen; 
	OS_ERR      err; 
	uint8_t rev_ok;
	uint8_t *ptr;
	
	rev_ok=0;
	rlen=len;
	ptr=rev_data;
	
//	printf("\r\n接收 nw:");
//	for(i=0;i<len;i++) printf("%02x",ptr[i]); 

	while(rlen)
	{
		if (0 == memcmp(ptr, STAA, 7))
		{
			
			dlen=ptr[8];
			dlen=(dlen<<8)+ptr[9];

			if((checksum_4g(&ptr[1],dlen+6+1+2)==ptr[dlen+10])&&(ptr[dlen+11]==0x16))
			{
//				rev_cmd(ptr[7],&ptr[10],dlen);
				
				
				if(dlen<=50)
				{
					if(++nw_ctl_Q_nub>=nw_ctl_Q_max) nw_ctl_Q_nub=0;
					nw_ctl_Q[nw_ctl_Q_nub].cmd=ptr[7];
					nw_ctl_Q[nw_ctl_Q_nub].len=dlen;
					for(i=0;i<dlen;i++) nw_ctl_Q[nw_ctl_Q_nub].data[i]=ptr[i+10];		
						//			nw_ctl_Q[nw_ctl_Q_nub].data=ptr;
	//			printf("\r\nm-s %x,%d:%d:%d",nw_ctl_Q[nw_ctl_Q_nub].cmd,systime[3],systime[4],systime[5]);
				
	//			printf("\r\n接收 nw:");					
	//			for(i=7;i<dlong+12;i++) printf("%x,",ptr[i]);				
					OSQPost((OS_Q*		)&NW_Msg,		
									(void*		)&nw_ctl_Q[nw_ctl_Q_nub],
									(OS_MSG_SIZE)sizeof(struct nw_ctl),
									(OS_OPT		)OS_OPT_POST_FIFO,
									(OS_ERR*	)&err);
				}
				else
				{
					OSQPost((OS_Q*		)&NW_Msg,		
									(void*		)&ptr[7],
									(OS_MSG_SIZE)sizeof(struct nw_ctl),
									(OS_OPT		)OS_OPT_POST_FIFO,
									(OS_ERR*	)&err);
				}

				rev_ok=1;
				ptr=ptr+(dlen+12);
								
				if(rlen>=dlen+12) rlen -=(dlen+12);
				else rlen=0;
			}
			else rlen=0;
		}	
		else rlen=0;
	}
	
	if(rev_ok==0)
	{
		printf("\r\nm->s data err");
	}
	
}

#define		NW_REV_Task_SIZE                     512
#define		NW_REV_Task_PRIO                         9
OS_TCB		NW_REV_Task_TCB;															//任务控制块
CPU_STK		NW_REV_Task_Stk [ NW_REV_Task_SIZE ];	//任务堆栈

#define		NW_SENT_Task_SIZE                     512
#define		NW_SENT_Task_PRIO                         9
OS_TCB		NW_SENT_Task_TCB;															//任务控制块
CPU_STK		NW_SENT_Task_Stk [ NW_SENT_Task_SIZE ];	//任务堆栈


void NW_REV_Task  (void * p_arg );
void NW_SENT_Task (void *p_arg);

void Ur3TaskStart (void *p_arg);
void Ur5TaskStart (void *p_arg);

OS_SEM read_RSSI;

void NWTaskStart (void *p_arg)
{
	OS_ERR      err;
	(void)p_arg;
//	signed char x;
	
//	Ur3TaskStart (0);
	Ur5TaskStart (0);
	
//	x=-32;
//	printf("\r\n<pan>%d</pan>\r\n",x);
	
	//		多值信号量控制块指针		//多值信号量名称 //资源数目或事件是否发生标志 //返回错误类型
	OSSemCreate (&read_RSSI, "read_RSSI",  0, &err); //3 	
//	//多值信号量控制块指针	//选项	 //返回错误类型
//			OSSemPost (&read_RSSI, OS_OPT_POST_FIFO, &err);    //处理接收到数据帧 
	
//	//多值信号量控制块指针	//选项	 //返回错误类型
//			OSSemPost (&read_RSSI, OS_OPT_POST_NO_SCHED, &err);    //处理接收到数据帧 
	
////								多值信号量指针	//等待超时时间	//选项 //等到信号量时的时间戳 //返回错误类型
//		OSSemPend (&read_RSSI, 2000, OS_OPT_PEND_BLOCKING,0, &err); 
	
#if 0		
	OSMutexCreate ((OS_MUTEX  *)&RX_4G,           //指向信号量变量的指针
                   (CPU_CHAR  *)"RX For 4G", //信号量的名字
                   (OS_ERR    *)&err);            //错误类型

	
	 OSSemCreate((OS_SEM      *)&RX_4G_AT,    //指向信号量变量的指针
               (CPU_CHAR    *)"urt5_AT",    //信号量的名字
               (OS_SEM_CTR   )0,             //表示现有资源数目
               (OS_ERR      *)&err);         //错误类型
	
#endif	
	OSQCreate ((OS_Q*		)&NW_Msg,	//消息队列
                (CPU_CHAR*	)"NW_Msg",	//消息队列名称
                (OS_MSG_QTY	)3,	//消息队列长度，这里设置为1
                (OS_ERR*	)&err);		//错误码
				

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




