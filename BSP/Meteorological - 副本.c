
//微气象传感器驱动

#include "bsp.h"
#include "nw.h"
#include <stdio.h>
#include <string.h>


void  sent_to_ts(char ID,char *CMD,char *CDATA);

extern void UART_data_clear(UART_HandleTypeDef *huart);	
char UART_Receive(UART_HandleTypeDef *huart,uint8_t **pack_point, uint16_t *pack_len,int wait_time);
char UART_Receive_s(UART_HandleTypeDef *huart,uint8_t **pack_point, uint16_t *pack_len,int time_out);

//void computer_2char_data_div10(unsigned char * cdata)
//{
//	signed short buf;
//	buf=((cdata[0]<<8)+cdata[1])/10;
//	cdata[0]=(buf>>8);
//	cdata[1]=buf;
//}

//int temp,wet;

//时间 	空	温度	湿度	空	大气压力		空	瞬时风速	瞬时风向 1分钟平均风速	1分钟平均风向	10分钟平均风速	10分钟平均风向	10分钟最大风速	空  总辐射瞬时值	空 	 
//14		36		4			4 	4			4 				4		4						4				4字节					4字节						4字节						4字节					4字节					4字节	4字节				16字节	
//Get:			00FA 	01D5 0000 2788 			0000 	0005 		0CD2 			0003 						0C94 				0000 							0000 						0006 					0000 		0007 		
//Get:			00FA	01D2 0000	2788			0000	0000		0000			0000						06ED				0000							06BD						0001					0000		0007			
//					00FC	01F2 0000	277F			0000	0000		0BB0			0000						000000000000000100000002
//					00FC	020200002781000000010C4A0000000000000000000200000000			
//			for(i=0;i<(ulen-72)/2;i++)	printf("%02X",Get[i]);

//采样时间（年+月+日+时+分+秒）（6字节）+温度（2字节）+湿度（1字节）+瞬时风速（2字节）+瞬时风向（2字节）+雨量（2字节）+气压（2字节）+日照（2字节）+
//										1分钟平均风速（2字节）+1分钟平均风向（2字节）+10分钟平均风速（2字节）+10分钟平均风向（2字节）+10分钟最大风速。
//第一包及以后各包格式：
//与上包采样时间差（2字节）+						 温度（2字节）+湿度（1字节）+    风速（2字节）+    风向（2字节）+雨量（2字节）+气压（2字节）+日照（2字节）+
//										1分钟平均风速（2字节）+1分钟平均风向（2字节）+10分钟平均风速（2字节）+10分钟平均风向（2字节）+10分钟最大风速。
//  6+2+1+2*10 ,29
//  2+2+1+2*10 ,25

//uint8_t qx_data_buf[256];// 1+ 29+ 25*buf[0]  1024-30 /25 = 39 最多39组数据
uint8_t qx_data_buf[1024] __attribute__((at(buf_qx)));

int last_qx_time;

