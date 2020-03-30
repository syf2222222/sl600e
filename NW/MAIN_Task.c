


#define  NW_MODULE
#include "global.h"
#include "nw.h"
#undef NW_MODULE

#include "sram.h"
#include "timer.h"
#include "usart.h"
#include "bsp_uart.h"
#include "FM25V10.h"
#include "power_ctrl.h"

void  nw_sent_st(struct nw_ctl *t_data);
void  nw_sent_8(uint8_t *t_data,uint8_t len);
void  nw_sent_ph_time(uint8_t ch);
void  nw_sent_ph(unsigned char *input,uint16_t len,uint16_t peket);

int get_qx_data(void);
char polling_ll_data();	//1s一次
char save_ll_data();

unsigned char sampled_step,sampled_data_nub,sampled_data_flag;
unsigned char WidthxHeight,STM32_rest_delay;
unsigned char bLevel,cLevel,sLevel;

uint8_t sent_cnt;
int duty_time,LastTxT;
int LastGDT=0,LastHBT=0,LastOLT=0;
const unsigned char ver[2]={1,0};

#define testdvi	5
unsigned char ip_seting;

unsigned short pk_nb,tt_pk_nb;
unsigned int tt_len;
#define		nw_pk_len		1024 //1460
#define		nw_pk_max		200000


extern void UART_data_clear(UART_HandleTypeDef *huart);	
char UART_Receive(UART_HandleTypeDef *huart,uint8_t **pack_point, uint16_t *pack_len,int wait_time);
char UART_Receive_s(UART_HandleTypeDef *huart,uint8_t **pack_point, uint16_t *pack_len,int time_out);
unsigned char checksum_4g(unsigned char* a,unsigned int n);

uint8_t HK_post_buf[300];//
uint16_t HK_post_cnt=0;

char check_signel(void);
int Get_Volt(void);
void sent_ll_data_22(unsigned char *input,unsigned char zbs);
void sent_qx_data_25(unsigned char *input,unsigned char zbs);
char hav_ll_data,hav_qx_data,hav_ph_data;
uint8_t nw_step=0,nw_tast_old,nw_tast_step;


