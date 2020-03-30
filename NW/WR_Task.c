
#include "global.h"
//#include "stm32l4xx_hal_def.h"
#include "bsp.h"
//#include "nw.h"

//��DT��ָ��ڿ���̨�У���ȷ��ʾϵͳ��ǰʱ�䡣
//��SIG��ָ���ʾGPRS���ź�ǿ��(0~31)��
//��RB��ָ��ն�������
//��FE��ָ��ͼ���ʽ��SD�������ͼƬ�����������ļ�ϵͳ
//��SS��ָ���ʾϵͳ����״̬��Ϣ��
//��VER��ָ���ʾ����Ĺ̼��汾��Ϣ��

//��CONFIGDTU��ָ����簴�ա�SERVERIP= 183.63.92.43���͡�SERVERPORT=12677��ָ�����õķ�����IP�Ͷ˿ں���������DTU����Ҫ������Ч��
//��RSDTU��ָ���ʾDTU��λ��Ϣ�����ϵ�������DTU��
//��FDS��ָ��ն������ָ�����������

//#include "work.h" 
//#include "FM25V10.h"

//void ur_ctler(char *udata);
//int PTZCtrl_presets(char chanle,char presetN); 
uint8_t TC_C=0,wr_sleep;

uint8_t ctl_buf[40];

void ur_test(uint8_t *rev_data)
{
	; 
}

void TC(uint8_t *rev_data,uint8_t rlen)
{
	uint8_t buf;		//rlen,
	int i;
	
//	printf("\r\n���� %x",rlen);
//	rlen  =strlen((char *)rev_data);
//	printf("\r\n���� %x",rlen);
//	printf(":%x,%x",rev_data[0],rev_data[1]);
	
	if(TC_C==1)	//ϵͳ���Դ���
	{
		printf("\r\nTC to ur1 %s",rev_data);
	}
	else	if(TC_C==2)//485���� �� \r\n
	{
		strcat((char *)rev_data,"\r\n");
		
		HAL_UART_Transmit_IT(&huart2, rev_data, strlen((char *)rev_data));
		printf("\r\nTC to ur2 %s",rev_data);
	}
	else	if(TC_C==3)//SIM7600 4G ���ݴ���
	{
		buf=0;
			
		for(i=0;i<rlen;i++)
		{
			buf += rev_data[i];
		}
		rev_data[rlen] = (0xff-buf);
		rev_data[rlen+1]=0x16;	
		rev_data[rlen+2]=0;	
		printf("\r\nTC to ur3:");
		if(rlen!=0)	
		{
			HAL_UART_Transmit_IT(&huart3, rev_data, rlen+2);
			for(i=0;i<rlen+2;i++)	printf("%x,",rev_data[i]);
		}
	}
	else	if(TC_C==5)//SIM7600 4G ָ���
	{
		strcat((char *)rev_data,"\r\n");
		HAL_UART_Transmit_IT(&huart5, rev_data, strlen((char *)rev_data));//, 300
		printf("\r\nTC to ur5 %s",rev_data);
	}
	else if(TC_C==6)//����
	{
		printf("\r\nTC to ur6 %s",rev_data);
		ur_test(rev_data);
	}
	else//	if(TC_C==4)//û��
	{
		printf("\r\nTC to unkuow");
	}
}

int check_signel(void);
extern unsigned char signal;
extern unsigned char ip_seting;

int UART_Receive_s(UART_HandleTypeDef *huart,uint8_t **pack_point, uint16_t *pack_len,int time_out);
void cmd_test(uint8_t *udata,uint16_t ulen);
void data_rst(void);
void data_read(void);
	
int get_ip(uint8_t *ip_buf,uint8_t *in,uint16_t len)
{
	char i,j=0,lenout=0;
	int buf32;
	
//	if(len>255)
//	{
//		printf("\r\ncommand too long");
//		return -1;
//	}
	
	for(i=0;i<len;i++)
	{
		if(i==len-1)
		{
			i=len; //��β
		}
		if((in[i]=='.')||(i==len))
		{
			buf32=c8_to_u32_d((uint8_t *)&in[lenout],(i-lenout));
			if((buf32<=255)&&(buf32>=0))
			{
				ip_buf[j++]=buf32;
				lenout=i+1;
				if(j>=5) 
				{
					printf("command err0");
					return -1;
				}
			}
			else
			{
				printf("\r\ncommand err1");
//				printf("\r\ncommand err1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg");
				return -1;
			}
		}
	}
	if(j==4) return 0;
	printf("\r\ncommand err2");
	return -1;
}

