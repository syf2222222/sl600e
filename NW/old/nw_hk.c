
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


extern OS_TCB   nw_ctr_hk_TCB;															//������ƿ�
extern OS_TCB   nw_get_photo_TCB;															//������ƿ�
//uint8_t nw_post_nb=0;
extern uint8_t GDT_OK,Streaming_OK,sampled_data_time;
//uint8_t GDT_OK=1,GDT_fun=0,sampled_data_time=0;

extern unsigned char bLevel,cLevel,sLevel;
	
void ph_test(void)
{
	OS_ERR      err;
	tt_len=((PH_LEN[0])<<16) +PH_LEN[1];
	if(tt_len>100000) tt_len=100000;
	tt_pk_nb=(tt_len>>10); //1024һ��
	if(tt_len&0x3ff) tt_pk_nb +=1;
	
	printf("\r\n׼������Ƭ%d��,�ܳ�%d,",tt_pk_nb,tt_len);
	for(pk_nb=1;pk_nb<=tt_pk_nb;pk_nb++)
	{
		if(pk_nb==tt_pk_nb) nw_sent_ph(PH_DATA+((pk_nb-1)*1024),tt_len-((pk_nb-1)*1024),pk_nb);
		else	nw_sent_ph(PH_DATA+((pk_nb-1)*1024),1024,pk_nb);
		OSTimeDly ( 100, OS_OPT_TIME_DLY, & err ); 
	}
}

#define		pk_len		1024 //1460
#define		len_max		200000

