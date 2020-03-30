


#include "global.h"
#include "nw.h"
#include "sram.h"
#include "sys_user.h"

extern const unsigned char ver[2];
extern unsigned char ip_seting;

void nw_sent_st(struct nw_ctl *t_data);
void power_set_DTU(uint8_t flag);
int Get_Volt(void);
int check_signel(void);
int check_sim7600(void);
int change_ip_done(void);

uint8_t Contact_step=0xf0,Contact_state,Contact_cnt,Contact_err;
uint16_t Contact_time;

uint8_t DTU_c_step=0xf0,DTU_c_state,DTU_c_cnt,DTU_c_err;
uint16_t DTU_c_time;

void sent_Sleep(void)
{
	nw_sc.cmd=0x0c;
	nw_sc.len=0;
	nw_sent_st(&nw_sc);
}

void sent_Contact()
{
	senttask_Pend |= data_flag00;	//�ȴ� data_flag00 ��
	nw_sc.cmd=Contact;
	nw_sc.len=2;
	nw_sc.data[0]=ver[0];
	nw_sc.data[1]=ver[1];
	nw_sent_st(&nw_sc);

}
void sent_CheckT(char flag)	//����Уʱ 0, ����Уʱ 1
{
	senttask_Pend |= data_flag01;	//�ȴ� data_flag01 ��	
	nw_sc.cmd=CheckT;
	if(flag==0)	nw_sc.len=0;
	else	nw_sc.len=6;
	memcpy(nw_sc.data,systime,6);
	nw_sent_st(&nw_sc);

}
void sent_HeartBeat(void)
{
	senttask_Pend |= data_flag05;
	nw_sc.cmd=HeartBeat;
	nw_sc.len=8;
	memcpy(nw_sc.data,systime,6);
	
	if(check_signel()==0) nw_sc.data[6]=signal;
	else nw_sc.data[6]=0;

	if(Get_Volt()==0) nw_sc.data[7]=volt;
	else nw_sc.data[7]=0;

	nw_sent_st(&nw_sc);
	

}

uint16_t printTime;
void DTU_Check_polling(void)
{
	OS_ERR      err;
		
	if((work_i100ms-printTime)>=10)
	{
		printf(".");
		printTime=work_i100ms;
	}
	
	switch(DTU_c_step)
	{
		case 0:	//��������
			if(ip_seting)
			{
				printf("\r\n����IP");
				DTU_reset();
				Contact_state=0;
				DTU_c_state=0;
				DTU_c_time=0;
				DTU_c_step=0xf0;
			}
			if(DTU_c_state==0)
			{
				printf("\r\n����DTU");
				DTU_reset();
				DTU_c_time=0;
				DTU_c_step=0xf1;
			}
			break;
		case 0xf0:	//
			printf("\r\nDTU�ϵ���");
			DTU_c_step++;
			DTU_c_time=0;
			break;
			
		case 0xf1:	//�ո�λ�ȴ� PB_DONE
			if(power_flag_DTU==0)
			{
				power_set_DTU(1);
			}
			if(check_sim7600()==1) 
			{
				DTU_c_step++;
				DTU_c_time=0;
				DTU_c_err=0;
			}
			else if(DTU_c_time>=700) //��ʱ
			{
				printf("\r\nDTU�ϵ���ʧ��");
				DTU_reset();
				DTU_c_time=0;
				DTU_c_err++;
			}
			break;
			
		case 0xf2:
			if(ip_seting)
			{
				OSTimeDly( 1000, OS_OPT_TIME_DLY, & err);
				if(change_ip_done()==1)
				{
					DTU_c_state=1;
					DTU_c_step=0;
					DTU_c_time=0;
					ip_seting=0;
					DTU_c_err=0;
					printf("\r\nchange ip ok");
				}
				else if(DTU_c_time>=200)
				{
					DTU_reset();
					DTU_c_step=0xf0; //����
					DTU_c_err++;
				}
			}
			else if(DTU_c_time>=60) //�ȴ�6s
			{
				DTU_c_state=1;
				DTU_c_step=0;
				DTU_c_time=0;
				printf("\r\nDTU ok");
			}
			break;

		default:
			DTU_reset();
			DTU_c_time=0;
			DTU_c_step=0xf0;
			break;
	}
	
}
extern uint32_t LastHBT,next_HBT;

