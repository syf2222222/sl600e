
#include "bsp.h"
#include "nw.h"

void reset_STM32(void)
{
	printf("\r\n准备复位系统.");
	HAL_NVIC_SystemReset();
}
void wakeup_dev(void)
{
}
void dev_sleep(void)
{
}


void check_function(void)
{
	char i;
		for(i=0;i<strlen((char *)function_buf);i++)
		{
			if(function_buf[i]==0x22)//	导地线拉力及倾角监测功能
			{
			}
			if(function_buf[i]==0x24)//	绝缘子泄漏电流监测功能
			{
			}
			if(function_buf[i]==0x25)//	气象数据监测功能
			{
			}
			if(function_buf[i]==0x26)//	导线温度、电流数据监测功能
			{
			}
			if(function_buf[i]==0x27)//	杆塔振动数据监测功能
			{
			}
			if(function_buf[i]==0x28)//	导线侧倾角监测功能
			{
			}
			if(function_buf[i]==0x29)//	舞动振幅频率监测功能
			{
			}
			if(function_buf[i]==0x2A)//	杆塔倾斜数据监测功能
			{
			}
			if(function_buf[i]==0x2B)//	导线微风震动数据监测功能
			{
			}
			if(function_buf[i]==0x2C)//	综合防盗功能
			{
			}
			if(function_buf[i]==0x2D)//	山火报警功能
			{
			}
			if(function_buf[i]==0x2E)//	大风舞动报警功能
			{
			}
			if(function_buf[i]==0x30)//	设备故障自检功能
			{
			}
			if(function_buf[i]==0x32)//	微风振动动态数据监测功能
			{
			}
			if(function_buf[i]==0x36)//	舞动动态数据监测功能
			{
			}
			if(function_buf[i]==0x41)//	污秽数据监测功能
			{
			}
			if(function_buf[i]==0x42)//	导线弧垂数据监测功能
			{
			}
			if(function_buf[i]==0x43)//	电缆温度数据监测功能
			{
			}
			if(function_buf[i]==0x44)//	电缆护层接地环流数据监测功能
			{
			}
			if(function_buf[i]==0x45)//	电缆故障定位数据监测功能
			{
			}
			if(function_buf[i]==0x46)//	电缆故障局放数据监测功能
			{
			}
			if(function_buf[i]==0x47)//	电缆局放谱图数据传输功能
			{
			}
			if(function_buf[i]==0x73)//	文件传输功能
			{
			}
			if(function_buf[i]==0x84)//	图像监测功能
			{
			}
			if(function_buf[i]==0x85)//	视频监测功能
			{
			}
		}
}
