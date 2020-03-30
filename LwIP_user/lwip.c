/**
 ******************************************************************************
  * File Name          : LWIP.c
  * Description        : This file provides initialization code for LWIP
  *                      middleWare.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
	
//һ�����͵� LWIP Ӧ��ϵͳ�����������������̣�
//�������������ϲ�Ӧ�ó�����̣�
//Ȼ���� LWIP Э��ջ���̣�
//����ǵײ�Ӳ�����ݰ����շ��ͽ��̡�
  
/* Includes ------------------------------------------------------------------*/
#include "lwip.h"
//#include "lwip/init.h"
//#include "lwip/netif.h"
//#include "lwip/tcpip.h"

#if defined ( __CC_ARM )  /* MDK ARM Compiler */
#include "lwip/sio.h"



#endif /* MDK ARM Compiler */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/* Private function prototypes -----------------------------------------------*/
/* ETH Variables initialization ----------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/* Variables Initialization */
struct netif gnetif;
ip4_addr_t ipaddr;
ip4_addr_t netmask;
ip4_addr_t gw;

/* USER CODE BEGIN 2 */

//void tcpip_init(tcpip_init_done_fn initfunc, void *arg);
/* USER CODE END 2 */

/**
  * LwIP initialization function
  */
//typedef void (*tcpip_init_done_fn)(void *arg);
//err_t tcpip_input(struct pbuf *p, struct netif *inp);
//void tcpip_init(tcpip_init_done_fn initfunc, void *arg);
	
void MX_LWIP_Init(void)
{
  /* Initilialize the LwIP stack with RTOS */
//	printf("\r\nMX_LWIP_Init");
	
  tcpip_init( NULL, NULL ); //�˴����� LWIP Э��ջ����

  /* IP addresses initialization with DHCP (IPv4) */
	IP4_ADDR(&ipaddr,LoclIP[0],LoclIP[1],LoclIP[2],LoclIP[3]);				//ip��ַ
	IP4_ADDR(&netmask,NETMASK_ADDR0,NETMASK_ADDR1,NETMASK_ADDR2,NETMASK_ADDR3);// ��������(����)
	IP4_ADDR(&gw,GWIP[0],GWIP[1],GWIP[2],GWIP[3]);	//����
	
  /* add the network interface (IPv4/IPv6) with RTOS */
  netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input); //ethernet_input  //&tcpip_input

  /* Registers the default network interface */
  netif_set_default(&gnetif);

  if (netif_is_link_up(&gnetif))
  {
    /* When the netif is fully configured this function must be called */
    netif_set_up(&gnetif);
  }
  else
  {
    /* When the netif link is down this function must be called */
    netif_set_down(&gnetif);
  }

  /* Start DHCP negotiation for a network interface (IPv4) */
  dhcp_start(&gnetif);

/* USER CODE BEGIN 3 */

/* USER CODE END 3 */
}

//dhcp_cleanup(struct netif *netif)
//netif_remove(struct netif *netif)

void close_hk()
{
	
	netif_set_down(&gnetif);
}

void open_hk()
{
	netif_set_up(&gnetif);
}

#ifdef USE_OBSOLETE_USER_CODE_SECTION_4
/* Kept to help code migration. (See new 4_1, 4_2... sections) */
/* Avoid to use this user section which will become obsolete. */
/* USER CODE BEGIN 4 */
/* USER CODE END 4 */
#endif

#if defined ( __CC_ARM )  /* MDK ARM Compiler */
/**
 * Opens a serial device for communication.
 *
 * @param devnum device number
 * @return handle to serial device if successful, NULL otherwise
 */
sio_fd_t sio_open(u8_t devnum)
{
  sio_fd_t sd;

/* USER CODE BEGIN 7 */
  sd = 0; // dummy code
/* USER CODE END 7 */
	
  return sd;
}

/**
 * Sends a single character to the serial device.
 *
 * @param c character to send
 * @param fd serial device handle
 *
 * @note This function will block until the character can be sent.
 */
void sio_send(u8_t c, sio_fd_t fd)
{
/* USER CODE BEGIN 8 */
/* USER CODE END 8 */
}

/**
 * Reads from the serial device.
 *
 * @param fd serial device handle
 * @param data pointer to data buffer for receiving
 * @param len maximum length (in bytes) of data to receive
 * @return number of bytes actually received - may be 0 if aborted by sio_read_abort
 *
 * @note This function will block until data can be received. The blocking
 * can be cancelled by calling sio_read_abort().
 */
u32_t sio_read(sio_fd_t fd, u8_t *data, u32_t len)
{
  u32_t recved_bytes;

/* USER CODE BEGIN 9 */
  recved_bytes = 0; // dummy code
/* USER CODE END 9 */	
  return recved_bytes;
}

