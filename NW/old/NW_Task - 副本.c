


#define  NW_MODULE
#include "global.h"
#include "nw.h"
#undef NW_MODULE

#include "sram.h"
#include "timer.h"
#include "usart.h"
#include "bsp_uart.h"
#include "FM25V10.h"


unsigned char sampled_step,sampled_data_nub,sampled_data_flag;
void  nw_sent_8(uint8_t *t_data,uint8_t len);
unsigned char WidthxHeight,STM32_rest_delay;
unsigned char bLevel,cLevel,sLevel;
int duty_time;
unsigned char flag_4g_ready=0;

const unsigned char ver[2]={1,0};
uint8_t nw_stete,sent_cnt;
unsigned int LastTxT;
#define testdvi	5
unsigned char ip_seting;


unsigned short pk_nb,tt_pk_nb;
unsigned int tt_len;
#define		nw_pk_len		1024 //1460
#define		nw_pk_max		200000


extern void UART_data_clear(UART_HandleTypeDef *huart);	
char UART_Receive(UART_HandleTypeDef *huart,uint8_t **pack_point, uint16_t *pack_len,int wait_time);
char UART_Receive_s(UART_HandleTypeDef *huart,uint8_t **pack_point, uint16_t *pack_len,int time_out);

uint16_t point3_now;
int get_nw_data(uint8_t **pack_point, uint16_t *pack_len)
{
	uint16_t date_len;
	
	uint8_t *udata;	//��ȡ�������ݵ�ַ
	uint16_t ulen;	//��ȡ�������ݳ���

	if(UART_Receive_s(&huart3,&udata, &ulen,100) != 2 ) return 0;
	
	if(ulen<12)//10
	{
		printf("\r\nm->s data too short err");
		return -1;	
	}
	
__date_rev:	
//	rdata=udata;
	
	if (0 != memcmp(udata, STAA, 7))
	{
		if(ulen>12)
		{
			(ulen)--;
			udata++;
			goto __date_rev;
		}
		else
		{
			printf("\r\nm->s data stat err");
//			point3_last=point3_now; //����һ������,��һ���ж��Ƿ��м�������
			return -1;	
		}
	}
	
	*pack_point=udata;
	*pack_len = date_len+12; //�ȸ������ ���� ���� С�� ����Ҫ��������İ���С
//	for(i=0;i<ulen;i++) printf("%x ",udata[i]);
	
	date_len=(udata[8]<<8)+udata[9]; //���ݳ���
	if(date_len>=(lenmax3-12))
	{
		printf("\r\nm->s data too long err");
		return -1;	
	}
	
	if((checksum_4g(&udata[1],date_len+6+1+2)!=udata[date_len+10]))
	{
		printf("\r\nm->s data checksum err");
		return -1;	
	}
	if(udata[date_len+11]!=0x16)
	{
		printf("\r\nm->s data end err");
		return -1;	
	}
	
	printf("\r\nM->S %x,%d:%d:%d",udata[7],systime[3],systime[4],systime[5]);
	
//	*udata=rdata;		//���ݵ�ַָ��
	
	

//	if(ulen >= date_len+12+12) //��������һ���Ĵ�С
//	{
//		printf("\r\nM->S ��������");
//		if(huart3.GetPoint >= date_len+12)
//		{
//			 huart3.GetPoint -= (date_len+12);
//		}
//		else
//		{
//			huart3.GetPoint =huart3.GetPoint + huart3.RxXferSize - (date_len+12);
//		}
//	}
	
	return 2;	//����OK
}

uint8_t HK_post_buf[300];//
uint16_t HK_post_cnt=0;

void  nw_sent_ph_time(uint8_t ch);
void  nw_sent_ph(unsigned char *input,uint16_t len,uint16_t peket);
	
