
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

//struct dm9000_config dm9000cfg;				//DM9000���ýṹ��


//uint16_t check_sum(uint8_t *packet,uint16_t packlen);

#define DM9000_PKT_MAX	1518  // IP 1500 + ��̫���ײ� 14 +CRC 4 //1518

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
//��DM9000�Ĵ���д����
void dm9000_reg_write(unsigned char reg, unsigned char data);
uint16_t dm9000_reg_read(unsigned char reg);

void dm9000_PHY_write(unsigned char reg, uint16_t data);

uint16_t dm9000_PHY_read(unsigned char reg);
	
//DM9000��ʼ��

void Test_DM9000_ID(void)
{
	uint32_t id;
	id  = dm9000_reg_read(0x28) << 0;	
	id |= dm9000_reg_read(0x29) << 8;
	id |= dm9000_reg_read(0x2A) << 16;
	id |= dm9000_reg_read(0x2B) << 24;  
  
#if printf_int_test
	printf("���� DM9000ID= 0x%X\r\n",id);//A4600,,90000A46
#endif
}

void Read_DM9000_MAC(void)
{
#if printf_int_test	
	uint16_t i;
	printf("���� MAC= ");
	
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
	return;//����ʼ�����������������
#endif	
#if printf_int_test	
	printf("//////////////////////////////��ʼ����ʼ//////////////////////////////\r\n");
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

/*���ϲ���������һ��IO�ڿ���DM9000��RST���ţ�ʹ�临λ����һ������ʡ�ԣ�����������������λ����*/
	

    dm9000_reg_write(GPCR, 0x01);//���� GPCR(1EH) bit[0]=1��ʹDM9000��GPIO3Ϊ�����
    dm9000_reg_write(GPR, 0x00);//GPR bit[0]=0 ʹDM9000��GPIO3���Ϊ���Լ����ڲ�PHY��
//    udelay(5000);//��ʱ2ms���ϵȴ�PHY�ϵ硣
		OSTimeDly ( 5, OS_OPT_TIME_DLY, & err );
    dm9000_reg_write(NCR, 0x03);//�����λ
//    udelay(3000);//��ʱ20us���ϵȴ������λ���
		OSTimeDly ( 3, OS_OPT_TIME_DLY, & err );
    dm9000_reg_write(NCR, 0x00);//��λ��ɣ�������������ģʽ��
    dm9000_reg_write(NCR, 0x03);//�ڶ��������λ��Ϊ��ȷ�������λ��ȫ�ɹ����˲����Ǳ�Ҫ�ġ�
		
		
		
		dm9000_reg_write(NCR, 0x00);
//    udelay(3000);
		OSTimeDly ( 3, OS_OPT_TIME_DLY, & err );
//   dm9000_reg_write(NCR, 0x54);//@@ ʹ���ڲ�PHY  dm9000_reg_write(NCR, 0x80);
		
		dm9000_reg_write(IMR, 0x80);
		
/*���������DM9000�ĸ�λ����*/
    dm9000_reg_write(NSR, 0x2c);//�������״̬��־λ
    dm9000_reg_write(ISR, 0x0f);//��������жϱ�־λ //0xbf
/*���������־λ*/
    dm9000_reg_write(RCR, 0x39);//���տ���  0x39	//0x3b  
		
    dm9000_reg_write(TCR, 0x00);//���Ϳ���
		dm9000_reg_write(TCR2, 0x80);	 /* Switch LED to mode 1 */
    dm9000_reg_write(BPTR, 0x3f);//����RX����ͷ�ֵ��С�ڽ�����ӵ��
    dm9000_reg_write(FCTR, 0x38);//����FIFO����3K��8K
    dm9000_reg_write(RTFCR, 0xff);//����һЩ���ƹ���
    dm9000_reg_write(SMCR, 0x00);//δ��������ģʽ
/*�����ǹ��ܿ��ƣ����幦�ܲο���һƪ�����е�˵������ο������ֲ�Ľ���*/

//	temp=*(vu32*)(0x1FFFF7E8);				//��ȡSTM32��ΨһID��ǰ24λ��ΪMAC��ַ�����ֽ�
//	dm9000cfg.mac_addr[0]=2;
//	dm9000cfg.mac_addr[1]=0;
//	dm9000cfg.mac_addr[2]=0;
//	dm9000cfg.mac_addr[3]=(temp>>16)&0XFF;	//�����ֽ���STM32��ΨһID
//	dm9000cfg.mac_addr[4]=(temp>>8)&0XFFF;
//	dm9000cfg.mac_addr[5]=temp&0XFF;
	
    for(i=0; i<6; i++)
        dm9000_reg_write(PAR + i, MAC_ADD[i]);//mac_addr[]�Լ�����һ�°ɣ�6���ֽڵ�MAC��ַ
				
//		for(i=0; i<8; i++)
//        dm9000_reg_write(MAR + i, 0xff);//mac_addr[]�Լ�����һ�°ɣ�6���ֽڵ�MAC��ַ		
				
/*���ϴ洢MAC��ַ�����������ַ����оƬ��ȥ������û����EEPROM��������Ҫ�Լ�д��ȥ*/
/*����MAC��ַ��˵����Ҫ�ο���������鼮������*/
    dm9000_reg_write(NSR, 0x2c);
    dm9000_reg_write(ISR, 0x3f);
/*Ϊ�˱��գ������������һ�α�־λ*/
    dm9000_reg_write(IMR, 0x81);
/*�ж�ʹ�ܣ�����˵�ж����Σ���������������Ҫ���жϣ��رղ���Ҫ�ģ�����ֻ������һ�������ж�*/
/*�������мĴ����ľ��庬��ο���һƪ���£���ο������ֲ�*/

	dm9000_PHY_write(BMCR,0x1000); //�Զ�Э��ʹ�� 0x1000   //0x2100

//	dm9000_reg_write(GPR, 0); //@@ �������
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
//				printf("//////////////////////////////�����쳣//////////////////////////////\r\n");
				printf("\r\n��Ƶ�����쳣");
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

		printf("//////////////////////////////��ʼ������//////////////////////////////\r\n");
#endif
}
#endif