/**
 * Tries to read from the serial device. Same as sio_read but returns
 * immediately if no data is available and never blocks.
 *
 * @param fd serial device handle
 * @param data pointer to data buffer for receiving
 * @param len maximum length (in bytes) of data to receive
 * @return number of bytes actually received
 */
u32_t sio_tryread(sio_fd_t fd, u8_t *data, u32_t len)
{
  u32_t recved_bytes;

/* USER CODE BEGIN 10 */
  recved_bytes = 0; // dummy code
/* USER CODE END 10 */	
  return recved_bytes;
}


OS_MUTEX dm9000_sem_lock;
OS_SEM dm9k_input_sem;

//#define  dm9k_in_check_SIZE                     50
//#define  dm9k_in_check_PRIO                        3
//OS_TCB   dm9k_in_check_TCB;															//������ƿ�
//CPU_STK  dm9k_in_check_Stk [ dm9k_in_check_SIZE ];	//�����ջ

//extern uint8_t check_rx(void);

//void dm9k_in_check(void * p_arg)
//{
//	OS_ERR      err;
//	int time;
//	OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err ); 

//	while(1)
//	{
//		OSTimeDly ( 3, OS_OPT_TIME_DLY, & err ); 

////						�����ź���ָ��	//��ʱʱ�䣨���ģ� //ѡ��	 //ʱ���-����ȡ //���ش�������
////		OSMutexPend (&dm9000_sem_lock, 0, 	OS_OPT_PEND_BLOCKING, 0, &err); 
////		printf("Task1");
//		
//		time++;
//		if(check_rx()||(time>=500))
//		{
//			time=0;
//	
//			//��ֵ�ź������ƿ�ָ��	//ѡ��	 //���ش�������
//			OSSemPost (&dm9k_input_sem, OS_OPT_POST_FIFO, &err);    //������յ�����֡ 
//		}
//		
////						�����ź���ָ��	 	//ѡ��	 			//���ش�������	
////		OSMutexPost (&dm9000_sem_lock, OS_OPT_POST_NONE, &err); 
//	}
//}

//dm9000_sem_lock
//��������LWIPЭ��ջ MX_LWIP_Init�ﴴ��
//���������� MX_LWIP_Init�ﴴ�� Create_ethernetif_input(void)
//Ӧ������Ӧ�ò㴴��
//��������ɨ������

void LwipTaskStart (void *p_arg)
{
	OS_ERR      err;

//		��ֵ�ź������ƿ�ָ��		//��ֵ�ź������� //��Դ��Ŀ���¼��Ƿ�����־ //���ش�������
	OSSemCreate (&dm9k_input_sem, "dm9k_input_sem", 3, &err); //3 	
	
//	OSMutexCreate (OS_MUTEX  *p_mutex, //�����ź���ָ��
//                     CPU_CHAR  *p_name,  //ȡ�ź���������
//                     OS_ERR    *p_err)   //���ش�������
	
	OSMutexCreate (&dm9000_sem_lock, //�����ź���ָ��
                 "dm9000_sem_lock",  //ȡ�ź���������
                  &err);   //���ش�������
		
////						�����ź���ָ��	//��ʱʱ�䣨���ģ� //ѡ��	 //ʱ���-����ȡ //���ش�������
//	OSMutexPend (&dm9000_sem_lock, 0, 	OS_OPT_PEND_BLOCKING, 0, &err); 
////						�����ź���ָ��	 	//ѡ��	 			//���ش�������	
//	OSMutexPost (&dm9000_sem_lock, OS_OPT_POST_1, &err); 
									 

	MX_LWIP_Init();

#if 0
		/* ���� ���� */
    OSTaskCreate((OS_TCB     *)&dm9k_in_check_TCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"dm9k_in_check",                             //��������
                 (OS_TASK_PTR ) dm9k_in_check,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     )	dm9k_in_check_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&dm9k_in_check_Stk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) dm9k_in_check_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) dm9k_in_check_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������
#endif 
}





#define  ethernetif_input_SIZE                     512
#define  ethernetif_input_PRIO                         6
OS_TCB   ethernetif_input_TCB;															//������ƿ�
CPU_STK  ethernetif_input_Stk [ ethernetif_input_SIZE ];	//�����ջ
void Create_ethernetif_input(void)
{
	OS_ERR      err;
	/* ���� ���� */
    OSTaskCreate((OS_TCB     *)&ethernetif_input_TCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"ethernetif_input",                             //��������
                 (OS_TASK_PTR ) ethernetif_input,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     )	ethernetif_input_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&ethernetif_input_Stk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) ethernetif_input_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) ethernetif_input_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������
}	
#endif /* MDK ARM Compiler */




/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
