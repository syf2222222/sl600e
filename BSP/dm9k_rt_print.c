
#include "main.h"
#include <stdio.h>
#include <string.h>

#include "DM9000.h"
#include "lwip.h"

#define eth_h_len	14

struct eth_h
{
	unsigned char dmac[6];    //Destination MAC 目的端MAC地址
	unsigned char smac[6];    //Source MAC 发送端MAC地址
	unsigned char Type[2];		//协议类型
};

#define arp_h_len	28
struct arp_h //eth_h.Type=0x0806
{
	unsigned char HwType[2];//硬件类型(1表示传输的是以太网MAC地址)
	unsigned char PtType[2]; //protocol type //协议类型(0x0800表示传输的是IP地址)
	unsigned char HwSize;//硬件地址长度(6)
	unsigned char PtSize;//协议地址长度(4)
	unsigned char Opcode[2];//操作(1表示ARP请求,2表示ARP应答)
	unsigned char Sender_MACadd[6];//发送端MAC地址
	unsigned char Sender_IPadd[4];//发送端IP地址
	unsigned char Target_MACadd[6];//目的端MAC地址
	unsigned char Target_IPadd[4];//目的端IP地址
};
#define ip_h_len	20
struct ip_h //以太网头部+IP首部结构
{
	unsigned char vhl;      //4位版本号4位首部长度(0x45)
	unsigned char tos;     //服务类型(0)
	unsigned char len[2];      //整个IP数据报总字节长度
	unsigned char ipid[2];          //IP标识
	unsigned char ipoffset[2];     //3位标识13位偏移
	unsigned char ttl;             //生存时间(32或64)
	unsigned char Type;         //协议(1表示ICMP,2表示IGMP,6表示TCP,17表示UDP)
	unsigned char ipchksum[2];    //首部校验和
	unsigned char Sender_IPadd[4];    //源IP
	unsigned char Target_IPadd[4];   //目的IP
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
	if(flags&0x20)	printf("-URG");//紧急标志位，说明紧急指针有效；
	if(flags&0x10)	printf("-ACK");//确认标志位，多数情况下空，说明确认序号有效；
	if(flags&0x08)	printf("-PSH");//推标志位，置位时表示接收方应立即请求将报文交给应用层；
	if(flags&0x04) 
	{
									printf("-RST");//复位标志，用于重建一个已经混乱的连接；
		if(r_t==0)
		{
//			tcp_client_close(tcp_client_pcb, tcp_client_pcb->callback_arg);
//			lwip_flag &= 0x7f;
		}
	}
	
	if(flags&0x02)	printf("-SYN");//同步标志，该标志仅在三次握手建立TCP连接时有效
	if(flags&0x01)	printf("-FIN");//结束标志，带该标志位的数据包用于结束一个TCP会话。
	

//unsigned char seqN[4];//sequence number
//	unsigned char ackN[4];//Acknowledgment number
//	unsigned char hdlen;//
//	unsigned char flags;//
//	unsigned char winsize[2];//
	
	xlh=((ptcp_h->seqN[0]<<24)+(ptcp_h->seqN[1]<<16)+(ptcp_h->seqN[2]<<8)+(ptcp_h->seqN[3]<<0));
	qrh=((ptcp_h->ackN[0]<<24)+(ptcp_h->ackN[1]<<16)+(ptcp_h->ackN[2]<<8)+(ptcp_h->ackN[3]<<0));
//	ck=((ptcp_h->winsize[0]<<8)+(ptcp_h->winsize[1]<<0));
//	ptcp_h->
	printf("\t序列号%x",xlh);
	printf("\t确认号%x",qrh);
//	printf("\t窗口大小%x",ck);
		
//	if((len>tcp_h_len)&&(r_t==1))
//	{
//		printf("\r\n低");
//		for(i=0;i<5;i++) printf("%c",datas[tcp_h_len+i]);
//		printf("%s",&datas[tcp_h_len]);
//	}
	if(len>tcp_h_len) 
		printf("\r\n底层打印\r\n%s",&datas[tcp_h_len]);
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
//		printf("\r\n低");
//		for(i=0;i<5;i++)
//		printf("%c",datas[udp_h_len+i]);
//	}
//	if(r_t==1)	printf("\r\n底层打印\r\n%s",&datas[udp_h_len]);
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
//		printf("\r\n收MAC：");
//		for(i=32;i<38;i++) printf("%X,",datas[i]);
//		printf("\r\n收IP：");
//		for(i=38;i<42;i++) printf("%X,",datas[i]);
//	}
	
