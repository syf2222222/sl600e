/**
  ******************************************************************************
  * �ļ�����: main.c 
  * ��    ��: ӲʯǶ��ʽ�����Ŷ�
  * ��    ��: V1.0
  * ��д����: 2017-03-30
  * ��    ��: TCP_Server����ʵ��
  ******************************************************************************
  * ˵����
  * ����������Ӳʯstm32������YS-F4Proʹ�á�
  * 
  * �Ա���
  * ��̳��http://www.ing10bbs.com
  * ��Ȩ��ӲʯǶ��ʽ�����Ŷ����У��������á�
  ******************************************************************************
  */
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
//#include "lwip/timeouts.h"
//#include "netif/etharp.h"
//#include "ethernetif.h"
//#include <string.h>

#include "lwip/timeouts.h"
#include "netif/ethernet.h"
#include "netif/etharp.h"
#include "lwip/ethip6.h"
#include "ethernetif.h"
#include <string.h>
//#include "cmsis_os.h"

#include "lwip.h"

/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
#define TIME_WAITING_FOR_INPUT ( portMAX_DELAY )
/* Stack size of the interface thread */
#define INTERFACE_THREAD_STACK_SIZE ( 350 )
/* Network interface name */
#define IFNAME0 's'
#define IFNAME1 't'

#if 0
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4   
#endif
__ALIGN_BEGIN ETH_DMADescTypeDef  DMARxDscrTab[ETH_RXBUFNB] __ALIGN_END;/* Ethernet Rx MA Descriptor */

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4   
#endif
__ALIGN_BEGIN ETH_DMADescTypeDef  DMATxDscrTab[ETH_TXBUFNB] __ALIGN_END;/* Ethernet Tx DMA Descriptor */

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4   
#endif
__ALIGN_BEGIN uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE] __ALIGN_END; /* Ethernet Receive Buffer */

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4   
#endif
__ALIGN_BEGIN uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE] __ALIGN_END; /* Ethernet Transmit Buffer */

/* ˽�б��� ------------------------------------------------------------------*/
ETH_HandleTypeDef EthHandle;

/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/

/* ������ --------------------------------------------------------------------*/
/**
  * ��������: ETHʱ�����ų�ʼ��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */

void HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{ 
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* ʹ�ܶ˿�ʱ�� */
  ETH_GPIO_ClK_ENABLE();

  /* ʹ��ETH����ʱ�� */
  ETH_RCC_CLK_ENABLE();

  /**ETH GPIO Configuration    
	PB2     ------> ETH_RST
  PC1     ------> ETH_MDC
  PA1     ------> ETH_REF_CLK
  PA2     ------> ETH_MDIO
  PA7     ------> ETH_CRS_DV
  PC4     ------> ETH_RXD0
  PC5     ------> ETH_RXD1
  PG11     ------> ETH_TX_EN
  PG13     ------> ETH_TXD0
  PG14     ------> ETH_TXD1 
  */
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,GPIO_PIN_SET);
	GPIO_InitStructure.Pin = GPIO_PIN_2;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStructure.Alternate = GPIO_AF0_TRACE;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  GPIO_InitStructure.Pin = GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStructure.Alternate = GPIO_AFx_ETH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_7;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStructure.Alternate = GPIO_AFx_ETH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.Pin = GPIO_PIN_11|GPIO_PIN_13|GPIO_PIN_14;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStructure.Alternate = GPIO_AFx_ETH;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);
}
#endif

/* MAC ADDRESS: MAC_ADDR0:MAC_ADDR1:MAC_ADDR2:MAC_ADDR3:MAC_ADDR4:MAC_ADDR5 */
//#define MAC_ADDR0   2U
//#define MAC_ADDR1   0U
//#define MAC_ADDR2   0U
//#define MAC_ADDR3   0U
//#define MAC_ADDR4   0U
//#define MAC_ADDR5   0U
/**
  * ��������: ETH�����ʼ��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */

void dm9k_init(unsigned char *MAC_ADD);
unsigned char	mac_addr[6]={0x02,0x00,0x00,0x00,0x00,0x00};

//osSemaphoreId s_xSemaphore = NULL;
//extern osSemaphoreId s_xSemaphore;

//#define osThreadDef(name, thread, priority, instances, stacksz)  \
//const osThreadDef_t os_thread_def_##name = \
//{ #name, (thread), (priority), (instances), (stacksz), NULL, NULL }

