
#include "main.h"
#include "fatfs.h"
#include <stdio.h>
#include <includes.h>
#include "global.h" 
extern uint8_t retSD;    /* Return value for SD */
extern char SDPath[4];   /* SD logical drive path */
extern FATFS SDFatFS;    /* File system object for SD logical drive */
extern FIL SDFile;       /* File object for SD */

unsigned char sd_read_buf[1470];
extern unsigned short tt_pk_nb;
extern unsigned int tt_len;
void  nw_sent_ph(unsigned char *input,uint16_t len,uint8_t ch,uint8_t preset,uint16_t peket);
void MX_SDMMC1_SD_Init(void);
extern SD_HandleTypeDef hsd1;
//const char wData[]=" 新程序试一试！";
//char rData2[100]="";
FIL fp; 
UINT bw;
UINT br;

char str[40];

void MX_FATFS_Init(void);

//   /* 移动文件读/写指针到相对于文件起始处偏移为5000字节处 */
//   res = f_lseek(file, 5000);

uint16_t ll_write_point[3],ll_read_point[3];
uint16_t qx_write_point,qx_read_point;
uint16_t ph_write_point,ph_read_point;

uint16_t sd_err=0;
uint8_t sd_sta_h=0;

void MX_FATFS_Init(void);
extern	OS_MUTEX sd_sem_lock;

extern void udelay_no_inv(int cnt);


FRESULT f_mount_Twice(void)
{
	FRESULT res;
	uint8_t fat_err_times=0;
	CPU_SR_ALLOC(); 
	
	CPU_CRITICAL_ENTER(); //关中断
	do
	{
		res=f_mount(&SDFatFS,SDPath,1 );//SDPath  "0:"
		fat_err_times++;
	}while((fat_err_times<3)&&(res!=FR_OK));
	
	if(res!=FR_OK)
	{
		printf("\r\n挂载失败");
	}
	else
	{
		if(fat_err_times>1) printf("\r\n挂载%d次成功",fat_err_times-1);
		else printf("\r\n挂载成功");
	}
	CPU_CRITICAL_EXIT();//开中断
	return res;
}

void FatFsTaskStart(void)//unsigned char *in
{
//	uint8_t fat_err_times=0;
	
	if(f_mount_Twice() != FR_OK) sd_sta_h=0;
	else sd_sta_h=1;
}

int sd_reset(void)
{
	HAL_SD_DeInit(&hsd1);	//关电源,关SMMCSD
	MX_SDMMC1_SD_Init(); //SD驱动初始化,只有句柄赋值,应该可以不用再次赋值
	MX_FATFS_Init();			//初始化最终会调用到 SD_initialize(BYTE lun)->BSP_SD_Init(void)->HAL_SD_Init(SD_HandleTypeDef *hsd) ->HAL_SD_MspInit(SD_HandleTypeDef* sdHandle)

	if(f_mount_Twice() != FR_OK) sd_sta_h=0;
	else sd_sta_h=1;
	return 0;
}

FRESULT f_mkfs_Twice(void)
{
	FRESULT res;
	uint8_t fat_err_times=0;
	CPU_SR_ALLOC(); 
	
	CPU_CRITICAL_ENTER(); //关中断
	do
	{
		res = f_mkfs("",FM_FAT32,0,sd_read_buf,sizeof(sd_read_buf));
		fat_err_times++;
	}while((fat_err_times<3)&&(res!=FR_OK));
	
	if(res!=FR_OK)
	{
		printf("\r\n格式化失败");
	}
	else
	{
		if(fat_err_times>1) printf("\r\n格式化%d次成功",fat_err_times-1);
		else printf("\r\n格式化成功");
	}
	CPU_CRITICAL_EXIT();//开中断	
	return res;
}

int sd_mkfs(void)
{
	int res;
	CPU_SR_ALLOC(); 
	//挂载文件系统invalid
//	res = f_mount(&SDFatFS,SDPath,1);//"1:" SDPath
//	printf("\r\nf_mount res =%d",res);
	res=0;
	CPU_CRITICAL_ENTER(); //关中断
	res=f_mkfs_Twice();
	if(res!=FR_OK) sd_sta_h=0;
	else
	{
		res=f_mount_Twice();
		if(res != FR_OK) sd_sta_h=0;
		else sd_sta_h=1;
	}	
	CPU_CRITICAL_EXIT();//开中断	
	
	return res;
}


