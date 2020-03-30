

#include "global.h"


void camera_on(char ch);
void camera_off(char ch);

int GET_Streaming_s(void);
int sent_to_camera(char *sentbuf,char *method,char *uri,char *str,char *body);
int PTZ(char *command,char *mode,char speed,int waitime);
int PUT_PTZ_s(char chanle,const char *command);
int get_photo(void);
int get_photo_s(void);
int PTZCtrl_continuous(char chanle,signed char *x,signed char *y,signed char *z);
int PTZCtrl_continuous_s(char chanle,signed char *x,signed char *y,signed char *z);
int PTZCtrl_presets(char chanle,char presetN);
int PTZCtrl_presets_s(char chanle,char presetN);
int PTZCtrl_presets_goto(char chanle,char presetN);
int PTZCtrl_presets_goto_s(char chanle,char presetN);
int PTZ_Streaming(uint8_t hWidthxHeight);
int PTZ_Streaming_s(uint8_t hWidthxHeight);

int PTZ_Image_s(signed char brightnessLevel,signed char contrastLevel,signed char saturationLevel);

void  nw_sent_88(uint8_t *t_data);

uint8_t preset_now[2],ph_step;

int ph_get(uint8_t preset)
{
	OS_ERR      err;
	int hk_err;
	uint8_t ph_get_ok=0;
	
	if((preset_now[0]!=preset)&&(preset != 0))
	{
		hk_err=PTZCtrl_presets_goto_s(1,preset);	//摄像机调节到指定预置点
		if(hk_err==0) preset_now[0]=preset;
		else preset_now[0]=0;
	}
	
	if(get_photo_s()==0)
	{
		while((senttask_Asim & data_flag84_1)==1) 
		{
			
		}
		senttask_Asim |= data_flag84_1;		//请求上传
	}		
}

extern unsigned char WidthxHeight,bLevel,cLevel,sLevel;

void  HK_Task_ch1 ( void * p_arg )
{
	OS_ERR         err;
	OS_MSG_SIZE    msg_size;
	CPU_TS         ts;
	CPU_INT32U     cpu_clk_freq;
	CPU_SR_ALLOC();
	signed char x,y,z;
	
	uint8_t * pMsg;
	
	uint8_t hMsg[3]={1,0,0}; //固定通道1
	int hk_err;
	
	(void)p_arg;

					 
//  cpu_clk_freq = BSP_CPU_ClkFreq();                 //获取CPU时钟，时间戳是以该时钟计数

	
	while (DEF_TRUE) {                                           //任务体
		
		pMsg=OSQPend((OS_Q*			)&hktask_Asim[0],   
											(OS_TICK		)1000,		//6000,	//							
											(OS_OPT			)OS_OPT_PEND_BLOCKING,
											(OS_MSG_SIZE*	)&msg_size,		
											(CPU_TS*		)0,
											(OS_ERR*		)&err);
		
		
		
		if(err==OS_ERR_NONE)//收到南网消息
		{
			if(pMsg[0] == 0x81)
			{
				if(msg_size>=5)
				{
					WidthxHeight=pMsg[1];
					printf("WidthxHeight=%d",pMsg[1]);
					PTZ_Streaming_s(pMsg[1]);
					bLevel=pMsg[2];
					cLevel=pMsg[3];
					sLevel=pMsg[4];
					PTZ_Image_s(pMsg[2],pMsg[3],pMsg[4]);
					senttask_Asim |= data_flag81_1;		
				}
			}
			else if(pMsg[0] == 0x83)
			{
				ph_get(hMsg[2]); //预置位拍照
			}
			else  if(pMsg[0]<=62)	//88指令
			{
				hMsg[1]=pMsg[0]; hMsg[2]=pMsg[1];
				hk_err=0;
				
				switch(hMsg[1])
				{
					case 1:camera_on(hMsg[0]);break;
					case 10:camera_off(hMsg[0]);break;
					case 2:
			//			while(PTZCtrl_presets_goto(chanle,ctl_dat[2]) !=0);//摄像机调节到指定预置点
						hk_err=PTZCtrl_presets_goto_s(hMsg[0],pMsg[2]);	//摄像机调节到指定预置点
					break;
					case 3:
						x=0;y=28;
						hk_err=PTZCtrl_continuous_s(hMsg[0],&x,&y,0);break;
					case 4:
						x=0;y=-28;
						hk_err=PTZCtrl_continuous_s(hMsg[0],&x,&y,0);break;
					case 5:
						x=-28;y=0;
						hk_err=PTZCtrl_continuous_s(hMsg[0],&x,&y,0);break;
					case 6:
						x=28;y=0;
						hk_err=PTZCtrl_continuous_s(hMsg[0],&x,&y,0);break;
			//			PUT_PTZ_s(chanle,PTZCMD[ctl_dat[1]-3]);break;
					
					case 7://焦距向远方调节1个单位（镜头变倍放大）
					z=-28;
					hk_err=PTZCtrl_continuous_s(hMsg[0],0,0,&z);
					break;
					
					case 8://焦距向近处调节1个单位（镜头变倍缩小）
					z=28;
					hk_err=PTZCtrl_continuous_s(hMsg[0],0,0,&z);
					break;
					
					case 9://保存当前位置为某预置点
			//		PTZCtrl_presets(chanle,ctl_dat[2]);//保存当前位置为某预置点
					hk_err=PTZCtrl_presets_s(hMsg[0],pMsg[2]);//保存当前位置为某预置点
					break;
					
					case 11://光圈放大1个单位
				
					break;
				}
				if(hk_err==0) nw_sent_88(hMsg); //原命令返回
			}
			
		}
				
	}
}

