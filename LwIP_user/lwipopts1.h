

#ifndef LWIPOPTS_H
#define LWIPOPTS_H
#include "main.h"
//*****************************************************************************
//
// ---------- SNMP options ----------
//
//*****************************************************************************
#define LWIP_SNMP                       0      　　//SNMP协议启用
//#if LWIP_SNMP
//#define SNMP_CONCURRENT_REQUESTS     1            //SNMP当前允许请求数量
//#define SNMP_TRAP_DESTINATIONS       1            //SNMP trap目的地址数目
//#define SNMP_PRIVATE_MIB             1      　　　 //SNMP 私有节点设置允许
//#define SNMP_SAFE_REQUESTS           1            //仅回复SNMP安全请求
//#endif

//*****************************************************************************
//
// ---------- Platform specific locking ----------
//
//*****************************************************************************
#define SYS_LIGHTWEIGHT_PROT              1          //临界中断保护开关(多任务模式下开启)
#define NO_SYS                            0          //LWIP独立运行或者基于操作系统，为0则基于操作系统

//*****************************************************************************
//
// ---------- Memory options ----------
//
//*****************************************************************************
//#define MEM_LIBC_MALLOC                 0                //采用LWIP自带函数分配动态内存
#define MEM_LIBC_MALLOC                 0                //内存池不通过内存堆来分配
#define MEM_ALIGNMENT                   4                //字节对齐(和CPU的位数有关,32位设置为4)
#define MEM_SIZE                        8*1024  　　　　　//堆内存大小，用于发送数据
#define MEMP_SANITY_CHECK               0                //mem_free调用后检查链表是否有循环 by zc
#define MEMP_OVERFLOW_CHECK             0                //lwip堆内存溢出检查 
#define MEM_USE_POOLS                   0                //内存堆不通过内存池分配
#define MEM_USE_POOLS_TRY_BIGGER_POOL   0             　 //申请内存失败不选择大内存池
#define MEMP_USE_CUSTOM_POOLS           0                //同上

//*****************************************************************************
//
// ---------- Internal Memory Pool Sizes ----------
//
//*****************************************************************************
#define MEMP_NUM_PBUF                       60    　　//来自memp的PBUF_ROM(ROM内存池)和PBUF_REF(RAM内存池)数目最大总和
#define MEMP_NUM_RAW_PCB                    4    　　 //RAW连接的PCB数目(需要LWIP RAW有效)
#define MEMP_NUM_UDP_PCB                    4        //能够同时连接的UDP的PCB数目
#define MEMP_NUM_TCP_PCB                    (TCP_WND + TCP_SND_BUF)/TCP_MSS  //能够同时连接的TCP的PCB数目 12
#define MEMP_NUM_TCP_PCB_LISTEN              1       //(TCP_WND + TCP_SND_BUF)/TCP_MSS  //能够同时监听的TCP的PCB数目
#define MEMP_NUM_TCP_SEG                    40  　　　//80 能够同时在队列里的TCP的PCB数目
#define MEMP_NUM_REASSDATA                   8       //最大同时等待重装的IP包数目，是整个IP包，不是IP分片
#define MEMP_NUM_ARP_QUEUE                  30       //最大等待回复ARP请求的数目(ARP_QUEUEING有效)
#define MEMP_NUM_IGMP_GROUP                  8       //多播组同时接收数据的最大成员数目(LWIP_IGMP有效)
#define MEMP_NUM_SYS_TIMEOUT                20       //能够同时激活的超时连接数目(NO_SYS==0有戏)
#define MEMP_NUM_NETBUF                     10       //netbufs结构的数目
#define MEMP_NUM_NETCONN                    16       //netconns结构的数目
#define MEMP_NUM_TCPIP_MSG_API              40       //tcpip_msg结构的最大数目，用于callback和API的通讯 by zc
#define MEMP_NUM_TCPIP_MSG_INPKT            40       //tcpip_msg接受数据包结构的最大数目 by zc
#define PBUF_POOL_SIZE                      48       //内存池数量(小内存减小该选项可大大减小内存占用)

