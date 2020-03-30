
#include "main.h"
#include <stdio.h>
#include <string.h>
#include "DM9000.h"

#include "lwip.h"
#include "lwip/tcpip.h"
#include "netif/ethernet.h"



#define use_dm9000

#define printf_test	0

#define printf_int_test	0

//struct dm9000_config dm9000cfg;				//DM9000配置结构体


//uint16_t check_sum(uint8_t *packet,uint16_t packlen);

#define DM9000_PKT_MAX	1518  // IP 1500 + 以太网首部 14 +CRC 4 //1518

#define DM_ADD (*((volatile uint16_t *) 0x6C000000))//FMC_BANK1_4  //0x6C000000
#define DM_CMD (*((volatile uint16_t *) 0x6C000002))

extern unsigned char	mac_addr[6];//={0x11,0x22,0x16,0x44,0x55,0x10};
	

void udelay(volatile uint32_t cnt)
{
	uint32_t i,j;
	OS_ERR      err;
	
	if(cnt>=1000)
	{
		OSTimeDly ( cnt/1000, OS_OPT_TIME_DLY, & err ); 
	}
	else
	{
		for(i=0;i<6;i++)
		{
			for(j=0;j<cnt;j++);
		}
	}
}
//向DM9000寄存器写数据
void dm9000_reg_write(unsigned char reg, unsigned char data);
uint16_t dm9000_reg_read(unsigned char reg);

void dm9000_PHY_write(unsigned char reg, uint16_t data);

uint16_t dm9000_PHY_read(unsigned char reg);
	
//DM9000初始化

void Test_DM9000_ID(void)
{
	uint32_t id;
	id  = dm9000_reg_read(0x28) << 0;	
	id |= dm9000_reg_read(0x29) << 8;
	id |= dm9000_reg_read(0x2A) << 16;
	id |= dm9000_reg_read(0x2B) << 24;  
  
#if printf_int_test
	printf("读到 DM9000ID= 0x%X\r\n",id);//A4600,,90000A46
#endif
}

void Read_DM9000_MAC(void)
{
#if printf_int_test	
	uint16_t i;
	printf("读到 MAC= ");
	
	for(i=0; i<6; i++) printf("%x ",dm9000_reg_read(PAR + i));
#endif
}
char phy_link=0;
char check_link(void)
{
	OS_ERR      err;
	OSMutexPend (&dm9000_sem_lock, 0, 	OS_OPT_PEND_BLOCKING, 0, &err); 
	if(dm9000_PHY_read(BMSR) & 0x20)
	{
		phy_link=1;
		OSMutexPost (&dm9000_sem_lock, OS_OPT_POST_NONE, &err); 
		return 1;
	}
	else
	{
		phy_link=0;
		OSMutexPost (&dm9000_sem_lock, OS_OPT_POST_NONE, &err); 
		return 0;
	}
	
}

