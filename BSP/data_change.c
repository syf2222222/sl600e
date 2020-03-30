
#include "stdint.h"
#include <stdio.h>
#include <string.h>
#define NULL 0

//static uint8_t num_to_char(uint8_t ch, uint8_t lower_upper)
uint8_t u4_to_c8(uint8_t ch, uint8_t lower_upper)	//16进制转字符，4位到8位，lower_upper定大小写
{
	if (ch <= 9) 								ch += '0';					//数字
	else if (0 == lower_upper)	ch += 'a' - 0x0A;		//输出 a-f 小写字母
	else												ch += 'A' - 0x0A;		//输出 a-f 大写字母

	return ch;
}

//static void byte16_to_char32(const uint8_t *in, uint8_t *out, uint8_t lower_upper)
void u8_to_c8(const uint8_t *in, uint8_t *out,uint8_t len, uint8_t lower_upper) //16进制转字符，8位数组到8位数组，len为输入长度
{
	uint8_t i;

	if ((NULL == in)|| (NULL == out)) return;

	for (i = 0; i < len; i++)
	{
		out[2 * i] = u4_to_c8(in[i] >> 4, lower_upper);
		out[2 * i + 1] = u4_to_c8(in[i] & 0x0F, lower_upper);
	}
}

uint8_t c8_to_u4(uint8_t ch)	//
{
	
	if((ch>='0')&&(ch<='9')) ch =ch-'0';
	else	if((ch>='a')&&(ch<='f')) ch =ch-'a'+10;
	else	if((ch>='A')&&(ch<='F')) ch =ch-'A'+10;
	
	else return 0xff;
	
	return ch;
}
int c8_to_u8(const uint8_t *in, uint8_t *out,uint8_t len) //字符转16进制，8位数组到8位数组，len为输入长度
{
	uint8_t i,j,buf;
	if ((NULL == in)|| (NULL == out)) return -1;

	j=0;
//	printf("\r\n");
	for (i = 0; i < len; i++)
	{
//		printf("%c:",in[i]);
		buf=c8_to_u4(in[i]);
		if(buf>0x0f) return -2;
//		printf("%x.",buf);
		if((i & 0x01)==0) out[j]=(buf<<4);
		else out[j++] += buf;
	}
	out[j]=0;
	
	return 0;
}

uint8_t c8_to_d4(uint8_t ch)	
{
	if((ch>='0')&&(ch<='9')) ch =ch-'0';
	else ch=100;//
	
	return ch;
}

int c8_to_d8(const uint8_t *in, uint8_t *out,uint8_t len) //转南网的年月日数据
{
	uint8_t i,j=0,flag=0;

	if ((NULL == in)|| (NULL == out)) return -1;
	
	flag=(len&0x01);
	j=0;
	if(flag) out[j++] = (c8_to_d4(in[0]));
	for (i = flag; i < len; i+=2)
	{
		flag=c8_to_d4(in[i]);
		if(flag>9) return -1;
		out[j] = flag*10;
		flag=c8_to_d4(in[i+1]);
		if(flag>9) return -1;
		out[j++] += flag;
	}	
	return 0;
}
int c8_to_u32_d(const uint8_t *in,uint8_t len)
{
	uint8_t i,buf=0;//j=0,
	uint32_t out;

	out=0;
	for(i=0;i<len;i++)
	{
		if(in[i]!=' ') 
		{
			out *=10;
			if(out>0x0fffffff) return -1;
			buf=c8_to_d4(in[i]);
			if(buf>9) return -1;
			out += buf;
		}
	}
	return out;
}

int c8_to_u32_x(const uint8_t *in,uint8_t len)
{
	uint8_t i,buf=0;//j=0,
	uint32_t out;

	out=0;
	for(i=0;i<len;i++)
	{
		if(in[i]!=' ') 
		{
			out =(out<<4); //*=16;
			if(out>0x0fffffff) return -1;
			buf=c8_to_u4(in[i]);
			if(buf>0x0f) return -1;
			out += buf;
		}
	}
	return out;
}

//int c16_to_u32(const uint8_t *in,uint8_t len)
//{
//	uint8_t i,flag=0;//j=0,
//	uint32_t out;

//	out=0;
//	for(i=0;i<len;i++)
//	{
//		if(in[i]!=' ') 
//		{
//			out *=10;
//			if(out>0x0fffffff) return -1;
//			flag=c8_to_d4(in[i]);
//			if(flag>9) return -1;
//			out += flag;
//		}
//	}
//	return out;
//}
