
#include "bsp.h"
//#include <stdio.h>
//#include <string.h>
//unsigned char start_and_device_number[7]={0x68,0x43,0x43,0x30,0x31,0x39,0x36};

#define	Contact								0x00	//	����������Ϣ	װ�ÿ�������������Ϣ
#define CheckT								0x01	//	Уʱ	�������������·�������
#define SetPassword 					0x02	//	����װ������	װ�ó������룺�ַ�����1234����31H32H33H34H��
#define Pcfg 									0x03	//	��վ�·���������	��ָ��Ҫ�����ݲɼ�װ�ý��յ��������ԭ�����
#define HeartBeat 						0x05	//	װ��������Ϣ	������վ���װ������ʱ�䡢IP��ַ�Ͷ˿ںš������ź�ǿ�ȼ����ص�ѹ
#define chang_ip	 						0x06	//	������վIP��ַ���˿ںźͿ���	
#define check_ip 							0x07	//	��ѯ��վIP��ַ���˿ںźͿ���	
#define ResetDev	 						0x08	//	װ�ø�λ	��վ��װ�ý��и�λ
#define waikup_device_m				0x09	//	���Ż���	��վ�Զ��ŷ�ʽ��������״̬��װ��
#define check_device_cfg 			0x0A	//	��ѯװ�����ò���	
#define device_function_cfg		0x0B	//	װ�ù�������	
#define device_Sleep 					0x0C	//	װ������	
#define check_device_time 		0x0D	//	��ѯװ���豸ʱ��	
#define sent_msg					 		0x0E	//	����ȷ�϶���
#define request_data 					0x21	//	��վ����װ������	��վ����ɼ�װ�òɼ����ݲ���������
#define request_la_qj 				0x22	//	�ϴ��������������������	
#define request_qx 						0x25	//	�ϴ���������	

/*
#define  0x26 //	�ϴ������¶ȡ����ߵ�������	
#define  0x27 //	�ϴ�����������	
#define  0x29 //	�ϴ��趯���Ƶ������	
#define  0x2A //	�ϴ�������б����	
#define  0x2B //	�ϴ�����΢��������	
#define  0x2C //	�ϴ��ۺϷ�������	
#define  0x2D //	����ɽ�𱨾�����	
#define  0x2E //	���ʹ���趯��������	
#define  0x30 //	�ϴ��豸������Ϣ	
#define  0x31 //	��վ����΢���񶯶�̬����	
#define  0x32 //	΢���񶯶�̬��������	
#define  0x33 //	΢���񶯶�̬�������ͽ������	
#define  0x34 //	΢���񶯶�̬���ݲ����·�	
#define  0x35 //	��վ�����趯��̬����	
#define  0x36 //	�趯��̬��������	
#define  0x37 //	�趯��̬�������ͽ������	
#define  0x38 //	�趯��̬���ݲ����·�	
#define  0x39 //	��վ����������ƫ�Ƕ�̬����	
#define  0x3A //	������ƫ�Ƕ�̬��������	
#define  0x3B //	������ƫ�Ƕ�̬�������ͽ������	
#define  0x3C //	������ƫ�Ƕ�̬���ݲ����·�	
#define  0x41 //	�ϴ��ۻ�����	
#define  0x42 //	�ϴ����߻�������	
#define  0x43 //	�ϴ������¶�����	
#define  0x44 //	�ϴ����»���ӵص�������	
#define  0x45 //	�ϴ����϶�λ����	
#define  0x46 //	�ϴ����¾ַ�����	
#define  0x47 //	�ϴ����¾ַ���ͼ����	
#define  0x60 //	��վ���ù��ϲ���ն˲���	
#define  0x6A //	��վ��ѯ���ϲ���ն˲���	
#define  0x61 //	�ϴ����ϲ���ն˹�������	
#define  0x62 //	�ն�װ������վ�����ϴ���Ƶ���ϲ�������	
#define  0x63 //	�ϴ���Ƶ���ϲ�������	
#define  0x64 //	��Ƶ���ϲ��������ϴ�������־	
#define  0x65 //	��վ���ն˷��͹�Ƶ���ϲ������ݲ���	
#define  0x66 //	�ն�װ������վ�����ϴ������в���������	
#define  0x67 //	�ϴ������в���������	
#define  0x68 //	�����в����������ϴ�������־	
#define  0x69 //	��վ���ն˷����в��������ݲ���	

//#define  0x����Ϊ�ļ����䲿��
71 //	��վ��ѯװ���ļ��б�	
72 //	��վ����װ�������ļ�	
73 //	װ�����������ļ�	
74 //	�ļ�����	
75 //	�ļ����ͽ������	
76 //	�ļ����������·�	
����Ϊͼ����Ƶ��ⲿ��
81 //	ͼ��ɼ���������	��վ�·��ɼ�װ�õ�ɫ��ѡ��ͼ���ߣ����ȣ��Աȶȣ����Ͷȵ���Ϣ
82 //	����ʱ�������	���òɼ�װ���Զ�������Ƭ��ʱ�䡢λ��
83 //	��վ����������Ƭ	�����ֶ�����������Ƭʱʹ��
84 //	�ɼ�װ������������Ƭ	�ɼ�װ�òɼ���ͼ�����ݺ�����վ��������ͼ������������վԭ�����
85 //	ͼ����������	����ͼ���ֺ��N�����ݰ�
86 //	ͼ���������ͽ������	������վ�ж�ͼ�������Ƿ��������
87 //	���������·�	��վ�ӵ�86Hָ��󣬽�������������δ�յ��İ����·����ɼ�װ��
88 //	�����Զ�̵���	�ϡ��¡����ҡ�Զ�������ڣ�Ԥ��λԶ�����á�����
89 //	����������Ƶ����	
8A //	��ֹ������Ƶ����	
8B //	��ѯ����ʱ���	
8C //	��Ƶ�ɼ���������	
8D //	��Ƶ�ɼ�������ѯ	
8E //	OSD��������	
8F //	OSD������ѯ	
90 //	¼����Բ�������	
91 //	¼����Բ�����ѯ	
92 //	ͨ��¼��״̬��ѯ	
93 //	��վ�����������Ƶ	
94 //	�ɼ��ն��������Ͷ���Ƶ	
95 //	����Ƶ��������	
96 //	����Ƶ�������ͽ������	
97 //	����Ƶ���������·�	
98 //	��վ��ѯ�ն�¼���ļ���Ŀ	
99 //	��վ��ѯ�ն�¼���ļ��б�	
9A //	��վ�������¼���ļ��ط�	
9B //	��վ�������¼���ļ��طſ���	
9C //	��վ�������¼���ļ��طŶϿ�	
9D //	��վ�������¼������	
9E //	��վ�������¼���ļ����ضϿ�	
A0 //	����������Ƶ����	
A1 //	��ֹ������Ƶ����	
A2 //	��վ�������ն˽��������㲥	
A3 //	��վ�������ն˶Ͽ������㲥	
A4 //	����������������	
A5 //	��������������ѯ	
A6 //	��·����������	
A7 //	��·��������ѯ	
A8 //	��·Ѳ���������	
A9 //	��·Ѳ�������ѯ	
AA //	���ܷ����澯�ϱ�	
AB //	������������	
AC //	����������ѯ	
AD //	������������Ƶ���Ӳ�������	
AE //	������������Ƶ���Ӳ�����ѯ	
B1 //	�����3D�������	
*/



