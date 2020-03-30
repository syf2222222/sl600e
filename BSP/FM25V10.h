//FM25V10 Op-code Commands
#define WREN           0x06 //Set Write Enable Latch
#define WRDI           0x04 //Write Disable
#define RDSR           0x05 //Read Status Register
#define WRSR           0x01 //Write Status Register
#define READ           0x03 //Read Memory Data
#define FSTRD          0x0B //Fast Read Memory Data
#define WRITE          0x02 //Write Memory Data
#define SLEEP          0xB9 //Enter Sleep Mode
#define RDID           0x9F //Read Device ID
#define SNR            0xC3 //Read S/N



//拍照时间	通道号	预置位号	包数高位	包数低位
//6字节			1字节		1字节			1字节	1字节
#define sys_buf_flash		0
//#define flash_save(ADD,len)		WriteFM25Operation(ADD-STAA+STAA_add,ADD,len)


void ReadFM25DeviceID(unsigned char *buf);
void WriteFM25Operation(unsigned int addr,unsigned char  * data,unsigned int len);
void ReadFM25Operation(unsigned int addr,unsigned char  * data,unsigned int len);


//extern uint8_t STAA[200];

	


extern int flash_save(unsigned char *ADD,unsigned char *buf,int len);
extern int flash_read(unsigned char *ADD,unsigned char *buf,int len);
	
#define		flash_index_cnt_star	4000
#define		flash_index_ph_star	4096
#define		flash_index_qx_star	(flash_index_ph_star+256*12)
#define		flash_index_ll_star	(flash_index_qx_star+256*30)