	if((peth_h->Type[0]==0x08)&&(peth_h->Type[1]==0x06)) //arp包
	{
		if((check_mac(peth_h->dmac)==0xff)||(check_mac(peth_h->dmac)==0)||(r_t==1))
		{		
//			printf_len(len,r_t);
//			printf("ping包");
			printf_arp(&datas[eth_h_len],len-eth_h_len,r_t);
		}
	}
	else	if((peth_h->Type[0]==0x08)&&(peth_h->Type[1]==0x00)) //ip包
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
//struct ip_h //以太网头部+IP首部结构
//{
//	struct eth_hdr ethhdr;    //以太网首部
//	unsigned char vhl,      //4位版本号4位首部长度(0x45)
//						 tos;     //服务类型(0)
//		 uint16 len,      //整个IP数据报总字节长度
//					 ipid,           //IP标识
//					 ipoffset;     //3位标识13位偏移
//	unsigned char ttl,             //生存时间(32或64)
//						proto;         //协议(1表示ICMP,2表示IGMP,6表示TCP,17表示UDP)
//	uint16 ipchksum;    //首部校验和
//	unsigned char srcipaddr[4],    //源IP
//							 destipaddr[4];   //目的IP
//};
//	
//struct arp_hdr 							//以太网头部+ARP首部结构
//{
//	struct eth_hdr ethhdr;    //以太网首部
//	uint16 hwtype;     				//硬件类型(1表示传输的是以太网MAC地址)
//	uint16 protocol;   				//协议类型(0x0800表示传输的是IP地址)
//	unsigned char hwlen;     	//硬件地址长度(6)
//	unsigned char protolen;   //协议地址长度(4)
//	uint16 opcode;     				//操作(1表示ARP请求,2表示ARP应答)
//	unsigned char smac[6];    //发送端MAC地址
//	unsigned char sipaddr[4]; //发送端IP地址
//	unsigned char dmac[6];    //目的端MAC地址
//	unsigned char dipaddr[4]; //目的端IP地址
//};



//    以上定义的三种首部结构，是根据TCP/IP协议的相关规范定义的，后面会对ARP协议进行详细讲解。
//【上半部分完】
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
	printf("\r\n以太网头：");
	
	printf("目的mac：");
	for(i=0;i<6;i++) printf("%X,",datas[i]);
	printf(",源mac..：");
	for(i=6;i<12;i++) printf("%X,",datas[i]);
	printf(",类型：");
	for(i=12;i<14;i++) printf("%X,",datas[i]);
}

