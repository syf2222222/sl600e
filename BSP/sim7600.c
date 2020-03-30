
#include "bsp.h"
#include "nw.h"


//+CPIN: READY
extern int c8_to_u32_d(const uint8_t *in,uint8_t len);
extern unsigned char signal,ip_seting;
//unsigned char flag_4g_ready;

extern void UART_data_clear(UART_HandleTypeDef *huart);	
//int UART_Receive(UART_HandleTypeDef *huart,uint8_t **pack_point, uint16_t *pack_len,int wait_time);
int UART_Receive_s(UART_HandleTypeDef *huart,uint8_t **pack_point, uint16_t *pack_len,int time_out);

//4g recv:
//4g recv:
//RDY

//4g recv:
//+CPIN: READY

//4g recv:
//SMS DONE

//4g recv:
//PB DONE
//signed char rev_PB_DONE_delay=-20;
int check_sim7600(void)
{
	uint8_t *udata;	//读取串口数据地址
	uint16_t ulen,i;	//读取串口数据长度
//	flag_4g_ready=0;
	
	if(UART_Receive_s(&huart5,&udata, &ulen,100) != 2) return 0;
//	printf("\r\n4g recv:");	
//	for(i=0;i<ulen;i++)	printf("%c",udata[i]);	
//	if ((0 == memcmp("\r\n+CPIN: READY", udata, sizeof("\r\n+CPIN: READY") - 1))||(0 == memcmp("\r\nRDY", udata, sizeof("\r\nRDY") - 1))
//		||(0 == memcmp("\r\nSMS DONE", udata, sizeof("\r\nSMS DONE") - 1))||(0 == memcmp("PB DONE", udata, sizeof("\r\nPB DONE") - 1)))
	if ((0 == memcmp("\r\nPB DONE", udata, sizeof("\r\nPB DONE") - 1))||(0 == memcmp("PB DONE", udata, sizeof("PB DONE") - 1)))
	{
//		flag_4g_ready +=1 ;
//		rev_PB_DONE_delay=10;
		printf("\r\nDUT_recv:");	
		for(i=0;i<ulen;i++)	printf("%c",udata[i]);	
		return 1;
	}

	return 0;
}


int change_ip_done(void)
{
	uint8_t *udata,buf[40];	//读取串口数据地址
	uint16_t ulen,i;	//读取串口数据长度
		
	sprintf((char *)buf,"AT+HOST=%d.%d.%d.%d,%d\r\n",HostIP[0],HostIP[1],HostIP[2],HostIP[3],*HostPort);//HostIP[0],HostIP[1],HostIP[2],HostIP[3],*HostPort
		printf("设置IP%s",buf);
//		HAL_UART_Transmit_IT(&huart5, buf, strlen((char *)buf));
	
	UART_data_clear(&huart5);
	HAL_UART_Transmit_IT(&huart5, buf, strlen((char *)buf));	//
	
	if(UART_Receive_s(&huart5,&udata, &ulen,1000) != 2) return -1;
	printf("\r\n4g recv:");	
  for(i=0;i<ulen;i++)	printf("%c",udata[i]);	

	if (0 == memcmp("\r\nOK", udata, sizeof("\r\nOK") - 1))
	{
		flash_save(HostIP,HostIP,6);
		printf("\r\n成功更改IP为:%d.%d.%d.%d,%d",HostIP[0],HostIP[1],HostIP[2],HostIP[3],*HostPort);
		return 1;
	}
	
	
	return 0;
}



//OSSemCreate() 创建一个信号量
//OSSemDel() 删除一个信号量
//OSSemPend() 等待一个信号量
//OSSemPost() 释放一个信号量
//OSSemPendAbort() 取消等待
//OSSemSet() 强制设置一个信号量的值
int check_signel(void)
{
//	OS_ERR      err;
	uint8_t *udata;	//读取串口数据地址
	uint16_t ulen;	//读取串口数据长度
	char a,j,sData[5];
	uint16_t buf;
	
	UART_data_clear(&huart5);
	HAL_UART_Transmit_IT(&huart5, (unsigned char *)"AT+CSQ\r\n", strlen("AT+CSQ\r\n"));
	
	if(UART_Receive_s(&huart5,&udata, &ulen,1000) == 2)	//收到数据
	{
		if (0 == memcmp("\r\n+CSQ:", udata, strlen("\r\n+CSQ:")))//+CSQ: 16,99  if (0 == memcmp("\r\n+CSQ:", udata, strlen("\r\n+CSQ:")))//+CSQ: 16,99
		{
			a=0;
			for(j=7;j<10;j++)
			{
				if((udata[j]>='0')&&(udata[j]<='9'))	sData[a++]=udata[j];
				else
				{
					if(udata[j]!=' ') break;
				}
			}
//			printf("\r\n信号强度%s",sData);
			buf=c8_to_u32_d((const uint8_t *)sData,a);
			if(buf>100)	signal=0xff;
			else if(buf>34) signal=100;
			else
			{
				buf=buf*100/34;
				signal=buf;
			}
//			printf("\r\n信号强度%x",signal);
		}
		return 0;
	}
	else
	{
		return -1;
	}
}

