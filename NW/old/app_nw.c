
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

OS_MUTEX RX_4G;                         //���������ź���

OS_SEM RX_4G_AT;          //��־KEY1�Ƿ񱻵����Ķ�ֵ�ź���

#define Pend_4G_Mutex()		OSMutexPend ((OS_MUTEX  *)&RX_4G, (OS_TICK )0, (OS_OPT)OS_OPT_PEND_BLOCKING, (CPU_TS *)0, (OS_ERR *)&err)      
//		OSMutexPend ((OS_MUTEX  *)&RX_4G,        //���뻥���ź��� mutex
//								 (OS_TICK    )0,                       //�����޵ȴ�
//								 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //������벻���Ͷ�������
//								 (CPU_TS    *)0,                       //������ʱ���
//								 (OS_ERR    *)&err);                   //���ش�������		

#define Post_4G_Mutex()		OSMutexPost ((OS_MUTEX  *)&RX_4G, (OS_OPT )OS_OPT_POST_NONE,(OS_ERR *)&err) 
//OSMutexPost ((OS_MUTEX  *)&RX_4G,                 //�ͷŻ����ź��� mutex
//								 (OS_OPT     )OS_OPT_POST_NONE,       //�����������
//								 (OS_ERR    *)&err);                  //���ش�������	
								 

OS_Q NW_Msg;				//����һ����Ϣ���У����ڰ�����Ϣ���ݣ�ģ����Ϣ����

void ph_test(void);
	extern OS_SEM http_rev_sem,http_200_sem,http_401_sem,http_data_sem;