//#define  AppTaskNWR_SIZE                     512
//#define  AppTaskNWR_PRIO                         9
//OS_TCB   AppTaskNWR_TCB;															//������ƿ�
//CPU_STK  AppTaskNWR_Stk [ AppTaskNWR_SIZE ];	//�����ջ
//void  AppTaskNWR  ( void * p_arg );
//void  sent_to_nw(unsigned char command,unsigned char * t_data,unsigned int dlong);
//struct nw_ctl
//{
//	unsigned char cmd;
//	unsigned int len;
//	unsigned char *data;   
//};

void  nw_sent(struct nw_ctl *t_data);
void rev_err(unsigned char ctl,unsigned char data);

#define  AppTaskNWT_SIZE                     512
#define  AppTaskNWT_PRIO                         9
OS_TCB   AppTaskNWT_TCB;															//������ƿ�
CPU_STK  AppTaskNWT_Stk [ AppTaskNWT_SIZE ];	//�����ջ
void  AppTaskNWT  ( void * p_arg );

#define  NWTask_Contact_SIZE                     512
#define  NWTask_Contact_PRIO                         9
OS_TCB   NWTask_Contact_TCB;															//������ƿ�
CPU_STK  NWTask_Contact_Stk [ NWTask_Contact_SIZE ];	//�����ջ
void  NWTask_Contact  ( void * p_arg );

#define  NWTask_CheckT_SIZE                     512
#define  NWTask_CheckT_PRIO                         9
OS_TCB   NWTask_CheckT_TCB;															//������ƿ�
CPU_STK  NWTask_CheckT_Stk [ NWTask_CheckT_SIZE ];	//�����ջ
void  NWTask_CheckT  ( void * p_arg );

#define  NWTask_HeartBeat_SIZE                     512
#define  NWTask_HeartBeat_PRIO                         9
OS_TCB   NWTask_HeartBeat_TCB;															//������ƿ�
CPU_STK  NWTask_HeartBeat_Stk [ NWTask_HeartBeat_SIZE ];	//�����ջ
void  NWTask_HeartBeat  ( void * p_arg );

#define  NWTask_Contact_SIZE                     512
#define  NWTask_Contact_PRIO                         9
OS_TCB   NWTask_Contact_TCB;															//������ƿ�
CPU_STK  NWTask_Contact_Stk [ NWTask_Contact_SIZE ];	//�����ջ
void  NWTask_Contact  ( void * p_arg );


OS_MUTEX RX_4G;                         //���������ź���
#define Pend_4G_Mutex()		OSMutexPend ((OS_MUTEX  *)&RX_4G, (OS_TICK )0, (OS_OPT)OS_OPT_PEND_BLOCKING, (CPU_TS *)0, (OS_ERR *)&err)      
//		OSMutexPend ((OS_MUTEX  *)&RX_4G,        //���뻥���ź��� mutex
//								 (OS_TICK    )0,                       //�����޵ȴ�
//								 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //������벻���Ͷ�������
//								 (CPU_TS    *)0,                       //������ʱ���
//								 (OS_ERR    *)&err);                   //���ش�������		

