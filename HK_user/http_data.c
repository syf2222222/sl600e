
#include <stdio.h>
#include <string.h>
#include "md5.h"
#include "lwip/api.h"


extern const char algorithm[9];
extern char qop[9];

extern const char username[10];
extern char realm[];//"DS-2DF5130W-D";//
extern const char password[20];

extern char nonce[50];//"596a686c5a6a6c6d4e4455365a6a42684d7a4d315a57553d";//
extern int nc_d;
//char nc[9]="00000001";
extern char cnonce[33];
extern void rand_num_create_str32(uint8_t *out);

//char method[10];
//char uri[120];
//char ip_host[20];//="192.168.1.11";
//char strA[100]="\0";
//char body[500]="\0";



int get_info(char *data,char *key,char **end)
{
	int i,sta=0,data_len=0,in_len;//j=0,k,
	char *in;
	
	in=*end;
	in_len=strlen(in);
	if((in_len==0)||(in_len>2000))
	{
		return -1;
	}
	
	for(i=0;i<in_len;i++)
	{
		if(sta==0)
		{
			if(strncmp(&in[i],key,strlen(key))==0) sta=1;
		}
		else	if(sta==1)
		{
			if((in[i]=='\"')||(in[i]==':')||(in[i]=='>')) sta=2;
		}
		else	if(sta==2)
		{
			if((in[i]=='\"')||(in[i]=='\r')||(in[i]=='\n')||(in[i]=='<'))//||(in[i]==',')
			{
				sta=3;
				break;
			}
			else
			{
				data[data_len++]=in[i];
				if(data_len>48) return -1;
			}
		}
	}
		
	*end=*end+i;//产生数据结尾
	data[data_len]=0;
	
	if(sta !=3) return -1;
	return 0;
}	
					
void get_digest_info(char *in)
{
	char *end;
	end=in;
//	printf("\r\nend=%x",end); //end没变
	
	get_info(qop,"qop",&end);
//	printf("\r\nend=%x",end);//打印地址
	
	get_info(realm,"realm",&end);
//	printf("\r\nend=%x",end);
	
	get_info(nonce,"nonce",&end);
//	printf("\r\nend=%x",end);
	
}

//extern int c8_to_u32_d(const uint8_t *in,uint8_t len);
//extern unsigned char bLevel,cLevel,sLevel;
	
int Content_Length(char **in)
{
	char *end;
	char Lendata[10];
	end=*in;
	if(get_info(Lendata,"Content-Length",&end)==0) *in=end;
	
	return c8_to_u32_d((uint8_t *)Lendata,strlen(Lendata));
	
}
//char chanle,signed char *brightnessLevel,signed char *contrastLevel,signed char *saturationLevel

int get_color_info(char *in)
{
	char *end;
	char Lendata[4];
	int getdata;
	
	end=in;
	if(get_info(Lendata,"brightnessLevel",&end)!=0)
	{
		printf("\r\n没找到 brightnessLevel的值");
		return -1;
	}
	getdata=c8_to_u32_d((uint8_t *)Lendata,strlen(Lendata));
	if(getdata>100)
	{
		printf("\r\nbrightnessLevel数值错误");
		return -1;
	}
	*bLevel=getdata;

	
	if(get_info(Lendata,"contrastLevel",&end)!=0)
	{
		printf("\r\n没找到 contrastLevel的值");
		return -1;
	}
		
	getdata=c8_to_u32_d((uint8_t *)Lendata,strlen(Lendata));
	if(getdata>100)
	{
		printf("\r\ncontrastLevel的值数值错误");
		return -1;
	}
	*cLevel=getdata;
	
	if(get_info(Lendata,"saturationLevel",&end)!=0)
	{
		printf("\r\n没找到 saturationLevel的值");
		return -1;
	}
	
	getdata=c8_to_u32_d((uint8_t *)Lendata,strlen(Lendata));
	if(getdata>100)
	{
		printf("\r\nsaturationLevel的值数值错误");
		return -1;
	}
	*sLevel=getdata;
	
	
	return 0;
}
//videoResolutionWidth
//extern unsigned char WidthxHeight;
extern const uint16_t Width[21];//= {0,320,640,704,800,1024,1280,1280,1920,960,1280,1600,2048,2592,2592,3072,3840,4000,4608,3200,4224};
extern const uint16_t Height[21];//={0,240,480,576,600,768, 1024,720, 1080,576,960, 1200,1536,1520,1944,2048,2160,3000,3456,2400,3136};
	
int get_Width_Height_info(char *in)
{
	char *end;
	char Lendata[6],hWidthxHeight;
	int hWidth,hHeight,i;
	
	end=in;
	if(get_info(Lendata,"videoResolutionWidth",&end)!=0)
	{
		printf("\r\n没找到 videoResolutionWidth的值");
		return -1;
	}
	
	hWidth=c8_to_u32_d((uint8_t *)Lendata,strlen(Lendata));
	
	if(get_info(Lendata,"videoResolutionHeight",&end)!=0)
	{
		printf("\r\n没找到 videoResolutionHeight的值");
		return -1;
	}
	
	hHeight=c8_to_u32_d((uint8_t *)Lendata,strlen(Lendata));
	
	hWidthxHeight=0;
	for(i=0;i<=21;i++)
	{
		if((hWidth==Width[i])&&(hHeight==Height[i]))
		{
			hWidthxHeight=i;
			break;
		}
	}

	if(hWidthxHeight==0)
	{
		printf("\r\n没找到WidthxHeight对应的表");
		return -1;
	}
	*WidthxHeight=hWidthxHeight;
	return 0;
}


