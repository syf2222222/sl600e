
#include "bsp.h"
#include "nw.h"
#include "global.h" 

extern SRAM_HandleTypeDef hsram1;

//��FDS��ָ��ն������ָ�����������
//�ָ����ò����������ã�
//TID=43433030303131323334   ID��CC0001��Э�����룺1234
//PSW=31323334   �ն����룺1234
//SV=6E   ��ѹ����������110
//���������1����
//���������10����
//����ʱ����0����
//����ʱ����10����
//Ӳ��������2��3��

//������IP��183.63.92.43
//�������˿ںţ�3205
//����IP��192.168.0.50
//����IP��192.168.0.1
//���IP 1��192.168.0.5
//���IP 2��192.168.0.6
//�������룺255.255.255.0

//VP=120 �� ������ѹ��12V
//VPT=30��10  ����ѹ����ʱ��
//MRT=24     ���������ʱ��
//PTS=20 ��   ��̨�ƶ�������
//DRC=5��   DTU������ֵ��5��
//WTC=1E5A0A00��  DTU��ʱ��30�룬�����ʱ��90�룬��������ʱ��10�� ����ͼ���0��
//POT=30,30 ���1����ʱ30����,���2����ʱ��30����
//PRC=50 ��һ�δ�ͼ����������50��
//PMN=200 ��SD���ѭ���洢200��
//��Ƶ��������ɫ��1280*720������60���Աȶ�60�����Ͷ�60
//ͨ�����ͣ�����֡��15��֡���50����������H264������1024��λ
//ModuleCfg:00000000 00000000 00000000 11000000 ����ͼ���⹦�ܺ���Ƶ��⹦��
//gfx send interval: 0  ���ݴ�����0��
//Camere1 move step:20,Camere2 move step:20  һ·�������/�ٶ�20����·�������/�ٶ� 20
//Picture resend  num:50 ͼƬ�ط�����50
//Picture max write num : 200  ͼ�����д���ڴ濨����200
//SD picture num:0   SD����ͼ������0
//Connect times : 0  00H���Ӷ��ٴ�����01H
//WPC:0 WPC����������
//RCS:20 RCS������С�̶�
//DTU:ON   Camera1:OFF   Camera2:OFF   Router:ON   ETH:OFF   
//DTU��Դ�������1��Դ�أ����2��Դ�أ�·�ɿ�����̫����

#define TID_INT "CC01961234"
#define PSW_INT "1234"


//extern	unsigned char Password[4];//
extern	unsigned char Heabtime;//���������װ��������Ϣ���ͼ������λ���ӣ���������ӦΪ1����;
extern	unsigned short gaindatetime;//�ɼ��������ÿ�����ٷ��Ӳ���һ�Σ��ɼ����������ʱ���޹أ�����λ���ӣ���������ӦΪ20���ӣ�
extern	unsigned short sleeptime;//����ʱ�������ݲɼ����ܹرջ�ͨ���豸����ʱ�䣬��ʱ���ڿ�֧�ֶ��Ż����绽�ѣ���λ���ӣ���Ϊ0��װ�ò����ߣ�
extern	unsigned short onlinetime;		//����ʱ����ͨ���豸�������ݲɼ�������ͨ���豸����ʱ�䣻��λ���ӣ�
extern	unsigned char resettime[3];	//3bye Ӳ������ʱ��㣺Ϊ��֤װ������ɿ�����װ��Ӧ֧�ֶ�ʱ������
extern	unsigned short checkword[4];	//������֤��4�ֽڣ�װ�ó�ʼΪ�ַ���1234����31H32H33H34H����Ϊȷ��װ�����ݵ���ȷ�ԣ���ֹ�Ƿ��û�������ƭ��������



//VP=120 �� ������ѹ��12V
//VPT=30��10  ����ѹ����ʱ��
//MRT=24     ���������ʱ��
//PTS=20 ��   ��̨�ƶ�������
//DRC=5��   DTU������ֵ��5��
//WTC=1E5A0A00��  DTU��ʱ��30�룬�����ʱ��90�룬��������ʱ��10�� ����ͼ���0��
//POT=30,30 ���1����ʱ30����,���2����ʱ��30����
//PRC=50 ��һ�δ�ͼ����������50��
//PMN=200 ��SD���ѭ���洢200��

#define VP			(( uint8_t*) (CH2Port+2))


uint16_t rData[10];


//��FDS��ָ��ն������ָ�����������
//�ָ����ò����������ã�
//TID=43433030303131323334   ID��CC0001��Э�����룺1234
//PSW=31323334   �ն����룺1234
//SV=6E   ��ѹ����������110
//���������1����
//���������10����
//����ʱ����0����
//����ʱ����10����
//Ӳ��������2��3��
//������IP��183.63.92.43
//�������˿ںţ�3205
//����IP��192.168.0.50
//����IP��192.168.0.1
//���IP 1��192.168.0.5
//���IP 2��192.168.0.6
//�������룺255.255.255.0
//VP=120 �� ������ѹ��12V
//VPT=30��10  ����ѹ����ʱ��
//MRT=24     ���������ʱ��
//PTS=20 ��   ��̨�ƶ�������
//DRC=5��   DTU������ֵ��5��
//WTC=1E5A0A00��  DTU��ʱ��30�룬�����ʱ��90�룬��������ʱ��10�� ����ͼ���0��
//POT=30,30 ���1����ʱ30����,���2����ʱ��30����
//PRC=50 ��һ�δ�ͼ����������50��
//PMN=200 ��SD���ѭ���洢200��
//��Ƶ��������ɫ��1280*720������60���Աȶ�60�����Ͷ�60
//ͨ�����ͣ�����֡��15��֡���50����������H264������1024��λ
//ModuleCfg:00000000 00000000 00000000 11000000 ����ͼ���⹦�ܺ���Ƶ��⹦��
//gfx send interval: 0  ���ݴ�����0��
//Camere1 move step:20,Camere2 move step:20  һ·�������/�ٶ�20����·�������/�ٶ� 20
//Picture resend  num:50 ͼƬ�ط�����50
//Picture max write num : 200  ͼ�����д���ڴ濨����200
//SD picture num:0   SD����ͼ������0
//Connect times : 0  00H���Ӷ��ٴ�����01H
//WPC:0 WPC����������
//RCS:20 RCS������С�̶�
//DTU:ON   Camera1:OFF   Camera2:OFF   Router:ON   ETH:OFF   
//DTU��Դ�������1��Դ�أ����2��Դ�أ�·�ɿ�����̫����

void WriteFM25Operation(unsigned int addr,unsigned char *databuf,unsigned int len);
void ReadFM25Operation(unsigned int addr,unsigned char *databuf,unsigned int len);
int clear_index_all(void);
int char_time_to_int_time(uint8_t *in,uint32_t *sec);
int int_time_to_char_time(uint8_t *out,uint32_t sec);
	void save_systime(void);
