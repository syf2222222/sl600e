

#ifndef LWIPOPTS_H
#define LWIPOPTS_H
#include "main.h"
//*****************************************************************************
//
// ---------- SNMP options ----------
//
//*****************************************************************************
#define LWIP_SNMP                       0      ����//SNMPЭ������
//#if LWIP_SNMP
//#define SNMP_CONCURRENT_REQUESTS     1            //SNMP��ǰ������������
//#define SNMP_TRAP_DESTINATIONS       1            //SNMP trapĿ�ĵ�ַ��Ŀ
//#define SNMP_PRIVATE_MIB             1      ������ //SNMP ˽�нڵ���������
//#define SNMP_SAFE_REQUESTS           1            //���ظ�SNMP��ȫ����
//#endif

//*****************************************************************************
//
// ---------- Platform specific locking ----------
//
//*****************************************************************************
#define SYS_LIGHTWEIGHT_PROT              1          //�ٽ��жϱ�������(������ģʽ�¿���)
#define NO_SYS                            0          //LWIP�������л��߻��ڲ���ϵͳ��Ϊ0����ڲ���ϵͳ

//*****************************************************************************
//
// ---------- Memory options ----------
//
//*****************************************************************************
//#define MEM_LIBC_MALLOC                 0                //����LWIP�Դ��������䶯̬�ڴ�
#define MEM_LIBC_MALLOC                 0                //�ڴ�ز�ͨ���ڴ��������
#define MEM_ALIGNMENT                   4                //�ֽڶ���(��CPU��λ���й�,32λ����Ϊ4)
#define MEM_SIZE                        8*1024  ����������//���ڴ��С�����ڷ�������
#define MEMP_SANITY_CHECK               0                //mem_free���ú��������Ƿ���ѭ�� by zc
#define MEMP_OVERFLOW_CHECK             0                //lwip���ڴ������� 
#define MEM_USE_POOLS                   0                //�ڴ�Ѳ�ͨ���ڴ�ط���
#define MEM_USE_POOLS_TRY_BIGGER_POOL   0             �� //�����ڴ�ʧ�ܲ�ѡ����ڴ��
#define MEMP_USE_CUSTOM_POOLS           0                //ͬ��

//*****************************************************************************
//
// ---------- Internal Memory Pool Sizes ----------
//
//*****************************************************************************
#define MEMP_NUM_PBUF                       60    ����//����memp��PBUF_ROM(ROM�ڴ��)��PBUF_REF(RAM�ڴ��)��Ŀ����ܺ�
#define MEMP_NUM_RAW_PCB                    4    ���� //RAW���ӵ�PCB��Ŀ(��ҪLWIP RAW��Ч)
#define MEMP_NUM_UDP_PCB                    4        //�ܹ�ͬʱ���ӵ�UDP��PCB��Ŀ
#define MEMP_NUM_TCP_PCB                    (TCP_WND + TCP_SND_BUF)/TCP_MSS  //�ܹ�ͬʱ���ӵ�TCP��PCB��Ŀ 12
#define MEMP_NUM_TCP_PCB_LISTEN              1       //(TCP_WND + TCP_SND_BUF)/TCP_MSS  //�ܹ�ͬʱ������TCP��PCB��Ŀ
#define MEMP_NUM_TCP_SEG                    40  ������//80 �ܹ�ͬʱ�ڶ������TCP��PCB��Ŀ
#define MEMP_NUM_REASSDATA                   8       //���ͬʱ�ȴ���װ��IP����Ŀ��������IP��������IP��Ƭ
#define MEMP_NUM_ARP_QUEUE                  30       //���ȴ��ظ�ARP�������Ŀ(ARP_QUEUEING��Ч)
#define MEMP_NUM_IGMP_GROUP                  8       //�ಥ��ͬʱ�������ݵ�����Ա��Ŀ(LWIP_IGMP��Ч)
#define MEMP_NUM_SYS_TIMEOUT                20       //�ܹ�ͬʱ����ĳ�ʱ������Ŀ(NO_SYS==0��Ϸ)
#define MEMP_NUM_NETBUF                     10       //netbufs�ṹ����Ŀ
#define MEMP_NUM_NETCONN                    16       //netconns�ṹ����Ŀ
#define MEMP_NUM_TCPIP_MSG_API              40       //tcpip_msg�ṹ�������Ŀ������callback��API��ͨѶ by zc
#define MEMP_NUM_TCPIP_MSG_INPKT            40       //tcpip_msg�������ݰ��ṹ�������Ŀ by zc
#define PBUF_POOL_SIZE                      48       //�ڴ������(С�ڴ��С��ѡ��ɴ���С�ڴ�ռ��)