int PUT_PTZ_s(char *command);
//PAN_LEFT ��ˮƽ����ת��
//PAN_RIGHT��ˮƽ����ת��
//TILT_UP����ֱ�����˶�
//TILT_DOWN����ֱ�����˶�
//ZOOM_IN����ͷ��Զ
//ZOOM_OUT����ͷ����
//UP_LEFT: ����ת���˶�
//UP_RIGHT: ����ת���˶�
//DOWN_LEFT: ����ת���˶�
//DOWN_RIGHT: ����ת���˶�
//PAN_AUTO: �Զ�Ѳ��	
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
		case 	Contact:retxtime=60/testdvi;	break;	//�������磬30s�ط�
		case 	CheckT:retxtime=120/testdvi;	break;	//Уʱ��2�����ط� 	//�������磬30s�ط�
		case 	HeartBeat:retxtime=*HBT*60/testdvi;	break;//	װ��������Ϣ	������վ���װ������ʱ�䡢IP��ַ�Ͷ˿ںš������ź�ǿ�ȼ����ص�ѹ
		case 	0x84:retxtime=2;	break;//
		case 	0x86:retxtime=2;	break;//	װ��������Ϣ	������վ���װ������ʱ�䡢IP��ַ�Ͷ˿ںš������ź�ǿ�ȼ����ص�ѹ
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
//		printf("����IP%s",buf);
		HAL_UART_Transmit_IT(&huart5, buf, strlen((char *)buf));
	}
	
	if(systime[5]==0) //������
	{
//		printf("\r\nһ����");
		if(((RST[0]==0)||(systime[2]==RST[0]))&&(systime[3]==RST[1])&&(systime[4]==RST[2])) //�� ʱ ��
		{
				reset_STM32();//�����λ
		}
		for(i=0;i<aut_point_group[0];i++)
		{
			if((systime[3]==(aut_get_point[i].h))&&(systime[4]==(aut_get_point[i].m+0)))
			{
				printf("\r\n����ʱ�� ǰ�˵�:%d",aut_get_point[i].p);

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
		
		if(err==OS_ERR_NONE)//�յ�������Ϣ
		{				
			printf("\r\nM->S %x,%d:%d:%d",nw_r_post->cmd,systime[3],systime[4],systime[5]);

			nw_in(nw_r_post);
			LastTxT=worktime;
		}
		else //��ʱ (1)//	
		{
//			if((worktime-sentTime)>3600*24) //24СʱУʱһ��
//			{
//				ctlnumber=CheckT;
//			}
			check_retxtime();
			
			
//			printf("\r\ndata_time=%d,GDT=%d,time=%d,GDT_fun=%d,GDT_OK=%d,ctlnumber=%d",sampled_data_time,GDT[0],(worktime-LastGetDataT),GDT_fun,GDT_OK,ctlnumber);

/////////////���ݲɼ�////////////////////////////////////////////////////////////////////			
			
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
			
			if(((worktime-LastTxT)>=retxtime)||(sampled_data_flag)) //�ط�
			{
				LastTxT=worktime;
				nw_sc.cmd=ctlnumber;
				sampled_data_flag=0;
				
				switch(ctlnumber)
				{					
					case 	Contact:			//00	//	����������Ϣ	װ�ÿ�������������Ϣ
						ctlnumberOld=Contact;
						nw_sc.len=sizeof(version);
//						nw_sc.data=version;
						memcpy(nw_sc.data,version,nw_sc.len);
						nw_sent(&nw_sc);
						break;
						
					case  CheckT:				//01	//	Уʱ	�������������·������֣�һ��һ��
						ctlnumberOld=CheckT;
						nw_sc.len=0;//sizeof(systime);
//						nw_sc.data=systime;
						memcpy(nw_sc.data,systime,6);
						nw_sent(&nw_sc);
						sentTime=worktime;
						break;
					
					case  HeartBeat:		//05	//	װ��������Ϣ	������վ���װ������ʱ�䡢IP��ַ�Ͷ˿ںš������ź�ǿ�ȼ����ص�ѹ
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
					
					case 	0x22:			//�ϴ��ɼ�����
						if(SenTimes)
						{
							sampled_data_sent_22(sampled_step,sampled_data_nub);
							SenTimes--;
						}
						if(SenTimes==0)//����,�ص���������
						{
							ctlnumber=ctlnumberOld;
							GDT_OK=1;
						}
						
						break;
					case 	0x25:			//�ϴ��ɼ�����
						if(SenTimes)
						{
							sampled_data_sent_25(sampled_data_nub);
							SenTimes--;
						}
						if(SenTimes==0)//����,�ص���������
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
						if(SenTimes==0)//����,�ص���������
						{
							ctlnumber=ctlnumberOld;
						}
					break;
						
					case  0x86:
						if(SenTimes)
						{
							nw_sc.cmd=0x86;//���ݽ���
							nw_sc.len=2;			
							nw_sc.data[0]=1;//ͨ����	
							nw_sc.data[1]=0;//Ԥ��λ��
							nw_sent(&nw_sc);
							SenTimes--;
						}
						if(SenTimes==0) //����,�ص���������
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
	
//	printf("\r\n���� nw:");
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
				
	//			printf("\r\n���� nw:");					
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
OS_TCB		NW_REV_Task_TCB;															//������ƿ�
CPU_STK		NW_REV_Task_Stk [ NW_REV_Task_SIZE ];	//�����ջ

#define		NW_SENT_Task_SIZE                     512
#define		NW_SENT_Task_PRIO                         9
OS_TCB		NW_SENT_Task_TCB;															//������ƿ�
CPU_STK		NW_SENT_Task_Stk [ NW_SENT_Task_SIZE ];	//�����ջ


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
	
	//		��ֵ�ź������ƿ�ָ��		//��ֵ�ź������� //��Դ��Ŀ���¼��Ƿ�����־ //���ش�������
	OSSemCreate (&read_RSSI, "read_RSSI",  0, &err); //3 	
//	//��ֵ�ź������ƿ�ָ��	//ѡ��	 //���ش�������
//			OSSemPost (&read_RSSI, OS_OPT_POST_FIFO, &err);    //������յ�����֡ 
	
//	//��ֵ�ź������ƿ�ָ��	//ѡ��	 //���ش�������
//			OSSemPost (&read_RSSI, OS_OPT_POST_NO_SCHED, &err);    //������յ�����֡ 
	
////								��ֵ�ź���ָ��	//�ȴ���ʱʱ��	//ѡ�� //�ȵ��ź���ʱ��ʱ��� //���ش�������
//		OSSemPend (&read_RSSI, 2000, OS_OPT_PEND_BLOCKING,0, &err); 
	
#if 0		
	OSMutexCreate ((OS_MUTEX  *)&RX_4G,           //ָ���ź���������ָ��
                   (CPU_CHAR  *)"RX For 4G", //�ź���������
                   (OS_ERR    *)&err);            //��������

	
	 OSSemCreate((OS_SEM      *)&RX_4G_AT,    //ָ���ź���������ָ��
               (CPU_CHAR    *)"urt5_AT",    //�ź���������
               (OS_SEM_CTR   )0,             //��ʾ������Դ��Ŀ
               (OS_ERR      *)&err);         //��������
	
#endif	
	OSQCreate ((OS_Q*		)&NW_Msg,	//��Ϣ����
                (CPU_CHAR*	)"NW_Msg",	//��Ϣ��������
                (OS_MSG_QTY	)3,	//��Ϣ���г��ȣ���������Ϊ1
                (OS_ERR*	)&err);		//������
				

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