#define Post_4G_Mutex()		OSMutexPost ((OS_MUTEX  *)&RX_4G, (OS_OPT )OS_OPT_POST_NONE,(OS_ERR *)&err) 
//OSMutexPost ((OS_MUTEX  *)&RX_4G,                 //�ͷŻ����ź��� mutex
//								 (OS_OPT     )OS_OPT_POST_NONE,       //�����������
//								 (OS_ERR    *)&err);                  //���ش�������	
								 

unsigned char command_4g=0xff;
unsigned int 	dlong_4g;
//unsigned char rdata_4g[100];

unsigned char version[2]={3,0};
//unsigned char systime[6]={0,0,0,0,0,0}; //�� �� �� ʱ �� ��
unsigned char ContactOK=0,CheckTOK=0;



OS_Q NW_REV_Msg;				//����һ����Ϣ���У����ڰ�����Ϣ���ݣ�ģ����Ϣ����

struct nw_ctl nw_r,nw_s;
unsigned char nw_r_data[100];  

//	����	4�ֽ�
//	�������	1�ֽ�
//	�ɼ����	2�ֽ�
//	����ʱ��	2�ֽ�
//	����ʱ��	2�ֽ�
//	Ӳ������ʱ���	3�ֽ�
//	������֤��	4�ֽ�
unsigned char Password[4]={'1','2','3','4'};//
unsigned char Heabtime=60;//���������װ��������Ϣ���ͼ������λ���ӣ���������ӦΪ1����;
unsigned short getdatetime=20;//�ɼ��������ÿ�����ٷ��Ӳ���һ�Σ��ɼ����������ʱ���޹أ�����λ���ӣ���������ӦΪ20���ӣ�
unsigned short sleeptime=0;//����ʱ�������ݲɼ����ܹرջ�ͨ���豸����ʱ�䣬��ʱ���ڿ�֧�ֶ��Ż����绽�ѣ���λ���ӣ���Ϊ0��װ�ò����ߣ�
unsigned short onlinetime=10;		//����ʱ����ͨ���豸�������ݲɼ�������ͨ���豸����ʱ�䣻��λ���ӣ�
unsigned char resettime[3];	//3bye Ӳ������ʱ��㣺Ϊ��֤װ������ɿ�����װ��Ӧ֧�ֶ�ʱ������
unsigned short checkword[4]={'1','2','3','4'};	//������֤��4�ֽڣ�װ�ó�ʼΪ�ַ���1234����31H32H33H34H����Ϊȷ��װ�����ݵ���ȷ�ԣ���ֹ�Ƿ��û�������ƭ��������
//���������ڷ�ֹ�Ƿ�װ���û������ݱ���վ�Ͽɣ���װʱװ���趨Ĭ�����ģ�������װ��ɺ�
//��վ�·�ָ���޸ĸ�װ�����ģ���װ����վ��¼������һ��ʱ�Ӹ����ݺϷ���Ч���������Ρ�




//char localpassw[50]="1234"

//void  NWTask_Contact  ( void * p_arg )
//{
//	OS_ERR      err;
//	CPU_SR_ALLOC();
//		
////	unsigned char t_data[2]={3,0};
////	unsigned int dlong; 
//	unsigned char mode=0;
//	
//	(void)p_arg;
////command_4g=uart3_t[7];
////dlong_4g=dlong;
////for(i=0;i<dlong;i++) rdata_4g[i]=uart3_t[i+10];				
//	while (DEF_TRUE)
//	{		
//		if(mode==0)
//		{
//			OS_CRITICAL_ENTER();                              //�����ٽ�Σ����⴮�ڴ�ӡ�����
////				sent_to_nw(Contact,version,sizeof(version));
//			OS_CRITICAL_EXIT();                               //�˳��ٽ��
//			
//			OSTaskSemPend ((OS_TICK   )6000,                     //�ȴ�6s
//										 (OS_OPT    )OS_OPT_PEND_BLOCKING,  //����ź��������þ͵ȴ�
//										 (CPU_TS   *)0,                   	//��ȡ�ź�����������ʱ���
//										 (OS_ERR   *)&err);                 //���ش�������	

////	OSTaskCreate(	(OS_TCB 	* )&StartTaskTCB,		//������ƿ�
////				 				(CPU_CHAR	* )"start task", 		//��������
////								(OS_TASK_PTR )start_task, 			//������
////								(void		* )0,					//���ݸ��������Ĳ���
////								(OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
////								(CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
////								(CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
////								(CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
////								(OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
////								(OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
////								(void   	* )0,					//�û�����Ĵ洢��
////								(OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
////								(OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ

//			if(err==OS_ERR_NONE)
//			{
//				Pend_4G_Mutex();

