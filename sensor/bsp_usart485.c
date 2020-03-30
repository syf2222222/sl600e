#include <bsp.h>

/*
#define  AppTaskRAT_SIZE                     512
#define  AppTaskRAT_PRIO                         6
OS_TCB   AppTaskRATTCB;															//任务控制块
CPU_STK  AppTaskRATStk [ AppTaskRAT_SIZE ];	//任务堆栈
void  AppTaskRAT  ( void * p_arg );
unsigned char at_r_data[80];
*/


#define     PWREN_485()				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)
#define     PWRDIS_485()			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET)

#define     PWREN_12V3()				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_RESET)
#define     PWRDIS_12V3()			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_SET)

#define     PWREN_12V()				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_RESET)
#define     PWRDIS_12V()			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_SET)




void mode_485_init(void)
{
	PWREN_12V();
	PWREN_485();
	PWREN_12V3();
}








