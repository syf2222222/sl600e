
#include <bsp.h>
#include "bsp_uart.h"
#include "global.h"

extern DMA_HandleTypeDef hdma_usart3_rx;

//unsigned char urd1[lenmax1*2],urd2[lenmax2*2],urd3[lenmax3*2],urd5[lenmax5*2];
//unsigned char urd3[lenmax3*2];

#if 0
uint8_t recv_end_flag=0;
uint16_t rx_len;

uint8_t aaa[10];

void UART3_check_dma(void)
{
	uint32_t tmp_flag = 0;
	uint32_t temp;
	
	return;
	
	tmp_flag =__HAL_UART_GET_FLAG(&huart3,UART_FLAG_IDLE);   //获取IDLE标志位
	if((tmp_flag != RESET))   //idle标志被置位
	{ 
			__HAL_UART_CLEAR_IDLEFLAG(&huart3);   //清除标志位
			temp = huart3.Instance->ISR; //清除状态寄存器SR（F0的HAL库USART_TypeDef结构体中名字为ISR：USART Interrupt and status register），读取SR可以清楚该寄存器
			temp = huart3.Instance->RDR; //读取数据寄存器中的数据,读取DR（F0中为RDR：USART Receive Data register）
			HAL_UART_DMAStop(&huart3); 
			temp  = hdma_usart3_rx.Instance->CNDTR; //获取DMA中未传输的数据个数，NDTR寄存器分析见下面
			rx_len =	lenmax3 - temp; //总计数减去未传输的数据个数，得到已经接收的数据个数
			recv_end_flag = 1;    //接受完成标志位置1    
	 }
}
#endif

void UART_MspInit(void)
{
	HAL_UART_Receive_IT(&huart1,(unsigned char *)urd1,lenmax1);
	HAL_UART_Receive_IT(&huart2,(unsigned char *)urd2,lenmax2);
	HAL_UART_Receive_IT(&huart5,(unsigned char *)urd5,lenmax5);
	
#ifdef	ur3_rdma
	//	__HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);        //使能idle中断
	HAL_UART_Receive_DMA(&huart3,(unsigned char *)urd3,lenmax3); //打开DMA接收，数据存入rx_buffer数组中。
#else	
	HAL_UART_Receive_IT(&huart3,(unsigned char *)urd3,lenmax3);
#endif
	

}


void UART_data_clear(UART_HandleTypeDef *huart)
{
	huart->GetPoint=huart->NowPoint;
	huart->ReadPoint=huart->NowPoint;
}

int UART_Receive(UART_HandleTypeDef *huart,uint8_t **pack_point, uint16_t *pack_len,int wait_time)
{
	uint16_t i;
	OS_ERR      err;
	
	if(huart->Instance->ISR & (uint32_t)(USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE)) //有错误?
	{
	}
	if(((huart->Instance->CR1)&(USART_CR1_PEIE | USART_CR1_RXNEIE)) != (USART_CR1_PEIE | USART_CR1_RXNEIE)) //检测到已关闭,重启
	{
//		printf("\r\n------------------------terrorflags=%x",terrorflags);
//		terrorflags=0;
//		printf("\r\nCR1=%04x",huart3.Instance->CR1);
		HAL_UART_Abort_IT(huart);
		if(huart==&huart1) HAL_UART_Receive_IT(huart,(unsigned char *)urd1,lenmax1);
		if(huart==&huart2) HAL_UART_Receive_IT(huart,(unsigned char *)urd2,lenmax2);
		if(huart==&huart3) HAL_UART_Receive_IT(huart,(unsigned char *)urd3,lenmax3);
		if(huart==&huart5) HAL_UART_Receive_IT(huart,(unsigned char *)urd5,lenmax5);
//		printf("\r\nuart3错误重启");
	}
	

	if(huart->GetPoint == huart->NowPoint) return 0; //没数据
	
	if(huart->ReadPoint != huart->NowPoint)
	{
		huart->ReadPoint=huart->NowPoint;
		if(wait_time==0) return 1;	//没收完一包

		OSTimeDly ( wait_time, OS_OPT_TIME_DLY, & err ); 
		if(huart->ReadPoint != huart->NowPoint)	
		{
			//		printf("\r\n没收完一包");
			return 1;	//没收完一包
		}
	}
	
	if(huart->ReadPoint > huart->GetPoint)
	{
		*pack_len=huart->ReadPoint - huart->GetPoint;
	}
	else
	{
		for(i=0;i < huart->ReadPoint ; i++)
		{
			huart->pRxBuffPtr[huart->RxXferSize+i] = huart->pRxBuffPtr[i];
		}
		*pack_len = huart->ReadPoint + huart->RxXferSize - huart->GetPoint;
	}
	*pack_point = & (huart->pRxBuffPtr[huart->GetPoint]);
	
	huart->GetPoint = huart->ReadPoint;
	return 2;
}

