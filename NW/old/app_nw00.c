
#include "bsp.h"
//#include <stdio.h>
//#include <string.h>
//unsigned char start_and_device_number[7]={0x68,0x43,0x43,0x30,0x31,0x39,0x36};

#define	Contact								0x00	//	开机联络信息	装置开机上送联络信息
#define CheckT								0x01	//	校时	该命令分请求和下发两部分
#define SetPassword 					0x02	//	设置装置密码	装置出厂密码：字符：‘1234’（31H32H33H34H）
#define Pcfg 									0x03	//	主站下发参数配置	该指令要求数据采集装置接收到该命令后原命令返回
#define HeartBeat 						0x05	//	装置心跳信息	用于主站监测装置上线时间、IP地址和端口号、传输信号强度及蓄电池电压
#define chang_ip	 						0x06	//	更改主站IP地址、端口号和卡号	
#define check_ip 							0x07	//	查询主站IP地址、端口号和卡号	
#define ResetDev	 						0x08	//	装置复位	主站对装置进行复位
#define waikup_device_m				0x09	//	短信唤醒	主站以短信方式唤醒休眠状态的装置
#define check_device_cfg 			0x0A	//	查询装置配置参数	
#define device_function_cfg		0x0B	//	装置功能配置	
#define device_Sleep 					0x0C	//	装置休眠	
#define check_device_time 		0x0D	//	查询装置设备时间	
#define sent_msg					 		0x0E	//	发送确认短信
#define request_data 					0x21	//	主站请求装置数据	主站请求采集装置采集数据并立即上送
#define request_la_qj 				0x22	//	上传导地线拉力及倾角数据	
#define request_qx 						0x25	//	上传气象数据	

/*
#define  0x26 //	上传导线温度、导线电流数据	
#define  0x27 //	上传杆塔振动数据	
#define  0x29 //	上传舞动振幅频率数据	
#define  0x2A //	上传杆塔倾斜数据	
#define  0x2B //	上传导线微风振动数据	
#define  0x2C //	上传综合防盗数据	
#define  0x2D //	上送山火报警数据	
#define  0x2E //	上送大风舞动报警数据	
#define  0x30 //	上传设备故障信息	
#define  0x31 //	主站请求微风振动动态数据	
#define  0x32 //	微风振动动态数据上送	
#define  0x33 //	微风振动动态数据上送结束标记	
#define  0x34 //	微风振动动态数据补报下发	
#define  0x35 //	主站请求舞动动态数据	
#define  0x36 //	舞动动态数据上送	
#define  0x37 //	舞动动态数据上送结束标记	
#define  0x38 //	舞动动态数据补包下发	
#define  0x39 //	主站请求拉力及偏角动态数据	
#define  0x3A //	拉力及偏角动态数据上送	
#define  0x3B //	拉力及偏角动态数据上送结束标记	
#define  0x3C //	拉力及偏角动态数据补包下发	
#define  0x41 //	上传污秽数据	
#define  0x42 //	上传导线弧垂数据	
#define  0x43 //	上传电缆温度数据	
#define  0x44 //	上传电缆护层接地电流数据	
#define  0x45 //	上传故障定位数据	
#define  0x46 //	上传电缆局放数据	
#define  0x47 //	上传电缆局放谱图数据	
#define  0x60 //	主站设置故障测距终端参数	
#define  0x6A //	主站查询故障测距终端参数	
#define  0x61 //	上传故障测距终端工况数据	
#define  0x62 //	终端装置向主站请求上传工频故障波形数据	
#define  0x63 //	上传工频故障波形数据	
#define  0x64 //	工频故障波形数据上传结束标志	
#define  0x65 //	主站向终端发送工频故障波形数据补包	
#define  0x66 //	终端装置向主站请求上传故障行波波形数据	
#define  0x67 //	上传故障行波波形数据	
#define  0x68 //	故障行波波形数据上传结束标志	
#define  0x69 //	主站向终端发送行波波形数据补包	

//#define  0x以下为文件传输部分
71 //	主站查询装置文件列表	
72 //	主站请求装置上送文件	
73 //	装置请求上送文件	
74 //	文件上送	
75 //	文件上送结束标记	
76 //	文件补包数据下发	
以下为图像视频监测部分
81 //	图像采集参数配置	主站下发采集装置的色彩选择、图像宽高＋亮度＋对比度＋饱和度等信息
82 //	拍照时间表设置	设置采集装置自动拍摄照片的时间、位置
83 //	主站请求拍摄照片	在需手动请求拍摄照片时使用
84 //	采集装置请求上送照片	采集装置采集完图像数据后向主站发送上送图像数据请求，主站原命令返回
85 //	图像数据上送	上送图像拆分后的N个数据包
86 //	图像数据上送结束标记	用于主站判断图像数据是否上送完毕
87 //	补包数据下发	主站接到86H指令后，进行数据整理，将未收到的包号下发给采集装置
88 //	摄像机远程调节	上、下、左、右、远、近调节，预置位远程设置、更改
89 //	启动摄像视频传输	
8A //	终止摄像视频传输	
8B //	查询拍照时间表	
8C //	视频采集参数配置	
8D //	视频采集参数查询	
8E //	OSD参数配置	
8F //	OSD参数查询	
90 //	录像策略参数配置	
91 //	录像策略参数查询	
92 //	通道录像状态查询	
93 //	主站请求拍摄短视频	
94 //	采集终端请求上送短视频	
95 //	短视频数据上送	
96 //	短视频数据上送结束标记	
97 //	短视频补包数据下发	
98 //	主站查询终端录像文件数目	
99 //	主站查询终端录像文件列表	
9A //	主站请求进行录像文件回放	
9B //	主站请求进行录像文件回放控制	
9C //	主站请求进行录像文件回放断开	
9D //	主站请求进行录像下载	
9E //	主站请求进行录像文件下载断开	
A0 //	启动摄像视频传输	
A1 //	终止摄像视频传输	
A2 //	主站请求与终端进行语音广播	
A3 //	主站请求与终端断开语音广播	
A4 //	塔基防范参数配置	
A5 //	塔基防范参数查询	
A6 //	线路检测参数配置	
A7 //	线路检测参数查询	
A8 //	线路巡检参数配置	
A9 //	线路巡检参数查询	
AA //	智能分析告警上报	
AB //	联动参数配置	
AC //	联动参数查询	
AD //	传感器数据视频叠加参数配置	
AE //	传感器数据视频叠加参数查询	
B1 //	摄像机3D控球调节	
*/



