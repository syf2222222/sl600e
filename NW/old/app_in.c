
#include "bsp.h"
#include "nw.h"
#include "app_nw.h"
#include "sram.h"
//#include "camera.h"
#include "FM25V10.h"

unsigned short pk_nb,tt_pk_nb;
unsigned int tt_len;
void nw_ctrl_hk(uint8_t *ctl_dat);
unsigned char WidthxHeight,STM32_rest_delay;

int PUT_PTZ_s(char chanle,const char *command);
int GET_Image_s(void);
int GET_Streaming_s(void);
int get_photo_s(void);
int PTZ_Image_s(signed char brightnessLevel,signed char contrastLevel,signed char saturationLevel);
int PTZ_Streaming_s(uint8_t hWidthxHeight);


extern OS_TCB   nw_ctr_hk_TCB;															//任务控制块
extern OS_TCB   nw_get_photo_TCB;															//任务控制块
//uint8_t nw_post_nb=0;
extern uint8_t GDT_OK,Streaming_OK,sampled_data_time;
//uint8_t GDT_OK=1,GDT_fun=0,sampled_data_time=0;

extern unsigned char bLevel,cLevel,sLevel;
	
void ph_test(void)
{
	OS_ERR      err;
	tt_len=((PH_LEN[0])<<16) +PH_LEN[1];
	if(tt_len>100000) tt_len=100000;
	tt_pk_nb=(tt_len>>10); //1024一次
	if(tt_len&0x3ff) tt_pk_nb +=1;
	
	printf("\r\n准备发照片%d包,总长%d,",tt_pk_nb,tt_len);
	for(pk_nb=1;pk_nb<=tt_pk_nb;pk_nb++)
	{
		if(pk_nb==tt_pk_nb) nw_sent_ph(PH_DATA+((pk_nb-1)*1024),tt_len-((pk_nb-1)*1024),pk_nb);
		else	nw_sent_ph(PH_DATA+((pk_nb-1)*1024),1024,pk_nb);
		OSTimeDly ( 100, OS_OPT_TIME_DLY, & err ); 
	}
}

#define		pk_len		1024 //1460
#define		len_max		200000

//ctl_dat[0]-通道  ctl_dat[1]-指令  ctl_dat[2]-参数

//struct nw_ctl nw_to_hk[10];
int PTZCtrl_presets_goto_s(char chanle,char presetN);
	