static void low_level_init( struct netif *netif )
{
	/* set MAC hardware address length */
	netif->hwaddr_len = 6;/* set MAC hardware address */
	netif->hwaddr[0] = mac_addr[0];
  netif->hwaddr[1] = mac_addr[1];
  netif->hwaddr[2] = mac_addr[2];
  netif->hwaddr[3] = mac_addr[3];
  netif->hwaddr[4] = mac_addr[4];
  netif->hwaddr[5] = mac_addr[5];

	/* maximum transfer unit */
	netif->mtu = 1500;
	/* broadcast capability */
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
	/* Initialise the EMAC.  This routine contains code that polls status bits.  
	If the Ethernet cable is not plugged in then this can take a considerable time.  
	To prevent this starving lower priority tasks of processing time welower our priority prior to the call,
	then raise it back again once theinitialisation is complete. */
	
//	etherdev_init(netif->hwaddr);
	dm9k_init(netif->hwaddr);
//�������Ҫд���ʼ���ĺ���

	
///* create a binary semaphore used for informing ethernetif of frame reception */
///*��������֪ͨ��̫������֡�Ķ������ź���*/
//  osSemaphoreDef(SEM);
//  s_xSemaphore = osSemaphoreCreate(osSemaphore(SEM) , 1 );

	
		Create_ethernetif_input();	//���� �ײ�Ӳ�����ݰ����շ��ͽ���  ethernetif_input 
///* create the task that handles the ETH_MAC */	
//	osThreadDef(EthIf, ethernetif_input, osPriorityRealtime, 0, 350);
//  osThreadCreate (osThread(EthIf), netif);						//���� �ײ�Ӳ�����ݰ����շ��ͽ���  ethernetif_input 
}  


/**
  * @brief This function should do the actual transmission of the packet. The packet is
  * contained in the pbuf that is passed to the function. This pbuf
  * might be chained.
  *
  * @param netif the lwip network interface structure for this ethernetif
  * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
  * @return ERR_OK if the packet could be sent
  *         an err_t value if the packet couldn't be sent
  *
  * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
  *       strange results. You might consider waiting for space in the DMA queue
  *       to become availale since the stack doesn't retry to send a packet
  *       dropped because of memory failure (except for the TCP timers).
  */

///**
//  * @brief Should allocate a pbuf and transfer the bytes of the incoming
//  * packet from the interface into the pbuf.
//  *
//  * @param netif the lwip network interface structure for this ethernetif
//  * @return a pbuf filled with the received packet (including MAC header)
//  *         NULL on memory error
//  */

//���ڷ������ݰ�����ײ㺯��(lwipͨ��netif->linkoutputָ��ú���)
//netif:�����ṹ��ָ��
//p:pbuf���ݽṹ��ָ��
//����ֵ:ERR_OK,��������
//       ERR_MEM,����ʧ��
void DM9000_SendPacket(struct pbuf *p);
struct pbuf *DM9000_Receive_Packet(void);

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
	DM9000_SendPacket(p);//��������
	return ERR_OK;
}
//���ڽ������ݰ�����ײ㺯��
//neitif:�����ṹ��ָ��
//����ֵ:pbuf���ݽṹ��ָ��
static struct pbuf * low_level_input(struct netif *netif)
{  
	struct pbuf *p;
	p=DM9000_Receive_Packet();
	return p;
}
/**
  * @brief This function should be called when a packet is ready to be read
  * from the interface. It uses the function low_level_input() that
  * should handle the actual reception of bytes from the network
  * interface. Then the type of the received packet is determined and
  * the appropriate input function is called.
  *
  * @param netif the lwip network interface structure for this ethernetif
  */
 
struct netif2 
{
  struct netif *next;

#if LWIP_IPV4
  /** IP address configuration in network byte order */
  ip_addr_t ip_addr;
  ip_addr_t netmask;
  ip_addr_t gw;
#endif /* LWIP_IPV4 */

