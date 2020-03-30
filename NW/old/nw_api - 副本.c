

#include "bsp.h"

#include "nw.h"

unsigned char sent[100];//={0x68,0x43,0x43,0x30,0x31,0x39,0x36}

unsigned char checksum_4g(unsigned char* a,unsigned int n)
{
	unsigned char checksum;
	unsigned int i;
	
	checksum=0;
	for(i=0;i<n;i++)
	{
		checksum += a[i];
	}
	checksum=~checksum;
	return checksum;	
}

extern uint8_t LastTxT;
void  nw_sent_8(uint8_t *t_data,uint8_t len) //unsigned char command,unsigned char * t_data,unsigned int dlong
{
	unsigned char i;
	CPU_SR_ALLOC();

	
	OS_CRITICAL_ENTER();                             //�����ٽ��
	HAL_UART_Transmit(&huart3, t_data, len,100);

	LastTxT=worktime;
	printf("\r\nS->M %x,%d:%d:%d",t_data[7],systime[3],systime[4],systime[5]);

	OS_CRITICAL_EXIT();                          //�ٽ��

}

void  nw_sent(struct nw_ctl *t_data) //unsigned char command,unsigned char * t_data,unsigned int dlong
{
	unsigned char i;
	CPU_SR_ALLOC();
	
	memcpy(sent,STAA, 7);
	sent[7]=t_data->cmd;
	sent[8]=(t_data->len>>8);
	sent[9]=t_data->len;
	
	for(i=0;i<t_data->len;i++)
	{
		sent[10+i]=t_data->data[i];
	}
	sent[10+t_data->len]=checksum_4g(&sent[1],9+t_data->len);
	sent[11+t_data->len]=0x16;
	
	nw_sent_8(sent,(12+t_data->len));
	
}

void  nw_sent_88(uint8_t *t_data) //unsigned char command,unsigned char * t_data,unsigned int dlong
{
	unsigned char i;
	CPU_SR_ALLOC();
	
	memcpy(sent,STAA, 7);
	sent[7]=0x88; //ָ��
	sent[8]=0;
	sent[9]=7;	//����7
	memcpy(&sent[10],PSW, 4);
	
	sent[14]=t_data[0]; //ͨ����
	sent[15]=t_data[1];
	sent[16]=t_data[2];//
	
	sent[17]=checksum_4g(&sent[1],16);
	sent[18]=0x16;
	
	nw_sent_8(sent,19);
	
}

void  nw_sent_ph_time(uint8_t ch) //unsigned char command,unsigned char * t_data,unsigned int dlong
{
	CPU_SR_ALLOC();
	
	unsigned char checksum;
	unsigned char *flash_ptr;
	unsigned short flash_len,i;
	
	if(ch==1)
	{
		flash_ptr=aut_point_group1;
	}
	else if(ch==2)
	{
		flash_ptr=aut_point_group2;
	}
	else
	{
		printf("\r\nͨ������");
		return;
	}
	
	flash_len=flash_ptr[0]*3+1;
	printf("\r\nflash_len=%d",flash_len);
	
//����	ͨ����	����	��1��	��2��	��3��	��
//			ʱ	��	Ԥ��λ��	ʱ	��	Ԥ��λ��	ʱ	��	Ԥ��
//λ��	
//4�ֽ�	1�ֽ�	1�ֽ�	1�ֽ�	1�ֽ�	1�ֽ�	1�ֽ�	1�ֽ�	1�ֽ�	1�ֽ�	1�ֽ�	1�ֽ�	
		
	memcpy(sent,STAA, 7);
	sent[7]=0x8b;//t_data->cmd;
	sent[8]=((flash_len+1)>>8); // +1 ͨ����
	sent[9]=(flash_len+1);
		
	sent[10]=ch; //ͨ����
	
	checksum=0;
	for(i=1;i<=10;i++)
	{
		checksum += sent[i];
	}
	for(i=0;i<flash_len;i++) //����+����������
	{
		checksum += flash_ptr[i];
	}
	checksum=~checksum;
	
	sent[14]=checksum;
	sent[15]=0x16;
	
//	printf("3");
	OS_CRITICAL_ENTER();                             //�����ٽ��
	HAL_UART_Transmit(&huart3, sent,11,100);
	HAL_UART_Transmit(&huart3, flash_ptr,flash_len,100);
	HAL_UART_Transmit(&huart3, &sent[14],2,100);
	
	LastTxT=worktime;
	printf("\r\nS->M %x,%d:%d:%d",sent[7],systime[3],systime[4],systime[5]);
	OS_CRITICAL_EXIT();                          //�ٽ��	
}