void nw_get_photo(void *arg)
{
	OS_ERR      err;
	int BF;
//	struct nw_ctl *pMsg;
	struct nw_ctl *nw_r_post;
	OS_MSG_SIZE msg_size;
	uint8_t cmding=0,i;
	uint16_t waitime=0;
	
	while(1)
	{
		if(cmding==0) waitime=0;
		else waitime=30000;
		
		nw_r_post = OSTaskQPend (//(OS_Q         *)&ur2_Q,                	//消息变量指针
                    (OS_TICK       )waitime,                 //等待时长
                    (OS_OPT        )OS_OPT_PEND_BLOCKING, //如果没有获取到信号量就等待
                    (OS_MSG_SIZE  *)&msg_size,            //获取消息的字节大小
                    (CPU_TS       *)0,                    //获取任务发送时的时间戳
                    (OS_ERR       *)&err);                //返回错误
		
		printf("\r\nget_post%x",nw_r_post->cmd);
		
		if(err!=OS_ERR_NONE)
		{
			cmding=0;
		}
		else
		{
			switch(nw_r_post->cmd)
			{	
				case 0xf0:	//主站请求拍摄照片
	//				PTZCtrl_presets_goto_s(1,((uint8_t *)nw_r_post)[1]);	//摄像机调节到指定预置点
				printf("\r\n开始预置拍照 后端点:%x",nw_r_post->data[0]);
				PTZCtrl_presets_goto_s(1,nw_r_post->data[0]);	//摄像机调节到指定预置点
				
				cmding=0x83;
					get_photo_s();	
						OSTimeDly( 1000, OS_OPT_TIME_DLY, & err);
						
						nw_sc.cmd=0x84;
						nw_sc.len=10;
											
						memcpy(nw_sc.data,systime,6);
						nw_sc.data[6]=1;//通道号	
						nw_sc.data[7]=0;//预置位号
						
						tt_len=((PH_LEN[0])<<16) +PH_LEN[1];
						if(tt_len>len_max) tt_len=len_max;
						tt_pk_nb=(tt_len/pk_len); //1024一次
						if(tt_len%pk_len) tt_pk_nb +=1;
						nw_sc.data[8]=(tt_pk_nb>>8);//包数高位
						nw_sc.data[9]=tt_pk_nb;			//包数低位
					
						nw_sent(&nw_sc);
						ctlnumber=0x84;
						SenTimes=5;
						retxtime=3;		//请求上送照片 1s重发,要求3s,最多5次
						sentTime=worktime;
				break;
				
				case 0x83:	//主站请求拍摄照片
					if((cmding==0)||(cmding>=0x86))
					{
						cmding=nw_r_post->cmd;
						nw_sent(nw_r_post);
						get_photo_s();	
						OSTimeDly( 1000, OS_OPT_TIME_DLY, & err);
						
						nw_sc.cmd=0x84;
						nw_sc.len=10;
											
						memcpy(nw_sc.data,systime,6);
						nw_sc.data[6]=1;//通道号	
						nw_sc.data[7]=0;//预置位号
						
						tt_len=((PH_LEN[0])<<16) +PH_LEN[1];
						if(tt_len>len_max) tt_len=len_max;
						tt_pk_nb=(tt_len/pk_len); //1024一次
						if(tt_len%pk_len) tt_pk_nb +=1;
						nw_sc.data[8]=(tt_pk_nb>>8);//包数高位
						nw_sc.data[9]=tt_pk_nb;			//包数低位
					
						nw_sent(&nw_sc);
						ctlnumber=0x84;
						SenTimes=5;
						retxtime=3;		//请求上送照片 1s重发,要求3s,最多5次
						sentTime=worktime;	
					}
					break;
				case 0x84:	//采集终端请求上送照片答复
					if(cmding==0x83)
					{
						cmding=nw_r_post->cmd;
						ctlnumber=0x85;
						printf("\r\n准备发照片%d包,总长%d,",tt_pk_nb,tt_len);
//						OSTimeDly( 100, OS_OPT_TIME_DLY, & err);
						
						for(pk_nb=1;pk_nb<=tt_pk_nb;pk_nb++)
						{
							if(pk_nb==tt_pk_nb) nw_sent_ph(PH_DATA+((pk_nb-1)*pk_len),tt_len-((pk_nb-1)*pk_len),pk_nb);
							else	nw_sent_ph(PH_DATA+((pk_nb-1)*pk_len),pk_len,pk_nb);
							OSTimeDly ( 3, OS_OPT_TIME_DLY, & err ); 
						}
						
						OSTimeDly ( 4000, OS_OPT_TIME_DLY, & err ); 
						
						retxtime=1;		//上送结束标记 1s重发,要求30s,最多5次
						sentTime=worktime;	
						nw_sc.cmd=0x86;//数据结束
						cmding=0x86;
						nw_sc.len=2;			
						nw_sc.data[0]=1;//通道号	
						nw_sc.data[1]=0;//预置位号
						
						nw_sent(&nw_sc);
						ctlnumber=0x86;
						SenTimes=6;
					}
					
					break;
					
					case 0x85:	//图像数据上送，只有发
				
					break;
					
					case 0x86:	//图像数据上送结束标记
						cmding=0;//数据包成功
						ctlnumber=ctlnumberOld;
					break;
					
					case 0x87:	//补包数据下发
						if(nw_r_post->data[2]==0)
						{
							cmding=0;//数据包成功
							ctlnumber=ctlnumberOld;
							printf("\r\n图片发送成功");
						}
						else if(cmding==0x86)
						{
//							SenTimes=4;
//							cmding=nw_r_post->cmd;
							for(i=0;i<nw_r_post->data[2];i++)
							{
								BF=nw_r_post->data[2*i+3];
								BF= (BF<<8)+nw_r_post->data[2*i+4];
								pk_nb=BF;
								if(pk_nb==tt_pk_nb) nw_sent_ph(PH_DATA+((pk_nb-1)*pk_len),tt_len-((pk_nb-1)*pk_len),pk_nb);
								else	nw_sent_ph(PH_DATA+((pk_nb-1)*pk_len),pk_len,pk_nb);
							}
						}
					break;

			}//switch
		}
		
	}
}
	
