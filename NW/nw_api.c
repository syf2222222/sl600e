

#include "bsp.h"

#include "nw.h"
#include "global.h"

unsigned char sent[100];//={0x68,0x43,0x43,0x30,0x31,0x39,0x36}
extern uint32_t LastTxT;
unsigned short sentdelay=0;


unsigned char checksum_4g(unsigned char* a,unsigned int n)
{
	unsigned char checksum;
	unsigned int i;
	
	checksum=0;
	for(i=0;i<n;i++)
	{
		checksum += a[i];
	}
	checksum=~checksum;
	return checksum;	
}

void  nw_sent_8(uint8_t *t_data,uint16_t len) //unsigned char command,unsigned char * t_data,unsigned int dlong
{
//	unsigned char i;
	OS_ERR      err; 
	if(sentdelay)
	{
		OSTimeDly( sentdelay*100, OS_OPT_TIME_DLY, & err);
		sentdelay=0;
	}
	CPU_SR_ALLOC();
	
	OS_CRITICAL_ENTER();                             //进入临界段
	HAL_UART_Transmit(&huart3, t_data, len,100);

	LastTxT=runningtime;
	printf("\r\nM->S %x,%d:%d:%d",t_data[7],systime[3],systime[4],systime[5]);
	sentdelay=delayT;
//	for(i=0;i<len;i++)	printf(" %02x",t_data[i]);
	OS_CRITICAL_EXIT();                          //临界段

}
void  nw_sent_st(struct nw_ctl *t_data) //unsigned char command,unsigned char * t_data,unsigned int dlong
{
	unsigned char i;
	OS_ERR      err; 
	if(sentdelay)
	{
		OSTimeDly( sentdelay*100, OS_OPT_TIME_DLY, & err);
		sentdelay=0;
	}
//	CPU_SR_ALLOC();
	
	memcpy(sent,STAA, 7);
	sent[7]=t_data->cmd;
	sent[8]=(t_data->len>>8);
	sent[9]=t_data->len;
	
	for(i=0;i<t_data->len;i++)
	{
		sent[10+i]=t_data->data[i];
	}
	sent[10+t_data->len]=checksum_4g(&sent[1],9+t_data->len);
	sent[11+t_data->len]=0x16;
	LastTxT=runningtime;
	nw_sent_8(sent,(12+t_data->len));
	
}
void rev_err(unsigned char ctl,unsigned char data)
{
	CPU_SR_ALLOC();
	OS_ERR      err; 
	if(sentdelay)
	{
		OSTimeDly( sentdelay*100, OS_OPT_TIME_DLY, & err);
		sentdelay=0;
	}
	
	memcpy(sent,STAA, 7);
	sent[7]=ctl;
	sent[8]=0;
	sent[9]=2;
	sent[10]=data;
	sent[11]=data;
	sent[12]=checksum_4g(&sent[1],11);
	sent[13]=0x16;
		
	OS_CRITICAL_ENTER();                             //进入临界段
	HAL_UART_Transmit(&huart3, sent, (14),100);
	sentdelay=delayT;
	LastTxT=runningtime;
	printf("\r\ne-m %x,%d:%d:%d",ctl,systime[3],systime[4],systime[5]);
	OS_CRITICAL_EXIT();                          //临界段
}



void  nw_sent_88(uint8_t *t_data,uint8_t ch) //unsigned char command,unsigned char * t_data,unsigned int dlong
{
//	unsigned char i;
//	CPU_SR_ALLOC();
	OS_ERR      err; 
	if(sentdelay)
	{
		OSTimeDly( sentdelay*100, OS_OPT_TIME_DLY, & err);
		sentdelay=0;
	}
	
	memcpy(sent,STAA, 7);
	sent[7]=0x88; //指令
	sent[8]=0;
	sent[9]=7;	//长度7
	memcpy(&sent[10],PSW, 4);
	
	sent[14]=ch; //通道号
	sent[15]=t_data[0];
	sent[16]=t_data[1];//
	
	sent[17]=checksum_4g(&sent[1],16);
	sent[18]=0x16;
//	LastTxT=worktime;
	nw_sent_8(sent,19);
}