#if 1
void dm9k_init(unsigned char *MAC_ADD)
{
    uint16_t i;
	OS_ERR      err;
	
#ifndef use_dm9000	
	return;//不初始化，球机跟电脑连接
#endif	
#if printf_int_test	
	printf("//////////////////////////////初始化开始//////////////////////////////\r\n");
#endif	
//	printf("\r\ninit eth");
	
	if(power_flag_ETH==0)
	{
		dm9k_init_flag=0;
		return;
	}
//	PWR_W_ETH(1);
//	udelay(50000);
	OSTimeDly ( 50, OS_OPT_TIME_DLY, & err ); 
	RST_L();//RST_H();
//	udelay(300000);
	OSTimeDly ( 300, OS_OPT_TIME_DLY, & err ); 
	RST_H();//RST_H();
//	udelay(300000);
	OSTimeDly ( 300, OS_OPT_TIME_DLY, & err );
	Test_DM9000_ID();

/*以上部分是利用一个IO口控制DM9000的RST引脚，使其复位。这一步可以省略，可以用下面的软件复位代替*/
	

    dm9000_reg_write(GPCR, 0x01);//设置 GPCR(1EH) bit[0]=1，使DM9000的GPIO3为输出。
    dm9000_reg_write(GPR, 0x00);//GPR bit[0]=0 使DM9000的GPIO3输出为低以激活内部PHY。
//    udelay(5000);//延时2ms以上等待PHY上电。
		OSTimeDly ( 5, OS_OPT_TIME_DLY, & err );
    dm9000_reg_write(NCR, 0x03);//软件复位
//    udelay(3000);//延时20us以上等待软件复位完成
		OSTimeDly ( 3, OS_OPT_TIME_DLY, & err );
    dm9000_reg_write(NCR, 0x00);//复位完成，设置正常工作模式。
    dm9000_reg_write(NCR, 0x03);//第二次软件复位，为了确保软件复位完全成功。此步骤是必要的。
		
		
		
		dm9000_reg_write(NCR, 0x00);
//    udelay(3000);
		OSTimeDly ( 3, OS_OPT_TIME_DLY, & err );
//   dm9000_reg_write(NCR, 0x54);//@@ 使用内部PHY  dm9000_reg_write(NCR, 0x80);
		
		dm9000_reg_write(IMR, 0x80);
		
/*以上完成了DM9000的复位操作*/
    dm9000_reg_write(NSR, 0x2c);//清除各种状态标志位
    dm9000_reg_write(ISR, 0x0f);//清除所有中断标志位 //0xbf
/*以上清除标志位*/
    dm9000_reg_write(RCR, 0x39);//接收控制  0x39	//0x3b  
		
    dm9000_reg_write(TCR, 0x00);//发送控制
		dm9000_reg_write(TCR2, 0x80);	 /* Switch LED to mode 1 */
    dm9000_reg_write(BPTR, 0x3f);//设置RX的最低阀值，小于将产生拥塞
    dm9000_reg_write(FCTR, 0x38);//接收FIFO门限3K，8K
    dm9000_reg_write(RTFCR, 0xff);//启动一些控制功能
    dm9000_reg_write(SMCR, 0x00);//未启动特殊模式
/*以上是功能控制，具体功能参考上一篇文章中的说明，或参考数据手册的介绍*/

//	temp=*(vu32*)(0x1FFFF7E8);				//获取STM32的唯一ID的前24位作为MAC地址后三字节
//	dm9000cfg.mac_addr[0]=2;
//	dm9000cfg.mac_addr[1]=0;
//	dm9000cfg.mac_addr[2]=0;
//	dm9000cfg.mac_addr[3]=(temp>>16)&0XFF;	//低三字节用STM32的唯一ID
//	dm9000cfg.mac_addr[4]=(temp>>8)&0XFFF;
//	dm9000cfg.mac_addr[5]=temp&0XFF;
	
    for(i=0; i<6; i++)
        dm9000_reg_write(PAR + i, MAC_ADD[i]);//mac_addr[]自己定义一下吧，6个字节的MAC地址
				
//		for(i=0; i<8; i++)
//        dm9000_reg_write(MAR + i, 0xff);//mac_addr[]自己定义一下吧，6个字节的MAC地址		
				
/*以上存储MAC地址（网卡物理地址）到芯片中去，这里没有用EEPROM，所以需要自己写进去*/
/*关于MAC地址的说明，要参考网络相关书籍或资料*/
    dm9000_reg_write(NSR, 0x2c);
    dm9000_reg_write(ISR, 0x3f);
/*为了保险，上面有清除了一次标志位*/
    dm9000_reg_write(IMR, 0x81);
/*中断使能（或者说中断屏蔽），即开启我们想要的中断，关闭不想要的，这里只开启的一个接收中断*/
/*以上所有寄存器的具体含义参考上一篇文章，或参考数据手册*/

	dm9000_PHY_write(BMCR,0x1000); //自动协商使能 0x1000   //0x2100

//	dm9000_reg_write(GPR, 0); //@@ 打开物理层
//	dm9000_reg_write(DM9000_RCR, RCR_DIS_LONG | RCR_DIS_CRC | RCR_RXEN);	/* RX enable */
//	dm9000_reg_write(DM9000_IMR, IMR_PAR);
	
		Read_DM9000_MAC();
#if printf_int_test	
		printf("\r\nBMCR= %x\r\n",dm9000_PHY_read(BMCR));
		printf("BMSR= %x\r\n",dm9000_PHY_read(BMSR));
		printf("PHYID1= %x\r\n",dm9000_PHY_read(PHYID1));
		printf("PHYID2= %x\r\n",dm9000_PHY_read(PHYID2));
#endif		
		i=0;
		while(!(dm9000_PHY_read(BMSR) & 0x20))
		{
			udelay(100);
			if (i++ >= 250)
			{
//				printf("//////////////////////////////连接异常//////////////////////////////\r\n");
				printf("\r\n视频连接异常");
				dm9k_init_flag=0;
				return;
			}
		}
		dm9k_init_flag=1;
#if printf_int_test					
		i = dm9000_PHY_read(17) >> 12;
    //DM9000_TRACE("operating at ");

    switch (i)
    {
		case 1:
		     //DM9000_TRACE("10M half duplex ");
			 printf("10M half\r\n");
		     break;
		case 2:
		     //DM9000_TRACE("10M full duplex ");
			 printf("10M full\r\n");
		     break;
		case 4:
		     //DM9000_TRACE("100M half duplex ");
			 printf("100M half\r\n");
		     break;
		case 8:
		     //DM9000_TRACE("100M full duplex ");
			 printf("100M full\r\n");
		     break;
		default:
		     //DM9000_TRACE("unknown: %d ", lnk);
			printf("Unknown: lnk=%d\r\n", i);
		     break;
    }

		printf("//////////////////////////////初始化结束//////////////////////////////\r\n");
#endif
}
#endif

