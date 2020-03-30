

#include "lwip.h"
#include <stdio.h>
#include <string.h>


#include "tcp_client.h"
#include "api.h"
#include "global.h"
#include "hk.h"

#define TCP_CLIENT_RX_BUFSIZE	2048	//���ջ���������  //�յ������2048
struct netconn *hk_ch1_conn,*rtsp_clientconn;					//TCP CLIENT�������ӽṹ��

uint8_t http_recvbuf[TCP_CLIENT_RX_BUFSIZE];	//TCP�ͻ��˽������ݻ�����
//uint8_t http_client_sendbuf[1000]="Explorer STM32F407 NETCONN TCP Client send data\r\n";	//TCP�ͻ��˷������ݻ�����


//uint8_t http_client_flag;		//TCP�ͻ������ݷ��ͱ�־λ
extern uint8_t http_sent_flag;												
extern OS_SEM http_rev_sem;
void http_data_post(char * in,uint16_t len);
char bstop=0,recvFIN=0;
extern uint16_t power_delay_hk;
uint8_t http_link=0,http_linkd=0;
void http_clientconn_ok(void);
void http_data_post(char * in,uint16_t len);
char check_link(void);
extern int hk_wait_time;

static void netconn_stop(void)
{
	netconn_close(hk_ch1_conn);
	netconn_delete(hk_ch1_conn);
	http_link=0;
//	bstop=1;
}
#define server_port	80

uint8_t NBLOCK_flag=0,netconn_connect_time,first_link;
extern uint16_t cam_on_time_detect;

void http_hk_thread(void *arg)
{
	static ip_addr_t server_ipaddr,loca_ipaddr;
	static u16_t 		 loca_port;
	err_t err;
	OS_ERR      oserr;

//	hk_ch1_conn->recv_timeout = 10;	
	(void)arg;//
	
	IP4_ADDR(&server_ipaddr, CH1IP[0],CH1IP[1], CH1IP[2],CH1IP[3]);//IP4_ADDR(&server_ipaddr, 192,168, 1,11);	
//	sprintf(ip_host,"%d.%d.%d.%d",LoclIP[0],LoclIP[1],LoclIP[2],LoclIP[3]);
//	printf(ip_host,"%d.%d.%d.%d",IP_ADDR0,IP_ADDR1,IP_ADDR2,IP_ADDR3);
	
//	OSTimeDly ( 3000, OS_OPT_TIME_DLY, & oserr ); 
//	hk_ch1_conn = netconn_new(NETCONN_TCP);  //����һ��TCP����
	while (1) 
	{
		if(http_link)
		{
			if(http_linkd==0) //�����ӹ����йص�
			{
//				netconn_delete(hk_ch1_conn); //ɾ��tcp_clientconn����
				netconn_stop();
			}
			else
			{				
				hk_ch1_conn->recv_timeout = 1000;	
				http_clientconn_ok(); //������ϵ����
			}
		}
		else
		{
			hk_ch1_conn->recv_timeout = 1000;	
			OSTimeDly ( 1000, OS_OPT_TIME_DLY, & oserr ); 
			
			if(power_flag_cam1==0)
			{
//				if(hk_power_delay<hk_power_on_time)	power_set_cam1(0);
//				power_delay_hk=hk_power_on_time;//�ص�״̬�� delay��ֵ�ȸ������
				
//				netbuf_delete();
				
				http_linkd=0;
			}
			else	if(power_delay_hk)
			{
				http_linkd=0;
				hk_wait_time=0;
				printf(","); //���������ʱ���
			}
			else if(http_sent_flag) //��ʱ��, ������Ҫ����
			{
//				int flags = lwip_fcntl(sock, F_GETFL, 0); 
//				fcntl(sock, F_SETFL, flags | O_NONBLOCK); 
//				fcntl
//				tcp_write(pcb_uart2,TCP_Data,1,0);
				
				hk_ch1_conn = netconn_new(NETCONN_TCP);  //����һ��TCP����
//				printf("?");
				NBLOCK_flag=1;
				netconn_connect_time=0;
				err = netconn_connect(hk_ch1_conn,&server_ipaddr,server_port);//���ӷ�����
				NBLOCK_flag=2;
				if(netconn_connect_time>=10) 
				{
					first_link=1;
					OSTimeDly ( 14000, OS_OPT_TIME_DLY, & oserr );  //������ϵ���ʱ���� 12s��70%��ֱ�Լ���  13s,1/15 ��ֱ�Լ�
				}
				NBLOCK_flag=0;
				
				if(err != ERR_OK) 
				{
					netconn_delete(hk_ch1_conn); //����ֵ������ERR_OK,ɾ��tcp_clientconn����
					http_linkd=0;		//�ⲿ״̬��ʾ
					printf("!");//�������ʧ�ܱ��  printf("http����ʧ��");
				}
				else
				{
					http_link=1;		//�����߼�״̬
					http_linkd=1;		//�ⲿ״̬��ʾ
					netconn_getaddr(hk_ch1_conn,&loca_ipaddr,&loca_port,1); //��ȡ����IP����IP��ַ�Ͷ˿ں�
					printf("$");//������ӳɹ����
					if(first_link) 
					{
						first_link=0;
						printf("\r\n�������OK,��ʱ%ds",cam_on_time_detect/10);//������ӳɹ����
					}
//					printf("?%d",cam_on_time_detect/10);
//					printf("\r\n����OK,�˿ں�:%d",loca_port);
//				printf("\r\n�����Ϸ�������%d.%d.%d.%d",DEST_IP_ADDR0,DEST_IP_ADDR1, DEST_IP_ADDR2,DEST_IP_ADDR3);
//				printf("\r\n����IP��%d.%d.%d.%d,�����˿ں�:%d",((loca_ipaddr.addr >> 0)&0x0ff),((loca_ipaddr.addr>>8)&0x0ff), (loca_ipaddr.addr>>16)&0x0ff,(loca_ipaddr.addr>>24)&0x0ff,loca_port);
				}
			}
		}
	}
}

