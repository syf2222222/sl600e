

#include  <os.h>
#include "global.h"

#define  NW_MODULE
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
uint32_t point3_now;

int get_uart3_data(uint8_t **pack_point, uint16_t *pack_len)
{
	uint32_t i,len;
	uint8_t * rdata;
	
	
	
	if(point3_last == point3) return 0; //û����

	printf("\r\npoint3_last=%d",point3_last);
	printf("\r\npoint3=%d",point3);
//	OSTimeDly (10, OS_OPT_TIME_DLY, & err ); 
	if(point3_now != point3)	
	{
		point3_now=point3;
		printf("\r\nû����һ��");
		return 1;	//û����һ��
	}
///////point3_now == point3 ˵�����ս���////////////////////////
		
	if(point3_now>point3_last)
	{
		len=point3_now-point3_last;
	}
	else
	{
		for(i=0;i<point3_now;i++)
		{
			urd3[lenmax3+i]=urd3[i];
		}
		len=point3_now+lenmax3-point3_last;
	}
	if(len<=10)
	{
		printf("\r\nm->s data too short err");
		point3_last=point3_now;
		return -1;	
	}
	
__date_rev:	//�˴�lenΪ�յ����ݵ��ܳ���
	rdata=&urd3[point3_last];
	
	if (0 != memcmp(rdata, STAA, 7))
	{
		if((point3_last<len)&&(len>10))
		{
			len--;
			point3_last++;
			goto __date_rev;
		}
		else
		{
			printf("\r\nm->s data stat err");
//			point3_last=point3_now; //����һ������,��һ���ж��Ƿ��м�������
			return -1;	
		}
	}
	
	//�˴�lenΪ�յ�����Э����������򳤶�
	for(i=0;i<len;i++) printf("%x ",rdata[i]);
	
	len=(rdata[8]<<8)+rdata[9]; //���ݳ���
	if(len>=(lenmax3-12))
	{
		printf("\r\nm->s data too long err");
		point3_last=point3_now;
		return -1;	
	}
	
	if((checksum_4g(&rdata[1],len+6+1+2)!=rdata[len+10]))
	{
		printf("\r\nm->s data checksum err");
		point3_last=point3_now;
		return -1;	
	}
	if(rdata[len+11]!=0x16)
	{
		printf("\r\nm->s data end err");
		point3_last=point3_now;
		return -1;	
	}
	
	printf("\r\nM->S %x,%d:%d:%d",rdata[7],systime[3],systime[4],systime[5]);
	
	*pack_point=rdata;		//���ݵ�ַָ��
	*pack_len=len+12;			//���ݳ���(������cmd��len)
			
	point3_last += len+12;
	if(point3_last>=lenmax3) point3_last -=lenmax3;

	
	return 2;	//����OK
}

void NW_REV_Task(void * p_arg)//pro
{
	OS_ERR      err;
//	int state;
	uint8_t *pack_in;
	uint16_t pack_len;
	(void)p_arg;	
	
	uint8_t cmd;
	uint8_t *data_in;
	uint16_t data_len;
	uint16_t i,j;
	
	printf("1");
	HAL_UART_Receive_IT(&huart3,(unsigned char *)urd3,1);
	while(1)
	{
		OSTimeDly ( 20, OS_OPT_TIME_DLY, & err );	

		if(get_uart3_data(&pack_in,&pack_len)==2) //�յ��������������ݰ�
		{
			cmd=pack_in[7];
			data_len=(pack_in[8]<<8)+pack_in[9];
			data_in=pack_in+10;

			switch(cmd)
			{
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
						printf("\r\n��������OK");
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
int check_signel(void);
int Get_Volt(void);
void sampled_data_sent_22(unsigned char step,unsigned char zbs);
void sampled_data_sent_25(unsigned char zbs);

void check_data_get_cmd()
{
	if(senttask_Asim==0) return;
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
		nw_stete=22;
	}
	else	if(senttask_Asim & data_flag25)	//�ϴ���������
	{
		sent_cnt=0;
		nw_stete=25;
	}
}


void  NW_SENT_Task  ( void * p_arg )//void  NWTask_task( void * p_arg )
{
	OS_ERR      err;
//	CPU_SR_ALLOC();
//	unsigned char mode=0;
//	struct nw_ctl *nw_r_post;
//	OS_MSG_SIZE*	 size;	
//	char i;
	
	(void)p_arg;	
		
	retxtime=0xff;
	function_buf[0]=0x22;
	function_buf[1]=0x25;
	function_nub=2;
	
	LastTxT=worktime;
	nw_stete=0;
	sent_cnt=0;
	duty_time=0;
	
	while (DEF_TRUE)
	{			
		OSTimeDly( 100, OS_OPT_TIME_DLY, & err);
		
		if(nw_stete==5)
		{
			check_data_get_cmd();
		}
		if(nw_stete==5)
		{
//			check_photo_ctr_cmd();
		}
		
		if((nw_stete==5)&&(senttask_Asim))
		{
			
		}
		
		if(nw_stete==0)
		{
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
				nw_stete=1;
			}
		}
		else if(nw_stete==1) //����Уʱ
		{
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
			}
		}
		else if(nw_stete==5)
		{
			if((worktime-LastTxT)>=*HBT/testdvi)
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
		}
		else if(nw_stete==22)
		{
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
		}
		else if(nw_stete==25)
		{
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
		}

		else if(nw_stete==25)
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
void NW_HK_Task_Create (void);

OS_SEM read_RSSI;

void NWTaskStart (void *p_arg)
{
	OS_ERR      err;
	(void)p_arg;

	
	Ur5TaskStart (0);

//		��ֵ�ź������ƿ�ָ��		//��ֵ�ź������� //��Դ��Ŀ���¼��Ƿ�����־ //���ش�������
	OSSemCreate (&read_RSSI, "read_RSSI",  0, &err); //3 				

	NW_HK_Task_Create();
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