void NW_contact_polling(void)
{
//	OS_ERR      err; 
	switch(Contact_step)
	{			
		case 0:
//			if(STM32_sleeping)
//			{
////				senttask_Asim |= data_flag05;
//				Contact_state=0;
//				
//				if((STM32_sleeping==1)&&((work_i100ms-LastTxT)>=50))
//				{
//				}
//				else if(Contact_state) //�����ϣ������ٷ�
//				{
//					NW_Tast_free=1;
//				}
//			}
//			else NW_Tast_free=0; //����������
			
			if(senttask_Asim & data_flag05)//����ʱ�䵽������Ҫ���������� ����LastTxT��Ҫ�ĳɽ���ʱ�� &&(NW_Tast_free==0) ((work_i100ms-LastTxT)>=(HBT[0]*599/testdvi))||
			{
				sent_HeartBeat();
				senttask_Asim &= (~data_flag05);
				Contact_step++;
				Contact_time=0;
				Contact_state=0;	//1���ӿ��ܶ���
				Contact_cnt=0;
			}	
			break;
			
		case 1:
					if((senttask_Pend & data_flag05)==0)
					{
						Contact_step=0;
						Contact_err=0;
						Contact_state=1;
						Contact_cnt=0;
//						if(STM32_sleeping)
//						{
//							Contact_state=1;
//							NW_Tast_free=1;
//						}
					}
					else if(Contact_time>=30) //�ȴ�2s
					{
						
						if(++Contact_cnt<3)
						{
							sent_HeartBeat();
							Contact_time=0;
						}
						else
						{
							printf("\r\nheartbeat err");
							if(++Contact_err>=10)
							{
								DTU_c_step=0;
								DTU_c_state=0;
							}
							Contact_step=0;
							Contact_state=0;
						}
						
//						if(STM32_sleeping) NW_Tast_free=1;
						
#ifndef ur3_rdma						
						if(Contact_err>10)
						{
							HAL_UART_Abort_IT(&huart3);
							HAL_UART_Receive_IT(&huart3,(unsigned char *)urd3,lenmax3);
						}
#endif							
					}
		
			break;
		case 0xf0:
//			Contact_time=1000; //��������������
			sent_Contact(); //��������
			Contact_time=0; //��������������
			Contact_state=0;
			Contact_step++;
			Contact_cnt=0;
			break;
			
		case 0xf1:		
			if((Contact_time>=600/testdvi)||((Contact_cnt==0)&&(Contact_time>=60)))
			{
				Contact_time=0;
				sent_Contact(); //��������
			}
			else	if((senttask_Pend & data_flag00)==0)
			{
//				printf("\r\n��������OK");
//				Contact_time=1000;
				sent_CheckT(0);
				Contact_time=0;
				Contact_step++;
//				printf("\r\n��������ok,��ʼУʱ");
			}
			break;
			
		case 0xf2:		//Уʱ
			if(Contact_time>=120/testdvi)
			{
				Contact_time=0;
				sent_CheckT(0);
			}
			else	if((senttask_Pend & data_flag01)==0)
			{
				printf("\r\ncheck time ok");
				sent_HeartBeat();
//				senttask_Asim |= data_flag05;
				Contact_step=1;
				Contact_time=0;
				LastHBT	=	runningtime;//  next_HBT;
				next_HBT = runningtime+60*HBT[0];
				senttask_Asim &= (~data_flag05);
			}
			break;
			
			default:
				Contact_step=0;
				break;
	}
}