void NW_REV_Task(void * p_arg)//pro
{
	OS_ERR      err;
//	int state;
	uint8_t *pack_in;
	uint16_t pack_len,buf;
	(void)p_arg;	
	uint8_t cmd;
	uint8_t *data_in;
	uint16_t data_len;
	uint16_t i,j;
	uint8_t *star_add;
	
	printf("1");
	UART_data_clear(&huart3);
	while(1)
	{
		OSTimeDly ( 20, OS_OPT_TIME_DLY, & err );	

		if(get_nw_data(&pack_in,&pack_len)==2) //�յ��������������ݰ�
		{
			cmd=pack_in[7];
			data_len=(pack_in[8]<<8)+pack_in[9];
			data_in=pack_in+10;
			if(HK_post_cnt>1000) HK_post_cnt=0; //

			switch(cmd)
			{
				case 0x81:	//ͼ��ɼ���������
				if(memcmp(PSW,data_in,4)==0)
				{
					star_add=&HK_post_buf[HK_post_cnt];
					
					HK_post_buf[HK_post_cnt++]=cmd;
//					HK_post_buf[HK_post_cnt++]=data_in[4]; //�ڰ�
					HK_post_buf[HK_post_cnt++]=data_in[5]; //��С
					HK_post_buf[HK_post_cnt++]=data_in[6];
					HK_post_buf[HK_post_cnt++]=data_in[7];
					HK_post_buf[HK_post_cnt++]=data_in[8];
					
					OSQPost(&hktask_Asim[0],star_add,5,OS_OPT_POST_FIFO,&err);
					
					HK_post_buf[HK_post_cnt++]=cmd;
//					HK_post_buf[HK_post_cnt++]=data_in[9]; //�ڰ�					
					HK_post_buf[HK_post_cnt++]=data_in[10]; //��С
					HK_post_buf[HK_post_cnt++]=data_in[11];
					HK_post_buf[HK_post_cnt++]=data_in[12];
					HK_post_buf[HK_post_cnt++]=data_in[13];
					
					OSQPost(&hktask_Asim[1],star_add,5,OS_OPT_POST_FIFO,&err);
				}
				else
				{
					rev_err(0x81,0xff);
				}
				break;
				
			case 0x82:	//����ʱ�������
				if((memcmp(PSW,data_in,4)==0)&&(data_in[5]<=255))
				{
//����0-3	ͨ����4	����5					��1��
//											ʱ			��			Ԥ��λ��
//4�ֽ�	1�ֽ�		1�ֽ�		1�ֽ�		1�ֽ�			1�ֽ�		
					buf=data_in[5]*3+1;
					if(data_in[4]==1)
					{
						memcpy(aut_point_group1,&data_in[5],buf);
						flash_save(aut_point_group1,buf);
						nw_sent_8(pack_in,pack_len);
					}
					if(data_in[4]==2)
					{
						memcpy(aut_point_group2,&data_in[5],buf);
						flash_save(aut_point_group2,(data_in[5]*3+1));
						nw_sent_8(pack_in,pack_len);
					}
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
					nw_sent_ph_time(data_in[0]); //ͨ����					
				}
				break;
					
				case 0x83:	//��վ����������Ƭ

					star_add=&HK_post_buf[HK_post_cnt];
					HK_post_buf[HK_post_cnt++]=cmd; //����ָ��
					HK_post_buf[HK_post_cnt++]=data_in[1];	//Ԥ��λ
					if(data_in[0]==1) OSQPost(&hktask_Asim[0],star_add,2,OS_OPT_POST_FIFO,&err);
					if(data_in[0]==2) OSQPost(&hktask_Asim[1],star_add,2,OS_OPT_POST_FIFO,&err);
					nw_sent_8(pack_in,pack_len);//����ԭ�����
					break;
				
				case 0x84:	//�ɼ��ն�����������Ƭ��
//					if(nw_stete==0x84)
					{
//						printf("\r\n׼������Ƭ%d��,�ܳ�%d,",tt_pk_nb,tt_len);
//						
//						for(pk_nb=1;pk_nb<=tt_pk_nb;pk_nb++)
//						{
//							if(pk_nb==tt_pk_nb) nw_sent_ph(PH_DATA+((pk_nb-1)*nw_pk_len),tt_len-((pk_nb-1)*nw_pk_len),pk_nb);
//							else	nw_sent_ph(PH_DATA+((pk_nb-1)*nw_pk_len),nw_pk_len,pk_nb);
////							OSTimeDly ( 3, OS_OPT_TIME_DLY, & err ); 
//						}
						senttask_Pend &= (~data_flag84_1);
//						sent_cnt=0;
//						nw_stete=0x86;	//����,׼���� 0x86ָ�������Ӧ��
					}
					break;
					
//					case 0x85:	//ͼ���������ͣ�ֻ�з�
//					break;
					
					case 0x86:	//ͼ���������ͽ������,ֻ�з�
//						senttask_Pend &= (~data_flag86_1);
					break;
					
					case 0x87:	//���������·�
//						senttask_Pend &= (~data_flag86_1); //�յ�87��
					if(senttask_Pend & data_flag86_1)
					{
						if(data_in[2]==0)	//���貹��
						{
							senttask_Pend &= (~data_flag86_1); 	//���ݰ��ɹ�
							printf("\r\nsend photo OK");
						}
						else //��Ҫ����
						{
							printf("\r\nresend photo ");
							for(i=0;i<data_in[2];i++) //data_in[2] �貹��������
							{								
								pk_nb=(data_in[2*i+3]<<8)+data_in[2*i+4]; //������ ��ֵ
								if(pk_nb==tt_pk_nb) nw_sent_ph(PH_DATA+((pk_nb-1)*nw_pk_len),tt_len-((pk_nb-1)*nw_pk_len),pk_nb);
								else	nw_sent_ph(PH_DATA+((pk_nb-1)*nw_pk_len),nw_pk_len,pk_nb);
								printf("%d ",pk_nb);
							}
						}
					}
					break;
			
			
				case 0x88:	//�����Զ�̵���
				if(memcmp(PSW,data_in,4)==0) //localpassw[50]
				{			
//����	ͨ����	����ָ��	ָ�����
//4�ֽ�	1�ֽ�	1�ֽ�	1�ֽ�
					star_add=&HK_post_buf[HK_post_cnt];
					HK_post_buf[HK_post_cnt++]=data_in[5]; //����ָ��
					HK_post_buf[HK_post_cnt++]=data_in[6];	//ָ�����
					if(data_in[4]==1) OSQPost(&hktask_Asim[0],star_add,2,OS_OPT_POST_FIFO,&err);
					if(data_in[4]==2) OSQPost(&hktask_Asim[1],star_add,2,OS_OPT_POST_FIFO,&err);
//				nw_sent_8(pack_in,pack_len);//����ԭ�����						
				}
				else
				{
					rev_err(0x88,0xff);
				}
				break;
				
				case  request_la_qj://22	//	�ϴ��������������������	
					if(data_len==0)//��վ��������
					{
						senttask_Asim |= data_flag22;	//
						
//						printf("\r\n��վ����22����");
//						sampled_step=0; 
//						sampled_data_sent_22(sampled_step,sampled_data_nub);
//						SenTimes=2;
					}
					else if(data_len==3)	//������
					{
//						�ڼ�֡��ʶ 1�ֽ�	AA55H
						if((data_in[1]==0xAA)&&(data_in[2]==0x55))
						{
//							printf("\r\n�յ���վ��22����,��ʶ��Ϊ%x",data_in[0]);
							if(data_in[0]==sampled_data_nub)
							{
								senttask_Pend &= (~data_flag22);
							}
							else
							{
//								printf("\r\n���ݱ�ʶ�Ŵ���,��ʶ��ӦΪ%x",sampled_data_nub);
							}
						}
					}
					break;
				case  request_qx:		//0x25	//	�ϴ���������	
					if(data_len==0)//��վ��������
					{
						senttask_Asim |= data_flag25;
					}
					else if(data_len==3)
					{
//						�ڼ�֡��ʶ 1�ֽ�	AA55H
						if((data_in[1]==0xAA)&&(data_in[2]==0x55))
						{
							printf("\r\n�յ���վ��25����,��ʶ��Ϊ%x",data_in[0]);
							if(data_in[0]==sampled_data_nub)
							{
								senttask_Pend &= (~data_flag25);	//�յ���
							}
							else
							{
								printf("\r\n���ݱ�ʶ�Ŵ���,��ʶ��ӦΪ%x",sampled_data_nub);
							}
						}
					}
					break;
					
				case 	Contact:			//00	//	����������Ϣ	װ�ÿ�������������Ϣ
								
				if(senttask_Pend & data_flag00)	//���ȴ��� 
				{
					if((data_in[0]==ver[0])&&(data_in[1]==ver[1]))
					{
						senttask_Pend &= ~data_flag00;
						
					}
				}
				else
					senttask_Asim |= data_flag00;
				
					break;
					
				case  CheckT:				//01	//	Уʱ	�������������·�������
					if(senttask_Pend & data_flag01) //����Уʱ
					{
						if((worktime-duty_time)<20)
						{
							senttask_Pend &= (~data_flag01);
							memcpy(systime,data_in,6);
						}
					}
					else
					{
						memcpy(systime,data_in,6);
						nw_sent_8(pack_in,pack_len);
//						senttask_Asim |= data_flag01;
					}									
					break;
					
				case  SetPassword :	//02	//	����װ������	װ�ó������룺�ַ�����1234����31H32H33H34H��
					printf("\r\nPSW=%x,%x,%x,%x",PSW[0],PSW[1],PSW[2],PSW[3]);
					if(memcmp(PSW,data_in,4)==0) //localpassw[50]
					{
						memcpy(PSW,&data_in[4],4);
						flash_save(PSW,4);
						nw_sent_8(pack_in,pack_len);
					}
					else
					{
						rev_err(SetPassword,0xff);
					}
					printf("\r\ndata_in=%x,%x,%x,%x",data_in[0],data_in[1],data_in[2],data_in[3]);
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
					printf("\r\n��������%x,%x,%x,%x",data_in[0],data_in[1],data_in[2],data_in[3]);
				
					printf("\r\ndata_in=%x,%x,%x,%x",data_in[0],data_in[1],data_in[2],data_in[3]);
				
					if((memcmp(PSW,data_in,4)==0)&&(memcmp(CHW,&data_in[14],4)==0)) //localpassw[50]
					{
						
						*HBT=data_in[4];
						*GDT=(data_in[5]<<8)+data_in[6];
						*SLT=(data_in[7]<<8)+data_in[8];
						*OLT=(data_in[9]<<8)+data_in[10];
						RST[0]=data_in[11]; //�� 0-28
						RST[1]=data_in[12];	//ʱ 0-24
						RST[2]=data_in[13];	//�� 0-60
						
						flash_save((uint8_t *)HBT,11);
						
						nw_sent_8(pack_in,pack_len);
						
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
					senttask_Pend &= (~data_flag05);
				
//					if(senttask_Pend & data_flag05)
//					{
//						senttask_Pend &= data_flag05;
//					}
//					else
//					{
//						senttask_Asim |= data_flag05;
//					}	
					break;
				case  chang_ip:			//06	//	������վIP��ַ���˿ںźͿ���	
					if(memcmp(PSW,data_in,4)==0)
					{
//����	��վIP	�˿ں�	��վIP	�˿ں�	��վ����	��վ����
//4�ֽ�	4�ֽ�	2�ֽ�	4�ֽ�	2�ֽ�	6�ֽ�	6�ֽ�
						if(memcmp(&data_in[4],&data_in[10],6)==0)
						{
							HostIP[0]=data_in[4];
							HostIP[1]=data_in[5];
							HostIP[2]=data_in[6];
							HostIP[3]=data_in[7];
							*HostPort=((data_in[8]<<8)+data_in[9]);
							
							flash_save(HostIP,6);
							
							nw_sent_8(pack_in,pack_len);
							senttask_Asim |= data_flag06; 	//��λ4g����
	//						STM32_rest_delay=5;
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
//				if(data_len==0)
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
					if(memcmp(PSW,data_in,4)==0)
					{
						nw_sent_8(pack_in,pack_len);
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
				
//					senttask_Asim |= data_flag0a;
				
//				if(data_len==0)
					{
						
						nw_sc.cmd=check_device_cfg;
							
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

						{
//							if(GET_Streaming_s()==0)	//ͼ��1ͼ���С
							{
								nw_sc.data[i++]=WidthxHeight;
//								Streaming_OK=1;
							}		
//							if(GET_Image_s()==0) //ͼ��1����,�Աȶ�,���Ͷ�
							{
								nw_sc.data[i++]=bLevel;nw_sc.data[i++]=cLevel;nw_sc.data[i++]=sLevel;
//								if(Streaming_OK==1) Streaming_OK=2;
							}
						}
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
					if(memcmp(PSW,data_in,4)==0)
					{
						function_nub=data_len-4;
						memcpy(function_buf,&data_in[4],function_nub);
						function_buf[data_len-4]=0; //��β��0
						
						nw_sent_8(pack_in,pack_len);
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
//					nw_sent(nw_r_post);
//				������	https://blog.csdn.net/qq_30460905/article/details/81805891
					break;
				case  request_data:	//21	//	��վ����װ������	��վ����ɼ�װ�òɼ����ݲ���������
					if(data_len==0)//�ϴ�δ�ɹ��ϴ�����ʷ���ݣ�������ʷ��Ƭ����װ������ʷ�������ϴ�
					{
//						sampled_data_time=1;
					}
					else if((data_len==2)&&(data_in[0]==0xBB)&&(data_in[1]==0xBB))//װ�����̲ɼ��������ݣ�ͼƬ���⣩����ɲɼ��������ϴ����ôβ�����Ӱ��ԭ�趨�ɼ������ִ��
					{
//						sampled_data_time=1;
					}
//					nw_sent(nw_r_post);
					break;

					
					
			}
		}
	}
}




//void check_retxtime()
//{
//	switch(ctlnumber)
//	{
//		case 	Contact:retxtime=60/testdvi;	break;	//�������磬30s�ط�
//		case 	CheckT:retxtime=120/testdvi;	break;	//Уʱ��2�����ط� 	//�������磬30s�ط�
//		case 	HeartBeat:retxtime=*HBT*60/testdvi;	break;//	װ��������Ϣ	������վ���װ������ʱ�䡢IP��ַ�Ͷ˿ںš������ź�ǿ�ȼ����ص�ѹ
//		case 	0x84:retxtime=2;	break;//
//		case 	0x86:retxtime=2;	break;//	װ��������Ϣ	������վ���װ������ʱ�䡢IP��ַ�Ͷ˿ںš������ź�ǿ�ȼ����ص�ѹ
//		case 	0x22:retxtime=2;	break;//
//		case 	0x25:retxtime=2;	break;//
//	}
//}
char check_signel(void);
int Get_Volt(void);
void sampled_data_sent_22(unsigned char step,unsigned char zbs);
void sampled_data_sent_25(unsigned char zbs);

char check_data_get_cmd()
{
	char check_state=0;
	if(senttask_Asim==0) return 0;
	
	
	if(senttask_Asim & data_flag00)	//���󿪻�����
	{
		senttask_Asim &= (~data_flag00);
	}
	else	if(senttask_Asim & data_flag01)	//����Уʱ?
	{
		senttask_Asim &= (~data_flag01);
	}
	else	if(senttask_Asim & data_flag05)	//��������?
	{
		senttask_Asim &= (~data_flag05);
	}
	else	if(senttask_Asim & data_flag06)	//����IP��ַ,�˿ں�,�Ϳ���--�ر�4G,�ٿ�,ʮ�������³�ʼ��
	{
		senttask_Asim &= (~data_flag06);
	}
	else	if(senttask_Asim & data_flag0a)	//��ѯװ�ò���, װ�ò������ϵ���������.
	{
		senttask_Asim &= (~data_flag0a);
	}
	else	if(senttask_Asim & data_flag22)	//�ϴ��ɼ���������
	{
		sent_cnt=0;
		check_state=0x22;
		senttask_Asim &= (~data_flag22);
	}
	else	if(senttask_Asim & data_flag25)	//�ϴ���������
	{
		sent_cnt=0;
		check_state=0x25;
		senttask_Asim &= (~data_flag25);
	}
	else	if(senttask_Asim & data_flag84_1)	//
	{
		sent_cnt=0;
		check_state=0x84;
		senttask_Asim &= (~data_flag84_1);
	}
	else	if(senttask_Asim & data_flag84_2)	//
	{
		sent_cnt=0;
		check_state=0x84;
		senttask_Asim &= (~data_flag84_2);
	}
//	else	if(senttask_Asim & data_flag86_1)	//
//	{
//		sent_cnt=0;
//		check_state=0x86;
//		senttask_Asim &= (~data_flag86_1);
//	}
//	else	if(senttask_Asim & data_flag86_2)	//
//	{
//		sent_cnt=0;
//		check_state=0x86;
//		senttask_Asim &= (~data_flag86_2);
//	}
	
	return check_state;
}

int LastGetDataT;



void NW_timer_work(void)
{
	unsigned char i;
	uint8_t *star_add;
	
	OS_ERR      err;
	
	if(STM32_rest_delay)
	{
		STM32_rest_delay--;
		if(STM32_rest_delay==0) reset_STM32();
	}
	
	if((worktime-LastGetDataT)>=(GDT[0]*60/testdvi))
	{
		senttask_Asim |= data_flag22;
		senttask_Asim |= data_flag25;
	}
//	if(ip_seting)
//	{
//		sprintf((char *)buf,"AT+HOST=%d.%d.%d.%d,%d\r\n",HostIP[0],HostIP[1],HostIP[2],HostIP[3],*HostPort);//HostIP[0],HostIP[1],HostIP[2],HostIP[3],*HostPort
//		HAL_UART_Transmit_IT(&huart5, buf, strlen((char *)buf));
//	}
	
	if(systime[5]==0) //������
	{
//		printf("\r\nһ����");
		if(((RST[0]==0)||(systime[2]==RST[0]))&&(systime[3]==RST[1])&&(systime[4]==RST[2])) //�� ʱ ��
		{
				reset_STM32();//�����λ
		}
		for(i=0;i<aut_point_group1[0];i++)
		{
			if((systime[3]==(aut_get_point1[i].h))&&(systime[4]==(aut_get_point1[i].m+0)))
			{
				printf("\r\n����ʱ�� ǰ�˵�:%d",aut_get_point1[i].p);

				star_add=&HK_post_buf[HK_post_cnt];
				HK_post_buf[HK_post_cnt++]=0x83; //����ָ��
				HK_post_buf[HK_post_cnt++]=aut_get_point1[i].p;	//Ԥ��λ
				star_add=&HK_post_buf[HK_post_cnt];
				OSQPost(&hktask_Asim[0],star_add,2,OS_OPT_POST_FIFO,&err);
			}
		}
		for(i=0;i<aut_point_group2[0];i++) //ͨ��2��ʱ����
		{
			if((systime[3]==(aut_get_point2[i].h))&&(systime[4]==(aut_get_point2[i].m+0)))
			{
				printf("\r\n����ʱ�� ǰ�˵�:%d",aut_get_point2[i].p);

				star_add=&HK_post_buf[HK_post_cnt];
				HK_post_buf[HK_post_cnt++]=0x83; //����ָ��
				HK_post_buf[HK_post_cnt++]=aut_get_point2[i].p;	//Ԥ��λ
				star_add=&HK_post_buf[HK_post_cnt];
				OSQPost(&hktask_Asim[1],star_add,2,OS_OPT_POST_FIFO,&err);
			}
		}
	}
}

char nw_step=0,nw_stete_old;

void  NW_SENT_Task  ( void * p_arg )//void  NWTask_task( void * p_arg )
{
	OS_ERR      err;
	
	(void)p_arg;	
	char buf;
	
		
	retxtime=0xff;
	function_buf[0]=0x22;
	function_buf[1]=0x25;
	function_nub=2;
	
	LastTxT=worktime;
	nw_stete=0;
	nw_step=0;
	sent_cnt=0;
	duty_time=0;
		
	while (DEF_TRUE)
	{	
		OSTimeDly( 100, OS_OPT_TIME_DLY, & err);
		if(nw_stete_old != nw_stete)
		{
			nw_stete_old=nw_stete;
			nw_step=0;
			sent_cnt=0;
		}
		switch(nw_stete)
		{
			case 0:printf("\r\n�ϵ�");nw_stete++;break;
			case 1:
				OSTimeDly( 1000, OS_OPT_TIME_DLY, & err);
				printf(".");
				if(++nw_step>=3)
				{
					nw_stete++;
				}
				break;
			case 2:		//��������
				OSTimeDly( 1000, OS_OPT_TIME_DLY, & err);
				printf("%d",worktime-duty_time);
				if(((worktime-duty_time)>=60/testdvi)||(sent_cnt==0))
				{
					senttask_Pend |= data_flag00;	//�ȴ� data_flag00 ��
					
					nw_sc.cmd=Contact;
					nw_sc.len=2;
					nw_sc.data[0]=ver[0];
					nw_sc.data[1]=ver[1];
					nw_sent(&nw_sc);
					sent_cnt++;
					duty_time=worktime;
				}
				if((sent_cnt)&&((senttask_Pend & data_flag00)==0))
				{
					sent_cnt=0;
					nw_stete++;
					printf("\r\n��������OK");
				}
				break;
				
			case 3:		//Уʱ
				OSTimeDly( 1000, OS_OPT_TIME_DLY, & err);
				printf("%d",worktime-duty_time);
			
				if(((worktime-duty_time)>=120/testdvi)||(sent_cnt==0))
				{
					senttask_Pend |= data_flag01;	//�ȴ� data_flag01 ��
					
					nw_sc.cmd=CheckT;
					nw_sc.len=0;
					memcpy(nw_sc.data,systime,6);
					nw_sent(&nw_sc);
					sent_cnt++;
					duty_time=worktime;
					LastTxT=worktime;
				}
				if((sent_cnt)&&((senttask_Pend & data_flag01)==0))
				{
					sent_cnt=0;
					nw_stete=5;
					printf("\r\nУʱOK");
				}
				break;
				
			case 5://����
				OSTimeDly( 1000, OS_OPT_TIME_DLY, & err);
				printf("%d",worktime-duty_time);
			
				buf=check_data_get_cmd();
				if(buf>5) nw_stete=buf;
				else if((worktime-LastTxT)>=*HBT*60/testdvi)
				{
					nw_sc.cmd=HeartBeat;
					nw_sc.len=8;
					memcpy(nw_sc.data,systime,6);
					
					if(check_signel()==0) nw_sc.data[6]=signal;
					else nw_sc.data[6]=0;
				
					if(Get_Volt()==0) nw_sc.data[7]=volt;
					else nw_sc.data[7]=0;

					nw_sent(&nw_sc);
					LastTxT=worktime;
				}
				break;
			
			case 0x22:
		
				sampled_data_nub++;
				
				sampled_data_sent_22(sampled_step,sampled_data_nub);
				
					nw_sc.cmd=HeartBeat;
					nw_sc.len=8;
					memcpy(nw_sc.data,systime,6);
					
					if(check_signel()==0) nw_sc.data[6]=signal;
					else nw_sc.data[6]=0;
				
					if(Get_Volt()==0) nw_sc.data[7]=volt;
					else nw_sc.data[7]=0;

					nw_sent(&nw_sc);
				LastTxT=worktime;
			break;
		
		case 0x25:
		
			sampled_data_nub++;
			
			sampled_data_sent_25(sampled_data_nub);
			if((worktime-LastTxT)>=*HBT*60/testdvi)
			{
				nw_sc.cmd=HeartBeat;
				nw_sc.len=8;
				memcpy(nw_sc.data,systime,6);
				
				if(check_signel()==0) nw_sc.data[6]=signal;
				else nw_sc.data[6]=0;
			
				if(Get_Volt()==0) nw_sc.data[7]=volt;
				else nw_sc.data[7]=0;

				nw_sent(&nw_sc);
				LastTxT=worktime;
			}
		break;
			
		case 0x81: //����ͼƬ����ԭ�����
		{
			
		}
		break;
		
		case 0x84: //����������Ƭ 1s�ط�,Ҫ��3s,���5��
		{			
			if(sent_cnt==0)
			{
				nw_sc.cmd=0x84;
				nw_sc.len=10;		
				memcpy(nw_sc.data,PH_GET_T,8); //time + ch + preset
				tt_len=((PH_LEN[0])<<16) +PH_LEN[1];
				if(tt_len>nw_pk_max) tt_len=nw_pk_max;
				tt_pk_nb=(tt_len/nw_pk_len); //1024һ��
				if(tt_len%nw_pk_len) tt_pk_nb +=1;
				nw_sc.data[8]=(tt_pk_nb>>8);//������λ
				nw_sc.data[9]=tt_pk_nb;			//������λ
				
				nw_sent(&nw_sc);
				senttask_Pend |= data_flag84_1;
				sent_cnt++;
			}
			else	if((senttask_Pend & data_flag84_1)==0)	//�յ���
			{
					sent_cnt=0;
					nw_stete=0x85;	
			}
			else if((worktime-LastTxT)>=3)
			{
				if(sent_cnt>=5) nw_stete=5;
				else
				{
					nw_sent(&nw_sc);
					sent_cnt++;
				}
			}
		}
		break;
		case 0x85:
		{
			printf("\r\nsend photo start,pk_nb=%d,tt_len=%d,",tt_pk_nb,tt_len);
			for(pk_nb=1;pk_nb<=tt_pk_nb;pk_nb++)
			{
				if(pk_nb==tt_pk_nb) nw_sent_ph(PH_DATA+((pk_nb-1)*nw_pk_len),tt_len-((pk_nb-1)*nw_pk_len),pk_nb);
				else	nw_sent_ph(PH_DATA+((pk_nb-1)*nw_pk_len),nw_pk_len,pk_nb);
//							OSTimeDly ( 3, OS_OPT_TIME_DLY, & err ); 
			}
			printf("\r\nsend photo complete");
			
			OSTimeDly ( 2000, OS_OPT_TIME_DLY, & err ); 
			sent_cnt=0;
			nw_stete=0x86;	//����,׼���� 0x86ָ�������Ӧ��
		}
		break;
		case 0x86:
		{
			if(sent_cnt==0)
			{
				nw_sc.cmd=0x86;//���ݽ���
				nw_sc.len=2;			
				nw_sc.data[0]=1;//ͨ����	
				nw_sc.data[1]=0;//Ԥ��λ��
				nw_sent(&nw_sc);
				senttask_Pend |= data_flag86_1;
				sent_cnt++;
			}
			else	if((senttask_Pend & data_flag86_1)==0)
			{
				nw_stete=5;
			}
			else if((worktime-LastTxT)>=30/testdvi)
			{
				nw_sent(&nw_sc);
			}
		}
		break;
			
		}


		
			
//		else if(nw_stete==25); 
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