void dm9k_reset(void)
{
	printf("\r\n复位DM9000");
	dm9000_reg_write(IMR, 0x80);//屏蔽网卡中断
	dm9k_init(mac_addr);//mac_addr //重新初始化
	dm9000_reg_write(IMR, 0x81);//打开网卡中断
}
	
//#define dm9000_reg_write	dm9000_reg_write 
//#define dm9000_io_read	dm9000_reg_read
//#define NCR_RST 0x01
//#define GPCR_GEP_CNTL 0x01

//这样就对DM9000初始化完成了，怎么样，挺简单的吧。
//3、发送、接收数据包
//同样，以程序为例，通过注释说明。
//发送数据包
//参数：datas为要发送的数据缓冲区（以字节为单位），length为要发送的数据长度（两个字节）。

//通过DM9000发送数据包
//p:pbuf结构体指针

#define DM9000_WriteReg	dm9000_reg_write
#define DM9000_ReadReg	dm9000_reg_read

void tcp_ip_printf(unsigned char *datas,uint16_t len,uint8_t r_t);
void r_t_data_printf(unsigned char *datas,uint16_t len,uint8_t r_t);

#if 1
void DM9000_SendPacket(struct pbuf *p)
{
	struct pbuf *q;
	uint16_t pbuf_index = 0;
	uint8_t word[2], word_index = 0;
	
	OS_ERR      err;
	
	if(dm9k_init_flag==0) return;
	
#ifndef use_dm9000	
	return ;//不初始化，球机跟电脑连接
#endif
	
//	xSemaphoreTake(dm9000lock,portMAX_DELAY); 	//请求互斥信号量,锁定DM9000 
//	OSSemPend(dm9000_sem_lock,0,&err);	
	
	//						互斥信号量指针	//超时时间（节拍） //选项	 //时间戳-不获取 //返回错误类型
	OSMutexPend (&dm9000_sem_lock, 0, 	OS_OPT_PEND_BLOCKING, 0, &err); 
	
	
	DM9000_WriteReg(IMR,IMR_PAR);		//关闭网卡中断 
	
	q=p;
	//向DM9000的TX SRAM中写入数据，一次写入两个字节数据
	//当要发送的数据长度为奇数的时候，我们需要将最后一个字节单独写入DM9000的TX SRAM中
//#if printf_test	
//	int i;
//	i=0;
//#endif	

	pbuf_index=0;
		DM_ADD = MWCMD;
//	DM9000->REG=DM9000_MWCMD;					//发送此命令后就可以将要发送的数据搬到DM9000 TX SRAM中	
		while (q)
		{
				if (pbuf_index < q->len)
				{
//					udelay(30);
						word[word_index++] = ((u8_t*)q->payload)[pbuf_index++];
//					cbuf[i]=word[word_index-1];
//					i++;
//					if(q->len)	word[word_index++] =http_client_recvbuf[pbuf_index++];
//					printf("%c",(char)word[word_index-1]);
						if (word_index == 2)
						{
//								DM9000_outw(DM9000_DATA_BASE, (word[1] << 8) | word[0]);
								DM_CMD=((word[1] << 8) | word[0]);
								word_index = 0;
						}
				}
				else
				{
						q = q->next;
						pbuf_index = 0;
				}
		}
//		pbuf_index=i; //总长度
	//还有一个字节未写入TX SRAM
	if(word_index==1)DM_CMD=word[0];

// 	while(q)
//	{
//		if (pbuf_index < q->len)
//		{
//			if(i<1300) cbuf[i++] = ((u8_t*)q->payload)[pbuf_index++];
//		}
//		else
//		{
//			q=q->next;
//			pbuf_index = 0;
//		}
//	}
////	printf("\r\ntot_len=%d",p->tot_len);
//	pbuf_index=i; //总长度
////	printf("\r\npbuf_index=%d",pbuf_index);
//	
////	printf("\r\npbuf_index=%d",pbuf_index);
////	pbuf_index=p->tot_len;
////	printf("\r\npbuf_index=%d",pbuf_index);
//	
//	DM_ADD = MWCMD;
////	DM9000->REG=DM9000_MWCMD;					//发送此命令后就可以将要发送的数据搬到DM9000 TX SRAM中	
//	if(pbuf_index&0x01)	
//	{
//		for(i=0;i<pbuf_index-1;i+=2)
//		{
//			DM_CMD=((uint16_t)cbuf[i+1]<<8)|cbuf[i];
//		}
//		DM_CMD=cbuf[i];
//	}		
//	else
//	for(i=0;i<pbuf_index;i+=2)
//	{
//		DM_CMD=((uint16_t)cbuf[i+1]<<8)|cbuf[i];
//	}
	
	//向DM9000写入发送长度
	DM9000_WriteReg(TXPLL,p->tot_len&0XFF);
	DM9000_WriteReg(TXPLH,(p->tot_len>>8)&0XFF);		//设置要发送数据的数据长度
	
//	printf("\r\ntot_len=%d",p->tot_len);
	
	DM9000_WriteReg(TCR,0X01);						//启动发送 
	while((DM9000_ReadReg(ISR)&0X02)==0);			//等待发送完成 
	DM9000_WriteReg(ISR,0X02);						//清除发送完成中断 
	dm9000_reg_write(IMR, 0x81);//DM9000网卡的接收中断使能
	
//						互斥信号量指针	 	//选项	 			//返回错误类型	

			OSMutexPost (&dm9000_sem_lock, OS_OPT_POST_NONE, &err); 
	
	
//	//还有一个字节未写入TX SRAM
//	if(word_index==1)DM_CMD=word[0];
//	//向DM9000写入发送长度
//	DM9000_WriteReg(TXPLL,p->tot_len&0XFF);
//	DM9000_WriteReg(TXPLH,(p->tot_len>>8)&0XFF);		//设置要发送数据的数据长度
//	DM9000_WriteReg(TCR,0X01);						//启动发送 
//	while((DM9000_ReadReg(ISR)&0X02)==0);			//等待发送完成 
//	DM9000_WriteReg(ISR,0X02);						//清除发送完成中断 
// 	DM9000_WriteReg(IMR,dm9000cfg.imr_all);			//DM9000网卡接收中断使能
	
#if printf_test	
		tcp_ip_printf(((u8_t*)p->payload),p->tot_len,1);
//		tcp_ip_printf(cbuf,pbuf_index,1);

//		if((pbuf_index>=165)&&(pbuf_index<=169)) 
//		{
//			for(i=0;i<10;i++) printf("%x",cbuf[i]);
//			printf("\r\n%s",&cbuf[54]);
//		}
			
#endif
	
}
#else
void DM9000_SendPacket(struct pbuf *p)
{
	struct pbuf *q;
	uint16_t pbuf_index = 0;
	u8 word[2], word_index = 0;
	
	xSemaphoreTake(dm9000lock,portMAX_DELAY); 	//请求互斥信号量,锁定DM9000 
	
	DM9000_WriteReg(IMR,IMR_PAR);		//关闭网卡中断 
	DM_ADD=MWCMD;					//发送此命令后就可以将要发送的数据搬到DM9000 TX SRAM中	//DM_ADD //DM9000->REG
	q=p;
	//向DM9000的TX SRAM中写入数据，一次写入两个字节数据
	//当要发送的数据长度为奇数的时候，我们需要将最后一个字节单独写入DM9000的TX SRAM中

 	while(q)
	{
		if (pbuf_index < q->len)
		{
			word[word_index++] = ((u8_t*)q->payload)[pbuf_index++];
			if (word_index == 2)
			{
				DM_CMD=((uint16_t)word[1]<<8)|word[0];
				word_index = 0;
			}
		}
		else
		{
			q=q->next;
			pbuf_index = 0;
		}
	}
	//还有一个字节未写入TX SRAM
	if(word_index==1)DM_CMD=word[0];
	//向DM9000写入发送长度
	DM9000_WriteReg(TXPLL,p->tot_len&0XFF);
	DM9000_WriteReg(TXPLH,(p->tot_len>>8)&0XFF);		//设置要发送数据的数据长度
	DM9000_WriteReg(TCR,0X01);						//启动发送 
	while((DM9000_ReadReg(ISR)&0X02)==0);			//等待发送完成 
	DM9000_WriteReg(ISR,0X02);						//清除发送完成中断 
 	DM9000_WriteReg(IMR,dm9000cfg.imr_all);			//DM9000网卡接收中断使能

	xSemaphoreGive(dm9000lock);								//发送互斥信号量,解锁DM9000
}
#endif
//以上是发送数据包，过程很简单。而接收数据包确需要些说明了。DM9000从网络中接到一个数据包后，会在数据包前面加上4个字节，分别为“01H”、
//“status”（同RSR寄存器的值）、“LENL”（数据包长度低8位）、“LENH”（数据包长度高8位）。所以首先要读取这4个字节来确定数据包的状态，
//第一个字节“01H”表示接下来的是有效数据包，若为“00H”则表示没有数据包，若为其它值则表示网卡没有正确初始化，需要从新初始化。
//    如果接收到的数据包长度小于60字节，则DM9000会自动为不足的字节补上0，使其达到60字节。同时，在接收到的数据包后DM9000还会自动添加4个CRC校验字节。
//可以不予处理。于是，接收到的数据包的最小长度也会是64字节。当然，可以根据TCP/IP协议从首部字节中出有效字节数，这部分在后面讲解。下面为接收数据包的函数。
//接收数据包
//参数：datas为接收到是数据存储位置（以字节为单位）
//返回值：接收成功返回数据包类型，不成功返回0

