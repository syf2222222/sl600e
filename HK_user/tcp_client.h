

#ifndef  __TCP_CLINET_H
#define  __TCP_CLIENT_H


#define LWIP_CONNECTED  0X80 //连接成功
#define LWIP_NEW_DATA   0x40 //有新的数据
#define LWIP_SEND_DATA  0x20 //有数据需要发送
#define LWIP_DEMO_BUF   200

#define LWIP_TCP_SERVER 0x80  //tcp 服务器功能
#define LWIP_TCP_CLIENT 0x40	//tcp 客户端功能
#define LWIP_UDP_SERVER 0x20  //UDP 服务器功能
#define LWIP_UDP_CLIENT 0x10  //UDP 客户端功能
#define LWIP_WEBSERVER  0x08  //UDP 客户端功能

struct tcp_client_state //TCP服务器状态
{
  unsigned char state;

};
	
#define LWIP_DEMO_DEBUG 1  //是否打印调试信息   

void Init_TCP_Client(void);
void Init_RTSP_Client(void);
void tcp_client_connect_remotehost(void); //尝试连接到远程主机

extern unsigned char lwip_flag; 

extern struct tcp_pcb* tcp_client_pcb;
extern struct tcp_pcb* rtsp_client_pcb;

extern unsigned char lwip_demo_buf[200];		//定义用来发送和接收数据的缓存

void tcp_client_close(struct tcp_pcb *tpcb, struct tcp_client_state* ts);

#endif