//#define PH_flag			(( uint8_t*) (http_sendbuf+0x800))	//
//#define PH_GET_T		(( uint8_t*) (PH_flag+1))
//#define PH_GET_CH		(( uint8_t*) (PH_GET_T+6))
//#define PH_GET_PS		(( uint8_t*) (PH_GET_CH+1))
//#define PH_LEN			(( uint8_t*) (PH_GET_PS+1))
//#define PH_DATA			(( uint8_t*) (PH_LEN+4))

//20191119202701_CC0196_1_1.jpg
//#define _MAX_LFN     40

FRESULT f_unlink_Twice(char *str)
{
	FRESULT res;
	uint8_t fat_err_times=0;
	CPU_SR_ALLOC(); 
	
	CPU_CRITICAL_ENTER(); //关中断
	do
	{
		res=f_unlink(str);				//删除原文件
		fat_err_times++;
	}while((fat_err_times<3)&&(res!=FR_OK));
	
	if(res!=FR_OK)
	{
		printf("\r\n删除 %s,失败",str); 
	}
	else
	{
		if(fat_err_times>1) printf("\r\ndelete %s,%d次成功",str,fat_err_times-1);
		else printf("\r\ndelete %s",str);
	}
	CPU_CRITICAL_EXIT();//开中断	
	return res;
}

int delet_ph(unsigned char *index)
{
	int res;
//	char str[40];
	sprintf(str, "0:%02d%02d%02d%02d%02d%02d_%d_%d.jpg",index[0],index[1],index[2],index[3],index[4],index[5],index[6],index[7]); 
	
	f_unlink_Twice(str);
		
	return res;
}

//FRESULT f_open_Twice(char *str,u8 rw_flag)
//{
//	int res;
//	uint8_t fat_err_times=0;
//	CPU_SR_ALLOC(); 
//	
//	CPU_CRITICAL_ENTER(); //关中断
//	do
//	{
//		if(rw_flag==1) res = f_open(&fp,str,FA_OPEN_ALWAYS|FA_READ|FA_WRITE); 				//写
//		else if(rw_flag==0) res = f_open(&fp,str,FA_OPEN_EXISTING|FA_READ);						//读
//		
//		fat_err_times++;
//	}while((fat_err_times<3)&&(res!=FR_OK));
//	
//	if(res!=FR_OK)
//	{
//		printf("\r\n打开 %s,失败",str); 
//	}
//	else
//	{
//		if(fat_err_times>1) printf("\r\n打开 %s,%d次成功",str,fat_err_times-1);
////		else printf("\r\n打开 %s",str);
//	}
//	CPU_CRITICAL_EXIT();//开中断	
//	return res;
//}