//				if((command_4g==Contact)&&(dlong_4g==sizeof(version)))
//				{
//					mode=1;
//					ContactOK=1;
//					OSTaskSemPost((OS_TCB *)&NWTask_CheckT_TCB,	(OS_OPT)OS_OPT_POST_NONE,(OS_ERR *)&err);// ��ʼУʱ
//				}
//				Post_4G_Mutex();
//			}
//		}
//		else
//		{
//			RUN_LED1_Toggle();
//			
//			OSTaskSemPend ((OS_TICK   )0,                     //�����޵ȴ�
//										 (OS_OPT    )OS_OPT_PEND_BLOCKING,  //����ź��������þ͵ȴ�
//										 (CPU_TS   *)0,                   	//��ȡ�ź�����������ʱ���
//										 (OS_ERR   *)&err);                 //���ش�������	
//			
//			Pend_4G_Mutex();

//			if((command_4g==Contact)&&(dlong_4g==0))
//			{
//				mode=0;
//			}
//			Post_4G_Mutex();
//		}
//	}
//}

unsigned int retxtime=0; //��ʼֵ60���ӣ����԰汾6s
unsigned int ctlnumber=0;
unsigned char sentbuf[50],signal=63,volt=121;


void change_IP(void)
{
}
void reset_STM32(void)
{
}
void wakeup_dev(void)
{
}
void dev_sleep(void)
{
}


unsigned char nw_ip[4];
unsigned short  nw_prot;
unsigned char  nw_phone_num[6];
unsigned char function_buf[20];
void check_function(void)
{
	char i;
		for(i=0;i<strlen((char *)function_buf);i++)
		{
			if(function_buf[i]==0x22)//	��������������Ǽ�⹦��
			{
			}
			if(function_buf[i]==0x24)//	��Ե��й©������⹦��
			{
			}
			if(function_buf[i]==0x25)//	�������ݼ�⹦��
			{
			}
			if(function_buf[i]==0x26)//	�����¶ȡ��������ݼ�⹦��
			{
			}
			if(function_buf[i]==0x27)//	���������ݼ�⹦��
			{
			}
			if(function_buf[i]==0x28)//	���߲���Ǽ�⹦��
			{
			}
			if(function_buf[i]==0x29)//	�趯���Ƶ�ʼ�⹦��
			{
			}
			if(function_buf[i]==0x2A)//	������б���ݼ�⹦��
			{
			}
			if(function_buf[i]==0x2B)//	����΢�������ݼ�⹦��
			{
			}
			if(function_buf[i]==0x2C)//	�ۺϷ�������
			{
			}
			if(function_buf[i]==0x2D)//	ɽ�𱨾�����
			{
			}
			if(function_buf[i]==0x2E)//	����趯��������
			{
			}
			if(function_buf[i]==0x30)//	�豸�����Լ칦��
			{
			}
			if(function_buf[i]==0x32)//	΢���񶯶�̬���ݼ�⹦��
			{
			}
			if(function_buf[i]==0x36)//	�趯��̬���ݼ�⹦��
			{
			}
			if(function_buf[i]==0x41)//	�ۻ����ݼ�⹦��
			{
			}
			if(function_buf[i]==0x42)//	���߻������ݼ�⹦��
			{
			}
			if(function_buf[i]==0x43)//	�����¶����ݼ�⹦��
			{
			}
			if(function_buf[i]==0x44)//	���»���ӵػ������ݼ�⹦��
			{
			}
			if(function_buf[i]==0x45)//	���¹��϶�λ���ݼ�⹦��
			{
			}
			if(function_buf[i]==0x46)//	���¹��Ͼַ����ݼ�⹦��
			{
			}
			if(function_buf[i]==0x47)//	���¾ַ���ͼ���ݴ��书��
			{
			}
			if(function_buf[i]==0x73)//	�ļ����书��
			{
			}
			if(function_buf[i]==0x84)//	ͼ���⹦��
			{
			}
			if(function_buf[i]==0x85)//	��Ƶ��⹦��
			{
			}
		}
}




