#include "stm32l4xx_hal.h"
#include "FM25V10.h"
#include <os.h>
#include "bsp.h"
#include "global.h" 
//#include <os_app_hooks.h>


#define  FM25_Enable()   do{unsigned char i;HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);for(i = 0;i < 5;i++);}while(0)
#define  FM25_Disable()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);

extern SPI_HandleTypeDef hspi2;
///////////////////////////////////////FM25///////////////////////////////////////

// /**
//  * @brief  Reads a byte from the SPI Flash.
//  *   This function must be used only if the Start_Read_Sequence
//  *   function has been previously called.
//  * @param  None
//  * @retval : Byte Read from the SPI Flash.
//  */
void ReadFM25DeviceID(unsigned char *buf)
{
	CPU_SR_ALLOC(); 
	uint8_t sendbuf[1];
	CPU_CRITICAL_ENTER(); //关中断

	FM25_Enable();
	sendbuf[0] = RDID;
	HAL_SPI_Transmit(&hspi2, sendbuf, 1,10);
	HAL_SPI_Receive(&hspi2, buf, 9,10);	
	FM25_Disable();

	CPU_CRITICAL_EXIT();//开中断	
}
/**
 Set Write Enable Latch
The WREN command must be issued prior to any write operation.
 Completing any write operation will automatically
 clear the write-enable latch and prevent further
writes without another WREN command.
*/
void SetFM25WriteEnable(void)
{
	uint8_t sendbuf[4];
	FM25_Enable();
	sendbuf[0] = WREN;
	HAL_SPI_Transmit(&hspi2, sendbuf, 1,10);
	FM25_Disable();
}
/**
 clearing the Write Enable Latch
*/
void ClearFM25WriteEnable(void)
{
	uint8_t sendbuf[4];
	FM25_Enable();
	sendbuf[0] = WRDI;
	HAL_SPI_Transmit(&hspi2, sendbuf, 1,10);
	FM25_Disable();
}

/*
Write Operation
All writes to the memory array begin with a WREN op-code. 
The next op-code is the WRITE instruction. 
This op-code is followed by a three-byte address value, 
which specifies the 17-bit address of the first data byte of the write operation.
Subsequent bytes are data and they are written sequentially. 
Addresses are incremented internally as long as the bus master continues to issue clocks.
If the last address of 1FFFFh is reached, the counter will roll over to 00000h.
Data is written MSB first.
*/
void WriteFM25Operation(unsigned int addr,unsigned char *databuf,unsigned int len)
{	
	uint32_t i;
	unsigned char sendbuf[4];
	CPU_SR_ALLOC(); 
	CPU_CRITICAL_ENTER();
	SetFM25WriteEnable();
  FM25_Enable();
	sendbuf[0] = WRITE;
	HAL_SPI_Transmit(&hspi2, sendbuf, 1,10);
	sendbuf[0] = (unsigned char)(addr >>16);
	HAL_SPI_Transmit(&hspi2, sendbuf, 1,10);
	sendbuf[0] = (unsigned char)(addr >>8);
	HAL_SPI_Transmit(&hspi2, sendbuf, 1,10);
	sendbuf[0] = (unsigned char)addr;
	HAL_SPI_Transmit(&hspi2, sendbuf, 1,10);	 
	for(i = 0;i < len;i++)
	 HAL_SPI_Transmit(&hspi2, &databuf[i], 1,10);	
	FM25_Disable();
	ClearFM25WriteEnable();   //   写禁能
	CPU_CRITICAL_EXIT();	

}
/*
Read Operation
After the falling edge of /S, the bus master can issue a READ op-code. 
Following this instruction is a three-byte address value (A16-A0),
specifying the address of the first data byte of the read operation.
Addresses are incremented internally as long as the bus master continues to issue clocks. 
If the last address of 1FFFFh is reached, the counter will roll over to 00000h. Data is read MSB first.
*/
void ReadFM25Operation(unsigned int addr,unsigned char *databuf,unsigned int len)
{

	unsigned char sendbuf[4];
	CPU_SR_ALLOC(); 
	CPU_CRITICAL_ENTER();
	FM25_Enable();
	sendbuf[0] = READ;
	HAL_SPI_Transmit(&hspi2, sendbuf, 1,10);
	sendbuf[0] = (unsigned char)(addr >>16);
	HAL_SPI_Transmit(&hspi2, sendbuf, 1,10);
	sendbuf[0] = (unsigned char)(addr >>8);
	HAL_SPI_Transmit(&hspi2, sendbuf, 1,10);
	sendbuf[0] = (unsigned char)addr;
	HAL_SPI_Transmit(&hspi2, sendbuf, 1,10);
	HAL_SPI_Receive(&hspi2, databuf, len,10);	
	FM25_Disable();
	CPU_CRITICAL_EXIT();	
}

