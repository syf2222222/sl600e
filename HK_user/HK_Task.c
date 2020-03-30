

#include "global.h"
#include "sram.h" 
#include "fatfs.h" 
#include "hk.h"
void power_set_cam1(uint8_t flag);

uint8_t preset_now[2];

//extern unsigned char WidthxHeight,bLevel,cLevel,sLevel;
uint8_t hk_state,http_sent_flag;
extern unsigned char http_linkd;
unsigned char power_buf_cam1;
extern int work_p100ms;
int wt_start,hk_wait_time,hk_tast_step;
extern uint8_t http_client_flag;		//TCP�ͻ������ݷ��ͱ�־λ
extern OS_SEM http_rev_sem,http_data_sem;

int camera_authorization(char *sttp_buf,const char *method,const char *uri,const char ch);
int GET_photo(char *sttp_buf,char ch);
int GET_Streaming(char *sttp_buf,char ch);//��ѯͼ���С
int GET_Streaming_capabilities(char *sttp_buf,char ch);// ��ѯ�������õ�ͼ���С���
int GET_Image(char *sttp_buf,char ch);//��ѯͼ������,�Աȶ�,���Ͷ�
int PTZCtrl_continuous(char *sttp_buf,signed char x,signed char y,signed char z,char ch);//Pan_Tilt_Zoom �������Ҵ�С�ƶ�
int PTZCtrl_presets(char *sttp_buf,char presetN,char ch);	//����Ԥ��λ
int PTZCtrl_presets_goto(char *sttp_buf,char presetN,char ch); //ת��Ԥ��λ
int PTZ(char *sttp_buf,char *command,char *mode,char speed,int waitime,char ch);//,char *mode,char speed,��������ת�ļ򵥷�ʽ
int PTZ_Image(char *sttp_buf,signed char brightnessLevel,signed char contrastLevel,signed char saturationLevel,char ch);//����ͼ������,�Աȶ�,���Ͷ�
int PTZ_Streaming(char *sttp_buf,uint8_t hWidthxHeight,char ch);//����ͼ���С��������С��ɫ��ѡ���

void get_digest_info(char *in);
int get_color_info(char *in);
int Content_Length(char **in);
int get_Width_Height_info(char *in);

void  nw_sent_88(uint8_t *t_data,uint8_t ch);


uint32_t data_len,data_rlen;
uint8_t waiting_data,recv_200,recv_401,recv_htty_end,recv_htty_err,http_linkd_old;//recv_htty_my,
int hk_last_stime;

extern unsigned char systime[6]; //20 (19�� 9�� 29�� 9�� 20�� 47��)
uint8_t sent_cmd;
extern uint8_t sd_sta_h;
//extern char write_ph(unsigned char *time,unsigned char *in,int len);

int f_open_s(int name);
int f_write_s(char * in,uint32_t len);
int f_close_s(void);
int f_lseek_s(void);

//extern uint16_t	ph_sent_cnt,ph_gain_cnt;

int index_ph_inc(uint8_t *in);
extern	FIL fp; 
extern	UINT bw;
extern	UINT br;
int flash_save(uint8_t *ADD,uint8_t *buf,int len);
extern uint8_t	STM32_reseting,STM32_sleeping;
extern int int_time_to_char_time(uint8_t *out,uint32_t sec); //��2000-1-1 0:0:0 �ֿ�ʼ��
int last_ph_time;