//ctl_dat[0]-ͨ��  ctl_dat[1]-ָ��  ctl_dat[2]-����
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
		
		nw_r_post = OSTaskQPend (//(OS_Q         *)&ur2_Q,                	//��Ϣ����ָ��
                    (OS_TICK       )waitime,                 //�ȴ�ʱ��
                    (OS_OPT        )OS_OPT_PEND_BLOCKING, //���û�л�ȡ���ź����͵ȴ�
                    (OS_MSG_SIZE  *)&msg_size,            //��ȡ��Ϣ���ֽڴ�С
                    (CPU_TS       *)0,                    //��ȡ������ʱ��ʱ���
                    (OS_ERR       *)&err);                //���ش���
		
		printf("\r\nget_post%x",nw_r_post->cmd);
		
		if(err!=OS_ERR_NONE)
		{
			cmding=0;
		}
		else
		{
			switch(nw_r_post->cmd)
			{	
				case 0xf0:	//��վ����������Ƭ
	//				PTZCtrl_presets_goto_s(1,((uint8_t *)nw_r_post)[1]);	//��������ڵ�ָ��Ԥ�õ�
				printf("\r\n��ʼԤ������ ��˵�:%x",nw_r_post->data[0]);
				PTZCtrl_presets_goto_s(1,nw_r_post->data[0]);	//��������ڵ�ָ��Ԥ�õ�
				
				cmding=0x83;
					get_photo_s();	
						OSTimeDly( 1000, OS_OPT_TIME_DLY, & err);
						
						nw_sc.cmd=0x84;
						nw_sc.len=10;
											
						memcpy(nw_sc.data,systime,6);
						nw_sc.data[6]=1;//ͨ����	
						nw_sc.data[7]=0;//Ԥ��λ��
						
						tt_len=((PH_LEN[0])<<16) +PH_LEN[1];
						if(tt_len>len_max) tt_len=len_max;
						tt_pk_nb=(tt_len/pk_len); //1024һ��
						if(tt_len%pk_len) tt_pk_nb +=1;
						nw_sc.data[8]=(tt_pk_nb>>8);//������λ
						nw_sc.data[9]=tt_pk_nb;			//������λ
					
						nw_sent(&nw_sc);
						ctlnumber=0x84;
						SenTimes=5;
						retxtime=3;		//����������Ƭ 1s�ط�,Ҫ��3s,���5��
						sentTime=worktime;
				break;
				
				case 0x83:	//��վ����������Ƭ
					if((cmding==0)||(cmding>=0x86))
					{
						cmding=nw_r_post->cmd;
						nw_sent(nw_r_post);
						get_photo_s();	
						OSTimeDly( 1000, OS_OPT_TIME_DLY, & err);
						
						nw_sc.cmd=0x84;
						nw_sc.len=10;
											
						memcpy(nw_sc.data,systime,6);
						nw_sc.data[6]=1;//ͨ����	
						nw_sc.data[7]=0;//Ԥ��λ��
						
						tt_len=((PH_LEN[0])<<16) +PH_LEN[1];
						if(tt_len>len_max) tt_len=len_max;
						tt_pk_nb=(tt_len/pk_len); //1024һ��
						if(tt_len%pk_len) tt_pk_nb +=1;
						nw_sc.data[8]=(tt_pk_nb>>8);//������λ
						nw_sc.data[9]=tt_pk_nb;			//������λ
					
						nw_sent(&nw_sc);
						ctlnumber=0x84;
						SenTimes=5;
						retxtime=3;		//����������Ƭ 1s�ط�,Ҫ��3s,���5��
						sentTime=worktime;	
					}
					break;
				case 0x84:	//�ɼ��ն�����������Ƭ��
					if(cmding==0x83)
					{
						cmding=nw_r_post->cmd;
						ctlnumber=0x85;
						printf("\r\n׼������Ƭ%d��,�ܳ�%d,",tt_pk_nb,tt_len);
//						OSTimeDly( 100, OS_OPT_TIME_DLY, & err);
						
						for(pk_nb=1;pk_nb<=tt_pk_nb;pk_nb++)
						{
							if(pk_nb==tt_pk_nb) nw_sent_ph(PH_DATA+((pk_nb-1)*pk_len),tt_len-((pk_nb-1)*pk_len),pk_nb);
							else	nw_sent_ph(PH_DATA+((pk_nb-1)*pk_len),pk_len,pk_nb);
							OSTimeDly ( 3, OS_OPT_TIME_DLY, & err ); 
						}
						
						OSTimeDly ( 4000, OS_OPT_TIME_DLY, & err ); 
						
						retxtime=1;		//���ͽ������ 1s�ط�,Ҫ��30s,���5��
						sentTime=worktime;	
						nw_sc.cmd=0x86;//���ݽ���
						cmding=0x86;
						nw_sc.len=2;			
						nw_sc.data[0]=1;//ͨ����	
						nw_sc.data[1]=0;//Ԥ��λ��
						
						nw_sent(&nw_sc);
						ctlnumber=0x86;
						SenTimes=6;
					}
					
					break;
					
					case 0x85:	//ͼ���������ͣ�ֻ�з�
				
					break;
					
					case 0x86:	//ͼ���������ͽ������
						cmding=0;//���ݰ��ɹ�
						ctlnumber=ctlnumberOld;
					break;
					
					case 0x87:	//���������·�
						if(nw_r_post->data[2]==0)
						{
							cmding=0;//���ݰ��ɹ�
							ctlnumber=ctlnumberOld;
							printf("\r\nͼƬ���ͳɹ�");
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
		nw_r_post = OSTaskQPend (//(OS_Q         *)&ur2_Q,                	//��Ϣ����ָ��
                    (OS_TICK       )0,                 //�ȴ�ʱ��
                    (OS_OPT        )OS_OPT_PEND_BLOCKING, //���û�л�ȡ���ź����͵ȴ�
                    (OS_MSG_SIZE  *)&msg_size,            //��ȡ��Ϣ���ֽڴ�С
                    (CPU_TS       *)0,                    //��ȡ������ʱ��ʱ���
                    (OS_ERR       *)&err);                //���ش���
	
//		printf("\r\nget_post%x",nw_r_post->cmd);
		
		switch(nw_r_post->cmd)
		{
			case 0x81:	//ͼ��ɼ���������
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
				
			case 0x82:	//����ʱ�������
				if((memcmp(PSW,nw_r_post->data,4)==0)&&(nw_r_post->data[5]<=255))
				{
//����0-3	ͨ����4	����5					��1��
//											ʱ			��			Ԥ��λ��
//4�ֽ�	1�ֽ�		1�ֽ�		1�ֽ�		1�ֽ�			1�ֽ�
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
				
			case 0x8b:	//��ѯ����ʱ���
//				if(memcmp(PSW,nw_r_post->data,4)==0)
				{
					printf("\r\n��ѯ����ʱ��");
					nw_sc.cmd=0x8b;
					if(aut_point_group[0]>10) aut_point_group[0]=0;

					nw_sc.len=2+aut_point_group[0]*3;
					nw_sc.data[0]=1; //ͨ����
					nw_sc.data[1]=aut_point_group[0]; //����
					memcpy(&nw_sc.data[2],(uint8_t *)aut_get_point,aut_point_group[0]*3);
					
					nw_sent(&nw_sc);
					printf("\r\n��ѯ����ʱ�����");
				}
//				else
//				{
//					rev_err(0x8b,0xff);
//				}
					
				break;
			
				case 0x88:	//�����Զ�̵���
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

#define	data_flag00				0x0001
#define	data_flag01				0x0002
#define	data_flag05				0x0004
#define	data_flag06				0x0010
#define	data_flag0a				0x0020
#define	data_flag22				0x0040
#define	data_flag25				0x0080

#define	ctr_para_cfg				1		//configuration parameter
#define	ctr_up							2		//
#define	ctr_down						3		//
#define	ctr_left						4		//
#define	ctr_right						5	
//#define	ctr_right						5	
//#define	ctr_right						5	
//#define	ctr_right						5	

uint32_t photo_ctr_flag[2];



void  NW_HK_Task  ( void * p_arg )//void  NWTask_task( void * p_arg )
{
	OS_ERR      err;
	OS_MSG_SIZE*	 size;	
	(void)p_arg;	
  uint8_t	*ctr;
		
	
	while (DEF_TRUE)
	{			
		ctr=OSQPend((OS_Q*			)&hktask_Asim[0],   
											(OS_TICK		)1000,		//6000,	//							
											(OS_OPT			)OS_OPT_PEND_NON_BLOCKING,
											(OS_MSG_SIZE*	)&size,		
											(CPU_TS*		)0,
											(OS_ERR*		)&err);
		

		if(ctr[0]==)
	}
}



#define  nw_ctr_hk_SIZE                     500
#define  nw_ctr_hk_PRIO                        20
OS_TCB   nw_ctr_hk_TCB;															//������ƿ�
CPU_STK  nw_ctr_hk_Stk[ nw_ctr_hk_SIZE ];	//�����ջ
void nw_ctr_hk(void *arg);

#define  nw_get_photo_SIZE                     500
#define  nw_get_photo_PRIO                      7
OS_TCB   nw_get_photo_TCB;															//������ƿ�
CPU_STK  nw_get_photo_Stk[ nw_get_photo_SIZE ];	//�����ջ
void nw_get_photo(void *arg);

void NW_HK_Task_Create(void)
{
	OS_ERR      err;
	
	OSQCreate ((OS_Q*		)&NW_Msg,	//��Ϣ����
                (CPU_CHAR*	)"NW_Msg",	//��Ϣ��������
                (OS_MSG_QTY	)3,	//��Ϣ���г��ȣ���������Ϊ1
                (OS_ERR*	)&err);		//������
	
	#if 1								 
		/* ���� ���� */
    OSTaskCreate((OS_TCB     *)&nw_ctr_hk_TCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"nw_ctr_hk",                             //��������
                 (OS_TASK_PTR ) nw_ctr_hk,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     )	nw_ctr_hk_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&nw_ctr_hk_Stk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) nw_ctr_hk_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) nw_ctr_hk_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 10u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������
							 
				/* ���� ���� */
    OSTaskCreate((OS_TCB     *)&nw_get_photo_TCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"nw_get_photo",                             //��������
                 (OS_TASK_PTR ) nw_get_photo,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     )	nw_get_photo_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&nw_get_photo_Stk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) nw_get_photo_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) nw_get_photo_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 3u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������
#endif	
}