//#define STAA_add		0

//#define flash_save(ADD,len)	WriteFM25Operation(ADD-STAA+STAA_add,ADD,len)
int flash_read(uint8_t *ADD,uint8_t *buf,int len)
{
//	int i;
		
	ReadFM25Operation(ADD-sys_buf_sram+sys_buf_flash,buf,len);
//	printf("\r\n读取数据:");
//	if(len<=10)
//	for(i=0;i<len;i++) printf("%d ",ADD[i]);
//	else
//	{
//		for(i=0;i<5;i++) printf("%d ",buf[i]);printf("...");
//		for(i=len-5;i<len;i++) printf("%d ",buf[i]);
//	}
	
	return 0;
}
int flash_save(uint8_t *ADD,uint8_t *buf,int len)
{
//	int i;
	
//	printf("\r\n存储数据:");
//	for(i=0;i<len;i++) printf("%d ",buf[i]);
	WriteFM25Operation(ADD-sys_buf_sram+sys_buf_flash,buf,len);
	
//	ReadFM25Operation(ADD-sys_buf_sram+sys_buf_flash,buf,len);
//	printf("\r\n验证数据:");
//	for(i=0;i<len;i++) printf("%02X ",buf[i]);
	
	return 0;
}

//uint16_t index_cnt[10];

//#define ll_sent_cnt1	index_cnt[0]
//#define ll_gain_cnt1		index_cnt[1]
//#define ll_sent_cnt2	index_cnt[2]
//#define ll_gain_cnt2		index_cnt[3]
//#define ll_sent_cnt3	index_cnt[4]
//#define ll_gain_cnt3		index_cnt[5]
//#define qx_sent_cnt		index_cnt[6]
//#define qx_gain_cnt		index_cnt[7]
//#define ph_sent_cnt		index_cnt[8]
//#define ph_gain_cnt		index_cnt[9]


//uint16_t	ph_sent_cnt,ph_gain_cnt,ll_sent_cnt[3],ll_sent_cnt[3],ll_gain_cnt[3],qx_sent_cnt,qx_gain_cnt;
int index_ph_read(uint8_t *out);

//GLOBAL_EXT uint16_t ll_sent_cnt[3];
//GLOBAL_EXT uint16_t ll_gain_cnt[3];

//GLOBAL_EXT uint16_t qx_sent_cnt;
//GLOBAL_EXT uint16_t qx_gain_cnt;

//GLOBAL_EXT uint16_t ph_sent_cnt;
//GLOBAL_EXT uint16_t ph_gain_cnt;
uint8_t buf[20];

//#define		flash_index_cnt_star	4000
//#define		flash_index_ph_star	4096
//#define		flash_index_qx_star	(flash_index_ph_star+256*12)
//#define		flash_index_ll_star	(flash_index_qx_star+256*30)

int clear_index_all(void)
{
	char i;
	ll_sent_cnt=0;
	ll_gain_cnt=0;

	qx_sent_cnt=0;
	qx_gain_cnt=0;
	ph_sent_cnt=0;
	ph_gain_cnt=0;
	
	for(i=0;i<20;i++)buf[i]=0;
	buf[17]=ph_sent_cnt;
	buf[19]=ph_gain_cnt;
	WriteFM25Operation(flash_index_cnt_star,buf,20);
	
	return 0;
}