//*****************************************************************************
//
// ---------- ARP options ----------
//
//*****************************************************************************
#define LWIP_ARP                            1        //ARPЭ������ 
#define ARP_TABLE_SIZE                      10  ������//ARPά���ı����С
#define ARP_QUEUEING                        1   ������//Ӳ����ַ����ʱ�����������ݰ��������
#define ETHARP_TRUST_IP_MAC                 1   ������//����IP���ݰ�����ֱ������ARP table�ĸ��£�
                                                     //Ϊ0��Ǳ�����IP-MAC��ϵ������ARP����,���Ա���IP��ƭ�������������ʱ
#define ETHARP_SUPPORT_VLAN                 0   ������//�������������Ϊ1������������ͨѶ��Ч

//*****************************************************************************
//
// ---------- IP options ----------
//
//*****************************************************************************
#define IP_FORWARD                          0    //������ƥ�����ݰ�ת��,��ӿ�ʱΪ1
#define IP_OPTIONS_ALLOWED                  1    //��IPѡ�����ݰ����� Ϊ0��������IP���ݰ�
#define IP_REASSEMBLY                       1    //�������IP����Ƭ��(Ϊ0���������ܹ����մ���MTU�İ�)
#define IP_FRAG                             1    //������IP����Ƭ��
#define IP_REASS_MAXAGE                     3    //������յ����ֶ���
#define IP_REASS_MAX_PBUFS                  10   //���������ڵ�IP��Ƭ��ռ�õ��ڴ�ظ���
#define IP_FRAG_USES_STATIC_BUF             1    //IP��Ƭʹ�þ�̬������
#define IP_FRAG_MAX_MTU                     1500 //IP��Ƭ��󻺳�����
#define IP_DEFAULT_TTL                      255  //IP���ݰ�������豸��Ŀ
#define IP_SOF_BROADCAST                    0    //IP���͹㲥������
#define IP_SOF_BROADCAST_RECV               0    //IP���չ㲥������

//*****************************************************************************
//
// ---------- ICMP options ----------
//
//*****************************************************************************
#define LWIP_ICMP                           1    //����ping������/����
#define ICMP_TTL                            (IP_DEFAULT_TTL) //ping��������豸��Ŀ
#define LWIP_BROADCAST_PING                 0    //���ظ��㲥ping��
#define LWIP_MULTICAST_PING                 0    //���ظ��ಥping��

//*****************************************************************************
//
// ---------- RAW options ----------
//
//*****************************************************************************
#define LWIP_RAW                            0              //�޲���ϵͳ���ڻص���������
#define RAW_TTL                           (IP_DEFAULT_TTL) //Ӧ�ò����ݴ������(����IP���TTL)

//*****************************************************************************
//
// ---------- DHCP options ----------
//
//*****************************************************************************
#define LWIP_DHCP                           0    //��̬����Э������(Ϊ1ʱ)
#define LWIP_AUTOIP                         0    //��̬����IP��ַ����(Ϊ1ʱ)
#define LWIP_DHCP_AUTOIP_COOP               0    //����������������ͬʱ������1���ӿ�(Ϊ1ʱ)
#define LWIP_DHCP_AUTOIP_COOP_TRIES         9    //DHCP��������ʧ�ܴ�����ʧ����ʹ��AUTOUP

//*****************************************************************************
//
// ---------- IGMP options ----------
//
//*****************************************************************************
#define LWIP_IGMP                           0            //LWIP�����Э��

//*****************************************************************************
//
// ---------- DNS options -----------
//
//*****************************************************************************
#define LWIP_DNS                            0     //����������ģ��(����UDPЭ��)
#define DNS_TABLE_SIZE                      4     //����������ά������������Ŀ
#define DNS_MAX_NAME_LENGTH                 256   //����������������ַ��󳤶�
#define DNS_MAX_SERVERS                     2     //������������������Ŀ
#define DNS_DOES_NAME_CHECK                 1     //��ѯ����������ʱ����ַ��
#define DNS_USES_STATIC_BUF                 1     //����������ʹ�þ�̬��ַ
#define DNS_MSG_SIZE                        512   //�����������������ͨѶ���ݳ���
#define DNS_LOCAL_HOSTLIST                  0     //�ڱ���ά����������������-��ַ��(Ϊ1ʱ)
#define DNS_LOCAL_HOSTLIST_IS_DYNAMIC       0     //����-��ַ��ʵʱ����(Ϊ1ʱ)

//*****************************************************************************
//
// ---------- UDP options ----------
//
//*****************************************************************************
#define LWIP_UDP                               1            //����UDPЭ��(snmpЭ����ڴ�)
#define LWIP_UDPLITE                           1            //UDPЭ�����þ����
#define UDP_TTL                                (IP_DEFAULT_TTL) //UDP���ݰ��������
#define LWIP_NETBUF_RECVINFO                   0            //���յ������ݰ����׸�������������Ŀ�ĵ�ַ�Ͷ˿�

//*****************************************************************************
//
// ---------- TCP options ----------
//
//*****************************************************************************
#define LWIP_TCP                            1                                  //����TCPЭ��(httpЭ����ڴ�)
#define TCP_TTL                             (IP_DEFAULT_TTL)                   //TCP���ݰ��������
#define TCP_WND                             4*TCP_MSS                          //tcp���ڴ�С
#define TCP_MAXRTX                          12                                 //��������ش�TCP���ݰ���Ŀ
#define TCP_SYNMAXRTX                       6                                  //��������ش�SYN���Ӱ���Ŀ
#define TCP_QUEUE_OOSEQ                     (LWIP_TCP)                         //TCP���ն��������ݿ�����
#define TCP_MSS                             1460                               //tcp�������ֶγ���
#define TCP_CALCULATE_EFF_SEND_MSS          1                                  //tcpԤ�Ʒ��͵ķֶγ��ȣ�Ϊ1����ݴ��ڴ�С����
#define TCP_SND_BUF                         (8*TCP_MSS)                        //TCP���ͻ�����  zc 7.1                                     
#define TCP_SND_QUEUELEN                    (4*(TCP_SND_BUF/TCP_MSS))          //TCP���Ͷ��г���
#define TCP_SNDLOWAT                        (TCP_SND_BUF/4)                    //TCP�ɷ������ݳ���
#define TCP_LISTEN_BACKLOG                  1                                  //TCP����������
#define TCP_DEFAULT_LISTEN_BACKLOG          0xff                               //TCP���ӺͰ����ӵ�����
#define LWIP_TCP_TIMESTAMPS                 0                                  //TCPʱ���ѡ��
#define TCP_WND_UPDATE_THRESHOLD            (TCP_WND / 4)                      //TCP���ڸ�����ֵ


//TCP��ʱ������ѡ��
#define  TCP_MSL                             10000UL                            //TCP���Ӵ���ʱ�� ��λms
//#define  TCP_FIN_WAIT_TIMEOUT                20000UL                            //FIN�ȵ�ACKӦ��ʱ�� ��λms
#define  TCP_TMR_INTERVAL                    20                                 //TCP��ʱ��������� 20ms

//*****************************************************************************
//         
// ---------- Internal Memory Pool Sizes ----------
//
//*****************************************************************************
 #define  PBUF_LINK_HLEN                 14           //Ϊ������̫��ͷ����İ�����(����MAC��ַ+Զ��MAC��ַ+Э������) 6+6+2                          #define  PBUF_POOL_BUFSIZE             256           //�����ڴ�س��ȣ�Ҫ���ǵ�4�ֽڶ������Ѵ�С
 #define  ETH_PAD_SIZE                  0             //��̫����䳤�ȣ�stm32����Ϊ0 ���ݷ��Ͱ��ж�
 