void http_data_post(char * in,uint16_t len)//pending_http_data(char *in,int len,uint8_t sent_cmd,uint8_t ch)
{
//	int res;
	char *end;
	int i;
		
//		printf("�յ�\r\n%s\r\n",in); //�յ�����http		
		
	if(strncmp("HTTP/1.1 401", (char *)in,12)==0)
	{
		printf("\r\nHTTP/1.1 401"); //�յ�����http
		get_digest_info((char *)in); 	//ȡ��֤����
		recv_401=1;
	}
	else	if(strncmp("HTTP/1.1 200 OK", (char *)in,15)==0)
	{
		printf("\r\nHTTP/1.1 200 OK"); //�յ�����http
		
		end=in+15;
						
		if(sent_cmd==0x8b)	//��ѯ����,�Աȶ�,���Ͷ�
		{
//				printf("\r\n�յ�%s\r\n",in); //�յ�����http		
			if(get_color_info(end)==0)	//�ҵ�ֵ
			{
				recv_200=1;
				flash_save(bLevel,bLevel,3);
			}
			else
			{
				recv_200=1;
				printf("\r\nû�ҵ���ɫֵ"); 	
			}
		}
		else	if(sent_cmd==0x8a)	//��ѯͼ���С
		{
//				printf("\r\n�յ�%s\r\n",in); //�յ�����http	
			if(get_Width_Height_info(end)==0)
			{
				recv_200=1;
				flash_save(WidthxHeight,WidthxHeight,1);
			}
			else
			{
				recv_200=1;
				printf("\r\nû�ҵ���Сֵ"); 	
			}
		}
		else	if(sent_cmd==0x83)	//����
		{
			data_len=Content_Length(&end);
			data_rlen=0;
			PH_LEN[0]=0;
			PH_LEN[1]=0;
			PH_LEN[2]=0;
			PH_LEN[3]=0;
			
			if(last_ph_time>=runningtime) last_ph_time=last_ph_time+1;
			else last_ph_time=runningtime;
			int_time_to_char_time(PH_GET_T,last_ph_time); 
//			memcpy(PH_GET_T,systime,6);
			*PH_GET_CH=1;//ch
			*PH_GET_PS=preset_now[0];			
			recv_200=1;
			waiting_data=1;
		}
		else recv_200=1;
	}
	else if((waiting_data==1)&&(sent_cmd==0x83))
	{	
		memcpy(PH_DATA+data_rlen,in,len);
		data_rlen +=len;
		
		if(data_rlen>=data_len)
		{
			waiting_data=0;	
			PH_LEN[0]=(data_len>>0);
			PH_LEN[1]=(data_len>>8);
			PH_LEN[2]=(data_len>>16);
			PH_LEN[3]=(data_len>>24);
			recv_htty_end=1;
		}
		
		if(data_rlen==len)	//��ʼ��
		{
			printf("\r\n");
			for(i=0;i<5;i++)	printf("%02x",in[i]); //��ӡǰ����
			printf("\r\n");
		}
		else if(recv_htty_end) //������
		{
			printf("\r\n");
			for(i=len-5;i<len;i++)	printf("%02x",in[i]); //��ӡ�󼸸�				
			printf("\r\nphoto data rev complete len=%d",data_rlen);	
		}
		else
		{
			printf(".");
		}
	}
}


	
uint8_t hk_step;	
uint8_t sent_times,hk_tast_old,HK_Task_free=0;
extern uint8_t NBLOCK_flag,netconn_connect_time;
uint8_t netconn_err;	
extern OS_TCB   http_hk_thread_TCB;															//������ƿ�
void Create_hk_thread(void);
extern uint16_t cam_on_time_detect;

extern uint8_t function84_flag;