void data_rst(void)
{
//	char i,buf[20];
	printf("\r\n��д����");
	*STAA=0x68;

	strncpy((char *)TID, "CC019612341234",14);

	*SV=1;
	*HBT=1;
	*GDT=10;
	*SLT=0;
	*OLT=10;	
	RST[0]=0;
	RST[1]=2;
	RST[2]=3;
	
//////IP	
	HostIP[0]=183;HostIP[1]=63;HostIP[2]=92;HostIP[3]=43; //183.63.92.43,17953
	*HostPort=13012;// 17953;//
	LoclIP[0]=192;LoclIP[1]=168;LoclIP[2]=1;LoclIP[3]=119;
	GWIP[0]=192;GWIP[1]=168;GWIP[2]=1;GWIP[3]=1;
	CH1IP[0]=192;CH1IP[1]=168;CH1IP[2]=1;CH1IP[3]=11;
	CH2IP[0]=192;CH2IP[1]=168;CH2IP[2]=1;CH2IP[3]=12;
	NetMst[0]=255;NetMst[1]=255;NetMst[2]=255;NetMst[3]=0;
	*WidthxHeight=7;//Ŀǰֻ֧�� 7�� 10  1280*720,1280*960
	*bLevel=60;
	*cLevel=60;
	*sLevel=60;
	
	runningtime=(1*24*3600);	// 737637
	save_systime();
	
	function_nub[0]=3;
	function_buf[0]=0x22;
	function_buf[1]=0x25;
	function_buf[2]=0x84;
	
	int_time_to_char_time(systime,runningtime);
	printf("\r\n����ʱ��:\r\n20%02d-%02d-%02d,%02d:%02d:%02d",systime[0],systime[1],systime[2],systime[3],systime[4],systime[5]);

	WriteFM25Operation(sys_buf_flash,sys_buf_sram,250);
	clear_index_all();
}
int gain_index_all(void);
extern const uint16_t Width[21];
extern const uint16_t Height[21];

//#define STAA1				(( uint8_t*) (FMC_BANK1_1)) 	//��ʼ�� 0x68
//#define TID1				(( uint8_t*) (STAA+1))	//ID�� CC0196
//#define CHW1				(( uint8_t*) (TID+6))		//������֤��"1234"
//#define PSW1				(( uint8_t*) (CHW+4))		//�ն�����  "1234"
//#define SV1				(( uint8_t*) (PSW+3))		//��ѹ��������?�汾��?

//#define HBT1				(( uint16_t*) ((( uint16_t*)SV1)+1))		//����ʱ��
//#define GDT1				(( uint16_t*) (HBT+2))	//����ʱ��
//#define SLT1				(( uint16_t*) (GDT+2))	//˯��ʱ��
//#define OLT1				(( uint16_t*) (SLT+2))	//����ʱ��
//#define RST1				(( uint16_t*) (OLT+2))	//��λʱ��

extern uint8_t function22_flag,function25_flag,function84_flag;