//��ϵ����.. Ĭ�ϼ���

//*****************************************************************************
//
// ---------- Sequential layer options ----------
//
//*****************************************************************************
#define LWIP_TCPIP_CORE_LOCKING           0           //?
#define LWIP_NETCONN                      1           //Ӧ�ò�ʹ��NETCONN��غ���

//*****************************************************************************
//
// ---------- Socket Options ----------
//
//*****************************************************************************
#define LWIP_SOCKET                       0               //�ر�SOCKETͨѶ
//#define LWIP_COMPAT_SOCKETS             1
//#define LWIP_POSIX_SOCKETS_IO_NAMES     1
#define LWIP_SO_RCVTIMEO                  0               //conn->acceptmbox/recvmbox�����г�ʱ����,��ʱ���Զ��Ͽ�����
//#define LWIP_SO_RCVBUF                  0
//#define SO_REUSE                        0

//*****************************************************************************
//
// ---------- Statistics options ----------
//
//*****************************************************************************
 #define LWIP_STATS                      0                      //LWIPͳ��ѡ��

//*****************************************************************************
//
// ---------- checksum options ----------
//
//*****************************************************************************
#define CHECKSUM_GEN_IP                      1                   //IPУ�������
#define CHECKSUM_GEN_UDP                     1                   //UDPУ�������
#define CHECKSUM_GEN_TCP                     1                   //TCPУ�������
#define CHECKSUM_CHECK_IP                    1                   //IPУ���У��
#define CHECKSUM_CHECK_UDP                   1                   //UDPУ���У��
#define CHECKSUM_CHECK_TCP                   1                   //TCPУ���У��

//*****************************************************************************
//
// ---------- TCP KEEPLIVE ----------
//
//*****************************************************************************
#define LWIP_TCP_KEEPALIVE                  1                //tcp���ʱ��
#define TCP_KEEPIDLE_DEFAULT                60000            //����ʱ�� 60s
#define TCP_KEEPINTVL_DEFAULT               10000            //����̽�� 10s
#define TCP_KEEPCNT_DEFAULT                 9U  

//*****************************************************************************
//
// ---------- DEBUG_OPTIONS  ----------
//
//*****************************************************************************
#define DEBUG_OPTIONS                                   0                    //���Դ���

#endif /* LWIPOPTS_H */