void  NWTask_Contact  ( void * p_arg )//void  NWTask_task( void * p_arg )
{
	OS_ERR      err;
	CPU_SR_ALLOC();
	unsigned char mode=0;
	struct nw_ctl *nw_r_post;
	OS_MSG_SIZE*	 size;	
	unsigned int sentTime,i;
	
	(void)p_arg;	
	
//		nw_r_post=OSQPend((OS_Q*			)&NW_REV_Msg,   
//											(OS_TICK		)retxtime,										
//											(OS_OPT			)OS_OPT_PEND_BLOCKING,
//											(OS_MSG_SIZE*	)&size,		
//											(CPU_TS*		)0,
//											(OS_ERR*		)&err);
//	
//	OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err ); 
	
//	nw_s.len=sizeof(version);
//					nw_s.data=version;
//					nw_sent(&nw_s);
	
	while (DEF_TRUE)
	{				
		nw_r_post=OSQPend((OS_Q*			)&NW_REV_Msg,   
											(OS_TICK		)retxtime,										
											(OS_OPT			)OS_OPT_PEND_BLOCKING,
											(OS_MSG_SIZE*	)&size,		
											(CPU_TS*		)0,
											(OS_ERR*		)&err);
		
//		printf("�ȵ���Ϣ");
		if(err==OS_ERR_NONE)
		{
			printf("\r\n�ȵ���Ϣ");
			
			printf("\r\nm-s %x,%d:%d:%d",nw_r_post->cmd,systime[3],systime[4],systime[5]);
			
			switch(nw_r_post->cmd)
			{
				case 	0xff:			//4g post
					nw_s.cmd=0;
					nw_s.len=sizeof(version);
					nw_s.data=version;
					nw_sent(&nw_s);
					retxtime=6000; 	//�������磬1�����ط�
					break;
					
				case 	Contact:			//00	//	����������Ϣ	װ�ÿ�������������Ϣ
					printf("\r\nCont len=:%d",nw_r_post->len);
				
					if((ctlnumber==Contact)&&(nw_r_post->len==2))//sizeof(version) ���ڿ�������ʱ���������ظ�����������Ϣ
					{
						if((nw_r_post->data[0]==nw_s.data[0])&&(nw_r_post->data[1]==nw_s.data[1]))
						{
							ctlnumber=CheckT;
							nw_s.cmd=ctlnumber;
							nw_s.len=sizeof(systime);
							nw_s.data=systime;
							nw_sent(&nw_s);
							retxtime=6000;		//Уʱ��2�����ط�
							sentTime=worktime;	
						}
					}
					else if(nw_r_post->len==0) //
					{
//						ctlnumber=Contact; //������Ϊ ����̨���������������ź�
//						mode=1;
					}
					
					retxtime=0; 	// ��һ�ε�ʱ��
					break;
					
				case  CheckT:				//01	//	Уʱ	�������������·�������
//					if(memcmp(sentTime,systime,5)==0)
					if(ctlnumber==Contact)
					{
						if((worktime-sentTime)<20)
						{
							memcpy(systime,nw_r_post->data,6);
							ctlnumber=HeartBeat;
						}
					}
					else
					{
						memcpy(systime,nw_r_post->data,6);
						nw_sent(nw_r_post);
//					ctlnumber=Contact;
//					mode=1;
					}
					retxtime=0;
									
					break;
				case  SetPassword :	//02	//	����װ������	װ�ó������룺�ַ�����1234����31H32H33H34H��
					if(memcmp(PSW,nw_r_post->data,4)==0) //localpassw[50]
					{
						memcpy(PSW,&nw_r_post->data[4],6);
						nw_sent(nw_r_post);
					}
					else
					{
						rev_err(SetPassword,0xff);
					}
					break;
					
//unsigned char Password[4]={'1','2','3','4'};//
//unsigned char Heabtime=60;//���������װ��������Ϣ���ͼ������λ���ӣ���������ӦΪ1����;
//unsigned short getdatetime=20;//�ɼ��������ÿ�����ٷ��Ӳ���һ�Σ��ɼ����������ʱ���޹أ�����λ���ӣ���������ӦΪ20���ӣ�
//unsigned short sleeptime=0;//����ʱ�������ݲɼ����ܹرջ�ͨ���豸����ʱ�䣬��ʱ���ڿ�֧�ֶ��Ż����绽�ѣ���λ���ӣ���Ϊ0��װ�ò����ߣ�
//unsigned short onlinetime=10;		//����ʱ����ͨ���豸�������ݲɼ�������ͨ���豸����ʱ�䣻��λ���ӣ�
//unsigned int resettime;	//3bye  Ӳ������ʱ��㣺Ϊ��֤װ������ɿ�����װ��Ӧ֧�ֶ�ʱ������
//unsigned short checkword[4]={'1','2','3','4'};	//������֤��4�ֽڣ�װ�ó�ʼΪ�ַ���1234����31H32H33H34H����Ϊȷ��װ�����ݵ���ȷ�ԣ���ֹ�Ƿ��û�������ƭ��������
////���������ڷ�ֹ�Ƿ�װ���û������ݱ���վ�Ͽɣ���װʱװ���趨Ĭ�����ģ�������װ��ɺ�
////��վ�·�ָ���޸ĸ�װ�����ģ���װ����վ��¼������һ��ʱ�Ӹ����ݺϷ���Ч���������Ρ�
				case  Pcfg :				//03	//	��վ�·���������	��ָ��Ҫ�����ݲɼ�װ�ý��յ��������ԭ�����
					if((memcmp(PSW,nw_r_post->data,4)==0)&&(memcmp(checkword,&nw_r_post->data[14],4)==0)) //localpassw[50]
					{
						
						Heabtime=nw_r_post->data[4];
						getdatetime=(nw_r_post->data[5]<<8)+nw_r_post->data[6];
						sleeptime=(nw_r_post->data[7]<<8)+nw_r_post->data[8];
						onlinetime=(nw_r_post->data[9]<<8)+nw_r_post->data[10];
						resettime[0]=nw_r_post->data[11]; //�� 0-28
						resettime[1]=nw_r_post->data[12];	//ʱ 0-24
						resettime[2]=nw_r_post->data[13];	//�� 0-60
						
						nw_sent(nw_r_post);
					}
					else
					{
						rev_err(Pcfg,0xff);
					}
					break;
				case  HeartBeat:		//05	//	װ��������Ϣ	������վ���װ������ʱ�䡢IP��ַ�Ͷ˿ںš������ź�ǿ�ȼ����ص�ѹ
					break;
				case  chang_ip:			//06	//	������վIP��ַ���˿ںźͿ���	
					if(memcmp(PSW,nw_r_post->data,4)==0)
					{
//����	��վIP	�˿ں�	��վIP	�˿ں�	��վ����	��վ����
//4�ֽ�	4�ֽ�	2�ֽ�	4�ֽ�	2�ֽ�	6�ֽ�	6�ֽ�
						if(memcmp(&nw_r_post->data[4],&nw_r_post->data[10],6)==0)
						{
							nw_sent(nw_r_post);
							change_IP();
							reset_STM32();
						}
						else
						{
							rev_err(chang_ip,0);
						}
					}
					else
					{
						rev_err(chang_ip,0xff);
					}
					break;
				case  check_ip:			//07	//	��ѯ��վIP��ַ���˿ںźͿ���	
//				if(nw_r_post->len==0)
					{
						nw_s.cmd=check_ip;
						nw_s.len=12;
						
						memcpy(sentbuf,nw_ip,4);
						sentbuf[4]=nw_prot>>8;
						sentbuf[5]=nw_prot;
						memcpy(&sentbuf[6],nw_phone_num,6);
						nw_s.data=sentbuf;
						
						nw_sent(&nw_s);
					}
					break;
				case  ResetDev:			//08	//	װ�ø�λ	��վ��װ�ý��и�λ
					if(memcmp(PSW,nw_r_post->data,4)==0)
					{
						nw_sent(nw_r_post);
						
					}
					else
					{
						rev_err(ResetDev,0xff);
					}
					break;
				case  waikup_device_m://09	//	���Ż���	��վ�Զ��ŷ�ʽ��������״̬��װ��
//					sys_mode=MODE_WORK;
					wakeup_dev();
					break;
				case  check_device_cfg://0A	//	��ѯװ�����ò���
//				if(nw_r_post->len==0)
					{
						nw_s.cmd=check_device_cfg;
//						memcpy(sentbuf,nw_ip,4);
						
//�������	1�ֽ�
//�ɼ����	2�ֽ�
//����ʱ��	2�ֽ�
//����ʱ��	2�ֽ�
//Ӳ������ʱ���	3�ֽ�
//ͨ��1ɫ��ѡ��	1�ֽ�
//ͨ��1ͼ���С	1�ֽ�
//ͨ��1����	1�ֽ�
//ͨ��1�Աȶ�	1�ֽ�
//ͨ��1���Ͷ�	1�ֽ�
//ͨ��2ɫ��ѡ��	1�ֽ�
//ͨ��2ͼ���С	1�ֽ�
//ͨ��2����	1�ֽ�
//ͨ��2�Աȶ�	1�ֽ�
//ͨ��2���Ͷ�	1�ֽ�
//��Ч����1	1�ֽ�
//��Ч����2	1�ֽ�
//����.	����
//��Ч����N	1�ֽ�				
						i=0;
						sentbuf[i++]=Heabtime;
						sentbuf[i++]=getdatetime>>8;
						sentbuf[i++]=getdatetime;
						sentbuf[i++]=sleeptime>>8;
						sentbuf[i++]=sleeptime;
						sentbuf[i++]=onlinetime>>8;
						sentbuf[i++]=onlinetime;
						sentbuf[i++]=resettime[0];
						sentbuf[i++]=resettime[1];
						sentbuf[i++]=resettime[2];
						sentbuf[i++]=1;sentbuf[i++]=1;sentbuf[i++]=60;sentbuf[i++]=60;sentbuf[i++]=60;
						sentbuf[i++]=1;sentbuf[i++]=1;sentbuf[i++]=60;sentbuf[i++]=60;sentbuf[i++]=60;
						memcpy(&sentbuf[i],function_buf,strlen((char *)function_buf));
						nw_s.len=i+strlen((char *)function_buf);
						nw_s.data=sentbuf;
						nw_sent(&nw_s);
					}
					break;
				case  device_function_cfg://0B	//	װ�ù�������	
					if(memcmp(PSW,nw_r_post->data,4)==0)
					{
						memcpy(function_buf,&nw_r_post->data[4],nw_r_post->len-4);
						function_buf[nw_r_post->len-4]=0; //��β��0
					}
					else
					{
						rev_err(device_function_cfg,0xff);
					}
					break;
				case  device_Sleep:	//0C	//	װ������	
					dev_sleep();
					break;
				case  check_device_time://0D	//	��ѯװ���豸ʱ��	
					nw_s.cmd=check_device_time;
					nw_s.len=6;
					nw_s.data=systime;
					nw_sent(&nw_s);
					break;
				case  sent_msg:
					nw_sent(nw_r_post);
//				������	https://blog.csdn.net/qq_30460905/article/details/81805891
					break;
				case  request_data:	//21	//	��վ����װ������	��վ����ɼ�װ�òɼ����ݲ���������
					if(nw_r_post->len==0)//�ϴ�δ�ɹ��ϴ�����ʷ���ݣ�������ʷ��Ƭ����װ������ʷ�������ϴ�
					{
						
					}
					else if((nw_r_post->len==2)&&(nw_r_post->data[0]==0xBB)&&(nw_r_post->data[1]==0xBB))//װ�����̲ɼ��������ݣ�ͼƬ���⣩����ɲɼ��������ϴ����ôβ�����Ӱ��ԭ�趨�ɼ������ִ��
					{
						
					}
					nw_sent(nw_r_post);
					break;
				case  request_la_qj://22	//	�ϴ��������������������	
					
					break;
				case  request_qx:		//0x25	//	�ϴ���������	
					break;
			}
		}
		else //��ʱ (1)//	
		{
			printf("\r\n��Ϣ��ʱ");
//			if((worktime-sentTime)>3600*24) //24СʱУʱһ��
//			{
//				ctlnumber=CheckT;
//			}
			
			if((systime[1]==resettime[0])&&(systime[2]==resettime[1])&&(systime[3]==resettime[2])) //24СʱУʱһ��
			{
					reset_STM32();//�����λ
			}
		
			nw_s.cmd=ctlnumber;
			
			switch(ctlnumber)
			{
				case 	Contact:			//00	//	����������Ϣ	װ�ÿ�������������Ϣ
					nw_s.len=sizeof(version);
					nw_s.data=version;
					nw_sent(&nw_s);
					retxtime=6000; 	//�������磬1�����ط�
					break;
					
				case  CheckT:				//01	//	Уʱ	�������������·������֣�һ��һ��
					nw_s.len=sizeof(systime);
					nw_s.data=systime;
					nw_sent(&nw_s);
					retxtime=12000;		//Уʱ��2�����ط�
					sentTime=worktime;	
//					if(mode==1)
//					{
//						ctlnumber=HeartBeat;
//					}
//				memcpy(sentTime,systime,6);
					break;
				
				case  SetPassword :	//02	//	����װ������	װ�ó������룺�ַ�����1234����31H32H33H34H��
//					memcmp(Password,systime,4)==0) //localpassw[50]
					break;
				
				case  Pcfg :				//03	//	��վ�·���������	��ָ��Ҫ�����ݲɼ�װ�ý��յ��������ԭ�����
					break;
				
				case  HeartBeat:		//05	//	װ��������Ϣ	������վ���װ������ʱ�䡢IP��ַ�Ͷ˿ںš������ź�ǿ�ȼ����ص�ѹ
					nw_s.len=8;
					memcpy(sentbuf,systime,6);
					sentbuf[6]=signal;
					sentbuf[7]=volt;
					nw_s.data=sentbuf;
					nw_sent(&nw_s);
					retxtime=Heabtime;		//Уʱ��2�����ط�
					sentTime=worktime;	
					break;
				
				case  chang_ip:			//06	//	������վIP��ַ���˿ںźͿ���	
					break;
				
				case  check_ip:			//07	//	��ѯ��վIP��ַ���˿ںźͿ���	
					break;
				case  ResetDev:			//08	//	װ�ø�λ	��վ��װ�ý��и�λ
					break;
				case  waikup_device_m://09	//	���Ż���	��վ�Զ��ŷ�ʽ��������״̬��װ��
					break;
				case  check_device_cfg://0A	//	��ѯװ�����ò���	
					break;
				case  device_function_cfg://0B	//	װ�ù�������	
					break;
				case  device_Sleep:	//0C	//	װ������	
					break;
				case  check_device_time://0D	//	��ѯװ���豸ʱ��	
					break;
				case  request_data:	//21	//	��վ����װ������	��վ����ɼ�װ�òɼ����ݲ���������
					break;
				case  request_la_qj://22	//	�ϴ��������������������	
					break;
				case  request_qx:		//0x25	//	�ϴ���������	
					break;
			}
		}
		

		
	}
}