int get_qx_data(void) //struct qx_data *qx_d
{
	char u2_sent[10];
	unsigned char str_len;
	OS_ERR      err;
	char Get[60];//,i
	signed short buf;
	uint8_t *udata;	//读取串口数据地址
	uint16_t ulen;	//读取串口数据长度
	uint8_t *date_save_p,i;	//
	struct qx_data *qx_d;
	int time_grow;
	
	printf("\r\n采集气象数据:");
	str_len=sprintf(u2_sent,"#250ZG");//#250ZG  #030CG"  #060FG
	
//	OSTimeDly ( 200, OS_OPT_TIME_DLY, & err ); //等待串口2稳定
	UART_data_clear(&huart2);	

//	printf("\r\nGetPoint=%x,NowPoint=%x,ReadPoint=%x",huart2.GetPoint,huart2.NowPoint,huart2.ReadPoint);
	
	HAL_UART_Transmit(&huart2, (unsigned char *)u2_sent, str_len,100);//if(uart1_t[0] != NULL) 	
	
//	68 43 43 30 31 39 36 25 00 a0 31 32 33 34 3a 06 13 0b 0c 0f 01 16 02 ed 3e 00 00 01 2a 00 00 03 f2 00 00 00 00 00 00 00 00 01 2a 00 02 00 18 02 ed 3e 00 00 01 2a 00 00 03 f2 00 00 00 00 00 00 00 00 01 2a 00 01 00 18 02 ed 3e 00 00 01 2a 00 00 03 f2 00 00 00 00 00 00 00 00 00 00 00 01 00 18 02 ed 3e 00 01 01 34 00 00 03 f2 00 00 00 00 00 00 00 00 00 00 00 01 00 18 02 ed 3e 00 00 01 28 00 00 03 f2 00 00 00 00 00 00 00 00 00 00 00 01 00 18 02 ed 3e 00 00 01 28 00 00 03 f2 00 00 00 00 00 00 00 00 00 00 00 01 e0 16 
//	OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err ); 
	printf("\r\nGetPoint=%x,NowPoint=%x,ReadPoint=%x",huart2.GetPoint,huart2.NowPoint,huart2.ReadPoint);
	if(UART_Receive_s(&huart2,&udata, &ulen,2000)==2)
	{
//		udata[ulen]=0;
//		printf("\r\nudata2=%s",udata);

		if((memcmp(udata,"250Z",4)==0)&&(ulen>=120)&&(udata[ulen-1]=='G'))	//&&(memcpy(&udata[114],"GG",2)==0) 128
		{
			c8_to_u8((uint8_t *)&udata[54],(uint8_t *)Get,ulen-72); //4 +14+36+16+2
//			printf("\r\n转换数据 Get:");
			qx_data_buf[0]=0; //////for test
			
			date_save_p= &qx_data_buf[1];
			if(qx_data_buf[0]==0)
			{
				memcpy(date_save_p,systime,6);
				qx_d =(struct qx_data *)(date_save_p+6);
			}
			else if(qx_data_buf[0]<39)
			{
				time_grow=worktime-last_qx_time;
				if(time_grow>0xffff) return -1;//大于两个字节数
				
				date_save_p +=qx_data_buf[0]*25+4; //总包数 加 第一包多四个
				date_save_p[0]= (time_grow >>8);
				date_save_p[1]= time_grow ;
				qx_d =(struct qx_data *)(date_save_p+2);
			}
			last_qx_time=worktime;
			qx_data_buf[0]++;
			printf("%x",qx_data_buf[0]);
			
// 温度（2字节）+湿度（1字节）+    风速（2字节）+    风向（2字节）+雨量（2字节）+气压（2字节）+日照（2字节）+
// 1分钟平均风速（2字节）+1分钟平均风向（2字节）+10分钟平均风速（2字节）+10分钟平均风向（2字节）+10分钟最大风速。
			
//时间 	空	温度 	湿度 	空 	大气压力	 空	瞬时风速	瞬时风向 1分钟平均风速	1分钟平均风向	10分钟平均风速	10分钟平均风向	10分钟最大风速	空  总辐射瞬时值	空 	 
//14		36		4			4 	4			4 				4		4						4				4字节					4字节						4字节						4字节					4字节					4字节	4字节				16字节		
//						01		23	45		67				89	1011		1213			1415					1617						1819						2021						2223
			i=0;
			buf=((Get[0]<<8)+Get[1])+500;
			qx_d->temp[0]=(buf>>8);qx_d->temp[1]=buf; //温度+500

			buf=((Get[2]<<8)+Get[3])/10;// 湿度/10
			qx_d->hum=buf;
	//			memcpy(qx_d->hum,&Get[4],2);//空	
			
			buf=((Get[6]<<8)+Get[7])/10;
			qx_d->pre[0]=(buf>>8);qx_d->pre[1]=buf; //气压
	//			memcpy(qx_d->pre,&Get[8],2);//空	

			qx_d->winds0[0]=Get[10];qx_d->winds0[1]=Get[11]; //风速
			buf=((Get[12]<<8)+Get[13])/10;
			qx_d->windd0[0]=(buf>>8);qx_d->windd0[1]=buf; //风向
			
			qx_d->winds1[0]=Get[14];qx_d->winds1[1]=Get[15]; //风速
			buf=((Get[16]<<8)+Get[17])/10;
			qx_d->windd1[0]=(buf>>8);qx_d->windd1[1]=buf; //风向
			
			qx_d->winds10[0]=Get[18];qx_d->winds10[1]=Get[19]; //风速
			buf=((Get[20]<<8)+Get[21])/10;
			qx_d->windd10[0]=(buf>>8);qx_d->windd10[1]=buf; //风向
			
			qx_d->windm10[0]=Get[22];qx_d->windm10[1]=Get[23]; //风速
			
			qx_d->rain[0]=0;qx_d->rain[1]=0;
			qx_d->sun[0]=0;qx_d->sun[1]=0;

//			memcpy(qx_d->temp,&Get[0],2);
//			
//			buf=((Get[2]<<8)+Get[3])/10;
//			qx_d->hum=buf;//  memcpy(qx_d->hum,&Get[3],1);//只要后一字节
////			memcpy(qx_d->hum,&Get[4],2);//空
//			memcpy(qx_d->pre,&Get[6],2);
////			memcpy(qx_d->pre,&Get[8],2);//空
//			memcpy(qx_d->winds0,&Get[10],2);
//			memcpy(qx_d->windd0,&Get[12],2);
//			
//			memcpy(qx_d->winds1,&Get[14],2);
//			memcpy(qx_d->windd1,&Get[16],2);
//			
//			memcpy(qx_d->winds10,&Get[18],2);
//			memcpy(qx_d->windd10,&Get[20],2);
//			
//			memcpy(qx_d->windm10,&Get[22],2);
//			
//			qx_d->rain[0]=0;qx_d->rain[1]=0;
//			qx_d->sun[0]=0;qx_d->sun[1]=0;

////温度加500;
//			buf=(qx_d->temp[0]<<8)+qx_d->temp[1];
//			buf=buf+500;
//			qx_d->temp[0]=(buf>>8);qx_d->temp[1]=buf;

////湿度/10;
////			qx_d->hum=qx_d->hum/10; 

////风向.气压/10;
//			computer_2char_data_div10(qx_d->windd0);
//			computer_2char_data_div10(qx_d->windd1);
//			computer_2char_data_div10(qx_d->windd10);
//			computer_2char_data_div10(qx_d->pre);
			
		}
	}
	else
	{
//		printf("\r\nur2_Q time out");
	}
	return 0;
}