void  NW_MAIN_Task  ( void * p_arg )//void  NWTask_task( void * p_arg )
{
	OS_ERR      err; 
	
	(void)p_arg;	
	char buf;
	uint8_t *star_add;
	
	function_buf[0]=0x22;
	function_buf[1]=0x25;
	function_nub=2;
	
	LastTxT=worktime;
	nw_step=0;
	sent_cnt=0;
	duty_time=0;
	nw_tast_step=0;
	
	while (DEF_TRUE)
	{
		OSTimeDly( 10, OS_OPT_TIME_DLY, & err);
		
	}

		
	while (DEF_TRUE)
	{		
		if(nw_tast_old != nw_tast_step)
		{
			nw_tast_old=nw_tast_step;
			nw_step=0;
			sent_cnt=0;
		}
		switch(nw_tast_step)
		{
			case 0: //开机,联络,校时(检测4g-ur5信号)
				OSTimeDly( 1000, OS_OPT_TIME_DLY, & err);
				nw_tast_poweron();
			break;
			
			case 1:	//心跳时间,数据上传时间,拍照时间,复位时间
				OSTimeDly( 100, OS_OPT_TIME_DLY, & err);
				nw_tast_step=check_work();		//获取工作量
			break;
			
			case 5:	//发心跳
				if(nw_step==0)
				{
//					OSTimeDly( 100, OS_OPT_TIME_DLY, & err);	
					sent_HeartBeat();
					nw_step++;
				}
				else
				{
					OSTimeDly( 100, OS_OPT_TIME_DLY, & err);	
					if(senttask_Pend & data_flag05)
					{
						nw_tast_step=1;
						OSTimeDly( 1000, OS_OPT_TIME_DLY, & err);	
					}
				}
			
//				buf=check_work();
//				if(buf) nw_tast_step=buf;
			break;
			
			case 6:	//
				if(ip_seting)
				{
					if(nw_step==0)
					{
						DTU_reset();
					}
					if(nw_step==1)
					{
						OSTimeDly( 1000, OS_OPT_TIME_DLY, & err);
						if(check_sim7600()==2) nw_step++; 				//7600准备好
					}
					if(nw_step==2)
					{
						if(change_ip_done()==1)
						{
							ip_seting=0;
							printf("\r\nchange ip ok");
							nw_tast_step=1;
						}	
					}
				}
			
			break;
			
			case 0x22:
				OSTimeDly( 100, OS_OPT_TIME_DLY, & err);
				if((sent_cnt==0)&&(nw_step<3))
				{
//					printf("\r\n包数0=%x",ll_data_buf[nw_step][0]);
					sampled_data_nub++;
					if(sampled_data_nub>0x49) sampled_data_nub=0;
					sent_ll_data_22(ll_data_buf[nw_step],sampled_data_nub);
					sent_cnt++;
				}
				
				if((senttask_Pend & data_flag22)==0)//发完一组 或没有数据需要发
				{
					if(nw_step<3) //下一包
					{
						ll_data_buf[nw_step][0]=0;
						sent_cnt=0;
						nw_step++;
					}
					else //发完全部
					{
						nw_tast_step=1;
					}
				}
				else if((worktime-LastTxT)>=5) //没收到回复
				{
					printf("\r\n22指令答复超时");
					if(sent_cnt>=2)	//发下一包
					{
						nw_tast_step=1; //超时退出
//						nw_step++;
//						sent_cnt=0;
					}
					else	if(nw_step<3) //重发包
					{
						printf("\r\n包数0=%x",ll_data_buf[nw_step][0]);
						sent_ll_data_22(ll_data_buf[nw_step],sampled_data_nub);
						sent_cnt++;
					}
					else
					{
						nw_tast_step=1; //超时退出
					}
				}

			break;
		
		case 0x25:		
				if(nw_step==0)
				{
					sampled_data_nub++;
					sent_qx_data_25(qx_data_buf,sampled_data_nub);
					nw_step++;
				}
				OSTimeDly( 100, OS_OPT_TIME_DLY, & err);
				
				if((senttask_Pend & data_flag25)==0)	//发完一组 或没有数据需要发
				{
					qx_data_buf[0]=0;		
					nw_tast_step=1;
				}
				else if((worktime-LastTxT)>=5) //没收到回复
				{
//					senttask_Pend &= (~data_flag25);
//					nw_tast_step=1;
					
					printf("\r\n25指令答复超时");
					if(sent_cnt>=2)	//发下一包
					{
						nw_tast_step=1; //超时退出
					}
					else//	 重发包
					{
						sent_qx_data_25(qx_data_buf,sampled_data_nub);
						sent_cnt++;
					}
				}
		break;
			
			case 0x81: //配置图片参数原命令返回
			{
				
			}
			break;
			
			case 0x84: //请求上送照片 1s重发,要求3s,最多5次		
				if(sent_cnt==0)
				{
					nw_sc.cmd=0x84;
					nw_sc.len=10;		
					memcpy(nw_sc.data,PH_GET_T,8); //time + ch + preset
					tt_len=((PH_LEN[0])<<0) +(PH_LEN[1]<<8)+(PH_LEN[1]<<16);
					if(tt_len>nw_pk_max) tt_len=nw_pk_max;
					tt_pk_nb=(tt_len/nw_pk_len);//1024一次
					if(tt_len%nw_pk_len) tt_pk_nb +=1;
					nw_sc.data[8]=(tt_pk_nb>>8);//包数高位
					nw_sc.data[9]=tt_pk_nb;			//包数低位
					
					nw_sent_st(&nw_sc);
					senttask_Pend |= data_flag84_1;
					sent_cnt++;
					printf("\r\n发完84");
				}
				else	if((senttask_Pend & data_flag84_1)==0)	//收到答复
				{
					nw_tast_step=0x85;
					printf("\r\n收到南网同意上传");
				}
				else if((worktime-LastTxT)>=3)
				{
//					LastTxT=worktime;
					printf("\r\n超时%x",sent_cnt);
					if(sent_cnt>=5) nw_tast_step=1;
					else
					{
						nw_sent_st(&nw_sc);
						sent_cnt++;
					}
				}
				else
				{
					OSTimeDly( 100, OS_OPT_TIME_DLY, & err);
//					printf(".");
				}
			break;
				
			case 0x85: //开始上传照片
			{
				OSTimeDly ( 100, OS_OPT_TIME_DLY, & err ); 
				printf("\r\nsend photo start,pk_nb=%d,tt_len=%d,",tt_pk_nb,tt_len);
				for(pk_nb=1;pk_nb<=tt_pk_nb;pk_nb++)
				{
					if(pk_nb==tt_pk_nb) nw_sent_ph(PH_DATA+((pk_nb-1)*nw_pk_len),tt_len-((pk_nb-1)*nw_pk_len),pk_nb);
					else	nw_sent_ph(PH_DATA+((pk_nb-1)*nw_pk_len),nw_pk_len,pk_nb);
//					printf("\r\nA");
//					OSTimeDly ( 3, OS_OPT_TIME_DLY, & err ); 
				}
				printf("\r\nsend photo complete");
				
				OSTimeDly ( 2000, OS_OPT_TIME_DLY, & err ); 
				sent_cnt=0;
				nw_tast_step=0x86;	//发完,准备发 0x86指令等南网应答
			}
			break;
			case 0x86: //上传结束
				if(sent_cnt==0)
				{
					nw_sc.cmd=0x86;//数据结束
					nw_sc.len=2;			
					nw_sc.data[0]=1;//通道号	
					nw_sc.data[1]=0;//预置位号
					nw_sent_st(&nw_sc);
					senttask_Pend |= data_flag86_1;
					sent_cnt++;
				}
				else	if((senttask_Pend & data_flag86_1)==0)
				{
					nw_tast_step=1;
				}
				else if((worktime-LastTxT)>=30/testdvi)
				{
//					LastTxT=worktime;
					printf("\r\n超时%x",sent_cnt);
					if(sent_cnt>=5) nw_tast_step=1;
					else
					{
						nw_sent_st(&nw_sc);
						sent_cnt++;
					}
				}
				else
				{
					OSTimeDly( 100, OS_OPT_TIME_DLY, & err);
//					printf(".");
				}
			break;
		}
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

void NW_HK_Task_Create (void);

OS_SEM read_RSSI;

void NWTaskStart (void *p_arg)
{
	OS_ERR      err;
	(void)p_arg;

	

//		多值信号量控制块指针		//多值信号量名称 //资源数目或事件是否发生标志 //返回错误类型
	OSSemCreate (&read_RSSI, "read_RSSI",  0, &err); //3 				

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


