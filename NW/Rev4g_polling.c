


#include "global.h"
#include "nw.h"


#include "sram.h"
#include "sys_user.h"
#include "usart.h"
#include "bsp_uart.h"
#include "FM25V10.h"
//#include "power_ctrl.h"

void nw_sent_st(struct nw_ctl *t_data);
void rev_err(unsigned char ctl,unsigned char data);

extern unsigned char sampled_step,sampled_data_nub,sampled_data_flag;
extern unsigned char STM32_rest_delay;
extern const unsigned char ver[2];
extern unsigned char ip_seting;

int read_ph(unsigned char *in,unsigned char *out,int len);
int int_time_to_char_time(uint8_t *out,uint32_t sec);
extern unsigned int runningtime;
extern int hk_tast_step;
extern uint8_t STM32_rest_wait,HK_Task_free;
unsigned char checksum_4g(unsigned char* a,unsigned int n);
extern void UART_data_clear(UART_HandleTypeDef *huart);	


void resent_ph(uint8_t *in,uint16_t len);

void  nw_sent_8(uint8_t *t_data,uint16_t len);
void  nw_sent_ph_time(uint8_t ch);

int gain_qx_data(void);
int get_a_data_ll(void);	//1sһ��
int gain_ll_data(void);
void sys_time_chang(void);
extern u8 DTU_c_state;
extern uint16_t Contact_time;
extern uint8_t wr_sleep;

extern u8 Contact_state;

int get_nw_data(uint8_t **pack_point, uint16_t *pack_len)
{
//	OS_ERR      err;
	uint16_t date_len;
	uint8_t *udata;	//��ȡ�������ݵ�ַ
	uint16_t ulen,i;	//��ȡ�������ݳ���

////	printf("\r\nCR1=%04x",huart3.Instance->CR1);	
//	 /* Clear the Error flags in the ICR register */
//    __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF | UART_CLEAR_NEF | UART_CLEAR_PEF | UART_CLEAR_FEF);
	
//	SET_BIT(huart->Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE);
	
#ifdef	ur3_rdma
	if(huart3.GetPoint < huart3.NowPoint)
	{
		ulen =huart3.NowPoint-huart3.GetPoint;	//lenmax3 - huart->ReadPoint; //�ܼ�����ȥδ��������ݸ������õ��Ѿ����յ����ݸ���
		udata=urd3+huart3.GetPoint;
		huart3.GetPoint= huart3.NowPoint;
		printf("\\");
//		for(i=0;i<ulen;i++) printf("%02x ",udata[i]);
	}
	else
#endif

	if(UART_Receive_s(&huart3,&udata, &ulen,100) != 2 ) 
	{
		return 0;
	}
//	printf("\r\nterrorflags=%x",terrorflags);
//	printf("\r\nur3");
	
	if(ulen<12)//10
	{
		if(DTU_c_state)	printf("\r\ns->m data too short err"); //���ϵ��Ǵβ���
//		printf("GetPoint=%x,NowPoint=%x,ReadPoint=%x,",huart3.GetPoint,huart3.NowPoint,huart3.ReadPoint);			
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
			printf("\r\ns->m data stat err");
//			point3_last=point3_now; //����һ������,��һ���ж��Ƿ��м�������
			return -1;	
		}
	}

