/****************************************Copyright (c)****************************************************
**
**                                 http://www.
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               .c
** Last modified Date:      2018-02-07
** Last Version:            1.0.0
** Descriptions:
**
**--------------------------------------------------------------------------------------------------------
** Created by:              feiyun.wu
** Created date:            2018-02-07
** Version:                 1.0.0
** Descriptions:            源文件
**
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Descriptions:
**
*********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>


const char Base[64] = {
'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
};

void base64_change(unsigned char *in_data,unsigned char * out_data, unsigned int length )
{
	int i,j;
//	unsigned char *p;
//	wData[0]=173;
//	wData[1]=186;
//	wData[2]=118;
	j=0;
	for(i=0;i<(length/3);i++)
	{
		out_data[j++]=Base[((in_data[i*3+0]>>2) & 0x3f)															];//i*4+0
		out_data[j++]=Base[((in_data[i*3+1]>>4) & 0x0f)|((in_data[i*3+0]<<4) & 0x30)];//i*4+1
		out_data[j++]=Base[((in_data[i*3+2]>>6) & 0x03)|((in_data[i*3+1]<<2) & 0x3c)];//i*4+2
		out_data[j++]=Base[															((in_data[i*3+2]<<0) & 0x3f)];//i*4+3
	}
	if((length%3)==1)
	{
		out_data[j++]=Base[((in_data[i*3+0]>>2) & 0x3f)															];
		out_data[j++]=Base[															((in_data[i*3+0]<<4) & 0x30)];
		out_data[j++]='=';
		out_data[j++]='=';
	}
	else if((length%3)==2)
	{
		out_data[j++]=Base[((in_data[i*3+0]>>2) & 0x3f)															];
		out_data[j++]=Base[((in_data[i*3+1]>>4) & 0x0f)|((in_data[i*3+0]<<4) & 0x30)];
		out_data[j++]=Base[															((in_data[i*3+1]<<2) & 0x3c)];
		out_data[j++]='=';
	}
	out_data[j++]=0;//结束符？
} 

/*********************************************************************************************************
END FILE
*********************************************************************************************************/
