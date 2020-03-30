#ifndef __NW__H
#define __NW__H

#ifdef   NW_MODULE
#define  NW_EXT
//unsigned char start_and_device_number[7]={0x68,0x43,0x43,0x30,0x31,0x39,0x36};
unsigned char version[2]={3,0};

//	密码	4字节
//	心跳间隔	1字节
//	采集间隔	2字节
//	休眠时长	2字节
//	在线时长	2字节
//	硬件重启时间点	3字节
//	密文验证码	4字节
//unsigned char Password[4]={'1','2','3','4'};//
//unsigned char Heabtime=60;//心跳间隔：装置心跳信息发送间隔，单位分钟，出厂配置应为1分钟;
//unsigned short getdatetime=20;//采集间隔：即每隔多少分钟采样一次（采集间隔与拍照时间无关），单位分钟，出厂配置应为20分钟；
//unsigned short sleeptime=0;//休眠时长：数据采集功能关闭或通信设备休眠时间，该时间内可支持短信或网络唤醒；单位分钟，若为0则装置不休眠；
//unsigned short onlinetime=10;		//在线时长：通信设备保持数据采集及网络通信设备在线时间；单位分钟；
//unsigned char resettime[3]={1,0,0};	//3bye 硬件重启时间点：为保证装置软件可靠运行装置应支持定时重启。
//unsigned short checkword[4]={'1','2','3','4'};	//密文认证：4字节，装置初始为字符‘1234’（31H32H33H34H），为确认装置数据的正确性，防止非法用户恶意欺骗服务器。
//该密文用于防止非法装置用户的数据被主站认可，安装时装置设定默认密文，上塔安装完成后，
//主站下发指令修改该装置密文，仅装置主站记录的密文一致时视该数据合法有效，否则屏蔽。
#else
#define  NW_EXT  extern
//extern	unsigned char start_and_device_number[7];
//extern	unsigned char version[2];

//extern	unsigned char Password[4];//
//extern	unsigned char Heabtime;//心跳间隔：装置心跳信息发送间隔，单位分钟，出厂配置应为1分钟;
//extern	unsigned short getdatetime;//采集间隔：即每隔多少分钟采样一次（采集间隔与拍照时间无关），单位分钟，出厂配置应为20分钟；
//extern	unsigned short sleeptime;//休眠时长：数据采集功能关闭或通信设备休眠时间，该时间内可支持短信或网络唤醒；单位分钟，若为0则装置不休眠；
//extern	unsigned short onlinetime;		//在线时长：通信设备保持数据采集及网络通信设备在线时间；单位分钟；
//extern	unsigned char resettime[3];	//3bye 硬件重启时间点：为保证装置软件可靠运行装置应支持定时重启。
//extern	unsigned short checkword[4];	//密文认证：4字节，装置初始为字符‘1234’（31H32H33H34H），为确认装置数据的正确性，防止非法用户恶意欺骗服务器。
#endif



struct aut_get_photo
{
	unsigned char h;
	unsigned char m;
	unsigned char p;   
};

//struct nw_cmd
//{
//	unsigned char cmd;
//	unsigned char cmd_state;
//	unsigned char data;   
//};

struct nw_ctl
{
	unsigned char cmd;
	unsigned int len;
	unsigned char data[50];   
};

NW_EXT struct nw_ctl nw_rc,nw_sc;



//unsigned char cmu[]={0x68,0x43,0x43,0x30,0x31,0x39,0x36,0x00,0x00,0x02,0x03,0x00};
//unsigned char start_and_series[]={0x68,0x43,0x43,0x30,0x31,0x39,0x36,0x00,0x00,0x02,0x03,0x00};
//unsigned char nw_sent_data[72]={0x68,0x43,0x43,0x30,0x31,0x39,0x36,0,0,0,0,0};
//unsigned char device_number[6]={0x43,0x43,0x30,0x31,0x39,0x36};
//unsigned char start_and_device_number[7]={0x68,0x43,0x43,0x30,0x31,0x39,0x36};