void AppTaskNWS(void * p_arg)
{
	OS_ERR      err;
	(void)p_arg;
	
#if 1		
	OSMutexCreate ((OS_MUTEX  *)&RX_4G,           //ָ���ź���������ָ��
                   (CPU_CHAR  *)"RX For 4G", //�ź���������
                   (OS_ERR    *)&err);            //��������
#endif
	
	OSQCreate ((OS_Q*		)&NW_REV_Msg,	//��Ϣ����
                (CPU_CHAR*	)"NW_REV_Msg",	//��Ϣ��������
                (OS_MSG_QTY	)3,	//��Ϣ���г��ȣ���������Ϊ1
                (OS_ERR*	)&err);		//������
				

#if 1	
						 
		/* ����  �������� ���� */
    OSTaskCreate((OS_TCB     *)&NWTask_Contact_TCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"NWTask_Contact_Name",                             //��������
                 (OS_TASK_PTR ) NWTask_Contact,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) NWTask_Contact_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&NWTask_Contact_Stk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) NWTask_Contact_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) NWTask_Contact_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������
#endif					

#if 0
						 
		/* ���� Уʱ ���� */
    OSTaskCreate((OS_TCB     *)&NWTask_CheckT_TCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"NWTask_CheckT_Name",                             //��������
                 (OS_TASK_PTR ) NWTask_CheckT,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) NWTask_CheckT_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&NWTask_CheckT_Stk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) NWTask_CheckT_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) NWTask_CheckT_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������
#endif	
								 
#if 0
						 
		/* ���� ���� ���� */
    OSTaskCreate((OS_TCB     *)&NWTask_HeartBeat_TCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"NWTask_HeartBeat_Name",                             //��������
                 (OS_TASK_PTR ) NWTask_HeartBeat,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) NWTask_HeartBeat_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&NWTask_HeartBeat_Stk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) NWTask_HeartBeat_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) NWTask_HeartBeat_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������
#endif									 
								 
	
	while (DEF_TRUE)
	{
	
		OSTimeDly ( 6000, OS_OPT_TIME_DLY, & err );	
	}
//	OSTaskDel ( & AppTaskNWS_TCB, & err );                     //ɾ����ʼ������������������ 
	
}

void rev_4g_data(uint8_t *rev_data,uint16_t len)
{
	unsigned int dlong,i; 
	OS_ERR      err; 
	
	//				if((comp(start_and_device_number,rev_data,7)==0))			
	if (0 == memcmp(rev_data, STA,7))
	{
		
		dlong=rev_data[8];
		dlong=(dlong<<8)+rev_data[9];
//if((comp(start_and_device_number,uart3_t,7)==0))
//				{
//					dlong=uart3_t[8];
//					dlong=(dlong<<8)+uart3_t[9];
//					
//					if((checksum_4g(&uart3_t[1],6+1+2+dlong)==uart3_t[dlong+10])&&(uart3_t[dlong+11]==0x16))
//					{
//						Pend_4G_Mutex();
//						
//						command_4g=uart3_t[7];
//						dlong_4g=dlong;
//						for(i=0;i<dlong;i++) rdata_4g[i]=uart3_t[i+10];			
//						
//						RX_4G_Post();
//						
//						Post_4G_Mutex();
//					}	
//				}	
		if((checksum_4g(&rev_data[1],dlong+6+1+2)==rev_data[dlong+10])&&(rev_data[dlong+11]==0x16))
		{
			
			nw_r.cmd=rev_data[7];
			nw_r.len=dlong;
		
			for(i=0;i<dlong;i++) rev_data[i]=rev_data[i+10];			
			nw_r.data=rev_data;
//			printf("\r\nm-s %x,%d:%d:%d",nw_r.cmd,systime[3],systime[4],systime[5]);
			
			OSQPost((OS_Q*		)&NW_REV_Msg,		
							(void*		)&nw_r,
							(OS_MSG_SIZE)1,
							(OS_OPT		)OS_OPT_POST_FIFO,
							(OS_ERR*	)&err);
							
			printf("\r\nm-s %x,%x",nw_r.cmd,nw_r.len);
		}
		else
		{
			printf("\r\nm-s data err");
		}
	}	
}
void rev_4g_cmmd(uint8_t *rev_data,uint16_t len)
{
	OS_ERR      err; 
	
	if (0 == memcmp("\r\n+CPIN: READY", rev_data, sizeof("\r\n+CPIN: READY") - 1))
	{
		nw_r.cmd=0xff; //4g Post 
		nw_r.len=0;
		OSQPost((OS_Q*		)&NW_REV_Msg,		
							(void*		)&nw_r,
							(OS_MSG_SIZE)1,
							(OS_OPT		)OS_OPT_POST_FIFO,
							(OS_ERR*	)&err);
							
		printf("\r\n4g-s:Post READY");			
	}
	else
	{
		printf("\r\n4g-s:%s",rev_data);
	}
}
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