void  HK_Task_ch1 ( void * p_arg )
{
	OS_ERR         err;
	OS_MSG_SIZE    msg_size;
//	char *in;
	
	uint8_t pMsg[5],*Get_Msg;
	
	int i;
	#define ch 1
		
	(void)p_arg;

					 
//  cpu_clk_freq = BSP_CPU_ClkFreq();                 //��ȡCPUʱ�ӣ�ʱ������Ը�ʱ�Ӽ���

	hk_state=0;
	hk_wait_time=0;
	while (DEF_TRUE) {                                           //������
		OSTimeDly( 100, OS_OPT_TIME_DLY, & err);
		
		if(hk_tast_old != hk_tast_step)
		{
			hk_tast_old=hk_tast_step;
			hk_step=0;
			hk_wait_time=0;
//			sent_cnt=0;
		}
		if(hk_tast_step==2) //�޿�״̬,�ȴ�����Ž�����һ������
		{
			OSTimeDly ( 300, OS_OPT_TIME_DLY, & err ); 
			if(PH_sram_buf==bempty) hk_tast_step=1;
		}
		
		if(hk_tast_step==0)
		{			
			Get_Msg=OSQPend((OS_Q*			)&hktask_Asim[0],   
											(OS_TICK		)1000,		//6000,	//							
											(OS_OPT			)OS_OPT_PEND_BLOCKING,
											(OS_MSG_SIZE*	)&msg_size,		
											(CPU_TS*		)0,
											(OS_ERR*		)&err);
			
			
			if((err==OS_ERR_NONE)&&(function84_flag))		//�յ�������Ϣ
			{
				HK_Task_free=0;
				hk_wait_time=0;
				for(i=0;i<5;i++) pMsg[i]=Get_Msg[i];
				printf("\r\ncamera got command %02x",pMsg[0]);
				if(pMsg[0]==1)	//����Դ
				{
					power_set_cam1(1);	//cam1 ����ͬʱ����ETH	
					power_buf_cam1=1;
					nw_sent_88(pMsg,ch); //ԭ�����
//					printf("�������Դ");
				}
				else if(pMsg[0]==10)//�ص�Դ
				{
					power_set_cam1(0);
					
						nw_sent_88(pMsg,ch); //ԭ�����
					power_buf_cam1=0;
//					printf("�ر������Դ");
				}
				else			
				{
					if((pMsg[0]==0x83)&&(sd_sta_h==0)&&((PH_sram_buf==bpre_bready)||(PH_sram_buf==bready)))
					{
						PH_sram_buf=bready;
						senttask_Asim |= data_flag84_1;	//��ƬOK���ϴ����߱��������������
						hk_tast_step=2;
					}
					else
					{
						if(power_flag_cam1==0)
						{
							power_set_cam1(1); //�������Դ
						}
						
						if((pMsg[0]!=0x83)||(sd_sta_h==0))
						{
							if(PH_sram_buf==bpre_bready)
							{
								PH_sram_buf=bready;
								senttask_Asim |= data_flag84_1;	//��ƬOK���ϴ����߱��������������
	//							printf("\r\n��Ƭ���ݸ�����");
							}
							if((PH_sram_buf!=bpre_sent_buf)&&(PH_sram_buf!=bsenting_buf)&&(PH_sram_buf!=bready))//&&(PH_sram_buf!=bpre_sent_sd)&&(PH_sram_buf!=bsenting_sd)
							{
								PH_sram_buf=bempty;
							}
						}					
						hk_tast_step=1;
						http_sendbuf[0]=0;
						sent_cmd=pMsg[0];
						sent_times=0;
						hk_step=0;
						recv_401=0;
						recv_200=0;
						power_buf_cam1=1;
					}					
				}
			}
			else 	//û����
			{
//				if((STM32_reseting)||(STM32_sleeping))
				if(STM32_reseting)
				{
					if((PH_sram_buf==bpre_bready)||(PH_sram_buf==bready)||(PH_sram_buf==bpre_sent_buf)||(PH_sram_buf==bsenting_buf)) //���ڷ�����ô����?
					{
						PH_sram_buf=bpre_load_hk;
						if(index_ph_inc(PH_GET_T)==0)
						{
							PH_sram_buf=bempty;
							HK_Task_free=1;
						}
					}
				}
				else if(PH_sram_buf==bpre_bready) //˯��ǰ�ȷ���Ƭ
				{
					PH_sram_buf=bready;
					senttask_Asim |= data_flag84_1;	//��ƬOK���ϴ����߱��������������
//					NW_Task_step =0;
				}
				else
				{
					HK_Task_free=1;
				}
				
				if((PH_sram_buf!=bpre_sent_buf)&&(PH_sram_buf!=bsenting_buf)&&(PH_sram_buf!=bready))//&&(PH_sram_buf!=bpre_sent_sd)&&(PH_sram_buf!=bsenting_sd)
				{
					PH_sram_buf=bempty;
				}
				if(((hk_wait_time)>600)&&(power_flag_cam1)) //��ʱû�����Զ��ر�
				{
					printf("\r\ncamera off by timeout");
					power_set_cam1(0); //�������Դ
					power_buf_cam1=0; 
				}
			}
		}
		
		
		else	if(hk_tast_step==1)
		{
//			signed char x,y,z;
//			if(http_sent_flag)
//			{
//			}
			
			if(recv_401) //���¿�ʼ����
			{
//				printf("\r\n������֤");
				recv_401=0;
				hk_step=0;
				sent_cmd=pMsg[0];
				hk_wait_time=0;
			}
			
			if(http_linkd==0)
			{
				http_linkd_old=0;
				if(hk_wait_time>(hk_power_on_time+1800))//&&(http_sent_flag)) 
				{
					printf("\r\nhttp���ӳ�ʱ,�����λ����");
					power_set_cam1(0);
					OSTimeDly ( 5000, OS_OPT_TIME_DLY, & err ); 
					power_set_cam1(1);
					hk_wait_time=0;
					hk_tast_step=0;
				}
				else
				{
//					printf("1");
					OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err ); 
					if(NBLOCK_flag) 
					{
						hk_wait_time=0;
						if(NBLOCK_flag==1)
						{
							printf("?"); //������������������� netconn_connect
							netconn_err++;
//							if(netconn_err==10)
//							{
//								printf("��������쳣,ɾ�������ؽ�");
//								OSTaskDel(&http_hk_thread_TCB,& err );
//								OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err ); 
//								Create_hk_thread();
//							}
//							else 
								if(netconn_err>=180)
							{
								netconn_err=0;
								printf("��������쳣���������,ɾ�������ؽ�");
								power_set_cam1(0);
								OSTimeDly ( 3000, OS_OPT_TIME_DLY, & err ); 
								power_set_cam1(1);
//								OSTaskDel(&http_hk_thread_TCB,& err );
//								OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err ); 
//								Create_hk_thread();
							}
//							HAL_NVIC_SystemReset(); //����ϵͳ
						}
						if(NBLOCK_flag==2)
						{
							netconn_err=0;
							printf("*");
						}
						netconn_connect_time++;
					}
					else netconn_err=0;
				}
			}
			else //if(http_sent_flag==1)
			{
				netconn_err=0;
				if(http_linkd_old==0)
				{
					http_linkd_old=1;
					hk_wait_time=0;
				}
				if(hk_wait_time>100)
				{
					if(sent_times<8)
					{
						sent_times++;
						http_sent_flag=1; //�ط�
						hk_wait_time=0;
					}
					else
					{
						printf("\r\nTCP����ʱ,ָ�ʧ");
						hk_tast_step=0;
					}
				}
			}
			
			
			
			
			
			switch(pMsg[0])//sent_cmd
			{					
				case 3:
				case 4:
				case 5:
				case 6:
				case 7://������Զ������1����λ����ͷ�䱶�Ŵ�
				case 8://�������������1����λ����ͷ�䱶��С��
					if(hk_step==0)
					{
//						if(pMsg[0]==3)		{x=0;y=28;z=0;}
//						if(pMsg[0]==4)		{x=0;y=-28;z=0;}
//						if(pMsg[0]==5)		{x=-28;y=0;z=0;}					
//						if(pMsg[0]==6)		{x=28;y=0;z=0;}
//						if(pMsg[0]==7)		{x=0;y=0;z=-28;}					
//						if(pMsg[0]==8)		{x=0;y=0;z=28;}
						
						if(pMsg[0]==3) PTZCtrl_continuous((char *)http_sendbuf,0,28,120,ch);
						if(pMsg[0]==4) PTZCtrl_continuous((char *)http_sendbuf,0,-28,120,ch);
						if(pMsg[0]==5) PTZCtrl_continuous((char *)http_sendbuf,-28,0,120,ch);
						if(pMsg[0]==6) PTZCtrl_continuous((char *)http_sendbuf,28,0,120,ch);
						if(pMsg[0]==7) PTZCtrl_continuous((char *)http_sendbuf,120,120,-28,ch);
						if(pMsg[0]==8) PTZCtrl_continuous((char *)http_sendbuf,120,120,28,ch);
						hk_step++;
						http_sent_flag=1;
						preset_now[0]=0; //Ԥ��λ����
					}
					if(hk_step==1)
					{
						if(recv_200)
						{
							OSTimeDly ( 300, OS_OPT_TIME_DLY, & err ); 
							PTZCtrl_continuous((char *)http_sendbuf,0,0,0,ch);	//ֹͣת��
							hk_step++;
							http_sent_flag=1;
							recv_200=0;
						}
					}
					if(hk_step==2)
					{
						if(recv_200)
						{
							nw_sent_88(pMsg,ch); //ԭ�����
							hk_tast_step=0;
							recv_200=0;
						}
					}					
				break;
				
				case 2://��������ڵ�ָ��Ԥ�õ�
				case 9://���浱ǰλ��ΪĳԤ�õ�		
				if(hk_step==0)
					{
						if(pMsg[0]==2) PTZCtrl_presets_goto((char *)http_sendbuf,pMsg[1],ch); //ת��Ԥ��λ
						if(pMsg[0]==9) PTZCtrl_presets((char *)http_sendbuf,pMsg[1],ch);	//���浱ǰλ��ΪĳԤ�õ�		
						hk_step++;
						http_sent_flag=1;
						preset_now[0]=0;	//���ĵ�ǰԤ��λ
					}
					if(hk_step==1)
					{
						if(recv_200)
						{
							preset_now[0]=pMsg[1];	//���ĵ�ǰԤ��λ
							nw_sent_88(pMsg,ch); //ԭ�����
							hk_tast_step=0;
							recv_200=0;
						}
					}
				break;
				
				case 0x81: 	//����ͼ�����  //����ͼƬ����ԭ����� @@
					if(hk_step==0)
					{
						PTZ_Streaming((char *)http_sendbuf,pMsg[1],ch); //����ͼ���С����
						hk_step++;
						http_sent_flag=1;
					}
					if(hk_step==1)
					{
						if(recv_200)
						{
							PTZ_Image((char *)http_sendbuf,pMsg[2],pMsg[3],pMsg[4],ch); //����ͼ������,�Աȶ�,���Ͷ�
							hk_step++;
							http_sent_flag=1;
							recv_200=0;
						}
					}
					if(hk_step==2)
					{
						if(recv_200) 
						{
							senttask_Asim |= data_flag81_1;	 //ԭ����أ���Ҫͨ��1��2ͬʱ�ɹ�
							hk_step++;
							recv_200=0;
						}
					}
//				case  0x8a:	//��ѯͼ���С
					if(hk_step==3)
					{
						sent_cmd=0x8a;
						printf("\r\n��ѯͼ���С");
						GET_Streaming((char *)http_sendbuf,ch);//��ѯͼ���С
						hk_step++;
						http_sent_flag=1;
					}
					if(hk_step==4)
					{
						if(recv_200) 
						{
//							printf("\r\n��ѯͼ���СOK");
							hk_step++;
							recv_200=0;
						}
					}
//				case  0x8b:	//��ѯ����,�Աȶ�,���Ͷ�
					if(hk_step==5)
					{
						sent_cmd=0x8b;
						printf("\r\n��ѯͼ������");
						GET_Image((char *)http_sendbuf,ch);
						hk_step++;
						http_sent_flag=1;
					}
					if(hk_step==6)
					{
						if(recv_200) 
						{
							hk_tast_step=0;
							recv_200=0;
						}
					}
				break;
								
				case 0x83://����
					if(hk_step==0)
					{
						if((pMsg[1] != preset_now[0])&&(pMsg[1] != 0))
						{
							sent_cmd=0x02;
							PTZCtrl_presets_goto((char *)http_sendbuf,pMsg[1],ch); //ת��Ԥ��λ
							hk_step++;
							http_sent_flag=1;
//							printf("\r\nת��Ԥ��λ%d",pMsg[1]);
						}
						else
						{
//							printf("\r\nԤ��λ��ȷ");
							hk_step=2;
						}
					}
					if(hk_step==1) //�ȴ�Ԥ��λ���
					{
						if(recv_200) 
						{
							hk_step++;
							recv_200=0;
							preset_now[0]=pMsg[1];
							printf("\r\nin preset %d",preset_now[0]);
						}
					}
						
					if(hk_step==2)
					{
						if((PH_sram_buf==bempty)||(PH_sram_buf==bpre_load_hk)||(PH_sram_buf==bloading_hk))	//���ջ���Ϊ��
						{
							PH_sram_buf=bpre_load_hk;//PH_flag[0]=0x10; //��������
							sent_cmd=0x83;
							GET_photo((char *)http_sendbuf,ch);
							hk_step++;
							http_sent_flag=1;
							recv_htty_end=0;
							recv_htty_err=0;
							recv_200=0;
							printf("\r\ngoing to take a photo");
						}						
						else if((PH_sram_buf==bready)||(PH_sram_buf==bpre_bready))	//ǰ������,�����̻�δ����
						{
							if(sd_sta_h)
							{
								PH_sram_buf=bpre_load_hk;//PH_flag[0]=0x10; //��������
								
								if(index_ph_inc(PH_GET_T)==0)
								{
									sent_cmd=0x83;
									GET_photo((char *)http_sendbuf,ch);
									hk_step++;
									http_sent_flag=1;
									recv_htty_end=0;
									recv_htty_err=0;
									recv_200=0;
									printf("\r\nsave ph in SD,going to take a photo");
								}
								else
								{
									PH_sram_buf=bready;//PH_flag[0]=1; //�ָ�����Ƭ����״̬
									printf("\r\nsave to SD fail");
									hk_tast_step=0; //�˳�,�ݲ�����
									OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err ); 
								}
							}
							else
							{
								PH_sram_buf=bready;//PH_flag[0]=1; //�ָ�����Ƭ����״̬
								printf("\r\nsave to SD fail");
								hk_tast_step=0; //�˳�,�ݲ�����
								OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err ); 
							}
						}
						else if((PH_sram_buf==bpre_sent_buf)||(PH_sram_buf==bsenting_buf))//  (PH_flag[0]==2)	//�����ϴ�������Ƭ
						{
							printf("\r\n�����ϴ���Ƭ,���յȴ�");
							hk_wait_time=0;
							OSTimeDly( 2000, OS_OPT_TIME_DLY, & err);
						}
//						else if((PH_sram_buf==bpre_sent_sd)||(PH_sram_buf==bsenting_sd))//(PH_flag[0]==0x84)	//�����ϴ��洢��Ƭ
//						{
//							printf("\r\n�����ϴ��洢��Ƭ,���յȴ�");
//							OSTimeDly( 2000, OS_OPT_TIME_DLY, & err);
//						}
						else
						{
							PH_sram_buf=bempty;
							hk_tast_step=0;
							printf("\r\nbuf�����쳣");
						}
					}
					if(hk_step==3)
					{
						if(recv_htty_end)	
						{
							PH_sram_buf=bpre_bready;//PH_flag[0]=1; //������,����������
							printf("\r\ngain a photo");
							hk_tast_step=0;
							recv_htty_end=0;
							recv_200=0;
						}
						if(recv_htty_err)	//���ݴ���
						{
							hk_tast_step=0;
						}
					}
					else OSTimeDly( 100, OS_OPT_TIME_DLY, & err);
					
				break;
				
				case  0x8a:	//��ѯͼ���С
					if(hk_step==0)
					{
						GET_Streaming((char *)http_sendbuf,ch);//��ѯͼ���С
						hk_step++;
						http_sent_flag=1;
					}
					if(hk_step==1)
					{
						if(recv_200) 
						{
							hk_tast_step=0;
							recv_200=0;
						}
					}
					break;
			
				case  0x8b:	//��ѯ����,�Աȶ�,���Ͷ�
					if(hk_step==0)
					{
						GET_Image((char *)http_sendbuf,ch);
						hk_step++;
						http_sent_flag=1;
					}
					if(hk_step==1)
					{
						if(recv_200) 
						{
							hk_tast_step=0;
							recv_200=0;
						}
					}
					break;
					
					default:hk_tast_step=0; break;
				}
			
		}
	}//while (DEF_TRUE)
}