//*****************************************************************************
//
// ---------- ARP options ----------
//
//*****************************************************************************
#define LWIP_ARP                            1        //ARP协议允许 
#define ARP_TABLE_SIZE                      10  　　　//ARP维护的表项大小
#define ARP_QUEUEING                        1   　　　//硬件地址解析时，将发送数据包计入队列
#define ETHARP_TRUST_IP_MAC                 1   　　　//所有IP数据包都会直接引起ARP table的更新，
                                                     //为0则非表项内IP-MAC关系会引起ARP请求,可以避免IP欺骗，不过会造成延时
#define ETHARP_SUPPORT_VLAN                 0   　　　//非虚拟局域网，为1则仅虚拟局域网通讯有效

//*****************************************************************************
//
// ---------- IP options ----------
//
//*****************************************************************************
#define IP_FORWARD                          0    //不允许不匹配数据包转发,多接口时为1
#define IP_OPTIONS_ALLOWED                  1    //带IP选项数据包允许 为0则丢弃所有IP数据包
#define IP_REASSEMBLY                       1    //允许接收IP包分片包(为0不允许，不能够接收大于MTU的包)
#define IP_FRAG                             1    //允许发送IP包分片包
#define IP_REASS_MAXAGE                     3    //允许接收的最大分段数
#define IP_REASS_MAX_PBUFS                  10   //最大允许存在的IP分片包占用的内存池个数
#define IP_FRAG_USES_STATIC_BUF             1    //IP分片使用静态缓冲区
#define IP_FRAG_MAX_MTU                     1500 //IP分片最大缓冲数量
#define IP_DEFAULT_TTL                      255  //IP数据包最大经历设备数目
#define IP_SOF_BROADCAST                    0    //IP发送广播包过滤
#define IP_SOF_BROADCAST_RECV               0    //IP接收广播包过滤

//*****************************************************************************
//
// ---------- ICMP options ----------
//
//*****************************************************************************
#define LWIP_ICMP                           1    //开启ping包接收/发送
#define ICMP_TTL                            (IP_DEFAULT_TTL) //ping包最大经历设备数目
#define LWIP_BROADCAST_PING                 0    //不回复广播ping包
#define LWIP_MULTICAST_PING                 0    //不回复多播ping包

//*****************************************************************************
//
// ---------- RAW options ----------
//
//*****************************************************************************
#define LWIP_RAW                            0              //无操作系统基于回调函数驱动
#define RAW_TTL                           (IP_DEFAULT_TTL) //应用层数据传输次数(基于IP层的TTL)

//*****************************************************************************
//
// ---------- DHCP options ----------
//
//*****************************************************************************
#define LWIP_DHCP                           0    //动态主机协议配置(为1时)
#define LWIP_AUTOIP                         0    //动态主机IP地址配置(为1时)
#define LWIP_DHCP_AUTOIP_COOP               0    //允许上述两种配置同时存在于1个接口(为1时)
#define LWIP_DHCP_AUTOIP_COOP_TRIES         9    //DHCP分配允许失败次数，失败则使用AUTOUP

//*****************************************************************************
//
// ---------- IGMP options ----------
//
//*****************************************************************************
#define LWIP_IGMP                           0            //LWIP组管理协议

//*****************************************************************************
//
// ---------- DNS options -----------
//
//*****************************************************************************
#define LWIP_DNS                            0     //域名服务器模块(依托UDP协议)
#define DNS_TABLE_SIZE                      4     //域名服务器维护的最大入口数目
#define DNS_MAX_NAME_LENGTH                 256   //域名服务器主机地址最大长度
#define DNS_MAX_SERVERS                     2     //域名服务器最大服务数目
#define DNS_DOES_NAME_CHECK                 1     //查询域名服务器时检测地址名
#define DNS_USES_STATIC_BUF                 1     //域名服务器使用静态地址
#define DNS_MSG_SIZE                        512   //域名服务器接收最大通讯数据长度
#define DNS_LOCAL_HOSTLIST                  0     //在本地维护域名服务器主机-地址表(为1时)
#define DNS_LOCAL_HOSTLIST_IS_DYNAMIC       0     //主机-地址表实时更新(为1时)

