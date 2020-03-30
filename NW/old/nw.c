

#include  <os.h>
#include "global.h"

#define  NW_MODULE
#include "nw.h"
#undef NW_MODULE

#include "sram.h"
#include "timer.h"
#include "usart.h"
#include "bsp_uart.h"
#include "FM25V10.h"


unsigned char sampled_step,sampled_data_nub,sampled_data_flag;
void  nw_sent_8(uint8_t *t_data,uint8_t len);
unsigned char WidthxHeight,STM32_rest_delay;
unsigned char bLevel,cLevel,sLevel;
int duty_time;
unsigned char flag_4g_ready=0;

const unsigned char ver[2]={1,0};
uint8_t nw_stete,sent_cnt;
unsigned int LastTxT;
#define testdvi	5
unsigned char ip_seting;
uint32_t point3_now;

int get_uart3_data(uint8_t **pack_point, uint16_t *pack_len)
{
	uint32_t i,len;
	uint8_t * rdata;
	
	
	
	if(point3_last == point3) return 0; //没数据

	printf("\r\npoint3_last=%d",point3_last);
	printf("\r\npoint3=%d",point3);
//	OSTimeDly (10, OS_OPT_TIME_DLY, & err ); 
	if(point3_now != point3)	
	{
		point3_now=point3;
		printf("\r\n没收完一包");
		return 1;	//没收完一包
	}
///////point3_now == point3 说明接收结束////////////////////////
		
	if(point3_now>point3_last)
	{
		len=point3_now-point3_last;
	}
	else
	{
		for(i=0;i<point3_now;i++)
		{
			urd3[lenmax3+i]=urd3[i];
		}
		len=point3_now+lenmax3-point3_last;
	}
	if(len<=10)
	{
		printf("\r\nm->s data too short err");
		point3_last=point3_now;
		return -1;	
	}
	
__date_rev:	//此处len为收到数据的总长度
	rdata=&urd3[point3_last];
	
	if (0 != memcmp(rdata, STAA, 7))
	{
		if((point3_last<len)&&(len>10))
		{
			len--;
			point3_last++;
			goto __date_rev;
		}
		else
		{
			printf("\r\nm->s data stat err");
//			point3_last=point3_now; //留着一段数据,下一次判断是否有继续接收
			return -1;	
		}
	}
	
	//此处len为收到南网协议包的数据域长度
	for(i=0;i<len;i++) printf("%x ",rdata[i]);
	
	len=(rdata[8]<<8)+rdata[9]; //数据长度
	if(len>=(lenmax3-12))
	{
		printf("\r\nm->s data too long err");
		point3_last=point3_now;
		return -1;	
	}
	
	if((checksum_4g(&rdata[1],len+6+1+2)!=rdata[len+10]))
	{
		printf("\r\nm->s data checksum err");
		point3_last=point3_now;
		return -1;	
	}
	if(rdata[len+11]!=0x16)
	{
		printf("\r\nm->s data end err");
		point3_last=point3_now;
		return -1;	
	}
	
	printf("\r\nM->S %x,%d:%d:%d",rdata[7],systime[3],systime[4],systime[5]);
	
	*pack_point=rdata;		//数据地址指向
	*pack_len=len+12;			//数据长度(不包含cmd和len)
			
	point3_last += len+12;
	if(point3_last>=lenmax3) point3_last -=lenmax3;

	
	return 2;	//数据OK
}