//SYS_LIGHTWEIGHT_PROT ƽ̨���������ؼ������ڻ���ķ������ͷ�
//NO_SYS --- �Ƿ������ϵͳ�� =1ʱ����=0ʱ������
//MEMCPY(dst,src,len) --- �ڴ濽�� ����ϣ���Լ�д������������滻
//SMEMCPY(dst,src,len) ---�ڴ濽�� ����ϣ���Լ�д������������滻
//MEM_LIBC_MALLOC --- �ڴ溯�����ʹ�ã�=1ʱʹ��c�� =0ʱʹ���Լ��Ŀ�
//MEMP_MEM_MALLOC --- = 1ʱ��ʹ��mem_malloc/mem_free��������
//MEM_ALIGNMENT --- �ֽڶ��� = 1ʱӦ����2�ֽڶ���
//MEM_SIZE --- ���ڴ��С ���������ݺܶ࣬��Ӧ���úܸ�
//MEMP_OVERFLOW_CHECK --- �������Ϊ0ʱ�رգ�������1��2��2�Ƚ���
//MEMP_SANITY_CHECK --- ����飬ÿ��free�����б��Ƿ���ѭ��
//MEM_USE_POOLS --- �Զ����ڴ��
//MEM_USE_POOLS_TRY_BIGGER_POOL--- �ڴ����������ϵͳ�ɿ���
//MEMP_USE_CUSTOM_POOLS --- ʹ���Զ����ڴ����lwippools.h�й�
//LWIP_ALLOW_MEM_FREE_FROM_OTHER_CONTEXT --- �����������ͷ��ڴ�
//MEMP_NUM_PBUF --- RAWЭ����ƿ�����
//MEMP_NUM_UDP_PCB --- UDPЭ����ƿ������
//MEMP_NUM_TCP_PCB --- TCPЭ����ƿ������
//MEMP_NUM_TCP_PCB_LISTEN ---TCP����������
//MEMP_NUM_TCP_SEG --- TCP�Ŷ�����
//MEMP_NUM_REASSDATA --- IP���ݰ��Ŷ�����
//MEMP_NUM_ARP_QUEUE --- arp������
//MEMP_NUM_IGMP_GROUP --- �鲥����ӿ���
//MEMP_NUM_SYS_TIMEOUT --- ϵͳ��ʱ����
//MEMP_NUM_NETBUF --- netbuf�ṹ��
//MEMP_NUM_NETCONN --- netconn�ṹ��
//MEMP_NUM_TCPIP_MSG_API --- tcpip��Ϣ��
//MEMP_NUM_TCPIP_MSG_INPKT --- tcpip���ݰ��������
//PBUF_POOL_SIZE --- pbuf�ش�С
//LWIP_ARP ---���ܿ����ر�
//ARP_TABLE_SIZE --- ip��ַ���������
//ARP_QUEUEING --- arp����
//ETHARP_TRUST_IP_MAC ---arp ip mac�ȹ�
//IP_FORWARD --- ip��ת��
//IP_OPTIONS_ALLOWED ---ipѡ������
//IP_REASSEMBLY --- IP�ϵ�����
//IP_FRAG --- IP�ϵ㷢��
//IP_REASS_MAXAGE --- �ֶ����ݰ����ʱ��
//IP_REASS_MAX_PBUFS --- �ֶ���װ����
//IP_FRAG_USES_STATIC_BUF --- ʹ�ܾ�̬������
//IP_FRAG_MAX_MTU ---IP��󻺳�����
//IP_DEFAULT_TTL --- IP���ttlʱ��
//IP_SOF_BROADCAST --- ip sof�㲥
//IP_SOF_BROADCAST_RECV --- spf�㲥����
//LWIP_ICMP --- ICMPʹ��
//ICMP_TTL --- ICMP���ttlʱ��
//LWIP_BROADCAST_PING --- �Ƿ���Ӧ�㲥ping
//LWIP_MULTICAST_PING --- �Ƿ���Ӧ�鲥ping
//LWIP_RAW ---rawЭ��ʹ��
//RAW_TTL --- raw ttlʱ��
//LWIP_DHCP --- DHCPʹ��
//DHCP_DOES_ARP_CHECK --- arp��ַ���
//LWIP_AUTOIP --- �Զ�ipʹ��
//LWIP_DHCP_AUTOIP_COOP --- dhcp �Զ�ip�ӿ�
//WIP_DHCP_AUTOIP_COOP_TRIES --- dhcp������
//LWIP_SNMP --- snmpʹ��
//SNMP_CONCURRENT_REQUESTS ---snmp������
//SNMP_TRAP_DESTINATIONS --- snmp �������
//SNMP_PRIVATE_MIB --- snmp ˽��mib
//SNMP_SAFE_REQUESTS --- snmp��ȫ���
//LWIP_IGMP --- IGMP����
//LWIP_DNS --- DNSʹ��
//DNS_TABLE_SIZE --- dns����¼��
//DNS_MAX_NAME_LENGTH --- dns��������󳤶�
//DNS_MAX_SERVERS --- ���dns������
//DNS_DOES_NAME_CHECK --- dns ��ӦУ��
//DNS_USES_STATIC_BUF --- dns���ػ�����
//DNS_MSG_SIZE --- dns��Ϣ��С
//DNS_LOCAL_HOSTLIST --- dns �����б�
//DNS_LOCAL_HOSTLIST_IS_DYNAMIC --- dns��̬�б�
//LWIP_UDP --- udpʹ��
//LWIP_UDPLITE --- udp���ݱ�
//UDP_TTL --- udp ttlʱ��
//LWIP_TCP --- TCP ʹ��
//TCP_TTL --- TCP TTLʱ��
//TCP_WND --- TCP���ڳ���
//TCP_MAXRTX --- TCP����ش�����
//TCP_SYNMAXRTX --- TCP syn������ش�����
//TCP_QUEUE_OOSEQ --- TCP���е���˳�� Ӧ�����������
//TCP_MSS --- TCP���δ�С
//TCP_CALCULATE_EFF_SEND_MSS --- ��������mms
//TCP_SND_BUF --- TCP���ͻ���ռ䣨�ֽڣ�
//TCP_SND_QUEUELEN ---TCP���ͻ���ռ䣨pbufs��
//TCP_SNDLOWAT --- TCPд�ռ䣨�ֽڣ�
//TCP_LISTEN_BACKLOG --- ����TCP��ѹѡ����Э����ƿ�
//TCP_DEFAULT_LISTEN_BACKLOG --- �������Э����ƿ������
//LWIP_TCP_TIMESTAMPS ---֧��TCPʱ���ѡ��
//TCP_WND_UPDATE_THRESHOLD --- ���ڸ����봥��
//LWIP_EVENT_API LWIP_CALLBACK_API --- ֻ��һ��Ϊ1 ��ϵͳ���ƻ��лص���������
//PBUF_LINK_HLEN --- ��·��ͷ
//PBUF_POOL_BUFSIZE --- pbuf�ش�С
//LWIP_NETIF_HOSTNAME --- ʹ��dhcp_option_hostname��netif��������
//lwip_netif_api --- �Ƿ�֧��netif API��1Ϊ֧��
//LWIP_NETIF_STATUS_CALLBACK --- �ص�����֧�ֽӿ�
//lwip_netif_link_callback --- ��·�������
//lwip_netif_hwaddrhint --- ������·���ַ����ʾ
//lwip_netif_loopback --- ֧�ַ������ݰ���Ŀ�ĵ�IP
//lwip_loopback_max_pbufs --- �ڶ��е����������pbufs����
//LWIP_NETIF_LOOPBACK_MULTITHREADING --- �Ƿ����ö��߳�
//LWIP_NETIF_TX_SINGLE_PBUF --- ��һPbuf����
//LWIP_HAVE_LOOPIF --- ֧�ֻ��͵�ַ(127.0.0.1) and loopif.c
//LWIP_HAVE_SLIPIF --- ֧�ֻ��������slipif C
//TCPIP_THREAD_NAME --- �������Ҫ��TCPIP�̵߳�����
//tcpip_thread_stacksize --- �����߳�ʹ��TCPIPջ��С
//TCPIP_THREAD_PRIO --- �������Ҫ��TCPIP�߳����ȼ�
//tcpip_mbox_size --- ����tcpip�߳���Ϣ�����С
//SLIPIF_THREAD_NAME --- SLIPIF �߳���
//SLIPIF_THREAD_STACKSIZE --- �����slipif_loop�߳����ȼ�
//PPP_THREAD_NAME --- PPPЭ���߳�����
//PPP_THREAD_STACKSIZE --- ��pppmain�߳�ʹ�õĶ�ջ��С
//ppp_thread_prio --- �����pppmain�߳����ȼ�
//DEFAULT_THREAD_NAME --- Ĭ��LWIP�߳���
//DEFAULT_THREAD_STACKSIZE --- ȱʡ��ջ��С
//default_thread_prio --- ������κ�����LwIP�߳����ȼ�
//default_raw_recvmbox_size --- ����raw�����С
//DEFAULT_UDP_RECVMBOX_SIZE --- ����udp�����С
//DEFAULT_TCP_RECVMBOX_SIZE --- ����tcp�����С
//DEFAULT_ACCEPTMBOX_SIZE --- ACCEPTMBOX��С
//lwip_tcpip_core_locking --- ��ʵ�飡��ƽʱ����
//lwip_netconn --- ʹ��netconn API����Ҫʹ��api_lib��C��
//lwip_socket --- ʹSocket API ����Ҫsockets.c��
//lwip_compat_sockets --- BSD�׽��ֺ�������
//lwip_posix_sockets_io_names ---ʹPOSIX�����׽��ֺ�������posixϵͳ
//LWIP_TCP_KEEPALIVE --- �������
//LWIP_SO_RCVBUF ---ʹ�������������
//RECV_BUFSIZE_DEFAULT --- ��ʹ�����������������Ϊ���������С
//SO_REUSE --- �ط���
//lwip_stats --- ʹ��lwip_statsͳ��
//LWIP_STATS_DISPLAY --- �������ͳ�ƹ���
//LINK_STATS --- ʹ������ͳ��
//ETHARP_STATS --- ʹ��arpͳ��
//IP_STATS --- ʹ��ipͳ��
//IPFRAG_STATS --- ip��Ƭͳ��
//ICMP_STATS --- icmpͳ��
//IGMP_STATS ---igmpͳ��
//UDP_STATS --- udpͳ��
//TCP_STATS --- tcpͳ��
//MEM_STATS ---�ڴ����ͳ��
//MEMP_STATS --- �ڴ�����ͳ��
//SYS_STATS --- ϵͳͳ��
//PPP_SUPPORT --- ʹ��pppЭ��
//PPPOE_SUPPORT --- ʹ��pppoeЭ��
//PPPOS_SUPPORT --- ʹ��ppposЭ��
//NUM_PPP --- ���ppp�Ự
//PAP_SUPPORT --- ʹ��pap������֤Э��
//CHAP_SUPPORT--- ʹ��chapЭ��
//MSCHAP_SUPPORT --- ʹ��mschapĿǰ��֧�֣�������
//CBCP_SUPPORT --- ʹ��CBCP Ŀǰ��֧�֣�������
//CCP_SUPPORT --- ʹ��ccp Ŀǰ��֧�֣�������
//VJ_SUPPORT --- ʹ�� ֧�ָñ�ͷѹ��
//md5_support --- ֧��MD5
//FSM_DEFTIMEOUT --- ���˳�ʱ��
//FSM_DEFMAXTERMREQS --- �����ֹ������
//FSM_DEFMAXCONFREQS --- �������������
//FSM_DEFMAXNAKLOOPS --- NAK�����������
//UPAP_DEFTIMEOUT --- �ط�����ʱ���룩
//UPAP_DEFREQTIME --- ʱ��ȴ���֤����ͬ��
//CHAP_DEFTIMEOUT --- chapĬ���˳�ʱ��
//CHAP_DEFTRANSMITS --- ���������ʱ��
//LCP_ECHOINTERVAL --- KeepAlive��������֮������0��Ϊ����
//LCP_MAXECHOFAILS --- ����֮ǰ����δ���Ļ���������
//PPP_MAXIDLEFLAG --- ��������ʱ�䣨�ھ���ǰ���ͱ�־�ַ���
//CHECKSUM_GEN_IP --- ����ip��У��
//CHECKSUM_GEN_UDP --- ����udp��У��
//CHECKSUM_GEN_TCP --- ����tcp��У��
//CHECKSUM_CHECK_IP ---����ip��У��
//CHECKSUM_CHECK_UDP ---����udp��У��
//CHECKSUM_CHECK_TCP --- ����tcp��У��
//�����Ǹ��ֵ���
//LWIP_DBG_MIN_LEVEL 
//LWIP_DBG_TYPES_ON
//ETHARP_DEBUG
//NETIF_DEBUG
//PBUF_DEBUG
//API_LIB_DEBUG
//API_MSG_DEBUG
//SOCKETS_DEBUG
//ICMP_DEBUG
//IGMP_DEBUG
//INET_DEBUG
//IP_DEBUG
//IP_REASS_DEBUG
//RAW_DEBUG
//MEM_DEBUG
//MEMP_DEBUG
//SYS_DEBUG
//TCP_DEBUG
//TCP_INPUT_DEBUG
//TCP_FR_DEBUG
//TCP_RTO_DEBUG
//TCP_CWND_DEBUG
//TCP_WND_DEBUG
//TCP_OUTPUT_DEBUG
//TCP_RST_DEBUG
//TCP_QLEN_DEBUG
//UDP_DEBUG
//TCPIP_DEBUG
//PPP_DEBUG
//SLIP_DEBUG
//DHCP_DEBUG
//AUTOIP_DEBUG
//SNMP_MSG_DEBUG
//SNMP_MIB_DEBUG
//DNS_DEBUG
