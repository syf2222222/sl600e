
#include <bsp.h>


#define     PWREN_4G()			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_11, GPIO_PIN_SET)
#define     PWRDIS_4G()			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_11, GPIO_PIN_RESET)
#define     Wake_up_4G()			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET)



void mode_4G_init(void)
{
	RUN_LED1_ON();
	PWREN_4G();
	Wake_up_4G();
}

