void NW_REV_Task(void * p_arg)//pro
{
	OS_ERR      err;
//	int state;
	uint8_t *pack_in;
	uint16_t pack_len;
	(void)p_arg;	
	
	uint8_t cmd;
	uint8_t *data_in;
	uint16_t data_len;
	uint16_t i,j;
	
	printf("1");
	HAL_UART_Receive_IT(&huart3,(unsigned char *)urd3,1);
	while(1)
	{
		OSTimeDly ( 20, OS_OPT_TIME_DLY, & err );	

		if(get_uart3_data(&pack_in,&pack_len)==2) //收到完整的南网数据包
		{
			cmd=pack_in[7];
			data_len=(pack_in[8]<<8)+pack_in[9];
			data_in=pack_in+10;

			switch(cmd)
			{
				case  request_la_qj://22	//	上传导地线拉力及倾角数据	
					if(data_len==0)//主站请求数据
					{
						senttask_Asim |= data_flag22;	//
						
//						printf("\r\n主站请求22数据");
//						sampled_step=0; 
//						sampled_data_sent_22(sampled_step,sampled_data_nub);
//						SenTimes=2;
					}
					else if(data_len==3)	//答复数据
					{
//						第几帧标识 1字节	AA55H
						if((data_in[1]==0xAA)&&(data_in[2]==0x55))
						{
//							printf("\r\n收到主站答复22数据,标识号为%x",data_in[0]);
							if(data_in[0]==sampled_data_nub)
							{
								senttask_Pend &= (~data_flag22);
							}
							else
							{
//								printf("\r\n数据标识号错误,标识号应为%x",sampled_data_nub);
							}
						}
					}
					break;
				case  request_qx:		//0x25	//	上传气象数据	
					if(data_len==0)//主站请求数据
					{
						senttask_Asim |= data_flag25;
					}
					else if(data_len==3)
					{
//						第几帧标识 1字节	AA55H
						if((data_in[1]==0xAA)&&(data_in[2]==0x55))
						{
							printf("\r\n收到主站答复25数据,标识号为%x",data_in[0]);
							if(data_in[0]==sampled_data_nub)
							{
								senttask_Pend &= (~data_flag25);	//收到答复
							}
							else
							{
								printf("\r\n数据标识号错误,标识号应为%x",sampled_data_nub);
							}
						}
					}
					break;
					
				case 	Contact:			//00	//	开机联络信息	装置开机上送联络信息
								
				if(senttask_Pend & data_flag00)	//正等待答复 
				{
					if((data_in[0]==ver[0])&&(data_in[1]==ver[1]))
					{
						senttask_Pend &= ~data_flag00;
						printf("\r\n开机联络OK");
					}
				}
				else
					senttask_Asim |= data_flag00;
				
					break;
					
				case  CheckT:				//01	//	校时	该命令分请求和下发两部分
					if(senttask_Pend & data_flag01) //请求校时
					{
						if((worktime-duty_time)<20)
						{
							senttask_Pend &= (~data_flag01);
							memcpy(systime,data_in,6);
						}
					}
					else
					{
						memcpy(systime,data_in,6);
						nw_sent_8(pack_in,pack_len);
//						senttask_Asim |= data_flag01;
					}									
					break;
					
				case  SetPassword :	//02	//	设置装置密码	装置出厂密码：字符：‘1234’（31H32H33H34H）
					printf("\r\nPSW=%x,%x,%x,%x",PSW[0],PSW[1],PSW[2],PSW[3]);
					if(memcmp(PSW,data_in,4)==0) //localpassw[50]
					{
						memcpy(PSW,&data_in[4],4);
						flash_save(PSW,4);
						nw_sent_8(pack_in,pack_len);
					}
					else
					{
						rev_err(SetPassword,0xff);
					}
					printf("\r\ndata_in=%x,%x,%x,%x",data_in[0],data_in[1],data_in[2],data_in[3]);
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
					printf("\r\n接收密码%x,%x,%x,%x",data_in[0],data_in[1],data_in[2],data_in[3]);
				
					printf("\r\ndata_in=%x,%x,%x,%x",data_in[0],data_in[1],data_in[2],data_in[3]);
				
					if((memcmp(PSW,data_in,4)==0)&&(memcmp(CHW,&data_in[14],4)==0)) //localpassw[50]
					{
						
						*HBT=data_in[4];
						*GDT=(data_in[5]<<8)+data_in[6];
						*SLT=(data_in[7]<<8)+data_in[8];
						*OLT=(data_in[9]<<8)+data_in[10];
						RST[0]=data_in[11]; //日 0-28
						RST[1]=data_in[12];	//时 0-24
						RST[2]=data_in[13];	//分 0-60
						
						flash_save((uint8_t *)HBT,11);
						
						nw_sent_8(pack_in,pack_len);
						
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
					senttask_Pend &= (~data_flag05);
				
//					if(senttask_Pend & data_flag05)
//					{
//						senttask_Pend &= data_flag05;
//					}
//					else
//					{
//						senttask_Asim |= data_flag05;
//					}	
					break;
				case  chang_ip:			//06	//	更改主站IP地址、端口号和卡号	
					if(memcmp(PSW,data_in,4)==0)
					{
//密码	主站IP	端口号	主站IP	端口号	主站卡号	主站卡号
//4字节	4字节	2字节	4字节	2字节	6字节	6字节
						if(memcmp(&data_in[4],&data_in[10],6)==0)
						{
							HostIP[0]=data_in[4];
							HostIP[1]=data_in[5];
							HostIP[2]=data_in[6];
							HostIP[3]=data_in[7];
							*HostPort=((data_in[8]<<8)+data_in[9]);
							
							flash_save(HostIP,6);
							
							nw_sent_8(pack_in,pack_len);
							senttask_Asim |= data_flag06; 	//复位4g驱动
	//						STM32_rest_delay=5;
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
//				if(data_len==0)
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
					if(memcmp(PSW,data_in,4)==0)
					{
						nw_sent_8(pack_in,pack_len);
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
				
//					senttask_Asim |= data_flag0a;
				
//				if(data_len==0)
					{
						
						nw_sc.cmd=check_device_cfg;
							
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

						{
//							if(GET_Streaming_s()==0)	//图像1图像大小
							{
								nw_sc.data[i++]=WidthxHeight;
//								Streaming_OK=1;
							}		
//							if(GET_Image_s()==0) //图像1亮度,对比度,饱和度
							{
								nw_sc.data[i++]=bLevel;nw_sc.data[i++]=cLevel;nw_sc.data[i++]=sLevel;
//								if(Streaming_OK==1) Streaming_OK=2;
							}
						}
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
					if(memcmp(PSW,data_in,4)==0)
					{
						function_nub=data_len-4;
						memcpy(function_buf,&data_in[4],function_nub);
						function_buf[data_len-4]=0; //结尾给0
						
						nw_sent_8(pack_in,pack_len);
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
//					nw_sent(nw_r_post);
//				发短信	https://blog.csdn.net/qq_30460905/article/details/81805891
					break;
				case  request_data:	//21	//	主站请求装置数据	主站请求采集装置采集数据并立即上送
					if(data_len==0)//上传未成功上传的历史数据，包含历史照片，若装置无历史数据则不上传
					{
//						sampled_data_time=1;
					}
					else if((data_len==2)&&(data_in[0]==0xBB)&&(data_in[1]==0xBB))//装置立刻采集所有数据（图片除外），完成采集后立刻上传。该次采样不影响原设定采集间隔的执行
					{
//						sampled_data_time=1;
					}
//					nw_sent(nw_r_post);
					break;

			}
		}
	}
}




//void check_retxtime()
//{
//	switch(ctlnumber)
//	{
//		case 	Contact:retxtime=60/testdvi;	break;	//开机联络，30s重发
//		case 	CheckT:retxtime=120/testdvi;	break;	//校时，2分钟重发 	//开机联络，30s重发
//		case 	HeartBeat:retxtime=*HBT*60/testdvi;	break;//	装置心跳信息	用于主站监测装置上线时间、IP地址和端口号、传输信号强度及蓄电池电压
//		case 	0x84:retxtime=2;	break;//
//		case 	0x86:retxtime=2;	break;//	装置心跳信息	用于主站监测装置上线时间、IP地址和端口号、传输信号强度及蓄电池电压
//		case 	0x22:retxtime=2;	break;//
//		case 	0x25:retxtime=2;	break;//
//	}
//}
int check_signel(void);
int Get_Volt(void);
void sampled_data_sent_22(unsigned char step,unsigned char zbs);
void sampled_data_sent_25(unsigned char zbs);

void check_data_get_cmd()
{
	if(senttask_Asim==0) return;
	if(senttask_Asim & data_flag00)	//请求开机联络
	{
		senttask_Asim &= (~data_flag00);
	}
	else	if(senttask_Asim & data_flag01)	//请求校时?
	{
		senttask_Asim &= (~data_flag01);
	}
	else	if(senttask_Asim & data_flag05)	//请求心跳?
	{
		senttask_Asim &= (~data_flag05);
	}
	else	if(senttask_Asim & data_flag06)	//更改IP地址,端口号,和卡号--关闭4G,再开,十秒内重新初始化
	{
		senttask_Asim &= (~data_flag06);
	}
	else	if(senttask_Asim & data_flag0a)	//查询装置参数, 装置参数需上电给球机配置.
	{
		senttask_Asim &= (~data_flag0a);
	}
	else	if(senttask_Asim & data_flag22)	//上传采集拉力数据
	{
		sent_cnt=0;
		nw_stete=22;
	}
	else	if(senttask_Asim & data_flag25)	//上传气象数据
	{
		sent_cnt=0;
		nw_stete=25;
	}
}


void  NW_SENT_Task  ( void * p_arg )//void  NWTask_task( void * p_arg )
{
	OS_ERR      err;
//	CPU_SR_ALLOC();
//	unsigned char mode=0;
//	struct nw_ctl *nw_r_post;
//	OS_MSG_SIZE*	 size;	
//	char i;
	
	(void)p_arg;	
		
	retxtime=0xff;
	function_buf[0]=0x22;
	function_buf[1]=0x25;
	function_nub=2;
	
	LastTxT=worktime;
	nw_stete=0;
	sent_cnt=0;
	duty_time=0;
	
	while (DEF_TRUE)
	{			
		OSTimeDly( 100, OS_OPT_TIME_DLY, & err);
		
		if(nw_stete==5)
		{
			check_data_get_cmd();
		}
		if(nw_stete==5)
		{
//			check_photo_ctr_cmd();
		}
		
		if((nw_stete==5)&&(senttask_Asim))
		{
			
		}
		
		if(nw_stete==0)
		{
			if(((worktime-duty_time)>=60/testdvi)||(sent_cnt==0))
			{
				senttask_Pend |= data_flag00;	//等待 data_flag00 答复
				
				nw_sc.cmd=Contact;
				nw_sc.len=2;
				nw_sc.data[0]=ver[0];
				nw_sc.data[1]=ver[1];
				nw_sent(&nw_sc);
				sent_cnt++;
				duty_time=worktime;
			}
			if((sent_cnt)&&((senttask_Pend & data_flag00)==0))
			{
				sent_cnt=0;
				nw_stete=1;
			}
		}
		else if(nw_stete==1) //请求校时
		{
			if(((worktime-duty_time)>=120/testdvi)||(sent_cnt==0))
			{
				senttask_Pend |= data_flag01;	//等待 data_flag01 答复
				
				nw_sc.cmd=CheckT;
				nw_sc.len=0;
				memcpy(nw_sc.data,systime,6);
				nw_sent(&nw_sc);
				sent_cnt++;
				duty_time=worktime;
				LastTxT=worktime;
			}
			if((sent_cnt)&&((senttask_Pend & data_flag01)==0))
			{
				sent_cnt=0;
				nw_stete=5;
			}
		}
		else if(nw_stete==5)
		{
			if((worktime-LastTxT)>=*HBT/testdvi)
			{
				nw_sc.cmd=HeartBeat;
				nw_sc.len=8;
				memcpy(nw_sc.data,systime,6);
				
				if(check_signel()==0) nw_sc.data[6]=signal;
				else nw_sc.data[6]=0;
			
				if(Get_Volt()==0) nw_sc.data[7]=volt;
				else nw_sc.data[7]=0;

				nw_sent(&nw_sc);
				LastTxT=worktime;
			}
		}
		else if(nw_stete==22)
		{
			sampled_data_nub++;
			
			sampled_data_sent_22(sampled_step,sampled_data_nub);
			
				nw_sc.cmd=HeartBeat;
				nw_sc.len=8;
				memcpy(nw_sc.data,systime,6);
				
				if(check_signel()==0) nw_sc.data[6]=signal;
				else nw_sc.data[6]=0;
			
				if(Get_Volt()==0) nw_sc.data[7]=volt;
				else nw_sc.data[7]=0;

				nw_sent(&nw_sc);
			LastTxT=worktime;
		}
		else if(nw_stete==25)
		{
			sampled_data_nub++;
			
			sampled_data_sent_25(sampled_data_nub);
			if((worktime-LastTxT)>=*HBT*60/testdvi)
			{
				nw_sc.cmd=HeartBeat;
				nw_sc.len=8;
				memcpy(nw_sc.data,systime,6);
				
				if(check_signel()==0) nw_sc.data[6]=signal;
				else nw_sc.data[6]=0;
			
				if(Get_Volt()==0) nw_sc.data[7]=volt;
				else nw_sc.data[7]=0;

				nw_sent(&nw_sc);
				LastTxT=worktime;
			}
		}

		else if(nw_stete==25)
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
void NW_HK_Task_Create (void);

OS_SEM read_RSSI;

void NWTaskStart (void *p_arg)
{
	OS_ERR      err;
	(void)p_arg;

	
	Ur5TaskStart (0);

//		多值信号量控制块指针		//多值信号量名称 //资源数目或事件是否发生标志 //返回错误类型
	OSSemCreate (&read_RSSI, "read_RSSI",  0, &err); //3 				

	NW_HK_Task_Create();
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


