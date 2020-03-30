
#include "main.h"
#include "fatfs.h"
#include <stdio.h>
#include <includes.h>
#include "global.h" 

void MX_LL_DATA_Init(void)
{
	char i;
	for(i=0;i<3;i++)
	{
		hll[i].sav_add=flash_index_cnt_star+4*i;
		
		ReadFM25Operation(hll[i].sav_add,(uint8_t *)(&hll[i]),4);	//读取存储数据
	}
}

void MX_QX_DATA_Init(void)
{
	hqx.sav_add=flash_index_cnt_star+12;
	
	ReadFM25Operation(hqx.sav_add,(uint8_t *)(&hqx),4);	//读取存储数据

}

void MX_PH_DATA_Init(void)
{
	hph.sav_add=flash_index_cnt_star+16;
	
	ReadFM25Operation(hph.sav_add,(uint8_t *)(&hph),4);	//读取存储数据
}





//int gain_index_all(void)