//#define  AppTaskNWR_SIZE                     512
//#define  AppTaskNWR_PRIO                         9
//OS_TCB   AppTaskNWR_TCB;															//任务控制块
//CPU_STK  AppTaskNWR_Stk [ AppTaskNWR_SIZE ];	//任务堆栈
//void  AppTaskNWR  ( void * p_arg );
//void  sent_to_nw(unsigned char command,unsigned char * t_data,unsigned int dlong);
//struct nw_ctl
//{
//	unsigned char cmd;
//	unsigned int len;
//	unsigned char *data;   
//};

void  nw_sent(struct nw_ctl *t_data);
void rev_err(unsigned char ctl,unsigned char data);

#define  AppTaskNWT_SIZE                     512
#define  AppTaskNWT_PRIO                         9
OS_TCB   AppTaskNWT_TCB;															//任务控制块
CPU_STK  AppTaskNWT_Stk [ AppTaskNWT_SIZE ];	//任务堆栈
void  AppTaskNWT  ( void * p_arg );

#define  NWTask_Contact_SIZE                     512
#define  NWTask_Contact_PRIO                         9
OS_TCB   NWTask_Contact_TCB;															//任务控制块
CPU_STK  NWTask_Contact_Stk [ NWTask_Contact_SIZE ];	//任务堆栈
void  NWTask_Contact  ( void * p_arg );

#define  NWTask_CheckT_SIZE                     512
#define  NWTask_CheckT_PRIO                         9
OS_TCB   NWTask_CheckT_TCB;															//任务控制块
CPU_STK  NWTask_CheckT_Stk [ NWTask_CheckT_SIZE ];	//任务堆栈
void  NWTask_CheckT  ( void * p_arg );

#define  NWTask_HeartBeat_SIZE                     512
#define  NWTask_HeartBeat_PRIO                         9
OS_TCB   NWTask_HeartBeat_TCB;															//任务控制块
CPU_STK  NWTask_HeartBeat_Stk [ NWTask_HeartBeat_SIZE ];	//任务堆栈
void  NWTask_HeartBeat  ( void * p_arg );

#define  NWTask_Contact_SIZE                     512
#define  NWTask_Contact_PRIO                         9
OS_TCB   NWTask_Contact_TCB;															//任务控制块
CPU_STK  NWTask_Contact_Stk [ NWTask_Contact_SIZE ];	//任务堆栈
void  NWTask_Contact  ( void * p_arg );


OS_MUTEX RX_4G;                         //声明互斥信号量
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
								 

unsigned char command_4g=0xff;
unsigned int 	dlong_4g;
//unsigned char rdata_4g[100];

unsigned char version[2]={3,0};
//unsigned char systime[6]={0,0,0,0,0,0}; //年 月 日 时 分 秒
unsigned char ContactOK=0,CheckTOK=0;



OS_Q NW_REV_Msg;				//定义一个消息队列，用于按键消息传递，模拟消息邮箱

struct nw_ctl nw_r,nw_s;
unsigned char nw_r_data[100];  