//char get_port(uint32_t *port,uint8_t *in,uint16_t len)
//{
//	*port=c8_to_u32_d((uint8_t *)in,len);
//	if(*port>65535)
//	{
//		printf("command err0");
//		return -1;
//	}
//	return 0;
//}
int sd_mkfs(void);

void cmd_no_parameter(uint8_t *udata,uint16_t ulen)	//�޲�ָ��
{	
	if (0 == strncmp("DT", (char *)udata,ulen))//��DT��ָ��ڿ���̨�У���ȷ��ʾϵͳ��ǰʱ�䡣
	{
		printf("\r\n20%d�� %d�� %d�� %d:%d:%d",systime[0],systime[1],systime[2],systime[3],systime[4],systime[5]);
	}
	if (0 == strncmp("SIG", (char *)udata,ulen))//��SIG��ָ���ʾGPRS���ź�ǿ��(0~31)��
	{
		if(check_signel()==0)
			
		printf("\r\n�ź�ǿ��:%d",signal);
	}
	if (0 == strncmp("RB", (char *)udata,ulen))//��RB��ָ��ն�������
	{
		printf("\r\n׼����λ");
		HAL_NVIC_SystemReset();
	}
		
	if (0 == strncmp("FE", (char *)udata,ulen))//��FE��ָ��ͼ���ʽ��SD�������ͼƬ�����������ļ�ϵͳ
	{
		printf("\r\n�յ���ʽ��ָ��");
		sd_mkfs();
//		printf("\r\n׼����ʽ��sd(δ���)");
	}
	if (0 == strncmp("SS", (char *)udata,ulen))//��SS��ָ���ʾϵͳ����״̬��Ϣ��
	{
		printf("\r\nδ���");
	}
	if (0 == strncmp("VER", (char *)udata,ulen))//��VER��ָ���ʾ����Ĺ̼��汾��Ϣ��
	{
		printf("\r\nδ���");
//		printf("\r\nver:%x.%x",version[0],version[1]);
	}
	if (0 == strncmp("CONFIGDTU", (char *)udata,ulen))//��CONFIGDTU��ָ����簴�ա�SERVERIP= 183.63.92.43���͡�SERVERPORT=12677��ָ�����õķ�����IP�Ͷ˿ں���������DTU����Ҫ������Ч��
	{					
		ip_seting=1;
//		senttask_Asim |= data_flag06; 	//�踴λ4g����
		printf("\r\n׼������IPΪ %d.%d.%d.%d,%d",HostIP[0],HostIP[1],HostIP[2],HostIP[3],*HostPort);
	}
	if (0 == strncmp("RSDTU", (char *)udata,ulen))//��RSDTU��ָ���ʾDTU��λ��Ϣ�����ϵ�������DTU��
	{
		printf("\r\nδ���");
//		printf("\r\nver:%x.%x",version[0],version[1]);
	}
	
	if (0 == strncmp("FDS", (char *)udata,ulen))//��FDS��ָ��ն������ָ�����������
	{
		data_rst();
		data_read();
	}
}

//1	0	���������Դ
//2	����Ԥ�õ�	��������ڵ�ָ��Ԥ�õ�
//3	0	���ϵ���1����λ
//4	0	���µ���1����λ
//5	0	�������1����λ
//6	0	���ҵ���1����λ
//7	0	������Զ������1����λ����ͷ�䱶�Ŵ�
//8	0	�������������1����λ
//����ͷ�䱶��С��
//9	��������Ԥ�õ�	���浱ǰλ��ΪĳԤ�õ�
//10	�ر��������Դ
	