uint8_t check_rx(void)
{
	OS_ERR      err;
	uint8_t ready;
//						互斥信号量指针	//超时时间（节拍） //选项	 //时间戳-不获取 //返回错误类型
		OSMutexPend (&dm9000_sem_lock, 0, 	OS_OPT_PEND_BLOCKING, 0, &err); 
	
	ready = dm9000_reg_read(MRCMDX);
	
		OSMutexPost (&dm9000_sem_lock, OS_OPT_POST_NONE, &err); 
	return ready;
}

uint16_t rx_status, rx_length;

struct pbuf *DM9000_Receive_Packet(void)
{
	struct pbuf* p;
	struct pbuf* q;
    uint8_t rxbyte;
//	vuint16_t
	
    uint16_t* data;
	uint16_t dummy; 
	int len;
	p=NULL; 
	OS_ERR      err;

#ifndef use_dm9000	
	return p;//不初始化，球机跟电脑连接
#else
	
#endif	
	
//#if printf_test	
//	int i;
//	i=0;
//#endif		
	if(dm9k_init_flag==0) return (struct pbuf*)p;
	
//						互斥信号量指针	//超时时间（节拍） //选项	 //时间戳-不获取 //返回错误类型
	OSMutexPend (&dm9000_sem_lock, 0, 	OS_OPT_PEND_BLOCKING, 0, &err); 

//__error_retry:	
	