//*****************************************************************************
//
// ---------- UDP options ----------
//
//*****************************************************************************
#define LWIP_UDP                               1            //启用UDP协议(snmp协议基于此)
#define LWIP_UDPLITE                           1            //UDP协议启用精简版
#define UDP_TTL                                (IP_DEFAULT_TTL) //UDP数据包传输次数
#define LWIP_NETBUF_RECVINFO                   0            //接收到的数据包除首个外其它不附加目的地址和端口

//*****************************************************************************
//
// ---------- TCP options ----------
//
//*****************************************************************************
#define LWIP_TCP                            1                                  //启用TCP协议(http协议基于此)
#define TCP_TTL                             (IP_DEFAULT_TTL)                   //TCP数据包传输次数
#define TCP_WND                             4*TCP_MSS                          //tcp窗口大小
#define TCP_MAXRTX                          12                                 //最大允许重传TCP数据包数目
#define TCP_SYNMAXRTX                       6                                  //最大允许重传SYN连接包数目
#define TCP_QUEUE_OOSEQ                     (LWIP_TCP)                         //TCP接收队列外数据块排序
#define TCP_MSS                             1460                               //tcp报文最大分段长度
#define TCP_CALCULATE_EFF_SEND_MSS          1                                  //tcp预计发送的分段长度，为1则根据窗口大小分配
#define TCP_SND_BUF                         (8*TCP_MSS)                        //TCP发送缓冲区  zc 7.1                                     
#define TCP_SND_QUEUELEN                    (4*(TCP_SND_BUF/TCP_MSS))          //TCP发送队列长度
#define TCP_SNDLOWAT                        (TCP_SND_BUF/4)                    //TCP可发送数据长度
#define TCP_LISTEN_BACKLOG                  1                                  //TCP多连接允许
#define TCP_DEFAULT_LISTEN_BACKLOG          0xff                               //TCP连接和半连接的总数
#define LWIP_TCP_TIMESTAMPS                 0                                  //TCP时间戳选项
#define TCP_WND_UPDATE_THRESHOLD            (TCP_WND / 4)                      //TCP窗口更新阈值


//TCP定时器设置选项
#define  TCP_MSL                             10000UL                            //TCP连接存在时间 单位ms
//#define  TCP_FIN_WAIT_TIMEOUT                20000UL                            //FIN等到ACK应答时间 单位ms
#define  TCP_TMR_INTERVAL                    20                                 //TCP定时器计数间隔 20ms

//*****************************************************************************
//         
// ---------- Internal Memory Pool Sizes ----------
//
//*****************************************************************************
 #define  PBUF_LINK_HLEN                 14           //为处理以太网头申请的包长度(本地MAC地址+远端MAC地址+协议类型) 6+6+2                          #define  PBUF_POOL_BUFSIZE             256           //单个内存池长度，要考虑到4字节对齐和最佳大小
 #define  ETH_PAD_SIZE                  0             //以太网填充长度，stm32设置为0 根据发送包判断
 
//关系不大.. 默认即可

//*****************************************************************************
//
// ---------- Sequential layer options ----------
//
//*****************************************************************************
#define LWIP_TCPIP_CORE_LOCKING           0           //?
#define LWIP_NETCONN                      1           //应用层使用NETCONN相关函数

//*****************************************************************************
//
// ---------- Socket Options ----------
//
//*****************************************************************************
#define LWIP_SOCKET                       0               //关闭SOCKET通讯
//#define LWIP_COMPAT_SOCKETS             1
//#define LWIP_POSIX_SOCKETS_IO_NAMES     1
#define LWIP_SO_RCVTIMEO                  0               //conn->acceptmbox/recvmbox接收有超时限制,超时后自动断开连接
//#define LWIP_SO_RCVBUF                  0
//#define SO_REUSE                        0

//*****************************************************************************
//
// ---------- Statistics options ----------
//
//*****************************************************************************
 #define LWIP_STATS                      0                      //LWIP统计选项

//*****************************************************************************
//
// ---------- checksum options ----------
//
//*****************************************************************************
#define CHECKSUM_GEN_IP                      1                   //IP校验和生成
#define CHECKSUM_GEN_UDP                     1                   //UDP校验和生成
#define CHECKSUM_GEN_TCP                     1                   //TCP校验和生成
#define CHECKSUM_CHECK_IP                    1                   //IP校验和校验
#define CHECKSUM_CHECK_UDP                   1                   //UDP校验和校验
#define CHECKSUM_CHECK_TCP                   1                   //TCP校验和校验