//������PT=1��Z+/Z-��ͨ��1�������Ӧ�������õ�ָ�������о�ͷ�Ŵ����С��
//������ָ��PT=1��H+/H-��ͨ��1�������Ӧ�������õ�ָ�������ˮƽ������ת��������ת��
//������ָ��PT=1��V+/V-��ͨ��1�������Ӧ�������õ�ָ����д�ֱ��������ת����������ת����
//������ָ��PT=1��R+/R-��ͨ��1�������Ӧ�������õ�ָ�����ˮƽ������������ת����
//PT=1��S��ֹͣת����
//PT=1��P1������Ԥ��λ1��
//PT=1��D1��ɾ��Ԥ��λ1��
//PT=1,T ͨ��1��������ڵ�ǰλ�����գ�
//PT=1��C1���ƶ���Ԥ��λ1��	
void cam_ctl_on_platform(uint8_t *udata,uint16_t ulen)	//���ָ��
{
	OS_ERR      err;
	uint16_t cnt_star;
	
	if(HK_post_cnt>HK_post_max) HK_post_cnt=0;
	cnt_star=HK_post_cnt;
	
	if((udata[0]=='Z')&&(udata[1]=='+')) HK_post_buf[HK_post_cnt++]=7;
	else if((udata[0]=='Z')&&(udata[1]=='-')) HK_post_buf[HK_post_cnt++]=8;		
	else if((udata[0]=='H')&&(udata[1]=='+')) HK_post_buf[HK_post_cnt++]=5;
	else if((udata[0]=='H')&&(udata[1]=='-')) HK_post_buf[HK_post_cnt++]=6;
	else if((udata[0]=='R')&&(udata[1]=='+')) HK_post_buf[HK_post_cnt++]=5;
	else if((udata[0]=='R')&&(udata[1]=='-')) HK_post_buf[HK_post_cnt++]=6;
	else if((udata[0]=='V')&&(udata[1]=='+')) HK_post_buf[HK_post_cnt++]=3;
	else if((udata[0]=='V')&&(udata[1]=='-')) HK_post_buf[HK_post_cnt++]=4;
	
	else if(udata[0]=='P')
	{
		if((udata[1]>'9')||(udata[1]<'0'))
		{
			printf("\r\nֻ֧�� 0-9 Ԥ�õ�");
		}
		else
		{
			HK_post_buf[HK_post_cnt++]=9;
			HK_post_buf[HK_post_cnt++]=udata[1]-'0'; //�ݶ�0-9
		}
	}
	else if(udata[0]=='C')
	{
		if((udata[1]>'9')||(udata[1]<'0'))
		{
			printf("\r\nֻ֧�� 0-9 Ԥ�õ�");
		}
		else
		{
			HK_post_buf[HK_post_cnt++]=2;
			HK_post_buf[HK_post_cnt++]=udata[1]-'0'; //�ݶ�0-9
		}
	}
	else if(udata[0]=='S')
	{
		HK_post_buf[HK_post_cnt++]=3;
	}
	else if(udata[0]=='D')
	{
		printf("\r\n��֧��ɾ��");
	}
	else if(udata[0]=='T')
	{
		HK_post_buf[HK_post_cnt++]=0x83;
		if(ulen==1) //�޲�
		{
			HK_post_buf[HK_post_cnt++]=0;
		}
		if(ulen==2) 
		{
			if((udata[1]>'9')||(udata[1]<'0'))
			{
				printf("\r\nֻ֧�� 0-9 Ԥ�õ�");
			}
			else
			{
				HK_post_buf[HK_post_cnt++]=udata[1]-'0'; //�ݶ�0-9
			}
		}
	}

	if(cnt_star!=HK_post_cnt)	OSQPost(&hktask_Asim[0],&HK_post_buf[cnt_star],2,OS_OPT_POST_FIFO,&err);
	else printf("\r\n��������");
}

//int char_da_to_int_da(uint8_t *in,uint32_t *da); //��2000-1-1 0:0:0 �ֿ�ʼ��  00�� 1 �� 1�� Ϊ 0

//int int_da_to_char_da(uint8_t *out,uint32_t da);

int char_time_to_int_time(uint8_t *in,uint32_t *sec); //��2000-1-1 0:0:0 �ֿ�ʼ��

int int_time_to_char_time(uint8_t *out,uint32_t sec); //��2000-1-1 0:0:0 �ֿ�ʼ��

int EncodeDate(u8 *in,uint32_t *Date); 

int DecodeDate(u8 * out,u32 Date); 
int flash_read(uint8_t *ADD,uint8_t *buf,int len);
void sys_time_chang(void);

