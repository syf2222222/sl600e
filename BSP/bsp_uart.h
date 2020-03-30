
#include <main.h>			//包含那些HAL功能

#ifndef  BSP_UART
#define  BSP_UART
//#include "stm32l4xx_hal_def.h"

#define		lenmax1		256
#define		lenmax2		256
#define		lenmax3		1024
#define		lenmax5		256

//extern unsigned char urd3[lenmax3*2];
//extern unsigned char urd1[lenmax1*2],urd2[lenmax2*2],urd3[lenmax3*2],urd5[lenmax5*2];
//extern unsigned int point1,point2,point3,point5;
//extern unsigned int point1_last,point2_last,point3_last,point5_last;

int UART_Receive_s(UART_HandleTypeDef *huart,uint8_t **pack_point, uint16_t *pack_len,int time_out);


#endif  /* BSP_UART */




