

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
		hk_err=PTZCtrl_presets_goto_s(1,preset);	//��������ڵ�ָ��Ԥ�õ�
		if(hk_err==0) preset_now[0]=preset;
		else preset_now[0]=0;
	}
	
	if(get_photo_s()==0)
	{
		while((senttask_Asim & data_flag84_1)==1) 
		{
			
		}
		senttask_Asim |= data_flag84_1;		//�����ϴ�
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
	
	uint8_t hMsg[3]={1,0,0}; //�̶�ͨ��1
	int hk_err;
	
	(void)p_arg;

					 
//  cpu_clk_freq = BSP_CPU_ClkFreq();                 //��ȡCPUʱ�ӣ�ʱ������Ը�ʱ�Ӽ���

	
	while (DEF_TRUE) {                                           //������
		
		pMsg=OSQPend((OS_Q*			)&hktask_Asim[0],   
											(OS_TICK		)1000,		//6000,	//							
											(OS_OPT			)OS_OPT_PEND_BLOCKING,
											(OS_MSG_SIZE*	)&msg_size,		
											(CPU_TS*		)0,
											(OS_ERR*		)&err);
		
		
		
		if(err==OS_ERR_NONE)//�յ�������Ϣ
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
				ph_get(hMsg[2]); //Ԥ��λ����
			}
			else  if(pMsg[0]<=62)	//88ָ��
			{
				hMsg[1]=pMsg[0]; hMsg[2]=pMsg[1];
				hk_err=0;
				
				switch(hMsg[1])
				{
					case 1:camera_on(hMsg[0]);break;
					case 10:camera_off(hMsg[0]);break;
					case 2:
			//			while(PTZCtrl_presets_goto(chanle,ctl_dat[2]) !=0);//��������ڵ�ָ��Ԥ�õ�
						hk_err=PTZCtrl_presets_goto_s(hMsg[0],pMsg[2]);	//��������ڵ�ָ��Ԥ�õ�
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
					
					case 7://������Զ������1����λ����ͷ�䱶�Ŵ�
					z=-28;
					hk_err=PTZCtrl_continuous_s(hMsg[0],0,0,&z);
					break;
					
					case 8://�������������1����λ����ͷ�䱶��С��
					z=28;
					hk_err=PTZCtrl_continuous_s(hMsg[0],0,0,&z);
					break;
					
					case 9://���浱ǰλ��ΪĳԤ�õ�
			//		PTZCtrl_presets(chanle,ctl_dat[2]);//���浱ǰλ��ΪĳԤ�õ�
					hk_err=PTZCtrl_presets_s(hMsg[0],pMsg[2]);//���浱ǰλ��ΪĳԤ�õ�
					break;
					
					case 11://��Ȧ�Ŵ�1����λ
				
					break;
				}
				if(hk_err==0) nw_sent_88(hMsg); //ԭ�����
			}
			
		}
				
	}
}

#define  http_hk_thread_SIZE                     600
#define  http_hk_thread_PRIO                         6
OS_TCB   http_hk_thread_TCB;															//������ƿ�
CPU_STK  http_hk_thread_Stk [ http_hk_thread_SIZE ];	//�����ջ

#define  HK_Task_ch1_SIZE                     600
#define  HK_Task_ch1_PRIO                         6
OS_TCB   HK_Task_ch1_TCB;															//������ƿ�
CPU_STK  HK_Task_ch1_Stk [ HK_Task_ch1_SIZE ];	//�����ջ

void http_hk_thread(void *arg);	
extern OS_SEM http_rev_sem,http_200_sem,http_401_sem,http_data_sem;

void HKTaskStart (void *p_arg)
{
	OS_ERR      err;
	
//		��ֵ�ź������ƿ�ָ��		//��ֵ�ź������� //��Դ��Ŀ���¼��Ƿ�����־ //���ش�������
	OSSemCreate (&http_rev_sem, "http_rev_sem",  0, &err); //3 	

//		��ֵ�ź������ƿ�ָ��		//��ֵ�ź������� //��Դ��Ŀ���¼��Ƿ�����־ //���ش�������
	OSSemCreate (&http_200_sem, "http_200_sem",  0, &err); //3 	
	
//		��ֵ�ź������ƿ�ָ��		//��ֵ�ź������� //��Դ��Ŀ���¼��Ƿ�����־ //���ش�������
	OSSemCreate (&http_data_sem, "http_data_sem",  0, &err); //3 	

//��Ϣ����	 //��Ϣ�������� //��Ϣ��������  //��Ϣ���г��� //������
	OSQCreate (&hktask_Asim[0],"hktask_Asim[0]",100,&err);	
	
	
	/* ���� ���� */
    OSTaskCreate((OS_TCB     *)&http_hk_thread_TCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"http_hk_thread",                             //��������
                 (OS_TASK_PTR ) http_hk_thread,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     )	http_hk_thread_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&http_hk_thread_Stk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) http_hk_thread_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) http_hk_thread_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������

								 
			/* ���� ���� */
    OSTaskCreate((OS_TCB     *)&HK_Task_ch1_TCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"HK_Task_ch1",                             //��������
                 (OS_TASK_PTR ) HK_Task_ch1,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     )	HK_Task_ch1_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&HK_Task_ch1_Stk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) HK_Task_ch1_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) HK_Task_ch1_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������						 

}


