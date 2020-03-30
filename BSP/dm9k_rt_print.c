
#include "main.h"
#include <stdio.h>
#include <string.h>

#include "DM9000.h"
#include "lwip.h"

#define eth_h_len	14

struct eth_h
{
	unsigned char dmac[6];    //Destination MAC Ŀ�Ķ�MAC��ַ
	unsigned char smac[6];    //Source MAC ���Ͷ�MAC��ַ
	unsigned char Type[2];		//Э������
};

#define arp_h_len	28
struct arp_h //eth_h.Type=0x0806
{
	unsigned char HwType[2];//Ӳ������(1��ʾ���������̫��MAC��ַ)
	unsigned char PtType[2]; //protocol type //Э������(0x0800��ʾ�������IP��ַ)
	unsigned char HwSize;//Ӳ����ַ����(6)
	unsigned char PtSize;//Э���ַ����(4)
	unsigned char Opcode[2];//����(1��ʾARP����,2��ʾARPӦ��)
	unsigned char Sender_MACadd[6];//���Ͷ�MAC��ַ
	unsigned char Sender_IPadd[4];//���Ͷ�IP��ַ
	unsigned char Target_MACadd[6];//Ŀ�Ķ�MAC��ַ
	unsigned char Target_IPadd[4];//Ŀ�Ķ�IP��ַ
};
#define ip_h_len	20
struct ip_h //��̫��ͷ��+IP�ײ��ṹ
{
	unsigned char vhl;      //4λ�汾��4λ�ײ�����(0x45)
	unsigned char tos;     //��������(0)
	unsigned char len[2];      //����IP���ݱ����ֽڳ���
	unsigned char ipid[2];          //IP��ʶ
	unsigned char ipoffset[2];     //3λ��ʶ13λƫ��
	unsigned char ttl;             //����ʱ��(32��64)
	unsigned char Type;         //Э��(1��ʾICMP,2��ʾIGMP,6��ʾTCP,17��ʾUDP)
	unsigned char ipchksum[2];    //�ײ�У���
	unsigned char Sender_IPadd[4];    //ԴIP
	unsigned char Target_IPadd[4];   //Ŀ��IP
};
#define tcp_h_len	20
struct tcp_h //eth_h.Type=0x0806
{
	unsigned char sport[2];//
	unsigned char dport[2]; //
	unsigned char seqN[4];//sequence number
	unsigned char ackN[4];//Acknowledgment number
	unsigned char hdlen;//
	unsigned char flags;//
	unsigned char winsize[2];//
	unsigned char checksem[2];//
	unsigned char urgpoint[2];//Urgent pointer
};
#define udp_h_len	8
struct udp_h //eth_h.Type=0x0806
{
	unsigned char sport[2];//
	unsigned char dport[2]; //
	unsigned char udplen[2]; //
	unsigned char checksem[2];//
};

uint8_t check_mac(unsigned char mac[6])
{
	
	if((mac[0]==0xff)&&(mac[1]==0xff)&&(mac[2]==0xff)&&
		(mac[3]==0xff)&&(mac[4]==0xff)&&(mac[5]==0xff))
				return 0xff;
	else if((mac[0]==0x02)&&(mac[1]==0)&&(mac[2]==0)&&
		(mac[3]==0)&&(mac[4]==0)&&(mac[5]==0))
				return 0;
	
	else return 1;
}

