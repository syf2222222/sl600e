


#include "global.h"
#include "nw.h"


#include "sram.h"
#include "sys_user.h"
#include "usart.h"
#include "bsp_uart.h"
#include "FM25V10.h"
//#include "power_ctrl.h"

void nw_sent_st(struct nw_ctl *t_data);
void rev_err(unsigned char ctl,unsigned char data);

extern unsigned char sampled_step,sampled_data_nub,sampled_data_flag;
extern unsigned char STM32_rest_delay;
extern const unsigned char ver[2];
extern unsigned char ip_seting;

int read_ph(unsigned char *in,unsigned char *out,int len);
int int_time_to_char_time(uint8_t *out,uint32_t sec);
extern unsigned int runningtime;
extern int hk_tast_step;
extern uint8_t STM32_rest_wait,HK_Task_free;
unsigned char checksum_4g(unsigned char* a,unsigned int n);
extern void UART_data_clear(UART_HandleTypeDef *huart);	


void resent_ph(uint8_t *in,uint16_t len);

void  nw_sent_8(uint8_t *t_data,uint16_t len);
void  nw_sent_ph_time(uint8_t ch);

int gain_qx_data(void);
int get_a_data_ll(void);	//1s一次
int gain_ll_data(void);
void sys_time_chang(void);
extern u8 DTU_c_state;
extern uint16_t Contact_time;
extern uint8_t wr_sleep;

extern u8 Contact_state;

int get_nw_data(uint8_t **pack_point, uint16_t *pack_len)
{
//	OS_ERR      err;
	uint16_t date_len;
	uint8_t *udata;	//读取串口数据地址
	uint16_t ulen,i;	//读取串口数据长度

////	printf("\r\nCR1=%04x",huart3.Instance->CR1);	
//	 /* Clear the Error flags in the ICR register */
//    __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF | UART_CLEAR_NEF | UART_CLEAR_PEF | UART_CLEAR_FEF);
	
//	SET_BIT(huart->Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE);
	
#ifdef	ur3_rdma
	if(huart3.GetPoint < huart3.NowPoint)
	{
		ulen =huart3.NowPoint-huart3.GetPoint;	//lenmax3 - huart->ReadPoint; //总计数减去未传输的数据个数，得到已经接收的数据个数
		udata=urd3+huart3.GetPoint;
		huart3.GetPoint= huart3.NowPoint;
		printf("\\");
//		for(i=0;i<ulen;i++) printf("%02x ",udata[i]);
	}
	else
#endif

	if(UART_Receive_s(&huart3,&udata, &ulen,100) != 2 ) 
	{
		return 0;
	}
//	printf("\r\nterrorflags=%x",terrorflags);
//	printf("\r\nur3");
	
	if(ulen<12)//10
	{
		if(DTU_c_state)	printf("\r\ns->m data too short err"); //刚上电那次不算
//		printf("GetPoint=%x,NowPoint=%x,ReadPoint=%x,",huart3.GetPoint,huart3.NowPoint,huart3.ReadPoint);			
		return -1;
	}
	
__date_rev:	
//	rdata=udata;
	
	if (0 != memcmp(udata, STAA, 7))
	{		
		if(ulen>12)
		{
			(ulen)--;
			udata++;
			goto __date_rev;
		}
		else
		{
			printf("\r\ns->m data stat err");
//			point3_last=point3_now; //留着一段数据,下一次判断是否有继续接收
			return -1;	
		}
	}

//	for(i=0;i<ulen;i++) printf("%x ",udata[i]);
	
	date_len=(udata[8]<<8)+udata[9]; //数据长度
		
	*pack_point=udata;
	*pack_len = date_len+12; //先给出结果 大于 等于 小于 都需要输出完整的包大小
		
	if(*pack_len>=lenmax3)
	{
		printf("\r\ns->m package length err:");
		return -1;	
	}
	
	if(*pack_len > ulen)
	{
		printf("\r\ns->m data length err:");
		return -1;	
	}
	
	if(udata[date_len+11]!=0x16)
	{
		printf("\r\ns->m data end err");
		return -1;	
	}
	
	if((checksum_4g(&udata[1],date_len+6+1+2)!=udata[date_len+10]))
	{
//		HAL_UART_Receive_IT(&huart3,(unsigned char *)urd3,lenmax3);
//		huart3.GetPoint=0;//huart->NowPoint;
//		huart3.ReadPoint=0;//=huart->NowPoint;
//		huart3.NowPoint=0;
		printf("\r\ns->m data checksum err");
//		for(i=0;i<ulen;i++) printf("%02x ",udata[i]);
//		bAAAAA=1;
		return -1;	
	}
		
	printf("\r\nS->M %x,%d:%d:%d",udata[7],systime[3],systime[4],systime[5]);
	
	
	if(ulen>=*pack_len+12) //还有另外一包的大小
	{
//		printf("\r\nS->M 连发两包");
//		printf("\r\nhuart3.GetPoint=%d ",huart3.GetPoint);
		if(huart3.GetPoint<(*pack_len))
		{
			huart3.GetPoint=huart3.GetPoint+lenmax3-(ulen-(*pack_len));
		}
		else huart3.GetPoint -= (ulen-(*pack_len)); //退格 剩余包数
		
//		printf("\r\nhuart3.GetPoint=%d ",huart3.GetPoint);
		
		huart3.ReadPoint=huart3.GetPoint;
//		printf("\r\n");
//		for(i=0;i<ulen;i++) printf("%02x ",udata[i]);
//		printf("\r\n%02x ",urd3[huart3.GetPoint]);
	}
	
//	*udata=rdata;		//数据地址指向
	
	

//	if(ulen >= date_len+12+12) //还有另外一包的大小
//	{
//		printf("\r\nS->M 连发两包");
//		if(huart3.GetPoint >= date_len+12)
//		{
//			 huart3.GetPoint -= (date_len+12);
//		}
//		else
//		{
//			huart3.GetPoint =huart3.GetPoint + huart3.RxXferSize - (date_len+12);
//		}
//	}
	
	return 2;	//数据OK
}
extern uint8_t sleepdelay;
extern uint8_t function22_flag,function25_flag,function84_flag;