//		if(hk_tast_step==2)
//		{
//			OSTimeDly( 100, OS_OPT_TIME_DLY, & err);
//			if(recv_401)
//			{
//				printf("\r\n�����յ�401");
////				load_http_sendbuf((char *)http_sendbuf,sent_cmd,&pMsg[1],ch);
//				recv_401=0;
//				http_sent_flag=1;
//				hk_last_stime=worktime;
//			}
//			else if(recv_200)
//			{
//				printf("\r\n�����յ�200");
//				if(pMsg[0]<=62)
//				{
//					nw_sent_88(pMsg,ch); //ԭ�����
//					hk_tast_step=0;
//				}
//				else if(pMsg[0]==0x81) 
//				{
//					if(sent_cmd==0x81)
//					{
//						sent_cmd=0x82;
//						sent_times=0;
//					}
//					else if(sent_cmd==0x82)
//					{
//						hk_tast_step=0;
//						senttask_Asim |= data_flag81_1;	 //ԭ����أ���Ҫͨ��1��2ͬʱ�ɹ�
//					}
//				}
//				else if(pMsg[0]==0x83) 
//				{
//					if(sent_cmd==2)
//					{
//						sent_cmd=0x83;
//						sent_times=0;
//					}
//					else if(sent_cmd==0x83)
//					{
//						hk_tast_step=0;
//						senttask_Asim |= data_flag84_1;	//��ƬOK���ϴ����߱��������������
//					}
//				}
//				
//				recv_200=0;
//				hk_tast_step=0;
//			}
//			else if((worktime-hk_last_stime)>=4)
//			{
//				printf("\r\n�������·���");
//				http_sent_flag=1;
//				hk_last_stime=worktime;
//			}
//		}
//			
//		if(hk_tast_step==1)
//		{
//			wt_start=worktime;
//			if(pMsg[0]==0x83) 
//			{
//				if((preset_now[0]!=pMsg[1])&&(pMsg[1] != 0))
//				{
//					sent_cmd=2;//��������ڵ�ָ��Ԥ�õ�
//				}
//			}
//			printf("\r\nHK_Task_ch1׼��sent����");
////			load_http_sendbuf((char *)http_sendbuf,sent_cmd,&pMsg[1],ch);
//			
//			if(http_sendbuf[0])
//			{
//				if(power_flag_cam1==0)
//				{
//					camera_on(1);
//					printf("\r\nHK_Task_ch1����ϵ�,�ȴ�30s");
//					OSTimeDly( 30000, OS_OPT_TIME_DLY, & err);
//				}
//				
//				printf("\r\nhttp_sent_flag��λ,׼����");
//				http_sent_flag=1;
//				
//				recv_200=0;		
//				recv_401=0;	
//				hk_tast_step=2;
//				
//			}
//		}		
//	}
//}
//		if(hk_tast_step==2)
//		{
//			in=OSQPend((OS_Q*			)&http_recv[ch-1],   
//											(OS_TICK		)3000,					
//											(OS_OPT			)OS_OPT_PEND_BLOCKING,
//											(OS_MSG_SIZE*	)&in_size,		
//											(CPU_TS*		)0,
//											(OS_ERR*		)&err);
//				
//				if(err==OS_ERR_NONE)	//�յ����ݣ����ݴ���
//				{
//					printf("�յ�\r\n%s\r\n",in); //�յ�����http
////					printf("\r\nHK_Task_ch1�յ�����");
//					pending_http_data(in,in_size,sent_cmd,ch);
//					if(recv_200==1)
//					{
//						if(pMsg[0]<=62)
//						{
//							nw_sent_88(pMsg,ch); //ԭ�����
//							hk_tast_step=0;
//						}
//						else if(pMsg[0]==0x81) 
//						{
//							if(sent_cmd==0x81)
//							{
//								sent_cmd=0x82;
//								sent_times=0;
//							}
//							else if(sent_cmd==0x82)
//							{
//								hk_tast_step=0;
//								senttask_Asim |= data_flag81_1;	 //ԭ����أ���Ҫͨ��1��2ͬʱ�ɹ�
//							}
//						}
//						else if(pMsg[0]==0x83) 
//						{
//							if(sent_cmd==2)
//							{
//								sent_cmd=0x83;
//								sent_times=0;
//							}
//							else if(sent_cmd==0x83)
//							{
//								hk_tast_step=0;
//								senttask_Asim |= data_flag84_1;	//��ƬOK���ϴ����߱��������������
//							}
//						}
//					}
//				}
//				else
//				{
//					printf("\r\nHK_Task_ch1��ʱ");
//					if(sent_times>6) //����ʧ��6��
//					{
//						printf("\r\n�����������ʧ��");
//						hk_tast_step=0;
//					}
//				}
//		}
#define  http_hk_thread_SIZE                     600
#define  http_hk_thread_PRIO                         6
OS_TCB   http_hk_thread_TCB;															//������ƿ�
CPU_STK  http_hk_thread_Stk [ http_hk_thread_SIZE ];	//�����ջ