int UART_Receive_3(UART_HandleTypeDef *huart,uint8_t **pack_point, uint16_t *pack_len,int wait_time)
{
	OS_ERR      err;
	uint16_t i,temp;
	
	huart->NowPoint = hdma_usart3_rx.Instance->CNDTR; //获取DMA中未传输的数据个数，NDTR寄存器分析见下面
//	SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);
	
	if((huart->Instance->CR3 & USART_CR3_DMAR) !=USART_CR3_DMAR)	//没开启
	{
		HAL_UART_DMAStop(&huart3); 
		HAL_UART_Receive_DMA(&huart3,(unsigned char *)urd3,lenmax3); //打开DMA接收，数据存入rx_buffer数组中。
		
		temp = huart3.Instance->ISR; //清除状态寄存器SR（F0的HAL库USART_TypeDef结构体中名字为ISR：USART Interrupt and status register），读取SR可以清楚该寄存器
		temp = huart3.Instance->RDR; //读取数据寄存器中的数据,读取DR（
	}
	if(huart->NowPoint==lenmax3)	//连续无数据
	{
//		huart->GetPoint++;
//		if(huart->GetPoint>=1000)
//		{
//			HAL_UART_DMAStop(&huart3); 
//			HAL_UART_Receive_DMA(&huart3,(unsigned char *)urd3,lenmax3); //打开DMA接收，数据存入rx_buffer数组中。
//			
//			huart->GetPoint = huart3.Instance->ISR; //清除状态寄存器SR（F0的HAL库USART_TypeDef结构体中名字为ISR：USART Interrupt and status register），读取SR可以清楚该寄存器
//			huart->GetPoint = huart3.Instance->RDR; //读取数据寄存器中的数据,读取DR（
//			huart->GetPoint=0;
//		}
		return 0; //没数据
	}
	
	
	if(huart->ReadPoint != huart->NowPoint)
	{
		huart->ReadPoint=huart->NowPoint;
		if(wait_time==0) return 1;	//没收完一包

		OSTimeDly ( wait_time, OS_OPT_TIME_DLY, & err ); 
		
		huart->NowPoint = hdma_usart3_rx.Instance->CNDTR;
				
		if(huart->ReadPoint != huart->NowPoint)	
		{
			//		printf("\r\n没收完一包");
			return 1;	//没收完一包
		}
	}
//	temp = huart3.Instance->ISR; //清除状态寄存器SR（F0的HAL库USART_TypeDef结构体中名字为ISR：USART Interrupt and status register），读取SR可以清楚该寄存器
//	temp = huart3.Instance->RDR; //读取数据寄存器中的数据,读取DR（F0中为RDR：USART Receive Data register）
	HAL_UART_DMAStop(&huart3); 

	for(i=0;i<lenmax3;i++) urd3[i]=urd3[lenmax3+i];
	
	*pack_len =	lenmax3 - huart->ReadPoint; //总计数减去未传输的数据个数，得到已经接收的数据个数
	*pack_point=urd3+lenmax3;
	
	huart->NowPoint=*pack_len;
	huart->ReadPoint=*pack_len;	

	HAL_UART_Receive_DMA(&huart3,(unsigned char *)(&urd3[lenmax3]),lenmax3); //打开DMA接收，数据存入rx_buffer数组中。
	return 2;
}

