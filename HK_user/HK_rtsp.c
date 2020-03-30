#include "main.h"
#include "lwip.h"

#include <stdio.h>
#include <string.h>
#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/timeouts.h"
#include "netif/etharp.h"
#include "ethernetif.h"

#include "DM9000.h"

#include "lwip/timeouts.h"
#include "tcp_client.h"

#include "MD5.h"
#include "api.h"


//#define HTTP

//#ifdef HTTP
//#define ch1_port	80
//#else
//#define ch1_port	554
//#endif

#define TCP_CLIENT_RX_BUFSIZE	2000	//接收缓冲区长度
//#define REMOTE_PORT				8087	//定义远端主机的IP地址
//#define LWIP_SEND_DATA			0X80    //定义有数据发送

struct netconn *http_clientconn,*rtsp_clientconn;					//TCP CLIENT网络连接结构体
u8 http_client_recvbuf[TCP_CLIENT_RX_BUFSIZE];	//TCP客户端接收数据缓冲区
u8 http_client_sendbuf[1000]="Explorer STM32F407 NETCONN TCP Client send data\r\n";	//TCP客户端发送数据缓冲区

u8 rtsp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE];	//TCP客户端接收数据缓冲区
u8 rtsp_client_sendbuf[1000]="Explorer STM32F407 NETCONN TCP Client send data\r\n";	//TCP客户端发送数据缓冲区

uint8_t http_client_flag,rtsp_client_flag;		//TCP客户端数据发送标志位

//TCP客户端任务
#define TCPCLIENT_PRIO		6
//任务堆栈大小
#define TCPCLIENT_STK_SIZE	300
//任务堆栈
//OS_STK TCPCLIENT_TASK_STK[TCPCLIENT_STK_SIZE];

extern uint8_t http_buf[1024];

int camera_authorization(const uint8_t *in, uint16_t in_len,
							const uint8_t *method, uint16_t method_len,
							const uint8_t *uri, uint16_t uri_len,
							const uint8_t *ip_host, uint8_t ip_host_len,
							const uint8_t *str, uint16_t str_len,
							struct netconn *conn);
								
extern int method_head_pkg(const uint8_t *method, uint16_t method_len,
					const uint8_t *uri, uint16_t uri_len,
					const uint8_t *ip_host, uint8_t ip_host_len,
					const uint8_t *str, uint16_t str_len,
					uint8_t *out, uint16_t *p_out_len);
					
void camera_aut_test(uint8_t *buf)
{
	int buf32;
	//HTTP/1.1 401 Unauthorized
//Date: Fri, 27 Sep 2019 14:21:46 GMT
//Server: webserver
//Content-Length: 255
//Content-Type: text/html
//Connection: keep-alive
//Keep-Alive: timeout=10, max=99
//WWW-Authenticate: Digest qop="auth", realm="IP Camera(C5654)", nonce="4d3249794e324d305a6d4d364e7a4d784f5459795a47493d", stale="FALSE"
	
//					strcpy((char *)http_client_recvbuf,"HTTP/1.1 401 Unauthorized\r\n");
//					strcat((char *)http_client_recvbuf,"Date: Fri, 27 Sep 2019 14:21:46 GMT\r\n");
//					strcat((char *)http_client_recvbuf,"Server: webserver\r\n");//1094-1095
//					strcat((char *)http_client_recvbuf,"Content-Length: 255\r\n");
//					
//					strcat((char *)http_client_recvbuf,"Content-Type: text/html\r\n");
//					strcat((char *)http_client_recvbuf,"Connection: keep-alive\r\n");
//					strcat((char *)http_client_recvbuf,"Keep-Alive: timeout=10, max=99\r\n");
//					strcat((char *)http_client_recvbuf,"WWW-Authenticate: Digest qop=\"auth\", realm=\"IP Camera(C5654)\", nonce=\"4d3249794e324d305a6d4d364e7a4d784f5459795a47493d\", stale=\"FALSE\"\r\n");
//					
//					strcat((char *)http_client_recvbuf,"\r\n\r\n");

//strcpy((char *)http_client_sendbuf,"GET /onvif-http/snapshot?Profile_1 HTTP/1.1\r\n");
//						strcat((char *)http_client_sendbuf,"Host: 192.168.1.11\r\n");
//						strcat((char *)http_client_sendbuf,"Connection: Keep-Alive\r\n");
//						strcat((char *)http_client_sendbuf,"\r\n\r\n");

//					buf32=camera_authorization((uint8_t *)http_client_recvbuf, strlen((char *)http_client_recvbuf),
//							(uint8_t *)"GET", strlen("GET"),
//							(uint8_t *)"/onvif-http/snapshot?Profile_1", strlen("/onvif-http/snapshot?Profile_1"),
//							(uint8_t *)"192.168.1.11",strlen("192.168.1.11"),
//							NULL,0,//);//const uint8_t *str, uint16_t str_len,
//							http_clientconn);
					
					buf32=camera_authorization((uint8_t *)buf, strlen((char *)buf),
							(uint8_t *)"PUT", strlen("PUT"),
							(uint8_t *)"/PTZCtrl/channels/1/presets/1/goto", strlen("/PTZCtrl/channels/1/presets/1/goto"),
							(uint8_t *)"192.168.1.11",strlen("192.168.1.11"),
							NULL,0,//);//const uint8_t *str, uint16_t str_len,
							http_clientconn);
					
					
//				buf32=method_head_pkg((uint8_t *)"GET", strlen("GET"),
//										(uint8_t *)"/onvif-http/snapshot?Profile_1", strlen("/onvif-http/snapshot?Profile_1"),
//										(uint8_t *)"192.168.1.11",strlen("192.168.1.11"),
//										NULL,0,
//										http_client_sendbuf,&out_len);
					printf("\r\nbuf32=%x",buf32);
					printf("\r\n\r\n以下为认证数据....................");
						printf("\r\n%s",http_buf);
					printf("\r\n以上为认证数据....................");
}