//	密码	4字节
//	心跳间隔	1字节
//	采集间隔	2字节
//	休眠时长	2字节
//	在线时长	2字节
//	硬件重启时间点	3字节
//	密文验证码	4字节
unsigned char Password[4]={'1','2','3','4'};//
unsigned char Heabtime=60;//心跳间隔：装置心跳信息发送间隔，单位分钟，出厂配置应为1分钟;
unsigned short getdatetime=20;//采集间隔：即每隔多少分钟采样一次（采集间隔与拍照时间无关），单位分钟，出厂配置应为20分钟；
unsigned short sleeptime=0;//休眠时长：数据采集功能关闭或通信设备休眠时间，该时间内可支持短信或网络唤醒；单位分钟，若为0则装置不休眠；
unsigned short onlinetime=10;		//在线时长：通信设备保持数据采集及网络通信设备在线时间；单位分钟；
unsigned char resettime[3];	//3bye 硬件重启时间点：为保证装置软件可靠运行装置应支持定时重启。
unsigned short checkword[4]={'1','2','3','4'};	//密文认证：4字节，装置初始为字符‘1234’（31H32H33H34H），为确认装置数据的正确性，防止非法用户恶意欺骗服务器。
//该密文用于防止非法装置用户的数据被主站认可，安装时装置设定默认密文，上塔安装完成后，
//主站下发指令修改该装置密文，仅装置主站记录的密文一致时视该数据合法有效，否则屏蔽。




//char localpassw[50]="1234"

//void  NWTask_Contact  ( void * p_arg )
//{
//	OS_ERR      err;
//	CPU_SR_ALLOC();
//		
////	unsigned char t_data[2]={3,0};
////	unsigned int dlong; 
//	unsigned char mode=0;
//	
//	(void)p_arg;
////command_4g=uart3_t[7];
////dlong_4g=dlong;
////for(i=0;i<dlong;i++) rdata_4g[i]=uart3_t[i+10];				
//	while (DEF_TRUE)
//	{		
//		if(mode==0)
//		{
//			OS_CRITICAL_ENTER();                              //进入临界段，避免串口打印被打断
////				sent_to_nw(Contact,version,sizeof(version));
//			OS_CRITICAL_EXIT();                               //退出临界段
//			
//			OSTaskSemPend ((OS_TICK   )6000,                     //等待6s
//										 (OS_OPT    )OS_OPT_PEND_BLOCKING,  //如果信号量不可用就等待
//										 (CPU_TS   *)0,                   	//获取信号量被发布的时间戳
//										 (OS_ERR   *)&err);                 //返回错误类型	

////	OSTaskCreate(	(OS_TCB 	* )&StartTaskTCB,		//任务控制块
////				 				(CPU_CHAR	* )"start task", 		//任务名字
////								(OS_TASK_PTR )start_task, 			//任务函数
////								(void		* )0,					//传递给任务函数的参数
////								(OS_PRIO	  )START_TASK_PRIO,     //任务优先级
////								(CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
////								(CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
////								(CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
////								(OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
////								(OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
////								(void   	* )0,					//用户补充的存储区
////								(OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
////								(OS_ERR 	* )&err);				//存放该函数错误时的返回值

//			if(err==OS_ERR_NONE)
//			{
//				Pend_4G_Mutex();

//				if((command_4g==Contact)&&(dlong_4g==sizeof(version)))
//				{
//					mode=1;
//					ContactOK=1;
//					OSTaskSemPost((OS_TCB *)&NWTask_CheckT_TCB,	(OS_OPT)OS_OPT_POST_NONE,(OS_ERR *)&err);// 开始校时
//				}
//				Post_4G_Mutex();
//			}
//		}
//		else
//		{
//			RUN_LED1_Toggle();
//			
//			OSTaskSemPend ((OS_TICK   )0,                     //无期限等待
//										 (OS_OPT    )OS_OPT_PEND_BLOCKING,  //如果信号量不可用就等待
//										 (CPU_TS   *)0,                   	//获取信号量被发布的时间戳
//										 (OS_ERR   *)&err);                 //返回错误类型	
//			
//			Pend_4G_Mutex();

//			if((command_4g==Contact)&&(dlong_4g==0))
//			{
//				mode=0;
//			}
//			Post_4G_Mutex();
//		}
//	}
//}

unsigned int retxtime=0; //初始值60分钟，测试版本6s
unsigned int ctlnumber=0;
unsigned char sentbuf[50],signal=63,volt=121;


void change_IP(void)
{
}
void reset_STM32(void)
{
}
void wakeup_dev(void)
{
}
void dev_sleep(void)
{
}


