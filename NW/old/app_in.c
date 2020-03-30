
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




extern unsigned char sampled_step,sampled_data_nub,sampled_data_flag;
void sampled_data_sent_22(unsigned char step,unsigned char zbs);
void sampled_data_sent_25(unsigned char zbs);


void nw_in(struct nw_ctl *nw_r_post)
{
	int i,j;
	OS_ERR      err;
	
//void  OSTaskQPost (OS_TCB       *p_tcb,    //Ŀ������
//                   void         *p_void,   //��Ϣ���ݵ�ַ
//                   OS_MSG_SIZE   msg_size, //��Ϣ����
//                   OS_OPT        opt,      //ѡ��
//                   OS_ERR       *p_err)    //���ش�������
	
//OSQPost ((OS_Q        *)&ur2_Q,                             //��Ϣ����ָ��
//             (void        *)rev_data,                //Ҫ���͵����ݵ�ָ�룬���ڴ���׵�ַͨ�����С����ͳ�ȥ��
//             (OS_MSG_SIZE  )len,     									//�����ֽڴ�С
//             (OS_OPT       )OS_OPT_POST_FIFO | OS_OPT_POST_ALL, //�Ƚ��ȳ��ͷ�����ȫ���������ʽ
//             (OS_ERR      *)&err);	                            //���ش�������		
	
//	printf("\r\nnw_r_post->cmd=%x",nw_r_post->cmd);	
	
	if(((nw_r_post->cmd>=0x81)&&(nw_r_post->cmd<=0xB8))||(nw_r_post->cmd==0xf0))
	{
//		memcpy(&nw_to_hk[nw_post_nb],nw_r_post,sizeof(struct nw_ctl));
		if(nw_r_post->cmd==0xf0)
		{
			OSTaskQPost ((OS_TCB      *)&nw_get_photo_TCB,                       //Ŀ������
								 (void        *)nw_r_post,                //Ҫ���͵����ݵ�ָ�룬���ڴ���׵�ַͨ�����С����ͳ�ȥ��
								 (OS_MSG_SIZE  )3,     										//�����ֽڴ�С
								 (OS_OPT       )OS_OPT_POST_FIFO , 				//�Ƚ��ȳ�
								 (OS_ERR      *)&err);	                            //���ش�������	
		}
	 	else if((nw_r_post->cmd>=0x83)&&(nw_r_post->cmd<=0x87))
		{
			OSTaskQPost ((OS_TCB      *)&nw_get_photo_TCB,                       //Ŀ������
								 (void        *)nw_r_post,                //Ҫ���͵����ݵ�ָ�룬���ڴ���׵�ַͨ�����С����ͳ�ȥ��
								 (OS_MSG_SIZE  )sizeof(struct nw_ctl),     									//�����ֽڴ�С
								 (OS_OPT       )OS_OPT_POST_FIFO , //�Ƚ��ȳ�
								 (OS_ERR      *)&err);	                            //���ش�������	
		}
		else
		{
			OSTaskQPost ((OS_TCB      *)&nw_ctr_hk_TCB,                       //Ŀ������
								 (void        *)nw_r_post,                //Ҫ���͵����ݵ�ָ�룬���ڴ���׵�ַͨ�����С����ͳ�ȥ��
								 (OS_MSG_SIZE  )sizeof(struct nw_ctl),     									//�����ֽڴ�С
								 (OS_OPT       )OS_OPT_POST_FIFO , //�Ƚ��ȳ�
								 (OS_ERR      *)&err);	                            //���ش�������	
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
					retxtime=3; 	//�������磬1�����ط�
					break;
				
				case  request_la_qj://22	//	�ϴ��������������������	
					if(nw_r_post->len==0)//��վ��������
					{
						printf("\r\n��վ����22����");
						sampled_step=0;
						sampled_data_sent_22(sampled_step,sampled_data_nub);
						SenTimes=2;
					}
					else if(nw_r_post->len==3)
					{
//						�ڼ�֡��ʶ 1�ֽ�	AA55H
						if((nw_r_post->data[1]==0xAA)&&(nw_r_post->data[2]==0x55))
						{
							printf("\r\n�յ���վ��22����,��ʶ��Ϊ%x",nw_r_post->data[0]);
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
								printf("\r\n���ݱ�ʶ�Ŵ���,��ʶ��ӦΪ%x",sampled_data_nub);
							}
						}
					}
					break;
				case  request_qx:		//0x25	//	�ϴ���������	
					if(nw_r_post->len==0)//��վ��������
					{
						printf("\r\n��վ����25����");
						sampled_step=3;
						sampled_data_sent_25(sampled_data_nub);
						ctlnumber=request_qx;
						SenTimes=2;
					}
					else if(nw_r_post->len==3)
					{
//						�ڼ�֡��ʶ 1�ֽ�	AA55H
						if((nw_r_post->data[1]==0xAA)&&(nw_r_post->data[2]==0x55))
						{
							printf("\r\n�յ���վ��25����,��ʶ��Ϊ%x",nw_r_post->data[0]);
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
								printf("\r\n���ݱ�ʶ�Ŵ���,��ʶ��ӦΪ%x",sampled_data_nub);
							}
						}
					}
					break;
					
				case 	Contact:			//00	//	����������Ϣ	װ�ÿ�������������Ϣ
					printf("\r\nCont len=:%d",nw_r_post->len);
				
					if((ctlnumber==Contact)&&(nw_r_post->len==2))//sizeof(version) ���ڿ�������ʱ���������ظ�����������Ϣ
					{
						if((nw_r_post->data[0]==nw_sc.data[0])&&(nw_r_post->data[1]==nw_sc.data[1]))
						{
							ctlnumber=CheckT;
							nw_sc.cmd=ctlnumber;
							nw_sc.len=0;//sizeof(systime);
							memcpy(nw_sc.data,systime,6);
							nw_sent(&nw_sc);
							retxtime=6;		//Уʱ��2�����ط�
							sentTime=worktime;	
						}
					}
					else if(nw_r_post->len==0) //
					{
//						ctlnumber=Contact; //������Ϊ ����̨���������������ź�
//						mode=1;
					}
					
//					retxtime=0; 	// ��һ�ε�ʱ��
					break;
					
				case  CheckT:				//01	//	Уʱ	�������������·�������
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
				case  SetPassword :	//02	//	����װ������	װ�ó������룺�ַ�����1234����31H32H33H34H��
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
//unsigned char Heabtime=60;//���������װ��������Ϣ���ͼ������λ���ӣ���������ӦΪ1����;
//unsigned short getdatetime=20;//�ɼ��������ÿ�����ٷ��Ӳ���һ�Σ��ɼ����������ʱ���޹أ�����λ���ӣ���������ӦΪ20���ӣ�
//unsigned short sleeptime=0;//����ʱ�������ݲɼ����ܹرջ�ͨ���豸����ʱ�䣬��ʱ���ڿ�֧�ֶ��Ż����绽�ѣ���λ���ӣ���Ϊ0��װ�ò����ߣ�
//unsigned short onlinetime=10;		//����ʱ����ͨ���豸�������ݲɼ�������ͨ���豸����ʱ�䣻��λ���ӣ�
//unsigned int resettime;	//3bye  Ӳ������ʱ��㣺Ϊ��֤װ������ɿ�����װ��Ӧ֧�ֶ�ʱ������
//unsigned short checkword[4]={'1','2','3','4'};	//������֤��4�ֽڣ�װ�ó�ʼΪ�ַ���1234����31H32H33H34H����Ϊȷ��װ�����ݵ���ȷ�ԣ���ֹ�Ƿ��û�������ƭ��������
////���������ڷ�ֹ�Ƿ�װ���û������ݱ���վ�Ͽɣ���װʱװ���趨Ĭ�����ģ�������װ��ɺ�
////��վ�·�ָ���޸ĸ�װ�����ģ���װ����վ��¼������һ��ʱ�Ӹ����ݺϷ���Ч���������Ρ�
				case  Pcfg :				//03	//	��վ�·���������	��ָ��Ҫ�����ݲɼ�װ�ý��յ��������ԭ�����
					
//3,0,12,31,32,33,34,  3c, 2,58, 0,0,  2,58,  0,2,3,  31,32,33,34,  b,16,
					printf("\r\n��������PSW=%x,%x,%x,%x",PSW[0],PSW[1],PSW[2],PSW[3]);
					printf("\r\n��������%x,%x,%x,%x",nw_r_post->data[0],nw_r_post->data[1],nw_r_post->data[2],nw_r_post->data[3]);
				
					printf("\r\nnw_r_post->data=%x,%x,%x,%x",nw_r_post->data[0],nw_r_post->data[1],nw_r_post->data[2],nw_r_post->data[3]);
				
					if((memcmp(PSW,nw_r_post->data,4)==0)&&(memcmp(CHW,&nw_r_post->data[14],4)==0)) //localpassw[50]
					{
						
						*HBT=nw_r_post->data[4];
						*GDT=(nw_r_post->data[5]<<8)+nw_r_post->data[6];
						*SLT=(nw_r_post->data[7]<<8)+nw_r_post->data[8];
						*OLT=(nw_r_post->data[9]<<8)+nw_r_post->data[10];
						RST[0]=nw_r_post->data[11]; //�� 0-28
						RST[1]=nw_r_post->data[12];	//ʱ 0-24
						RST[2]=nw_r_post->data[13];	//�� 0-60
						
						flash_save((uint8_t *)HBT,11);
						
						nw_sent(nw_r_post);
						printf("RST[0]=%d,RST[1]=%d,RST[2]=%d",RST[0],RST[1],RST[2]);
						if(RST[0]==0)
						{
							if(((RST[1]==systime[3])&&(RST[2]==systime[4])))
							{
								printf("׼����λ");
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
				case  HeartBeat:		//05	//	װ��������Ϣ	������վ���װ������ʱ�䡢IP��ַ�Ͷ˿ںš������ź�ǿ�ȼ����ص�ѹ
					break;
				case  chang_ip:			//06	//	������վIP��ַ���˿ںźͿ���	
					if(memcmp(PSW,nw_r_post->data,4)==0)
					{
//����	��վIP	�˿ں�	��վIP	�˿ں�	��վ����	��վ����
//4�ֽ�	4�ֽ�	2�ֽ�	4�ֽ�	2�ֽ�	6�ֽ�	6�ֽ�
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
							rev_err(chang_ip,0); //��������վIP���˿ںź���վ���Ŷ�Ӧ�ֽڲ���ȫ��ͬ ���ݳ���
						}
					}
					else
					{
						rev_err(chang_ip,0xff); //�������
					}
					break;
				case  check_ip:			//07	//	��ѯ��վIP��ַ���˿ںźͿ���	
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
				case  ResetDev:			//08	//	װ�ø�λ	��վ��װ�ý��и�λ
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
				case  waikup_device_m://09	//	���Ż���	��վ�Զ��ŷ�ʽ��������״̬��װ��
//					sys_mode=MODE_WORK;
					wakeup_dev();
					break;
				case  check_device_cfg://0A	//	��ѯװ�����ò���   68 43 43 30 31 39 36 0a 00 00 9f 16 
				//																								:68 43 43 30 31 39 36 0a 00 00 9f 16 
//				if(nw_r_post->len==0)
					{
						
						nw_sc.cmd=check_device_cfg;
//						memcpy(nw_sc.data,nw_ip,4);
						
//�������	1�ֽ�
//�ɼ����	2�ֽ�
//����ʱ��	2�ֽ�
//����ʱ��	2�ֽ�
//Ӳ������ʱ���	3�ֽ�
//ͨ��1ɫ��ѡ��	1�ֽ�
//ͨ��1ͼ���С	1�ֽ�
//ͨ��1����	1�ֽ�
//ͨ��1�Աȶ�	1�ֽ�
//ͨ��1���Ͷ�	1�ֽ�
//ͨ��2ɫ��ѡ��	1�ֽ�
//ͨ��2ͼ���С	1�ֽ�
//ͨ��2����	1�ֽ�
//ͨ��2�Աȶ�	1�ֽ�
//ͨ��2���Ͷ�	1�ֽ�
//��Ч����1	1�ֽ�
//��Ч����2	1�ֽ�
//����.	����
//��Ч����N	1�ֽ�				
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
						
						nw_sc.data[i++]=1;				//ͼ��1ɫ��ѡ��
//����ͼ���С�������£�
//320 X 240Ϊ1��
//640 X 480Ϊ2��
//704 X 576Ϊ3��
//��չͼ���С�������£�
//800 X 600Ϊ4��
//1024 X 768 Ϊ 5��
//1280 X 1024 Ϊ 6
//1280 X 720 Ϊ7
//1920 X 1080 Ϊ 8
//960H	960 x 576 Ϊ9
//960P 1280 x 960 Ϊ10 
//1200P  1600 x 1200Ϊ11  
//QXGA	2048 x 1536Ϊ12
//400W	2592 x 1520 Ϊ13
//500W	2592 x 1944 Ϊ14
//600W  3072*2048 Ϊ15
//800W(4K)	3840 x 2160Ϊ16
//1200W   4000 x 3000Ϊ17
//1600W	4608 x 3456Ϊ18
//QUXGA   3200x2400Ϊ19
//4224 x 3136Ϊ20			
//						if(Streaming_OK>=2)
//						{
//							nw_sc.data[i++]=WidthxHeight;
//							nw_sc.data[i++]=bLevel;nw_sc.data[i++]=cLevel;nw_sc.data[i++]=sLevel;
//						}
//						else
						{
							if(GET_Streaming_s()==0)	//ͼ��1ͼ���С
							{
								nw_sc.data[i++]=WidthxHeight;
								Streaming_OK=1;
							}		
							if(GET_Image_s()==0) //ͼ��1����,�Աȶ�,���Ͷ�
							{
								nw_sc.data[i++]=bLevel;nw_sc.data[i++]=cLevel;nw_sc.data[i++]=sLevel;
								if(Streaming_OK==1) Streaming_OK=2;
							}
						}
						
//						int GET_Image_s(void)//char chanle,signed char *brightnessLevel,signed char *contrastLevel,signed char *saturationLevel

						nw_sc.data[i++]=1;nw_sc.data[i++]=1;nw_sc.data[i++]=60;nw_sc.data[i++]=60;nw_sc.data[i++]=60; //ͨ��2����
						
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
				case  device_function_cfg://0B	//	װ�ù�������	
					if(memcmp(PSW,nw_r_post->data,4)==0)
					{
						function_nub=nw_r_post->len-4;
						memcpy(function_buf,&nw_r_post->data[4],function_nub);
						function_buf[nw_r_post->len-4]=0; //��β��0
						
						nw_sent(nw_r_post);
					}
					else
					{
						rev_err(device_function_cfg,0xff);
					}
					break;
				case  device_Sleep:	//0C	//	װ������	
					dev_sleep();
					break;
				case  check_device_time://0D	//	��ѯװ���豸ʱ��	
					nw_sc.cmd=check_device_time;
					nw_sc.len=6;
//					nw_sc.data=systime;
					memcpy(nw_sc.data,systime,6);
					nw_sent(&nw_sc);
					break;
				case  sent_msg:
					nw_sent(nw_r_post);
//				������	https://blog.csdn.net/qq_30460905/article/details/81805891
					break;
				case  request_data:	//21	//	��վ����װ������	��վ����ɼ�װ�òɼ����ݲ���������
					if(nw_r_post->len==0)//�ϴ�δ�ɹ��ϴ�����ʷ���ݣ�������ʷ��Ƭ����װ������ʷ�������ϴ�
					{
						sampled_data_time=1;
					}
					else if((nw_r_post->len==2)&&(nw_r_post->data[0]==0xBB)&&(nw_r_post->data[1]==0xBB))//װ�����̲ɼ��������ݣ�ͼƬ���⣩����ɲɼ��������ϴ����ôβ�����Ӱ��ԭ�趨�ɼ������ִ��
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
				}
				else
				{
					rev_err(0x81,0xff);
				}
				break;
				
			case 0x82:	//����ʱ�������
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
			case 0x83:	//��վ����������Ƭ
					if((cmding==0)||(cmding>=0x86))
					{
						cmding=nw_r_post->cmd;
						nw_sent(nw_r_post);
						get_photo_s();	
						OSTimeDly( 2000, OS_OPT_TIME_DLY, & err);
						
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
						SenTimes=4;
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
		//					OSTimeDly ( 10, OS_OPT_TIME_DLY, & err ); 
						}
						
						OSTimeDly ( 2000, OS_OPT_TIME_DLY, & err ); 
						
						retxtime=1;		//���ͽ������ 1s�ط�,Ҫ��30s,���5��
						sentTime=worktime;	
						nw_sc.cmd=0x86;//���ݽ���
						cmding=0x86;
						nw_sc.len=2;			
						nw_sc.data[0]=1;//ͨ����	
						nw_sc.data[1]=0;//Ԥ��λ��
						
						nw_sent(&nw_sc);
						ctlnumber=0x86;
						SenTimes=4;
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