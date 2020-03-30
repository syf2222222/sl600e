
#include "bsp.h"


extern unsigned char volt;



int Get_Volt(void)
{
	uint32_t adc_val=0,adc_cnt=0,batV;
	uint8_t t,times=10;
	OS_ERR      err;
	
	for(t=0;t<times;t++)
	{
		HAL_ADC_Start(&hadc3);
		HAL_ADC_PollForConversion(&hadc3, 10);
		adc_val=HAL_ADC_GetValue(&hadc3);
		adc_cnt+=adc_val;		
//		printf("\r\nbat_adc=%d",adc_val);
		OSTimeDly ( 5, OS_OPT_TIME_DLY, & err );
	}
//	batV=adc_cnt/times /4096*33*11;//
	batV=(adc_cnt *363+512*times)/1024/times;
	
//	printf("\r\nADC=%d",(adc_cnt+times/2)/times);
//	printf("\r\nbatV=%d",batV);
	
	if((batV!=0)&&(batV<255))
	{
//		printf("\r\nbatV=%d",batV);
		volt=batV;
		return 0;
	}
	else
	{
		return -1;
	}
	
}



void  ADC_test  ( void * p_arg )
{
	OS_ERR      err;

	(void)p_arg;
	
	
	while (DEF_TRUE)
	{
		volt=Get_Volt();
		printf("\r\nbatV=%d",volt);
		
		OSTimeDly ( 2000, OS_OPT_TIME_DLY, & err );

	}
			
}