unsigned char nw_ip[4];
unsigned short  nw_prot;
unsigned char  nw_phone_num[6];
unsigned char function_buf[20];
void check_function(void)
{
	char i;
		for(i=0;i<strlen((char *)function_buf);i++)
		{
			if(function_buf[i]==0x22)//	导地线拉力及倾角监测功能
			{
			}
			if(function_buf[i]==0x24)//	绝缘子泄漏电流监测功能
			{
			}
			if(function_buf[i]==0x25)//	气象数据监测功能
			{
			}
			if(function_buf[i]==0x26)//	导线温度、电流数据监测功能
			{
			}
			if(function_buf[i]==0x27)//	杆塔振动数据监测功能
			{
			}
			if(function_buf[i]==0x28)//	导线侧倾角监测功能
			{
			}
			if(function_buf[i]==0x29)//	舞动振幅频率监测功能
			{
			}
			if(function_buf[i]==0x2A)//	杆塔倾斜数据监测功能
			{
			}
			if(function_buf[i]==0x2B)//	导线微风震动数据监测功能
			{
			}
			if(function_buf[i]==0x2C)//	综合防盗功能
			{
			}
			if(function_buf[i]==0x2D)//	山火报警功能
			{
			}
			if(function_buf[i]==0x2E)//	大风舞动报警功能
			{
			}
			if(function_buf[i]==0x30)//	设备故障自检功能
			{
			}
			if(function_buf[i]==0x32)//	微风振动动态数据监测功能
			{
			}
			if(function_buf[i]==0x36)//	舞动动态数据监测功能
			{
			}
			if(function_buf[i]==0x41)//	污秽数据监测功能
			{
			}
			if(function_buf[i]==0x42)//	导线弧垂数据监测功能
			{
			}
			if(function_buf[i]==0x43)//	电缆温度数据监测功能
			{
			}
			if(function_buf[i]==0x44)//	电缆护层接地环流数据监测功能
			{
			}
			if(function_buf[i]==0x45)//	电缆故障定位数据监测功能
			{
			}
			if(function_buf[i]==0x46)//	电缆故障局放数据监测功能
			{
			}
			if(function_buf[i]==0x47)//	电缆局放谱图数据传输功能
			{
			}
			if(function_buf[i]==0x73)//	文件传输功能
			{
			}
			if(function_buf[i]==0x84)//	图像监测功能
			{
			}
			if(function_buf[i]==0x85)//	视频监测功能
			{
			}
		}
}




