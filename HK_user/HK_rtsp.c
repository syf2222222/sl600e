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

#define TCP_CLIENT_RX_BUFSIZE	2000	//���ջ���������
//#define REMOTE_PORT				8087	//����Զ��������IP��ַ
//#define LWIP_SEND_DATA			0X80    //���������ݷ���

struct netconn *http_clientconn,*rtsp_clientconn;					//TCP CLIENT�������ӽṹ��
u8 http_client_recvbuf[TCP_CLIENT_RX_BUFSIZE];	//TCP�ͻ��˽������ݻ�����
u8 http_client_sendbuf[1000]="Explorer STM32F407 NETCONN TCP Client send data\r\n";	//TCP�ͻ��˷������ݻ�����

u8 rtsp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE];	//TCP�ͻ��˽������ݻ�����
u8 rtsp_client_sendbuf[1000]="Explorer STM32F407 NETCONN TCP Client send data\r\n";	//TCP�ͻ��˷������ݻ�����

uint8_t http_client_flag,rtsp_client_flag;		//TCP�ͻ������ݷ��ͱ�־λ

//TCP�ͻ�������
#define TCPCLIENT_PRIO		6
//�����ջ��С
#define TCPCLIENT_STK_SIZE	300
//�����ջ
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
					printf("\r\n\r\n����Ϊ��֤����....................");
						printf("\r\n%s",http_buf);
					printf("\r\n����Ϊ��֤����....................");
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



