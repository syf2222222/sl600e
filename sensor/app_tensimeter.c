
#include "bsp.h"
#include "nw.h"
#include <stdio.h>
#include <string.h>

//				char st1_id[]="01";
//				char broadcast_id[]="00";
unsigned char checksum_ts(unsigned char* a,unsigned int n);



//22 0A-10
#define  ID_LL1		10

//struct TenS_data
//{
//	unsigned short LL;
//	unsigned short FP;
//	unsigned short QX;   
//};
struct TenS_data_LL MaxLL[3],MinLL[3];
struct TenS_data_FP MaxFP[3],MinFP[3];
uint8_t clean_LLQJ[3];

void  sent_to_ts(char ID,char *CMD,char *CDATA);

extern void UART_data_clear(UART_HandleTypeDef *huart);	
char UART_Receive(UART_HandleTypeDef *huart,uint8_t **pack_point, uint16_t *pack_len,int wait_time);
char UART_Receive_s(UART_HandleTypeDef *huart,uint8_t **pack_point, uint16_t *pack_len,int time_out);

int Get_TS_info(char ID,int *get_data)
{
	uint8_t *udata;	//��ȡ�������ݵ�ַ
	uint16_t ulen;	//��ȡ�������ݳ���
	
	UART_data_clear(&huart2);	
//	printf("\r\ntest ID:%d",ID);
	sent_to_ts( ID,"Q",0);
	UART_Receive_s(&huart2,&udata, &ulen,1000);
	
	if(UART_Receive_s(&huart2,&udata, &ulen,1000)==2)
	{
//		printf("\r\npMsglen:%d",ulen); //pMsglen:9
//		printf("\r\npMsg:%s",udata); //udata:<00CDDC
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
//		printf("\r\nGet_TS_info time out");
		return -3;
	}
	
	return 0;
}


void Test_LL_QJ(uint8_t i)
{
	int hLL,hFP,hQX;
	int flag0,flag1;
	
	flag0=Get_TS_info((i+1)*10+0,&hLL);
	flag1=Get_TS_info((i+1)*10+1,&hFP);
				Get_TS_info((i+1)*10+2,&hQX);
	
	if(clean_LLQJ[i]==0) //��һ������
	{
		clean_LLQJ[i]=1;
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

void computer_2char_data_div10(unsigned char * cdata)
{
	signed short buf;
	buf=((cdata[0]<<8)+cdata[1])/10;
	cdata[0]=(buf>>8);
	cdata[1]=buf;
}

int temp,wet;
int get_qx_data(struct qx_data *qx_d) //struct qx_data *qx_d
{
	char u2_sent[10];
	unsigned char str_len;
	OS_ERR      err;
	char Get[60];//,i
	signed short buf;
	uint8_t *udata;	//��ȡ�������ݵ�ַ
	uint16_t ulen;	//��ȡ�������ݳ���
	
	str_len=sprintf(u2_sent,"#250ZG");//#250ZG  #030CG"  #060FG
	
	UART_data_clear(&huart2);	

	HAL_UART_Transmit(&huart2, (unsigned char *)u2_sent, str_len,100);//if(uart1_t[0] != NULL) 	
		
	if(UART_Receive_s(&huart2,&udata, &ulen,1000)==2)
	{
//		printf("\r\npMsglen:%d",ulen);
//		printf("\r\npMsg:%s",udata);

		if((memcmp(udata,"250Z",4)==0)&&(ulen>=120)&&(udata[ulen-1]=='G'))	//&&(memcpy(&udata[114],"GG",2)==0) 128
		{
			c8_to_u8((uint8_t *)&udata[54],(uint8_t *)Get,ulen-72); //4 +14+36+16+2
//			printf("\r\nת������ Get:");
			
//ʱ�� 	��	�¶� 	ʪ�� 	�� 	����ѹ��	 ��	˲ʱ����	˲ʱ���� 1����ƽ������	1����ƽ������	10����ƽ������	10����ƽ������	10����������	��  �ܷ���˲ʱֵ	�� 	 
//14		36		4			4 	4			4 				4		4						4				4�ֽ�					4�ֽ�						4�ֽ�						4�ֽ�					4�ֽ�					4�ֽ�	4�ֽ�				16�ֽ�	
//Get:			00FA 	01D5 0000 2788 			0000 	0005 		0CD2 			0003 						0C94 				0000 							0000 						0006 					0000 		0007 		
//Get:			00FA	01D2 0000	2788			0000	0000		0000			0000						06ED				0000							06BD						0001					0000		0007			
//					00FC	01F2 0000	277F			0000	0000		0BB0			0000						000000000000000100000002
//					00FC	020200002781000000010C4A0000000000000000000200000000			
//			for(i=0;i<(ulen-72)/2;i++)	printf("%02X",Get[i]);

			memcpy(qx_d->time,systime,6);
			
			memcpy(qx_d->temp,&Get[0],2);
			
			buf=((Get[2]<<8)+Get[3])/10;
			qx_d->hum=buf;//  memcpy(qx_d->hum,&Get[3],1);//ֻҪ��һ�ֽ�
//			memcpy(qx_d->hum,&Get[4],2);//��
			memcpy(qx_d->pre,&Get[6],2);
//			memcpy(qx_d->pre,&Get[8],2);//��
			memcpy(qx_d->winds0,&Get[10],2);
			memcpy(qx_d->windd0,&Get[12],2);
			memcpy(qx_d->winds1,&Get[14],2);
			memcpy(qx_d->windd1,&Get[16],2);
			memcpy(qx_d->winds10,&Get[18],2);
			memcpy(qx_d->windd10,&Get[20],2);
			memcpy(qx_d->windm10,&Get[22],2);
			qx_d->rain[0]=0;qx_d->rain[1]=0;
			qx_d->sun[0]=0;qx_d->sun[1]=0;

//�¶ȼ�500;
			buf=(qx_d->temp[0]<<8)+qx_d->temp[1];
			buf=buf+500;
			qx_d->temp[0]=(buf>>8);qx_d->temp[1]=buf;

//ʪ��/10;
//			qx_d->hum=qx_d->hum/10; 

//����.��ѹ/10;
			computer_2char_data_div10(qx_d->windd0);
			computer_2char_data_div10(qx_d->windd1);
			computer_2char_data_div10(qx_d->windd10);
			computer_2char_data_div10(qx_d->pre);
			
		}
	}
	else
	{
//		printf("\r\nur2_Q time out");
	}
	return 0;
}


void AppTaskTS(void * p_arg)
{
	OS_ERR      err;
	(void)p_arg;
//	char CAM[10] = "R",i;
	uint8_t i;
	int lastTime;

	lastTime=worktime;
	for(i=0;i<3;i++) clean_LLQJ[i]=0;
	
//	HAL_UART_Receive_IT(&huart2,(unsigned char *)urd2,1);
	
	while (DEF_TRUE)
	{
	
		OSTimeDly ( 2000, OS_OPT_TIME_DLY, & err );	
				
		if((worktime-lastTime)>=60) 
		{
			lastTime=worktime;
			for(i=0;i<3;i++) clean_LLQJ[i]=0;
		}
			
		for(i=0;i<3;i++)
		{
//			OSTimeDly (1000, OS_OPT_TIME_DLY, & err );	
			Test_LL_QJ(i);
		}
	}

	
}

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
	HAL_UART_Transmit(&huart2, (unsigned char *)u2_sent, str_len,100);

}