void  nw_sent_ph(unsigned char *input,uint16_t len,uint16_t peket) //unsigned char command,unsigned char * t_data,unsigned int dlong
{
	unsigned char checksum;
	unsigned short datalen;
	unsigned int i;
	CPU_SR_ALLOC();
	
	
//	printf("\r\n����Ƭ����%d,��%d\r\n",peket,len);
	memcpy(sent,STAA, 7);
	sent[7]=0x85;//t_data->cmd;
	datalen=len+4;
	sent[8]=(datalen>>8);
	sent[9]=datalen;
	

	sent[10]=1;						//ͨ����
	sent[11]=0;						//Ԥ��λ��
	sent[12]=(peket>>8);	//������λ
	sent[13]=peket;				//������λ	
	
//	printf("2");
	checksum=0;
	for(i=1;i<=13;i++)
	{
		checksum += sent[i];
	}
	for(i=0;i<len;i++)
	{
		checksum += input[i];
	}
	checksum=~checksum;
	
	sent[14]=checksum;
	sent[15]=0x16;
	
//	printf("3");
	OS_CRITICAL_ENTER();                             //�����ٽ��
	HAL_UART_Transmit(&huart3, sent,14,100);
	HAL_UART_Transmit(&huart3, input,len,100);
	HAL_UART_Transmit(&huart3, &sent[14],2,100);
	
	LastTxT=worktime;
//	printf("\r\nS->M %x,peket=%d,len=%d,data=",sent[7],peket,len);
//	for(i=0;i<5;i++) printf("%02x",input[i]); printf("..."); for(i=len-5;i<len;i++) printf("%x",input[i]); 
	OS_CRITICAL_EXIT();                          //�ٽ��
}
void rev_err(unsigned char ctl,unsigned char data)
{
	CPU_SR_ALLOC();
	
	memcpy(sent,STAA, 7);
	sent[7]=ctl;
	sent[8]=0;
	sent[9]=2;
	sent[10]=data;
	sent[11]=data;
	sent[12]=checksum_4g(&sent[1],11);
	sent[13]=0x16;
	OS_CRITICAL_ENTER();                             //�����ٽ��
	HAL_UART_Transmit(&huart3, sent, (14),100);
	
	printf("\r\ne-m %x,%d:%d:%d",ctl,systime[3],systime[4],systime[5]);
	OS_CRITICAL_EXIT();                          //�ٽ��
}

uint16_t max_ll,fpj,qxj;



int get_qx_data(struct qx_data *qx_d);
extern struct TenS_data_LL MaxLL[3],MinLL[3];
extern struct TenS_data_FP MaxFP[3],MinFP[3];
extern uint8_t clean_LLQJ[3];

void DDDD(uint16_t *in,int len)
{
	int i;
	
	for(i=0;i<len;i++)
	{
		in[i]=(in[i]<<8)+(in[i]>>8);
	}
}

int Load_llqj_data(uint8_t i,uint8_t *load)
{
	
	if(i>2) return -1;
	
	memcpy(&load[0],&MaxLL[i],6);
	memcpy(&load[6],&MinLL[i],6);
	memcpy(&load[12],&MaxFP[i],6);
	memcpy(&load[18],&MinFP[i],6);
	
//	clean_LLQJ[i]=0;
	DDDD((uint16_t *)load,12);
		
	return 0;
}
void sampled_data_sent_22(unsigned char step,unsigned char zbs)
{
	char i;
						
//������֤	֡��ʶ	����	���ܵ�Ԫʶ����	�װ�	��һ��	�ڶ���	����	��N��
//4�ֽ�	1�ֽ�	1�ֽ�	1�ֽ�					
//�װ���ʽ��
//����ʱ�䣨��+��+��+ʱ+��+�룩��6�ֽڣ�+
//���������2�ֽڣ�+��ƫ�ǣ�2�ֽڣ�+��б�ǣ�2�ֽڣ�+
//��С������2�ֽڣ�+��ƫ�ǣ�2�ֽڣ�+��б�ǣ�2�ֽڣ�+
//����ƫ�ǣ�2�ֽڣ�+��б�ǣ�2�ֽڣ�+������2�ֽڣ�+
//��С��ƫ�ǣ�2�ֽڣ�+��б�ǣ�2�ֽڣ�+������2�ֽڣ�
					
//��һ�����Ժ������ʽ��
//���ϰ�����ʱ��2�ֽڣ�+���������2�ֽڣ�+��ƫ�ǣ�2�ֽڣ�+��б�ǣ�2�ֽڣ�+��С������2�ֽڣ�+��ƫ�ǣ�2�ֽڣ�+��б�ǣ�2�ֽڣ�+����ƫ�ǣ�2�ֽڣ�+��б�ǣ�2�ֽڣ�+������2�ֽڣ�+��С��ƫ�ǣ�2�ֽڣ�+��б�ǣ�2�ֽڣ�+������2�ֽڣ�
	if(step<=2)
	{
		nw_sc.cmd=0x22;		
		i=0;
		memcpy(nw_sc.data,PSW,4);
		i+=4;
		nw_sc.data[i++]=zbs;//֡��ʶ
		nw_sc.data[i++]=1;//����
		nw_sc.data[i++]=(0x10<<step);//���ܵ�Ԫʶ����
		
		memcpy(&nw_sc.data[i],systime,6);//����ʱ��
		i+=6;
		
		Load_llqj_data(step,&nw_sc.data[i]);
		i+=24;

		nw_sc.len=i;
		nw_sent(&nw_sc);
	}
}

void sampled_data_sent_25(unsigned char zbs)
{
	char i;
					
	{
//							68 43 43 30 30 30 31 22 00 25 31 32 33 34 11 01 10 13 0a 1d 09 23 3b 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 e4 16 
		nw_sc.cmd=0x25;
	
		i=0;
		memcpy(nw_sc.data,PSW,4);
		i+=4;
		nw_sc.data[i++]=zbs;//֡��ʶ
		nw_sc.data[i++]=1;//����
		
		get_qx_data((struct qx_data *) &nw_sc.data[i]);
		i+=sizeof(struct qx_data);	
		printf("\r\n��ȡ��������");

		nw_sc.len=i;
		nw_sent(&nw_sc);
	}
}

void nw_out(void)
{
//	unsigned int i;
		
	
}

