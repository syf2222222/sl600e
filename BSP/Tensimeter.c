
#include "bsp.h"
#include "nw.h"
#include <stdio.h>
#include <string.h>
#include "sram.h"
#include "global.h" 
//��������������

#define  ID_LL1		10

//struct TenS_data
//{
//	unsigned short LL;
//	unsigned short FP;
//	unsigned short QX;   
//};

struct TenS_data_LL
{
	unsigned short LL;
	unsigned short FP;
	unsigned short QX;   
};
struct TenS_data_FP
{
	unsigned short FP;
	unsigned short QX;  
	unsigned short LL;
};

struct TenS_data_LL MaxLL[3],MinLL[3];
struct TenS_data_FP MaxFP[3],MinFP[3];
uint8_t LL_test_duty_start[3];

void  sent_to_ts(char ID,char *CMD,char *CDATA);

extern void UART_data_clear(UART_HandleTypeDef *huart);	
//int UART_Receive(UART_HandleTypeDef *huart,uint8_t **pack_point, uint16_t *pack_len,int wait_time);
int UART_Receive_s(UART_HandleTypeDef *huart,uint8_t **pack_point, uint16_t *pack_len,int time_out);


//�������	
//	�����Ƽ�����Ǵ�������ID����һ·1#����Ѿ�����Ϊ02��03��ÿ����Ǵ�������X�����ݺ�Y�������������ݣ�ID�ŷֱ�Ϊ02��03���ֱ�����ƫ�Ǻ���б�ǣ�
//        ���ڹ������>02Q0E������յ��ظ�<006EVV��VVΪУ��λ��006EΪ��ƫ�����ݣ���Ϊ1.1�ȣ�006Eʮ������תΪʮ���ƣ��ٳ���100��
//        ���ڹ������>03Q0D������յ��ظ�<806EVV��VVΪУ��λ��806EΪ��б�����ݣ���Ϊ -1.1�ȣ�8�����ţ�006Eʮ������תΪʮ���ƣ��ٳ���100��
//        ����յ��ظ�<906EVV��VVΪУ��λ��906EΪ��б�����ݣ���Ϊ -42.06�ȣ�8�����ţ�106Eʮ������תΪʮ���ƣ��ٳ���100��
void  sent_to_ts(char ID,char *CMD,char *CDATA)
{
	char u2_sent[50];//,str_buf[10],str_buf2[10]="\r\n";
	
	unsigned char checksum,i,str_len;

//>00C1 00_ID_VV_CR_LF
//	C1~����
//		ID~���豸��š�ע��00Ϊ�㲥ID��
//		VV~У���룬�ۼӺͰ�λȡ����Ϊ���Է��㣬����ֱ���00�����������У����
//		CR_LF~�س�����
//	if(CDATA !=0) str_len=sprintf(u2_sent,">%2d%s%4d%02x\r\n",ID,CMD,*CDATA,0);
//	else	str_len=sprintf(u2_sent,">%02d%s%02x\r\n",ID,CMD,0);
	
	if(CDATA !=0) str_len=sprintf(u2_sent,">%2d%s%4d",ID,CMD,*CDATA);
	else	str_len=sprintf(u2_sent,">%02d%s",ID,CMD);
	checksum=0;
	for(i=0;i<str_len;i++)
	{
		checksum += u2_sent[i];
	}
	checksum=~checksum;

	str_len += sprintf(&u2_sent[str_len],"%02X\r\n",checksum);
	
//	printf("\r\nu2s:%s",u2_sent);
//	if(((huart2.Instance->CR1)&(USART_CR1_PEIE | USART_CR1_RXNEIE)) != (USART_CR1_PEIE | USART_CR1_RXNEIE))
//	{
//		printf("\r\nhuart2------------------------terrorflags");
//		HAL_UART_Abort_IT(&huart2);
//		HAL_UART_Receive_IT(&huart2,(unsigned char *)urd2,lenmax2);
//	}
	
	HAL_UART_Transmit(&huart2, (unsigned char *)u2_sent, str_len,100);
//	HAL_UART_Transmit(&huart1, (unsigned char *)u2_sent, str_len,100);

}
//char UART_Receive_s1(UART_HandleTypeDef *huart,uint8_t **pack_point, uint16_t *pack_len,int time_out)
//{