void data_read(void)
{
	uint32_t time_saved_buf;
	uint8_t j;
	
	ReadFM25Operation(sys_buf_flash,sys_buf_sram,250);
	printf("\r\n");
	printf("\r\nSTAA\t=%x",STAA[0]);
	printf("\r\nTID\t=%02x%02x%02x%02x%02x%02x",TID[0],TID[1],TID[2],TID[3],TID[4],TID[5]);
	printf("\r\nCHW\t=%02x%02x%02x%02x",CHW[0],CHW[1],CHW[2],CHW[3]);printf("(��֤����)");
	printf("\r\nPSW\t=%02x%02x%02x%02x",PSW[0],PSW[1],PSW[2],PSW[3]);
	printf("\r\nSV\t=%d",*SV);
	printf("\r\nHBT\t=%d",*HBT);
	printf("\r\nGDT\t=%d",*GDT);
	printf("\r\nSLT\t=%d",*SLT);
	printf("\r\nOLT\t=%d",*OLT);
	printf("\r\nRST:\t%d�� %dʱ %d��",RST[0],RST[1],RST[2]);
	printf("\r\nHostIP:\t%d,%d,%d,%d,%d",HostIP[0],HostIP[1],HostIP[2],HostIP[3],*HostPort); printf("(Ϊ�洢����,�� CONFIGDTU ����)");
//	printf("\r\nHostIP:\t%d,%d,%d,%d,%d",0,0,0,0,0);
	printf("\r\nLoclIP:\t%d,%d,%d,%d",LoclIP[0],LoclIP[1],LoclIP[2],LoclIP[3]);
//	printf("\r\nLoclIP:\t%d,%d,%d,%d,%d",0,0,0,0,0);
	printf("\r\nGWIP:\t%d,%d,%d,%d",GWIP[0],GWIP[1],GWIP[2],GWIP[3]);
	printf("\r\nCH1IP:\t%d,%d,%d,%d",CH1IP[0],CH1IP[1],CH1IP[2],CH1IP[3]);
	printf("\r\nCH2IP:\t%d,%d,%d,%d",CH2IP[0],CH2IP[1],CH2IP[2],CH2IP[3]);
	printf("\r\nNetMst:\t%d,%d,%d,%d",NetMst[0],NetMst[1],NetMst[2],NetMst[3]);
	
	
	printf("\r\n��Ƶ��С%d*%d",Width[*WidthxHeight],Height[*WidthxHeight]);//1280*720,1280*960
	printf("(Ϊ�洢����)");
	if((*WidthxHeight!=7)&&(*WidthxHeight!=10)) printf("\r\n��Ƶ��С���������Ҫ��?");
	printf("\r\n����  :%d",*bLevel);printf("(Ϊ�洢����)");
	printf("\r\n�Աȶ�:%d",*cLevel);printf("(Ϊ�洢����)");
	printf("\r\n���Ͷ�:%d",*sLevel);printf("(Ϊ�洢����)");
	
	time_saved_buf=(runningtime_buf[0]*0x1000000)+(runningtime_buf[1]*0x10000)+(runningtime_buf[2]*0x100)+(runningtime_buf[3]);
	printf("\r\ntime_saved_buf=%08x",time_saved_buf);
	
	if(runningtime==0) runningtime =time_saved_buf; //�ϵ�Ϊ0,��ȡֵ
	else if(runningtime >(time_saved_buf+60)) save_systime();	//��ѯ����һ��
	else if(runningtime <(time_saved_buf)) printf("\r\nrunningtime err"); //�ȱ���ֵ��С,������
	
//	printf("\r\n����ʱ��32:\r\n%02d,%02d,%02d,%02d",systime32_buf[0],systime32_buf[1],systime32_buf[2],systime32_buf[3]);
	printf("\r\nrunningtime=%08x",runningtime);
	int_time_to_char_time(systime,runningtime);
	printf("\r\nʱ��:\r\n20%02d-%02d-%02d,%02d:%02d:%02d",systime[0],systime[1],systime[2],systime[3],systime[4],systime[5]);
	
	printf("\r\n��������:");
	if(function_nub[0]>10) 
	{
		function_nub[0]=3;
		function_buf[0]=0x22;
		function_buf[1]=0x25;
		function_buf[1]=0x84;
	}
	for(j=0;j<function_nub[0];j++)
	{
		printf("0x%02x,",function_buf[j]);
		if(function_buf[j]==0x22)
		{
			function22_flag=1;
		}
		if(function_buf[j]==0x25)
		{
			function25_flag=1;
		}
		if(function_buf[j]==0x84)
		{
			function84_flag=1;
		}
	}
	
	gain_index_all();
	printf("\r\n��������:");
	printf("\r\ninc ll_sent_cnt=%d,ll_gain_cnt=%d",ll_sent_cnt,ll_gain_cnt);
	printf("\r\ninc qx_sent_cnt=%d,qx_gain_cnt=%d",qx_sent_cnt,qx_gain_cnt);
	printf("\r\ninc ph_sent_cnt=%d,ph_gain_cnt=%d",ph_sent_cnt,ph_gain_cnt);
	
	flash_read(aut_point_group1,aut_point_group1,1024);//��ȡ����ʱ���
}

void data_write()
{
//	char i,buf[20];
	
	if(*STAA !=0x68)
	{
		printf("\r\n���洢����Ϊ��,��ʼ��");
		data_rst();
	}
	else 
	{
//		printf("\r\n������д洢����");
	}
}

void SRAM_data_int(void)
{
//	uint32_t temp;
	
//	data_rst();
	data_read();
	data_write();

}

