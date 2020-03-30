

//“DT”指令，在控制台中，正确显示系统当前时间。
//“SIG”指令，显示GPRS的信号强度(0~31)。
//“RB”指令，终端重启。
//“FE”指令，低级格式化SD卡，清除图片索引，更新文件系统
//“SS”指令，显示系统运行状态信息。
//“VER”指令，显示主板的固件版本信息。

//“CONFIGDTU”指令，例如按照“SERVERIP= 183.63.92.43”和“SERVERPORT=12677”指令设置的服务器IP和端口号重新配置DTU，需要重启生效。
//“RSDTU”指令，显示DTU复位信息，并断电重启动DTU。
//“FDS”指令，终端立即恢复出厂参数。

#include "work.h"
#include "FM25V10.h"

void ur_ctler(char *rdata);
int PTZCtrl_presets(char chanle,char presetN);
uint8_t TC_C=0;

void ur_test(uint8_t *rev_data)
{
	
}

void TC(uint8_t *rev_data,uint8_t rlen)
{
	uint8_t buf;		//rlen,
	int i;
	
//	printf("\r\n长度 %x",rlen);
//	rlen  =strlen((char *)rev_data);
//	printf("\r\n长度 %x",rlen);
//	printf(":%x,%x",rev_data[0],rev_data[1]);
	
	if(TC_C==1)	//系统调试串口
	{
		printf("\r\nTC to ur1 %s",rev_data);
	}
	else	if(TC_C==2)//485串口 加 \r\n
	{
		strcat((char *)rev_data,"\r\n");
		
		HAL_UART_Transmit_IT(&huart2, rev_data, strlen((char *)rev_data));
		printf("\r\nTC to ur2 %s",rev_data);
	}
	else	if(TC_C==3)//SIM7600 4G 数据串口
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
	else	if(TC_C==5)//SIM7600 4G 指令串口
	{
		strcat((char *)rev_data,"\r\n");
		HAL_UART_Transmit_IT(&huart5, rev_data, strlen((char *)rev_data));//, 300
		printf("\r\nTC to ur5 %s",rev_data);
	}
	else if(TC_C==6)//测试
	{
		printf("\r\nTC to ur6 %s",rev_data);
		ur_test(rev_data);
	}
	else//	if(TC_C==4)//没开
	{
		printf("\r\nTC to unkuow");
	}
}


void rev_ur1_data(uint8_t *rev_data,uint8_t len)
{
	rev_data[len]=0;
	
	printf("\r\nlen=%d",len);
	printf("\r\nrev_data=%s",rev_data);
	
		if((strncmp((char *)rev_data, "uart", 4)==0)&&(len>=5))//串口工具不勾选发送新行
		{
			TC_C=rev_data[4]-'0';
			if(len>6) len=len-6;
			else len=0;
			TC(&rev_data[6],len);
		}
		else if(TC_C !=0)
		{
			TC(rev_data,len);
		}
		else
		{
			ur_ctler((char *)rev_data);
		}
}

//extern int c8_to_d8(const uint8_t *in, uint8_t *out,uint8_t len);
//extern int c8_to_u32_d(const uint8_t *in,uint8_t len);
void data_rst(void);
void data_read(void);
void check_signel(void);
int GET_Image_s(void);
	
void ur_ctler(char *rdata) //rdata 结尾 13 10
{
	char str[]="DT",i,j,len,lenout,cmdsiz;
	uint32_t buf32;
	uint8_t buf8[5];
	
//	rdata[2]=0;
//	printf("%d,%d,%d,%d",rdata[0],rdata[1],rdata[2],rdata[3]);
	
	for(i=0;i<100;i++)
	{
		if((unsigned char)rdata[i]<32)
		{
			rdata[i]=0; //屏蔽结尾的奇怪符号
			len=i;
			i=100;
		}
		else if((rdata[i]>=0x61)&&(rdata[i]<=0x7a))
		{
			rdata[i] -=32;
		}
	}
	
	if (0 == strcmp("GET", rdata))
	{
		printf("\r\n准备GET_Image");
//		GET_Image_s();
//		PTZCtrl_presets(1,1);
	}
	if (0 == strcmp("DT", rdata))//if((strncmp("DT", rdata, 2)==0)&&(strlen(rdata)<=4))  //if (0 == strcmp(str, rdata))
	{
		printf("\r\n20%d年 %d月 %d日 %d:%d:%d",systime[0],systime[1],systime[2],systime[3],systime[4],systime[5]);
	}
	if (0 == strcmp("SIG", rdata))
	{
		check_signel();
		
		printf("\r\n信号强度:%d",signal);
	}
	if (0 == strcmp("RB", rdata))
	{
		printf("\r\n准备复位");
		HAL_NVIC_SystemReset();
	}
	if (0 == strcmp("FE", rdata))
	{
		printf("\r\n准备格式化sd(未完成)");
	}
	if (0 == strcmp("VER", rdata))
	{
//		printf("\r\nver:%x.%x",version[0],version[1]);
	}
	if (0 == strcmp("FDS", rdata))
	{
//		data_rst();
//		data_read();
	}
	
	if (0 == strncmp("T=", rdata,2))
	{
		cmdsiz=2;
		lenout=(len-cmdsiz)/2+(len-cmdsiz)%2;
		if(lenout != 6)
		{
			printf("command err");
		}
		else	if(c8_to_d8((uint8_t *)&rdata[cmdsiz], (uint8_t *)rdata,(len-cmdsiz))==0)
		{
			for(i=0;i<6;i++)
			{
				systime[i]=rdata[i];
			}
			printf("systime=");
			for(i=0;i<6;i++) printf("%d,",systime[i]);
		}
		else
		{
			printf("\r\ncommand err");
		}
	}
	if (0 == strncmp("MRT=", rdata,4))
	{
		cmdsiz=4;
		buf32=c8_to_u32_d((uint8_t *)&rdata[cmdsiz],(len-cmdsiz));//strlen(&rdata[cmdsiz])
		if(buf32<=255)
		{
			*OLT=buf32;
			flash_save((uint8_t *)OLT,sizeof(*OLT));	//flash_save(OLT);
			
			printf("\r\nonline time=%d",*OLT);
		}
		else printf("\r\ncommand err");
	}
	if (0 == strncmp("PT=", rdata,11))
	{
	}
	if (0 == strncmp("NETBOARDIP=", rdata,11))
	{
		cmdsiz=11;
		lenout=cmdsiz;
		j=0;
		for(i=cmdsiz;i<len;i++)
		{
			if(i==len-1) i=len; //结尾
			if((rdata[i]=='.')||(i==len))
			{
				buf32=c8_to_u32_d((uint8_t *)&rdata[lenout],(i-lenout));
				if(buf32<=255)
				{
					buf8[j++]=buf32;
					lenout=i+1;
					if(j>=5) 
					{
						printf("command err");
						break;
					}
				}
				else
				{
					printf("\r\ncommand err");
					break;
				}
			}
		}
		if(j==4) 
		{
			for(i=0;i<4;i++)	LoclIP[i]=buf8[i];
			
			flash_save(LoclIP,4);
			
			printf("\r\nLoclIP=%d.%d.%d.%d",LoclIP[0],LoclIP[1],LoclIP[2],LoclIP[3]);
		}
		else printf("\r\ncommand err");
	}
	if (0 == strncmp("CAMERAIP=", rdata,11))
	{
		
	}

}