#define FLASH_BANK1_BASE	0x08000000
#define FLASH_BANK2_BASE	0x08080000
#define Boot_ADD			FLASH_BANK1_BASE

void (*jump2app)();
int iap_load_app(uint32_t appxaddr)
{
	if((((*(volatile uint32_t*)appxaddr)&0x2FFE0000)==0x20000000)||(((*(volatile uint32_t*)appxaddr)&0x1FFE0000)==0x10000000))	//检查栈顶地址是否合法.
	{
		printf("\r\n转入%08x",appxaddr);
		jump2app=(void(*)())*(volatile uint32_t*)(appxaddr+4);		//用户代码区第二个字为程序开始地址(复位地址)		
		__set_MSP(*(volatile uint32_t*) appxaddr);//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		
		for(int i = 0; i < 8; i++)
		{			
			NVIC->ICER[i] = 0xFFFFFFFF;	/* 关闭中断*/
			NVIC->ICPR[i] = 0xFFFFFFFF;	/* 清除中断标志位 */
		}
		jump2app();									//跳转到APP.
		
		printf("\r\n转入%08x没成功?",appxaddr);
	}
	else
	{
		printf("\r\niap栈顶地址错误");
		return -1;
	}
	return 0;
}

void Rev4g_polling(void * p_arg)
{
	OS_ERR      err;
//	int state;
	uint8_t *pack_in;
	uint16_t pack_len,buf_len;
	(void)p_arg;	
	uint8_t cmd;
	uint8_t *data_in;
	uint16_t data_len;
	uint16_t i,j;
	uint8_t *star_add;
	int ur_err;
	
//	printf("1");
//	UART_data_clear(&huart3);
//	SRAM_Test();
	
//	while(1)
	{
//		OSTimeDly ( 20, OS_OPT_TIME_DLY, & err );	
//		if(b1s)
//		{
//			b1s=0;
//			printf("\r\n1s到");
//		}
		
		ur_err=get_nw_data(&pack_in,&pack_len);
//#ifndef	ur3_rdma
//		if(ur_err==-1)
//		{
//			if((((huart3.Instance->CR1)&(USART_CR1_PEIE | USART_CR1_RXNEIE)) != (USART_CR1_PEIE | USART_CR1_RXNEIE))||(terrorflags))
//			{
//				printf("\r\n------------------------terrorflags=%x",terrorflags);
//				terrorflags=0;
//		//		printf("\r\nCR1=%04x",huart3.Instance->CR1);
//				
//				HAL_UART_Abort_IT(&huart3);
//				HAL_UART_Receive_IT(&huart3,(unsigned char *)urd3,lenmax3);
//		//		printf("\r\nuart3错误重启");
//			}
//			
////			HAL_UART_Abort_IT(&huart3);
////			HAL_UART_Receive_IT(&huart3,(unsigned char *)urd3,lenmax3);
////			printf("\r\nuart3重启");
//		}
//#endif
		if(ur_err==2) //收到完整的南网数据包
		{		
//			STM32_sleeping=0;
			cmd=pack_in[7];
			data_len=(pack_in[8]<<8)+pack_in[9];
			data_in=pack_in+10;
			if(sleepdelay<6)	sleepdelay=6;
			if(HK_post_cnt>HK_post_max) HK_post_cnt=0; //

			switch(cmd)
			{
				case 	updata_online:			//在线升级
								
				if(memcmp(CHW,&data_in[1],4)==0)	//密码正确
				{
					if(data_in[0]==1)
					{
						nw_sent_8(pack_in,pack_len);
						updata_flag[0]=0x55;
						
//						__set_CONTROL(0);					
//						iap_load_app(Boot_ADD);
						HAL_NVIC_SystemReset();
					}
				}
				else
				{
					rev_err(updata_online,0xff); //密码错误
				}
				break;
							
				case 	Contact:			//00	//	开机联络信息	装置开机上送联络信息
								
				if(senttask_Pend & data_flag00)	//正等待答复 
				{
					if((data_in[0]==ver[0])&&(data_in[1]==ver[1]))
					{
						senttask_Pend &= ~data_flag00;
					}
				}
				else
//					senttask_Asim |= data_flag00;
				
					break;
					
				case  CheckT:				//01	//	校时	该命令分请求和下发两部分
					if((data_in[0]<y_min)||(data_in[0]>y_max)||(data_in[1]==0)||(data_in[1]>12)||(data_in[2]==0)||(data_in[2]>31))
					{
						printf("\r\n校时时间超出正常时间");
					}
					else
					{
						if(senttask_Pend & data_flag01) //请求校时
						{
							if((Contact_time)<200)
							{
								senttask_Pend &= (~data_flag01);
								if(data_in[0])
								{
									memcpy(systime,data_in,6);
									sys_time_chang();
									nw_sent_8(pack_in,pack_len);//??需要原命令返回吗?
								}
							}
						}
						else
						{
							memcpy(systime,data_in,6);
							sys_time_chang();
							OSTimeDly ( 200, OS_OPT_TIME_DLY, & err );	
							nw_sent_8(pack_in,pack_len);//sent_CheckT(0);
						}
						 
					}
							
										
					break;
					
				case  SetPassword :	//02	//	设置装置密码	装置出厂密码：字符：‘1234’（31H32H33H34H）
					printf("\r\nPSW=%x,%x,%x,%x",PSW[0],PSW[1],PSW[2],PSW[3]);
					if(memcmp(PSW,data_in,4)==0) //localpassw[50]
					{
						memcpy(PSW,&data_in[4],4);
						flash_save(PSW,PSW,4);
						nw_sent_8(pack_in,pack_len);
					}
					else
					{
						rev_err(SetPassword,0xff);
					}
					printf("\r\ndata_in=%x,%x,%x,%x",data_in[0],data_in[1],data_in[2],data_in[3]);
					printf("\r\nPSW=%x,%x,%x,%x",PSW[0],PSW[1],PSW[2],PSW[3]);
					break;
					
				case  Pcfg :				//03	//	主站下发参数配置	该指令要求数据采集装置接收到该命令后原命令返回
					
//3,0,12,31,32,33,34,  3c, 2,58, 0,0,  2,58,  0,2,3,  31,32,33,34,  b,16,
//					printf("\r\n本地密码PSW=%x,%x,%x,%x",PSW[0],PSW[1],PSW[2],PSW[3]);
//					printf("\r\n接收密码%x,%x,%x,%x",data_in[0],data_in[1],data_in[2],data_in[3]);
				
//					printf("\r\ndata_in=%x,%x,%x,%x",data_in[0],data_in[1],data_in[2],data_in[3]);
				
					if((memcmp(PSW,data_in,4)==0)&&(memcmp(CHW,&data_in[14],4)==0)) //localpassw[50]
					{
						*HBT=data_in[4];
						*GDT=(data_in[5]<<8)+data_in[6];
						*SLT=(data_in[7]<<8)+data_in[8];
						*OLT=(data_in[9]<<8)+data_in[10];
						RST[0]=data_in[11]; //日 0-28
						RST[1]=data_in[12];	//时 0-24
						RST[2]=data_in[13];	//分 0-60
						
						flash_save((uint8_t *)HBT,(uint8_t *)HBT,11);
						OSTimeDly( 100, OS_OPT_TIME_DLY, & err);
						nw_sent_8(pack_in,pack_len);
						
						printf("RST[0]=%d,RST[1]=%d,RST[2]=%d",RST[0],RST[1],RST[2]);
						if(RST[0]==0)
						{
							if(((RST[1]==systime[3])&&(RST[2]==systime[4])))
							{
								printf("准备复位");
								STM32_rest_delay=5;
							}
						}
					}
					else
					{
						rev_err(Pcfg,0xff);
					}
					break;
				case  HeartBeat:		//05	//	装置心跳信息	用于主站监测装置上线时间、IP地址和端口号、传输信号强度及蓄电池电压
					
//				if(ph_sent_cnt<ph_gain_cnt)	//有照片数据发
//				{
//					senttask_Asim |= data_flag84_1;
//				}
				
//					senttask_Asim |= data_flag05_ok;
					if(senttask_Pend & data_flag05)
					{
//						if(sleepdelay==2) sleepdelay=1;	//如果是心跳，把之前的延时改为1s，可以立即休眠
						senttask_Pend &= (~data_flag05);
					}
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
							
//							flash_save(HostIP,HostIP,6); //更改IP成功才保存
							
							nw_sent_8(pack_in,pack_len);
							ip_seting=1;
							senttask_Asim |= data_flag06; 	//需复位4g驱动
	//						STM32_rest_delay=5;

							ip_seting=1;
							printf("\r\n准备更改IP为 %d.%d.%d.%d,%d",HostIP[0],HostIP[1],HostIP[2],HostIP[3],*HostPort);
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
				case  check_ip:			//07 查询主站IP地址、端口号和卡号,直接读flash的，sram的可能为更改未完成的
//				if(data_len==0)
					{
						nw_sc.cmd=check_ip;
						nw_sc.len=12;
						flash_read(HostIP,nw_sc.data,6);// nw_sc.data[4],nw_sc.data[5] 为 *HostPort,大小端调换
						nw_sc.data[13]=nw_sc.data[4]; //nw_sc.data[13]临时buf
						nw_sc.data[4]=nw_sc.data[5];
						nw_sc.data[5]=nw_sc.data[13];
//						memcpy(nw_sc.data,HostIP,4); //HostIP[0],HostIP[1],HostIP[2],HostIP[3],*HostPort
//						nw_sc.data[4]=*HostPort>>8;
//						nw_sc.data[5]=*HostPort;
//						memcpy(&nw_sc.data[6],nw_phone_num,6);//nw_phone_num
						memset(&nw_sc.data[6],0,6);
						OSTimeDly( 100, OS_OPT_TIME_DLY, & err);
						nw_sent_st(&nw_sc);
					}
					 
					break;
				case  ResetDev:			//08	//	装置复位	主站对装置进行复位
					if(memcmp(PSW,data_in,4)==0)
					{
						nw_sent_8(pack_in,pack_len);
//						STM32_reseting=1;
						STM32_rest_delay=1;
					}
					else
					{
						rev_err(ResetDev,0xff);
					}
					 
					break;
				case  waikup_device_m://09	//	短信唤醒	主站以短信方式唤醒休眠状态的装置
//					sys_mode=MODE_WORK;
//					wakeup_dev();
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
								nw_sc.data[i++]=*WidthxHeight;
//								Streaming_OK=1;
							}		
//							if(GET_Image_s()==0) //图像1亮度,对比度,饱和度
							{
								nw_sc.data[i++]=*bLevel;nw_sc.data[i++]=*cLevel;nw_sc.data[i++]=*sLevel;
//								if(Streaming_OK==1) Streaming_OK=2;
							}
						}
						nw_sc.data[i++]=1;nw_sc.data[i++]=1;nw_sc.data[i++]=60;nw_sc.data[i++]=60;nw_sc.data[i++]=60; //通道2配置
						
						for(j=0;j<function_nub[0];j++)
						{
							nw_sc.data[i++]=function_buf[j];
						}
//						memcpy(&nw_sc.data[i],function_buf,strlen((char *)function_buf));
						nw_sc.len=i;//+strlen((char *)function_buf);
//						nw_sc.data=nw_sc.data;
						nw_sent_st(&nw_sc);
					}
					break;
				case  device_function_cfg://0B	//	装置功能配置	
					 
					if(memcmp(PSW,data_in,4)==0)
					{
						function_nub[0]=data_len-4;
						memcpy(function_buf,&data_in[4],function_nub[0]);
						function_buf[data_len-4]=0; //结尾给0
						
						function22_flag=0;
						function25_flag=0;
						function84_flag=0;
						printf(",\t有效功能:");
						for(j=0;j<function_nub[0];j++)
						{
							if(function_buf[j]==0x22)
							{
								function22_flag=1;
								printf("0x%02x,",function_buf[j]);
							}
							if(function_buf[j]==0x25)
							{
								function25_flag=1;
								printf("0x%02x,",function_buf[j]);
							}
							if(function_buf[j]==0x84)
							{
								function84_flag=1;
								printf("0x%02x,",function_buf[j]);
							}
						}
						printf(",\t无效配置:");
						for(j=0;j<function_nub[0];j++)
						{
							if((function_buf[j]!=0x22)&&(function_buf[j]!=0x25)&&(function_buf[j]!=0x84))
							{
								printf("0x%02x,",function_buf[j]);
								function_buf[j]=0;
							}
						}
						function_nub[0]=0;
						if(function22_flag)
						{
							function_buf[function_nub[0]++]=0x22;
						}
						if(function25_flag)
						{
							function_buf[function_nub[0]++]=0x25;
						}
						if(function84_flag)
						{
							function_buf[function_nub[0]++]=0x84;
						}
						flash_save(function_nub,function_nub,(function_nub[0]+1));
						
						nw_sent_8(pack_in,pack_len);
					}
					else
					{
						rev_err(device_function_cfg,0xff);
					}
					break;
				case  device_Sleep:	//0C	//	装置休眠	
					 
					wr_sleep=1;
					break;
				case  check_device_time://0D	//	查询装置设备时间	
					 
					nw_sc.cmd=check_device_time;
					nw_sc.len=6;
//					nw_sc.data=systime;
					memcpy(nw_sc.data,systime,6);
					nw_sent_st(&nw_sc);
					break;
				case  sent_msg:
					 
//					nw_sent(nw_r_post);
//				发短信	https://blog.csdn.net/qq_30460905/article/details/81805891
					break;
				case  request_data:	//21	//	主站请求装置数据	主站请求采集装置采集数据并立即上送
					 
					if(data_len==0)//如果数据域为0字节，上传未成功上传的历史数据，包含历史照片，若装置无历史数据则不上传
					{
						if(function22_flag) senttask_Asim |= data_flag22;
						if(function25_flag) senttask_Asim |= data_flag25;
						Contact_state=1;
					}
					else if((data_len==2)&&(data_in[0]==0xBB)&&(data_in[1]==0xBB))//装置立刻采集所有数据（图片除外），完成采集后立刻上传。该次采样不影响原设定采集间隔的执行
					{
						printf("\r\n立即采集数据上传");
						if(function22_flag)
						{
							get_a_data_ll();	//
							if(gain_ll_data()==0) senttask_Asim |= data_flag22;
						}
						if(function25_flag)
						{
							if(gain_qx_data()==0) senttask_Asim |= data_flag25;
						}
						Contact_state=1;
					}
//					nw_sent(nw_r_post);
					break;

					case  request_la_qj://22	//	上传拉力数据	装置收到该命令后立即将所有未传送成功的数据上送主站。
						
					if(data_len==0)//主站请求数据
					{
						printf("\r\n请求拉力数据");
//						get_a_data_ll();	//1s一次
//						gain_ll_data();
//						LastGDT=work_p100ms;
						if(function22_flag) senttask_Asim |= data_flag22;	//
						else
						{
							printf("\r\n没有配置22功能");
						}
						Contact_state=1;
						 
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
//								if(requestdata)
//								{
//								}
							}
							else
							{
//								printf("\r\n数据标识号错误,标识号应为%x",sampled_data_nub);
							}
						}
					}
					break;
				case  request_qx:		//0x25	//	上传气象数据	装置收到该命令后立即将所有未传送成功的数据上送主站。
					if(data_len==0)//主站请求数据
					{
						printf("\r\n请求气象数据");
//						get_qx_data();
//						LastGDT=work_p100ms;
						if(function25_flag) senttask_Asim |= data_flag25;
						else
						{
							printf("\r\n没有配置25功能");
						}
						Contact_state=1;
						 
					}
					else if(data_len==3)
					{
//						第几帧标识 1字节	AA55H
						if((data_in[1]==0xAA)&&(data_in[2]==0x55))
						{
//							printf("\r\n收到主站答复25数据,标识号为%x",data_in[0]);
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
					
				case 0x81:	//图像采集参数配置
					 
				if(memcmp(PSW,data_in,4)==0)
				{
					star_add=&HK_post_buf[HK_post_cnt];
					
					HK_post_buf[HK_post_cnt++]=cmd;
//					HK_post_buf[HK_post_cnt++]=data_in[4]; //黑白
					HK_post_buf[HK_post_cnt++]=data_in[5]; //大小
					HK_post_buf[HK_post_cnt++]=data_in[6];//亮度
					HK_post_buf[HK_post_cnt++]=data_in[7];//对比度
					HK_post_buf[HK_post_cnt++]=data_in[8];//饱和度
					
					OSQPost(&hktask_Asim[0],star_add,5,OS_OPT_POST_FIFO,&err);
					
					HK_post_buf[HK_post_cnt++]=cmd;
//					HK_post_buf[HK_post_cnt++]=data_in[9]; //黑白					
					HK_post_buf[HK_post_cnt++]=data_in[10]; //大小
					HK_post_buf[HK_post_cnt++]=data_in[11];
					HK_post_buf[HK_post_cnt++]=data_in[12];
					HK_post_buf[HK_post_cnt++]=data_in[13];
					
					OSQPost(&hktask_Asim[1],star_add,5,OS_OPT_POST_FIFO,&err);
				}
				else
				{
					rev_err(0x81,0xff);
				}
				break;
				
			case 0x82:	//拍照时间表设置
				 
				if((memcmp(PSW,data_in,4)==0)&&(data_in[5]<=255))
				{
//密码0-3	通道号4	组数5					第1组
//											时			分			预置位号
//4字节	1字节		1字节		1字节		1字节			1字节		
					buf_len=data_in[5]*3+1;
					if(data_in[4]==1)
					{
						memcpy(aut_point_group1,&data_in[5],buf_len);
						flash_save(aut_point_group1,aut_point_group1,buf_len);
						nw_sent_8(pack_in,pack_len);
					}
					if(data_in[4]==2)
					{
						memcpy(aut_point_group2,&data_in[5],buf_len);
						flash_save(aut_point_group2,aut_point_group2,buf_len);
						nw_sent_8(pack_in,pack_len);
					}
				}
				else
				{
					rev_err(0x82,0xff);
				}
				break;
				
				case 0x8b:	//查询拍照时间表
					 
//				if(memcmp(PSW,nw_r_post->data,4)==0)
				{
//					printf("\r\n查询拍照时间");
					nw_sent_ph_time(data_in[0]); //通道号					
				}
				break;
					
				case 0x83:	//主站请求拍摄照片
					if(function84_flag)
					{
						star_add=&HK_post_buf[HK_post_cnt];
						HK_post_buf[HK_post_cnt++]=cmd; //拍照指令
						HK_post_buf[HK_post_cnt++]=data_in[1];	//预置位
						if(data_in[0]==1) OSQPost(&hktask_Asim[0],star_add,2,OS_OPT_POST_FIFO,&err);
						if(data_in[0]==2) OSQPost(&hktask_Asim[1],star_add,2,OS_OPT_POST_FIFO,&err);
						nw_sent_8(pack_in,pack_len);//立即原命令返回
					}
					else
					{
						printf("\r\n没有配置84功能");
					}
					
					break;
				
				case 0x84:	//采集终端请求上送照片答复
						 
						senttask_Pend &= (~data_flag84_1);

					break;
					
//					case 0x85:	//图像数据上送，只有发
//					break;
					
					case 0x86:	//图像数据上送结束标记,只有发
//						senttask_Pend &= (~data_flag86_1);
					break;
					
					case 0x87:	//补包数据下发
//						senttask_Pend &= (~data_flag86_1); //收到87答复
					if(senttask_Pend & data_flag86_1)
					{
						if(data_in[2]==0)	//不需补包
						{
							senttask_Pend &= (~data_flag86_1); 	//数据包成功
//							printf("\r\nsend photo OK");
						}
						else //需要补发
						{
							 
							resent_ph(data_in,data_len);
						}
					}
					break;
			
			
				case 0x88:	//摄像机远程调节
					 
				if(memcmp(PSW,data_in,4)==0) //localpassw[50]
				{			
//密码	通道号	动作指令	指令参数
//4字节	1字节	1字节	1字节
					star_add=&HK_post_buf[HK_post_cnt];
					HK_post_buf[HK_post_cnt++]=data_in[5]; //动作指令
					HK_post_buf[HK_post_cnt++]=data_in[6];	//指令参数
					if(data_in[4]==1) OSQPost(&hktask_Asim[0],star_add,2,OS_OPT_POST_FIFO,&err);
					if(data_in[4]==2) OSQPost(&hktask_Asim[1],star_add,2,OS_OPT_POST_FIFO,&err);
//				nw_sent_8(pack_in,pack_len);//立即原命令返回						
				}
				else
				{
					rev_err(0x88,0xff);
				}
				break;	

			default:
				break;
			}
		}
	}
}