#define  http_hk_thread_SIZE                     600
#define  http_hk_thread_PRIO                         6
OS_TCB   http_hk_thread_TCB;															//任务控制块
CPU_STK  http_hk_thread_Stk [ http_hk_thread_SIZE ];	//任务堆栈

#define  HK_Task_ch1_SIZE                     600
#define  HK_Task_ch1_PRIO                         6
OS_TCB   HK_Task_ch1_TCB;															//任务控制块
CPU_STK  HK_Task_ch1_Stk [ HK_Task_ch1_SIZE ];	//任务堆栈

void http_hk_thread(void *arg);	
extern OS_SEM http_rev_sem,http_200_sem,http_401_sem,http_data_sem;

void HKTaskStart (void *p_arg)
{
	OS_ERR      err;
	
//		多值信号量控制块指针		//多值信号量名称 //资源数目或事件是否发生标志 //返回错误类型
	OSSemCreate (&http_rev_sem, "http_rev_sem",  0, &err); //3 	

//		多值信号量控制块指针		//多值信号量名称 //资源数目或事件是否发生标志 //返回错误类型
	OSSemCreate (&http_200_sem, "http_200_sem",  0, &err); //3 	
	
//		多值信号量控制块指针		//多值信号量名称 //资源数目或事件是否发生标志 //返回错误类型
	OSSemCreate (&http_data_sem, "http_data_sem",  0, &err); //3 	

//消息队列	 //消息队列名称 //消息队列名称  //消息队列长度 //错误码
	OSQCreate (&hktask_Asim[0],"hktask_Asim[0]",100,&err);	
	
	
	/* 创建 任务 */
    OSTaskCreate((OS_TCB     *)&http_hk_thread_TCB,                             //任务控制块地址
                 (CPU_CHAR   *)"http_hk_thread",                             //任务名称
                 (OS_TASK_PTR ) http_hk_thread,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     )	http_hk_thread_PRIO,                         //任务的优先级
                 (CPU_STK    *)&http_hk_thread_Stk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) http_hk_thread_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) http_hk_thread_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型

								 
			/* 创建 任务 */
    OSTaskCreate((OS_TCB     *)&HK_Task_ch1_TCB,                             //任务控制块地址
                 (CPU_CHAR   *)"HK_Task_ch1",                             //任务名称
                 (OS_TASK_PTR ) HK_Task_ch1,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     )	HK_Task_ch1_PRIO,                         //任务的优先级
                 (CPU_STK    *)&HK_Task_ch1_Stk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) HK_Task_ch1_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) HK_Task_ch1_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型						 

}


