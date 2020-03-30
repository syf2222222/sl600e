
#include "main.h"
#include "fatfs.h"
#include <stdio.h>
#include <includes.h>

extern uint8_t retSD;    /* Return value for SD */
extern char SDPath[4];   /* SD logical drive path */
extern FATFS SDFatFS;    /* File system object for SD logical drive */
extern FIL SDFile;       /* File object for SD */



const char wData[]=" �³�����һ�ԣ�";
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
		printf("�ؿ�%d,",reFR);
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
		printf("��д%d,",reFR);
		res = f_write(&fp,in,len,&bw);
		if(++reFR>=10) break;
	}
	 MX_FATFS_Init();
//	reFR=0;
//	while(res!=FR_OK)
//	{
//		printf("��д%d,",reFR);
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
		printf("�ع�%d,",reFR);
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
		printf("����%d,",reFR);
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
//	printf("\r\n\r\n����һ��FATFS�ļ�ϵͳ��ֲʵ��");
//	
//	OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err ); 
//	
//	//�����ļ�ϵͳinvalid
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
//		//��ʽ����Ҫȡ�����������¹����ļ�ϵͳ
//		res = f_mount(NULL,"1:",1);
//		res = f_mount(&fsObject,"1:",1);
//		
//		printf("\r\nsecond f_mount res =%d",res);
//	}
//#endif
//	
//#if 1	
//	res = f_open(&fp,"0:s0.jpeg",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
//////											�����ļ���abcdefgadfasd.txt
//	printf("\r\nf_open res =%d",res);
//	
////	res=f_mkdir("0:/s3");
////	printf("\r\nf_mkdir res =%d",res);
//	
//	if(res == FR_OK)
//	{
//		printf("\r\n���ļ�OK");
//		res = f_write(&fp,wData,sizeof(wData),&bw);
//		printf ("\r\nbw= %d",bw);		

//		if(res == FR_OK)
//		{
////			f_lseek(&fp,0);
//			res = f_read (&fp,rData2,f_size(&fp),&br);
//			if(res == FR_OK)
//				printf ("\r\n�ļ����ݣ�%s br= %d",rData2,br);		
//		}	
//		
//		OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err ); 
//		printf("\r\n׼���ر�");
//		f_close(&fp);
//		printf("\r\n�ر�fp");
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
//OS_TCB   FatFsTask_TCB;															//������ƿ�
//CPU_STK  FatFsTask_Stk [ FatFsTask_SIZE ];	//�����ջ
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
//	/* ���� ���� */
//    OSTaskCreate((OS_TCB     *)&FatFsTask_TCB,                             //������ƿ��ַ
//                 (CPU_CHAR   *)"FatFsTask",                             //��������
//                 (OS_TASK_PTR ) FatFsTask,                                //������
//                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
//                 (OS_PRIO     )	FatFsTask_PRIO,                         //��������ȼ�
//                 (CPU_STK    *)&FatFsTask_Stk[0],                          //�����ջ�Ļ���ַ
//                 (CPU_STK_SIZE) FatFsTask_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
//                 (CPU_STK_SIZE) FatFsTask_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
//                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
//                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
//                 (void       *) 0,                                          //������չ��0����չ��
//                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
//                 (OS_ERR     *)&err);                                       //���ش�������
}

//void http_rev_data(char * in,uint16_t len)
//{
//	int res,i;
//	
//	
////	printf("��http\r\n%s\r\n",buf);
//	if(strncmp("HTTP/1.1 401", (char *)in,12)==0)
//	{
//		get_nonce(in);
//		r401=1;
//		printf("��\r\n%s\r\n",in); //�յ�����http
//	}
//	else	if(strncmp("HTTP/1.1 200 OK", (char *)in,12)==0) //��֤���
//	{
//		r200=1;
//		cnt=2;
////		if(waiting_jpeg==1)
////		{
////			res=f_close_s();
////			printf("\r\n�ر��ļ� res=%d",res);
////			
////			waiting_jpeg=2;
////		}
////		else 
//			if(waiting_jpeg==0)
//		{
//			res = f_open_s(++cnumb);
//			printf("\r\n���ļ�%d res=%d",cnumb,res);
//			
//			if(res==0)
//			{
//				res=f_lseek_s();
//				printf("\r\n��д�ļ� res=%d",res);
//				waiting_jpeg=1;
//				rnumb=0;
//			}
//			else waiting_jpeg=2;
//			
//		}
//		printf("��\r\n%s\r\n",in); //�յ�����http
//	}
//	else if(waiting_jpeg==1)
//	{
//		if(len==0) 
//		{
//			waiting_jpeg=2;
//			printf("\r\nд�ļ� ����,len=%d",len);
//			res=f_close_s();
//			printf("\r\n�ر��ļ� res=%d",res);
//						
//			if(res==0)	cmdcnt=30;//���¶�����
//			else waiting_jpeg=2;
//			
//		}
//		else
//		{
//			rnumb++;
//			res = f_write_s(in,len);		
//			printf("\r\nд�ļ� res=%d,len=%d,%d",res,len,rnumb);
//			if(res !=0) waiting_jpeg=2;
//		}
//		
//		printf("\r\n");
//		for(i=0;i<10;i++)	printf("%x",in[i]); //�յ�����http
//		for(i=len-10;i<len;i++)	printf("%x",in[i]); //�յ�����http
//	}
//	
//	
//}