//uint16_t EXTI2_flag=0,EXTI2_flag2;
char authorization=0;

void load_sent_data(uint8_t *buf)
{
#if 1
					
//					if(authorization==1)
//					{
//						strcpy((char *)buf,(char *)http_buf);
//						authorization=0;
//					}
//					else
					{
						strcpy((char *)buf,"PUT /PTZCtrl/channels/1/presets/1/goto HTTP/1.1\r\n");
						strcat((char *)buf,"Host: 192.168.1.11\r\n");
						strcat((char *)buf,"Connection: Keep-Alive\r\n");
						strcat((char *)buf,"\r\n\r\n");

//						strcpy((char *)buf,"GET /onvif-http/snapshot?Profile_1 HTTP/1.1\r\n");
//						strcat((char *)buf,"Host: 192.168.1.11\r\n");
//						strcat((char *)buf,"Connection: Keep-Alive\r\n");
//						strcat((char *)buf,"\r\n\r\n");
					}
					
		//GET /onvif-http/snapshot?Profile_1 HTTP/1.1\r\n
		//Host: 192.168.146.11\r\n
		//Connection: Keep-Alive\r\n
		//Accept: image/jpeg\r\n\r\n
					
#else			
//					strcpy((char *)tcp_client_sendbuf,"OPTIONS rtsp://192.0.1.100/Streaming/Channels/101 RTSP/1.0\r\n");
//					strcat((char *)tcp_client_sendbuf,"CSeq: 1\r\n");
//					strcat((char *)tcp_client_sendbuf,"User-Agent: Hikplayer\r\n");//1094-1095
//					strcat((char *)tcp_client_sendbuf,"\r\n\r\n");
					
					strcpy((char *)buf,"SETUP rtsp://192.168.1.11/Streaming/Channels/101/trackID=1 RTSP/1.0\r\n");
					strcat((char *)buf,"CSeq: 3\r\n");
					strcat((char *)buf,"Transport: RTP/AVP;unicast;client_port=1094-49653\r\n");//1094-1095
					strcat((char *)buf,"User-Agent: Hikplayer\r\n");
					strcat((char *)buf,"\r\n\r\n");
#endif	
}