void cmd_hav_parameter(uint8_t *udata,uint16_t ulen) //�в�ָ��
{
	char i,lenout,cmdsiz;
	uint32_t buf32;
	uint8_t buf8[22];
	
//��T=��ָ�����ϵͳʱ�䣬T=������ʱ���루ʮ���ƣ�
//����T=180522132720���ڿ���̨�У�����ȷ��ָ���ʽ���룬��ʾ����ϵͳʱ��ɹ���		
	if (0 == strncmp("T=", (char *)udata,2))
	{
		cmdsiz=2;
		lenout=(ulen-cmdsiz+1)/2;
		if(udata[2]=='?')
		{
			printf("\r\nsystime:\t\t");
			printf("20%02d-%02d-%02d,%02d:%02d:%02d",systime[0],systime[1],systime[2],systime[3],systime[4],systime[5]);
			
			flash_read(runningtime_buf,runningtime_buf,4);
			printf("\r\n����ʱ��32:\r\n%02x,%02x,%02x,%02x",runningtime_buf[0],runningtime_buf[1],runningtime_buf[2],runningtime_buf[3]);
				
			printf("\r\nrunningtime:%08x\t",runningtime);
			int_time_to_char_time(buf8,runningtime);
			printf("20%02d-%02d-%02d,%02d:%02d:%02d",buf8[0],buf8[1],buf8[2],buf8[3],buf8[4],buf8[5]);
		}
		else
		{
			if(lenout != 6)
			{
				printf("\r\ncommand err");
			}
			else	if(c8_to_d8((uint8_t *)&udata[cmdsiz], (uint8_t *)udata,(ulen-cmdsiz))==0)
			{
				for(i=0;i<6;i++)
				{
					systime[i]=udata[i];
				}
				printf("\r\nsystime:");
				printf("20%02d-%02d-%02d,%02d:%02d:%02d",systime[0],systime[1],systime[2],systime[3],systime[4],systime[5]);
				
	//			char_da_to_int_da(systime,&buf32);
//				EncodeDate(systime,&buf32);
//				printf("\r\nwork da =%d",buf32);
//	//			int_da_to_char_da(buf8,buf32);
//				DecodeDate(buf8,buf32);
//				printf("\r\n sys da:20%02d-%02d-%02d",buf8[0],buf8[1],buf8[2]);
				sys_time_chang();
			}
			else
			{
				printf("\r\ncommand err");
			}
		}
	}
//��MRT=��ָ������������ʱ�䣬����0��Ч��
//��������MRT=18����ʾ�ɹ���Ϣ������ϵͳ��ʱ��λ����ɹ���
	if (0 == strncmp("MRT=", (char *)udata,4))
	{
		cmdsiz=4;
		buf32=c8_to_u32_d((uint8_t *)&udata[cmdsiz],(ulen-cmdsiz));//strlen(&udata[cmdsiz])
		if(buf32<=255)
		{
			*OLT=buf32;
			flash_save((uint8_t *)OLT,(uint8_t *)OLT,sizeof(*OLT));	//flash_save(OLT);
			
			printf("\r\nonline time=%d",*OLT);
		}
		else printf("\r\ncommand err");
	}
//��PT=��ָ������������
	if (0 == strncmp("PT=", (char *)udata,3))
	{
		if((udata[3]=='1')&&(ulen>=6))
		{
			cam_ctl_on_platform(&udata[5],ulen-5);
		}
		else if(udata[3]=='2')
		{
			printf("\r\nû��ͨ��2");
		}
		else
		{
			printf("\r\n��������");
		}
	}
	
	if (0 == strncmp("NETBOARDIP=", (char *)udata,11))
	{					
		cmdsiz=11;
		if(get_ip(buf8,&udata[cmdsiz],ulen-cmdsiz)==0)
		{
			for(i=0;i<4;i++)	LoclIP[i]=buf8[i];						
			flash_save(LoclIP,LoclIP,4);						
			printf("\r\nLoclIP=%d.%d.%d.%d",LoclIP[0],LoclIP[1],LoclIP[2],LoclIP[3]);
		}
	}
	
	if (0 == strncmp("CAMERAIP=", (char *)udata,9))
	{
		cmdsiz=9;
		if(get_ip(buf8,&udata[cmdsiz],ulen-cmdsiz)==0)
		{
			for(i=0;i<4;i++)	CH1IP[i]=buf8[i];		
			flash_save(CH1IP,CH1IP,4);
			flash_read(CH1IP,CH1IP,4);
			printf("\r\nCH1IP=%d.%d.%d.%d",CH1IP[0],CH1IP[1],CH1IP[2],CH1IP[3]);
		}
	}	
	if (0 == strncmp("SERVERIP=", (char *)udata,9))
	{//SERVERIP=183.63.92.43
		cmdsiz=9;
		if(get_ip(buf8,&udata[cmdsiz],ulen-cmdsiz)==0)
		{
			for(i=0;i<4;i++)	HostIP[i]=buf8[i];		
			printf("\r\nSERVERIP=%d.%d.%d.%d",HostIP[0],HostIP[1],HostIP[2],HostIP[3]);
		}
	}	

//	if (0 == strncmp("SERVERPORT=", (char *)udata,11))
//	{
//		cmdsiz=11;
//		if(get_port(&buf32,&udata[cmdsiz],ulen-cmdsiz)==0)
//		{
//			*HostPort=buf32;
//			printf("\r\nSERVERPORT=%d",*HostPort);
//		}	
//		else
//		{
//			printf("\r\nָ�����:");
//			for(i=0;i<ulen;i++) printf("%c",udata[i]);
//		}
//	}
	if (0 == strncmp("SERVERPORT=",(char *) udata,11))
	{
		buf32=c8_to_u32_d((uint8_t *)(&udata[11]),ulen-11);
		if(buf32<65535)
		{
			*HostPort=buf32;
			printf("\r\nSERVERPORT=%d",*HostPort);
		}
		else
		{
			printf("\r\nָ�����:");
			for(i=0;i<ulen;i++) printf("%c",udata[i]);
		}
	}
	
//��STC=��ָ� [�������(1bytes),���������2bytes��������ʱ����2bytes��������ʱ����2bytes��,3c��ʱ�䵥λ60s��]ʮ�����ơ�
//���� STC=01000A0000000A3C��
//�����ü��ʱ��ɹ����ն�Ӧ�������õ��������ʱ�䷢��������Ĭ��1���ӣ�����������ɼ����������ݣ���ʱû����ӣ�������ʱ��������ߣ�0���ӣ�������ʱ���������У�10���ӣ���
	if (0 == strncmp("STC=", (char *)udata,strlen("STC=")))
	{
		printf("\r\nδ���");
	}
	
//	��PIC=1��ָ�������ձ�
//����PIC=1������ȷ��ʽ����ָ���ʱ����ʱ����������Ϊ��,����վ�������ϲ�ѯ����ʱ��Ӧ����ʾΪ��
//��PIC=2��ָ����¼����Ա�
//����PIC=2������ȷ��ʽ����ָ���ʱ����ʱ����������Ϊ��
	if (0 == strncmp("PIC=", (char *)udata,strlen("PIC=")))
	{
		if(udata[4]=='1')
		{
			aut_point_group1[0]=0;
			flash_save(aut_point_group1,aut_point_group1,1);
			printf("\r\n���ͨ��1��ʱ����");
		}
		else if(udata[4]=='2')
		{
			printf("\r\nû��¼����");
		}
		else
		{
			printf("\r\n��������");
		}
		
	}
	
//��TID=��ָ������ն˺��룬 [�ն˺��루6bytes��+������֤�루4bytes��]ʮ�����ƣ�
//����TID=43433030303131323334��
//ǰ�����豸ID�ţ�����������31323334��
//����ȷ��ʽ����ָ��س����к���ʾ�ɹ���Ϣ��
	if (0 == strncmp("TID=", (char *)udata,strlen("TID=")))
	{
		if(ulen==14)
		{
			for(i=0;i<10;i++) TID[i]=udata[i+4];
			printf("\r\nTID=");
			flash_save(TID,TID,10);
			flash_read(TID,TID,10);
			for(i=0;i<10;i++) printf("%c",TID[i]); 
		}
		else printf("\r\n���ȴ���");
	}
//��PSW=��ָ������ն����� ��[�ն����루4bytes��]ʮ�����ơ�
//����PSW=31323334��
//����ȷ��ʽ����ָ���ʾ�����ն�����ɹ���Ϣ��
	if (0 == strncmp("PSW=", (char *)udata,strlen("PSW=")))
	{
		if(ulen==8)
		{
			for(i=0;i<4;i++) PSW[i]=udata[i+4];
			printf("\r\nPSW=");
			flash_save(PSW,PSW,4);
			flash_read(PSW,PSW,4);
			for(i=0;i<4;i++) printf("%c",PSW[i]); 
		}
		else printf("\r\n���ȴ���");
	}
//��RT=��ָ�� ����������ʱ�䣬[��(2bytes)+ʱ(2bytes)+��(bytes)]ʮ���ƣ�
//����RT=000203��
//������ȷ��ʽ����ָ���ʾ�����ն˶�ʱ����ʱ���ɹ���Ϣ��
//��������Ϊ00���ն�ÿ����������ÿ���2��3�ֶ�ʱ������
	if (0 == strncmp("RT=", (char *)udata,strlen("RT=")))
	{
		printf("\r\nδ���");
	}
	
//��SV=��ָ����õ�ѹУ׼ֵ��[��ѹ{1bytes)]ʮ������}��ΧΪ90-150��
//���� SV=6E��
//������ȷ��ʽ����ָ���ʾ�ɹ���Ϣ,���ѹУ׼ֵΪ110��

//��VP=��ָ��������������ѹ�� [��Դ��ѹ(1bytes)]ʮ����
//����VP=120
//����ȷ��ʽ����ָ���ʾ�ɹ���Ϣ�����ն˵�ѹ��������ֵʱ���·��������Դ�������̨Ӧ������ʾ��ϵͳ��ѹ�����ż�ֵ���޷��������Դ�����ñ�����������Դ��ѹӦΪ�㡣

//��WTC=��ָ� [DTU�ϵ��ʼ��ʱ�䣨1bytes��+����ϵ��Լ�ʱ�䣨1bytes�������������ݲɼ�ʱ�䣨1bytes�ޣ�+ͼƬ��������1bytes��]ʮ�����ƣ�
//����WTC=1E780A01����ͼ���Ϊ1/64��ÿ��
//������ָ����ȷ�����ø�����豸���ϵ���ʱ����ʱ��ɹ���
	
//��PTS=��ָ��,������̨ת���;�ͷ�����Ŀ���ʱ�䲽����[2bytes]ʮ�����ơ�
//����PTS=20��20,
//������ȷ��ʽ����ָ���ʾ�ɹ���Ϣ�����óɹ��������巢����̨�ƶ�����󣬵ȴ����õĿ���ʱ�䣬����ᷢ����ֹ̨ͣ�ƶ��������ֵ��Сʱ������յ���̨����·�����ת����ת���ת���ķ��Ȼ��С����ֵ����ʱ������յ���̨����·�����ת����ת���ת���ķ��Ȼ���
//�����óɹ������巢����ͷ�����󣬵ȴ����õĿ���ʱ�䣬����ᷢ����ͷֹͣ�����������ֵ��Сʱ������յ���̨����·��ľ�ͷ�Ŵ����С�����ͷ�����ķ��Ȼ��С����ֵ����ʱ������յ���̨����·��ľ�ͷ�Ŵ����С�����ͷ�����ķ��Ȼ���

//��POT=[xx]��ָ�[1byte]ʮ���ƣ�
//����POT=30��30
//����ȷ��ʽ����ָ���ʾ�ɹ���Ϣ�������óɹ�����ʾ�����Դ�ڴ򿪵�����£����κβ�������30����ʱ����Զ��ر������Դ��

//��PRC=[xx]��ָ�[1byte]ʮ���ƣ�
//����PRC=50��
//����ȷ��ʽ����ָ���ʾ�ɹ���Ϣ����������󲹰�����Ϊ50�����������������ͻὫͼƬ�洢��SD���ȴ��źź�ʱ���ϴ���

//��DRC=[xx]��ָ�[1byte] ʮ�����ƣ�����0��Ч��
//����DRC=05��
//����ȷ��ʽ����ָ���ʾ�ɹ���Ϣ�������÷���5������ָ��֮��δ�յ���վ�ظ�������DTU�� 
   
//��PMWN=[xx]��ָ�[1byte]ʮ���ƣ�
//����PMWN=200��
//SD�����洢200��ͼƬ���źŲ����ϴ����˵�ͼƬ�������ڼ����յ�ͼƬ���ϴ������ͼƬ���ͻ�洢��SD�ڡ�

//��CVD=[xx,xx]��ָ�[1byte��1byte]ʮ���ƣ�ͨ���ţ��Ķ���Ƶʱ��
//����CVD=1��30
//���ͨ��1������Ƶ30�롣

//��PHOTOSIZE=[xx,xx]��ָ�[1byte,1byte]
//����PHOTOSIZE=7,7
//����ͨ��1��ͨ��2��ͼƬ��СΪ1280*720.������ֵ�ο��Ϸ���Լ�������ֻ֧��1280*720=>7��1920*1080=>8��1280*960=>10)
//��CNT=5����ָ�豸����ʱ����������5�κ��豸δ�յ���Ӧ�����豸����00H������01H�����ٷ�00H�������豸����01H������5�κ�Ҳδ�յ���Ӧ��������Уʱ����������05H��

//��CSDF=[xx]��ָ�[1byte]
//���磺CSDF=1�������ʽ�������SD��
//��MCFG=[xx]��ָ�����������ָ��
//b0-WD,b1-SD,b2-FS,b3-FX,b4-QY,b5-RZ,in HEX (1 bytes),bit = 0:�ޣ�bit=1���С�
//bit 5:����
//bit 4:��ѹ
//bit 3:����
//bit 2:����
//bit 1:ʪ��
//bit 0:����
//���磺��Ҫ�������й�����Ч��MCFG=FF

//��WCFG=[XXXXXXXX]��ָ��[4byte]����������ָ�1�����У�0������
	
//PWR=1(��DTU)	PWR=2���ر�DTU)
//PWR=3(�����1��	PWR=4(�ر����1��
//PWR=5�������2��	PWR=6���ر����2��
//PWR=9����·�ɣ�	PWR=A(�ر�·�ɣ�
//PWR=B(����̫����	PWR=C(�ر���̫����
if (0 == strncmp("PWR=", (char *)udata,strlen("PWR=")))
{
	if(udata[4]=='1') {PWR_W_DTU(1);printf("\r\n��DTU");}
	else if(udata[4]=='2') {PWR_W_DTU(0);printf("\r\n�ر�DTU");}
	else if(udata[4]=='3') {PWR_W_CAM1(1);printf("\r\n��CAMERA1");}
	else if(udata[4]=='4') {PWR_W_CAM1(0);printf("\r\n�ر�CAMERA1");}
	else if(udata[4]=='5') {PWR_W_CAM2(1);printf("\r\n��CAMERA2");}
	else if(udata[4]=='6') {PWR_W_CAM2(0);printf("\r\n�ر�CAMERA2");}
	
	else if(udata[4]=='9') printf("\r\nû��·��");
	else if(udata[4]=='A') printf("\r\nû��·��");
	
	else if(udata[4]=='B') {PWR_W_ETH(1);printf("\r\n����̫��");}
	else if(udata[4]=='C') {PWR_W_ETH(0);printf("\r\n�ر���̫��");}
}

//��WPC=[xx]��ָ�[1 byte]
//WPC=1��12V΢���󴫸�����Դ����

//��RCS=[xx]��ָ��,[1 byte]
//���磺RCS=20,������У׼ϵ����Ĭ��Ϊ20������0.2mm

//��SRP=[xx,xx]����[1 byte,1 byte]
//���磺SRP=2��80  2��������Ϊ����80��������ع���������80��

//��IOUT=[1~6]
//1������1����2������1�أ�3������2����4������2�أ�5������3����6����3��

//UDB=[0~6],{n bytes in HEX} 
//���磺UDB=0,123.�����̨���ڷ���123���ݣ���ԭ���ݷ���123

	
}