uint8_t check_ip(unsigned char ip[4])
{
	if((ip[0]==LoclIP[0])&&(ip[1]==LoclIP[1])&&(ip[2]==LoclIP[2])&&(ip[3]==LoclIP[3]))
				return 0;
	else return 1;
}
void printf_len(uint16_t len,uint8_t r_t)
{
	if(r_t==0) printf("\r\nrlen=");
	else
	{
		printf("\r\ntlen=");
	}
	
	printf("%d.",len);
}
void printf_arp(unsigned char *datas,uint16_t len,uint8_t r_t)
{
	struct arp_h *darp_h;
	
	darp_h =(struct arp_h *) datas;

	if((check_ip(darp_h->Target_IPadd)==0)||(r_t==1))
	{
		printf_len(len+eth_h_len,r_t);
//		printf("\r\n");
		if(darp_h->Opcode[1]==1) printf("\twho has %d,%d,%d,%d",darp_h->Target_IPadd[0],darp_h->Target_IPadd[1],darp_h->Target_IPadd[2],darp_h->Target_IPadd[3]);
		else	if(darp_h->Opcode[1]==2) printf("\ti has %d,%d,%d,%d",darp_h->Sender_IPadd[0],darp_h->Sender_IPadd[1],darp_h->Sender_IPadd[2],darp_h->Sender_IPadd[3]);
		else printf("\tOpcode=%x",darp_h->Opcode[1]);
	}
}

//void printf_ICMP(unsigned char *datas,uint16_t len,r_t)
//{
//}
//void printf_UDP(unsigned char *datas,uint16_t len,r_t)
//{
//}
void printf_TCP(unsigned char *datas,uint16_t len,uint8_t r_t)
{
	struct tcp_h *ptcp_h;
	uint16_t port0,port1;
	uint8_t flags;
	uint32_t xlh,qrh;
//	int i;
	
	ptcp_h=(struct tcp_h *)datas;
	
	port0=(ptcp_h->sport[0]<<8)+ptcp_h->sport[1];
	port1=(ptcp_h->dport[0]<<8)+ptcp_h->dport[1];
	printf("\ttcp:%d to %d",port0,port1);
	
	flags=ptcp_h->flags;
	if(flags&0x20)	printf("-URG");//������־λ��˵������ָ����Ч��
	if(flags&0x10)	printf("-ACK");//ȷ�ϱ�־λ����������¿գ�˵��ȷ�������Ч��
	if(flags&0x08)	printf("-PSH");//�Ʊ�־λ����λʱ��ʾ���շ�Ӧ�������󽫱��Ľ���Ӧ�ò㣻
	if(flags&0x04) 
	{
									printf("-RST");//��λ��־�������ؽ�һ���Ѿ����ҵ����ӣ�
		if(r_t==0)
		{
//			tcp_client_close(tcp_client_pcb, tcp_client_pcb->callback_arg);
//			lwip_flag &= 0x7f;
		}
	}
	
	if(flags&0x02)	printf("-SYN");//ͬ����־���ñ�־�����������ֽ���TCP����ʱ��Ч
	if(flags&0x01)	printf("-FIN");//������־�����ñ�־λ�����ݰ����ڽ���һ��TCP�Ự��
	

//unsigned char seqN[4];//sequence number
//	unsigned char ackN[4];//Acknowledgment number
//	unsigned char hdlen;//
//	unsigned char flags;//
//	unsigned char winsize[2];//
	
	xlh=((ptcp_h->seqN[0]<<24)+(ptcp_h->seqN[1]<<16)+(ptcp_h->seqN[2]<<8)+(ptcp_h->seqN[3]<<0));
	qrh=((ptcp_h->ackN[0]<<24)+(ptcp_h->ackN[1]<<16)+(ptcp_h->ackN[2]<<8)+(ptcp_h->ackN[3]<<0));
//	ck=((ptcp_h->winsize[0]<<8)+(ptcp_h->winsize[1]<<0));
//	ptcp_h->
	printf("\t���к�%x",xlh);
	printf("\tȷ�Ϻ�%x",qrh);
//	printf("\t���ڴ�С%x",ck);
		
//	if((len>tcp_h_len)&&(r_t==1))
//	{
//		printf("\r\n��");
//		for(i=0;i<5;i++) printf("%c",datas[tcp_h_len+i]);
//		printf("%s",&datas[tcp_h_len]);
//	}
	if(len>tcp_h_len) 
		printf("\r\n�ײ��ӡ\r\n%s",&datas[tcp_h_len]);
	if(len>tcp_h_len) printf("\r\n%d",len-tcp_h_len);
}