#if 1
int save_ph(unsigned char *index) //PH_GET_T[0] 开始
{
	int res,i,dlen;//reFR,
//	char str[40];
	int res0=0;
	CPU_SR_ALLOC(); 
	uint8_t fat_err_times=0;
	
	//191116155305_1_9: 2019年11月16日 15点53分05秒 通道1 预置位9  time + ch + preset	
	sprintf(str, "0:%02d%02d%02d%02d%02d%02d_%d_%d.jpg",index[0],index[1],index[2],index[3],index[4],index[5],index[6],index[7]); 		
	dlen=PH_LEN[0]+(PH_LEN[1]<<8)+(PH_LEN[2]<<16)+(PH_LEN[3]<<24);
	printf("\r\nsave photo,tt_len=%d,ph_name:%s",dlen,str);
	
//	if(sd_err>=10)
//	{
//		//格式化?
//		return -1;
//	}
	
	CPU_CRITICAL_ENTER(); //关中断
	
	__save_ph_star:
	
	res0=0;
	
	fat_err_times=0;
	do
	{
		res = f_open(&fp,str,FA_OPEN_ALWAYS|FA_READ|FA_WRITE); 				//写
		if(res!=FR_OK)
		{
			fat_err_times++;
			printf("\r\n打开SD照片失败,重新打开");
		}
		else
		{
			fat_err_times=0;
		}
	}while((fat_err_times<=3)&&(fat_err_times!=0));
	
	if(fat_err_times==0)
	{
		res=f_lseek(&fp,0);
		if(res==FR_OK)
		{
			for(i=0;i<dlen;i+=511)
			{
				if((i+511)>=dlen) res = f_write(&fp,&PH_DATA[i],dlen-i,&bw);
				else	res = f_write(&fp,&PH_DATA[i],511,&bw);
				if(res!=FR_OK)
				{
					res0 = -1;
					printf("\r\n写入失败");
					break;
				}
			}
		}
		else
		{
			res0 = -1;
			printf("\r\n移到文件开头失败");
		}
	}
	else
	{
		res0 = -1;
		printf("\r\n打开图片文件失败%s",str);		
	}

	
	do
	{
		res = f_close(&fp);	
		if((res!=FR_OK)&&(res0==0)) //前面没错误,关闭不了?
		{
			printf("\r\n关闭SD照片失败");
			fat_err_times++;
		}
		else
		{
			fat_err_times=0;
		}
	}while((fat_err_times<=3)&&(fat_err_times!=0));
	
	if(fat_err_times) res0=-1;
	
	if(res0 != 0)
	{
		sd_err++;
		if(sd_err>=3)
		{
			printf("\r\nSD卡多次失败,发\"FE\"格式化");
			sd_sta_h=0;
		}
		else
		{
			printf("\r\nSD操作失败,重新初始化SD");
			sd_reset();
			if(sd_sta_h==0)
			{
				printf("\r\nSD异常");
			}
			else
			{
				goto __save_ph_star;
			}
		}
	}
	else
	{
		sd_err=0;
	}
		

	CPU_CRITICAL_EXIT();//开中断	
	
	
	return res0;
}

#endif