void  nw_sent_ph_time(uint8_t ch) //unsigned char command,unsigned char * t_data,unsigned int dlong
{
	CPU_SR_ALLOC();
	
	unsigned char checksum;
	unsigned char *flash_ptr;
	unsigned short flash_len,i;
	
	OS_ERR      err; 
	if(sentdelay)
	{
		OSTimeDly( sentdelay*100, OS_OPT_TIME_DLY, & err);
		sentdelay=0;
	}
	
	if(ch==1)
	{
		flash_read(aut_point_group1,aut_point_group1,1024);
		flash_ptr=aut_point_group1;
	}
	else if(ch==2)
	{
		flash_read(aut_point_group2,aut_point_group2,1024);
		flash_ptr=aut_point_group2;
	}
	else
	{
		printf("\r\n通道错误");
		return;
	}
	
	flash_len=flash_ptr[0]*3+1;
//	printf("\r\nflash_len=%d",flash_len);
	
//密码	通道号	组数	第1组	第2组	第3组	…
//			时	分	预置位号	时	分	预置位号	时	分	预置
//位号	
//4字节	1字节	1字节	1字节	1字节	1字节	1字节	1字节	1字节	1字节	1字节	1字节	
		
	memcpy(sent,STAA, 7);
	sent[7]=0x8b;//t_data->cmd;
	sent[8]=((flash_len+1)>>8); // +1 通道号
	sent[9]=(flash_len+1);
		
	sent[10]=ch; //通道号
	
	checksum=0;
	for(i=1;i<=10;i++)
	{
		checksum += sent[i];
	}
	for(i=0;i<flash_len;i++) //组数+拍照组数据
	{
		checksum += flash_ptr[i];
	}
	checksum=~checksum;
	
	sent[14]=checksum;
	sent[15]=0x16;
	
//	printf("3");
	OS_CRITICAL_ENTER();                             //进入临界段
	HAL_UART_Transmit(&huart3, sent,11,100);
	HAL_UART_Transmit(&huart3, flash_ptr,flash_len,100);
	HAL_UART_Transmit(&huart3, &sent[14],2,100);
	sentdelay=delayT;
	LastTxT=runningtime;
	printf("\r\nM->S %x,%d:%d:%d",sent[7],systime[3],systime[4],systime[5]);
	OS_CRITICAL_EXIT();                          //临界段	
}

extern unsigned short tt_pk_nb;
//															发的数据,			 数据长度,   通道,      预置位,         第几包
void  nw_sent_ph(unsigned char *input,uint16_t len,uint8_t ch,uint8_t preset,uint16_t peket) 
{
	unsigned char checksum;
	unsigned short datalen;
	unsigned int i;
	CPU_SR_ALLOC();
	OS_ERR      err; 
	if(sentdelay)
	{
		OSTimeDly( sentdelay*100, OS_OPT_TIME_DLY, & err);
		sentdelay=0;
	}
//	printf("\r\n发照片数据%d,长%d\r\n",peket,len);
	memcpy(sent,STAA, 7);
	sent[7]=0x85;//t_data->cmd;
	datalen=len+4;
	sent[8]=(datalen>>8);
	sent[9]=datalen;
	

	sent[10]=ch;						//通道号
	sent[11]=preset;						//预置位号
	sent[12]=(peket>>8);	//包数高位
	sent[13]=peket;				//包数低位	
	
//	printf("2");
	checksum=0;
	for(i=1;i<=13;i++)
	{
		checksum += sent[i];
	}
	for(i=0;i<len;i++)
	{
		checksum += input[i];
	}
	checksum=~checksum;
	
	sent[14]=checksum;
	sent[15]=0x16;
	
//	printf("3");
	OS_CRITICAL_ENTER();                             //进入临界段
	HAL_UART_Transmit(&huart3, sent,14,100);
	HAL_UART_Transmit(&huart3, input,len,100);
	HAL_UART_Transmit(&huart3, &sent[14],2,100);
	LastTxT=runningtime;
	if(peket==1)	
	{
		printf("\r\nM->S %x,peket=%d,len=%d",sent[7],peket,len);//,data=
		printf("\r\n");
//		for(i=0;i<5;i++) printf("%02x",input[i]);
	}
	else if(peket==tt_pk_nb)// if(len<nw_pk_len)//
	{
		printf("\r\nM->S %x,peket=%d,len=%d",sent[7],peket,len);
//		printf("\r\n");
//		for(i=len-5;i<len;i++) printf("%02x",input[i]); 
	}
	else printf(".");
	
//	printf(",data=");
//	for(i=0;i<5;i++) printf("%02x",input[i]); printf("..."); for(i=len-5;i<len;i++) printf("%02x",input[i]); 
	OS_CRITICAL_EXIT();                          //临界段
}



