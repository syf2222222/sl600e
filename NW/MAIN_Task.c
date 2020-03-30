


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
char polling_ll_data();	//1sһ��
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
			case 0: //����,����,Уʱ(���4g-ur5�ź�)
				OSTimeDly( 1000, OS_OPT_TIME_DLY, & err);
				nw_tast_poweron();
			break;
			
			case 1:	//����ʱ��,�����ϴ�ʱ��,����ʱ��,��λʱ��
				OSTimeDly( 100, OS_OPT_TIME_DLY, & err);
				nw_tast_step=check_work();		//��ȡ������
			break;
			
			case 5:	//������
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
						if(check_sim7600()==2) nw_step++; 				//7600׼����
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
//					printf("\r\n����0=%x",ll_data_buf[nw_step][0]);
					sampled_data_nub++;
					if(sampled_data_nub>0x49) sampled_data_nub=0;
					sent_ll_data_22(ll_data_buf[nw_step],sampled_data_nub);
					sent_cnt++;
				}
				
				if((senttask_Pend & data_flag22)==0)//����һ�� ��û��������Ҫ��
				{
					if(nw_step<3) //��һ��
					{
						ll_data_buf[nw_step][0]=0;
						sent_cnt=0;
						nw_step++;
					}
					else //����ȫ��
					{
						nw_tast_step=1;
					}
				}
				else if((worktime-LastTxT)>=5) //û�յ��ظ�
				{
					printf("\r\n22ָ��𸴳�ʱ");
					if(sent_cnt>=2)	//����һ��
					{
						nw_tast_step=1; //��ʱ�˳�
//						nw_step++;
//						sent_cnt=0;
					}
					else	if(nw_step<3) //�ط���
					{
						printf("\r\n����0=%x",ll_data_buf[nw_step][0]);
						sent_ll_data_22(ll_data_buf[nw_step],sampled_data_nub);
						sent_cnt++;
					}
					else
					{
						nw_tast_step=1; //��ʱ�˳�
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
				
				if((senttask_Pend & data_flag25)==0)	//����һ�� ��û��������Ҫ��
				{
					qx_data_buf[0]=0;		
					nw_tast_step=1;
				}
				else if((worktime-LastTxT)>=5) //û�յ��ظ�
				{
//					senttask_Pend &= (~data_flag25);
//					nw_tast_step=1;
					
					printf("\r\n25ָ��𸴳�ʱ");
					if(sent_cnt>=2)	//����һ��
					{
						nw_tast_step=1; //��ʱ�˳�
					}
					else//	 �ط���
					{
						sent_qx_data_25(qx_data_buf,sampled_data_nub);
						sent_cnt++;
					}
				}
		break;
			
			case 0x81: //����ͼƬ����ԭ�����
			{
				
			}
			break;
			
			case 0x84: //����������Ƭ 1s�ط�,Ҫ��3s,���5��		
				if(sent_cnt==0)
				{
					nw_sc.cmd=0x84;
					nw_sc.len=10;		
					memcpy(nw_sc.data,PH_GET_T,8); //time + ch + preset
					tt_len=((PH_LEN[0])<<0) +(PH_LEN[1]<<8)+(PH_LEN[1]<<16);
					if(tt_len>nw_pk_max) tt_len=nw_pk_max;
					tt_pk_nb=(tt_len/nw_pk_len);//1024һ��
					if(tt_len%nw_pk_len) tt_pk_nb +=1;
					nw_sc.data[8]=(tt_pk_nb>>8);//������λ
					nw_sc.data[9]=tt_pk_nb;			//������λ
					
					nw_sent_st(&nw_sc);
					senttask_Pend |= data_flag84_1;
					sent_cnt++;
					printf("\r\n����84");
				}
				else	if((senttask_Pend & data_flag84_1)==0)	//�յ���
				{
					nw_tast_step=0x85;
					printf("\r\n�յ�����ͬ���ϴ�");
				}
				else if((worktime-LastTxT)>=3)
				{
//					LastTxT=worktime;
					printf("\r\n��ʱ%x",sent_cnt);
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
				
			case 0x85: //��ʼ�ϴ���Ƭ
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
				nw_tast_step=0x86;	//����,׼���� 0x86ָ�������Ӧ��
			}
			break;
			case 0x86: //�ϴ�����
				if(sent_cnt==0)
				{
					nw_sc.cmd=0x86;//���ݽ���
					nw_sc.len=2;			
					nw_sc.data[0]=1;//ͨ����	
					nw_sc.data[1]=0;//Ԥ��λ��
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
					printf("\r\n��ʱ%x",sent_cnt);
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
OS_TCB		NW_REV_Task_TCB;															//������ƿ�
CPU_STK		NW_REV_Task_Stk [ NW_REV_Task_SIZE ];	//�����ջ

#define		NW_SENT_Task_SIZE                     512
#define		NW_SENT_Task_PRIO                         9
OS_TCB		NW_SENT_Task_TCB;															//������ƿ�
CPU_STK		NW_SENT_Task_Stk [ NW_SENT_Task_SIZE ];	//�����ջ




void NW_REV_Task  (void * p_arg );
void NW_SENT_Task (void *p_arg);

void NW_HK_Task_Create (void);

OS_SEM read_RSSI;

void NWTaskStart (void *p_arg)
{
	OS_ERR      err;
	(void)p_arg;

	

//		��ֵ�ź������ƿ�ָ��		//��ֵ�ź������� //��Դ��Ŀ���¼��Ƿ�����־ //���ش�������
	OSSemCreate (&read_RSSI, "read_RSSI",  0, &err); //3 				

#if 1
						 
		/* ����  �������� ���� */
    OSTaskCreate((OS_TCB     *)&NW_REV_Task_TCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"NW_REV_Task_Name",                             //��������
                 (OS_TASK_PTR ) NW_REV_Task,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) NW_REV_Task_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&NW_REV_Task_Stk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) NW_REV_Task_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) NW_REV_Task_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������
#endif	
								 
#if 1
						 
		/* ����  �������� ���� */
    OSTaskCreate((OS_TCB     *)&NW_SENT_Task_TCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"NW_SENT_Task_Name",                             //��������
                 (OS_TASK_PTR ) NW_SENT_Task,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) NW_SENT_Task_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&NW_SENT_Task_Stk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) NW_SENT_Task_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) NW_SENT_Task_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������
#endif		
							 

}