int sent_ph_sd(uint8_t *index,uint8_t *data_in,int tt_len)
{
//	OS_ERR err;
	uint8_t fat_err_times=0;
	CPU_SR_ALLOC(); 
	int res;
	int res0=0;
	uint16_t pk_len,pk_nb,i,tmax;
//	char str[40];
	
	sprintf(str, "0:%02d%02d%02d%02d%02d%02d_%d_%d.jpg",index[0],index[1],index[2],index[3],index[4],index[5],index[6],index[7]); 
	
	if(data_in==NULL)
	{
		tmax=tt_pk_nb;
		printf("\r\nsend photo,pk_nb=%d,tt_len=%d,ph_name:%s",tt_pk_nb,tt_len,str);
	}
	else 
	{
		printf("\r\nresend:");
		tmax=data_in[2];
	}
//	if(tt_len>nw_pk_max) tt_len=nw_pk_max;
	tt_pk_nb=(tt_len/nw_pk_len);//1024一次
	if(tt_len%nw_pk_len) tt_pk_nb +=1;
	
	
	CPU_CRITICAL_ENTER(); //关中断
//	OSMutexPend (&sd_sem_lock, 0, 	OS_OPT_PEND_BLOCKING, 0, &err); 
	
//	sprintf(str, "0:PH1%04x.jpg",ph_sent_cnt); 
	
	

	fat_err_times=0;
	do
	{
//		res = f_open(&fp,str,FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
		res = f_open(&fp,str,FA_OPEN_EXISTING|FA_READ);
		if(res!=FR_OK)
		{
			printf("\r\n打开SD照片失败");
			fat_err_times++;
		}
		else
		{
			fat_err_times=0;
		}
	}while((fat_err_times<=3)&&(fat_err_times!=0));
	
	if(fat_err_times) res0=-1;
	else	//打开文件成功
	{
		fat_err_times=0;
		
//	for(i=0;i<data_in[2];i++) //data_in[2] 需补包的数量
//		{			
//			OSTimeDly( 20, OS_OPT_TIME_DLY, & err);
//			pk_nb=(data_in[2*i+3]<<8)+data_in[2*i+4]; //补包的 包值
//			if(pk_nb==tt_pk_nb) nw_sent_ph(PH_DATA+((pk_nb-1)*nw_pk_len),tt_len-((pk_nb-1)*nw_pk_len),*PH_GET_CH,*PH_GET_PS,pk_nb);  //结尾包,长度为 tt_len-((pk_nb-1)*nw_pk_len
//			else	nw_sent_ph(PH_DATA+((pk_nb-1)*nw_pk_len),nw_pk_len,*PH_GET_CH,*PH_GET_PS,pk_nb); //非结尾包 长度为 nw_pk_len
//			printf("%d ",pk_nb);
//		}		
		for(i=1;i<=tmax;i++) //临时测试
		{
			if(data_in==NULL) pk_nb=i;
			else 
			{
				pk_nb=(data_in[2*i+3-2]<<8)+data_in[2*i+4-2]; //补包的 包值
				udelay_no_inv(20000); //补包多延时  20000?
			}
			if(pk_nb==tt_pk_nb)
			{
				pk_len=tt_len % nw_pk_len;
				udelay_no_inv(20000); //最后一包多延时  20000?
			}			
			else pk_len=nw_pk_len;
			
			do
			{
				res=f_lseek(&fp,(pk_nb-1)*nw_pk_len);
				if(res!=FR_OK)
				{
					printf("\r\n读取SD照片失败0");
					fat_err_times++;
				}
				else
				{
					fat_err_times=0;
				}
			}while((fat_err_times<=3)&&(fat_err_times!=0));
			
			if(fat_err_times) 
			{
				res0=-1;
				break;
			}
			else
			{
				do
				{
					res = f_read(&fp,sd_read_buf,pk_len,&bw);
					if(res!=FR_OK)
					{
						printf("\r\n读取SD照片失败");
						fat_err_times++;
					}
					else
					{
						fat_err_times=0;
					}
				}while((fat_err_times<=3)&&(fat_err_times!=0));
				if(fat_err_times) 
				{
					res0=-1;
					break;
				}
				else
				{
//					udelay_no_inv(5000);
					nw_sent_ph(sd_read_buf,pk_len,index[6],index[7],pk_nb);//发包
					if(data_in!=NULL) printf("%d ",pk_nb);
				}
			}
		}
		fat_err_times=0; //打开照片成功，关闭照片有三次机会
	}
	
	do
	{
		res = f_close(&fp);	
		if(res!=FR_OK)
		{
			printf("\r\n关闭SD照片失败");
			fat_err_times++;
		}
		else
		{
			fat_err_times=0;
		}
	}while((fat_err_times<=3)&&(fat_err_times!=0));
	if(fat_err_times) res0=-1;
	
//	OSMutexPost (&sd_sem_lock, OS_OPT_POST_NONE, &err); 
	CPU_CRITICAL_EXIT();//开中断	
	
	return res0;
}

void HAL_Delay(uint32_t Delay)
{
	OS_ERR      err;
	OSTimeDly ( 2, OS_OPT_TIME_DLY, & err ); 
}