void  NWTask_Contact  ( void * p_arg )//void  NWTask_task( void * p_arg )
{
	OS_ERR      err;
	CPU_SR_ALLOC();
	unsigned char mode=0;
	struct nw_ctl *nw_r_post;
	OS_MSG_SIZE*	 size;	
	unsigned int sentTime,i;
	
	(void)p_arg;	
	
//		nw_r_post=OSQPend((OS_Q*			)&NW_REV_Msg,   
//											(OS_TICK		)retxtime,										
//											(OS_OPT			)OS_OPT_PEND_BLOCKING,
//											(OS_MSG_SIZE*	)&size,		
//											(CPU_TS*		)0,
//											(OS_ERR*		)&err);
//	
//	OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err ); 
	
//	nw_s.len=sizeof(version);
//					nw_s.data=version;
//					nw_sent(&nw_s);
	
	while (DEF_TRUE)
	{				
		nw_r_post=OSQPend((OS_Q*			)&NW_REV_Msg,   
											(OS_TICK		)retxtime,										
											(OS_OPT			)OS_OPT_PEND_BLOCKING,
											(OS_MSG_SIZE*	)&size,		
											(CPU_TS*		)0,
											(OS_ERR*		)&err);
		
//		printf("等到消息");
		if(err==OS_ERR_NONE)
		{
			printf("\r\n等到消息");
			
			printf("\r\nm-s %x,%d:%d:%d",nw_r_post->cmd,systime[3],systime[4],systime[5]);
			
			switch(nw_r_post->cmd)
			{
				case 	0xff:			//4g post
					nw_s.cmd=0;
					nw_s.len=sizeof(version);
					nw_s.data=version;
					nw_sent(&nw_s);
					retxtime=6000; 	//开机联络，1分钟重发
					break;
					
				case 	Contact:			//00	//	开机联络信息	装置开机上送联络信息
					printf("\r\nCont len=:%d",nw_r_post->len);
				
					if((ctlnumber==Contact)&&(nw_r_post->len==2))//sizeof(version) 正在开机联络时，服务器回复开机联络信息
					{
						if((nw_r_post->data[0]==nw_s.data[0])&&(nw_r_post->data[1]==nw_s.data[1]))
						{
							ctlnumber=CheckT;
							nw_s.cmd=ctlnumber;
							nw_s.len=sizeof(systime);
							nw_s.data=systime;
							nw_sent(&nw_s);
							retxtime=6000;		//校时，2分钟重发
							sentTime=worktime;	
						}
					}
					else if(nw_r_post->len==0) //
					{
//						ctlnumber=Contact; //否则认为 服务台主动发开机联络信号
//						mode=1;
					}
					
					retxtime=0; 	// 下一次的时间
					break;
					
				case  CheckT:				//01	//	校时	该命令分请求和下发两部分
//					if(memcmp(sentTime,systime,5)==0)
					if(ctlnumber==Contact)
					{
						if((worktime-sentTime)<20)
						{
							memcpy(systime,nw_r_post->data,6);
							ctlnumber=HeartBeat;
						}
					}
					else
					{
						memcpy(systime,nw_r_post->data,6);
						nw_sent(nw_r_post);
//					ctlnumber=Contact;
//					mode=1;
					}
					retxtime=0;
									
					break;
				case  SetPassword :	//02	//	设置装置密码	装置出厂密码：字符：‘1234’（31H32H33H34H）
					if(memcmp(PSW,nw_r_post->data,4)==0) //localpassw[50]
					{
						memcpy(PSW,&nw_r_post->data[4],6);
						nw_sent(nw_r_post);
					}
					else
					{
						rev_err(SetPassword,0xff);
					}
					break;
					
//unsigned char Password[4]={'1','2','3','4'};//
//unsigned char Heabtime=60;//心跳间隔：装置心跳信息发送间隔，单位分钟，出厂配置应为1分钟;
//unsigned short getdatetime=20;//采集间隔：即每隔多少分钟采样一次（采集间隔与拍照时间无关），单位分钟，出厂配置应为20分钟；
//unsigned short sleeptime=0;//休眠时长：数据采集功能关闭或通信设备休眠时间，该时间内可支持短信或网络唤醒；单位分钟，若为0则装置不休眠；
//unsigned short onlinetime=10;		//在线时长：通信设备保持数据采集及网络通信设备在线时间；单位分钟；
//unsigned int resettime;	//3bye  硬件重启时间点：为保证装置软件可靠运行装置应支持定时重启。
//unsigned short checkword[4]={'1','2','3','4'};	//密文认证：4字节，装置初始为字符‘1234’（31H32H33H34H），为确认装置数据的正确性，防止非法用户恶意欺骗服务器。
////该密文用于防止非法装置用户的数据被主站认可，安装时装置设定默认密文，上塔安装完成后，
////主站下发指令修改该装置密文，仅装置主站记录的密文一致时视该数据合法有效，否则屏蔽。
				case  Pcfg :				//03	//	主站下发参数配置	该指令要求数据采集装置接收到该命令后原命令返回
					if((memcmp(PSW,nw_r_post->data,4)==0)&&(memcmp(checkword,&nw_r_post->data[14],4)==0)) //localpassw[50]
					{
						
						Heabtime=nw_r_post->data[4];
						getdatetime=(nw_r_post->data[5]<<8)+nw_r_post->data[6];
						sleeptime=(nw_r_post->data[7]<<8)+nw_r_post->data[8];
						onlinetime=(nw_r_post->data[9]<<8)+nw_r_post->data[10];
						resettime[0]=nw_r_post->data[11]; //日 0-28
						resettime[1]=nw_r_post->data[12];	//时 0-24
						resettime[2]=nw_r_post->data[13];	//分 0-60
						
						nw_sent(nw_r_post);
					}
					else
					{
						rev_err(Pcfg,0xff);
					}
					break;
				case  HeartBeat:		//05	//	装置心跳信息	用于主站监测装置上线时间、IP地址和端口号、传输信号强度及蓄电池电压
					break;
				case  chang_ip:			//06	//	更改主站IP地址、端口号和卡号	
					if(memcmp(PSW,nw_r_post->data,4)==0)
					{
//密码	主站IP	端口号	主站IP	端口号	主站卡号	主站卡号
//4字节	4字节	2字节	4字节	2字节	6字节	6字节
						if(memcmp(&nw_r_post->data[4],&nw_r_post->data[10],6)==0)
						{
							nw_sent(nw_r_post);
							change_IP();
							reset_STM32();
						}
						else
						{
							rev_err(chang_ip,0);
						}
					}
					else
					{
						rev_err(chang_ip,0xff);
					}
					break;
				case  check_ip:			//07	//	查询主站IP地址、端口号和卡号	
//				if(nw_r_post->len==0)
					{
						nw_s.cmd=check_ip;
						nw_s.len=12;
						
						memcpy(sentbuf,nw_ip,4);
						sentbuf[4]=nw_prot>>8;
						sentbuf[5]=nw_prot;
						memcpy(&sentbuf[6],nw_phone_num,6);
						nw_s.data=sentbuf;
						
						nw_sent(&nw_s);
					}
					break;
				case  ResetDev:			//08	//	装置复位	主站对装置进行复位
					if(memcmp(PSW,nw_r_post->data,4)==0)
					{
						nw_sent(nw_r_post);
						
					}
					else
					{
						rev_err(ResetDev,0xff);
					}
					break;
				case  waikup_device_m://09	//	短信唤醒	主站以短信方式唤醒休眠状态的装置
//					sys_mode=MODE_WORK;
					wakeup_dev();
					break;
				case  check_device_cfg://0A	//	查询装置配置参数
//				if(nw_r_post->len==0)
					{
						nw_s.cmd=check_device_cfg;
//						memcpy(sentbuf,nw_ip,4);
						
//心跳间隔	1字节
//采集间隔	2字节
//休眠时长	2字节
//在线时长	2字节
//硬件重启时间点	3字节
//通道1色彩选择	1字节
//通道1图像大小	1字节
//通道1亮度	1字节
//通道1对比度	1字节
//通道1饱和度	1字节
//通道2色彩选择	1字节
//通道2图像大小	1字节
//通道2亮度	1字节
//通道2对比度	1字节
//通道2饱和度	1字节
//有效功能1	1字节
//有效功能2	1字节
//…….	……
//有效功能N	1字节				
						i=0;
						sentbuf[i++]=Heabtime;
						sentbuf[i++]=getdatetime>>8;
						sentbuf[i++]=getdatetime;
						sentbuf[i++]=sleeptime>>8;
						sentbuf[i++]=sleeptime;
						sentbuf[i++]=onlinetime>>8;
						sentbuf[i++]=onlinetime;
						sentbuf[i++]=resettime[0];
						sentbuf[i++]=resettime[1];
						sentbuf[i++]=resettime[2];
						sentbuf[i++]=1;sentbuf[i++]=1;sentbuf[i++]=60;sentbuf[i++]=60;sentbuf[i++]=60;
						sentbuf[i++]=1;sentbuf[i++]=1;sentbuf[i++]=60;sentbuf[i++]=60;sentbuf[i++]=60;
						memcpy(&sentbuf[i],function_buf,strlen((char *)function_buf));
						nw_s.len=i+strlen((char *)function_buf);
						nw_s.data=sentbuf;
						nw_sent(&nw_s);
					}
					break;
				case  device_function_cfg://0B	//	装置功能配置	
					if(memcmp(PSW,nw_r_post->data,4)==0)
					{
						memcpy(function_buf,&nw_r_post->data[4],nw_r_post->len-4);
						function_buf[nw_r_post->len-4]=0; //结尾给0
					}
					else
					{
						rev_err(device_function_cfg,0xff);
					}
					break;
				case  device_Sleep:	//0C	//	装置休眠	
					dev_sleep();
					break;
				case  check_device_time://0D	//	查询装置设备时间	
					nw_s.cmd=check_device_time;
					nw_s.len=6;
					nw_s.data=systime;
					nw_sent(&nw_s);
					break;
				case  sent_msg:
					nw_sent(nw_r_post);
//				发短信	https://blog.csdn.net/qq_30460905/article/details/81805891
					break;
				case  request_data:	//21	//	主站请求装置数据	主站请求采集装置采集数据并立即上送
					if(nw_r_post->len==0)//上传未成功上传的历史数据，包含历史照片，若装置无历史数据则不上传
					{
						
					}
					else if((nw_r_post->len==2)&&(nw_r_post->data[0]==0xBB)&&(nw_r_post->data[1]==0xBB))//装置立刻采集所有数据（图片除外），完成采集后立刻上传。该次采样不影响原设定采集间隔的执行
					{
						
					}
					nw_sent(nw_r_post);
					break;
				case  request_la_qj://22	//	上传导地线拉力及倾角数据	
					
					break;
				case  request_qx:		//0x25	//	上传气象数据	
					break;
			}
		}
		else //超时 (1)//	
		{
			printf("\r\n消息超时");
//			if((worktime-sentTime)>3600*24) //24小时校时一次
//			{
//				ctlnumber=CheckT;
//			}
			
			if((systime[1]==resettime[0])&&(systime[2]==resettime[1])&&(systime[3]==resettime[2])) //24小时校时一次
			{
					reset_STM32();//软件复位
			}
		
			nw_s.cmd=ctlnumber;
			
			switch(ctlnumber)
			{
				case 	Contact:			//00	//	开机联络信息	装置开机上送联络信息
					nw_s.len=sizeof(version);
					nw_s.data=version;
					nw_sent(&nw_s);
					retxtime=6000; 	//开机联络，1分钟重发
					break;
					
				case  CheckT:				//01	//	校时	该命令分请求和下发两部分，一天一次
					nw_s.len=sizeof(systime);
					nw_s.data=systime;
					nw_sent(&nw_s);
					retxtime=12000;		//校时，2分钟重发
					sentTime=worktime;	
//					if(mode==1)
//					{
//						ctlnumber=HeartBeat;
//					}
//				memcpy(sentTime,systime,6);
					break;
				
				case  SetPassword :	//02	//	设置装置密码	装置出厂密码：字符：‘1234’（31H32H33H34H）
//					memcmp(Password,systime,4)==0) //localpassw[50]
					break;
				
				case  Pcfg :				//03	//	主站下发参数配置	该指令要求数据采集装置接收到该命令后原命令返回
					break;
				
				case  HeartBeat:		//05	//	装置心跳信息	用于主站监测装置上线时间、IP地址和端口号、传输信号强度及蓄电池电压
					nw_s.len=8;
					memcpy(sentbuf,systime,6);
					sentbuf[6]=signal;
					sentbuf[7]=volt;
					nw_s.data=sentbuf;
					nw_sent(&nw_s);
					retxtime=Heabtime;		//校时，2分钟重发
					sentTime=worktime;	
					break;
				
				case  chang_ip:			//06	//	更改主站IP地址、端口号和卡号	
					break;
				
				case  check_ip:			//07	//	查询主站IP地址、端口号和卡号	
					break;
				case  ResetDev:			//08	//	装置复位	主站对装置进行复位
					break;
				case  waikup_device_m://09	//	短信唤醒	主站以短信方式唤醒休眠状态的装置
					break;
				case  check_device_cfg://0A	//	查询装置配置参数	
					break;
				case  device_function_cfg://0B	//	装置功能配置	
					break;
				case  device_Sleep:	//0C	//	装置休眠	
					break;
				case  check_device_time://0D	//	查询装置设备时间	
					break;
				case  request_data:	//21	//	主站请求装置数据	主站请求采集装置采集数据并立即上送
					break;
				case  request_la_qj://22	//	上传导地线拉力及倾角数据	
					break;
				case  request_qx:		//0x25	//	上传气象数据	
					break;
			}
		}
		

		
	}
}