void dm9k_reset(void)
{
	printf("\r\n��λDM9000");
	dm9000_reg_write(IMR, 0x80);//���������ж�
	dm9k_init(mac_addr);//mac_addr //���³�ʼ��
	dm9000_reg_write(IMR, 0x81);//�������ж�
}
	
//#define dm9000_reg_write	dm9000_reg_write 
//#define dm9000_io_read	dm9000_reg_read
//#define NCR_RST 0x01
//#define GPCR_GEP_CNTL 0x01

//�����Ͷ�DM9000��ʼ������ˣ���ô����ͦ�򵥵İɡ�
//3�����͡��������ݰ�
//ͬ�����Գ���Ϊ����ͨ��ע��˵����
//�������ݰ�
//������datasΪҪ���͵����ݻ����������ֽ�Ϊ��λ����lengthΪҪ���͵����ݳ��ȣ������ֽڣ���

//ͨ��DM9000�������ݰ�
//p:pbuf�ṹ��ָ��

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
	return ;//����ʼ�����������������
#endif
	
//	xSemaphoreTake(dm9000lock,portMAX_DELAY); 	//���󻥳��ź���,����DM9000 
//	OSSemPend(dm9000_sem_lock,0,&err);	
	
	//						�����ź���ָ��	//��ʱʱ�䣨���ģ� //ѡ��	 //ʱ���-����ȡ //���ش�������
	OSMutexPend (&dm9000_sem_lock, 0, 	OS_OPT_PEND_BLOCKING, 0, &err); 
	
	
	DM9000_WriteReg(IMR,IMR_PAR);		//�ر������ж� 
	
	q=p;
	//��DM9000��TX SRAM��д�����ݣ�һ��д�������ֽ�����
	//��Ҫ���͵����ݳ���Ϊ������ʱ��������Ҫ�����һ���ֽڵ���д��DM9000��TX SRAM��
//#if printf_test	
//	int i;
//	i=0;
//#endif	

	pbuf_index=0;
		DM_ADD = MWCMD;