//	OS_ERR      err;
//	char Result=0;

//	while((Result!=2)&&(time_out>=0))
//	{
//		Result = UART_Receive(huart,pack_point, pack_len,0); 
//		if(Result !=2)
//		{
//			OSTimeDly ( 10, OS_OPT_TIME_DLY, & err ); 
//			time_out -= 10;
//		}
//		
//		printf("\r\ntime_out=%d",time_out);
//	}
//	return Result;
//}

int Get_TS_info(char ID,int *get_data)
{
	uint8_t *udata;	//��ȡ�������ݵ�ַ
	uint16_t ulen;	//��ȡ�������ݳ���
	
	UART_data_clear(&huart2);	
//	printf("\r\ntest ID:%d",ID);
	sent_to_ts( ID,"Q",0);
	
	
	if(UART_Receive_s(&huart2,&udata, &ulen,100)==2)
	{
//		udata[ulen]=0;
//		printf("\r\nhuart2=%s",udata);
		
		if((udata[0]=='<')&&(ulen>=6)&&(ulen<=15))
		{
			*get_data=c8_to_u32_x((const uint8_t *)&udata[1],ulen-5);
			if(*get_data>0x0ffff) *get_data=0;
//			printf("\r\nget_data=%x",*get_data);
		}
		else
		{
			return -2;
		}
	}
	else
	{
//		printf("\r\nhuart2 û�յ�");
		return -3;
	}
	
	return 0;
}


int lastTime;

uint8_t ll_data_buf[3][32] __attribute__((at(buf_ll)));// 1+ 29+ 25*buf[0]  1024-30 /25 = 39 ���39������

//__align(8) uint8_t value[20] __attribute__((at(0x20000100))); MDK�ж���

int get_a_data_ll(void)	//1sһ��
{
	uint8_t i;
	int hLL,hFP,hQX;
	int flag0,flag1;
			
//	i=0;
	for(i=0;i<3;i++)
	{
//		printf("\r\n\r\n׼����");
		if(i==0)
		{
			flag0=Get_TS_info((i)*10+1,&hLL);//
			flag1=Get_TS_info((i)*10+2,&hFP);
						Get_TS_info((i)*10+3,&hQX);
		}
		else
		{
			flag0=Get_TS_info((i)*10+0,&hLL);//
			flag1=Get_TS_info((i)*10+1,&hFP);
						Get_TS_info((i)*10+2,&hQX);
		}
			
		if(LL_test_duty_start[i]==0) //��һ������
		{
			LL_test_duty_start[i]=1;
			if(flag0==0)
			{
				MaxLL[i].LL=hLL;
				MaxLL[i].FP=hFP;
				MaxLL[i].QX=hQX;
				
				MinLL[i].LL=hLL;
				MinLL[i].FP=hFP;
				MinLL[i].QX=hQX;
			}
			else
			{
				MaxLL[i].LL=0;
				MaxLL[i].FP=0;
				MaxLL[i].QX=0;
				
				MinLL[i].LL=0;
				MinLL[i].FP=0;
				MinLL[i].QX=0;
			}
			
			if(flag1==0)
			{
				MaxFP[i].LL=hLL;
				MaxFP[i].FP=hFP;
				MaxFP[i].QX=hQX;
				
				MinFP[i].LL=hLL;
				MinFP[i].FP=hFP;
				MinFP[i].QX=hQX;
			}
			else
			{
				MaxFP[i].LL=0;
				MaxFP[i].FP=0;
				MaxFP[i].QX=0;
				
				MinFP[i].LL=0;
				MinFP[i].FP=0;
				MinFP[i].QX=0;
			}
		}
		
		if((hLL>MaxLL[i].LL)&&(flag0==0))
		{
			MaxLL[i].LL=hLL;
			MaxLL[i].FP=hFP;
			MaxLL[i].QX=hQX;
		}
		else if((hLL<MinLL[i].LL)&&(flag0==0))
		{
			MinLL[i].LL=hLL;
			MinLL[i].FP=hFP;
			MinLL[i].QX=hQX;
		}
		
		if(((hFP&0x7fff)>(MaxFP[i].FP&0x7fff))&&(flag1==0))
		{
			MaxFP[i].LL=hLL;
			MaxFP[i].FP=hFP;
			MaxFP[i].QX=hQX;
		}
		else if(((hFP&0x7fff)<(MinFP[i].FP&0x7fff))&&(flag1==0))
		{
			MinFP[i].LL=hLL;
			MinFP[i].FP=hFP;
			MinFP[i].QX=hQX;
		}
	}
	return 0;
}