void print_ARP(unsigned char *datas,uint16_t len)
{
	uint16_t i;
	printf("\r\nARP：");
	printf("\r\n硬件："); for(i=14;i<16;i++) printf("%X,",datas[i]);
	if((datas[14]==0x00)&&(datas[15]==0x01)) printf("-表示传输的是以太网MAC地址");
	printf("\r\n协议：");					for(i=16;i<18;i++) printf("%X,",datas[i]);
	if((datas[16]==0x08)&&(datas[17]==0x00)) printf("-传输的是IP");
	printf("\r\nAd长："); 														for(i=18;i<20;i++) printf("%X,",datas[i]);
	printf("\r\n操作："); 					for(i=20;i<22;i++) printf("%X,",datas[i]);
	if(datas[21]==1) printf("-arp请求");
	if(datas[21]==2) printf("-\tarp应答");
	printf("\r\n发MAC："); 															for(i=22;i<28;i++) printf("%X,",datas[i]);
	printf("\r\n发IP：");
	for(i=28;i<32;i++) printf("%X,",datas[i]);
	printf("\r\n收MAC：");
	for(i=32;i<38;i++) printf("%X,",datas[i]);
	printf("\r\n收IP：");
	for(i=38;i<42;i++) printf("%X,",datas[i]);
	printf("\r\n其他：");
	for(i=42;i<len;i++) printf("%X,",datas[i]);
}
void print_IP_header(unsigned char *datas,uint16_t len)
{
	uint16_t i;
	printf("\r\nIP头：");
	printf("\r\n版本号："); printf("%X,",(datas[14]>>4));
	printf("\r\n首部长度："); printf("%X,",(datas[14]&0x0f));
	printf("\r\n服务类型："); printf("%X,",(datas[15]));
	
	printf("\r\n字节数："); printf("%d,",((uint16_t)datas[16]<<8)+datas[17]);
	printf("\r\n标识："); printf("%x,",((uint16_t)datas[18]<<8)+datas[19]);
	//16bit的标识字段唯一的标识主机发送的每一份数据报，由主机生成具有唯一性。通常每发送一份报文该值加1。该值在数据包分片时，
	//会复制到每一个片中。所以在重组分片包的时候会观察该值，把该值相同的分片收集到一起重组，后面会继续讨论分片
	printf("\r\n标志和偏移："); printf("%x,",((uint16_t)datas[20]<<8)+datas[21]);
	printf("\r\n生存时间："); printf("%x,",datas[22]);
	printf("\r\n协议："); 	printf("%x,",datas[23]); 
	if(datas[23]==0x1)	printf("-ICMP"); 
	if(datas[23]==0x6)	printf("-TCP"); 
	if(datas[23]==0x11)	printf("-UDP"); 
	
	printf("\r\nIP首部校验和："); printf("%x,",((uint16_t)datas[24]<<8)+datas[25]);
	
	printf("\r\n源IP："); for(i=26;i<30;i++) printf("%X,",datas[i]);
	printf("\r\n目的IP："); for(i=30;i<34;i++) printf("%X,",datas[i]);
}
void r_t_data_printf(unsigned char *datas,uint16_t len,uint8_t r_t)
{
	uint16_t i;//
	
	return;
	
	if(r_t==0) printf("\r\nrlen=");
	else printf("\r\ntlen=");
	printf("%d.......xxxxxx",len);

	if(len<42) printf("\r\n  长度小于42：");
	print_eth_header(datas,14);
	


				if((datas[12]==0x08)&&(datas[13]==0x06)) // ARP 请求/应答 28
				{
					print_ARP(datas,len);
				}
				else if((datas[12]==0x08)&&(datas[13]==0x00)) // IPV4 数据 46-1500
				{
									
					if(datas[23]==1)	//ICMP PING
					{
						print_IP_header(datas,len);
						printf("\r\nICMP头：");
						printf("\r\nTYPE&CODE：");for(i=34;i<36;i++) printf("%X,",datas[i]); printf("\t 0800-回显请求(PING)  0000-回显回答"); 
						if((datas[34]==0x08)&&(datas[35]==0x00)) printf("-回显请求(PING)");
						if((datas[34]==0x00)&&(datas[35]==0x00)) printf("-回显回答");
						
						printf("\r\nICMP校验：");for(i=36;i<38;i++) printf("%X,",datas[i]);
						printf("\r\n标识符：");for(i=38;i<40;i++) printf("%X,",datas[i]);
						printf("\r\n序列号：");for(i=40;i<42;i++) printf("%X,",datas[i]);
						printf("\r\n回显数据："); for(i=42;i<len;i++) printf("%X,",datas[i]);
					}
					else if(datas[23]==6) //TCP报文
					{
						print_IP_header(datas,len);
						printf("\r\nTCP头：");
						printf("\r\n源端口：");for(i=34;i<36;i++) printf("%X,",datas[i]);
						printf("\r\n目的端口：");for(i=36;i<38;i++) printf("%X,",datas[i]);
						printf("\r\n序号：");for(i=38;i<42;i++) printf("%X,",datas[i]);
						printf("\r\n确认号：");for(i=42;i<46;i++) printf("%X,",datas[i]);
						printf("\r\n首部长度：");printf("%X,",datas[46]);
						printf("\r\n标志：");printf("%X,",datas[47]);
						if(datas[47]&0x20) printf("-URG");//紧急标志位，说明紧急指针有效；
						if(datas[47]&0x10) printf("-ACK");//确认标志位，多数情况下空，说明确认序号有效；
						if(datas[47]&0x08) printf("-PSH");//推标志位，置位时表示接收方应立即请求将报文交给应用层；
						if(datas[47]&0x04) printf("-RST");//复位标志，用于重建一个已经混乱的连接；
						if(datas[47]&0x02) printf("-SYN");//同步标志，该标志仅在三次握手建立TCP连接时有效
						if(datas[47]&0x01) printf("-FIN");//结束标志，带该标志位的数据包用于结束一个TCP会话。
//						if()
						printf("\r\n窗口：");for(i=48;i<50;i++) printf("%X,",datas[i]);
						printf("\r\n校验和：");for(i=50;i<52;i++) printf("%X,",datas[i]);
						printf("\r\n紧急指针：");for(i=52;i<54;i++) printf("%X,",datas[i]);
						
//						printf("\r\n选项：");for(i=54;i<len-20;i++) printf("%X,",datas[i]);
						
						printf("\r\n数据：");for(i=54;i<len;i++) printf("%X,",datas[i]);
//						for(i=54;i<len;i++) if(datas[i]!=0) break;
//						i=400;
//						printf("第%d字节开始\r\n",i);	printf("%s",&datas[i]);
//						for(i=54;i<len;i++) if(datas[i]!=0) break;
//						printf("\r\n%s",&datas[i]);
					}
					else if(datas[23]==0x11) //UDP报文
					{
//						print_IP_header(datas,len);
						printf("\r\nUDP头：");
						printf("\r\n源端口：");for(i=34;i<36;i++) printf("%X,",datas[i]);
						printf("\r\n目的端口：");for(i=36;i<38;i++) printf("%X,",datas[i]);
						printf("\r\n长度：");for(i=38;i<40;i++) printf("%X,",datas[i]);
						printf("\r\n校验和：");for(i=40;i<42;i++) printf("%X,",datas[i]);
	
												
						printf("\r\n数据：");//for(i=54;i<len;i++) printf("%X,",datas[i]);
						printf("第%d字节开始\r\n",i);	printf("%s",&datas[i]);
					}
					else
					{	
//						print_IP_header(datas,len);
						printf("\r\n其他数据："); //for(i=34;i<len;i++) printf("%X,",datas[i]);
						printf("%s,",&datas[34]);
					}
					
				}
				else if((datas[12]==0x86)&&(datas[13]==0xDD)) // IPV6
				{
					printf("\r\nIPV6,忽略数据：");
//					for(i=14;i<len;i++) 
//					printf("%s,",&datas[14]);
				}
				else if((datas[12]==0x08)&&(datas[13]==0x35)) // RARP 请求/应答 28
				{
					printf("\r\nRARP 数据：");
//					for(i=14;i<len;i++) printf("%X,",datas[i]);
					printf("%s,",&datas[14]);
				}
				else
				{
					printf("\r\n未知数据：");
//					for(i=14;i<len;i++) printf("%X,",datas[i]);
					printf("%s,",&datas[14]);
				}	
				printf("\r\n");
}
#endif