//	DM9000->REG=DM9000_MWCMD;					//���ʹ������Ϳ��Խ�Ҫ���͵����ݰᵽDM9000 TX SRAM��	
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
//		pbuf_index=i; //�ܳ���
	//����һ���ֽ�δд��TX SRAM
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
//	pbuf_index=i; //�ܳ���
////	printf("\r\npbuf_index=%d",pbuf_index);
//	
////	printf("\r\npbuf_index=%d",pbuf_index);
////	pbuf_index=p->tot_len;
////	printf("\r\npbuf_index=%d",pbuf_index);
//	
//	DM_ADD = MWCMD;
////	DM9000->REG=DM9000_MWCMD;					//���ʹ������Ϳ��Խ�Ҫ���͵����ݰᵽDM9000 TX SRAM��	
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
	
	//��DM9000д�뷢�ͳ���
	DM9000_WriteReg(TXPLL,p->tot_len&0XFF);
	DM9000_WriteReg(TXPLH,(p->tot_len>>8)&0XFF);		//����Ҫ�������ݵ����ݳ���
	
//	printf("\r\ntot_len=%d",p->tot_len);
	
	DM9000_WriteReg(TCR,0X01);						//�������� 
	while((DM9000_ReadReg(ISR)&0X02)==0);			//�ȴ�������� 
	DM9000_WriteReg(ISR,0X02);						//�����������ж� 
	dm9000_reg_write(IMR, 0x81);//DM9000�����Ľ����ж�ʹ��
	
//						�����ź���ָ��	 	//ѡ��	 			//���ش�������	

			OSMutexPost (&dm9000_sem_lock, OS_OPT_POST_NONE, &err); 
	
	
//	//����һ���ֽ�δд��TX SRAM
//	if(word_index==1)DM_CMD=word[0];
//	//��DM9000д�뷢�ͳ���
//	DM9000_WriteReg(TXPLL,p->tot_len&0XFF);
//	DM9000_WriteReg(TXPLH,(p->tot_len>>8)&0XFF);		//����Ҫ�������ݵ����ݳ���
//	DM9000_WriteReg(TCR,0X01);						//�������� 
//	while((DM9000_ReadReg(ISR)&0X02)==0);			//�ȴ�������� 
//	DM9000_WriteReg(ISR,0X02);						//�����������ж� 
// 	DM9000_WriteReg(IMR,dm9000cfg.imr_all);			//DM9000���������ж�ʹ��
	
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
	
	xSemaphoreTake(dm9000lock,portMAX_DELAY); 	//���󻥳��ź���,����DM9000 
	
	DM9000_WriteReg(IMR,IMR_PAR);		//�ر������ж� 
	DM_ADD=MWCMD;					//���ʹ������Ϳ��Խ�Ҫ���͵����ݰᵽDM9000 TX SRAM��	//DM_ADD //DM9000->REG
	q=p;
	//��DM9000��TX SRAM��д�����ݣ�һ��д�������ֽ�����
	//��Ҫ���͵����ݳ���Ϊ������ʱ��������Ҫ�����һ���ֽڵ���д��DM9000��TX SRAM��

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
	//����һ���ֽ�δд��TX SRAM
	if(word_index==1)DM_CMD=word[0];
	//��DM9000д�뷢�ͳ���
	DM9000_WriteReg(TXPLL,p->tot_len&0XFF);
	DM9000_WriteReg(TXPLH,(p->tot_len>>8)&0XFF);		//����Ҫ�������ݵ����ݳ���
	DM9000_WriteReg(TCR,0X01);						//�������� 
	while((DM9000_ReadReg(ISR)&0X02)==0);			//�ȴ�������� 
	DM9000_WriteReg(ISR,0X02);						//�����������ж� 
 	DM9000_WriteReg(IMR,dm9000cfg.imr_all);			//DM9000���������ж�ʹ��

	xSemaphoreGive(dm9000lock);								//���ͻ����ź���,����DM9000
}
#endif
//�����Ƿ������ݰ������̺ܼ򵥡����������ݰ�ȷ��ҪЩ˵���ˡ�DM9000�������нӵ�һ�����ݰ��󣬻������ݰ�ǰ�����4���ֽڣ��ֱ�Ϊ��01H����
//��status����ͬRSR�Ĵ�����ֵ������LENL�������ݰ����ȵ�8λ������LENH�������ݰ����ȸ�8λ������������Ҫ��ȡ��4���ֽ���ȷ�����ݰ���״̬��
//��һ���ֽڡ�01H����ʾ������������Ч���ݰ�����Ϊ��00H�����ʾû�����ݰ�����Ϊ����ֵ���ʾ����û����ȷ��ʼ������Ҫ���³�ʼ����
//    ������յ������ݰ�����С��60�ֽڣ���DM9000���Զ�Ϊ������ֽڲ���0��ʹ��ﵽ60�ֽڡ�ͬʱ���ڽ��յ������ݰ���DM9000�����Զ����4��CRCУ���ֽڡ�
//���Բ��账�����ǣ����յ������ݰ�����С����Ҳ����64�ֽڡ���Ȼ�����Ը���TCP/IPЭ����ײ��ֽ��г���Ч�ֽ������ⲿ���ں��潲�⡣����Ϊ�������ݰ��ĺ�����
//�������ݰ�
//������datasΪ���յ������ݴ洢λ�ã����ֽ�Ϊ��λ��
//����ֵ�����ճɹ��������ݰ����ͣ����ɹ�����0