int read_write_sd(char *name,unsigned int addr,unsigned char *databuf,unsigned int len,uint8_t flag)//sent_ph_sd(uint8_t *index,uint8_t *data_in,int tt_len)
{
//	OS_ERR err;
	uint8_t fat_err_times=0;
	CPU_SR_ALLOC(); 
	int res;
	int res0=0;
//	uint16_t pk_len,pk_nb,i,tmax;
	
//	sprintf(name, "0:index_ll.txt"); 
	
//	printf("\r\n准备读写%d:%s",flag,name);
	
	if(strncmp(name,"0:",2)!=0)
	{
		printf("\r\n文件名错误");
		return -1;
	}
	if(strlen(name)>=40) 
	{
		printf("\r\n文件名超长");
		return -1;
	}
	if(flag>1)
	{
		printf("\r\nSD无效操作");
		return -1;
	}
	
	CPU_CRITICAL_ENTER(); //关中断

	__read_write_star:
	
	res0=0;
	
	fat_err_times=0;
	do
	{
		if(flag==1) res = f_open(&fp,name,FA_OPEN_ALWAYS|FA_READ|FA_WRITE); 				//写
		else if(flag==0) res = f_open(&fp,name,FA_OPEN_EXISTING|FA_READ);						//读

		if(res!=FR_OK)
		{
			printf("\r\n打开%s失败",name);
			fat_err_times++;
		}
		else
		{
			fat_err_times=0;
		}
	}while((fat_err_times<=3)&&(fat_err_times!=0));
	
	if(fat_err_times) res0=-1;
	else	//打开文件成功
	{
		fat_err_times=0;
		do
		{
			res=f_lseek(&fp,addr);
			if(res!=FR_OK)
			{
				printf("\r\n移位%s失败",name);
				fat_err_times++;
			}
			else
			{
				fat_err_times=0;
			}
		}while((fat_err_times<=3)&&(fat_err_times!=0));
		
		if(fat_err_times) 
		{
			res0=-1;
		}
		else
		{
			do
			{
				if(flag==1) res = f_write(&fp,databuf,len,&bw);
				else if(flag==0) res = f_read(&fp,databuf,len,&bw);
				if(res!=FR_OK)
				{
					printf("\r\n读写%s失败",name);
					fat_err_times++;
				}
				else
				{
//					printf("\r\n读写%s成功",name);
					fat_err_times=0;
				}
			}while((fat_err_times<=3)&&(fat_err_times!=0));
			if(fat_err_times) 
			{
				res0=-1;
			}
		}
		fat_err_times=0; //打开照片成功，关闭照片有三次机会
	}
	
	do
	{
		res = f_close(&fp);	
		if(res!=FR_OK)
		{
			printf("\r\n关闭%s失败",name);
			fat_err_times++;
		}
		else
		{
			fat_err_times=0;
		}
	}while((fat_err_times<=3)&&(fat_err_times!=0));
	if(fat_err_times) res0=-1;
	
	if(flag)	//写不进
	{
		if(res0 != 0)
		{
			sd_err++;
			if(sd_err>=3)
			{
				printf("\r\nSD卡多次失败,发\"FE\"格式化");
				sd_sta_h=0;
			}
			else
			{
				printf("\r\nSD操作失败,重新初始化SD");
				sd_reset();
				if(sd_sta_h==0)
				{
					printf("\r\nSD异常");
				}
				else
				{
					goto __read_write_star;
				}
			}
		}
		else
		{
			sd_err=0;
		}
	}
	else	//读不到直接丢失?
	{
		
	}
	
//	OSMutexPost (&sd_sem_lock, OS_OPT_POST_NONE, &err); 
	CPU_CRITICAL_EXIT();//开中断	
		
	return res0;
}


#if 0
void http_rev_data(char * in,uint16_t len)
{
	int res,i;
	
	
//	printf("收http\r\n%s\r\n",buf);
	if(strncmp("HTTP/1.1 401", (char *)in,12)==0)
	{
		get_nonce(in);
		r401=1;
		printf("收\r\n%s\r\n",in); //收到数据http
	}
	else	if(strncmp("HTTP/1.1 200 OK", (char *)in,12)==0) //认证完成
	{
		r200=1;
		cnt=2;
//		if(waiting_jpeg==1)
//		{
//			res=f_close_s();
//			printf("\r\n关闭文件 res=%d",res);
//			
//			waiting_jpeg=2;
//		}
//		else 
			if(waiting_jpeg==0)
		{
			res = f_open_s(++cnumb);
			printf("\r\n打开文件%d res=%d",cnumb,res);
			
			if(res==0)
			{
				res=f_lseek_s();
				printf("\r\n重写文件 res=%d",res);
				waiting_jpeg=1;
				rnumb=0;
			}
			else waiting_jpeg=2;
			
		}
		printf("收\r\n%s\r\n",in); //收到数据http
	}
	else if(waiting_jpeg==1)
	{
		if(len==0) 
		{
			waiting_jpeg=2;
			printf("\r\n写文件 结束,len=%d",len);
			res=f_close_s();
			printf("\r\n关闭文件 res=%d",res);
						
			if(res==0)	cmdcnt=30;//重新读数据
			else waiting_jpeg=2;
			
		}
		else
		{
			rnumb++;
			res = f_write_s(in,len);		
			printf("\r\n写文件 res=%d,len=%d,%d",res,len,rnumb);
			if(res !=0) waiting_jpeg=2;
		}
		
		printf("\r\n");
		for(i=0;i<10;i++)	printf("%x",in[i]); //收到数据http
		for(i=len-10;i<len;i++)	printf("%x",in[i]); //收到数据http
	}
	
	
}
#endif