//unsigned char cmu[]={0x68,0x43,0x43,0x30,0x31,0x39,0x36,0x00,0x00,0x02,0x03,0x00};
//unsigned char start_and_series[]={0x68,0x43,0x43,0x30,0x31,0x39,0x36,0x00,0x00,0x02,0x03,0x00};
//unsigned char nw_sent_data[72]={0x68,0x43,0x43,0x30,0x31,0x39,0x36,0,0,0,0,0};
//unsigned char device_number[6]={0x43,0x43,0x30,0x31,0x39,0x36};
//unsigned char start_and_device_number[7]={0x68,0x43,0x43,0x30,0x31,0x39,0x36};

//typedef struct
//{
//	unsigned char cmd;
//	unsigned int dlong;
//	unsigned char *p;   
//}nw_a;



void  nw_sent(struct nw_ctl *t_data) //unsigned char command,unsigned char * t_data,unsigned int dlong
{
	unsigned char sent[100],i;
	CPU_SR_ALLOC();
	
	memcpy(sent,STA, 7);
	sent[7]=t_data->cmd;
	sent[8]=(t_data->len>>8);
	sent[9]=t_data->len;
	
	for(i=0;i<t_data->len;i++)
	{
		sent[10+i]=t_data->data[i];
	}
	sent[10+t_data->len]=checksum_4g(&sent[1],9+t_data->len);
	sent[11+t_data->len]=0x16;
	
	OS_CRITICAL_ENTER();                             //�����ٽ��
	HAL_UART_Transmit(&huart3, sent, (12+t_data->len),100);
	USART_PRINTF_FLAG=P_MODE_huart1;
	printf("\r\ns-m %x,%d:%d:%d",t_data->cmd,systime[3],systime[4],systime[5]);
	
	HAL_UART_Transmit_IT(&huart1, uart1_t, strlen((char *)uart1_t));
	OS_CRITICAL_EXIT();                          //�ٽ��
	
//	memcpy(sent,start_and_device_number, sizeof(start_and_device_number));
}

void rev_err(unsigned char ctl,unsigned char data)
{
	unsigned char sent[14];
	CPU_SR_ALLOC();
	memcpy(sent,STA, 7);
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


