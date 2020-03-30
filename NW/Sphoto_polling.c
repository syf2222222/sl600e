



#include "global.h"
#include "nw.h"
#include "sram.h"

void nw_sent_st(struct nw_ctl *t_data);
unsigned short tt_pk_nb;//pk_nb,
unsigned int tt_len;


extern unsigned short sentdelay;
extern uint8_t waiting_data;

int read_data_ph(uint16_t rcnt,uint8_t *out);
int index_ph_dec(unsigned char *index);
int read_ph(unsigned char *in,unsigned char *out,int len);
//void  nw_sent_ph_time(uint8_t ch);
void  nw_sent_ph(unsigned char *input,uint16_t len,uint8_t ch,uint8_t preset,uint16_t peket);
int sent_ph_sd(uint8_t *index,uint8_t *data_in,int tt_len);


uint8_t ph_index_buf[11];

#define	sent_wait		0
#define	sent_sd			1
#define	sent_sram		2


extern uint8_t Contact_state;

uint8_t Sphoto_step=0,Sphoto_state,Sphoto_cnt,Sphoto_err,ReSphoto_cnt;
uint16_t Sphoto_time;

extern uint8_t sd_sta_h;

uint8_t function84_flag;

void resent_ph(uint8_t *data_in,uint16_t data_len)
{
	OS_ERR      err; 
	uint16_t i;
	
	if(Sphoto_state==sent_sd)
	{
//		printf("\r\nre_send photo,pk_nb=%d,tt_len=%d,",tt_pk_nb,tt_len);
//		sent_ph_sd(ph_index_buf,data_in,tt_len);
		if(sent_ph_sd(ph_index_buf,data_in,tt_len)==0)
		{
			sentdelay=delayT;
			Sphoto_time=10; //��ֵ1s, 3s��
			ReSphoto_cnt=0;	//�ط���������
			Sphoto_step=7;	
		}
		else
		{
			printf("\r\n�ط����ļ�ʧ�ܣ������˰�");
			index_ph_dec(ph_index_buf); //δ����������һ
			Sphoto_step=0;
		}
	}
	else
	{
		unsigned short pk_nb;
		for(i=0;i<data_in[2];i++) //data_in[2] �貹��������
		{			
			OSTimeDly( 20, OS_OPT_TIME_DLY, & err);
			pk_nb=(data_in[2*i+3]<<8)+data_in[2*i+4]; //������ ��ֵ
			if(pk_nb==tt_pk_nb) nw_sent_ph(PH_DATA+((pk_nb-1)*nw_pk_len),tt_len-((pk_nb-1)*nw_pk_len),*PH_GET_CH,*PH_GET_PS,pk_nb);  //��β��,����Ϊ tt_len-((pk_nb-1)*nw_pk_len
			else	nw_sent_ph(PH_DATA+((pk_nb-1)*nw_pk_len),nw_pk_len,*PH_GET_CH,*PH_GET_PS,pk_nb); //�ǽ�β�� ����Ϊ nw_pk_len
			printf("%d ",pk_nb);
		}
		sentdelay=delayT;
		Sphoto_time=10; //��ֵ1s, 3s��
		ReSphoto_cnt=0; //�ط���������
		Sphoto_step=7;	
	}

}