//tcp客户端任务函数
void http_hk_thread(void *arg)
{
//	OS_CPU_SR cpu_sr;
	u32 data_len = 0;
	struct pbuf *q;
	err_t err,recv_err;
	static ip_addr_t server_ipaddr,loca_ipaddr;
	static u16_t 		 server_port,loca_port;
	int buf32;
	
	LWIP_UNUSED_ARG(arg);
	server_port = 80;
	IP4_ADDR(&server_ipaddr, 192,168, 1,11);
	
	IP4_ADDR(&loca_ipaddr, 192,168, 1,20);
	loca_port=19530;//49153?	
	vTaskDelay(  pdMS_TO_TICKS(1000) );
	
	while (1) 
	{
		vTaskDelay(  pdMS_TO_TICKS(1000) );
		http_clientconn = netconn_new(NETCONN_TCP);  //创建一个TCP链接
		err = netconn_connect(http_clientconn,&server_ipaddr,server_port);//连接服务器
		
		printf("\r\n吱-3声");
		if(err != ERR_OK)  netconn_delete(http_clientconn); //返回值不等于ERR_OK,删除tcp_clientconn连接
		else if (err == ERR_OK)    //处理新连接的数据
		{ 
			struct netbuf *recvbuf;
			http_clientconn->recv_timeout = 1000;
			netconn_getaddr(http_clientconn,&loca_ipaddr,&loca_port,1); //获取本地IP主机IP地址和端口号
			printf("连接上服务器%d.%d.%d.%d,本机端口号为:%d\r\n",192,168, 1,20,loca_port);
			while(1)
			{
				http_sent_test(http_client_sendbuf);
				
				if((http_client_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) //有数据要发送
				{
					printf("准备发送1\r\n");
					err = netconn_write(http_clientconn ,http_client_sendbuf,strlen((char*)http_client_sendbuf),NETCONN_COPY); //发送http_server_sentbuf中的数据
					if(err != ERR_OK)
					{
						printf("发送失败\r\n");
					}
					http_client_flag &= ~LWIP_SEND_DATA;
				}
					
				if((recv_err = netconn_recv(http_clientconn,&recvbuf)) == ERR_OK)  //接收到数据
				{	
					printf("准备接收\r\n");
//					OS_ENTER_CRITICAL(); //关中断
					memset(http_client_recvbuf,0,TCP_CLIENT_RX_BUFSIZE);  //数据接收缓冲区清零
					for(q=recvbuf->p;q!=NULL;q=q->next)  //遍历完整个pbuf链表
					{
						//判断要拷贝到TCP_CLIENT_RX_BUFSIZE中的数据是否大于TCP_CLIENT_RX_BUFSIZE的剩余空间，如果大于
						//的话就只拷贝TCP_CLIENT_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据
						if(q->len > (TCP_CLIENT_RX_BUFSIZE-data_len)) memcpy(http_client_recvbuf+data_len,q->payload,(TCP_CLIENT_RX_BUFSIZE-data_len));//拷贝数据
						else memcpy(http_client_recvbuf+data_len,q->payload,q->len);
						data_len += q->len;  	
						if(data_len > TCP_CLIENT_RX_BUFSIZE) break; //超出TCP客户端接收数组,跳出	
					}
					
					http_rev_data(http_client_recvbuf,data_len);
//					OS_EXIT_CRITICAL();  //开中断
					data_len=0;  //复制完成后data_len要清零。					

					netbuf_delete(recvbuf);
				}else if(recv_err == ERR_CLSD)  //关闭连接
				{
					printf("关闭连接\r\n");
					netconn_close(http_clientconn);
					netconn_delete(http_clientconn);
					printf("服务器%d.%d.%d.%d断开连接\r\n",192,168, 1,11);
					break;
				}
			}
		}
	}
}
uint8_t rtsp_st;
void rtsp_sent_test(uint8_t * buf)
{
	rtsp_st++;
	printf("T%x,",rtsp_st);
	if((rtsp_st>=2)&&(authorization))
	{
		rtsp_st=0;
		
		rtsp_client_flag |= LWIP_SEND_DATA;
		
		strcpy((char *)buf,"SETUP rtsp://192.168.1.11/Streaming/Channels/101/trackID=1 RTSP/1.0\r\n");
		strcat((char *)buf,"CSeq: 3\r\n");
		strcat((char *)buf,"Transport: RTP/AVP;unicast;client_port=1094-49653\r\n");//1094-1095
		strcat((char *)buf,"User-Agent: Hikplayer\r\n");
		strcat((char *)buf,"\r\n\r\n");
	}
}

void rtsp_rev_data(uint8_t * buf,uint8_t len)
{
	printf("收到rtsp\r\n%s\r\n",buf); //
	
	
}


//tcp客户端任务函数
void rtsp_hk_thread(void *arg)
{
//	OS_CPU_SR cpu_sr;
	u32 data_len = 0;
	struct pbuf *q;
	err_t err,recv_err;
	static ip_addr_t server_ipaddr,loca_ipaddr;
	static u16_t 		 server_port,loca_port;
	int buf32;


	printf("\r\nT-1");
	LWIP_UNUSED_ARG(arg);
	server_port = 554;
	IP4_ADDR(&server_ipaddr, 192,168, 1,11);
	
	IP4_ADDR(&loca_ipaddr, 192,168, 1,20);
	loca_port=19530;//49153?
	
	vTaskDelay(  pdMS_TO_TICKS(1000) );
	printf("\r\nT-2");

	while (1) 
	{
		vTaskDelay(  pdMS_TO_TICKS(1000) );
		rtsp_clientconn = netconn_new(NETCONN_TCP);  //创建一个TCP链接
		err = netconn_connect(rtsp_clientconn,&server_ipaddr,server_port);//连接服务器

		printf("\r\nT-3");
		if(err != ERR_OK)  netconn_delete(rtsp_clientconn); //返回值不等于ERR_OK,删除rtsp_clientconn连接
		else if (err == ERR_OK)    //处理新连接的数据
		{ 
			struct netbuf *recvbuf;
			rtsp_clientconn->recv_timeout = 1000;
			netconn_getaddr(rtsp_clientconn,&loca_ipaddr,&loca_port,1); //获取本地IP主机IP地址和端口号
			printf("连接上服务器%d.%d.%d.%d,本机端口号为:%d\r\n",192,168, 1,11,loca_port);
			while(1)
			{			
				rtsp_sent_test(rtsp_client_sendbuf);
				
				if((rtsp_client_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) //有数据要发送
				{
					printf("准备发送1\r\n");
					err = netconn_write(rtsp_clientconn ,rtsp_client_sendbuf,strlen((char*)rtsp_client_sendbuf),NETCONN_COPY); //发送rtsp_server_sentbuf中的数据
					if(err != ERR_OK)
					{
						printf("发送失败\r\n");
					}
					rtsp_client_flag &= ~LWIP_SEND_DATA;
				}
					
				if((recv_err = netconn_recv(rtsp_clientconn,&recvbuf)) == ERR_OK)  //接收到数据
				{	
					printf("准备接收\r\n");
//					OS_ENTER_CRITICAL(); //关中断
					memset(rtsp_client_recvbuf,0,TCP_CLIENT_RX_BUFSIZE);  //数据接收缓冲区清零
					for(q=recvbuf->p;q!=NULL;q=q->next)  //遍历完整个pbuf链表
					{
						//判断要拷贝到TCP_CLIENT_RX_BUFSIZE中的数据是否大于TCP_CLIENT_RX_BUFSIZE的剩余空间，如果大于
						//的话就只拷贝TCP_CLIENT_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据
						if(q->len > (TCP_CLIENT_RX_BUFSIZE-data_len)) memcpy(rtsp_client_recvbuf+data_len,q->payload,(TCP_CLIENT_RX_BUFSIZE-data_len));//拷贝数据
						else memcpy(rtsp_client_recvbuf+data_len,q->payload,q->len);
						data_len += q->len;  	
						if(data_len > TCP_CLIENT_RX_BUFSIZE) break; //超出TCP客户端接收数组,跳出	
					}
					rtsp_rev_data(rtsp_client_recvbuf,data_len);
					
//					OS_EXIT_CRITICAL();  //开中断
					data_len=0;  //复制完成后data_len要清零。					
					netbuf_delete(recvbuf);
				}else if(recv_err == ERR_CLSD)  //关闭连接
				{
					printf("关闭连接\r\n");
					netconn_close(rtsp_clientconn);
					netconn_delete(rtsp_clientconn);
					printf("服务器%d.%d.%d.%d断开连接\r\n",192,168, 1,11);
					break;
				}
			}
		}
	}
}
void hk_Task(void)
{
	xTaskCreate(  http_hk_thread,        /* 任务函数指针 */             
									"http_client_thread",               /* 任务名：调试使用 */                  
									500,                    /* 栈深 */              
									NULL,                   /* 任务参数 */                 
									2,                          /* 优先级. */           
									NULL );                /* 任务 handle */
	
	xTaskCreate(  rtsp_hk_thread,        /* 任务函数指针 */             
									"rtsp_hk_thread",               /* 任务名：调试使用 */                  
									500,                    /* 栈深 */              
									NULL,                   /* 任务参数 */                 
									2,                          /* 优先级. */           
									NULL );                /* 任务 handle */
}