void nw_ctr_hk(void *arg)
{
	
	OS_ERR      err;
	int BF;
//	struct nw_ctl *pMsg;
	struct nw_ctl *nw_r_post;
	OS_MSG_SIZE msg_size;

	while(1)
	{
		nw_r_post = OSTaskQPend (//(OS_Q         *)&ur2_Q,                	//消息变量指针
                    (OS_TICK       )0,                 //等待时长
                    (OS_OPT        )OS_OPT_PEND_BLOCKING, //如果没有获取到信号量就等待
                    (OS_MSG_SIZE  *)&msg_size,            //获取消息的字节大小
                    (CPU_TS       *)0,                    //获取任务发送时的时间戳
                    (OS_ERR       *)&err);                //返回错误
	
//		printf("\r\nget_post%x",nw_r_post->cmd);
		
		switch(nw_r_post->cmd)
		{
			case 0x81:	//图像采集参数配置
				if(memcmp(PSW,nw_r_post->data,4)==0)
				{
					WidthxHeight=nw_r_post->data[5];
					bLevel=nw_r_post->data[6];
					cLevel=nw_r_post->data[7];
					sLevel=nw_r_post->data[8];
					PTZ_Image_s(bLevel,cLevel,sLevel);
					printf("WidthxHeight=%d",WidthxHeight);
					PTZ_Streaming_s(WidthxHeight);
					Streaming_OK=0;
					nw_sent(nw_r_post);
				}
				else
				{
					rev_err(0x81,0xff);
				}
				break;
				
			case 0x82:	//拍照时间表设置
				if((memcmp(PSW,nw_r_post->data,4)==0)&&(nw_r_post->data[5]<=255))
				{
//密码0-3	通道号4	组数5					第1组
//											时			分			预置位号
//4字节	1字节		1字节		1字节		1字节			1字节
					aut_point_group[0]=nw_r_post->data[5];
					memcpy(aut_get_point,&nw_r_post->data[6],nw_r_post->data[5]*3);
					
					flash_save(aut_point_group,(nw_r_post->data[5]*3+1));
					
					nw_sent(nw_r_post);
				}
				else
				{
					rev_err(0x82,0xff);
				}
					
				break;
				
			case 0x8b:	//查询拍照时间表
//				if(memcmp(PSW,nw_r_post->data,4)==0)
				{
					printf("\r\n查询拍照时间");
					nw_sc.cmd=0x8b;
					if(aut_point_group[0]>10) aut_point_group[0]=0;

					nw_sc.len=2+aut_point_group[0]*3;
					nw_sc.data[0]=1; //通道号
					nw_sc.data[1]=aut_point_group[0]; //组数
					memcpy(&nw_sc.data[2],(uint8_t *)aut_get_point,aut_point_group[0]*3);
					
					nw_sent(&nw_sc);
					printf("\r\n查询拍照时间结束");
				}
//				else
//				{
//					rev_err(0x8b,0xff);
//				}
					
				break;
			
				case 0x88:	//摄像机远程调节
				if(memcmp(PSW,nw_r_post->data,4)==0) //localpassw[50]
				{			
	//			  const	char *PTZCMD[9]={"","","TILT_UP","TILT_DOWN","PAN_LEFT","PAN_RIGHT","ZOOM_IN","ZOOM_OUT",""};
	//				PUT_PTZ_s(nw_r_post->data[4],PTZCMD[nw_r_post->data[5]-1]);
					nw_ctrl_hk(&nw_r_post->data[4]);
				
					nw_sent(nw_r_post);
						
				}
				else
				{
					rev_err(0x88,0xff);
				}
				break;
		}
	}
}




extern unsigned char sampled_step,sampled_data_nub,sampled_data_flag;
void sampled_data_sent_22(unsigned char step,unsigned char zbs);
void sampled_data_sent_25(unsigned char zbs);