void Sphoto_polling(void)
{
//	OS_ERR      err; 
	
	switch(Sphoto_step)
	{
		case 0:
			if((ph_sent_cnt!=ph_gain_cnt)&&(sd_sta_h==1)&&(function84_flag))
			{
				Sphoto_state=sent_sd;
//				printf("\r\n׼�����洢��Ƭ");
				Sphoto_step=2;
			}
			else if((PH_sram_buf==bready)&&(function84_flag))//����׼����
			{
				PH_sram_buf=bpre_sent_buf;
				Sphoto_state=sent_sram;
//				printf("\r\n׼����������Ƭ");
				Sphoto_step=3;
			}
			else
			{
				Sphoto_state=sent_wait;
				if((PH_sram_buf==bpre_sent_buf)||(PH_sram_buf==bsenting_buf))
				{
					PH_sram_buf=bempty;
				}
			}
			break;
		case 2:
			if(read_data_ph(ph_sent_cnt,ph_index_buf)==0)	//��ȡ����������ȷ
			{
				Sphoto_step=4;
				Sphoto_cnt=0;	
			}
			else //��ȡ�������ݴ���
			{
				printf("\r\n///��ʧһ��ͼƬ,84��ȡ�洢�������ݴ���");
				index_ph_dec(ph_index_buf); //�����˰�
				Sphoto_step=0;
			}
			break;
		
		case 3:
			if(PH_sram_buf==bpre_sent_buf) //������Ȩ
			{
				Sphoto_step=4;
				Sphoto_cnt=0;
				PH_sram_buf=bsenting_buf;
				memcpy(ph_index_buf,PH_GET_T,11);
			}
			else
			{
				printf("\r\n84���ݳ�ͻ,PH_sram_buf=%d",PH_sram_buf);
				Sphoto_step=0;
			}
			break;
			
		case 4://����������Ƭ 1s�ط�,Ҫ��3s,���5��		
__Sphoto_polling_case4:
		
			nw_sc.cmd=0x84;
			nw_sc.len=10;		

			memcpy(nw_sc.data,ph_index_buf,8);
			tt_len=((ph_index_buf[8])<<0) +(ph_index_buf[9]<<8)+(ph_index_buf[10]<<16);
			
			
			if(tt_len>nw_pk_max) tt_len=nw_pk_max;
			tt_pk_nb=(tt_len/nw_pk_len);//1024һ��
			if(tt_len%nw_pk_len) tt_pk_nb +=1;
//					printf("\r\nsend photo start0,pk_nb=%d,tt_len=%d,",tt_pk_nb,tt_len);					
			
			nw_sc.data[8]=(tt_pk_nb>>8);//������λ
			nw_sc.data[9]=tt_pk_nb;			//������λ
			
			nw_sent_st(&nw_sc);
			senttask_Pend |= data_flag84_1;
			Sphoto_step++;
			Sphoto_cnt++;
			Sphoto_time=0;
			break;
		
		case 5://�ȴ���
			if((senttask_Pend & data_flag84_1)==0)	//�յ���
			{
				Sphoto_time=0;
				Sphoto_step++;
//						printf("\r\n�յ�����ͬ���ϴ�");
			}
			else if((Sphoto_time)>=30)
			{

//					printf("\r\n��ʱ%x",sent_cnt);
				if(Sphoto_cnt>=5)
				{
//					if(Sphoto_state==sent_sram) index_ph_inc(PH_GET_T); //δ����������һ,����ͼƬ
					if(Sphoto_state==sent_sd)
					{
						if((ph_index_buf[0]<y_min)||(ph_index_buf[0]>y_max)||(ph_index_buf[1]==0)||(ph_index_buf[1]>12)||(ph_index_buf[2]==0)||(ph_index_buf[2]>31)) //ʱ��Ϊy_min-y_max���������,0��
						{
							printf("\r\nʱ�����ݴ���,����");
						}
					}
					Sphoto_step=0;
				}
				else  //�������ڼ��յ���?
				{
					Sphoto_step--;
					goto __Sphoto_polling_case4;
				}
			}
			break;
			
		case 6:	//��ʼ�ϴ���Ƭ
			if(((Sphoto_time>=1)&&(waiting_data==0))||(Sphoto_time>=30))	//�ȴ�100ms
			{
				if(Sphoto_state==sent_sd) 
				{
//					printf("\r\nsend photo,pk_nb=%d,tt_len=%d,",tt_pk_nb,tt_len);
//					sent_ph_all(read_buf,tt_len);
					if(sent_ph_sd(ph_index_buf,0,tt_len)==0)
					{
						sentdelay=delayT; 
						printf("\r\nsend photo complete");
						
						Sphoto_time=0;
						Sphoto_cnt=0;
						ReSphoto_cnt=0;
						Sphoto_step++;	//����,׼���� 0x86ָ�������Ӧ��
					}
					else
					{
						printf("\r\n///��ʧһ��ͼƬ,���ļ�ʧ��");
						index_ph_dec(ph_index_buf); //δ����������һ
						Sphoto_step=0;
					}
				}
				else
				{
					unsigned short pk_nb;
					printf("\r\nsend photo,pk_nb=%d,tt_len=%d,sram",tt_pk_nb,tt_len);

					for(pk_nb=1;pk_nb<=tt_pk_nb;pk_nb++) //��ʱ����
					{
						if(pk_nb==tt_pk_nb) nw_sent_ph(PH_DATA+((pk_nb-1)*nw_pk_len),tt_len-((pk_nb-1)*nw_pk_len),*PH_GET_CH,*PH_GET_PS,pk_nb);
						else	nw_sent_ph(PH_DATA+((pk_nb-1)*nw_pk_len),nw_pk_len,*PH_GET_CH,*PH_GET_PS,pk_nb);
	//					printf("\r\nA");
	//					OSTimeDly ( 3, OS_OPT_TIME_DLY, & err ); 
					}
					
					sentdelay=delayT; 
					printf("\r\nsend photo complete");
					
					Sphoto_time=0;
					Sphoto_cnt=0;
					ReSphoto_cnt=0;
					Sphoto_step++;	//����,׼���� 0x86ָ�������Ӧ��
				}
				
				
			}
			break;

				
		case 7:	
			if(Sphoto_time>=30) //3s��
			{
__Photo_case7:				
				nw_sc.cmd=0x86;//���ݽ���
				nw_sc.len=2;			
				nw_sc.data[0]=*PH_GET_CH;//ͨ����	
				nw_sc.data[1]=*PH_GET_PS;//Ԥ��λ��
				senttask_Pend |= data_flag86_1;
			
				nw_sent_st(&nw_sc);
				Sphoto_step=8;	
				Sphoto_time=0;
				Sphoto_cnt++;	//��������
				ReSphoto_cnt++; //ÿ�β������ؼ���
			}
			break;
			
		case 8:
			if((senttask_Pend & data_flag86_1)==0)
				{
					printf("\r\nsend photo OK");
					if(Sphoto_state==sent_sd)	index_ph_dec(ph_index_buf); //δ����������һ
					Sphoto_step=0;
					Sphoto_err=0;
				}
				else if((Sphoto_time)>=300/testdvi)
				{
//					if(STM32_sleeping)
//					{
//						Sphoto_step=0; //�ȴ�����,�Ƚ���
//						if((Sphoto_state==sent_sram)&&(PH_sram_buf==bsenting_buf)) PH_sram_buf=bready;	//��������û�ı�״̬�Ļ�, �������·ŵ�׼����	
//					}
//					else 
					if((Sphoto_cnt>=20)||(ReSphoto_cnt>=5)) //���ϲ�����Ĵ���,�ܴ�������20��,���ߵ����ط���������5��, ����ʧ��
					{
						Sphoto_err++;
						if(Sphoto_err>=3) //����3��,�����˰�
						{
							if(Sphoto_state==sent_sd)	index_ph_dec(ph_index_buf); //δ����������һ
							if((Sphoto_state==sent_sram)&&(PH_sram_buf==bsenting_buf))	PH_sram_buf=bempty;
							
							printf("\r\n///��ʧһ��ͼƬ,�����ϴ�ʧ��");
						}
						else
						{
							if((Sphoto_state==sent_sram)&&(PH_sram_buf==bsenting_buf)) PH_sram_buf=bready;	//��������û�ı�״̬�Ļ�, �������·ŵ�׼����	
							//								if(Sphoto_state==sent_sd)//������
						}
						Contact_state=0;	//ͨѶ����
						Sphoto_step=0;
					}
					else
					{
						printf("\r\n��ʱ�ط�86");
						Sphoto_step--;
//						nw_sent_st(&nw_sc);
//						Sphoto_time=0;
//						Sphoto_cnt++;
						goto __Photo_case7;
					}
				}
			break;
				
		default:
			
			Sphoto_step=0;
		
			break;
				
	}
}