	rxbyte = dm9000_reg_read(MRCMDX); // 第一次读取 ready=4001
	
	if(rxbyte  != 0x01)
	{
		udelay(4000);
		rxbyte = dm9000_reg_read(MRCMDX); // 第二次读取，总能获取到数据

		if(rxbyte   != 0x01)//& 0x01
		{
			if(rxbyte  != 0x00) //若第二次读取到的不是 01H 或 00H ，则表示没有初始化成功 //& 0x01
			{
				printf("读数据错误，重启 ready=%X\r\n",rxbyte);
				dm9000_reg_write(IMR, 0x80);//屏蔽网卡中断
				dm9k_init(mac_addr);//mac_addr //重新初始化
				dm9000_reg_write(IMR, 0x81);//打开网卡中断
			}
			else
			{
//				printf("没有数据 ready=%X\r\n",rxbyte);
			}
//						互斥信号量指针	 	//选项	 			//返回错误类型	
			OSMutexPost (&dm9000_sem_lock, OS_OPT_POST_NONE, &err); 
			return (struct pbuf*)p;
		}
	}	
	
//	DM9000_ReadReg(MRCMDX);					//假读
//	rxbyte=(uint8_t)DM_CMD;			//DM9000->DATA;						//进行第二次读取 
//	if(rxbyte)										//接收到数据
	
	
	{
//		if(rxbyte>1)								//rxbyte大于1，接收到的数据错误,挂了		
//		{
//			printf("dm9000 rx: rx error, stop device\r\n");
//			DM9000_WriteReg(RCR,0x00);
//			DM9000_WriteReg( ISR,0x80);		 
//			return (struct pbuf*)p;
//		}
		
		DM_ADD= MRCMD;
		rx_status=DM_CMD;
		rx_length=DM_CMD;  
		
		if(rx_length>DM9000_PKT_MAX)
		{
			return (struct pbuf*)p;
		}
//		if(rx_length>=1000) printf("rx_length=%d",rx_length);
		p=pbuf_alloc(PBUF_RAW,rx_length,PBUF_POOL);	//pbufs内存池分配pbuf
		if(p!=NULL)									//内存申请成功
		{
				for(q=p;q!=NULL;q=q->next)
				{
						data=(uint16_t*)q->payload;
					
						len=q->len;
						while(len>0)
						{
								*data=DM_CMD;
								
								data++;
								len-= 2;
						}
				}
#if printf_test						
		tcp_ip_printf((uint8_t*)p->payload,(p->len-4),0);
#endif		
		}
		else										//内存申请失败
		{
			printf("pbuf内存申请失败:%d\r\n",rx_length);
            data=&dummy;
			udelay(20);
			len=rx_length;
			udelay(20);
			while(len)
			{
				*data=DM_CMD;
				len-=2;
			}
        }	
		//根据rx_status判断接收数据是否出现如下错误：FIFO溢出、CRC错误
		//对齐错误、物理层错误，如果有任何一个出现的话丢弃该数据帧，
		//当rx_length小于64或者大于最大数据长度的时候也丢弃该数据帧
		if((rx_status&0XBF00) || (rx_length < 0X40) || (rx_length > DM9000_PKT_MAX))
		{
			printf("rx_status:%#x\r\n",rx_status);
			if (rx_status & 0x100)printf("rx fifo error\r\n");
            if (rx_status & 0x200)printf("rx crc error\r\n");
            if (rx_status & 0x8000)printf("rx length error\r\n");
            if (rx_length>DM9000_PKT_MAX)
			{
				printf("rx length too big\r\n");
				DM9000_WriteReg(NCR, NCR_RST); 	//复位DM9000
				udelay(5000);
			}
			if(p!=NULL)pbuf_free((struct pbuf*)p);		//释放内存
			p=NULL;
//			goto __error_retry;
		}
	}
//	else
//    {
//        DM9000_WriteReg(ISR,ISR_PTS);			//清除所有中断标志位
//        dm9000cfg.imr_all=IMR_PAR|IMR_PRI;				//重新接收中断 
//        DM9000_WriteReg(IMR, dm9000cfg.imr_all);
//    } 
	
//						互斥信号量指针	 	//选项	 			//返回错误类型	
			OSMutexPost (&dm9000_sem_lock, OS_OPT_POST_NONE, &err); 