#define	Contact								0x00	//	开机联络信息	装置开机上送联络信息
#define CheckT								0x01	//	校时	该命令分请求和下发两部分
#define SetPassword 					0x02	//	设置装置密码	装置出厂密码：字符：‘1234’（31H32H33H34H）
#define Pcfg 									0x03	//	主站下发参数配置	该指令要求数据采集装置接收到该命令后原命令返回
#define HeartBeat 						0x05	//	装置心跳信息	用于主站监测装置上线时间、IP地址和端口号、传输信号强度及蓄电池电压
#define chang_ip	 						0x06	//	更改主站IP地址、端口号和卡号	
#define check_ip 							0x07	//	查询主站IP地址、端口号和卡号	
#define ResetDev	 						0x08	//	装置复位	主站对装置进行复位
#define waikup_device_m				0x09	//	短信唤醒	主站以短信方式唤醒休眠状态的装置
#define check_device_cfg 			0x0A	//	查询装置配置参数	
#define device_function_cfg		0x0B	//	装置功能配置	
#define device_Sleep 					0x0C	//	装置休眠	
#define check_device_time 		0x0D	//	查询装置设备时间	
#define sent_msg					 		0x0E	//	发送确认短信
#define request_data 					0x21	//	主站请求装置数据	主站请求采集装置采集数据并立即上送
#define request_la_qj 				0x22	//	上传导地线拉力及倾角数据	
#define request_qx 						0x25	//	上传气象数据	

#define	updata_online					0xff	//	开机联络信息	装置开机上送联络信息
/*
#define  0x26 //	上传导线温度、导线电流数据	
#define  0x27 //	上传杆塔振动数据	
#define  0x29 //	上传舞动振幅频率数据	
#define  0x2A //	上传杆塔倾斜数据	
#define  0x2B //	上传导线微风振动数据	
#define  0x2C //	上传综合防盗数据	
#define  0x2D //	上送山火报警数据	
#define  0x2E //	上送大风舞动报警数据	
#define  0x30 //	上传设备故障信息	
#define  0x31 //	主站请求微风振动动态数据	
#define  0x32 //	微风振动动态数据上送	
#define  0x33 //	微风振动动态数据上送结束标记	
#define  0x34 //	微风振动动态数据补报下发	
#define  0x35 //	主站请求舞动动态数据	
#define  0x36 //	舞动动态数据上送	
#define  0x37 //	舞动动态数据上送结束标记	
#define  0x38 //	舞动动态数据补包下发	
#define  0x39 //	主站请求拉力及偏角动态数据	
#define  0x3A //	拉力及偏角动态数据上送	
#define  0x3B //	拉力及偏角动态数据上送结束标记	
#define  0x3C //	拉力及偏角动态数据补包下发	
#define  0x41 //	上传污秽数据	
#define  0x42 //	上传导线弧垂数据	
#define  0x43 //	上传电缆温度数据	
#define  0x44 //	上传电缆护层接地电流数据	
#define  0x45 //	上传故障定位数据	
#define  0x46 //	上传电缆局放数据	
#define  0x47 //	上传电缆局放谱图数据	
#define  0x60 //	主站设置故障测距终端参数	
#define  0x6A //	主站查询故障测距终端参数	
#define  0x61 //	上传故障测距终端工况数据	
#define  0x62 //	终端装置向主站请求上传工频故障波形数据	
#define  0x63 //	上传工频故障波形数据	
#define  0x64 //	工频故障波形数据上传结束标志	
#define  0x65 //	主站向终端发送工频故障波形数据补包	
#define  0x66 //	终端装置向主站请求上传故障行波波形数据	
#define  0x67 //	上传故障行波波形数据	
#define  0x68 //	故障行波波形数据上传结束标志	
#define  0x69 //	主站向终端发送行波波形数据补包	

//#define  0x以下为文件传输部分
71 //	主站查询装置文件列表	
72 //	主站请求装置上送文件	
73 //	装置请求上送文件	
74 //	文件上送	
75 //	文件上送结束标记	
76 //	文件补包数据下发	
以下为图像视频监测部分
81 //	图像采集参数配置	主站下发采集装置的色彩选择、图像宽高＋亮度＋对比度＋饱和度等信息
82 //	拍照时间表设置	设置采集装置自动拍摄照片的时间、位置
83 //	主站请求拍摄照片	在需手动请求拍摄照片时使用
84 //	采集装置请求上送照片	采集装置采集完图像数据后向主站发送上送图像数据请求，主站原命令返回
85 //	图像数据上送	上送图像拆分后的N个数据包
86 //	图像数据上送结束标记	用于主站判断图像数据是否上送完毕
87 //	补包数据下发	主站接到86H指令后，进行数据整理，将未收到的包号下发给采集装置
88 //	摄像机远程调节	上、下、左、右、远、近调节，预置位远程设置、更改
89 //	启动摄像视频传输	
8A //	终止摄像视频传输	
8B //	查询拍照时间表	
8C //	视频采集参数配置	
8D //	视频采集参数查询	
8E //	OSD参数配置	
8F //	OSD参数查询	
90 //	录像策略参数配置	
91 //	录像策略参数查询	
92 //	通道录像状态查询	
93 //	主站请求拍摄短视频	
94 //	采集终端请求上送短视频	
95 //	短视频数据上送	
96 //	短视频数据上送结束标记	
97 //	短视频补包数据下发	
98 //	主站查询终端录像文件数目	
99 //	主站查询终端录像文件列表	
9A //	主站请求进行录像文件回放	
9B //	主站请求进行录像文件回放控制	
9C //	主站请求进行录像文件回放断开	
9D //	主站请求进行录像下载	
9E //	主站请求进行录像文件下载断开	
A0 //	启动摄像视频传输	
A1 //	终止摄像视频传输	
A2 //	主站请求与终端进行语音广播	
A3 //	主站请求与终端断开语音广播	
A4 //	塔基防范参数配置	
A5 //	塔基防范参数查询	
A6 //	线路检测参数配置	
A7 //	线路检测参数查询	
A8 //	线路巡检参数配置	
A9 //	线路巡检参数查询	
AA //	智能分析告警上报	
AB //	联动参数配置	
AC //	联动参数查询	
AD //	传感器数据视频叠加参数配置	
AE //	传感器数据视频叠加参数查询	
B1 //	摄像机3D控球调节	
*/


//HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);


//void AppTaskNWS(void);
//LogFlag:调试配置，bit0
//bit7:LOG=80,打印看门狗任务的运行情况
//bit6:LOG=40,打印文件系统数据存储动作、模块数据初始化记录、
//bit5:LOG=20,打印超声波传感器调试信息
//bit4:LOG=10,打印导线温度/电流、拉力、风向、电压采样值(MCP3202)、数据信息
//bit3:LOG=08,保留
//bit2:LOG=04,测试拍照通道2
//bit1:LOG=02,测试拍照通道1
//bit0:LOG=01,打印数据采集情况

//typedef __packed struct 
//{
//	uint8_t  DtuRunStep;              //通信阶段状态
//  uint32_t DtuCheckCount;	
//	uint8_t  UpdataFlag;              //有数据需要发送标志
//	uint8_t  UpdataType;              //不同数据发送标志
//	uint8_t  SramFlag;               //SDRAM里面存储数据是否有效标志
//	uint8_t  ConnModeStatus;          //通信模块工作状态 正常:0 配置 :1
//	uint8_t  PicSendRetryCount;       //图片上传各阶段的重试次数
//	uint8_t  PicSendRetryCycle;       //图片上传各阶段的重试周期
//	uint8_t  PicSendFlag;             //图片发送过程标志 00H: 请求上传, 01H: 上传数据, 02H: 上传结束标志, 03H: 重传
//	uint32_t PicSendIndex;            //图片发送数据包号
//	uint8_t  VideoSendRetryCount;      //小视频上传各阶段的重试次数
//	uint8_t  VideoSendRetryCycle;      //小视频上传各阶段的重试周期
//	uint8_t  VideoSendFlag;            //小视频发送过程标志 00H: 请求上传, 01H: 上传数据, 02H: 上传结束标志, 03H: 重传
//	uint32_t VideoSendIndex;           //小视频发送数据包号
//	uint32_t DtuDisconnect;           //DTU通信未连次数
//	uint32_t DtuDisconnectTimes;           //DTU通信未连次数
//  uint16_t DtuRemainingTime;        //DTU在线或休眠剩余时间 单位：秒
//  uint32_t DtuRemainingTimeCount;
//	uint8_t  dtu_sleep_flag;
//	uint8_t  DtuFirstSleepFlag;
//	uint16_t DtuSubRemainingTime;     //DTU电源控制剩余时间   单位：秒
//	uint8_t  DtuPwr;	                 //DTU电源控制 1:打开; 0: 关闭
//	uint8_t  DtuWorkStatus;           //DTU工作状态 1:工作;0:休眠
//  uint8_t  UpdataChannl[3];            //需要上传的通道