//*****************************************************************************
//
// ---------- TCP KEEPLIVE ----------
//
//*****************************************************************************
#define LWIP_TCP_KEEPALIVE                  1                //tcp保活定时器
#define TCP_KEEPIDLE_DEFAULT                60000            //保活时间 60s
#define TCP_KEEPINTVL_DEFAULT               10000            //保活探测 10s
#define TCP_KEEPCNT_DEFAULT                 9U  

//*****************************************************************************
//
// ---------- DEBUG_OPTIONS  ----------
//
//*****************************************************************************
#define DEBUG_OPTIONS                                   0                    //调试代码

#endif /* LWIPOPTS_H */


//SYS_LIGHTWEIGHT_PROT 平台锁，保护关键区域内缓存的分配与释放
//NO_SYS --- 是否带操作系统， =1时带，=0时，不带
//MEMCPY(dst,src,len) --- 内存拷贝 若你希望自己写拷贝函数则可替换
//SMEMCPY(dst,src,len) ---内存拷贝 若你希望自己写拷贝函数则可替换
//MEM_LIBC_MALLOC --- 内存函数库的使用，=1时使用c库 =0时使用自己的库
//MEMP_MEM_MALLOC --- = 1时，使用mem_malloc/mem_free代替分配池
//MEM_ALIGNMENT --- 字节对齐 = 1时应该是2字节对齐
//MEM_SIZE --- 堆内存大小 若发送内容很多，则应设置很高
//MEMP_OVERFLOW_CHECK --- 溢出保护为0时关闭，可以有1和2，2比较慢
//MEMP_SANITY_CHECK --- 链检查，每次free后检查列表是否有循环
//MEM_USE_POOLS --- 自定义内存池
//MEM_USE_POOLS_TRY_BIGGER_POOL--- 内存池增大，增加系统可靠性
//MEMP_USE_CUSTOM_POOLS --- 使用自定义内存池与lwippools.h有关
//LWIP_ALLOW_MEM_FREE_FROM_OTHER_CONTEXT --- 其他上下文释放内存
//MEMP_NUM_PBUF --- RAW协议控制块数量
//MEMP_NUM_UDP_PCB --- UDP协议控制块的数量
//MEMP_NUM_TCP_PCB --- TCP协议控制块的数量
//MEMP_NUM_TCP_PCB_LISTEN ---TCP监听连接数
//MEMP_NUM_TCP_SEG --- TCP排队数量
//MEMP_NUM_REASSDATA --- IP数据包排队数量
//MEMP_NUM_ARP_QUEUE --- arp队列数
//MEMP_NUM_IGMP_GROUP --- 组播网络接口数
//MEMP_NUM_SYS_TIMEOUT --- 系统超时设置
//MEMP_NUM_NETBUF --- netbuf结构数
//MEMP_NUM_NETCONN --- netconn结构数
//MEMP_NUM_TCPIP_MSG_API --- tcpip消息数
//MEMP_NUM_TCPIP_MSG_INPKT --- tcpip数据包相关设置
//PBUF_POOL_SIZE --- pbuf池大小
//LWIP_ARP ---功能开启关闭
//ARP_TABLE_SIZE --- ip地址缓冲对数量
//ARP_QUEUEING --- arp队列
//ETHARP_TRUST_IP_MAC ---arp ip mac先关
//IP_FORWARD --- ip包转发
//IP_OPTIONS_ALLOWED ---ip选项允许
//IP_REASSEMBLY --- IP断点续传
//IP_FRAG --- IP断点发送
//IP_REASS_MAXAGE --- 分段数据包最大时间
//IP_REASS_MAX_PBUFS --- 分段组装条数
//IP_FRAG_USES_STATIC_BUF --- 使能静态缓冲区
//IP_FRAG_MAX_MTU ---IP最大缓冲数量
//IP_DEFAULT_TTL --- IP最大ttl时间
//IP_SOF_BROADCAST --- ip sof广播
//IP_SOF_BROADCAST_RECV --- spf广播过滤
//LWIP_ICMP --- ICMP使能
//ICMP_TTL --- ICMP最大ttl时间
//LWIP_BROADCAST_PING --- 是否响应广播ping
//LWIP_MULTICAST_PING --- 是否响应组播ping
//LWIP_RAW ---raw协议使能
//RAW_TTL --- raw ttl时间
//LWIP_DHCP --- DHCP使能
//DHCP_DOES_ARP_CHECK --- arp地址检查
//LWIP_AUTOIP --- 自动ip使能
//LWIP_DHCP_AUTOIP_COOP --- dhcp 自动ip接口
//WIP_DHCP_AUTOIP_COOP_TRIES --- dhcp发现数
//LWIP_SNMP --- snmp使能
//SNMP_CONCURRENT_REQUESTS ---snmp并发数
//SNMP_TRAP_DESTINATIONS --- snmp 陷阱相关
//SNMP_PRIVATE_MIB --- snmp 私用mib
//SNMP_SAFE_REQUESTS --- snmp安全相关
//LWIP_IGMP --- IGMP设置
//LWIP_DNS --- DNS使能
//DNS_TABLE_SIZE --- dns最大记录数
//DNS_MAX_NAME_LENGTH --- dns主机名最大长度
//DNS_MAX_SERVERS --- 最大dns服务数
//DNS_DOES_NAME_CHECK --- dns 响应校验
//DNS_USES_STATIC_BUF --- dns本地缓存数
//DNS_MSG_SIZE --- dns消息大小
//DNS_LOCAL_HOSTLIST --- dns 主机列表
//DNS_LOCAL_HOSTLIST_IS_DYNAMIC --- dns动态列表
//LWIP_UDP --- udp使能
//LWIP_UDPLITE --- udp数据报
//UDP_TTL --- udp ttl时间
//LWIP_TCP --- TCP 使能
//TCP_TTL --- TCP TTL时间
//TCP_WND --- TCP窗口长度
//TCP_MAXRTX --- TCP最大重传次数
//TCP_SYNMAXRTX --- TCP syn段最大重传次数
//TCP_QUEUE_OOSEQ --- TCP队列到达顺序 应该是排序相关
//TCP_MSS --- TCP最大段大小
//TCP_CALCULATE_EFF_SEND_MSS --- 用于限制mms
//TCP_SND_BUF --- TCP发送缓冲空间（字节）
//TCP_SND_QUEUELEN ---TCP发送缓冲空间（pbufs）
//TCP_SNDLOWAT --- TCP写空间（字节）
//TCP_LISTEN_BACKLOG --- 启用TCP积压选择听协议控制块
//TCP_DEFAULT_LISTEN_BACKLOG --- 允许最大协议控制块监听数
//LWIP_TCP_TIMESTAMPS ---支持TCP时间戳选项
//TCP_WND_UPDATE_THRESHOLD --- 窗口更新与触发
//LWIP_EVENT_API LWIP_CALLBACK_API --- 只有一个为1 由系统控制或有回调函数控制
//PBUF_LINK_HLEN --- 链路层头
//PBUF_POOL_BUFSIZE --- pbuf池大小
//LWIP_NETIF_HOSTNAME --- 使用dhcp_option_hostname用netif的主机名
//lwip_netif_api --- 是否支持netif API，1为支持
//LWIP_NETIF_STATUS_CALLBACK --- 回调函数支持接口
//lwip_netif_link_callback --- 链路故障相关
//lwip_netif_hwaddrhint --- 缓存链路层地址的提示
//lwip_netif_loopback --- 支持发送数据包的目的地IP
//lwip_loopback_max_pbufs --- 在队列的最大数量的pbufs回送
//LWIP_NETIF_LOOPBACK_MULTITHREADING --- 是否启用多线程
//LWIP_NETIF_TX_SINGLE_PBUF --- 单一Pbuf发送
//LWIP_HAVE_LOOPIF --- 支持回送地址(127.0.0.1) and loopif.c
//LWIP_HAVE_SLIPIF --- 支持滑动界面和slipif C
//TCPIP_THREAD_NAME --- 分配给主要的TCPIP线程的名称
//tcpip_thread_stacksize --- 由主线程使用TCPIP栈大小
//TCPIP_THREAD_PRIO --- 分配给主要的TCPIP线程优先级
//tcpip_mbox_size --- 用于tcpip线程消息邮箱大小
//SLIPIF_THREAD_NAME --- SLIPIF 线程名
//SLIPIF_THREAD_STACKSIZE --- 分配给slipif_loop线程优先级
//PPP_THREAD_NAME --- PPP协议线程名称
//PPP_THREAD_STACKSIZE --- 由pppmain线程使用的堆栈大小
//ppp_thread_prio --- 分配给pppmain线程优先级
//DEFAULT_THREAD_NAME --- 默认LWIP线程名
//DEFAULT_THREAD_STACKSIZE --- 缺省堆栈大小
//default_thread_prio --- 分配给任何其他LwIP线程优先级
//default_raw_recvmbox_size --- 其他raw邮箱大小
//DEFAULT_UDP_RECVMBOX_SIZE --- 其他udp邮箱大小
//DEFAULT_TCP_RECVMBOX_SIZE --- 其他tcp邮箱大小
//DEFAULT_ACCEPTMBOX_SIZE --- ACCEPTMBOX大小
//lwip_tcpip_core_locking --- （实验！）平时不用
//lwip_netconn --- 使能netconn API（需要使用api_lib。C）
//lwip_socket --- 使Socket API （需要sockets.c）
//lwip_compat_sockets --- BSD套接字函数名称
//lwip_posix_sockets_io_names ---使POSIX风格的套接字函数名称posix系统
//LWIP_TCP_KEEPALIVE --- 死链检测
//LWIP_SO_RCVBUF ---使能阻塞处理程序
//RECV_BUFSIZE_DEFAULT --- 若使能阻塞处理，则该设置为阻塞缓冲大小
//SO_REUSE --- 重发送
//lwip_stats --- 使能lwip_stats统计
//LWIP_STATS_DISPLAY --- 编译输出统计功能
//LINK_STATS --- 使能连接统计
//ETHARP_STATS --- 使能arp统计
//IP_STATS --- 使能ip统计
//IPFRAG_STATS --- ip碎片统计
//ICMP_STATS --- icmp统计
//IGMP_STATS ---igmp统计
//UDP_STATS --- udp统计
//TCP_STATS --- tcp统计
//MEM_STATS ---内存管理统计
//MEMP_STATS --- 内存链表统计
//SYS_STATS --- 系统统计
//PPP_SUPPORT --- 使能ppp协议
//PPPOE_SUPPORT --- 使能pppoe协议
//PPPOS_SUPPORT --- 使能pppos协议
//NUM_PPP --- 最大ppp会话
//PAP_SUPPORT --- 使能pap密码认证协议
//CHAP_SUPPORT--- 使能chap协议
//MSCHAP_SUPPORT --- 使能mschap目前不支持，不设置
//CBCP_SUPPORT --- 使能CBCP 目前不支持，不设置
//CCP_SUPPORT --- 使能ccp 目前不支持，不设置
//VJ_SUPPORT --- 使能 支持该报头压缩
//md5_support --- 支持MD5
//FSM_DEFTIMEOUT --- 贞退出时间
//FSM_DEFMAXTERMREQS --- 最大终止请求传输
//FSM_DEFMAXCONFREQS --- 最大配置请求传输
//FSM_DEFMAXNAKLOOPS --- NAK环的最大数量
//UPAP_DEFTIMEOUT --- 重发请求超时（秒）
//UPAP_DEFREQTIME --- 时间等待认证请求同行
//CHAP_DEFTIMEOUT --- chap默认退出时间
//CHAP_DEFTRANSMITS --- 竞争最大发送时间
//LCP_ECHOINTERVAL --- KeepAlive回送请求之间间隔，0秒为禁用
//LCP_MAXECHOFAILS --- 故障之前悬而未决的回声请求数
//PPP_MAXIDLEFLAG --- 最大发射空闲时间（在经过前发送标志字符）
//CHECKSUM_GEN_IP --- 生成ip包校验
//CHECKSUM_GEN_UDP --- 生成udp包校验
//CHECKSUM_GEN_TCP --- 生成tcp包校验
//CHECKSUM_CHECK_IP ---传入ip包校验
//CHECKSUM_CHECK_UDP ---传入udp包校验
//CHECKSUM_CHECK_TCP --- 传入tcp包校验
//以下是各种调试
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