//	for(i=0;i<ulen;i++) printf("%x ",udata[i]);
	
	date_len=(udata[8]<<8)+udata[9]; //���ݳ���
		
	*pack_point=udata;
	*pack_len = date_len+12; //�ȸ������ ���� ���� С�� ����Ҫ��������İ���С
		
	if(*pack_len>=lenmax3)
	{
		printf("\r\ns->m package length err:");
		return -1;	
	}
	
	if(*pack_len > ulen)
	{
		printf("\r\ns->m data length err:");
		return -1;	
	}
	
	if(udata[date_len+11]!=0x16)
	{
		printf("\r\ns->m data end err");
		return -1;	
	}
	
	if((checksum_4g(&udata[1],date_len+6+1+2)!=udata[date_len+10]))
	{
//		HAL_UART_Receive_IT(&huart3,(unsigned char *)urd3,lenmax3);
//		huart3.GetPoint=0;//huart->NowPoint;
//		huart3.ReadPoint=0;//=huart->NowPoint;
//		huart3.NowPoint=0;
		printf("\r\ns->m data checksum err");
//		for(i=0;i<ulen;i++) printf("%02x ",udata[i]);
//		bAAAAA=1;
		return -1;	
	}
		
	printf("\r\nS->M %x,%d:%d:%d",udata[7],systime[3],systime[4],systime[5]);
	
	
	if(ulen>=*pack_len+12) //��������һ���Ĵ�С
	{
//		printf("\r\nS->M ��������");
//		printf("\r\nhuart3.GetPoint=%d ",huart3.GetPoint);
		if(huart3.GetPoint<(*pack_len))
		{
			huart3.GetPoint=huart3.GetPoint+lenmax3-(ulen-(*pack_len));
		}
		else huart3.GetPoint -= (ulen-(*pack_len)); //�˸� ʣ�����
		
//		printf("\r\nhuart3.GetPoint=%d ",huart3.GetPoint);
		
		huart3.ReadPoint=huart3.GetPoint;
//		printf("\r\n");
//		for(i=0;i<ulen;i++) printf("%02x ",udata[i]);
//		printf("\r\n%02x ",urd3[huart3.GetPoint]);
	}
	
//	*udata=rdata;		//���ݵ�ַָ��
	
	

//	if(ulen >= date_len+12+12) //��������һ���Ĵ�С
//	{
//		printf("\r\nS->M ��������");
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
extern uint8_t sleepdelay;
extern uint8_t function22_flag,function25_flag,function84_flag;

#define FLASH_BANK1_BASE	0x08000000
#define FLASH_BANK2_BASE	0x08080000
#define Boot_ADD			FLASH_BANK1_BASE

void (*jump2app)();
int iap_load_app(uint32_t appxaddr)
{
	if((((*(volatile uint32_t*)appxaddr)&0x2FFE0000)==0x20000000)||(((*(volatile uint32_t*)appxaddr)&0x1FFE0000)==0x10000000))	//���ջ����ַ�Ƿ�Ϸ�.
	{
		printf("\r\nת��%08x",appxaddr);
		jump2app=(void(*)())*(volatile uint32_t*)(appxaddr+4);		//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		
		__set_MSP(*(volatile uint32_t*) appxaddr);//��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
		
		for(int i = 0; i < 8; i++)
		{			
			NVIC->ICER[i] = 0xFFFFFFFF;	/* �ر��ж�*/
			NVIC->ICPR[i] = 0xFFFFFFFF;	/* ����жϱ�־λ */
		}
		jump2app();									//��ת��APP.
		
		printf("\r\nת��%08xû�ɹ�?",appxaddr);
	}
	else
	{
		printf("\r\niapջ����ַ����");
		return -1;
	}
	return 0;
}