int http_test_sent=0;
void load_http_sendbuf(char *sttp_buf,uint8_t Cmd,uint8_t *Par,char ch);

struct pbuf *q;
struct netbuf *recvbuf;



void http_clientconn_ok()
{
	CPU_SR_ALLOC(); 
	err_t err;
	OS_ERR      oserr;
	uint32_t data_len = 0;
	
	if(http_sent_flag) //������Ҫ����
	{
//			printf("\r\n׼������");
		err = netconn_write(hk_ch1_conn ,http_sendbuf,strlen((char*)http_sendbuf),NETCONN_COPY); //����http_server_sentbuf�е����� NETCONN_NOCOPY NETCONN_COPY
		if(err != ERR_OK)
		{
			printf("\r\nhttp����ʧ��\r\n");
			netconn_stop();
			return;
		}
		else
		{
			http_sent_flag=0;
		}
	}
	
	err = netconn_recv(hk_ch1_conn,&recvbuf);		
	if(err == ERR_OK)  //���յ�����
	{
		data_len=0;  //������ɺ�data_lenҪ���㡣	
		memset(http_recvbuf,0,TCP_CLIENT_RX_BUFSIZE);  //���ݽ��ջ���������			
		CPU_CRITICAL_ENTER(); //���ж�

		for(q=recvbuf->p;q!=NULL;q=q->next)  //����������pbuf����
		{
			if((q->len > (TCP_CLIENT_RX_BUFSIZE-data_len-1))||(q->len==0))
			{
				recvFIN=1;				
				break; //�յ���FIN��
			}
			else
			{
				memcpy(http_recvbuf+data_len,q->payload,q->len);
				data_len += q->len;
			}
		}

		CPU_CRITICAL_EXIT();//���ж�	
		netbuf_delete(recvbuf);
		if(recvFIN)
		{
			recvFIN=0;
			netconn_stop();
//				printf("\r\n������%d.%d.%d.%d�Ͽ�����\r\n",DEST_IP_ADDR0,DEST_IP_ADDR1, DEST_IP_ADDR2,DEST_IP_ADDR3);
		}
		else	if(data_len>0)	
		{
			http_data_post((char *)http_recvbuf,data_len);
		}
	}
	else if(err == ERR_CLSD)  //�ر�����
	{
		netconn_stop();
//			printf("������%d.%d.%d.%d�Ͽ�����\r\n",DEST_IP_ADDR0,DEST_IP_ADDR1, DEST_IP_ADDR2,DEST_IP_ADDR3);
	}
	else
	{
		OSTimeDly ( 3, OS_OPT_TIME_DLY, & oserr ); 
	}
}
		