//tcp�ͻ���������
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
		http_clientconn = netconn_new(NETCONN_TCP);  //����һ��TCP����
		err = netconn_connect(http_clientconn,&server_ipaddr,server_port);//���ӷ�����
		
		printf("\r\n֨-3��");
		if(err != ERR_OK)  netconn_delete(http_clientconn); //����ֵ������ERR_OK,ɾ��tcp_clientconn����
		else if (err == ERR_OK)    //���������ӵ�����
		{ 
			struct netbuf *recvbuf;
			http_clientconn->recv_timeout = 1000;
			netconn_getaddr(http_clientconn,&loca_ipaddr,&loca_port,1); //��ȡ����IP����IP��ַ�Ͷ˿ں�
			printf("�����Ϸ�����%d.%d.%d.%d,�����˿ں�Ϊ:%d\r\n",192,168, 1,20,loca_port);
			while(1)
			{
				http_sent_test(http_client_sendbuf);
				
				if((http_client_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) //������Ҫ����
				{
					printf("׼������1\r\n");
					err = netconn_write(http_clientconn ,http_client_sendbuf,strlen((char*)http_client_sendbuf),NETCONN_COPY); //����http_server_sentbuf�е�����
					if(err != ERR_OK)
					{
						printf("����ʧ��\r\n");
					}
					http_client_flag &= ~LWIP_SEND_DATA;
				}
					
				if((recv_err = netconn_recv(http_clientconn,&recvbuf)) == ERR_OK)  //���յ�����
				{	
					printf("׼������\r\n");
//					OS_ENTER_CRITICAL(); //���ж�
					memset(http_client_recvbuf,0,TCP_CLIENT_RX_BUFSIZE);  //���ݽ��ջ���������
					for(q=recvbuf->p;q!=NULL;q=q->next)  //����������pbuf����
					{
						//�ж�Ҫ������TCP_CLIENT_RX_BUFSIZE�е������Ƿ����TCP_CLIENT_RX_BUFSIZE��ʣ��ռ䣬�������
						//�Ļ���ֻ����TCP_CLIENT_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
						if(q->len > (TCP_CLIENT_RX_BUFSIZE-data_len)) memcpy(http_client_recvbuf+data_len,q->payload,(TCP_CLIENT_RX_BUFSIZE-data_len));//��������
						else memcpy(http_client_recvbuf+data_len,q->payload,q->len);
						data_len += q->len;  	
						if(data_len > TCP_CLIENT_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����	
					}
					
					http_rev_data(http_client_recvbuf,data_len);
//					OS_EXIT_CRITICAL();  //���ж�
					data_len=0;  //������ɺ�data_lenҪ���㡣					

					netbuf_delete(recvbuf);
				}else if(recv_err == ERR_CLSD)  //�ر�����
				{
					printf("�ر�����\r\n");
					netconn_close(http_clientconn);
					netconn_delete(http_clientconn);
					printf("������%d.%d.%d.%d�Ͽ�����\r\n",192,168, 1,11);
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
	printf("�յ�rtsp\r\n%s\r\n",buf); //
	
	
}


//tcp�ͻ���������
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
		rtsp_clientconn = netconn_new(NETCONN_TCP);  //����һ��TCP����
		err = netconn_connect(rtsp_clientconn,&server_ipaddr,server_port);//���ӷ�����

		printf("\r\nT-3");
		if(err != ERR_OK)  netconn_delete(rtsp_clientconn); //����ֵ������ERR_OK,ɾ��rtsp_clientconn����
		else if (err == ERR_OK)    //���������ӵ�����
		{ 
			struct netbuf *recvbuf;
			rtsp_clientconn->recv_timeout = 1000;
			netconn_getaddr(rtsp_clientconn,&loca_ipaddr,&loca_port,1); //��ȡ����IP����IP��ַ�Ͷ˿ں�
			printf("�����Ϸ�����%d.%d.%d.%d,�����˿ں�Ϊ:%d\r\n",192,168, 1,11,loca_port);
			while(1)
			{			
				rtsp_sent_test(rtsp_client_sendbuf);
				
				if((rtsp_client_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) //������Ҫ����
				{
					printf("׼������1\r\n");
					err = netconn_write(rtsp_clientconn ,rtsp_client_sendbuf,strlen((char*)rtsp_client_sendbuf),NETCONN_COPY); //����rtsp_server_sentbuf�е�����
					if(err != ERR_OK)
					{
						printf("����ʧ��\r\n");
					}
					rtsp_client_flag &= ~LWIP_SEND_DATA;
				}
					
				if((recv_err = netconn_recv(rtsp_clientconn,&recvbuf)) == ERR_OK)  //���յ�����
				{	
					printf("׼������\r\n");
//					OS_ENTER_CRITICAL(); //���ж�
					memset(rtsp_client_recvbuf,0,TCP_CLIENT_RX_BUFSIZE);  //���ݽ��ջ���������
					for(q=recvbuf->p;q!=NULL;q=q->next)  //����������pbuf����
					{
						//�ж�Ҫ������TCP_CLIENT_RX_BUFSIZE�е������Ƿ����TCP_CLIENT_RX_BUFSIZE��ʣ��ռ䣬�������
						//�Ļ���ֻ����TCP_CLIENT_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
						if(q->len > (TCP_CLIENT_RX_BUFSIZE-data_len)) memcpy(rtsp_client_recvbuf+data_len,q->payload,(TCP_CLIENT_RX_BUFSIZE-data_len));//��������
						else memcpy(rtsp_client_recvbuf+data_len,q->payload,q->len);
						data_len += q->len;  	
						if(data_len > TCP_CLIENT_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����	
					}
					rtsp_rev_data(rtsp_client_recvbuf,data_len);
					
//					OS_EXIT_CRITICAL();  //���ж�
					data_len=0;  //������ɺ�data_lenҪ���㡣					
					netbuf_delete(recvbuf);
				}else if(recv_err == ERR_CLSD)  //�ر�����
				{
					printf("�ر�����\r\n");
					netconn_close(rtsp_clientconn);
					netconn_delete(rtsp_clientconn);
					printf("������%d.%d.%d.%d�Ͽ�����\r\n",192,168, 1,11);
					break;
				}
			}
		}
	}
}
void hk_Task(void)
{
	xTaskCreate(  http_hk_thread,        /* ������ָ�� */             
									"http_client_thread",               /* ������������ʹ�� */                  
									500,                    /* ջ�� */              
									NULL,                   /* ������� */                 
									2,                          /* ���ȼ�. */           
									NULL );                /* ���� handle */
	
	xTaskCreate(  rtsp_hk_thread,        /* ������ָ�� */             
									"rtsp_hk_thread",               /* ������������ʹ�� */                  
									500,                    /* ջ�� */              
									NULL,                   /* ������� */                 
									2,                          /* ���ȼ�. */           
									NULL );                /* ���� handle */
}