void nw_in(struct nw_ctl *nw_r_post)
{
	int i,j;
	OS_ERR      err;
	
//void  OSTaskQPost (OS_TCB       *p_tcb,    //目标任务
//                   void         *p_void,   //消息内容地址
//                   OS_MSG_SIZE   msg_size, //消息长度
//                   OS_OPT        opt,      //选项
//                   OS_ERR       *p_err)    //返回错误类型
	
//OSQPost ((OS_Q        *)&ur2_Q,                             //消息变量指针
//             (void        *)rev_data,                //要发送的数据的指针，将内存块首地址通过队列“发送出去”
//             (OS_MSG_SIZE  )len,     									//数据字节大小
//             (OS_OPT       )OS_OPT_POST_FIFO | OS_OPT_POST_ALL, //先进先出和发布给全部任务的形式
//             (OS_ERR      *)&err);	                            //返回错误类型		
	
//	printf("\r\nnw_r_post->cmd=%x",nw_r_post->cmd);	
	
	if(((nw_r_post->cmd>=0x81)&&(nw_r_post->cmd<=0xB8))||(nw_r_post->cmd==0xf0))
	{
//		memcpy(&nw_to_hk[nw_post_nb],nw_r_post,sizeof(struct nw_ctl));
		if(nw_r_post->cmd==0xf0)
		{
			OSTaskQPost ((OS_TCB      *)&nw_get_photo_TCB,                       //目标任务
								 (void        *)nw_r_post,                //要发送的数据的指针，将内存块首地址通过队列“发送出去”
								 (OS_MSG_SIZE  )3,     										//数据字节大小
								 (OS_OPT       )OS_OPT_POST_FIFO , 				//先进先出
								 (OS_ERR      *)&err);	                            //返回错误类型	
		}
	 	else if((nw_r_post->cmd>=0x83)&&(nw_r_post->cmd<=0x87))
		{
			OSTaskQPost ((OS_TCB      *)&nw_get_photo_TCB,                       //目标任务
								 (void        *)nw_r_post,                //要发送的数据的指针，将内存块首地址通过队列“发送出去”
								 (OS_MSG_SIZE  )sizeof(struct nw_ctl),     									//数据字节大小
								 (OS_OPT       )OS_OPT_POST_FIFO , //先进先出
								 (OS_ERR      *)&err);	                            //返回错误类型	
		}
		else
		{
			OSTaskQPost ((OS_TCB      *)&nw_ctr_hk_TCB,                       //目标任务
								 (void        *)nw_r_post,                //要发送的数据的指针，将内存块首地址通过队列“发送出去”
								 (OS_MSG_SIZE  )sizeof(struct nw_ctl),     									//数据字节大小
								 (OS_OPT       )OS_OPT_POST_FIFO , //先进先出
								 (OS_ERR      *)&err);	                            //返回错误类型	
		}

//		printf("\r\nerr=%d",err);
		return;
	}
//	nw_ctr_hkaaa(nw_r_post);
	
	switch(nw_r_post->cmd)
			{
				case 	0xff:			//4g post
//					nw_sc.cmd=0;
//					nw_sc.len=sizeof(version);
//					nw_sc.data=version;
//					nw_sent(&nw_sc);
					retxtime=3; 	//开机联络，1分钟重发
					break;
				
				case  request_la_qj://22	//	上传导地线拉力及倾角数据	
					if(nw_r_post->len==0)//主站请求数据
					{
						printf("\r\n主站请求22数据");
						sampled_step=0;
						sampled_data_sent_22(sampled_step,sampled_data_nub);
						SenTimes=2;
					}
					else if(nw_r_post->len==3)
					{
//						第几帧标识 1字节	AA55H
						if((nw_r_post->data[1]==0xAA)&&(nw_r_post->data[2]==0x55))
						{
							printf("\r\n收到主站答复22数据,标识号为%x",nw_r_post->data[0]);
							if(nw_r_post->data[0]==sampled_data_nub)
							{
								sampled_data_nub++;
								sampled_step++;
								if(sampled_step<=2)
								{
									sampled_data_sent_22(sampled_step,sampled_data_nub);
									SenTimes=2;
								}
//								else	if(sampled_data_flag)
//								{
////									if(ctlnumber==request_la_qj) 
//									sampled_step=3;
//									sampled_data_sent(sampled_step,sampled_data_nub);
//									SenTimes=2;
//									ctlnumber=0x25;
//								}
								else
								{
									ctlnumber=ctlnumberOld;
									GDT_OK=1;
								}
							}
							else
							{
								printf("\r\n数据标识号错误,标识号应为%x",sampled_data_nub);
							}
						}
					}
					break;
				case  request_qx:		//0x25	//	上传气象数据	
					if(nw_r_post->len==0)//主站请求数据
					{
						printf("\r\n主站请求25数据");
						sampled_step=3;
						sampled_data_sent_25(sampled_data_nub);
						ctlnumber=request_qx;
						SenTimes=2;
					}
					else if(nw_r_post->len==3)
					{
//						第几帧标识 1字节	AA55H
						if((nw_r_post->data[1]==0xAA)&&(nw_r_post->data[2]==0x55))
						{
							printf("\r\n收到主站答复25数据,标识号为%x",nw_r_post->data[0]);
							if(nw_r_post->data[0]==sampled_data_nub)
							{
								sampled_data_nub++;
								sampled_step++;
//								if(sampled_data_flag)
//								{
//									sampled_data_flag=0;
//								}
								ctlnumber=ctlnumberOld;
								GDT_OK=1;
							}
							else
							{
								printf("\r\n数据标识号错误,标识号应为%x",sampled_data_nub);
							}
						}
					}
					break;
					
				case 	Contact:			//00	//	开机联络信息	装置开机上送联络信息
					printf("\r\nCont len=:%d",nw_r_post->len);
				
					if((ctlnumber==Contact)&&(nw_r_post->len==2))//sizeof(version) 正在开机联络时，服务器回复开机联络信息
					{
						if((nw_r_post->data[0]==nw_sc.data[0])&&(nw_r_post->data[1]==nw_sc.data[1]))
						{
							ctlnumber=CheckT;
							nw_sc.cmd=ctlnumber;
							nw_sc.len=0;//sizeof(systime);
							memcpy(nw_sc.data,systime,6);
							nw_sent(&nw_sc);
							retxtime=6;		//校时，2分钟重发
							sentTime=worktime;	
						}
					}
					else if(nw_r_post->len==0) //
					{
//						ctlnumber=Contact; //否则认为 服务台主动发开机联络信号
//						mode=1;
					}
					
//					retxtime=0; 	// 下一次的时间
					break;
					
				case  CheckT:				//01	//	校时	该命令分请求和下发两部分
//					if(memcmp(sentTime,systime,5)==0)
					if(ctlnumber==CheckT)
					{
//						printf("\r\nworktime=%x,sentTime%x",worktime,sentTime);
						if((worktime-sentTime)<20)
						{
							memcpy(systime,nw_r_post->data,6);
							ctlnumber=HeartBeat;
//							printf("\r\nHeartBeat=%x",HeartBeat);
						}
					}
					else
					{
						memcpy(systime,nw_r_post->data,6);
						nw_sent(nw_r_post);
					}
//					retxtime=0;
									
					break;
				case  SetPassword :	//02	//	设置装置密码	装置出厂密码：字符：‘1234’（31H32H33H34H）
					printf("\r\nPSW=%x,%x,%x,%x",PSW[0],PSW[1],PSW[2],PSW[3]);
					if(memcmp(PSW,nw_r_post->data,4)==0) //localpassw[50]
					{
						memcpy(PSW,&nw_r_post->data[4],4);
						flash_save(PSW,4);
						nw_sent(nw_r_post);
					}
					else
					{
						rev_err(SetPassword,0xff);
					}
					printf("\r\nnw_r_post->data=%x,%x,%x,%x",nw_r_post->data[0],nw_r_post->data[1],nw_r_post->data[2],nw_r_post->data[3]);
					printf("\r\nPSW=%x,%x,%x,%x",PSW[0],PSW[1],PSW[2],PSW[3]);
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
					
//3,0,12,31,32,33,34,  3c, 2,58, 0,0,  2,58,  0,2,3,  31,32,33,34,  b,16,
					printf("\r\n本地密码PSW=%x,%x,%x,%x",PSW[0],PSW[1],PSW[2],PSW[3]);
					printf("\r\n接收密码%x,%x,%x,%x",nw_r_post->data[0],nw_r_post->data[1],nw_r_post->data[2],nw_r_post->data[3]);
				
					printf("\r\nnw_r_post->data=%x,%x,%x,%x",nw_r_post->data[0],nw_r_post->data[1],nw_r_post->data[2],nw_r_post->data[3]);
				
					if((memcmp(PSW,nw_r_post->data,4)==0)&&(memcmp(CHW,&nw_r_post->data[14],4)==0)) //localpassw[50]
					{
						
						*HBT=nw_r_post->data[4];
						*GDT=(nw_r_post->data[5]<<8)+nw_r_post->data[6];
						*SLT=(nw_r_post->data[7]<<8)+nw_r_post->data[8];
						*OLT=(nw_r_post->data[9]<<8)+nw_r_post->data[10];
						RST[0]=nw_r_post->data[11]; //日 0-28
						RST[1]=nw_r_post->data[12];	//时 0-24
						RST[2]=nw_r_post->data[13];	//分 0-60
						
						flash_save((uint8_t *)HBT,11);
						
						nw_sent(nw_r_post);
						printf("RST[0]=%d,RST[1]=%d,RST[2]=%d",RST[0],RST[1],RST[2]);
						if(RST[0]==0)
						{
							if(((RST[1]==systime[3])&&(RST[2]==systime[4])))
							{
								printf("准备复位");
								STM32_rest_delay=5;
//								reset_STM32();
							}
						}
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
							HostIP[0]=nw_r_post->data[4];
							HostIP[1]=nw_r_post->data[5];
							HostIP[2]=nw_r_post->data[6];
							HostIP[3]=nw_r_post->data[7];
							*HostPort=((nw_r_post->data[8]<<8)+nw_r_post->data[9]);
							
							flash_save(HostIP,6);
							
							STM32_rest_delay=5;
	//						reset_STM32();
						}
						else
						{
							rev_err(chang_ip,0); //若两组主站IP、端口号和主站卡号对应字节不完全相同 数据出错
						}
					}
					else
					{
						rev_err(chang_ip,0xff); //密码错误
					}
					break;
				case  check_ip:			//07	//	查询主站IP地址、端口号和卡号	
//				if(nw_r_post->len==0)
					{
						nw_sc.cmd=check_ip;
						nw_sc.len=12;
						
						memcpy(nw_sc.data,HostIP,4); //HostIP[0],HostIP[1],HostIP[2],HostIP[3],*HostPort
						nw_sc.data[4]=*HostPort>>8;
						nw_sc.data[5]=*HostPort;
						memcpy(&nw_sc.data[6],nw_phone_num,6);
//						nw_sc.data=nw_sc.data;
						
						nw_sent(&nw_sc);
					}
					break;
				case  ResetDev:			//08	//	装置复位	主站对装置进行复位
					if(memcmp(PSW,nw_r_post->data,4)==0)
					{
						nw_sent(nw_r_post);
						reset_STM32();
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
				case  check_device_cfg://0A	//	查询装置配置参数   68 43 43 30 31 39 36 0a 00 00 9f 16 
				//																								:68 43 43 30 31 39 36 0a 00 00 9f 16 
//				if(nw_r_post->len==0)
					{
						
						nw_sc.cmd=check_device_cfg;
//						memcpy(nw_sc.data,nw_ip,4);
						
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
						nw_sc.data[i++]=*HBT;
						nw_sc.data[i++]=*GDT>>8;
						nw_sc.data[i++]=*GDT;
						nw_sc.data[i++]=*SLT>>8;
						nw_sc.data[i++]=*SLT;
						nw_sc.data[i++]=*OLT>>8;
						nw_sc.data[i++]=*OLT;
						nw_sc.data[i++]=RST[0];
						nw_sc.data[i++]=RST[1];
						nw_sc.data[i++]=RST[2];
						
						nw_sc.data[i++]=1;				//图像1色彩选择
//基本图像大小定义如下：
//320 X 240为1；
//640 X 480为2；
//704 X 576为3；
//扩展图像大小定义如下：
//800 X 600为4；
//1024 X 768 为 5；
//1280 X 1024 为 6
//1280 X 720 为7
//1920 X 1080 为 8
//960H	960 x 576 为9
//960P 1280 x 960 为10 
//1200P  1600 x 1200为11  
//QXGA	2048 x 1536为12
//400W	2592 x 1520 为13
//500W	2592 x 1944 为14
//600W  3072*2048 为15
//800W(4K)	3840 x 2160为16
//1200W   4000 x 3000为17
//1600W	4608 x 3456为18
//QUXGA   3200x2400为19
//4224 x 3136为20			
//						if(Streaming_OK>=2)
//						{
//							nw_sc.data[i++]=WidthxHeight;
//							nw_sc.data[i++]=bLevel;nw_sc.data[i++]=cLevel;nw_sc.data[i++]=sLevel;
//						}
//						else
						{
							if(GET_Streaming_s()==0)	//图像1图像大小
							{
								nw_sc.data[i++]=WidthxHeight;
								Streaming_OK=1;
							}		
							if(GET_Image_s()==0) //图像1亮度,对比度,饱和度
							{
								nw_sc.data[i++]=bLevel;nw_sc.data[i++]=cLevel;nw_sc.data[i++]=sLevel;
								if(Streaming_OK==1) Streaming_OK=2;
							}
						}
						
//						int GET_Image_s(void)//char chanle,signed char *brightnessLevel,signed char *contrastLevel,signed char *saturationLevel

						nw_sc.data[i++]=1;nw_sc.data[i++]=1;nw_sc.data[i++]=60;nw_sc.data[i++]=60;nw_sc.data[i++]=60; //通道2配置
						
						for(j=0;j<function_nub;j++)
						{
							nw_sc.data[i++]=function_buf[j];
						}
//						memcpy(&nw_sc.data[i],function_buf,strlen((char *)function_buf));
						nw_sc.len=i;//+strlen((char *)function_buf);
//						nw_sc.data=nw_sc.data;
						nw_sent(&nw_sc);
					}
					break;
				case  device_function_cfg://0B	//	装置功能配置	
					if(memcmp(PSW,nw_r_post->data,4)==0)
					{
						function_nub=nw_r_post->len-4;
						memcpy(function_buf,&nw_r_post->data[4],function_nub);
						function_buf[nw_r_post->len-4]=0; //结尾给0
						
						nw_sent(nw_r_post);
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
					nw_sc.cmd=check_device_time;
					nw_sc.len=6;
//					nw_sc.data=systime;
					memcpy(nw_sc.data,systime,6);
					nw_sent(&nw_sc);
					break;
				case  sent_msg:
					nw_sent(nw_r_post);
//				发短信	https://blog.csdn.net/qq_30460905/article/details/81805891
					break;
				case  request_data:	//21	//	主站请求装置数据	主站请求采集装置采集数据并立即上送
					if(nw_r_post->len==0)//上传未成功上传的历史数据，包含历史照片，若装置无历史数据则不上传
					{
						sampled_data_time=1;
					}
					else if((nw_r_post->len==2)&&(nw_r_post->data[0]==0xBB)&&(nw_r_post->data[1]==0xBB))//装置立刻采集所有数据（图片除外），完成采集后立刻上传。该次采样不影响原设定采集间隔的执行
					{
						sampled_data_time=1;
					}
					nw_sent(nw_r_post);
					break;

			}
}




void nw_ctr_hkaaa(struct nw_ctl *nw_r_post)
{
	OS_ERR      err;
	int BF;
//	struct nw_ctl *pMsg;
//	struct nw_ctl *nw_r_post;
	OS_MSG_SIZE msg_size;
	uint8_t cmding=0,i;
	uint16_t waitime=0;
	
	switch(nw_r_post->cmd)
		{
			case 0x81:	//图像采集参数配置
				if(memcmp(PSW,nw_r_post->data,4)==0)
				{
					WidthxHeight=nw_r_post->data[5];
					bLevel=nw_r_post->data[6];
					cLevel=nw_r_post->data[7];
					sLevel=nw_r_post->data[8];
					PTZ_Image_s(bLevel,cLevel,sLevel);
					printf("WidthxHeight=%d",WidthxHeight);
					PTZ_Streaming_s(WidthxHeight);
					Streaming_OK=0;
				}
				else
				{
					rev_err(0x81,0xff);
				}
				break;
				
			case 0x82:	//拍照时间表设置
				if(memcmp(PSW,nw_r_post->data,4)==0)
				{
					WidthxHeight=nw_r_post->data[5];
					bLevel=nw_r_post->data[6];
					cLevel=nw_r_post->data[7];
					sLevel=nw_r_post->data[8];
					PTZ_Image_s(bLevel,cLevel,sLevel);
					printf("WidthxHeight=%d",WidthxHeight);
					PTZ_Streaming_s(WidthxHeight);
				}
				else
				{
					rev_err(0x81,0xff);
				}
					
				break;
			case 0x83:	//主站请求拍摄照片
					if((cmding==0)||(cmding>=0x86))
					{
						cmding=nw_r_post->cmd;
						nw_sent(nw_r_post);
						get_photo_s();	
						OSTimeDly( 2000, OS_OPT_TIME_DLY, & err);
						
						nw_sc.cmd=0x84;
						nw_sc.len=10;
											
						memcpy(nw_sc.data,systime,6);
						nw_sc.data[6]=1;//通道号	
						nw_sc.data[7]=0;//预置位号
						
						tt_len=((PH_LEN[0])<<16) +PH_LEN[1];
						if(tt_len>len_max) tt_len=len_max;
						tt_pk_nb=(tt_len/pk_len); //1024一次
						if(tt_len%pk_len) tt_pk_nb +=1;
						nw_sc.data[8]=(tt_pk_nb>>8);//包数高位
						nw_sc.data[9]=tt_pk_nb;			//包数低位
					
						nw_sent(&nw_sc);
						ctlnumber=0x84;
						SenTimes=4;
						retxtime=3;		//请求上送照片 1s重发,要求3s,最多5次
						sentTime=worktime;	
					}
					break;
				case 0x84:	//采集终端请求上送照片答复
					if(cmding==0x83)
					{
						cmding=nw_r_post->cmd;
						ctlnumber=0x85;
						printf("\r\n准备发照片%d包,总长%d,",tt_pk_nb,tt_len);
//						OSTimeDly( 100, OS_OPT_TIME_DLY, & err);
						
						for(pk_nb=1;pk_nb<=tt_pk_nb;pk_nb++)
						{
							if(pk_nb==tt_pk_nb) nw_sent_ph(PH_DATA+((pk_nb-1)*pk_len),tt_len-((pk_nb-1)*pk_len),pk_nb);
							else	nw_sent_ph(PH_DATA+((pk_nb-1)*pk_len),pk_len,pk_nb);
		//					OSTimeDly ( 10, OS_OPT_TIME_DLY, & err ); 
						}
						
						OSTimeDly ( 2000, OS_OPT_TIME_DLY, & err ); 
						
						retxtime=1;		//上送结束标记 1s重发,要求30s,最多5次
						sentTime=worktime;	
						nw_sc.cmd=0x86;//数据结束
						cmding=0x86;
						nw_sc.len=2;			
						nw_sc.data[0]=1;//通道号	
						nw_sc.data[1]=0;//预置位号
						
						nw_sent(&nw_sc);
						ctlnumber=0x86;
						SenTimes=4;
					}
					
					break;
					
					case 0x85:	//图像数据上送，只有发
				
					break;
					
					case 0x86:	//图像数据上送结束标记
						cmding=0;//数据包成功
						ctlnumber=ctlnumberOld;
					break;
					
					case 0x87:	//补包数据下发
						if(nw_r_post->data[2]==0)
						{
							cmding=0;//数据包成功
							ctlnumber=ctlnumberOld;
						}
						else if(cmding==0x86)
						{
//							SenTimes=4;
//							cmding=nw_r_post->cmd;
							for(i=0;i<nw_r_post->data[2];i++)
							{
								BF=nw_r_post->data[2*i+3];
								BF= (BF<<8)+nw_r_post->data[2*i+4];
								pk_nb=BF;
								if(pk_nb==tt_pk_nb) nw_sent_ph(PH_DATA+((pk_nb-1)*pk_len),tt_len-((pk_nb-1)*pk_len),pk_nb);
								else	nw_sent_ph(PH_DATA+((pk_nb-1)*pk_len),pk_len,pk_nb);
							}
						}
					break;
			
				case 0x88:	//摄像机远程调节
				if(memcmp(PSW,nw_r_post->data,4)==0) //localpassw[50]
				{			
	//			  const	char *PTZCMD[9]={"","","TILT_UP","TILT_DOWN","PAN_LEFT","PAN_RIGHT","ZOOM_IN","ZOOM_OUT",""};
	//				PUT_PTZ_s(nw_r_post->data[4],PTZCMD[nw_r_post->data[5]-1]);
					nw_ctrl_hk(&nw_r_post->data[4]);
				
					nw_sent(nw_r_post);
						
				}
				else
				{
					rev_err(0x88,0xff);
				}
				break;
		}
}