//void http_rev_data(char * in,uint16_t len)
//{
//	int i;
//	OS_ERR      oserr;
//	
//	http_data_post(in,len);
//	return;
//	
////	printf("��http\r\n%s\r\n",buf);
//	if(strncmp("HTTP/1.1 401", (char *)in,12)==0)
//	{
//		printf("�յ�\r\n%s\r\n",in); //�յ�����http
//		get_digest_info(in);
////		OSTimeDly ( 200, OS_OPT_TIME_DLY, & oserr ); 
//		sent_to_camera((char *)http_client_sendbuf,method,uri,strA,body); //������֤��Ϣ�ط�
//		cnt=0;
//	}
//	else	if(strncmp("HTTP/1.1 200 OK", (char *)in,12)==0) //��֤���
//	{
//		r200=1;
//		cnt=2;
////HTTP/1.1 200 OK
////Content-Type: image/jpeg
////Connection: close
////Content-Length:124378		
//			
//		if(waiting_jpeg==0)
//		{
//				waiting_jpeg=1;
//				rnumb=0;
//				datalen_buf=Content_Length(&in[20]);
//				printf("��������=%d",datalen_buf);	
////				PH_flag[0]=0;
////				PH_LEN[0]=(datalen_buf>>16);
////				PH_LEN[1]=datalen_buf;
//				datalen_r=0;
//		}
//		printf("��\r\n%s\r\n",in); //�յ�����http
//		
//	}
//	else if(waiting_jpeg==1)
//	{
//		rnumb++;
//		
////		memcpy(PH_DATA+datalen_r,in,len);
//		datalen_r +=len;
//	
//		if(datalen_r>=datalen_buf)
//		{
//			waiting_jpeg=2;
//			cmdcnt=30;//���¶�����
////			PH_flag[0]=1; //ͼƬ��������
//		}
//	
//		printf("\r\n");
//		for(i=0;i<5;i++)	printf("%x",in[i]); //�յ�����http
//		printf(".");
//		for(i=len-5;i<len;i++)	printf("%x",in[i]); //�յ�����http
//	}
//}


//void http_sent_test(char * sentbuf)
//{
//	cnt++;
//	printf("֨A%x,",cnt);
//	if(cmdcnt>=30) 
//	{
//		if(cmdcnt<32) cmdcnt++;
//		else
//		{
//			havcmd=1;
//			strcpy((char *)method,"GET");
//			strcpy((char *)uri,"/onvif-http/snapshot?Profile_1");//strcpy((char *)uri,"/PTZCtrl/channels/1/presets/1/goto");//strcpy((char *)uri,"/onvif-http/snapshot?Profile_1");
//			strcpy((char *)strA,"Accept: image/jpeg\r\n");//strcpy((char *)str,"\0");//str[0]=0;//	image/jpeg
//			strcpy((char *)body,"\0");
//		
//			step=0;
//			cnt=0;
//			cmdcnt=0;
//			waiting_jpeg=0;
//		}
//	}
//	
//	if(step==0)
//	{
//		sent_to_camera(sentbuf,method,uri,strA,body);
//		cmdcnt=0;
//		step++;
//		cnt=0;
//	}
//	else if(step==1) //
//	{
//		if(cnt>=20) //�ط�
//		{
//			sent_to_camera(sentbuf,method,uri,strA,body);
//			cnt=0;
//		}
//		
//		if(r200)
//		{
//			step=10; 
//			r200=0;
//			cnt=0;
//		}
//	}
//	else	if(step==10)
//	{
////		cmdcnt++;
//	}
//}