#define  HK_Task_ch1_SIZE                     800
#define  HK_Task_ch1_PRIO                         6
OS_TCB   HK_Task_ch1_TCB;															//������ƿ�
CPU_STK  HK_Task_ch1_Stk [ HK_Task_ch1_SIZE ];	//�����ջ

void http_hk_thread(void *arg);	

//OS_SEM http_rev_sem,http_200_sem,http_401_sem,http_data_sem;

void Create_hk_thread(void)
{
	OS_ERR      err;
	int i;
	for(i=0;i<http_hk_thread_SIZE;i++)
	{
		http_hk_thread_Stk [ i ]=0;
	}
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
}
void HKTaskStart (void *p_arg)
{
	OS_ERR      err;
	
//		��ֵ�ź������ƿ�ָ��		//��ֵ�ź������� //��Դ��Ŀ���¼��Ƿ�����־ //���ش�������
//	OSSemCreate (&http_rev_sem, "http_rev_sem",  0, &err); //3 	

////		��ֵ�ź������ƿ�ָ��		//��ֵ�ź������� //��Դ��Ŀ���¼��Ƿ�����־ //���ش�������
//	OSSemCreate (&http_200_sem, "http_200_sem",  0, &err); //3 	
//	
////		��ֵ�ź������ƿ�ָ��		//��ֵ�ź������� //��Դ��Ŀ���¼��Ƿ�����־ //���ش�������
//	OSSemCreate (&http_data_sem, "http_data_sem",  0, &err); //3 	

//��Ϣ����	 //��Ϣ�������� //��Ϣ��������  //��Ϣ���г��� //������
	OSQCreate (&hktask_Asim[0],"hktask_Asim[0]",256,&err);	

//��Ϣ����	 //��Ϣ�������� //��Ϣ��������  //��Ϣ���г��� //������
	OSQCreate (&http_recv[0],"http_recv[0]",1,&err);	

	
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







