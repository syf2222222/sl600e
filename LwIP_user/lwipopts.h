
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

//#define MEMP_NUM_TCP_SEG                    40  　　　//80 能够同时在队列里的TCP的PCB数目
//*****************************************************************************
//#define LWIP_TCP                            1                                  //启用TCP协议(http协议基于此)
//#define TCP_TTL                             (IP_DEFAULT_TTL)                   //TCP数据包传输次数
//#define TCP_WND                             4*TCP_MSS                          //tcp窗口大小
//#define TCP_MAXRTX                          12                                 //最大允许重传TCP数据包数目
//#define TCP_SYNMAXRTX                       6                                  //最大允许重传SYN连接包数目
//#define TCP_QUEUE_OOSEQ                     (LWIP_TCP)                         //TCP接收队列外数据块排序
//#define TCP_MSS                             1460                               //tcp报文最大分段长度
//#define TCP_CALCULATE_EFF_SEND_MSS          1                                  //tcp预计发送的分段长度，为1则根据窗口大小分配
//#define TCP_SND_BUF                         (2*TCP_MSS)                        //TCP发送缓冲区  zc 7.1                                     
//#define TCP_SND_QUEUELEN                    (4*(TCP_SND_BUF/TCP_MSS))          //TCP发送队列长度
//#define TCP_SNDLOWAT                        (TCP_SND_BUF/4)                    //TCP可发送数据长度
//#define TCP_LISTEN_BACKLOG                  1                                  //TCP多连接允许
//#define TCP_DEFAULT_LISTEN_BACKLOG          0xff                               //TCP连接和半连接的总数
//#define LWIP_TCP_TIMESTAMPS                 0                                  //TCP时间戳选项
//#define TCP_WND_UPDATE_THRESHOLD            (TCP_WND / 4)                      //TCP窗口更新阈值

//#define  SO_REUSE 	1//	--- 重发送
//#define LWIP_NETIF_API	1

//TCP定时器设置选项
#define  TCP_MSL                             10000UL                            //TCP连接存在时间 单位ms
//#define  TCP_FIN_WAIT_TIMEOUT                20000UL                            //FIN等到ACK应答时间 单位ms
#define  TCP_TMR_INTERVAL                    20                                 //TCP定时器计数间隔 20ms


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
//#define LWIP_COMPAT_MUTEX               1 //没有互斥锁兼容标志 不使用互斥锁
//#define LWIP_COMPAT_MUTEX_ALLOWED		//允许可能出现的异常 (优先级反转) 的情况
//#define SYS_LIGHTWEIGHT_PROT    0		//非轻便接口,避免添加sys_arch_protect(void) 函数 需要临界保护?的问题

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

#define TCP_MSS                             1460                               //tcp报文最大分段长度
#define LWIP_SO_RCVTIMEO                  1               //conn->acceptmbox/recvmbox接收有超时限制,超时后自动断开连接
/*-----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */
#define DEBUG_OPTIONS                                   1                   //调试代码
/* USER CODE END 1 */
#define LWIP_TCPIP_CORE_LOCKING         1

#ifdef __cplusplus
}
#endif
#endif /*__LWIPOPTS__H__ */

/************************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