void sent_ll_data_22(unsigned char *input,unsigned char zbs)
{
	int peket_len;

	unsigned char checksum;
	unsigned short datalen;
	unsigned int i;
	CPU_SR_ALLOC();
	OS_ERR      err; 
	
	
	if(input[0]==0)
	{
		printf("\r\n没有数据");
		return;
	}
	if(sentdelay)
	{
		OSTimeDly( sentdelay*100, OS_OPT_TIME_DLY, & err);
		sentdelay=0;
	}
//	printf("\r\n包数1=%x",input[0]);
//	input[0]=1;
	memcpy(sent,STAA, 7);
	sent[7]=0x22;//t_data->cmd;
	
	peket_len=input[0]*26+6;
	
	datalen=peket_len+5;
//	printf("\r\ninput[0]=%d,peket_len=%d",input[0],peket_len);
	sent[8]=(datalen>>8);
	sent[9]=datalen;
	
	memcpy(&sent[10],PSW,4);
	sent[14]=zbs;//帧标识
	

	checksum=0;
	for(i=1;i<=14;i++)
	{
		checksum += sent[i];
	}
	for(i=0;i<peket_len;i++)
	{
		checksum += input[i];
	}
	checksum=~checksum;
	
	sent[15]=checksum;
	sent[16]=0x16;
	
//	printf("3");
	OS_CRITICAL_ENTER();                             //进入临界段
	HAL_UART_Transmit(&huart3, sent,15,100);
	HAL_UART_Transmit(&huart3, input,peket_len,100);
	HAL_UART_Transmit(&huart3, &sent[15],2,100);
	sentdelay=delayT;
	LastTxT=runningtime;
//	printf()
	printf("\r\nM->S %x,%d:%d:%d",sent[7],systime[3],systime[4],systime[5]);
//	for(i=0;i<15;i++)	printf(" %02x",sent[i]);
//	for(i=0;i<peket_len;i++)	printf(" %02x",input[i]);
//	for(i=15;i<17;i++)	printf(" %02x",sent[i]);
	
//	68 434330313936
//	22 0733 132333472616161f1600000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
	OS_CRITICAL_EXIT();                          //临界段
	
	senttask_Pend |= data_flag22;
}

void sent_qx_data_25(unsigned char *input,unsigned char zbs)
{
	int peket_len;

	unsigned char checksum;
	unsigned short datalen;
	unsigned int i;
	CPU_SR_ALLOC();
	OS_ERR      err; 
	
	if(input[0]==0)
	{
		printf("\r\n没有数据");
		return;
	}
	
	if(sentdelay)
	{
		OSTimeDly( sentdelay*100, OS_OPT_TIME_DLY, & err);
		sentdelay=0;
	}
	
	memcpy(sent,STAA, 7);
	sent[7]=0x25;//t_data->cmd;
	
	peket_len=input[0]*25+5;
	
	datalen=peket_len+5;
	sent[8]=(datalen>>8);
	sent[9]=datalen;
	
	memcpy(&sent[10],PSW,4);
	sent[14]=zbs;//帧标识
	

	checksum=0;
	for(i=1;i<=14;i++)
	{
		checksum += sent[i];
	}
	for(i=0;i<peket_len;i++)
	{
//		sent[15+i]=input[i];
		checksum += input[i];
	}
	checksum=~checksum;
	
	sent[15]=checksum;
	sent[16]=0x16;
	
//	printf("3");
	OS_CRITICAL_ENTER();                             //进入临界段
	HAL_UART_Transmit(&huart3, sent,15,100);
	HAL_UART_Transmit(&huart3, input,peket_len,100);
	HAL_UART_Transmit(&huart3, &sent[15],2,100);
	
//	HAL_UART_Transmit(&huart3, sent,peket_len+17,100);
	sentdelay=delayT;
	LastTxT=runningtime;
	printf("\r\nM->S %x,%d:%d:%d",sent[7],systime[3],systime[4],systime[5]);
	
//	for(i=0;i<15;i++)	printf(" %02x",sent[i]);
//	for(i=0;i<peket_len;i++)	printf(" %02x",input[i]);
//	for(i=15;i<17;i++)	printf(" %02x",sent[i]);
	OS_CRITICAL_EXIT();                          //临界段
	
	senttask_Pend |= data_flag25;

}


