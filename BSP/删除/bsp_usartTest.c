
#include <bsp.h>

//extern UART_HandleTypeDef huart1;
//extern UART_HandleTypeDef huart1;
//unsigned char RX_Buf[500];


/*
void USART2_IRQHandler(void)                    
{ 
        u8 Res;        if(USART1->SR & (1<<6))//如果发送完成,置标志位
        {
            USART1->SR &= ~(1<<6);//清发送完成中标志
        }   
        if(USART1->SR & (1<<3))//如果上溢错误
        {
            USART1->SR &= ~(1<<3);
        }           
        if(USART2->SR & (1<<5))//如果接受完成
        {       
            Res = USART2->DR;   //读取数据寄存器,清SR中断标志
            USART2->SR &= ~(1<<5);//清RXNE标志    
            if((USART2_RX_STA & 0xfff) == 13)  //接收帧尾
            {
                ++USART2_RX_STA;
                USART2_RX_BUF[USART2_RX_STA & 0xfff] = Res; //接收校验位
                ////收到一帧，进行CRC校验，并执行，然后回复
                if(!pc_cmd_crc())
                {                    if(USART2_RX_BUF[2]==0xff || USART2_RX_BUF[2]==GD_BUF[INDEX_ID]) //是否是正确的ID
                    {                        if(run_pc_cmd())//如果是正确的指令,则用往DMA传数据
                        pc_cmd_reply();     
                    }
                }
                USART2_RX_STA = 0;
            }            else if(USART2_RX_STA & 0x4000) //接收一帧数据的第3个----第9个数据
            {
                ++USART2_RX_STA;
                USART2_RX_BUF[USART2_RX_STA & 0xfff] = Res;
            }            else if(USART2_RX_STA & 0x8000) //接收一帧数据的第二个数据 ---0xfe
            {                if(Res != 0xfe)
                USART2_RX_STA = 0;                else
                {
                    USART2_RX_BUF[(++USART2_RX_STA)&0xfff] = Res;
                    USART2_RX_STA |= 0x4000;
                }
            }            else if(Res == 0xef)  //接收一帧数据的第一个数据 ---0xef
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
                                     //进入中断
	
	HAL_UART_IRQHandler(&huart1);
	ucCNT++;
	if(ucCNT>=6)
	{
		ucCNT=0;
		OSTaskSemPost((OS_TCB  *)&AppTaskUsartTCB,   //目标任务
									(OS_OPT   )OS_OPT_POST_NONE, //没选项要求
									(OS_ERR  *)&err);            //返回错误类型		
	}

	


#if 0	
	
	/* 从内存分区 mem 获取一个内存块 */
	p_mem_blk = OSMemGet((OS_MEM      *)&mem,
										   (OS_ERR      *)&err);
	
//	if ( USART_GetITStatus ( macUSARTx, USART_IT_RXNE ) != RESET )
//	if ( HAL_UART_Receive_IT(&huart1, p_mem_blk, 100) != HAL_ERROR )	 //  ==HAL_OK //获取接收到的数据
//	if ( HAL_UART_Receive(&huart1, p_mem_blk, 1, 1000) != HAL_ERROR )	 //  ==HAL_OK //获取接收到的数据	
	{
			 p_mem_blk =ucRdata;
		
    /* 发布任务消息到任务 AppTaskUsart */
		OSTaskQPost ((OS_TCB      *)&AppTaskUsartTCB,      //目标任务的控制块
								 (void        *)p_mem_blk,             //消息内容的首地址
								 (OS_MSG_SIZE  )10,                     //消息长度
								 (OS_OPT       )OS_OPT_POST_FIFO,      //发布到任务消息队列的入口端
								 (OS_ERR      *)&err);                 //返回错误类型
		
//		EXTI_ClearITPendingBit(macEXTI_LINE);      //清除中断标志位										 
	}
#endif
	OSIntExit();	                                       //退出中断
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}
#endif

#if 0
void macUSART_INT_FUN(void)
{
	OS_ERR   err;
	
	unsigned 	char *   p_mem_blk;
	
	
	OSIntEnter(); 	                                     //进入中断
	
	/* 从内存分区 mem 获取一个内存块 */
	p_mem_blk = OSMemGet((OS_MEM      *)&mem,
										   (OS_ERR      *)&err);
	
//	if ( USART_GetITStatus ( macUSARTx, USART_IT_RXNE ) != RESET )
//	if ( HAL_UART_Receive_IT(&huart1, p_mem_blk, 100) != HAL_ERROR )	 //  ==HAL_OK //获取接收到的数据
//	if ( HAL_UART_Receive(&huart1, p_mem_blk, 1, 1000) != HAL_ERROR )	 //  ==HAL_OK //获取接收到的数据	
	
	{
			p_mem_blk=huart1.pRxBuffPtr;
    /* 发布任务消息到任务 AppTaskUsart */
		OSTaskQPost ((OS_TCB      *)&AppTaskUsartTCB,      //目标任务的控制块
								 (void        *)p_mem_blk,             //消息内容的首地址
								 (OS_MSG_SIZE  )1,                     //消息长度
								 (OS_OPT       )OS_OPT_POST_FIFO,      //发布到任务消息队列的入口端
								 (OS_ERR      *)&err);                 //返回错误类型
		
//		EXTI_ClearITPendingBit(macEXTI_LINE);      //清除中断标志位						 
	}
	
	OSIntExit();	                                       //退出中断
	
}
#endif




#if 0
///重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
		/* 等待串口输入数据 */
		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(DEBUG_USARTx);
}
#endif