int UART_Receive_s(UART_HandleTypeDef *huart,uint8_t **pack_point, uint16_t *pack_len,int time_out)
{

	OS_ERR      err;
	int Result=0;
	


	while((Result!=2)&&(time_out>=0))
	{
#ifdef	ur3_rdma
		if(huart==&huart3) 
		{
			Result = UART_Receive_3(huart,pack_point, pack_len,0); 
			huart->GetPoint=huart->NowPoint;
		}
		else	
#endif
		Result = UART_Receive(huart,pack_point, pack_len,0); 
		
		if(Result !=2)
		{
			OSTimeDly ( 10, OS_OPT_TIME_DLY, & err ); 
			time_out -= 10;
		}
//		printf("\r\ntime_out=%d",time_out);
	}
	
	
	
	return Result;
}

#if 1
///重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
	
	HAL_UART_Transmit(&huart1,(uint8_t *) &ch, 1, 1000);
	return (ch);
}
#endif
/*
int fgetc(FILE *f) {  
  return (PutCh(GetCh()));  
}*/

#if 0
typedef struct __UART_HandleTypeDef
{
  USART_TypeDef            *Instance;                /*!< UART registers base address        */

  UART_InitTypeDef         Init;                     /*!< UART communication parameters      */

  UART_AdvFeatureInitTypeDef AdvancedInit;           /*!< UART Advanced Features initialization parameters */

  uint8_t                  *pTxBuffPtr;              /*!< Pointer to UART Tx transfer Buffer */

  uint16_t                 TxXferSize;               /*!< UART Tx Transfer size              */

  __IO uint16_t            TxXferCount;              /*!< UART Tx Transfer Counter           */

  uint8_t                  *pRxBuffPtr;              /*!< Pointer to UART Rx transfer Buffer */

  uint16_t                 RxXferSize;               /*!< UART Rx Transfer size              */

  __IO uint16_t            RxXferCount;              /*!< UART Rx Transfer Counter           */

  uint16_t                 Mask;                     /*!< UART Rx RDR register mask          */
	
	uint16_t                 GetPoint;          
	uint16_t                 NowPoint;      
	uint16_t                 ReadPoint;      


  void (*RxISR)(struct __UART_HandleTypeDef *huart); /*!< Function pointer on Rx IRQ handler   */

  void (*TxISR)(struct __UART_HandleTypeDef *huart); /*!< Function pointer on Tx IRQ handler   */

  DMA_HandleTypeDef        *hdmatx;                  /*!< UART Tx DMA Handle parameters      */

  DMA_HandleTypeDef        *hdmarx;                  /*!< UART Rx DMA Handle parameters      */

  HAL_LockTypeDef           Lock;                    /*!< Locking object                     */

  __IO HAL_UART_StateTypeDef    gState;              /*!< UART state information related to global Handle management
                                                          and also related to Tx operations.
                                                          This parameter can be a value of @ref HAL_UART_StateTypeDef */

  __IO HAL_UART_StateTypeDef    RxState;             /*!< UART state information related to Rx operations.
                                                          This parameter can be a value of @ref HAL_UART_StateTypeDef */

  __IO uint32_t                 ErrorCode;           /*!< UART Error code                    */


} UART_HandleTypeDef;

void UART_RxISR_8BIT(UART_HandleTypeDef *huart)
{
  uint16_t uhMask = huart->Mask;
  uint16_t  uhdata;

  if (huart->RxState == HAL_UART_STATE_BUSY_RX)
  {
		uhdata = (uint16_t) READ_REG(huart->Instance->RDR);
		huart->pRxBuffPtr[huart->NowPoint++] = (uint8_t)(uhdata & (uint8_t)uhMask);
		if(huart->NowPoint >= huart->RxXferSize) huart->NowPoint=0;
  }
  else
  {
    /* Clear RXNE interrupt flag */
    __HAL_UART_SEND_REQ(huart, UART_RXDATA_FLUSH_REQUEST);
  }
}

#endif












