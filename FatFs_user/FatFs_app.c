
#include "main.h"
#include "fatfs.h"
#include <stdio.h>
#include <includes.h>

extern uint8_t retSD;    /* Return value for SD */
extern char SDPath[4];   /* SD logical drive path */
extern FATFS SDFatFS;    /* File system object for SD logical drive */
extern FIL SDFile;       /* File object for SD */



const char wData[]=" 新程序试一试！";
char rData2[100]="";
FIL fp; 
UINT bw;
UINT br;

void MX_FATFS_Init(void);
	
void HAL_Delay(uint32_t Delay)
{
	OS_ERR      err;
	OSTimeDly ( 2, OS_OPT_TIME_DLY, & err ); 
}

int f_open_s(int in)
{
	FRESULT res;
	char str[20];
	int reFR=0;
	
	sprintf(str, "0:sa%d.jpeg", in);
	res = f_open(&fp,str,FA_OPEN_ALWAYS|FA_READ|FA_WRITE);

	while(res!=FR_OK)
	{
		printf("重开%d,",reFR);
		res = f_open(&fp,str,FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
		if(++reFR>=10) break;
	}
		
	return res;
}

int f_write_s(char * in,uint32_t len)
{
	FRESULT res;
	int reFR=0;
	res = f_write(&fp,in,len,&bw);
	while(res!=FR_OK)
	{
		printf("重写%d,",reFR);
		res = f_write(&fp,in,len,&bw);
		if(++reFR>=10) break;
	}
	 MX_FATFS_Init();
//	reFR=0;
//	while(res!=FR_OK)
//	{
//		printf("重写%d,",reFR);
//		res = f_write(&fp,in,len,&bw);
//		if(reFR++>10) break;
//	}
	return res;
}


int f_close_s(void)
{
	FRESULT res;
	int reFR=0;
	res = f_close(&fp);
	while(res!=FR_OK)
	{
		printf("重关%d,",reFR);
		res = f_close(&fp);
		if(++reFR>=10) break;
	}
	return res;
}
int f_lseek_s(void)
{
	FRESULT res;
	int reFR=0;
	res=f_lseek(&fp,0);
	while(res!=FR_OK)
	{
		printf("重移%d,",reFR);
		res=f_lseek(&fp,0);
		if(++reFR>=10) break;
	}
	return res;
}



//NO spece in execution regions with .ANY selector matching

//void FatFsTask( void *pvParameters )
//{
//	OS_ERR      err;
//	FRESULT res;
//	
//	printf("\r\n\r\n这是一个FATFS文件系统移植实验");
//	
//	OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err ); 
//	
//	//挂载文件系统invalid
////	res = f_mount(&SDFatFS,SDPath,1);//"1:"
//	res=f_mount(&SDFatFS,"0:",1 );
//	
//	printf("\r\nf_mount res =%d",res);
//	
//#if 0		
//	if(res == FR_NO_FILESYSTEM)
//	{
//		res = f_mkfs(SDPath,0,0);//"1:"
//		printf("\r\nf_mkfs res =%d",res);
//		//格式化后要取消挂载再重新挂载文件系统
//		res = f_mount(NULL,"1:",1);
//		res = f_mount(&fsObject,"1:",1);
//		
//		printf("\r\nsecond f_mount res =%d",res);
//	}
//#endif
//	
//#if 1	
//	res = f_open(&fp,"0:s0.jpeg",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
//////											中文文件名abcdefgadfasd.txt
//	printf("\r\nf_open res =%d",res);
//	
////	res=f_mkdir("0:/s3");
////	printf("\r\nf_mkdir res =%d",res);
//	
//	if(res == FR_OK)
//	{
//		printf("\r\n打开文件OK");
//		res = f_write(&fp,wData,sizeof(wData),&bw);
//		printf ("\r\nbw= %d",bw);		

//		if(res == FR_OK)
//		{
////			f_lseek(&fp,0);
//			res = f_read (&fp,rData2,f_size(&fp),&br);
//			if(res == FR_OK)
//				printf ("\r\n文件内容：%s br= %d",rData2,br);		
//		}	
//		
//		OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err ); 
//		printf("\r\n准备关闭");
//		f_close(&fp);
//		printf("\r\n关闭fp");
//	}
//	
//	while(1)
//	{
////		vTaskDelay(  pdMS_TO_TICKS(1000) ); 
//		OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err ); 
//	}
//	#endif
//}


//#define  FatFsTask_SIZE                     500
//#define  FatFsTask_PRIO                         10
//OS_TCB   FatFsTask_TCB;															//任务控制块
//CPU_STK  FatFsTask_Stk [ FatFsTask_SIZE ];	//任务堆栈
//void AAA (void)
//{
//	FRESULT res;
//	res=f_mount(&SDFatFS,"0:",1 );
//}

void FatFsTaskStart (void *p_arg)
{
//	FRESULT res;
	res=f_mount(&SDFatFS,"0:",1 );
	
//	return;
//	
//	OS_ERR      err;
//	/* 创建 任务 */
//    OSTaskCreate((OS_TCB     *)&FatFsTask_TCB,                             //任务控制块地址
//                 (CPU_CHAR   *)"FatFsTask",                             //任务名称
//                 (OS_TASK_PTR ) FatFsTask,                                //任务函数
//                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
//                 (OS_PRIO     )	FatFsTask_PRIO,                         //任务的优先级
//                 (CPU_STK    *)&FatFsTask_Stk[0],                          //任务堆栈的基地址
//                 (CPU_STK_SIZE) FatFsTask_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
//                 (CPU_STK_SIZE) FatFsTask_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
//                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
//                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
//                 (void       *) 0,                                          //任务扩展（0表不扩展）
//                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
//                 (OS_ERR     *)&err);                                       //返回错误类型
}

//void http_rev_data(char * in,uint16_t len)
//{
//	int res,i;
//	
//	
////	printf("收http\r\n%s\r\n",buf);
//	if(strncmp("HTTP/1.1 401", (char *)in,12)==0)
//	{
//		get_nonce(in);
//		r401=1;
//		printf("收\r\n%s\r\n",in); //收到数据http
//	}
//	else	if(strncmp("HTTP/1.1 200 OK", (char *)in,12)==0) //认证完成
//	{
//		r200=1;
//		cnt=2;
////		if(waiting_jpeg==1)
////		{
////			res=f_close_s();
////			printf("\r\n关闭文件 res=%d",res);
////			
////			waiting_jpeg=2;
////		}
////		else 
//			if(waiting_jpeg==0)
//		{
//			res = f_open_s(++cnumb);
//			printf("\r\n打开文件%d res=%d",cnumb,res);
//			
//			if(res==0)
//			{
//				res=f_lseek_s();
//				printf("\r\n重写文件 res=%d",res);
//				waiting_jpeg=1;
//				rnumb=0;
//			}
//			else waiting_jpeg=2;
//			
//		}
//		printf("收\r\n%s\r\n",in); //收到数据http
//	}
//	else if(waiting_jpeg==1)
//	{
//		if(len==0) 
//		{
//			waiting_jpeg=2;
//			printf("\r\n写文件 结束,len=%d",len);
//			res=f_close_s();
//			printf("\r\n关闭文件 res=%d",res);
//						
//			if(res==0)	cmdcnt=30;//重新读数据
//			else waiting_jpeg=2;
//			
//		}
//		else
//		{
//			rnumb++;
//			res = f_write_s(in,len);		
//			printf("\r\n写文件 res=%d,len=%d,%d",res,len,rnumb);
//			if(res !=0) waiting_jpeg=2;
//		}
//		
//		printf("\r\n");
//		for(i=0;i<10;i++)	printf("%x",in[i]); //收到数据http
//		for(i=len-10;i<len;i++)	printf("%x",in[i]); //收到数据http
//	}
//	
//	
//}