extern uint8_t sleepdelay;

void WR_Task( void * p_arg )
{
	OS_ERR      err;
	(void)p_arg;
	uint8_t *udata;
	uint16_t ulen;
	

	while (DEF_TRUE)
	{
		OSTimeDly ( 100, OS_OPT_TIME_DLY, & err ); 
		if(UART_Receive_s(&huart1,&udata, &ulen,1000)==2)
		{
//			printf("\r\nulen=%d",ulen);
//			printf("\r\nrev_data=%s",udata);
			sleepdelay=6;
			
			if((strncmp((char *)udata, "uart", 4)==0)&&(ulen>=5))
			{
				TC_C=udata[4]-'0';
				if(ulen>6) ulen=ulen-6;
				else ulen=0;
				TC(&udata[6],ulen);
			}
			else if(TC_C !=0)
			{
				TC(udata,ulen);
			}
			else
			{
				if((ulen>=2)&&(ulen<255))
				{
					if((udata[ulen-2]=='\r')&&(udata[ulen-1]=='\n')&&(ulen>=4))
					{
						ulen -= 2;
					}
					cmd_test(udata,ulen);
					cmd_no_parameter(udata,ulen);
					cmd_hav_parameter(udata,ulen);
				}
			}
		}
//		else
//		{
//			if(((huart1.Instance->CR1)&(USART_CR1_PEIE | USART_CR1_RXNEIE)) != (USART_CR1_PEIE | USART_CR1_RXNEIE))
//			{
//				printf("\r\nhuart1------------------------terrorflags");
//				HAL_UART_Abort_IT(&huart1);
//				HAL_UART_Receive_IT(&huart1,(unsigned char *)urd1,lenmax1);
//			}
//		}
	}
}