uint32_t last_ll_time;
//������֤	֡��ʶ	����	���ܵ�Ԫʶ����	�װ�	��һ��	�ڶ���	����	��N��
//4�ֽ�	1�ֽ�	1�ֽ�	1�ֽ�					
//�װ���ʽ��
//����ʱ�䣨��+��+��+ʱ+��+�룩��6�ֽڣ�+
//���������2�ֽڣ�+��ƫ�ǣ�2�ֽڣ�+��б�ǣ�2�ֽڣ�+
//��С������2�ֽڣ�+��ƫ�ǣ�2�ֽڣ�+��б�ǣ�2�ֽڣ�+
//����ƫ�ǣ�2�ֽڣ�+��б�ǣ�2�ֽڣ�+������2�ֽڣ�+
//��С��ƫ�ǣ�2�ֽڣ�+��б�ǣ�2�ֽڣ�+������2�ֽڣ�
					
//��һ�����Ժ������ʽ��
//���ϰ�����ʱ��2�ֽڣ�+���������2�ֽڣ�+��ƫ�ǣ�2�ֽڣ�+��б�ǣ�2�ֽڣ�+��С������2�ֽڣ�+��ƫ�ǣ�2�ֽڣ�+��б�ǣ�2�ֽڣ�+����ƫ�ǣ�2�ֽڣ�+��б�ǣ�2�ֽڣ�+������2�ֽڣ�+��С��ƫ�ǣ�2�ֽڣ�+��б�ǣ�2�ֽڣ�+������2�ֽڣ�
int index_ll_inc(uint8_t *in);
extern uint32_t systime32;

int gain_ll_data(void)
{
	uint8_t buf;
	uint8_t *date_save_p,i,j;	//
	int time_grow;
//	printf("\r\n�ɼ���������:");
	buf=0;
	for(j=0;j<function_nub[0];j++)
	{
		if(function_buf[j]==0x22)
		{
			buf=1;
		}
	}
	if(buf==0) 
	{
		return -1;
	}
	
	for(i=0;i<3;i++)
	{
		date_save_p= &ll_data_buf[i][2];
		
		ll_data_buf[i][0]=0;  //////for test
		
		if(ll_data_buf[i][0]==0)
		{
			ll_data_buf[i][1]=(0x10<<i);//���ܵ�Ԫʶ����
			memcpy(date_save_p,systime,6);
			date_save_p += 6;
		}
		else if(ll_data_buf[i][0]<39)
		{
			time_grow=(systime32-last_ll_time);
			if(time_grow>0xffff) return -1;//���������ֽ���
			
			date_save_p +=ll_data_buf[i][0]*26+4; //�ܰ��� �� ��һ�����ĸ�
			date_save_p[0]= (time_grow>>8);
			date_save_p[1]= time_grow;
			date_save_p += 2;
		}
		else
		{
			ll_data_buf[i][0]=0;
		}
		
		last_ll_time=systime32;
		ll_data_buf[i][0]++;
//		printf("%x %x,",ll_data_buf[i][0],ll_data_buf[i][1]);		
		memcpy(&date_save_p[0],&MaxLL[i],6);
		memcpy(&date_save_p[6],&MinLL[i],6);
		memcpy(&date_save_p[12],&MaxFP[i],6);
		memcpy(&date_save_p[18],&MinFP[i],6);
		
//		for(j=0;j<24;j++)
//		{
//				date_save_p[j]=0;
//		}
		for(j=0;j<24;j+=2)
		{
			buf=date_save_p[j];
			date_save_p[j]=date_save_p[j+1];
			date_save_p[j+1]=buf;
		}
		
		LL_test_duty_start[i]=0;
		index_ll_inc(ll_data_buf[i]);
		
	}
		
		
	return 0;
}





