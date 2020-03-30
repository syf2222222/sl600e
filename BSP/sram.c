
#include "bsp.h"
#include "nw.h"
#include "global.h" 

extern SRAM_HandleTypeDef hsram1;

//“FDS”指令，终端立即恢复出厂参数。
//恢复配置参数出厂设置：
//TID=43433030303131323334   ID：CC0001，协议密码：1234
//PSW=31323334   终端密码：1234
//SV=6E   电压修正参数：110
//心跳间隔：1分钟
//采样间隔：10分钟
//休眠时长：0分钟
//在线时长：10分钟
//硬件重启：2点3分

//服务器IP：183.63.92.43
//服务器端口号：3205
//本地IP：192.168.0.50
//网关IP：192.168.0.1
//球机IP 1：192.168.0.5
//球机IP 2：192.168.0.6
//子网掩码：255.255.255.0

//VP=120 ， 保护电压：12V
//VPT=30，10  ，电压保护时间
//MRT=24     ，最大运行时间
//PTS=20 ，   云台移动步进量
//DRC=5，   DTU重连阈值：5次
//WTC=1E5A0A00，  DTU打开时间30秒，球机打开时间90秒，传感器打开时间10秒 ，传图间隔0秒
//POT=30,30 球机1在线时30分钟,球机2在线时长30分钟
//PRC=50 ，一次传图补包最大次数50次
//PMN=200 ，SD最大循环存储200张
//视频参数：彩色，1280*720，亮度60，对比度60，饱和度60
//通道类型：主，帧率15，帧间隔50，编码类型H264，码率1024，位
//ModuleCfg:00000000 00000000 00000000 11000000 配置图像检测功能和视频检测功能
//gfx send interval: 0  数据传输间隔0秒
//Camere1 move step:20,Camere2 move step:20  一路球机步长/速度20，二路球机补仓/速度 20
//Picture resend  num:50 图片重发数量50
//Picture max write num : 200  图像最大写入内存卡数量200
//SD picture num:0   SD卡中图像数量0
//Connect times : 0  00H连接多少次跳到01H
//WPC:0 WPC传感器常关
//RCS:20 RCS雨量最小刻度
//DTU:ON   Camera1:OFF   Camera2:OFF   Router:ON   ETH:OFF   
//DTU电源开，球机1电源关，球机2电源关，路由开，以太网关

#define TID_INT "CC01961234"
#define PSW_INT "1234"


//extern	unsigned char Password[4];//
extern	unsigned char Heabtime;//心跳间隔：装置心跳信息发送间隔，单位分钟，出厂配置应为1分钟;
extern	unsigned short gaindatetime;//采集间隔：即每隔多少分钟采样一次（采集间隔与拍照时间无关），单位分钟，出厂配置应为20分钟；
extern	unsigned short sleeptime;//休眠时长：数据采集功能关闭或通信设备休眠时间，该时间内可支持短信或网络唤醒；单位分钟，若为0则装置不休眠；
extern	unsigned short onlinetime;		//在线时长：通信设备保持数据采集及网络通信设备在线时间；单位分钟；
extern	unsigned char resettime[3];	//3bye 硬件重启时间点：为保证装置软件可靠运行装置应支持定时重启。
extern	unsigned short checkword[4];	//密文认证：4字节，装置初始为字符‘1234’（31H32H33H34H），为确认装置数据的正确性，防止非法用户恶意欺骗服务器。



//VP=120 ， 保护电压：12V
//VPT=30，10  ，电压保护时间
//MRT=24     ，最大运行时间
//PTS=20 ，   云台移动步进量
//DRC=5，   DTU重连阈值：5次
//WTC=1E5A0A00，  DTU打开时间30秒，球机打开时间90秒，传感器打开时间10秒 ，传图间隔0秒
//POT=30,30 球机1在线时30分钟,球机2在线时长30分钟
//PRC=50 ，一次传图补包最大次数50次
//PMN=200 ，SD最大循环存储200张

#define VP			(( uint8_t*) (CH2Port+2))


uint16_t rData[10];