void printf_UDP(unsigned char *datas,uint16_t len,uint8_t r_t)
{
	struct udp_h *pudp_h;
	uint16_t port0,port1;
//	char i;
	
	pudp_h=(struct udp_h *)datas;
	
	
	port0=(pudp_h->sport[0]<<8)+pudp_h->sport[1];
	port1=(pudp_h->dport[0]<<8)+pudp_h->dport[1];
	printf("\tudp:%d to %d",port0,port1);
	
//	if(len>udp_h_len) 
//	{
//		printf("\r\n��");
//		for(i=0;i<5;i++)
//		printf("%c",datas[udp_h_len+i]);
//	}
//	if(r_t==1)	printf("\r\n�ײ��ӡ\r\n%s",&datas[udp_h_len]);
//	if(len>udp_h_len) printf("\r\n%d",len-udp_h_len);
}

void printf_ip(unsigned char *datas,uint16_t len,uint8_t r_t)
{
	struct ip_h *dip_h;
	
	
	dip_h=(struct ip_h *)datas;
		
	if((check_ip(dip_h->Target_IPadd)==0)||(r_t==1))
	{
		
//		if(darp_h->Type==1)	printf_ICMP(unsigned char *datas,uint16_t len,r_t)		
//		if(darp_h->Type==0x11) printf_UDP(unsigned char *datas,uint16_t len,r_t)
		if(dip_h->Type==6) //tcp
		{
			printf_len(len+eth_h_len,r_t);
//			printf("\tipok");
//			printf("\ttcp");
			printf_TCP(&datas[ip_h_len],len-ip_h_len,r_t);
		}
		else if(dip_h->Type==0x11) //udp
		{
			printf_ip(&datas[ip_h_len],len-ip_h_len,r_t);
		}
		
//		else		{printf("\tType=%x",dip_h->Type);}
	}
}


void tcp_ip_printf(unsigned char *datas,uint16_t len,uint8_t r_t)
{
	struct eth_h *peth_h;
	
//	return;
//	uint16_t i;
		
//	datas[len]=0;
	peth_h=(struct eth_h *)datas;
	
//	if(datas[21]==2)
//	{
//		printf("\r\nMAC==%x",peth_h->dmac[0]);
//		printf("\r\nMAC==%x",peth_h->dmac[1]);
//		
//		printf("\r\n��MAC��");
//		for(i=32;i<38;i++) printf("%X,",datas[i]);
//		printf("\r\n��IP��");
//		for(i=38;i<42;i++) printf("%X,",datas[i]);
//	}
	
	if((peth_h->Type[0]==0x08)&&(peth_h->Type[1]==0x06)) //arp��
	{
		if((check_mac(peth_h->dmac)==0xff)||(check_mac(peth_h->dmac)==0)||(r_t==1))
		{		
//			printf_len(len,r_t);
//			printf("ping��");
			printf_arp(&datas[eth_h_len],len-eth_h_len,r_t);
		}
	}
	else	if((peth_h->Type[0]==0x08)&&(peth_h->Type[1]==0x00)) //ip��
	{
		if((check_mac(peth_h->dmac)==0)||(r_t==1))
		{
//			printf_len(len,r_t);
//			printf("\tip");
			printf_ip(&datas[eth_h_len],len-eth_h_len,r_t);
		}
		else
		{
//			 printf("\r\nmac=");
//			for(i=0;i<6;i++) printf("%x",peth_h->dmac[i]);
		}
	}
	else
	{
//		for(i=0;i<6;i++) printf("%x",datas[i]);
//		printf("\r\ntype=");
//		printf("%x,%x",peth_h->Type[0],peth_h->Type[1]);
	}

//	printf("\r\n");

}
//	
//struct ip_h //��̫��ͷ��+IP�ײ��ṹ
//{
//	struct eth_hdr ethhdr;    //��̫���ײ�
//	unsigned char vhl,      //4λ�汾��4λ�ײ�����(0x45)
//						 tos;     //��������(0)
//		 uint16 len,      //����IP���ݱ����ֽڳ���
//					 ipid,           //IP��ʶ
//					 ipoffset;     //3λ��ʶ13λƫ��
//	unsigned char ttl,             //����ʱ��(32��64)
//						proto;         //Э��(1��ʾICMP,2��ʾIGMP,6��ʾTCP,17��ʾUDP)
//	uint16 ipchksum;    //�ײ�У���
//	unsigned char srcipaddr[4],    //ԴIP
//							 destipaddr[4];   //Ŀ��IP
//};
//	
//struct arp_hdr 							//��̫��ͷ��+ARP�ײ��ṹ
//{
//	struct eth_hdr ethhdr;    //��̫���ײ�
//	uint16 hwtype;     				//Ӳ������(1��ʾ���������̫��MAC��ַ)
//	uint16 protocol;   				//Э������(0x0800��ʾ�������IP��ַ)
//	unsigned char hwlen;     	//Ӳ����ַ����(6)
//	unsigned char protolen;   //Э���ַ����(4)
//	uint16 opcode;     				//����(1��ʾARP����,2��ʾARPӦ��)
//	unsigned char smac[6];    //���Ͷ�MAC��ַ
//	unsigned char sipaddr[4]; //���Ͷ�IP��ַ
//	unsigned char dmac[6];    //Ŀ�Ķ�MAC��ַ
//	unsigned char dipaddr[4]; //Ŀ�Ķ�IP��ַ
//};