int gain_index_all(void)
{	
	ReadFM25Operation(flash_index_cnt_star,buf,20);

	ll_sent_cnt=(buf[0]<<8)+buf[1];
	ll_gain_cnt=(buf[2]<<8)+buf[3];
	
	qx_sent_cnt=(buf[12]<<8)+buf[13];
	qx_gain_cnt=(buf[14]<<8)+buf[15];
	ph_sent_cnt=(buf[16]<<8)+buf[17];
	ph_gain_cnt=(buf[18]<<8)+buf[19];
	
	if((ll_sent_cnt>ll_gain_cnt)||(qx_sent_cnt>qx_gain_cnt)||(ph_sent_cnt>ph_gain_cnt))
	{
//		clear_index_all();
	}
	return 0;
}
	
int save_index_ll(void)
{
	buf[0]=(ll_sent_cnt>>8);//0;//
	buf[1]=(ll_sent_cnt);
	buf[2]=(ll_gain_cnt>>8);//0;//
	buf[3]=(ll_gain_cnt);
	
	WriteFM25Operation(flash_index_cnt_star,buf,4);
	return 0;
}
int save_index_qx(void)
{
	buf[0]=(qx_sent_cnt>>8);//0;//
	buf[1]=(qx_sent_cnt);
	buf[2]=(qx_gain_cnt>>8);//0;//
	buf[3]=(qx_gain_cnt);
	WriteFM25Operation(flash_index_cnt_star+12,buf,4);
	return 0;
}

int save_index_ph(void)
{
	buf[0]=(ph_sent_cnt>>8);//0;//
	buf[1]=(ph_sent_cnt);
	buf[2]=(ph_gain_cnt>>8);//0;//
	buf[3]=(ph_gain_cnt);
	WriteFM25Operation(flash_index_cnt_star+16,buf,4);
	return 0;
}

int read_write_sd(char *name,unsigned int addr,unsigned char *databuf,unsigned int len,uint8_t flag);

int read_data_ph(uint16_t rcnt,uint8_t *out)
{	
//	ReadFM25Operation(flash_index_ph_star+ph_sent_cnt*11,out,11);
	read_write_sd("0:index_ph.txt",ph_sent_cnt*11,out,11,0);
	
	
	if((out[0]<y_min)||(out[0]>y_max)||(out[1]==0)||(out[1]>12)||(out[2]==0)||(out[2]>31)) //年,月,日,不对
	{
		printf("\r\n拍照时间错误");
		return -1;
	}
//	printf("\r\n读取一个照片索引%d",ph_sent_cnt);
//	printf("\r\nread out=%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,",out[0],out[1],out[2],out[3],out[4],out[5],out[6],out[7],out[8],out[9],out[10]);
	return 0;
}
int save_data_ph(uint8_t *in)//每个数据11字节
{	
	uint16_t buf_cnt;
	
	
	if((in[0]<y_min)||(in[0]>y_max)||(in[1]==0)||(in[1]>12)||(in[2]==0)||(in[2]>31)) //年,月,日,不对
	{
		printf("\r\n拍照时间错误");
		return -1;
	}
	
	if(ph_gain_cnt>0) buf_cnt=ph_gain_cnt-1;
	else buf_cnt=0xffff;//255
	
	read_write_sd("0:index_ph.txt",buf_cnt*11,in,11,1);
//	WriteFM25Operation(flash_index_ph_star+buf_cnt*11,in,11); //time 6+ ch 1+ ps 1 + len 3
//	printf("\r\n保存一个照片索引%d",(ph_gain_cnt-1));
//	printf("\r\nsave in=%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,",in[0],in[1],in[2],in[3],in[4],in[5],in[6],in[7],in[8],in[9],in[10]);
	return 0;
}

char save_ph(unsigned char *in);

