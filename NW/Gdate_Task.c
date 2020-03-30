



#include "global.h"
#include "nw.h"
#include "sram.h"
#include "sys_user.h"

int int_time_to_char_time(uint8_t *out,uint32_t sec);
void Fixed_reset_STM32(void);
int gain_qx_data(void);
int get_a_data_ll(void);	//1sһ��
int gain_ll_data(void);


extern uint32_t LastTxT;
int get_a_data_ll(void);
int gain_qx_data(void);	
int gain_ll_data(void);

extern uint8_t STM32_sleeping;

uint32_t lastRSTDA=0;
uint32_t LastHBT=0,next_HBT;  //LastGDT=0,LastOLT=0,
uint32_t LastGDT=0; //����

uint8_t data_tast_step,DATA_Tast_free;
extern uint8_t HK_Task_free,NW_Tast_free,function84_flag,function22_flag,function25_flag;;


extern uint32_t systime32;

void Runing_clock(void)
{
	unsigned char i;
	uint8_t *star_add;
	OS_ERR      err;
	uint8_t wort_time[6],aaa;
	u16 time_min;
	
	runningtime++;
	int_time_to_char_time(wort_time,runningtime);
	time_min=wort_time[3]*60+wort_time[4];
	
	if(wort_time[5]==0)
	{
		if(lastRSTDA==0) lastRSTDA=runningtime;
		if(((RST[0]==0)||(wort_time[2]==RST[0]))&&(wort_time[3]==RST[1])&&(wort_time[4]==RST[2])&&(work_i100ms>=100)) //�� ʱ ��
		{		
			printf("\r\nϵͳ��ʱ��λʱ��");
			Fixed_reset_STM32();
			lastRSTDA=runningtime;
			
			STM32_sleeping=0;
		}
	}
////////////////////////////////////////////////////////////////////////////
	if(runningtime>=next_HBT) //���ĳ�����ݴ�����ܵ������������ͺ�����û��
	{
		senttask_Asim |= data_flag05;
		if(runningtime>=(next_HBT+5)) LastHBT	=	runningtime;
		else LastHBT	=	next_HBT;
		NW_Tast_free=0;
		next_HBT =LastHBT + 60*HBT[0];
	}
	if(LastTxT>=LastHBT+5)
	{
//		LastHBT=LastTxT;
		next_HBT = 60*HBT[0]+LastTxT;
	}
	if(next_HBT>(systime32+60*HBT[0])) //�쳣�ָ�
	{
		next_HBT=systime32+60*HBT[0];
	}
		
////////////////////////////////////////////////////////////////////////////	
	if((wort_time[5]==2)&&((function22_flag)||(function25_flag)))
	{
		if(GDT[0]>=6) aaa=3;
		else if(GDT[0]>=4) aaa=2;
		else  aaa=1;
		if((wort_time[4]%aaa)==0)  //1����һ�Σ�
		{
//			ll_poll_time=0;
			datagettask_Asim |= gat_a_ll_req;
			DATA_Tast_free=0;
		}
		
		if(((time_min-LastGDT)>=GDT[0])||((time_min<LastGDT)&&((time_min+1440-LastGDT)>=GDT[0]))) //�ɼ�ʱ��//*60/testdvi
		{
			datagettask_Asim |= gat_data_time;
			LastGDT=time_min;	//��ȷ������
			DATA_Tast_free=0;
		}
	}
////////////////////////////////////////////////////////////////////////////	
	if((wort_time[5]==2)&&(function84_flag)) //
	{		
		for(i=0;i<aut_point_group1[0];i++)
		{
//			if((aut_get_point1[i].h * 60 + aut_get_point1[i].m)==time_min)
			
			if((wort_time[3]==(aut_get_point1[i].h))&&(wort_time[4]==(aut_get_point1[i].m)))
			{
				printf("\r\ntake a photo in %d",aut_get_point1[i].p);
//				printf("\r\nsystime[3]=%d,systime[4]=%d,runningtime=%d,systime32=%d",systime[3],systime[4],runningtime,systime32);
//				printf("\r\nwort_time[3]=%d,wort_time[4]=%d,h=%d,m=%d",wort_time[3],wort_time[4],aut_get_point1[i].h,aut_get_point1[i].m);

				if(HK_post_cnt>=HK_post_max) HK_post_cnt=0;
				star_add=&HK_post_buf[HK_post_cnt];
				HK_post_buf[HK_post_cnt++]=0x83; //����ָ��
				HK_post_buf[HK_post_cnt++]=aut_get_point1[i].p;	//Ԥ��λ
				OSQPost(&hktask_Asim[0],star_add,2,OS_OPT_POST_FIFO,&err);
				HK_Task_free=0;
				if(power_flag_cam1==0) power_set_cam1(1); //�������Դ
			}
		}
////////////////////////////////////////////////////////////////////////////
	}
	
}
	
void GDATA_Task(void * p_arg )
{
	data_tast_step=0;
	switch(data_tast_step)
	{
		case 0:
			if((datagettask_Asim & gat_a_ll_req)||(datagettask_Asim & gat_data_time))
			{
				DATA_Tast_free=0;
				if((power_on_delay_485==0)&&(power_flag_485)) //Ĭ�Ͽ����Ϳ� power
				{
					if(datagettask_Asim & gat_a_ll_req)
					{
						get_a_data_ll();
						datagettask_Asim &= (~gat_a_ll_req);
					}
					else if(datagettask_Asim & gat_data_time)
					{
						if(gain_ll_data()==0) senttask_Asim |= data_flag22;
						if(gain_qx_data()==0) senttask_Asim |= data_flag25;
						
						datagettask_Asim &= (~gat_data_time);
					}
				}
				else //��ʱ�ɼ�����ʱ���ߺͶ�ʱ��λֹͣ
				{
//					power_set_485(1);
				}
			}
			else
			{
				DATA_Tast_free=1;
			}
			
			break;
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		default:data_tast_step=0;break;
	}
	
}








#if 0
void NWTaskStart (void *p_arg)
{
	OS_ERR      err;
	(void)p_arg;

	

//		��ֵ�ź������ƿ�ָ��		//��ֵ�ź������� //��Դ��Ŀ���¼��Ƿ�����־ //���ش�������
	OSSemCreate (&read_RSSI, "read_RSSI",  0, &err); //3 		
//	OSMutexCreate (&sd_sem_lock, //�����ź���ָ��
//                 "dm9000_sem_lock",  //ȡ�ź���������
//                  &err);   //���ش�������

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
#endif