void AppTaskNWS(void * p_arg)
{
	OS_ERR      err;
	(void)p_arg;
	
#if 1		
	OSMutexCreate ((OS_MUTEX  *)&RX_4G,           //指向信号量变量的指针
                   (CPU_CHAR  *)"RX For 4G", //信号量的名字
                   (OS_ERR    *)&err);            //错误类型
#endif
	
	OSQCreate ((OS_Q*		)&NW_REV_Msg,	//消息队列
                (CPU_CHAR*	)"NW_REV_Msg",	//消息队列名称
                (OS_MSG_QTY	)3,	//消息队列长度，这里设置为1
                (OS_ERR*	)&err);		//错误码
				

#if 1	
						 
		/* 创建  开机联络 任务 */
    OSTaskCreate((OS_TCB     *)&NWTask_Contact_TCB,                             //任务控制块地址
                 (CPU_CHAR   *)"NWTask_Contact_Name",                             //任务名称
                 (OS_TASK_PTR ) NWTask_Contact,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) NWTask_Contact_PRIO,                         //任务的优先级
                 (CPU_STK    *)&NWTask_Contact_Stk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) NWTask_Contact_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) NWTask_Contact_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型
#endif					

#if 0
						 
		/* 创建 校时 任务 */
    OSTaskCreate((OS_TCB     *)&NWTask_CheckT_TCB,                             //任务控制块地址
                 (CPU_CHAR   *)"NWTask_CheckT_Name",                             //任务名称
                 (OS_TASK_PTR ) NWTask_CheckT,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) NWTask_CheckT_PRIO,                         //任务的优先级
                 (CPU_STK    *)&NWTask_CheckT_Stk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) NWTask_CheckT_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) NWTask_CheckT_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型
