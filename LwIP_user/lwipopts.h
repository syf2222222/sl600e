
/**
  ******************************************************************************
  * File Name          : lwipopts.h
  * Description        : This file overrides LwIP stack default configuration
  *                      done in opt.h file.
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
 
/* Define to prevent recursive inclusion --------------------------------------*/
#ifndef __LWIPOPTS__H__
#define __LWIPOPTS__H__

#include "main.h"

/*-----------------------------------------------------------------------------*/
/* Current version of LwIP supported by CubeMx: 2.0.3 -*/
/*-----------------------------------------------------------------------------*/

/* Within 'USER CODE' section, code will be kept by default at each generation */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

#ifdef __cplusplus
 extern "C" {
#endif

/* STM32CubeMX Specific Parameters (not defined in opt.h) ---------------------*/
/* Parameters set in STM32CubeMX LwIP Configuration GUI -*/
/*----- WITH_RTOS enabled (Since FREERTOS is set) -----*/
#define WITH_RTOS 1
/*----- CHECKSUM_BY_HARDWARE disabled -----*/
#define CHECKSUM_BY_HARDWARE 0
/*-----------------------------------------------------------------------------*/

/* LwIP Stack Parameters (modified compared to initialization value in opt.h) -*/
/* Parameters set in STM32CubeMX LwIP Configuration GUI -*/
/*----- Value in opt.h for LWIP_DHCP: 0 -----*/
#define LWIP_DHCP 1
/*----- Value in opt.h for MEM_ALIGNMENT: 1 -----*/
#define MEM_ALIGNMENT 4
/*----- Value in opt.h for MEMP_NUM_SYS_TIMEOUT: (LWIP_TCP + IP_REASSEMBLY + LWIP_ARP + (2*LWIP_DHCP) + LWIP_AUTOIP + LWIP_IGMP + LWIP_DNS + (PPP_SUPPORT*6*MEMP_NUM_PPP_PCB) + (LWIP_IPV6 ? (1 + LWIP_IPV6_REASS + LWIP_IPV6_MLD) : 0)) -*/
#define MEMP_NUM_SYS_TIMEOUT 5
/*----- Value in opt.h for LWIP_ETHERNET: LWIP_ARP || PPPOE_SUPPORT -*/
#define LWIP_ETHERNET 1
/*----- Value in opt.h for LWIP_DNS_SECURE: (LWIP_DNS_SECURE_RAND_XID | LWIP_DNS_SECURE_NO_MULTIPLE_OUTSTANDING | LWIP_DNS_SECURE_RAND_SRC_PORT) -*/
#define LWIP_DNS_SECURE 7


///*----- Default Value for TCP_TTL: 255 ---*/
//#define TCP_TTL 128
/////*----- Default Value for TCP_WND: 4800 ---*/
////#define TCP_WND 10000
///*----- Default Value for TCP_MSS: 536 ---*/
//#define TCP_MSS 1200

//#define MEMP_NUM_TCP_SEG                    40  ������//80 �ܹ�ͬʱ�ڶ������TCP��PCB��Ŀ
//*****************************************************************************
//#define LWIP_TCP                            1                                  //����TCPЭ��(httpЭ����ڴ�)
//#define TCP_TTL                             (IP_DEFAULT_TTL)                   //TCP���ݰ��������
//#define TCP_WND                             4*TCP_MSS                          //tcp���ڴ�С
//#define TCP_MAXRTX                          12                                 //��������ش�TCP���ݰ���Ŀ
//#define TCP_SYNMAXRTX                       6                                  //��������ش�SYN���Ӱ���Ŀ
//#define TCP_QUEUE_OOSEQ                     (LWIP_TCP)                         //TCP���ն��������ݿ�����
//#define TCP_MSS                             1460                               //tcp�������ֶγ���
//#define TCP_CALCULATE_EFF_SEND_MSS          1                                  //tcpԤ�Ʒ��͵ķֶγ��ȣ�Ϊ1����ݴ��ڴ�С����
//#define TCP_SND_BUF                         (2*TCP_MSS)                        //TCP���ͻ�����  zc 7.1                                     
//#define TCP_SND_QUEUELEN                    (4*(TCP_SND_BUF/TCP_MSS))          //TCP���Ͷ��г���
//#define TCP_SNDLOWAT                        (TCP_SND_BUF/4)                    //TCP�ɷ������ݳ���
//#define TCP_LISTEN_BACKLOG                  1                                  //TCP����������
//#define TCP_DEFAULT_LISTEN_BACKLOG          0xff                               //TCP���ӺͰ����ӵ�����
//#define LWIP_TCP_TIMESTAMPS                 0                                  //TCPʱ���ѡ��
//#define TCP_WND_UPDATE_THRESHOLD            (TCP_WND / 4)                      //TCP���ڸ�����ֵ

//#define  SO_REUSE 	1//	--- �ط���
//#define LWIP_NETIF_API	1

//TCP��ʱ������ѡ��
#define  TCP_MSL                             10000UL                            //TCP���Ӵ���ʱ�� ��λms
//#define  TCP_FIN_WAIT_TIMEOUT                20000UL                            //FIN�ȵ�ACKӦ��ʱ�� ��λms
#define  TCP_TMR_INTERVAL                    20                                 //TCP��ʱ��������� 20ms


/*----- Value in opt.h for TCP_SND_QUEUELEN: (4*TCP_SND_BUF + (TCP_MSS - 1))/TCP_MSS -----*/
#define TCP_SND_QUEUELEN 9
/*----- Value in opt.h for TCP_SNDLOWAT: LWIP_MIN(LWIP_MAX(((TCP_SND_BUF)/2), (2 * TCP_MSS) + 1), (TCP_SND_BUF) - 1) -*/
#define TCP_SNDLOWAT 1071
/*----- Value in opt.h for TCP_SNDQUEUELOWAT: LWIP_MAX(TCP_SND_QUEUELEN)/2, 5) -*/
#define TCP_SNDQUEUELOWAT 5
/*----- Value in opt.h for TCP_WND_UPDATE_THRESHOLD: LWIP_MIN(TCP_WND/4, TCP_MSS*4) -----*/
#define TCP_WND_UPDATE_THRESHOLD 536
/*----- Value in opt.h for TCPIP_THREAD_STACKSIZE: 0 -----*/
#define TCPIP_THREAD_STACKSIZE 1024
/*----- Value in opt.h for TCPIP_THREAD_PRIO: 1 -----*/
#define TCPIP_THREAD_PRIO 3
/*----- Value in opt.h for TCPIP_MBOX_SIZE: 0 -----*/
#define TCPIP_MBOX_SIZE 6
/*----- Value in opt.h for SLIPIF_THREAD_STACKSIZE: 0 -----*/
#define SLIPIF_THREAD_STACKSIZE 1024
/*----- Value in opt.h for SLIPIF_THREAD_PRIO: 1 -----*/
#define SLIPIF_THREAD_PRIO 3
/*----- Value in opt.h for DEFAULT_THREAD_STACKSIZE: 0 -----*/
#define DEFAULT_THREAD_STACKSIZE 1024
/*----- Value in opt.h for DEFAULT_THREAD_PRIO: 1 -----*/
#define DEFAULT_THREAD_PRIO 3
/*----- Value in opt.h for DEFAULT_UDP_RECVMBOX_SIZE: 0 -----*/
#define DEFAULT_UDP_RECVMBOX_SIZE 6
/*----- Value in opt.h for DEFAULT_TCP_RECVMBOX_SIZE: 0 -----*/
#define DEFAULT_TCP_RECVMBOX_SIZE 6
/*----- Value in opt.h for DEFAULT_ACCEPTMBOX_SIZE: 0 -----*/
#define DEFAULT_ACCEPTMBOX_SIZE 6
/*----- Value in opt.h for RECV_BUFSIZE_DEFAULT: INT_MAX -----*/
#define RECV_BUFSIZE_DEFAULT 2000000000
/*----- Value in opt.h for LWIP_STATS: 1 -----*/
//#define LWIP_STATS 1
/*----- Value in opt.h for CHECKSUM_GEN_IP: 1 -----*/
#define CHECKSUM_GEN_IP 1
/*----- Value in opt.h for CHECKSUM_GEN_UDP: 1 -----*/
#define CHECKSUM_GEN_UDP 1
/*----- Value in opt.h for CHECKSUM_GEN_TCP: 1 -----*/
#define CHECKSUM_GEN_TCP 1
/*----- Value in opt.h for CHECKSUM_GEN_ICMP: 1 -----*/
#define CHECKSUM_GEN_ICMP 1
/*----- Value in opt.h for CHECKSUM_GEN_ICMP6: 1 -----*/
#define CHECKSUM_GEN_ICMP6 1
/*----- Value in opt.h for CHECKSUM_CHECK_IP: 1 -----*/
#define CHECKSUM_CHECK_IP 1
/*----- Value in opt.h for CHECKSUM_CHECK_UDP: 1 -----*/
#define CHECKSUM_CHECK_UDP 1
/*----- Value in opt.h for CHECKSUM_CHECK_TCP: 1 -----*/
#define CHECKSUM_CHECK_TCP 1
/*----- Value in opt.h for CHECKSUM_CHECK_ICMP: 1 -----*/
#define CHECKSUM_CHECK_ICMP 1
/*----- Value in opt.h for CHECKSUM_CHECK_ICMP6: 1 -----*/
#define CHECKSUM_CHECK_ICMP6 1

//#define LWIP_NETCONN                    1
//#define LWIP_SOCKET                     1
//#define LWIP_COMPAT_MUTEX               1 //û�л��������ݱ�־ ��ʹ�û�����
//#define LWIP_COMPAT_MUTEX_ALLOWED		//������ܳ��ֵ��쳣 (���ȼ���ת) �����
//#define SYS_LIGHTWEIGHT_PROT    0		//�����ӿ�,�������sys_arch_protect(void) ���� ��Ҫ�ٽ籣��?������

///*----- Default Value for LWIP_TFTP: 0 ---*/
//#define LWIP_TFTP 1
///*----- Default Value for LWIP_DISABLE_TCP_SANITY_CHECKS: 0 ---*/
//#define LWIP_DISABLE_TCP_SANITY_CHECKS 1
///*----- Default Value for LWIP_DISABLE_MEMP_SANITY_CHECKS: 0 ---*/
//#define LWIP_DISABLE_MEMP_SANITY_CHECKS 1
///*----- Default Value for LWIP_PERF: 0 ---*/
//#define LWIP_PERF 1
///*----- Default Value for LWIP_STATS: 0 ---*/
//#define LWIP_STATS 0
///*----- Value in opt.h for MIB2_STATS: 0 or SNMP_LWIP_MIB2 -----*/
//#define MIB2_STATS 0
///*----- Default Value for LWIP_CHECKSUM_CTRL_PER_NETIF: 0 ---*/
//#define LWIP_CHECKSUM_CTRL_PER_NETIF 1

#define TCP_MSS                             1460                               //tcp�������ֶγ���
#define LWIP_SO_RCVTIMEO                  1               //conn->acceptmbox/recvmbox�����г�ʱ����,��ʱ���Զ��Ͽ�����
/*-----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */
#define DEBUG_OPTIONS                                   1                   //���Դ���
/* USER CODE END 1 */
#define LWIP_TCPIP_CORE_LOCKING         1

#ifdef __cplusplus
}
#endif
#endif /*__LWIPOPTS__H__ */

/************************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