//	uint16_t  Voltage;                 //蓄电池电压
//	uint32_t HeartBeatCount;
//  uint32_t HeartRemaingTime;        //心跳剩余时间          
//  uint8_t  SoftSwitchEnable;        //串口切换标志
//	uint8_t  LogFlag;                 //调试配置
//	uint8_t  RtcTime[6];              //Year,Month,Date,Hours,Minutes,Seconds
//	uint32_t RtcSubSecond;
//	uint8_t  ChannelSelect;           //相机通道选择
//	uint16_t CameraPowerResidue[2];      //相机电源控制(命令控制),电源离关闭时间,
//	uint32_t CameraPowerResidueCount[2];  //相机电源控制(命令控制),电源离关闭时间,
//	uint16_t  ModulePowerResidue;      //传感器电源控制(命令控制),电源离关闭时间,单位:30S
//	uint32_t  OversampleTime;          //模块过采样时间, 单位: 秒	
//	uint8_t  PitureStoreFlag;         //图片存储标志
//	uint8_t  PicureSearchFlag;        //图片搜索标志
//	uint8_t  SdPictureNum;
//	uint8_t  NoSignalFlag;            //没有信号，能够收到服务器返回即OK
//  uint8_t  NoSignalFlagOld;
//	uint8_t  DtuSignal;               //DTU信号强度	
//	uint8_t time_camera_schedule_count[2];  //定时拍照数记录	
//	uint8_t time_continuous_flag[2]; //摄像机连拍标?
//	uint32_t continuous_capture_prev[2];//连拍超时标志
//	uint8_t schedule_total[2];//拍照表每个时间段的拍照数量		
//	uint8_t FaultTimerFlag;	
//	uint8_t camera_preset ;
//	uint8_t camera_channel ;
//	uint32_t SD_noraml_volume;//SD卡容量
//	float display_Voltage;//电压打印信息

//	uint8_t  dtu_config_flag;

//  uint32_t UpdataRecordTime;             //历史记录发送时间间隔
//	uint8_t take_photos_type;   //拍照类型
//	uint8_t record_new_picture; //记录新的照片
//	uint8_t sampling_self_checking_flag;//自检采样
//	uint8_t Camera_waiting_time;//球机等待时间 
//	uint8_t SamplingMboxData[3];//传感器采集
//	uint8_t read_towerangle_flag;
//	uint8_t ModuleBusyFlag;
//	uint8_t SamplingStartFlag;//传感器采集标志
//	uint8_t  RainfallCounter;//雨量计数器
//	uint8_t MPSample_Start_End_Flag;//MP采集开始和停止
//	
//	uint8_t  Cruise_Serial_Num;
//	uint8_t Cruise_cmd[2];//巡航配置命令
//	uint8_t group_number[2];//巡航配置组数
//	uint8_t video_time[2];//球机拍摄时长
//	uint8_t record_new_socket_flag;//录像建立新socket标志

//	uint16_t video_time_count[2];//录像时间计算
//  uint32_t video_time_prev[2];
//	uint32_t file_num_index;//录像列表文件索引
//	uint32_t file_num_size;//录像列表文件大小
//	uint8_t  time_video_type[2];//定时录像类型
//	uint8_t  time_video_parameter[2];//定时录像参数
//	uint16_t mdat_nalu_count;//MP4 mdat 数据长度
//	uint16_t mdat_nalu_count_add;//MP4 mdat 数据长度 不完整的帧，只有开头，没有结束
//  uint32_t mdat_data_total_num;//MP4 mdat 数据大小
//	uint32_t ftyp_moov_boox_offset;//Mp4文件头的偏移量
//	uint8_t  camere_set_flag;//球机操作标志
//	uint16_t camera_live_open_time[2];//实时视频开机时间
//	uint16_t dtu_recieve_count;//DTU接收等待时间
//	
//	
//	uint8_t camera_busy_log[2];//球机忙标志	
//	uint8_t test_dayin_flag;//调试打印信息
//}SysValue;

//unsigned char Password[4]={'1','2','3','4'};//
//unsigned char Heabtime=60;//心跳间隔：装置心跳信息发送间隔，单位分钟，出厂配置应为1分钟;
//unsigned short getdatetime=20;//采集间隔：即每隔多少分钟采样一次（采集间隔与拍照时间无关），单位分钟，出厂配置应为20分钟；
//unsigned short sleeptime=0;//休眠时长：数据采集功能关闭或通信设备休眠时间，该时间内可支持短信或网络唤醒；单位分钟，若为0则装置不休眠；
//unsigned short onlinetime=10;		//在线时长：通信设备保持数据采集及网络通信设备在线时间；单位分钟；
//unsigned int resettime;	//3bye  硬件重启时间点：为保证装置软件可靠运行装置应支持定时重启。
//unsigned short checkword[4]={'1','2','3','4'};	//密文认证：4字节，装置初始为字符‘1234’（31H32H33H34H），为确认装置数据的正确性，防止非法用户恶意欺骗服务器。
////该密文用于防止非法装置用户的数据被主站认可，安装时装置设定默认密文，上塔安装完成后，
////主站下发指令修改该装置密文，仅装置主站记录的密文一致时视该数据合法有效，否则屏蔽。
#endif  /* __NW__H */