#endif	
								 
#if 0
						 
		/* 创建 心跳 任务 */
    OSTaskCreate((OS_TCB     *)&NWTask_HeartBeat_TCB,                             //任务控制块地址
                 (CPU_CHAR   *)"NWTask_HeartBeat_Name",                             //任务名称
                 (OS_TASK_PTR ) NWTask_HeartBeat,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) NWTask_HeartBeat_PRIO,                         //任务的优先级
                 (CPU_STK    *)&NWTask_HeartBeat_Stk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) NWTask_HeartBeat_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) NWTask_HeartBeat_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型
#endif									 
								 
	
	while (DEF_TRUE)
	{
	
		OSTimeDly ( 6000, OS_OPT_TIME_DLY, & err );	
	}
//	OSTaskDel ( & AppTaskNWS_TCB, & err );                     //删除起始任务本身，该任务不再运行 
	
}

void rev_4g_data(uint8_t *rev_data,uint16_t len)
{
	unsigned int dlong,i; 
	OS_ERR      err; 
	
	//				if((comp(start_and_device_number,rev_data,7)==0))			
	if (0 == memcmp(rev_data, STA,7))
	{
		
		dlong=rev_data[8];
		dlong=(dlong<<8)+rev_data[9];
//if((comp(start_and_device_number,uart3_t,7)==0))
//				{
//					dlong=uart3_t[8];
//					dlong=(dlong<<8)+uart3_t[9];
//					
//					if((checksum_4g(&uart3_t[1],6+1+2+dlong)==uart3_t[dlong+10])&&(uart3_t[dlong+11]==0x16))
//					{
//						Pend_4G_Mutex();
//						
//						command_4g=uart3_t[7];
//						dlong_4g=dlong;
//						for(i=0;i<dlong;i++) rdata_4g[i]=uart3_t[i+10];			
//						
//						RX_4G_Post();
//						
//						Post_4G_Mutex();
//					}	
//				}	
		if((checksum_4g(&rev_data[1],dlong+6+1+2)==rev_data[dlong+10])&&(rev_data[dlong+11]==0x16))
		{
			
			nw_r.cmd=rev_data[7];
			nw_r.len=dlong;
		
			for(i=0;i<dlong;i++) rev_data[i]=rev_data[i+10];			
			nw_r.data=rev_data;
//			printf("\r\nm-s %x,%d:%d:%d",nw_r.cmd,systime[3],systime[4],systime[5]);
			
			OSQPost((OS_Q*		)&NW_REV_Msg,		
							(void*		)&nw_r,
							(OS_MSG_SIZE)1,
							(OS_OPT		)OS_OPT_POST_FIFO,
							(OS_ERR*	)&err);
							
			printf("\r\nm-s %x,%x",nw_r.cmd,nw_r.len);
		}
		else
		{
			printf("\r\nm-s data err");
		}
	}	
}
void rev_4g_cmmd(uint8_t *rev_data,uint16_t len)
{
	OS_ERR      err; 
	
	if (0 == memcmp("\r\n+CPIN: READY", rev_data, sizeof("\r\n+CPIN: READY") - 1))
	{
		nw_r.cmd=0xff; //4g Post 
		nw_r.len=0;
		OSQPost((OS_Q*		)&NW_REV_Msg,		
							(void*		)&nw_r,
							(OS_MSG_SIZE)1,
							(OS_OPT		)OS_OPT_POST_FIFO,
							(OS_ERR*	)&err);
							
		printf("\r\n4g-s:Post READY");			
	}
	else
	{
		printf("\r\n4g-s:%s",rev_data);
	}
}
unsigned char checksum_4g(unsigned char* a,unsigned int n)
{
	unsigned char checksum;
	unsigned int i;
	
	checksum=0;
	for(i=0;i<n;i++)
	{
		checksum += a[i];
	}
	checksum=~checksum;
	return checksum;	
}

