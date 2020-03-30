

#ifndef  __TCP_CLINET_H
#define  __TCP_CLIENT_H


#define LWIP_CONNECTED  0X80 //���ӳɹ�
#define LWIP_NEW_DATA   0x40 //���µ�����
#define LWIP_SEND_DATA  0x20 //��������Ҫ����
#define LWIP_DEMO_BUF   200

#define LWIP_TCP_SERVER 0x80  //tcp ����������
#define LWIP_TCP_CLIENT 0x40	//tcp �ͻ��˹���
#define LWIP_UDP_SERVER 0x20  //UDP ����������
#define LWIP_UDP_CLIENT 0x10  //UDP �ͻ��˹���
#define LWIP_WEBSERVER  0x08  //UDP �ͻ��˹���

struct tcp_client_state //TCP������״̬
{
  unsigned char state;

};
	
#define LWIP_DEMO_DEBUG 1  //�Ƿ��ӡ������Ϣ   

void Init_TCP_Client(void);
void Init_RTSP_Client(void);
void tcp_client_connect_remotehost(void); //�������ӵ�Զ������

extern unsigned char lwip_flag; 

extern struct tcp_pcb* tcp_client_pcb;
extern struct tcp_pcb* rtsp_client_pcb;

extern unsigned char lwip_demo_buf[200];		//�����������ͺͽ������ݵĻ���

void tcp_client_close(struct tcp_pcb *tpcb, struct tcp_client_state* ts);

#endif