  /** This function is called by the network device driver
   *  to pass a packet up the TCP/IP stack. */
  netif_input_fn input;
#if LWIP_IPV4
  /** This function is called by the IP module when it wants
   *  to send a packet on the interface. This function typically
   *  first resolves the hardware address, then sends the packet.
   *  For ethernet physical layer, this is usually etharp_output() */
  netif_output_fn output;
#endif /* LWIP_IPV4 */
  /** This function is called by ethernet_output() when it wants
   *  to send a packet on the interface. This function outputs
   *  the pbuf as-is on the link medium. */
  netif_linkoutput_fn linkoutput;
#if LWIP_NETIF_LINK_CALLBACK
  /** This function is called when the netif link is set to up or down
   */
  netif_status_callback_fn link_callback;
#endif /* LWIP_NETIF_LINK_CALLBACK */
  void *state;
#ifdef netif_get_client_data
  void* client_data[LWIP_NETIF_CLIENT_DATA_INDEX_MAX + LWIP_NUM_NETIF_CLIENT_DATA];
#endif

#if LWIP_IPV6_SEND_ROUTER_SOLICIT
  /** Number of Router Solicitation messages that remain to be sent. */
  u8_t rs_count;
#endif /* LWIP_IPV6_SEND_ROUTER_SOLICIT */
  /** maximum transfer unit (in bytes) */
  u16_t mtu;
  /** number of bytes used in hwaddr */
  u8_t hwaddr_len;
  /** link level hardware address of this interface */
  u8_t hwaddr[NETIF_MAX_HWADDR_LEN];
  /** flags (@see @ref netif_flags) */
  u8_t flags;
  /** descriptive abbreviation */
  char name[2];
  /** number of this interface */
  u8_t num;
};

extern	struct netif gnetif;
uint8_t check_rx(void);
	
void ethernetif_input ( void const * argument )  
{
  struct pbuf *p;
  struct netif *netif = (struct netif *) argument;
	OS_ERR      err;
	int time=0;
	
	netif=&gnetif;
	
  while(1)
  {
//								��ֵ�ź���ָ��	//�ȴ���ʱʱ��	//ѡ�� //�ȵ��ź���ʱ��ʱ��� //���ش�������
//		err =  
//		OSSemPend (&dm9k_input_sem, 0, OS_OPT_PEND_BLOCKING,0, &err); 
//		printf("\r\nTask2 err=%x",err);
		
		OSTimeDly ( 10, OS_OPT_TIME_DLY, & err ); 
		time++;
//		if(flag||(time>=500))
		if(check_rx()||(time>=500))
//    if(err  == ERR_OK)//if (osSemaphoreWait( s_xSemaphore, TIME_WAITING_FOR_INPUT)==osOK)
    {
			time=0;
//			printf("\r\n//////////////ethernetif_input_in//////////////");
     do
		 {   
        p = low_level_input( netif );
        if   (p != NULL)
        {
          if (netif->input( p, netif) != ERR_OK ) //ת�� tcpip_input
          {
							pbuf_free(p);
						printf("pbuf_free5");
          }
//					printf("\r\nethernetif_input�յ�����");
        }
      } while(p!=NULL);
		 
			
//			printf("\r\n//////////////ethernetif_input_out//////////////\r\n");
//			OSTimeDly()
//			OSTimeDly ( 100, OS_OPT_TIME_DLY, & err ); 
    }
  }
}

/**
  * @brief Should be called at the beginning of the program to set up the
  * network interface. It calls the function low_level_init() to do the
  * actual setup of the hardware.
  *
  * This function should be passed as a parameter to netif_add().
  *
  * @param netif the lwip network interface structure for this ethernetif
  * @return ERR_OK if the loopif is initialized
  *         ERR_MEM if private data couldn't be allocated
  *         any other err_t on error
  */
err_t ethernetif_init(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));
  
#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
  netif->output = etharp_output;
  netif->linkoutput = low_level_output;

  /* initialize the hardware */
  low_level_init(netif);

  return ERR_OK;
}

/**
  * @brief  Returns the current time in milliseconds
  *         when LWIP_TIMERS == 1 and NO_SYS == 1
  * @param  None
  * @retval Current Time value
  */
u32_t sys_now(void)
{
  return HAL_GetTick();
}

/**
  * @brief  This function sets the netif link status.
  * @param  netif: the network interface
  * @retval None
  */