//    ���϶���������ײ��ṹ���Ǹ���TCP/IPЭ�����ع淶����ģ�������ARPЭ�������ϸ���⡣
//���ϰ벿���꡿
//struct arp_data
//{
//	struct arp_hdr hdr;
//	unsigned char rdata[1000];
//};
//struct ip_data
//{
//	struct ip_hdr hdr;
//	unsigned char rdata[1000];
//};


//uint16_t check_sum(uint8_t *packet,uint16_t packlen)
//{
//	uint64_t hsum = 0;
//	uint16_t i,len;
//		
//	hsum=0;
//	if(packlen&0x0001) len=packlen-1;
//	else len=packlen;
//	for(i=0;i<len;i+=2)
//	{
//		hsum +=((uint16_t)packet[i]<<8)+packet[i+1];
//	}
//	if(len !=packlen)
//	{
//		hsum +=((uint16_t)packet[i]<<8);
//	}	
//	
//	while(hsum >>16)
//	{
//		hsum = (hsum & 0x0ffff) + (hsum >> 16);//&0x0ffffffffffff
//	}
//	return (uint16_t) (~hsum);
//}

#if 0
void print_eth_header(unsigned char *datas,uint16_t len)
{
	uint16_t i;
	printf("\r\n��̫��ͷ��");
	
	printf("Ŀ��mac��");
	for(i=0;i<6;i++) printf("%X,",datas[i]);
	printf(",Դmac..��");
	for(i=6;i<12;i++) printf("%X,",datas[i]);
	printf(",���ͣ�");
	for(i=12;i<14;i++) printf("%X,",datas[i]);
}