	return (struct pbuf*)p; 
}

//注意：上面的函数用到了一些宏定义，已经在头文件中定义过，这里说明一下：其中uint16定义为两个字节的变量，根据C编译器进行定义。


void DMA9000_ISRHandler(void)
{
//	BaseType_t xHigherPriorityTaskWoken;
	uint16_t int_status;
	uint16_t last_io; 
	OS_ERR      err;
	
	
	last_io = DM_ADD;
	int_status=DM9000_ReadReg(ISR); 
	DM9000_WriteReg(ISR,int_status);	//清除中断标志位，DM9000的ISR寄存器的bit0~bit5写1清零
	
	if(int_status & ISR_ROS)
	{
		printf("overflow \r\n");
	}
    if(int_status & ISR_ROOS)
	{
		printf("overflow counter overflow \r\n");
	}		
	if(int_status & ISR_PRS)		//接收中断
	{  
//多值信号量控制块指针	//选项	 //返回错误类型
			OSSemPost (&dm9k_input_sem, OS_OPT_POST_NO_SCHED, &err);    //处理接收到数据帧 
	} 
	if(int_status&ISR_PTS)			//发送中断
	{ 
									//接收中断处理,这里没用到
	}
	DM_ADD=last_io;	
}


struct arp_hdr //以太网头部+ARP首部结构
{
	struct eth_hdr ethhdr;    //以太网首部
	uint16_t hwtype;     //硬件类型(1表示传输的是以太网MAC地址)
	uint16_t protocol;   //协议类型(0x0800表示传输的是IP地址)
	unsigned char hwlen;     //硬件地址长度(6)
	unsigned char protolen;    //协议地址长度(4)
	uint16_t opcode;     //操作(1表示ARP请求,2表示ARP应答)
	unsigned char smac[6];    //发送端MAC地址
	unsigned char sipaddr[4];    //发送端IP地址
	unsigned char dmac[6];    //目的端MAC地址
	unsigned char dipaddr[4];    //目的端IP地址
};