int index_ph_inc(uint8_t *in)
{
	signed char res=-1;
	
//	while((res!=0)&&(cnt<3))
//	{
//		res=save_ph(in);
//		ph_gain_cnt++;//if(++ph_gain_cnt>=256) ph_gain_cnt=0;
//	}

	res=save_ph(in);		//存储照片
	
	if(res==0)
	{
//		if(++ph_gain_cnt>=256) ph_gain_cnt=0;
//		ph_gain_cnt++;
		ph_gain_cnt++;//if(++ph_gain_cnt>=256) ph_gain_cnt=0;
		save_index_ph();  //保存索引值
		save_data_ph(in);	//保存照片信息
	}
	else	//卸载文件系统再挂载
	{
//		sd_err++;
		return -1;
	}

//	printf("\r\ninc ph_sent_cnt=%d,ph_gain_cnt=%d",ph_sent_cnt,ph_gain_cnt);
	return 0;
}

int delet_ph(unsigned char *index);
	
int index_ph_dec(unsigned char *index)
{
	if(ph_sent_cnt !=ph_gain_cnt)
	{
		ph_sent_cnt++;//if(++ph_sent_cnt>=256)ph_sent_cnt=0;
		save_index_ph();
		delet_ph(index);
	}
	else
	{
		printf("\r\nSD照片数据已为空");
	}
	
//	printf("\r\ndec ph_sent_cnt=%d,ph_gain_cnt=%d",ph_sent_cnt,ph_gain_cnt);
	return 0;
}

#define qx_data_nb	30

int read_data_qx(uint16_t rcnt,uint8_t *out)
{
	if(qx_sent_cnt==qx_gain_cnt)
	{
		printf("\r\n没有气象数据");
		return -1;
	}
//	ReadFM25Operation(flash_index_qx_star+qx_sent_cnt*qx_data_nb,out,qx_data_nb); 
	read_write_sd("0:index_qx.txt",qx_sent_cnt*qx_data_nb,out,qx_data_nb,0);
	
	if(out[0]==0) 
	{
		printf("\r\n气象数据 数量为0错误");
		return -1;
	}
	
	if((out[1]<y_min)||(out[1]>y_max)||(out[2]==0)||(out[2]>12)||(out[3]==0)||(out[3]>31)) //年,月,日,不对
	{
		printf("\r\n气象数据时间错误");
		return -1;
	}
	
//	printf("\r\n读取一个气象数据%d",qx_sent_cnt);
	return 0;
//	printf("\r\nread out=%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,",out[0],out[1],out[2],out[3],out[4],out[5],out[6],out[7],out[8],out[9],out[10]);
}
int save_data_qx(uint8_t *in)
{	
	uint16_t buf_cnt;
	
	if(in[0]==0) 
	{
		printf("\r\n气象数据 数量为0错误");
		return -1;
	}
	
	if((in[1]<y_min)||(in[1]>y_max)||(in[2]==0)||(in[2]>12)||(in[3]==0)||(in[3]>31)) //年,月,日,不对
	{
		printf("\r\n气象数据时间错误");
		return -1;
	}
	
	if(qx_gain_cnt>0) buf_cnt=qx_gain_cnt-1;
	else buf_cnt=0xffff;//255;
	read_write_sd("0:index_qx.txt",buf_cnt*qx_data_nb,in,qx_data_nb,1);	
//	WriteFM25Operation(flash_index_qx_star+buf_cnt*qx_data_nb,in,qx_data_nb); //time 6+ ch 1+ ps 1 + len 3
//	printf("\r\n保存一个气象数据%d",(qx_gain_cnt-1));
	return 0;
}