void print_ARP(unsigned char *datas,uint16_t len)
{
	uint16_t i;
	printf("\r\nARP��");
	printf("\r\nӲ����"); for(i=14;i<16;i++) printf("%X,",datas[i]);
	if((datas[14]==0x00)&&(datas[15]==0x01)) printf("-��ʾ���������̫��MAC��ַ");
	printf("\r\nЭ�飺");					for(i=16;i<18;i++) printf("%X,",datas[i]);
	if((datas[16]==0x08)&&(datas[17]==0x00)) printf("-�������IP");
	printf("\r\nAd����"); 														for(i=18;i<20;i++) printf("%X,",datas[i]);
	printf("\r\n������"); 					for(i=20;i<22;i++) printf("%X,",datas[i]);
	if(datas[21]==1) printf("-arp����");
	if(datas[21]==2) printf("-\tarpӦ��");
	printf("\r\n��MAC��"); 															for(i=22;i<28;i++) printf("%X,",datas[i]);
	printf("\r\n��IP��");
	for(i=28;i<32;i++) printf("%X,",datas[i]);
	printf("\r\n��MAC��");
	for(i=32;i<38;i++) printf("%X,",datas[i]);
	printf("\r\n��IP��");
	for(i=38;i<42;i++) printf("%X,",datas[i]);
	printf("\r\n������");
	for(i=42;i<len;i++) printf("%X,",datas[i]);
}
void print_IP_header(unsigned char *datas,uint16_t len)
{
	uint16_t i;
	printf("\r\nIPͷ��");
	printf("\r\n�汾�ţ�"); printf("%X,",(datas[14]>>4));
	printf("\r\n�ײ����ȣ�"); printf("%X,",(datas[14]&0x0f));
	printf("\r\n�������ͣ�"); printf("%X,",(datas[15]));
	
	printf("\r\n�ֽ�����"); printf("%d,",((uint16_t)datas[16]<<8)+datas[17]);
	printf("\r\n��ʶ��"); printf("%x,",((uint16_t)datas[18]<<8)+datas[19]);
	//16bit�ı�ʶ�ֶ�Ψһ�ı�ʶ�������͵�ÿһ�����ݱ������������ɾ���Ψһ�ԡ�ͨ��ÿ����һ�ݱ��ĸ�ֵ��1����ֵ�����ݰ���Ƭʱ��
	//�Ḵ�Ƶ�ÿһ��Ƭ�С������������Ƭ����ʱ���۲��ֵ���Ѹ�ֵ��ͬ�ķ�Ƭ�ռ���һ�����飬�����������۷�Ƭ
	printf("\r\n��־��ƫ�ƣ�"); printf("%x,",((uint16_t)datas[20]<<8)+datas[21]);
	printf("\r\n����ʱ�䣺"); printf("%x,",datas[22]);
	printf("\r\nЭ�飺"); 	printf("%x,",datas[23]); 
	if(datas[23]==0x1)	printf("-ICMP"); 
	if(datas[23]==0x6)	printf("-TCP"); 
	if(datas[23]==0x11)	printf("-UDP"); 
	
	printf("\r\nIP�ײ�У��ͣ�"); printf("%x,",((uint16_t)datas[24]<<8)+datas[25]);
	
	printf("\r\nԴIP��"); for(i=26;i<30;i++) printf("%X,",datas[i]);
	printf("\r\nĿ��IP��"); for(i=30;i<34;i++) printf("%X,",datas[i]);
}
void r_t_data_printf(unsigned char *datas,uint16_t len,uint8_t r_t)
{
	uint16_t i;//
	
	return;
	
	if(r_t==0) printf("\r\nrlen=");
	else printf("\r\ntlen=");
	printf("%d.......xxxxxx",len);

	if(len<42) printf("\r\n  ����С��42��");
	print_eth_header(datas,14);
	


				if((datas[12]==0x08)&&(datas[13]==0x06)) // ARP ����/Ӧ�� 28
				{
					print_ARP(datas,len);
				}
				else if((datas[12]==0x08)&&(datas[13]==0x00)) // IPV4 ���� 46-1500
				{
									
					if(datas[23]==1)	//ICMP PING
					{
						print_IP_header(datas,len);
						printf("\r\nICMPͷ��");
						printf("\r\nTYPE&CODE��");for(i=34;i<36;i++) printf("%X,",datas[i]); printf("\t 0800-��������(PING)  0000-���Իش�"); 
						if((datas[34]==0x08)&&(datas[35]==0x00)) printf("-��������(PING)");
						if((datas[34]==0x00)&&(datas[35]==0x00)) printf("-���Իش�");
						
						printf("\r\nICMPУ�飺");for(i=36;i<38;i++) printf("%X,",datas[i]);
						printf("\r\n��ʶ����");for(i=38;i<40;i++) printf("%X,",datas[i]);
						printf("\r\n���кţ�");for(i=40;i<42;i++) printf("%X,",datas[i]);
						printf("\r\n�������ݣ�"); for(i=42;i<len;i++) printf("%X,",datas[i]);
					}
					else if(datas[23]==6) //TCP����
					{
						print_IP_header(datas,len);
						printf("\r\nTCPͷ��");
						printf("\r\nԴ�˿ڣ�");for(i=34;i<36;i++) printf("%X,",datas[i]);
						printf("\r\nĿ�Ķ˿ڣ�");for(i=36;i<38;i++) printf("%X,",datas[i]);
						printf("\r\n��ţ�");for(i=38;i<42;i++) printf("%X,",datas[i]);
						printf("\r\nȷ�Ϻţ�");for(i=42;i<46;i++) printf("%X,",datas[i]);
						printf("\r\n�ײ����ȣ�");printf("%X,",datas[46]);
						printf("\r\n��־��");printf("%X,",datas[47]);
						if(datas[47]&0x20) printf("-URG");//������־λ��˵������ָ����Ч��
						if(datas[47]&0x10) printf("-ACK");//ȷ�ϱ�־λ����������¿գ�˵��ȷ�������Ч��
						if(datas[47]&0x08) printf("-PSH");//�Ʊ�־λ����λʱ��ʾ���շ�Ӧ�������󽫱��Ľ���Ӧ�ò㣻
						if(datas[47]&0x04) printf("-RST");//��λ��־�������ؽ�һ���Ѿ����ҵ����ӣ�
						if(datas[47]&0x02) printf("-SYN");//ͬ����־���ñ�־�����������ֽ���TCP����ʱ��Ч
						if(datas[47]&0x01) printf("-FIN");//������־�����ñ�־λ�����ݰ����ڽ���һ��TCP�Ự��
//						if()
						printf("\r\n���ڣ�");for(i=48;i<50;i++) printf("%X,",datas[i]);
						printf("\r\nУ��ͣ�");for(i=50;i<52;i++) printf("%X,",datas[i]);
						printf("\r\n����ָ�룺");for(i=52;i<54;i++) printf("%X,",datas[i]);
						
//						printf("\r\nѡ�");for(i=54;i<len-20;i++) printf("%X,",datas[i]);
						
						printf("\r\n���ݣ�");for(i=54;i<len;i++) printf("%X,",datas[i]);
//						for(i=54;i<len;i++) if(datas[i]!=0) break;
//						i=400;
//						printf("��%d�ֽڿ�ʼ\r\n",i);	printf("%s",&datas[i]);
//						for(i=54;i<len;i++) if(datas[i]!=0) break;
//						printf("\r\n%s",&datas[i]);
					}
					else if(datas[23]==0x11) //UDP����
					{
//						print_IP_header(datas,len);
						printf("\r\nUDPͷ��");
						printf("\r\nԴ�˿ڣ�");for(i=34;i<36;i++) printf("%X,",datas[i]);
						printf("\r\nĿ�Ķ˿ڣ�");for(i=36;i<38;i++) printf("%X,",datas[i]);
						printf("\r\n���ȣ�");for(i=38;i<40;i++) printf("%X,",datas[i]);
						printf("\r\nУ��ͣ�");for(i=40;i<42;i++) printf("%X,",datas[i]);
	
												
						printf("\r\n���ݣ�");//for(i=54;i<len;i++) printf("%X,",datas[i]);
						printf("��%d�ֽڿ�ʼ\r\n",i);	printf("%s",&datas[i]);
					}
					else
					{	
//						print_IP_header(datas,len);
						printf("\r\n�������ݣ�"); //for(i=34;i<len;i++) printf("%X,",datas[i]);
						printf("%s,",&datas[34]);
					}
					
				}
				else if((datas[12]==0x86)&&(datas[13]==0xDD)) // IPV6
				{
					printf("\r\nIPV6,�������ݣ�");
//					for(i=14;i<len;i++) 
//					printf("%s,",&datas[14]);
				}
				else if((datas[12]==0x08)&&(datas[13]==0x35)) // RARP ����/Ӧ�� 28
				{
					printf("\r\nRARP ���ݣ�");
//					for(i=14;i<len;i++) printf("%X,",datas[i]);
					printf("%s,",&datas[14]);
				}
				else
				{
					printf("\r\nδ֪���ݣ�");
//					for(i=14;i<len;i++) printf("%X,",datas[i]);
					printf("%s,",&datas[14]);
				}	
				printf("\r\n");
}
#endif


