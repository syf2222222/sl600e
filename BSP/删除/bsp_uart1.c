
#include <bsp.h>

//��DT��ָ��ڿ���̨�У���ȷ��ʾϵͳ��ǰʱ�䡣
//��SIG��ָ���ʾGPRS���ź�ǿ��(0~31)��
//��RB��ָ��ն�������
//��FE��ָ��ͼ���ʽ��SD�������ͼƬ�����������ļ�ϵͳ
//��SS��ָ���ʾϵͳ����״̬��Ϣ��
//��VER��ָ���ʾ����Ĺ̼��汾��Ϣ��

//��CONFIGDTU��ָ����簴�ա�SERVERIP= 183.63.92.43���͡�SERVERPORT=12677��ָ�����õķ�����IP�Ͷ˿ں���������DTU����Ҫ������Ч��
//��RSDTU��ָ���ʾDTU��λ��Ϣ�����ϵ�������DTU��
//��FDS��ָ��ն������ָ�����������

extern unsigned char signal,volt;
extern unsigned char version[2];


void rev_ur1_data(uint8_t *rev_data,uint16_t len);


unsigned char rec_ur1;
uint8_t uart1_d[2][100];

void  AppTaskUr1 ( void * p_arg )
{
	OS_ERR      err; 
	uint8_t t=0,lt,i;
	
	unsigned char rec_ur1_thisT;
//	unsigned int dlong; 
	for(i=0;i<72;i++)
	{
		uart1_d[0][i]=0;
		uart1_d[1][i]=0;
	}
	HAL_UART_Receive_IT(&huart1,(unsigned char *)uart1_d[t],90);
	
	while (DEF_TRUE)
	{
		if(rec_ur1)
		{	
			rec_ur1_thisT=rec_ur1;
			OSTimeDly ( 3, OS_OPT_TIME_DLY, & err ); 
			if(rec_ur1_thisT == rec_ur1)
			{
				HAL_UART_AbortReceive_IT(&huart1);
				lt=t;
				t=((t+1)&0x01);
				HAL_UART_Receive_IT(&huart1,(unsigned char *)uart1_d[t],90);
				rec_ur1=0;
				rev_ur1_data(uart1_d[lt],90);
				for(i=0;i<100;i++) uart1_d[lt][i]=0;
			}
		}
		else
		{
			OSTimeDly ( 50, OS_OPT_TIME_DLY, & err );	
		}

	}
}

void USER_USART1_IRQHandler(void)  //������֤ void LPUART1_IRQHandler(void)
{
//  OS_ERR   err;
//  CPU_SR_ALLOC();                                
	
#if 1	
//	rec_ur1=1;
	if (((READ_REG(huart1.Instance->ISR) & USART_ISR_RXNE) != 0U)
        && ((READ_REG(huart1.Instance->CR1) & USART_CR1_RXNEIE) != 0U))
	{
		rec_ur1++;
/*		
		OSSemPost((OS_SEM  *)&SemOfur1,                                  //����SemOfKey
										(OS_OPT   )OS_OPT_POST_ALL,                            //���������еȴ�����
										(OS_ERR  *)&err);                                      //���ش�������
*/		
	}
#endif	
	
  
}

unsigned char rec_ur2;
uint8_t uart2_d[2][64];

void  AppTaskUr2 ( void * p_arg )
{
	OS_ERR      err; 
	uint8_t t=0,lt,i;
	
	unsigned char rec_ur2_thisT;
//	unsigned int dlong; 
	for(i=0;i<72;i++)
	{
		uart2_d[0][i]=0;
		uart2_d[1][i]=0;
	}
	HAL_UART_Receive_IT(&huart2,(unsigned char *)uart2_d[t],60);
	
	while (DEF_TRUE)
	{
		if(rec_ur2)
		{	
			rec_ur2_thisT=rec_ur2;
			OSTimeDly ( 10, OS_OPT_TIME_DLY, & err ); 
			if(rec_ur2_thisT == rec_ur2)
			{
				HAL_UART_AbortReceive_IT(&huart2);
				lt=t;
				t=((t+1)&0x01);
				HAL_UART_Receive_IT(&huart2,(unsigned char *)uart2_d[t],60);
				rec_ur2=0;
//				rev_ur2_data(uart2_d[lt],60);
				strcat((char *)uart2_d[lt], "-2");
				HAL_UART_Transmit_IT(&huart1, uart2_d[lt], strlen((char *)uart2_d[lt])); 
				
				for(i=0;i<60;i++) uart2_d[lt][i]=0;
			}
		}
		else
		{
			OSTimeDly ( 50, OS_OPT_TIME_DLY, & err );	
		}

	}
}

void USER_USART2_IRQHandler(void)
{
//OS_ERR   err;
#if 1
//	RUN_LED1_Toggle();
	if (((READ_REG(huart2.Instance->ISR) & USART_ISR_RXNE) != 0U)
        && ((READ_REG(huart2.Instance->CR1) & USART_CR1_RXNEIE) != 0U))
	{
		rec_ur2++;
/*		
		OSSemPost((OS_SEM  *)&SemOfur2,                                  //����SemOfur2
										(OS_OPT   )OS_OPT_POST_ALL,                            //���������еȴ�����
										(OS_ERR  *)&err);                                      //���ش�������
*/		
	}
#endif	
}
#if 0
void  AppTaskUr3 ( void * p_arg )
{
	OS_ERR      err; 
	
	
	HAL_UART_Receive_IT(&huart3,(unsigned char *)uart3_r,64);
	while (DEF_TRUE)
	{
		if(rec_ur3==0)
			{
				OSSemPend ((OS_SEM   *)&SemOfur3,               		//�ȴ����ź��� SemOfur1
								       (OS_TICK   )0,                    		//û�յ��źţ������ڵȵ�
								       (OS_OPT    )OS_OPT_PEND_BLOCKING,		//
								       (CPU_TS   *)0,                       //����ȡʱ���
								       (OS_ERR   *)&err);                   //���ش�������
				
//				rec_ur3=1;
			}	
			else
			{
				OSSemPend ((OS_SEM   *)&SemOfur3,               		//�ȴ����ź��� SemOfur1
								       (OS_TICK   )50,                      //10ms��ʱ��˵����ur�ѷ���
								       (OS_OPT    )OS_OPT_PEND_BLOCKING,		//
								       (CPU_TS   *)0,                       //����ȡʱ���
								       (OS_ERR   *)&err);                   //���ش�������

//				if(err == OS_ERR_NONE) rec_ur3++;
				if(err != OS_ERR_NONE) 
				{
					char i;
						HAL_UART_AbortReceive_IT(&huart3);					
					
//						uart_buf[0]  =0;
						for(i=0;i<72;i++)
						{
							uart3_t[i] = uart3_r[i];
							uart3_r[i]=0;
						}
//						uart_buf[0] = (0xff-uart_buf[0]);
//						uart1_t[rec_ur1]=uart_buf[0];
//						uart1_t[rec_ur1+1]=0x16;
						
//					strcpy((char *)uart3_t,(char *)uart3_r);
//					HAL_UART_AbortReceive_IT(&huart3);		
		
					HAL_UART_Receive_IT(&huart3,(unsigned char *)uart3_r,64);

//					strcpy((char *)uart_buf,"-3");
//					strcat((char *)uart3_t, (char *)uart_buf);
					HAL_UART_Transmit_IT(&huart1, uart3_t, rec_ur3); //͸����uart1
					
					rec_ur3=0;			
				}
			}
	}
}
#endif