void Rev4g_polling(void * p_arg)
{
	OS_ERR      err;
//	int state;
	uint8_t *pack_in;
	uint16_t pack_len,buf_len;
	(void)p_arg;	
	uint8_t cmd;
	uint8_t *data_in;
	uint16_t data_len;
	uint16_t i,j;
	uint8_t *star_add;
	int ur_err;
	
//	printf("1");
//	UART_data_clear(&huart3);
//	SRAM_Test();
	
//	while(1)
	{
//		OSTimeDly ( 20, OS_OPT_TIME_DLY, & err );	
//		if(b1s)
//		{
//			b1s=0;
//			printf("\r\n1s��");
//		}
		
		ur_err=get_nw_data(&pack_in,&pack_len);
//#ifndef	ur3_rdma
//		if(ur_err==-1)
//		{
//			if((((huart3.Instance->CR1)&(USART_CR1_PEIE | USART_CR1_RXNEIE)) != (USART_CR1_PEIE | USART_CR1_RXNEIE))||(terrorflags))
//			{
//				printf("\r\n------------------------terrorflags=%x",terrorflags);
//				terrorflags=0;
//		//		printf("\r\nCR1=%04x",huart3.Instance->CR1);
//				
//				HAL_UART_Abort_IT(&huart3);
//				HAL_UART_Receive_IT(&huart3,(unsigned char *)urd3,lenmax3);
//		//		printf("\r\nuart3��������");
//			}
//			
////			HAL_UART_Abort_IT(&huart3);
////			HAL_UART_Receive_IT(&huart3,(unsigned char *)urd3,lenmax3);
////			printf("\r\nuart3����");
//		}
//#endif
		if(ur_err==2) //�յ��������������ݰ�
		{		
//			STM32_sleeping=0;
			cmd=pack_in[7];
			data_len=(pack_in[8]<<8)+pack_in[9];
			data_in=pack_in+10;
			if(sleepdelay<6)	sleepdelay=6;
			if(HK_post_cnt>HK_post_max) HK_post_cnt=0; //

			switch(cmd)
			{
				case 	updata_online:			//��������
								
				if(memcmp(CHW,&data_in[1],4)==0)	//������ȷ
				{
					if(data_in[0]==1)
					{
						nw_sent_8(pack_in,pack_len);
						updata_flag[0]=0x55;
						
//						__set_CONTROL(0);					
//						iap_load_app(Boot_ADD);
						HAL_NVIC_SystemReset();
					}
				}
				else
				{
					rev_err(updata_online,0xff); //�������
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
//					senttask_Asim |= data_flag00;
				
					break;
					
				case  CheckT:				//01	//	Уʱ	�������������·�������
					if((data_in[0]<y_min)||(data_in[0]>y_max)||(data_in[1]==0)||(data_in[1]>12)||(data_in[2]==0)||(data_in[2]>31))
					{
						printf("\r\nУʱʱ�䳬������ʱ��");
					}
					else
					{
						if(senttask_Pend & data_flag01) //����Уʱ
						{
							if((Contact_time)<200)
							{
								senttask_Pend &= (~data_flag01);
								if(data_in[0])
								{
									memcpy(systime,data_in,6);
									sys_time_chang();
									nw_sent_8(pack_in,pack_len);//??��Ҫԭ�������?
								}
							}
						}
						else
						{
							memcpy(systime,data_in,6);
							sys_time_chang();
							OSTimeDly ( 200, OS_OPT_TIME_DLY, & err );	
							nw_sent_8(pack_in,pack_len);//sent_CheckT(0);
						}
						 
					}
							
										
					break;
					
				case  SetPassword :	//02	//	����װ������	װ�ó������룺�ַ�����1234����31H32H33H34H��
					printf("\r\nPSW=%x,%x,%x,%x",PSW[0],PSW[1],PSW[2],PSW[3]);
					if(memcmp(PSW,data_in,4)==0) //localpassw[50]
					{
						memcpy(PSW,&data_in[4],4);
						flash_save(PSW,PSW,4);
						nw_sent_8(pack_in,pack_len);
					}
					else
					{
						rev_err(SetPassword,0xff);
					}
					printf("\r\ndata_in=%x,%x,%x,%x",data_in[0],data_in[1],data_in[2],data_in[3]);
					printf("\r\nPSW=%x,%x,%x,%x",PSW[0],PSW[1],PSW[2],PSW[3]);
					break;
					
				case  Pcfg :				//03	//	��վ�·���������	��ָ��Ҫ�����ݲɼ�װ�ý��յ��������ԭ�����
					
//3,0,12,31,32,33,34,  3c, 2,58, 0,0,  2,58,  0,2,3,  31,32,33,34,  b,16,
//					printf("\r\n��������PSW=%x,%x,%x,%x",PSW[0],PSW[1],PSW[2],PSW[3]);
//					printf("\r\n��������%x,%x,%x,%x",data_in[0],data_in[1],data_in[2],data_in[3]);
				
//					printf("\r\ndata_in=%x,%x,%x,%x",data_in[0],data_in[1],data_in[2],data_in[3]);
				
					if((memcmp(PSW,data_in,4)==0)&&(memcmp(CHW,&data_in[14],4)==0)) //localpassw[50]
					{
						*HBT=data_in[4];
						*GDT=(data_in[5]<<8)+data_in[6];
						*SLT=(data_in[7]<<8)+data_in[8];
						*OLT=(data_in[9]<<8)+data_in[10];
						RST[0]=data_in[11]; //�� 0-28
						RST[1]=data_in[12];	//ʱ 0-24
						RST[2]=data_in[13];	//�� 0-60
						
						flash_save((uint8_t *)HBT,(uint8_t *)HBT,11);
						OSTimeDly( 100, OS_OPT_TIME_DLY, & err);
						nw_sent_8(pack_in,pack_len);
						
						printf("RST[0]=%d,RST[1]=%d,RST[2]=%d",RST[0],RST[1],RST[2]);
						if(RST[0]==0)
						{
							if(((RST[1]==systime[3])&&(RST[2]==systime[4])))
							{
								printf("׼����λ");
								STM32_rest_delay=5;
							}
						}
					}
					else
					{
						rev_err(Pcfg,0xff);
					}
					break;
				case  HeartBeat:		//05	//	װ��������Ϣ	������վ���װ������ʱ�䡢IP��ַ�Ͷ˿ںš������ź�ǿ�ȼ����ص�ѹ
					
//				if(ph_sent_cnt<ph_gain_cnt)	//����Ƭ���ݷ�
//				{
//					senttask_Asim |= data_flag84_1;
//				}
				
//					senttask_Asim |= data_flag05_ok;
					if(senttask_Pend & data_flag05)
					{
//						if(sleepdelay==2) sleepdelay=1;	//�������������֮ǰ����ʱ��Ϊ1s��������������
						senttask_Pend &= (~data_flag05);
					}
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
							
//							flash_save(HostIP,HostIP,6); //����IP�ɹ��ű���
							
							nw_sent_8(pack_in,pack_len);
							ip_seting=1;
							senttask_Asim |= data_flag06; 	//�踴λ4g����
	//						STM32_rest_delay=5;

							ip_seting=1;
							printf("\r\n׼������IPΪ %d.%d.%d.%d,%d",HostIP[0],HostIP[1],HostIP[2],HostIP[3],*HostPort);
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
				case  check_ip:			//07 ��ѯ��վIP��ַ���˿ںźͿ���,ֱ�Ӷ�flash�ģ�sram�Ŀ���Ϊ����δ��ɵ�
//				if(data_len==0)
					{
						nw_sc.cmd=check_ip;
						nw_sc.len=12;
						flash_read(HostIP,nw_sc.data,6);// nw_sc.data[4],nw_sc.data[5] Ϊ *HostPort,��С�˵���
						nw_sc.data[13]=nw_sc.data[4]; //nw_sc.data[13]��ʱbuf
						nw_sc.data[4]=nw_sc.data[5];
						nw_sc.data[5]=nw_sc.data[13];
//						memcpy(nw_sc.data,HostIP,4); //HostIP[0],HostIP[1],HostIP[2],HostIP[3],*HostPort
//						nw_sc.data[4]=*HostPort>>8;
//						nw_sc.data[5]=*HostPort;
//						memcpy(&nw_sc.data[6],nw_phone_num,6);//nw_phone_num
						memset(&nw_sc.data[6],0,6);
						OSTimeDly( 100, OS_OPT_TIME_DLY, & err);
						nw_sent_st(&nw_sc);
					}
					 
					break;
				case  ResetDev:			//08	//	װ�ø�λ	��վ��װ�ý��и�λ
					if(memcmp(PSW,data_in,4)==0)
					{
						nw_sent_8(pack_in,pack_len);
//						STM32_reseting=1;
						STM32_rest_delay=1;
					}
					else
					{
						rev_err(ResetDev,0xff);
					}
					 
					break;
				case  waikup_device_m://09	//	���Ż���	��վ�Զ��ŷ�ʽ��������״̬��װ��
//					sys_mode=MODE_WORK;
//					wakeup_dev();
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
								nw_sc.data[i++]=*WidthxHeight;
//								Streaming_OK=1;
							}		
//							if(GET_Image_s()==0) //ͼ��1����,�Աȶ�,���Ͷ�
							{
								nw_sc.data[i++]=*bLevel;nw_sc.data[i++]=*cLevel;nw_sc.data[i++]=*sLevel;
//								if(Streaming_OK==1) Streaming_OK=2;
							}
						}
						nw_sc.data[i++]=1;nw_sc.data[i++]=1;nw_sc.data[i++]=60;nw_sc.data[i++]=60;nw_sc.data[i++]=60; //ͨ��2����
						
						for(j=0;j<function_nub[0];j++)
						{
							nw_sc.data[i++]=function_buf[j];
						}
//						memcpy(&nw_sc.data[i],function_buf,strlen((char *)function_buf));
						nw_sc.len=i;//+strlen((char *)function_buf);
//						nw_sc.data=nw_sc.data;
						nw_sent_st(&nw_sc);
					}
					break;
				case  device_function_cfg://0B	//	װ�ù�������	
					 
					if(memcmp(PSW,data_in,4)==0)
					{
						function_nub[0]=data_len-4;
						memcpy(function_buf,&data_in[4],function_nub[0]);
						function_buf[data_len-4]=0; //��β��0
						
						function22_flag=0;
						function25_flag=0;
						function84_flag=0;
						printf(",\t��Ч����:");
						for(j=0;j<function_nub[0];j++)
						{
							if(function_buf[j]==0x22)
							{
								function22_flag=1;
								printf("0x%02x,",function_buf[j]);
							}
							if(function_buf[j]==0x25)
							{
								function25_flag=1;
								printf("0x%02x,",function_buf[j]);
							}
							if(function_buf[j]==0x84)
							{
								function84_flag=1;
								printf("0x%02x,",function_buf[j]);
							}
						}
						printf(",\t��Ч����:");
						for(j=0;j<function_nub[0];j++)
						{
							if((function_buf[j]!=0x22)&&(function_buf[j]!=0x25)&&(function_buf[j]!=0x84))
							{
								printf("0x%02x,",function_buf[j]);
								function_buf[j]=0;
							}
						}
						function_nub[0]=0;
						if(function22_flag)
						{
							function_buf[function_nub[0]++]=0x22;
						}
						if(function25_flag)
						{
							function_buf[function_nub[0]++]=0x25;
						}
						if(function84_flag)
						{
							function_buf[function_nub[0]++]=0x84;
						}
						flash_save(function_nub,function_nub,(function_nub[0]+1));
						
						nw_sent_8(pack_in,pack_len);
					}
					else
					{
						rev_err(device_function_cfg,0xff);
					}
					break;
				case  device_Sleep:	//0C	//	װ������	
					 
					wr_sleep=1;
					break;
				case  check_device_time://0D	//	��ѯװ���豸ʱ��	
					 
					nw_sc.cmd=check_device_time;
					nw_sc.len=6;
//					nw_sc.data=systime;
					memcpy(nw_sc.data,systime,6);
					nw_sent_st(&nw_sc);
					break;
				case  sent_msg:
					 
//					nw_sent(nw_r_post);
//				������	https://blog.csdn.net/qq_30460905/article/details/81805891
					break;
				case  request_data:	//21	//	��վ����װ������	��վ����ɼ�װ�òɼ����ݲ���������
					 
					if(data_len==0)//���������Ϊ0�ֽڣ��ϴ�δ�ɹ��ϴ�����ʷ���ݣ�������ʷ��Ƭ����װ������ʷ�������ϴ�
					{
						if(function22_flag) senttask_Asim |= data_flag22;
						if(function25_flag) senttask_Asim |= data_flag25;
						Contact_state=1;
					}
					else if((data_len==2)&&(data_in[0]==0xBB)&&(data_in[1]==0xBB))//װ�����̲ɼ��������ݣ�ͼƬ���⣩����ɲɼ��������ϴ����ôβ�����Ӱ��ԭ�趨�ɼ������ִ��
					{
						printf("\r\n�����ɼ������ϴ�");
						if(function22_flag)
						{
							get_a_data_ll();	//
							if(gain_ll_data()==0) senttask_Asim |= data_flag22;
						}
						if(function25_flag)
						{
							if(gain_qx_data()==0) senttask_Asim |= data_flag25;
						}
						Contact_state=1;
					}
//					nw_sent(nw_r_post);
					break;

					case  request_la_qj://22	//	�ϴ���������	װ���յ������������������δ���ͳɹ�������������վ��
						
					if(data_len==0)//��վ��������
					{
						printf("\r\n������������");
//						get_a_data_ll();	//1sһ��
//						gain_ll_data();
//						LastGDT=work_p100ms;
						if(function22_flag) senttask_Asim |= data_flag22;	//
						else
						{
							printf("\r\nû������22����");
						}
						Contact_state=1;
						 
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
//								if(requestdata)
//								{
//								}
							}
							else
							{
//								printf("\r\n���ݱ�ʶ�Ŵ���,��ʶ��ӦΪ%x",sampled_data_nub);
							}
						}
					}
					break;
				case  request_qx:		//0x25	//	�ϴ���������	װ���յ������������������δ���ͳɹ�������������վ��
					if(data_len==0)//��վ��������
					{
						printf("\r\n������������");
//						get_qx_data();
//						LastGDT=work_p100ms;
						if(function25_flag) senttask_Asim |= data_flag25;
						else
						{
							printf("\r\nû������25����");
						}
						Contact_state=1;
						 
					}
					else if(data_len==3)
					{
//						�ڼ�֡��ʶ 1�ֽ�	AA55H
						if((data_in[1]==0xAA)&&(data_in[2]==0x55))
						{
//							printf("\r\n�յ���վ��25����,��ʶ��Ϊ%x",data_in[0]);
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
					
				case 0x81:	//ͼ��ɼ���������
					 
				if(memcmp(PSW,data_in,4)==0)
				{
					star_add=&HK_post_buf[HK_post_cnt];
					
					HK_post_buf[HK_post_cnt++]=cmd;
//					HK_post_buf[HK_post_cnt++]=data_in[4]; //�ڰ�
					HK_post_buf[HK_post_cnt++]=data_in[5]; //��С
					HK_post_buf[HK_post_cnt++]=data_in[6];//����
					HK_post_buf[HK_post_cnt++]=data_in[7];//�Աȶ�
					HK_post_buf[HK_post_cnt++]=data_in[8];//���Ͷ�
					
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
					buf_len=data_in[5]*3+1;
					if(data_in[4]==1)
					{
						memcpy(aut_point_group1,&data_in[5],buf_len);
						flash_save(aut_point_group1,aut_point_group1,buf_len);
						nw_sent_8(pack_in,pack_len);
					}
					if(data_in[4]==2)
					{
						memcpy(aut_point_group2,&data_in[5],buf_len);
						flash_save(aut_point_group2,aut_point_group2,buf_len);
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
//					printf("\r\n��ѯ����ʱ��");
					nw_sent_ph_time(data_in[0]); //ͨ����					
				}
				break;
					
				case 0x83:	//��վ����������Ƭ
					if(function84_flag)
					{
						star_add=&HK_post_buf[HK_post_cnt];
						HK_post_buf[HK_post_cnt++]=cmd; //����ָ��
						HK_post_buf[HK_post_cnt++]=data_in[1];	//Ԥ��λ
						if(data_in[0]==1) OSQPost(&hktask_Asim[0],star_add,2,OS_OPT_POST_FIFO,&err);
						if(data_in[0]==2) OSQPost(&hktask_Asim[1],star_add,2,OS_OPT_POST_FIFO,&err);
						nw_sent_8(pack_in,pack_len);//����ԭ�����
					}
					else
					{
						printf("\r\nû������84����");
					}
					
					break;
				
				case 0x84:	//�ɼ��ն�����������Ƭ��
						 
						senttask_Pend &= (~data_flag84_1);

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
//							printf("\r\nsend photo OK");
						}
						else //��Ҫ����
						{
							 
							resent_ph(data_in,data_len);
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

			default:
				break;
			}
		}
	}
}