uint8_t check_rx(void)
{
	OS_ERR      err;
	uint8_t ready;
//						�����ź���ָ��	//��ʱʱ�䣨���ģ� //ѡ��	 //ʱ���-����ȡ //���ش�������
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
	return p;//����ʼ�����������������
#else
	
#endif	
	
//#if printf_test	
//	int i;
//	i=0;
//#endif		
	if(dm9k_init_flag==0) return (struct pbuf*)p;
	
//						�����ź���ָ��	//��ʱʱ�䣨���ģ� //ѡ��	 //ʱ���-����ȡ //���ش�������
	OSMutexPend (&dm9000_sem_lock, 0, 	OS_OPT_PEND_BLOCKING, 0, &err); 

//__error_retry:	
	
	rxbyte = dm9000_reg_read(MRCMDX); // ��һ�ζ�ȡ ready=4001
	
	if(rxbyte  != 0x01)
	{
		udelay(4000);
		rxbyte = dm9000_reg_read(MRCMDX); // �ڶ��ζ�ȡ�����ܻ�ȡ������

		if(rxbyte   != 0x01)//& 0x01
		{
			if(rxbyte  != 0x00) //���ڶ��ζ�ȡ���Ĳ��� 01H �� 00H �����ʾû�г�ʼ���ɹ� //& 0x01
			{
				printf("�����ݴ������� ready=%X\r\n",rxbyte);
				dm9000_reg_write(IMR, 0x80);//���������ж�
				dm9k_init(mac_addr);//mac_addr //���³�ʼ��
				dm9000_reg_write(IMR, 0x81);//�������ж�
			}
			else
			{
//				printf("û������ ready=%X\r\n",rxbyte);
			}
//						�����ź���ָ��	 	//ѡ��	 			//���ش�������	
			OSMutexPost (&dm9000_sem_lock, OS_OPT_POST_NONE, &err); 
			return (struct pbuf*)p;
		}
	}	
	
//	DM9000_ReadReg(MRCMDX);					//�ٶ�
//	rxbyte=(uint8_t)DM_CMD;			//DM9000->DATA;						//���еڶ��ζ�ȡ 
//	if(rxbyte)										//���յ�����
	
	
	{
//		if(rxbyte>1)								//rxbyte����1�����յ������ݴ���,����		
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
		p=pbuf_alloc(PBUF_RAW,rx_length,PBUF_POOL);	//pbufs�ڴ�ط���pbuf
		if(p!=NULL)									//�ڴ�����ɹ�
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
		else										//�ڴ�����ʧ��
		{
			printf("pbuf�ڴ�����ʧ��:%d\r\n",rx_length);
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
		//����rx_status�жϽ��������Ƿ�������´���FIFO�����CRC����
		//���������������������κ�һ�����ֵĻ�����������֡��
		//��rx_lengthС��64���ߴ���������ݳ��ȵ�ʱ��Ҳ����������֡
		if((rx_status&0XBF00) || (rx_length < 0X40) || (rx_length > DM9000_PKT_MAX))
		{
			printf("rx_status:%#x\r\n",rx_status);
			if (rx_status & 0x100)printf("rx fifo error\r\n");
            if (rx_status & 0x200)printf("rx crc error\r\n");
            if (rx_status & 0x8000)printf("rx length error\r\n");
            if (rx_length>DM9000_PKT_MAX)
			{
				printf("rx length too big\r\n");
				DM9000_WriteReg(NCR, NCR_RST); 	//��λDM9000
				udelay(5000);
			}
			if(p!=NULL)pbuf_free((struct pbuf*)p);		//�ͷ��ڴ�
			p=NULL;
//			goto __error_retry;
		}
	}
//	else
//    {
//        DM9000_WriteReg(ISR,ISR_PTS);			//��������жϱ�־λ
//        dm9000cfg.imr_all=IMR_PAR|IMR_PRI;				//���½����ж� 
//        DM9000_WriteReg(IMR, dm9000cfg.imr_all);
//    } 
	
//						�����ź���ָ��	 	//ѡ��	 			//���ش�������	
			OSMutexPost (&dm9000_sem_lock, OS_OPT_POST_NONE, &err); 

	return (struct pbuf*)p; 
}

//ע�⣺����ĺ����õ���һЩ�궨�壬�Ѿ���ͷ�ļ��ж����������˵��һ�£�����uint16����Ϊ�����ֽڵı���������C���������ж��塣


void DMA9000_ISRHandler(void)
{
//	BaseType_t xHigherPriorityTaskWoken;
	uint16_t int_status;
	uint16_t last_io; 
	OS_ERR      err;
	
	
	last_io = DM_ADD;
	int_status=DM9000_ReadReg(ISR); 
	DM9000_WriteReg(ISR,int_status);	//����жϱ�־λ��DM9000��ISR�Ĵ�����bit0~bit5д1����
	
	if(int_status & ISR_ROS)
	{
		printf("overflow \r\n");
	}
    if(int_status & ISR_ROOS)
	{
		printf("overflow counter overflow \r\n");
	}		
	if(int_status & ISR_PRS)		//�����ж�
	{  
//��ֵ�ź������ƿ�ָ��	//ѡ��	 //���ش�������
			OSSemPost (&dm9k_input_sem, OS_OPT_POST_NO_SCHED, &err);    //������յ�����֡ 
	} 
	if(int_status&ISR_PTS)			//�����ж�
	{ 
									//�����жϴ���,����û�õ�
	}
	DM_ADD=last_io;	
}


struct arp_hdr //��̫��ͷ��+ARP�ײ��ṹ
{
	struct eth_hdr ethhdr;    //��̫���ײ�
	uint16_t hwtype;     //Ӳ������(1��ʾ���������̫��MAC��ַ)
	uint16_t protocol;   //Э������(0x0800��ʾ�������IP��ַ)
	unsigned char hwlen;     //Ӳ����ַ����(6)
	unsigned char protolen;    //Э���ַ����(4)
	uint16_t opcode;     //����(1��ʾARP����,2��ʾARPӦ��)
	unsigned char smac[6];    //���Ͷ�MAC��ַ
	unsigned char sipaddr[4];    //���Ͷ�IP��ַ
	unsigned char dmac[6];    //Ŀ�Ķ�MAC��ַ
	unsigned char dipaddr[4];    //Ŀ�Ķ�IP��ַ
};

void dm9000_reg_write(unsigned char reg, unsigned char data)
{
//	udelay(30);
	DM_ADD =reg;//���Ĵ�����ַд��INDEX�˿� ((( uint16_t)reg)<<8);// 
//	udelay(30);
	DM_CMD =data;// cmd;//((( uint16_t)data)<<8)+0x23;// data;// ������д��DATA�˿ڣ���д���Ĵ��� 
}
//��DM9000�Ĵ���������
uint16_t dm9000_reg_read(unsigned char reg)
{
 //   udelay(30);
    DM_ADD = reg;	//((( uint16_t)reg)<<8);//
 //   udelay(30);
    return DM_CMD;//�����ݴӼĴ����ж���
}

void dm9000_PHY_write(unsigned char reg, uint16_t data)
{
	dm9000_reg_write(EPAR_PHY_AR, (0x40|reg));	// д PHY ��ַ//0x40|
	dm9000_reg_write(EPDRL_PHY_DRH, (unsigned char)(data>>8));		// д���ݸ��ֽ�
	dm9000_reg_write(EPDRL_PHY_DRL, (unsigned char)data);		// д���ݵ��ֽ�
	dm9000_reg_write(EPCR_PHY_CR, 0x0a);	// ��ʼд������
	
	dm9000_reg_write(EPCR_PHY_CR, 0x00);		// ���дʹ��
}



uint16_t dm9000_PHY_read(unsigned char reg)
{
	uint16_t data;
	dm9000_reg_write(EPAR_PHY_AR, (0x40|reg));	// д PHY ��ַ
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