int index_qx_inc(uint8_t *in)
{
//	qx_gain_cnt++;
	qx_gain_cnt++;//if(++qx_gain_cnt>=256) qx_gain_cnt=0;//暂时限制最大256个数据
	save_index_qx();
	save_data_qx(in);
//	printf("\r\ninc qx_sent_cnt=%d,qx_gain_cnt=%d",qx_sent_cnt,qx_gain_cnt);
	return 0;
}
int index_qx_dec(void)
{
	if(qx_sent_cnt!=qx_gain_cnt)
	{
		qx_sent_cnt++;//if(++qx_sent_cnt>=256) qx_sent_cnt=0;
	}
	save_index_qx();
//	printf("\r\ndec qx_sent_cnt=%d,qx_gain_cnt=%d",qx_sent_cnt,qx_gain_cnt);
	return 0;
}

#define ll_data_nb	32

int read_data_ll(uint16_t rcnt,uint8_t *out)
{
//	char i;
	int add;
	
	if(ll_sent_cnt==ll_gain_cnt)
	{
		printf("\r\n没有拉力数据");
		return -1;
	}
	
	add=flash_index_ll_star+ll_sent_cnt*ll_data_nb;
	
	read_write_sd("0:index_ll.txt",add,out,ll_data_nb,0);	
//	ReadFM25Operation(add,out,ll_data_nb); 
	
	if(out[0]==0) 
	{
		printf("\r\n拉力数据 数量为0错误");
		return -1;
	}
	if((out[1]!=0x10)&&(out[1]!=0x20)&&(out[1]!=0x40))
	{
		printf("\r\n拉力数据 功能识别码错误");
		return -1;
	}
	if((out[2]<y_min)||(out[2]>y_max)||(out[3]==0)||(out[3]>12)||(out[4]==0)||(out[4]>31)) //年,月,日,不对
	{
		printf("\r\n拉力数据时间错误");
		return -1;
	}
	
//	printf("\r\n读取一个拉力数据%d:",ll_sent_cnt);
//	for(i=0;i<32;i++) printf("%02x ",out[i]);
	return 0;
//	printf("\r\nread out=%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,",out[0],out[1],out[2],out[3],out[4],out[5],out[6],out[7],out[8],out[9],out[10]);
}
int save_data_ll(uint8_t *in)
{	
//	char i;
	int add;
	uint16_t buf_cnt;
	
	if(in[0]==0) 
	{
		printf("\r\n拉力数据 数量为0错误");
		return -1;
	}
	if((in[1]!=0x10)&&(in[1]!=0x20)&&(in[1]!=0x40))
	{
		printf("\r\n拉力数据 功能识别码错误");
		return -1;
	}
	if((in[2]<y_min)||(in[2]>y_max)||(in[3]==0)||(in[3]>12)||(in[4]==0)||(in[4]>31)) //年,月,日,不对
	{
		printf("\r\n拉力数据时间错误");
		return -1;
	}
	
	if(ll_gain_cnt>0) buf_cnt=ll_gain_cnt-1;
	else buf_cnt=0xffff;//255;
	
	add=flash_index_ll_star+buf_cnt*ll_data_nb;
	read_write_sd("0:index_ll.txt",add,in,ll_data_nb,1);	
//	WriteFM25Operation(add,in,ll_data_nb); //time 6+ ch 1+ ps 1 + len 3
//	printf("\r\n保存一个拉力数据%d:",(ll_gain_cnt-1));
//	for(i=0;i<32;i++) printf("%02x ",in[i]);
	return 0;
}

int index_ll_inc(uint8_t *in)
{
//	ll_gain_cnt++;
	ll_gain_cnt++;//if(++ll_gain_cnt>=256) ll_gain_cnt=0;//暂时限制最大256个数据
	save_index_ll();
	save_data_ll(in);
//	printf("\r\ninc ll_sent_cnt=%d,ll_gain_cnt=%d",ll_sent_cnt,ll_gain_cnt);
	return 0;
}
int index_ll_dec(void)
{
	if(ll_sent_cnt!=ll_gain_cnt)
	{
		ll_sent_cnt++;//if(++ll_sent_cnt>=256) ll_sent_cnt=0;
	}
	save_index_ll();
//	printf("\r\ndec ll_sent_cnt=%d,ll_gain_cnt=%d",ll_sent_cnt,ll_gain_cnt);
	return 0;
}