#define		WR_Task_SIZE                     512
#define		WR_Task_PRIO                     9
OS_TCB		WR_Task_TCB;															//������ƿ�
CPU_STK		WR_Task_Stk [ WR_Task_SIZE ];	//�����ջ

void WRTaskStart (void *p_arg)
{
	OS_ERR      err;
	(void)p_arg;

	
	

#if 1
						 
		/* ����  �������� ���� */
    OSTaskCreate((OS_TCB     *)&WR_Task_TCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"WR_Task_Name",                             //��������
                 (OS_TASK_PTR ) WR_Task,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) WR_Task_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&WR_Task_Stk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) WR_Task_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) WR_Task_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������
#endif	
								 

}
int read_data_ph(uint16_t rcnt,uint8_t *out);
int index_ph_dec(unsigned char *index);
void sent_HeartBeat(void);
extern uint8_t Sphoto_step;


void cmd_test(uint8_t *udata,uint16_t ulen)
{
	OS_ERR      err;
//	char aaa[3];
	
	if (0 == strncmp("UR3 RST0", (char *)udata,8))
	{
		printf("\r\nur3��λ0");
		HAL_UART_Abort_IT(&huart3);
		MX_USART3_UART_Init();
		HAL_UART_Receive_IT(&huart3,(unsigned char *)urd3,lenmax3);
		huart3.GetPoint=0;//huart->NowPoint;
		huart3.ReadPoint=0;//=huart->NowPoint;
		huart3.NowPoint=0;
	}
	if (0 == strncmp("UR3 RST1", (char *)udata,8))
	{
		printf("\r\nur3��λ1");
		huart3.GetPoint=0;//huart->NowPoint;
		huart3.ReadPoint=0;//=huart->NowPoint;
		huart3.NowPoint=0;
	}
	if (0 == strncmp("UR3 RST2", (char *)udata,8))
	{
		printf("\r\nur3��λ2");
		HAL_UART_Receive_IT(&huart3,(unsigned char *)urd3,lenmax3);
	}
	if (0 == strncmp("UR3 RST3", (char *)udata,8))
	{
		printf("\r\nur3��λ3");
		HAL_UART_Abort_IT(&huart3);
		HAL_UART_Receive_IT(&huart3,(unsigned char *)urd3,lenmax3);
	}
	if (0 == strncmp("UR3 RST4", (char *)udata,8))
	{
		printf("\r\nur3��λ4");
		MX_USART3_UART_Init();
		HAL_UART_Receive_IT(&huart3,(unsigned char *)urd3,lenmax3);
	}
	if (0 == strncmp("DTU RST", (char *)udata,7))
	{
		printf("\r\nDTU��λ");
		DTU_reset();
	}
	if (0 == strncmp("UR3 DTU RST", (char *)udata,11))
	{
		printf("\r\nUR3DTU��λ");
		DTU_reset();
		HAL_UART_Abort_IT(&huart3);
		MX_USART3_UART_Init();
		HAL_UART_Receive_IT(&huart3,(unsigned char *)urd3,lenmax3);
		huart3.GetPoint=0;//huart->NowPoint;
		huart3.ReadPoint=0;//=huart->NowPoint;
		huart3.NowPoint=0;
	}
	
	if (0 == strncmp("CDBF", (char *)udata,4))
	{
		data_read();
	}
	if (0 == strncmp("delete ph", (char *)udata,9))
	{
		if(read_data_ph(ph_sent_cnt,ctl_buf)==0)	 
		{
			Sphoto_step=0;
			index_ph_dec(ctl_buf); //�����˰�
//			printf("\r\nwr0");
		}
//			printf("\r\nwr1");
	}
	if (0 == strncmp("sleep", (char *)udata,5))
	{
		wr_sleep=1;
//			printf("\r\nwr1");
	}
	if (0 == strncmp("heart", (char *)udata,5))
	{
		sent_HeartBeat();
	}
	
	if(0 == strncmp("task", (char *)udata,4))
	{
		extern OS_TCB   TaskDSP_TCB;			//������ƿ�
		extern OS_MEM  mem; 
		char *   p_mem_blk;
		
		
//		OSTaskSemPost((OS_TCB  *)&TaskDSP_TCB,   //Ŀ������
//									(OS_OPT   )OS_OPT_POST_NONE, //ûѡ��Ҫ��
//									(OS_ERR  *)&err);            //���ش�������	
		if((udata[4]>='0')&&(udata[4]<='9'))
		{
			/* ���ڴ���� mem ��ȡһ���ڴ�� */
			p_mem_blk = OSMemGet((OS_MEM      *)&mem,
										   (OS_ERR      *)&err);
			
			* p_mem_blk = udata[4]-'0';
			
			OSTaskQPost ((OS_TCB      *)&TaskDSP_TCB,      //Ŀ������Ŀ��ƿ�
									 (void        *)p_mem_blk,             //��Ϣ���ݵ��׵�ַ
									 (OS_MSG_SIZE  )1,                     //��Ϣ����
									 (OS_OPT       )OS_OPT_POST_FIFO,      //������������Ϣ���е���ڶ�
									 (OS_ERR      *)&err);                 //���ش�������
		}
	}
	
		if(0 == strncmp("updata", (char *)udata,strlen("updata")))
		{
			updata_flag[0]=0xaa;
			HAL_NVIC_SystemReset(); 
		}
	
	
//	if (0 == strncmp("heart2", (char *)udata,5))
//	{
//		struct nw_ctl abc;
//			
//		senttask_Pend |= data_flag05;
//		aaa.cmd=HeartBeat;
//		aaa.len=8;
//		memcpy(aaa.data,systime,6);
//		
//		if(check_signel()==0) aaa.data[6]=signal;
//		else aaa.data[6]=0;

//		if(Get_Volt()==0) nw_sc.data[7]=volt;
//		else nw_sc.data[7]=0;

//		nw_sent_st(&nw_sc);
//	}
//	printf("\r\nwr2");
}