#if 0
void ethernetif_set_link(struct netif *netif)
{
  uint32_t regvalue = 0;
  
//  /* Read PHY_MISR*/
//  HAL_ETH_ReadPHYRegister(&EthHandle, PHY_MISR, &regvalue);
//  
//  /* Check whether the link interrupt has occurred or not */
//  if((regvalue & PHY_LINK_INTERRUPT) != (uint16_t)RESET)
//  {
//    /* Read PHY_SR*/
//    HAL_ETH_ReadPHYRegister(&EthHandle, PHY_SR, &regvalue);
//    
//    /* Check whether the link is up or down*/
//    if((regvalue & PHY_LINK_STATUS)!= (uint16_t)RESET)
//    {
//      netif_set_link_up(netif);
//    }
//    else
//    {
//      netif_set_link_down(netif);
//    }
//  }
}
#endif
/**
  * @brief  Link callback function, this function is called on change of link status
  *         to update low level driver configuration.
  * @param  netif: The network interface
  * @retval None
  */
#if 0	
void ethernetif_update_config(struct netif *netif)
{
  __IO uint32_t tickstart = 0;
  uint32_t regvalue = 0;
  
  if(netif_is_link_up(netif))
  { 
    printf("����ص�����,���ѳɹ�����\n");
    /* Restart the auto-negotiation */
    if(EthHandle.Init.AutoNegotiation != ETH_AUTONEGOTIATION_DISABLE)
    {
      /* Enable Auto-Negotiation */
      HAL_ETH_WritePHYRegister(&EthHandle, PHY_BCR, PHY_AUTONEGOTIATION);
      
      /* Get tick */
      tickstart = HAL_GetTick();
      
      /* Wait until the auto-negotiation will be completed */
      do
      {
        HAL_ETH_ReadPHYRegister(&EthHandle, PHY_BSR, &regvalue);
        
        /* Check for the Timeout ( 1s ) */
        if((HAL_GetTick() - tickstart ) > 1000)
        {
          /* In case of timeout */
          goto error;
        }
        
      } while (((regvalue & PHY_AUTONEGO_COMPLETE) != PHY_AUTONEGO_COMPLETE));
      
      /* Read the result of the auto-negotiation */
      HAL_ETH_ReadPHYRegister(&EthHandle, PHY_SR, &regvalue);
      
      /* Configure the MAC with the Duplex Mode fixed by the auto-negotiation process */
      if((regvalue & PHY_DUPLEX_STATUS) != (uint32_t)RESET)
      {
        /* Set Ethernet duplex mode to Full-duplex following the auto-negotiation */
        EthHandle.Init.DuplexMode = ETH_MODE_FULLDUPLEX;  
      }
      else
      {
        /* Set Ethernet duplex mode to Half-duplex following the auto-negotiation */
        EthHandle.Init.DuplexMode = ETH_MODE_HALFDUPLEX;           
      }
      /* Configure the MAC with the speed fixed by the auto-negotiation process */
      if(regvalue & PHY_SPEED_STATUS)
      {  
        /* Set Ethernet speed to 10M following the auto-negotiation */
        EthHandle.Init.Speed = ETH_SPEED_10M; 
      }
      else
      {   
        /* Set Ethernet speed to 100M following the auto-negotiation */ 
        EthHandle.Init.Speed = ETH_SPEED_100M;
      }
    }
    else /* AutoNegotiation Disable */
    {
    error :
      /* Check parameters */
      assert_param(IS_ETH_SPEED(EthHandle.Init.Speed));
      assert_param(IS_ETH_DUPLEX_MODE(EthHandle.Init.DuplexMode));
      
      /* Set MAC Speed and Duplex Mode to PHY */
      HAL_ETH_WritePHYRegister(&EthHandle, PHY_BCR, ((uint16_t)(EthHandle.Init.DuplexMode >> 3) |
                                                     (uint16_t)(EthHandle.Init.Speed >> 1))); 
    }

    /* ETHERNET MAC Re-Configuration */
    HAL_ETH_ConfigMAC(&EthHandle, (ETH_MACInitTypeDef *) NULL);

    /* Restart MAC interface */
    HAL_ETH_Start(&EthHandle);   
  }
  else
  {
    /* Stop MAC interface */
    HAL_ETH_Stop(&EthHandle);
  }

  ethernetif_notify_conn_changed(netif);
}
#endif

/**
  * @brief  This function notify user about link status changement.
  * @param  netif: the network interface
  * @retval None
  */
__weak void ethernetif_notify_conn_changed(struct netif *netif)
{
  /* NOTE : This is function could be implemented in user file 
            when the callback is needed,
  */  
}
/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/