//unsigned char cmu[]={0x68,0x43,0x43,0x30,0x31,0x39,0x36,0x00,0x00,0x02,0x03,0x00};
//unsigned char start_and_series[]={0x68,0x43,0x43,0x30,0x31,0x39,0x36,0x00,0x00,0x02,0x03,0x00};
//unsigned char nw_sent_data[72]={0x68,0x43,0x43,0x30,0x31,0x39,0x36,0,0,0,0,0};
//unsigned char device_number[6]={0x43,0x43,0x30,0x31,0x39,0x36};
//unsigned char start_and_device_number[7]={0x68,0x43,0x43,0x30,0x31,0x39,0x36};

//typedef struct
//{
//	unsigned char cmd;
//	unsigned int dlong;
//	unsigned char *p;   
//}nw_a;



void  nw_sent(struct nw_ctl *t_data) //unsigned char command,unsigned char * t_data,unsigned int dlong
{
	unsigned char sent[100],i;
	CPU_SR_ALLOC();
	
	memcpy(sent,STA, 7);
	sent[7]=t_data->cmd;
	sent[8]=(t_data->len>>8);
	sent[9]=t_data->len;
	
	for(i=0;i<t_data->len;i++)
	{
		sent[10+i]=t_data->data[i];
	}
	sent[10+t_data->len]=checksum_4g(&sent[1],9+t_data->len);
	sent[11+t_data->len]=0x16;
	
	OS_CRITICAL_ENTER();                             //进入临界段
	HAL_UART_Transmit(&huart3, sent, (12+t_data->len),100);
	USART_PRINTF_FLAG=P_MODE_huart1;
	printf("\r\ns-m %x,%d:%d:%d",t_data->cmd,systime[3],systime[4],systime[5]);
	
	HAL_UART_Transmit_IT(&huart1, uart1_t, strlen((char *)uart1_t));
	OS_CRITICAL_EXIT();                          //临界段
	
//	memcpy(sent,start_and_device_number, sizeof(start_and_device_number));
}

void rev_err(unsigned char ctl,unsigned char data)
{
	unsigned char sent[14];
	CPU_SR_ALLOC();
	memcpy(sent,STA, 7);
	sent[7]=ctl;
	sent[8]=0;
	sent[9]=2;
	sent[10]=data;
	sent[11]=data;
	sent[12]=checksum_4g(&sent[1],11);
	sent[13]=0x16;
	OS_CRITICAL_ENTER();                             //进入临界段
	HAL_UART_Transmit(&huart3, sent, (14),100);
	
	printf("\r\ne-m %x,%d:%d:%d",ctl,systime[3],systime[4],systime[5]);
	OS_CRITICAL_EXIT();                          //临界段
}


