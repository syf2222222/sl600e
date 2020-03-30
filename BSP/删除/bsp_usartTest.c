
#include <bsp.h>

//extern UART_HandleTypeDef huart1;
//extern UART_HandleTypeDef huart1;
//unsigned char RX_Buf[500];


/*
void USART2_IRQHandler(void)                    
{ 
        u8 Res;        if(USART1->SR & (1<<6))//����������,�ñ�־λ
        {
            USART1->SR &= ~(1<<6);//�巢������б�־
        }   
        if(USART1->SR & (1<<3))//����������
        {
            USART1->SR &= ~(1<<3);
        }           
        if(USART2->SR & (1<<5))//����������
        {       
            Res = USART2->DR;   //��ȡ���ݼĴ���,��SR�жϱ�־
            USART2->SR &= ~(1<<5);//��RXNE��־    
            if((USART2_RX_STA & 0xfff) == 13)  //����֡β
            {
                ++USART2_RX_STA;
                USART2_RX_BUF[USART2_RX_STA & 0xfff] = Res; //����У��λ
                ////�յ�һ֡������CRCУ�飬��ִ�У�Ȼ��ظ�
                if(!pc_cmd_crc())
                {                    if(USART2_RX_BUF[2]==0xff || USART2_RX_BUF[2]==GD_BUF[INDEX_ID]) //�Ƿ�����ȷ��ID
                    {                        if(run_pc_cmd())//�������ȷ��ָ��,������DMA������
                        pc_cmd_reply();     
                    }
                }
                USART2_RX_STA = 0;
            }            else if(USART2_RX_STA & 0x4000) //����һ֡���ݵĵ�3��----��9������
            {
                ++USART2_RX_STA;
                USART2_RX_BUF[USART2_RX_STA & 0xfff] = Res;
            }            else if(USART2_RX_STA & 0x8000) //����һ֡���ݵĵڶ������� ---0xfe
            {                if(Res != 0xfe)
                USART2_RX_STA = 0;                else
                {
                    USART2_RX_BUF[(++USART2_RX_STA)&0xfff] = Res;
                    USART2_RX_STA |= 0x4000;
                }
            }            else if(Res == 0xef)  //����һ֡���ݵĵ�һ������ ---0xef
            {
                USART2_RX_BUF[USART2_RX_STA & 0xfff] = Res;
                USART2_RX_STA |= 0x8000;
            }                   
        }   
} 
*/

//extern unsigned char ucRdata[20];
//extern unsigned char ucCNT;
#if 0
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */

  OS_ERR   err;
	
//	unsigned 	char *   p_mem_blk;
	
	
	OSIntEnter(); 	
                                     //�����ж�
	
	HAL_UART_IRQHandler(&huart1);
	ucCNT++;
	if(ucCNT>=6)
	{
		ucCNT=0;
		OSTaskSemPost((OS_TCB  *)&AppTaskUsartTCB,   //Ŀ������
									(OS_OPT   )OS_OPT_POST_NONE, //ûѡ��Ҫ��
									(OS_ERR  *)&err);            //���ش�������		
	}

	


#if 0	
	
	/* ���ڴ���� mem ��ȡһ���ڴ�� */
	p_mem_blk = OSMemGet((OS_MEM      *)&mem,
										   (OS_ERR      *)&err);
	
//	if ( USART_GetITStatus ( macUSARTx, USART_IT_RXNE ) != RESET )
//	if ( HAL_UART_Receive_IT(&huart1, p_mem_blk, 100) != HAL_ERROR )	 //  ==HAL_OK //��ȡ���յ�������
//	if ( HAL_UART_Receive(&huart1, p_mem_blk, 1, 1000) != HAL_ERROR )	 //  ==HAL_OK //��ȡ���յ�������	
	{
			 p_mem_blk =ucRdata;
		
    /* ����������Ϣ������ AppTaskUsart */
		OSTaskQPost ((OS_TCB      *)&AppTaskUsartTCB,      //Ŀ������Ŀ��ƿ�
								 (void        *)p_mem_blk,             //��Ϣ���ݵ��׵�ַ
								 (OS_MSG_SIZE  )10,                     //��Ϣ����
								 (OS_OPT       )OS_OPT_POST_FIFO,      //������������Ϣ���е���ڶ�
								 (OS_ERR      *)&err);                 //���ش�������
		
//		EXTI_ClearITPendingBit(macEXTI_LINE);      //����жϱ�־λ										 
	}
#endif
	OSIntExit();	                                       //�˳��ж�
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}
#endif

#if 0
void macUSART_INT_FUN(void)
{
	OS_ERR   err;
	
	unsigned 	char *   p_mem_blk;
	
	
	OSIntEnter(); 	                                     //�����ж�
	
	/* ���ڴ���� mem ��ȡһ���ڴ�� */
	p_mem_blk = OSMemGet((OS_MEM      *)&mem,
										   (OS_ERR      *)&err);
	
//	if ( USART_GetITStatus ( macUSARTx, USART_IT_RXNE ) != RESET )
//	if ( HAL_UART_Receive_IT(&huart1, p_mem_blk, 100) != HAL_ERROR )	 //  ==HAL_OK //��ȡ���յ�������
//	if ( HAL_UART_Receive(&huart1, p_mem_blk, 1, 1000) != HAL_ERROR )	 //  ==HAL_OK //��ȡ���յ�������	
	
	{
			p_mem_blk=huart1.pRxBuffPtr;
    /* ����������Ϣ������ AppTaskUsart */
		OSTaskQPost ((OS_TCB      *)&AppTaskUsartTCB,      //Ŀ������Ŀ��ƿ�
								 (void        *)p_mem_blk,             //��Ϣ���ݵ��׵�ַ
								 (OS_MSG_SIZE  )1,                     //��Ϣ����
								 (OS_OPT       )OS_OPT_POST_FIFO,      //������������Ϣ���е���ڶ�
								 (OS_ERR      *)&err);                 //���ش�������
		
//		EXTI_ClearITPendingBit(macEXTI_LINE);      //����жϱ�־λ						 
	}
	
	OSIntExit();	                                       //�˳��ж�
	
}
#endif




#if 0
///�ض���c�⺯��scanf�����ڣ���д����ʹ��scanf��getchar�Ⱥ���
int fgetc(FILE *f)
{
		/* �ȴ������������� */
		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(DEBUG_USARTx);
}
#endif