//“FDS”指令，终端立即恢复出厂参数。
//恢复配置参数出厂设置：
//TID=43433030303131323334   ID：CC0001，协议密码：1234
//PSW=31323334   终端密码：1234
//SV=6E   电压修正参数：110
//心跳间隔：1分钟
//采样间隔：10分钟
//休眠时长：0分钟
//在线时长：10分钟
//硬件重启：2点3分
//服务器IP：183.63.92.43
//服务器端口号：3205
//本地IP：192.168.0.50
//网关IP：192.168.0.1
//球机IP 1：192.168.0.5
//球机IP 2：192.168.0.6
//子网掩码：255.255.255.0
//VP=120 ， 保护电压：12V
//VPT=30，10  ，电压保护时间
//MRT=24     ，最大运行时间
//PTS=20 ，   云台移动步进量
//DRC=5，   DTU重连阈值：5次
//WTC=1E5A0A00，  DTU打开时间30秒，球机打开时间90秒，传感器打开时间10秒 ，传图间隔0秒
//POT=30,30 球机1在线时30分钟,球机2在线时长30分钟
//PRC=50 ，一次传图补包最大次数50次
//PMN=200 ，SD最大循环存储200张
//视频参数：彩色，1280*720，亮度60，对比度60，饱和度60
//通道类型：主，帧率15，帧间隔50，编码类型H264，码率1024，位
//ModuleCfg:00000000 00000000 00000000 11000000 配置图像检测功能和视频检测功能
//gfx send interval: 0  数据传输间隔0秒
//Camere1 move step:20,Camere2 move step:20  一路球机步长/速度20，二路球机补仓/速度 20
//Picture resend  num:50 图片重发数量50
//Picture max write num : 200  图像最大写入内存卡数量200
//SD picture num:0   SD卡中图像数量0
//Connect times : 0  00H连接多少次跳到01H
//WPC:0 WPC传感器常关
//RCS:20 RCS雨量最小刻度
//DTU:ON   Camera1:OFF   Camera2:OFF   Router:ON   ETH:OFF   
//DTU电源开，球机1电源关，球机2电源关，路由开，以太网关

void WriteFM25Operation(unsigned int addr,unsigned char *databuf,unsigned int len);
void ReadFM25Operation(unsigned int addr,unsigned char *databuf,unsigned int len);
int clear_index_all(void);
int char_time_to_int_time(uint8_t *in,uint32_t *sec);
int int_time_to_char_time(uint8_t *out,uint32_t sec);
	void save_systime(void);
void data_rst(void)
{
//	char i,buf[20];
	printf("\r\n重写数据");
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
	*WidthxHeight=7;//目前只支持 7和 10  1280*720,1280*960
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
	printf("\r\n启动时间:\r\n20%02d-%02d-%02d,%02d:%02d:%02d",systime[0],systime[1],systime[2],systime[3],systime[4],systime[5]);

	WriteFM25Operation(sys_buf_flash,sys_buf_sram,250);
	clear_index_all();
}
int gain_index_all(void);
extern const uint16_t Width[21];
extern const uint16_t Height[21];

//#define STAA1				(( uint8_t*) (FMC_BANK1_1)) 	//起始码 0x68
//#define TID1				(( uint8_t*) (STAA+1))	//ID码 CC0196
//#define CHW1				(( uint8_t*) (TID+6))		//密文验证："1234"
//#define PSW1				(( uint8_t*) (CHW+4))		//终端密码  "1234"
//#define SV1				(( uint8_t*) (PSW+3))		//电压修正参数?版本号?

//#define HBT1				(( uint16_t*) ((( uint16_t*)SV1)+1))		//心跳时间
//#define GDT1				(( uint16_t*) (HBT+2))	//采样时间
//#define SLT1				(( uint16_t*) (GDT+2))	//睡眠时间
//#define OLT1				(( uint16_t*) (SLT+2))	//在线时间
//#define RST1				(( uint16_t*) (OLT+2))	//复位时间

extern uint8_t function22_flag,function25_flag,function84_flag;