//int cnt_time_grow(uint8_t last_time[6])
//{
//	char now_time[6],i,datmax;
//	int time_grow;
//	
//	memcpy(now_time,systime,6);
//	
//	for(i=0;i<6;i++)
//	{
//		now_time[i]=now_time[i]-last_time[i];
//	}
//	if(now_time[5]<0) 
//	{
//		now_time[5] +=60;//秒
//		now_time[4] --;
//	}
//	if(now_time[4]<0) 
//	{
//		now_time[4] +=60;//分
//		now_time[3] --;
//	}
//	if(now_time[3]<0) 
//	{
//		now_time[3] +=24;//时
//		now_time[2] --;
//	}
//	if(now_time[2]<0) //日  -- 有不同值
//	{
//		if(systime[1]==2)
//		{
//			if(systime[0]%4!=0) now_time[2] +=28;//非润年
//			else now_time[2] +=29;//非润年
//		}
//		else if((systime[1]==2)||(systime[1]==4)||(systime[1]==6)||(systime[1]==9)||(systime[1]==11)) now_time[2] +=30;// 小月
//		else now_time[2] +=31;
//		
//		
//		now_time[1] --;
//	}
//	if(now_time[1]<0) 
//	{
//		now_time[1] +=12;//月
//		now_time[0] --;
//	}
//	
//	if(now_time[0]<0) 
//	{
//		return -1; //现在时间比上次时间小.
//	}
//	if((now_time[0])||(now_time[1]))
//	{
//		return -1; //时间跨度大于一个月.
//	}
//	
//	time_grow =(( now_time[2]*24 + now_time[3] )* 60 + now_time[4])*60+ now_time[4];
//	
//	if(time_grow>65535) return -1; //数据大于两字节.
//}