void dm9000_reg_write(unsigned char reg, unsigned char data)
{
//	udelay(30);
	DM_ADD =reg;//将寄存器地址写到INDEX端口 ((( uint16_t)reg)<<8);// 
//	udelay(30);
	DM_CMD =data;// cmd;//((( uint16_t)data)<<8)+0x23;// data;// 将数据写到DATA端口，即写进寄存器 
}
//从DM9000寄存器读数据
uint16_t dm9000_reg_read(unsigned char reg)
{
 //   udelay(30);
    DM_ADD = reg;	//((( uint16_t)reg)<<8);//
 //   udelay(30);
    return DM_CMD;//将数据从寄存器中读出
}

void dm9000_PHY_write(unsigned char reg, uint16_t data)
{
	dm9000_reg_write(EPAR_PHY_AR, (0x40|reg));	// 写 PHY 地址//0x40|
	dm9000_reg_write(EPDRL_PHY_DRH, (unsigned char)(data>>8));		// 写数据高字节
	dm9000_reg_write(EPDRL_PHY_DRL, (unsigned char)data);		// 写数据低字节
	dm9000_reg_write(EPCR_PHY_CR, 0x0a);	// 开始写入数据
	
	dm9000_reg_write(EPCR_PHY_CR, 0x00);		// 清除写使能
}



uint16_t dm9000_PHY_read(unsigned char reg)
{
	uint16_t data;
	dm9000_reg_write(EPAR_PHY_AR, (0x40|reg));	// 写 PHY 地址
	dm9000_reg_write(EPCR_PHY_CR, 0xc);	/* Issue phyxcer read command */
	
	udelay(100000);
	
	dm9000_reg_write(EPCR_PHY_CR, 0x00);	
	
	data=dm9000_reg_read(EPDRL_PHY_DRH);
	data=(data<<8)+dm9000_reg_read(EPDRL_PHY_DRL);
	
	return data;
}	

//uint64_t hsum = 0;
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