void data_read(void)
{
	uint32_t time_saved_buf;
	uint8_t j;
	
	ReadFM25Operation(sys_buf_flash,sys_buf_sram,250);
	printf("\r\n");
	printf("\r\nSTAA\t=%x",STAA[0]);
	printf("\r\nTID\t=%02x%02x%02x%02x%02x%02x",TID[0],TID[1],TID[2],TID[3],TID[4],TID[5]);
	printf("\r\nCHW\t=%02x%02x%02x%02x",CHW[0],CHW[1],CHW[2],CHW[3]);printf("(验证密文)");
	printf("\r\nPSW\t=%02x%02x%02x%02x",PSW[0],PSW[1],PSW[2],PSW[3]);
	printf("\r\nSV\t=%d",*SV);
	printf("\r\nHBT\t=%d",*HBT);
	printf("\r\nGDT\t=%d",*GDT);
	printf("\r\nSLT\t=%d",*SLT);
	printf("\r\nOLT\t=%d",*OLT);
	printf("\r\nRST:\t%d日 %d时 %d分",RST[0],RST[1],RST[2]);
	printf("\r\nHostIP:\t%d,%d,%d,%d,%d",HostIP[0],HostIP[1],HostIP[2],HostIP[3],*HostPort); printf("(为存储数据,发 CONFIGDTU 配置)");
//	printf("\r\nHostIP:\t%d,%d,%d,%d,%d",0,0,0,0,0);
	printf("\r\nLoclIP:\t%d,%d,%d,%d",LoclIP[0],LoclIP[1],LoclIP[2],LoclIP[3]);
//	printf("\r\nLoclIP:\t%d,%d,%d,%d,%d",0,0,0,0,0);
	printf("\r\nGWIP:\t%d,%d,%d,%d",GWIP[0],GWIP[1],GWIP[2],GWIP[3]);
	printf("\r\nCH1IP:\t%d,%d,%d,%d",CH1IP[0],CH1IP[1],CH1IP[2],CH1IP[3]);
	printf("\r\nCH2IP:\t%d,%d,%d,%d",CH2IP[0],CH2IP[1],CH2IP[2],CH2IP[3]);
	printf("\r\nNetMst:\t%d,%d,%d,%d",NetMst[0],NetMst[1],NetMst[2],NetMst[3]);
	
	
	printf("\r\n视频大小%d*%d",Width[*WidthxHeight],Height[*WidthxHeight]);//1280*720,1280*960
	printf("(为存储数据)");
	if((*WidthxHeight!=7)&&(*WidthxHeight!=10)) printf("\r\n视频大小不符合球机要求?");
	printf("\r\n亮度  :%d",*bLevel);printf("(为存储数据)");
	printf("\r\n对比度:%d",*cLevel);printf("(为存储数据)");
	printf("\r\n饱和度:%d",*sLevel);printf("(为存储数据)");
	
	time_saved_buf=(runningtime_buf[0]*0x1000000)+(runningtime_buf[1]*0x10000)+(runningtime_buf[2]*0x100)+(runningtime_buf[3]);
	printf("\r\ntime_saved_buf=%08x",time_saved_buf);
	
	if(runningtime==0) runningtime =time_saved_buf; //上电为0,读取值
	else if(runningtime >(time_saved_buf+60)) save_systime();	//查询保存一次
	else if(runningtime <(time_saved_buf)) printf("\r\nrunningtime err"); //比保存值还小,不可能
	
//	printf("\r\n启动时间32:\r\n%02d,%02d,%02d,%02d",systime32_buf[0],systime32_buf[1],systime32_buf[2],systime32_buf[3]);
	printf("\r\nrunningtime=%08x",runningtime);
	int_time_to_char_time(systime,runningtime);
	printf("\r\n时间:\r\n20%02d-%02d-%02d,%02d:%02d:%02d",systime[0],systime[1],systime[2],systime[3],systime[4],systime[5]);
	
	printf("\r\n功能配置:");
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
	printf("\r\n索引数据:");
	printf("\r\ninc ll_sent_cnt=%d,ll_gain_cnt=%d",ll_sent_cnt,ll_gain_cnt);
	printf("\r\ninc qx_sent_cnt=%d,qx_gain_cnt=%d",qx_sent_cnt,qx_gain_cnt);
	printf("\r\ninc ph_sent_cnt=%d,ph_gain_cnt=%d",ph_sent_cnt,ph_gain_cnt);
	
	flash_read(aut_point_group1,aut_point_group1,1024);//读取拍照时间表
}

void data_write()
{
//	char i,buf[20];
	
	if(*STAA !=0x68)
	{
		printf("\r\n检测存储数据为空,初始化");
		data_rst();
	}
	else 
	{
//		printf("\r\n检测已有存储数据");
	}
}

void SRAM_data_int(void)
{
//	uint32_t temp;
	
//	data_rst();
	data_read();
	data_write();

}

