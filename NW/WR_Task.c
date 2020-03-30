
#include "global.h"
//#include "stm32l4xx_hal_def.h"
#include "bsp.h"
//#include "nw.h"

//“DT”指令，在控制台中，正确显示系统当前时间。
//“SIG”指令，显示GPRS的信号强度(0~31)。
//“RB”指令，终端重启。
//“FE”指令，低级格式化SD卡，清除图片索引，更新文件系统
//“SS”指令，显示系统运行状态信息。
//“VER”指令，显示主板的固件版本信息。

//“CONFIGDTU”指令，例如按照“SERVERIP= 183.63.92.43”和“SERVERPORT=12677”指令设置的服务器IP和端口号重新配置DTU，需要重启生效。
//“RSDTU”指令，显示DTU复位信息，并断电重启动DTU。
//“FDS”指令，终端立即恢复出厂参数。

//#include "work.h" 
//#include "FM25V10.h"

//void ur_ctler(char *udata);
//int PTZCtrl_presets(char chanle,char presetN); 
uint8_t TC_C=0,wr_sleep;

uint8_t ctl_buf[40];

void ur_test(uint8_t *rev_data)
{
	; 
}

void TC(uint8_t *rev_data,uint8_t rlen)
{
	uint8_t buf;		//rlen,
	int i;
	
//	printf("\r\n长度 %x",rlen);
//	rlen  =strlen((char *)rev_data);
//	printf("\r\n长度 %x",rlen);
//	printf(":%x,%x",rev_data[0],rev_data[1]);
	
	if(TC_C==1)	//系统调试串口
	{
		printf("\r\nTC to ur1 %s",rev_data);
	}
	else	if(TC_C==2)//485串口 加 \r\n
	{
		strcat((char *)rev_data,"\r\n");
		
		HAL_UART_Transmit_IT(&huart2, rev_data, strlen((char *)rev_data));
		printf("\r\nTC to ur2 %s",rev_data);
	}
	else	if(TC_C==3)//SIM7600 4G 数据串口
	{
		buf=0;
			
		for(i=0;i<rlen;i++)
		{
			buf += rev_data[i];
		}
		rev_data[rlen] = (0xff-buf);
		rev_data[rlen+1]=0x16;	
		rev_data[rlen+2]=0;	
		printf("\r\nTC to ur3:");
		if(rlen!=0)	
		{
			HAL_UART_Transmit_IT(&huart3, rev_data, rlen+2);
			for(i=0;i<rlen+2;i++)	printf("%x,",rev_data[i]);
		}
	}
	else	if(TC_C==5)//SIM7600 4G 指令串口
	{
		strcat((char *)rev_data,"\r\n");
		HAL_UART_Transmit_IT(&huart5, rev_data, strlen((char *)rev_data));//, 300
		printf("\r\nTC to ur5 %s",rev_data);
	}
	else if(TC_C==6)//测试
	{
		printf("\r\nTC to ur6 %s",rev_data);
		ur_test(rev_data);
	}
	else//	if(TC_C==4)//没开
	{
		printf("\r\nTC to unkuow");
	}
}

int check_signel(void);
extern unsigned char signal;
extern unsigned char ip_seting;

int UART_Receive_s(UART_HandleTypeDef *huart,uint8_t **pack_point, uint16_t *pack_len,int time_out);
void cmd_test(uint8_t *udata,uint16_t ulen);
void data_rst(void);
void data_read(void);
	
int get_ip(uint8_t *ip_buf,uint8_t *in,uint16_t len)
{
	char i,j=0,lenout=0;
	int buf32;
	
//	if(len>255)
//	{
//		printf("\r\ncommand too long");
//		return -1;
//	}
	
	for(i=0;i<len;i++)
	{
		if(i==len-1)
		{
			i=len; //结尾
		}
		if((in[i]=='.')||(i==len))
		{
			buf32=c8_to_u32_d((uint8_t *)&in[lenout],(i-lenout));
			if((buf32<=255)&&(buf32>=0))
			{
				ip_buf[j++]=buf32;
				lenout=i+1;
				if(j>=5) 
				{
					printf("command err0");
					return -1;
				}
			}
			else
			{
				printf("\r\ncommand err1");
//				printf("\r\ncommand err1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg1123456789abcdefg");
				return -1;
			}
		}
	}
	if(j==4) return 0;
	printf("\r\ncommand err2");
	return -1;
}

//char get_port(uint32_t *port,uint8_t *in,uint16_t len)
//{
//	*port=c8_to_u32_d((uint8_t *)in,len);
//	if(*port>65535)
//	{
//		printf("command err0");
//		return -1;
//	}
//	return 0;
//}
int sd_mkfs(void);

void cmd_no_parameter(uint8_t *udata,uint16_t ulen)	//无参指令
{	
	if (0 == strncmp("DT", (char *)udata,ulen))//“DT”指令，在控制台中，正确显示系统当前时间。
	{
		printf("\r\n20%d年 %d月 %d日 %d:%d:%d",systime[0],systime[1],systime[2],systime[3],systime[4],systime[5]);
	}
	if (0 == strncmp("SIG", (char *)udata,ulen))//“SIG”指令，显示GPRS的信号强度(0~31)。
	{
		if(check_signel()==0)
			
		printf("\r\n信号强度:%d",signal);
	}
	if (0 == strncmp("RB", (char *)udata,ulen))//“RB”指令，终端重启。
	{
		printf("\r\n准备复位");
		HAL_NVIC_SystemReset();
	}
		
	if (0 == strncmp("FE", (char *)udata,ulen))//“FE”指令，低级格式化SD卡，清除图片索引，更新文件系统
	{
		printf("\r\n收到格式化指令");
		sd_mkfs();
//		printf("\r\n准备格式化sd(未完成)");
	}
	if (0 == strncmp("SS", (char *)udata,ulen))//“SS”指令，显示系统运行状态信息。
	{
		printf("\r\n未完成");
	}
	if (0 == strncmp("VER", (char *)udata,ulen))//“VER”指令，显示主板的固件版本信息。
	{
		printf("\r\n未完成");
//		printf("\r\nver:%x.%x",version[0],version[1]);
	}
	if (0 == strncmp("CONFIGDTU", (char *)udata,ulen))//“CONFIGDTU”指令，例如按照“SERVERIP= 183.63.92.43”和“SERVERPORT=12677”指令设置的服务器IP和端口号重新配置DTU，需要重启生效。
	{					
		ip_seting=1;
//		senttask_Asim |= data_flag06; 	//需复位4g驱动
		printf("\r\n准备更改IP为 %d.%d.%d.%d,%d",HostIP[0],HostIP[1],HostIP[2],HostIP[3],*HostPort);
	}
	if (0 == strncmp("RSDTU", (char *)udata,ulen))//“RSDTU”指令，显示DTU复位信息，并断电重启动DTU。
	{
		printf("\r\n未完成");
//		printf("\r\nver:%x.%x",version[0],version[1]);
	}
	
	if (0 == strncmp("FDS", (char *)udata,ulen))//“FDS”指令，终端立即恢复出厂参数。
	{
		data_rst();
		data_read();
	}
}

//1	0	打开摄像机电源
//2	所需预置点	摄像机调节到指定预置点
//3	0	向上调节1个单位
//4	0	向下调节1个单位
//5	0	向左调节1个单位
//6	0	向右调节1个单位
//7	0	焦距向远方调节1个单位（镜头变倍放大）
//8	0	焦距向近处调节1个单位
//（镜头变倍缩小）
//9	所需设置预置点	保存当前位置为某预置点
//10	关闭摄像机电源
	
//若输入PT=1，Z+/Z-，通道1的摄像机应按照设置的指令来进行镜头放大或缩小；
//若输入指令PT=1，H+/H-，通道1的摄像机应按照设置的指令进行向水平方向左转或者向右转；
//若输入指令PT=1，V+/V-，通道1的摄像机应按照设置的指令进行垂直方向向上转动或者向下转动；
//若输入指令PT=1，R+/R-，通道1的摄像机应按照设置的指令进行水平方向向左向右转动；
//PT=1，S，停止转动；
//PT=1，P1，保存预置位1；
//PT=1，D1，删除预置位1；
//PT=1,T 通道1的摄像机在当前位置拍照；
//PT=1，C1，移动到预置位1。	
void cam_ctl_on_platform(uint8_t *udata,uint16_t ulen)	//相机指令
{
	OS_ERR      err;
	uint16_t cnt_star;
	
	if(HK_post_cnt>HK_post_max) HK_post_cnt=0;
	cnt_star=HK_post_cnt;
	
	if((udata[0]=='Z')&&(udata[1]=='+')) HK_post_buf[HK_post_cnt++]=7;
	else if((udata[0]=='Z')&&(udata[1]=='-')) HK_post_buf[HK_post_cnt++]=8;		
	else if((udata[0]=='H')&&(udata[1]=='+')) HK_post_buf[HK_post_cnt++]=5;
	else if((udata[0]=='H')&&(udata[1]=='-')) HK_post_buf[HK_post_cnt++]=6;
	else if((udata[0]=='R')&&(udata[1]=='+')) HK_post_buf[HK_post_cnt++]=5;
	else if((udata[0]=='R')&&(udata[1]=='-')) HK_post_buf[HK_post_cnt++]=6;
	else if((udata[0]=='V')&&(udata[1]=='+')) HK_post_buf[HK_post_cnt++]=3;
	else if((udata[0]=='V')&&(udata[1]=='-')) HK_post_buf[HK_post_cnt++]=4;
	
	else if(udata[0]=='P')
	{
		if((udata[1]>'9')||(udata[1]<'0'))
		{
			printf("\r\n只支持 0-9 预置点");
		}
		else
		{
			HK_post_buf[HK_post_cnt++]=9;
			HK_post_buf[HK_post_cnt++]=udata[1]-'0'; //暂定0-9
		}
	}
	else if(udata[0]=='C')
	{
		if((udata[1]>'9')||(udata[1]<'0'))
		{
			printf("\r\n只支持 0-9 预置点");
		}
		else
		{
			HK_post_buf[HK_post_cnt++]=2;
			HK_post_buf[HK_post_cnt++]=udata[1]-'0'; //暂定0-9
		}
	}
	else if(udata[0]=='S')
	{
		HK_post_buf[HK_post_cnt++]=3;
	}
	else if(udata[0]=='D')
	{
		printf("\r\n不支持删除");
	}
	else if(udata[0]=='T')
	{
		HK_post_buf[HK_post_cnt++]=0x83;
		if(ulen==1) //无参
		{
			HK_post_buf[HK_post_cnt++]=0;
		}
		if(ulen==2) 
		{
			if((udata[1]>'9')||(udata[1]<'0'))
			{
				printf("\r\n只支持 0-9 预置点");
			}
			else
			{
				HK_post_buf[HK_post_cnt++]=udata[1]-'0'; //暂定0-9
			}
		}
	}

	if(cnt_star!=HK_post_cnt)	OSQPost(&hktask_Asim[0],&HK_post_buf[cnt_star],2,OS_OPT_POST_FIFO,&err);
	else printf("\r\n参数错误");
}

//int char_da_to_int_da(uint8_t *in,uint32_t *da); //从2000-1-1 0:0:0 分开始算  00年 1 月 1日 为 0

//int int_da_to_char_da(uint8_t *out,uint32_t da);

int char_time_to_int_time(uint8_t *in,uint32_t *sec); //从2000-1-1 0:0:0 分开始算

int int_time_to_char_time(uint8_t *out,uint32_t sec); //从2000-1-1 0:0:0 分开始算

int EncodeDate(u8 *in,uint32_t *Date); 

int DecodeDate(u8 * out,u32 Date); 
int flash_read(uint8_t *ADD,uint8_t *buf,int len);
void sys_time_chang(void);

void cmd_hav_parameter(uint8_t *udata,uint16_t ulen) //有参指令
{
	char i,lenout,cmdsiz;
	uint32_t buf32;
	uint8_t buf8[22];
	
//“T=”指令，设置系统时间，T=年月日时分秒（十进制）
//例如T=180522132720，在控制台中，按正确的指令格式输入，提示设置系统时间成功。		
	if (0 == strncmp("T=", (char *)udata,2))
	{
		cmdsiz=2;
		lenout=(ulen-cmdsiz+1)/2;
		if(udata[2]=='?')
		{
			printf("\r\nsystime:\t\t");
			printf("20%02d-%02d-%02d,%02d:%02d:%02d",systime[0],systime[1],systime[2],systime[3],systime[4],systime[5]);
			
			flash_read(runningtime_buf,runningtime_buf,4);
			printf("\r\n启动时间32:\r\n%02x,%02x,%02x,%02x",runningtime_buf[0],runningtime_buf[1],runningtime_buf[2],runningtime_buf[3]);
				
			printf("\r\nrunningtime:%08x\t",runningtime);
			int_time_to_char_time(buf8,runningtime);
			printf("20%02d-%02d-%02d,%02d:%02d:%02d",buf8[0],buf8[1],buf8[2],buf8[3],buf8[4],buf8[5]);
		}
		else
		{
			if(lenout != 6)
			{
				printf("\r\ncommand err");
			}
			else	if(c8_to_d8((uint8_t *)&udata[cmdsiz], (uint8_t *)udata,(ulen-cmdsiz))==0)
			{
				for(i=0;i<6;i++)
				{
					systime[i]=udata[i];
				}
				printf("\r\nsystime:");
				printf("20%02d-%02d-%02d,%02d:%02d:%02d",systime[0],systime[1],systime[2],systime[3],systime[4],systime[5]);
				
	//			char_da_to_int_da(systime,&buf32);
//				EncodeDate(systime,&buf32);
//				printf("\r\nwork da =%d",buf32);
//	//			int_da_to_char_da(buf8,buf32);
//				DecodeDate(buf8,buf32);
//				printf("\r\n sys da:20%02d-%02d-%02d",buf8[0],buf8[1],buf8[2]);
				sys_time_chang();
			}
			else
			{
				printf("\r\ncommand err");
			}
		}
	}
//“MRT=”指令，设置最大运行时间，大于0生效。
//例如设置MRT=18，显示成功信息，设置系统定时复位间隔成功。
	if (0 == strncmp("MRT=", (char *)udata,4))
	{
		cmdsiz=4;
		buf32=c8_to_u32_d((uint8_t *)&udata[cmdsiz],(ulen-cmdsiz));//strlen(&udata[cmdsiz])
		if(buf32<=255)
		{
			*OLT=buf32;
			flash_save((uint8_t *)OLT,(uint8_t *)OLT,sizeof(*OLT));	//flash_save(OLT);
			
			printf("\r\nonline time=%d",*OLT);
		}
		else printf("\r\ncommand err");
	}
//“PT=”指令，控制摄像机。
	if (0 == strncmp("PT=", (char *)udata,3))
	{
		if((udata[3]=='1')&&(ulen>=6))
		{
			cam_ctl_on_platform(&udata[5],ulen-5);
		}
		else if(udata[3]=='2')
		{
			printf("\r\n没有通道2");
		}
		else
		{
			printf("\r\n参数错误");
		}
	}
	
	if (0 == strncmp("NETBOARDIP=", (char *)udata,11))
	{					
		cmdsiz=11;
		if(get_ip(buf8,&udata[cmdsiz],ulen-cmdsiz)==0)
		{
			for(i=0;i<4;i++)	LoclIP[i]=buf8[i];						
			flash_save(LoclIP,LoclIP,4);						
			printf("\r\nLoclIP=%d.%d.%d.%d",LoclIP[0],LoclIP[1],LoclIP[2],LoclIP[3]);
		}
	}
	
	if (0 == strncmp("CAMERAIP=", (char *)udata,9))
	{
		cmdsiz=9;
		if(get_ip(buf8,&udata[cmdsiz],ulen-cmdsiz)==0)
		{
			for(i=0;i<4;i++)	CH1IP[i]=buf8[i];		
			flash_save(CH1IP,CH1IP,4);
			flash_read(CH1IP,CH1IP,4);
			printf("\r\nCH1IP=%d.%d.%d.%d",CH1IP[0],CH1IP[1],CH1IP[2],CH1IP[3]);
		}
	}	
	if (0 == strncmp("SERVERIP=", (char *)udata,9))
	{//SERVERIP=183.63.92.43
		cmdsiz=9;
		if(get_ip(buf8,&udata[cmdsiz],ulen-cmdsiz)==0)
		{
			for(i=0;i<4;i++)	HostIP[i]=buf8[i];		
			printf("\r\nSERVERIP=%d.%d.%d.%d",HostIP[0],HostIP[1],HostIP[2],HostIP[3]);
		}
	}	

//	if (0 == strncmp("SERVERPORT=", (char *)udata,11))
//	{
//		cmdsiz=11;
//		if(get_port(&buf32,&udata[cmdsiz],ulen-cmdsiz)==0)
//		{
//			*HostPort=buf32;
//			printf("\r\nSERVERPORT=%d",*HostPort);
//		}	
//		else
//		{
//			printf("\r\n指令错误:");
//			for(i=0;i<ulen;i++) printf("%c",udata[i]);
//		}
//	}
	if (0 == strncmp("SERVERPORT=",(char *) udata,11))
	{
		buf32=c8_to_u32_d((uint8_t *)(&udata[11]),ulen-11);
		if(buf32<65535)
		{
			*HostPort=buf32;
			printf("\r\nSERVERPORT=%d",*HostPort);
		}
		else
		{
			printf("\r\n指令错误:");
			for(i=0;i<ulen;i++) printf("%c",udata[i]);
		}
	}
	
//“STC=”指令， [心跳间隔(1bytes),采样间隔（2bytes），休眠时长（2bytes），在线时长（2bytes）,3c（时间单位60s）]十六进制。
//例如 STC=01000A0000000A3C，
//若配置间隔时间成功，终端应按照设置的心跳间隔时间发送心跳（默认1分钟）、采样间隔采集传感器数据（暂时没有添加）、休眠时间进行休眠（0分钟），在线时长进行运行（10分钟）。
	if (0 == strncmp("STC=", (char *)udata,strlen("STC=")))
	{
		printf("\r\n未完成");
	}
	
//	“PIC=1”指令，清除拍照表。
//例如PIC=1，按正确格式输入指令，定时拍照时间表立即清除为零,在主站服务器上查询拍照时，应该显示为无
//“PIC=2”指令，清除录像策略表。
//例如PIC=2，按正确格式输入指令，定时拍照时间表立即清除为零
	if (0 == strncmp("PIC=", (char *)udata,strlen("PIC=")))
	{
		if(udata[4]=='1')
		{
			aut_point_group1[0]=0;
			flash_save(aut_point_group1,aut_point_group1,1);
			printf("\r\n清除通道1定时拍照");
		}
		else if(udata[4]=='2')
		{
			printf("\r\n没有录像功能");
		}
		else
		{
			printf("\r\n参数错误");
		}
		
	}
	
//“TID=”指令，设置终端号码， [终端号码（6bytes）+密文验证码（4bytes）]十六进制，
//例如TID=43433030303131323334，
//前面是设备ID号，后面是密码31323334，
//按正确格式输入指令，回车换行后，提示成功信息。
	if (0 == strncmp("TID=", (char *)udata,strlen("TID=")))
	{
		if(ulen==14)
		{
			for(i=0;i<10;i++) TID[i]=udata[i+4];
			printf("\r\nTID=");
			flash_save(TID,TID,10);
			flash_read(TID,TID,10);
			for(i=0;i<10;i++) printf("%c",TID[i]); 
		}
		else printf("\r\n长度错误");
	}
//“PSW=”指令，设置终端密码 ，[终端密码（4bytes）]十六进制。
//例如PSW=31323334，
//按正确格式输入指令，提示配置终端密码成功信息。
	if (0 == strncmp("PSW=", (char *)udata,strlen("PSW=")))
	{
		if(ulen==8)
		{
			for(i=0;i<4;i++) PSW[i]=udata[i+4];
			printf("\r\nPSW=");
			flash_save(PSW,PSW,4);
			flash_read(PSW,PSW,4);
			for(i=0;i<4;i++) printf("%c",PSW[i]); 
		}
		else printf("\r\n长度错误");
	}
//“RT=”指令 ，设置重启时间，[日(2bytes)+时(2bytes)+分(bytes)]十进制，
//例如RT=000203，
//按照正确格式输入指令，提示配置终端定时重启时间点成功信息。
//若配置日为00，终端每天重启，则每天的2点3分定时重启。
	if (0 == strncmp("RT=", (char *)udata,strlen("RT=")))
	{
		printf("\r\n未完成");
	}
	
//“SV=”指令，设置电压校准值，[电压{1bytes)]十六进制}范围为90-150。
//例如 SV=6E，
//按照正确格式输入指令，提示成功信息,则电压校准值为110。

//“VP=”指令，设置球机保护电压， [电源电压(1bytes)]十进制
//例如VP=120
//按正确格式输入指令，提示成功信息，当终端电压低于设置值时，下发打开球机电源命令，控制台应出现提示，系统电压低于门槛值，无法打开球机电源，万用表测量摄像机电源电压应为零。

//“WTC=”指令， [DTU上电初始化时间（1bytes）+球机上电自检时间（1bytes），传感器数据采集时间（1bytes无）+图片传输间隔（1bytes）]十六进制，
//例如WTC=1E780A01，传图间隔为1/64秒每包
//若输入指令正确，配置各相关设备的上电延时操作时间成功。
	
//“PTS=”指令,设置云台转动和镜头伸缩的控制时间步长，[2bytes]十六进制。
//例如PTS=20，20,
//按照正确格式输入指令，提示成功信息若设置成功，当主板发出云台移动命令后，等待设置的控制时间，主板会发出云台停止移动的命令。数值调小时，球机收到后台软件下发的左转或右转命令，转动的幅度会变小，数值调大时，球机收到后台软件下发的左转或右转命令，转动的幅度会变大。
//若设置成功，主板发出镜头伸缩后，等待设置的控制时间，主板会发出镜头停止伸缩的命令。数值调小时，球机收到后台软件下发的镜头放大或缩小命令，镜头伸缩的幅度会变小，数值调大时，球机收到后台软件下发的镜头放大或缩小命令，镜头伸缩的幅度会变大。

//“POT=[xx]”指令，[1byte]十进制，
//例如POT=30，30
//按正确格式输入指令，提示成功信息。若配置成功，表示球机电源在打开的情况下，无任何操作，打开30分钟时间后，自动关闭球机电源。

//“PRC=[xx]”指令，[1byte]十进制，
//例如PRC=50，
//按正确格式输入指令，提示成功信息，则设置最大补包次数为50，超过补包次数，就会将图片存储到SD，等待信号好时再上传。

//“DRC=[xx]”指令，[1byte] 十六进制，大于0生效。
//例如DRC=05，
//按正确格式输入指令，提示成功信息，则设置发送5条心跳指令之后未收到主站回复就重启DTU。 
   
//“PMWN=[xx]”指令，[1byte]十进制，
//例如PMWN=200，
//SD卡最大存储200张图片，信号不好上传不了的图片、休眠期间拍照的图片、上传错误的图片，就会存储到SD内。

//“CVD=[xx,xx]”指令，[1byte，1byte]十进制，通道号，拍短视频时间
//例如CVD=1，30
//球机通道1，短视频30秒。

//“PHOTOSIZE=[xx,xx]”指令，[1byte,1byte]
//例如PHOTOSIZE=7,7
//代表通道1和通道2的图片大小为1280*720.其他数值参考南方规约。（球机只支持1280*720=>7、1920*1080=>8、1280*960=>10)
//“CNT=5”是指设备上线时，发送联络5次后，设备未收到响应，则设备跳过00H，发送01H（不再发00H），若设备发送01H，发送5次后也未收到响应，则跳过校时，发送心跳05H。

//“CSDF=[xx]”指令，[1byte]
//例如：CSDF=1，代表格式化球机的SD卡
//“MCFG=[xx]”指令，气象功能配置指令
//b0-WD,b1-SD,b2-FS,b3-FX,b4-QY,b5-RZ,in HEX (1 bytes),bit = 0:无；bit=1：有。
//bit 5:日照
//bit 4:气压
//bit 3:风速
//bit 2:风向
//bit 1:湿度
//bit 0:气温
//例如：需要配置所有功能生效，MCFG=FF

//“WCFG=[XXXXXXXX]”指令[4byte]，功能配置指令，1代表有，0代表无
	
//PWR=1(打开DTU)	PWR=2（关闭DTU)
//PWR=3(打开球机1）	PWR=4(关闭球机1）
//PWR=5（打开球机2）	PWR=6（关闭球机2）
//PWR=9（打开路由）	PWR=A(关闭路由）
//PWR=B(打开以太网）	PWR=C(关闭以太网）
if (0 == strncmp("PWR=", (char *)udata,strlen("PWR=")))
{
	if(udata[4]=='1') {PWR_W_DTU(1);printf("\r\n打开DTU");}
	else if(udata[4]=='2') {PWR_W_DTU(0);printf("\r\n关闭DTU");}
	else if(udata[4]=='3') {PWR_W_CAM1(1);printf("\r\n打开CAMERA1");}
	else if(udata[4]=='4') {PWR_W_CAM1(0);printf("\r\n关闭CAMERA1");}
	else if(udata[4]=='5') {PWR_W_CAM2(1);printf("\r\n打开CAMERA2");}
	else if(udata[4]=='6') {PWR_W_CAM2(0);printf("\r\n关闭CAMERA2");}
	
	else if(udata[4]=='9') printf("\r\n没有路由");
	else if(udata[4]=='A') printf("\r\n没有路由");
	
	else if(udata[4]=='B') {PWR_W_ETH(1);printf("\r\n打开以太网");}
	else if(udata[4]=='C') {PWR_W_ETH(0);printf("\r\n关闭以太网");}
}

//“WPC=[xx]”指令，[1 byte]
//WPC=1，12V微气象传感器电源常开

//“RCS=[xx]”指令,[1 byte]
//例如：RCS=20,雨量的校准系数，默认为20，代表0.2mm

//“SRP=[xx,xx]”，[1 byte,1 byte]
//例如：SRP=2，80  2代表类型为气象，80代表气象回滚到已上送80条

//“IOUT=[1~6]
//1：雨量1开；2：雨量1关；3：雨量2开；4：雨量2关；5：雨量3开；6雨量3关

//UDB=[0~6],{n bytes in HEX} 
//例如：UDB=0,123.向控制台串口发送123数据，按原数据返回123

	
}

extern uint8_t sleepdelay;

void WR_Task( void * p_arg )
{
	OS_ERR      err;
	(void)p_arg;
	uint8_t *udata;
	uint16_t ulen;
	

	while (DEF_TRUE)
	{
		OSTimeDly ( 100, OS_OPT_TIME_DLY, & err ); 
		if(UART_Receive_s(&huart1,&udata, &ulen,1000)==2)
		{
//			printf("\r\nulen=%d",ulen);
//			printf("\r\nrev_data=%s",udata);
			sleepdelay=6;
			
			if((strncmp((char *)udata, "uart", 4)==0)&&(ulen>=5))
			{
				TC_C=udata[4]-'0';
				if(ulen>6) ulen=ulen-6;
				else ulen=0;
				TC(&udata[6],ulen);
			}
			else if(TC_C !=0)
			{
				TC(udata,ulen);
			}
			else
			{
				if((ulen>=2)&&(ulen<255))
				{
					if((udata[ulen-2]=='\r')&&(udata[ulen-1]=='\n')&&(ulen>=4))
					{
						ulen -= 2;
					}
					cmd_test(udata,ulen);
					cmd_no_parameter(udata,ulen);
					cmd_hav_parameter(udata,ulen);
				}
			}
		}
//		else
//		{
//			if(((huart1.Instance->CR1)&(USART_CR1_PEIE | USART_CR1_RXNEIE)) != (USART_CR1_PEIE | USART_CR1_RXNEIE))
//			{
//				printf("\r\nhuart1------------------------terrorflags");
//				HAL_UART_Abort_IT(&huart1);
//				HAL_UART_Receive_IT(&huart1,(unsigned char *)urd1,lenmax1);
//			}
//		}
	}
}

#define		WR_Task_SIZE                     512
#define		WR_Task_PRIO                     9
OS_TCB		WR_Task_TCB;															//任务控制块
CPU_STK		WR_Task_Stk [ WR_Task_SIZE ];	//任务堆栈

void WRTaskStart (void *p_arg)
{
	OS_ERR      err;
	(void)p_arg;

	
	

#if 1
						 
		/* 创建  开机联络 任务 */
    OSTaskCreate((OS_TCB     *)&WR_Task_TCB,                             //任务控制块地址
                 (CPU_CHAR   *)"WR_Task_Name",                             //任务名称
                 (OS_TASK_PTR ) WR_Task,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) WR_Task_PRIO,                         //任务的优先级
                 (CPU_STK    *)&WR_Task_Stk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) WR_Task_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) WR_Task_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型
#endif	
								 

}
int read_data_ph(uint16_t rcnt,uint8_t *out);
int index_ph_dec(unsigned char *index);
void sent_HeartBeat(void);
extern uint8_t Sphoto_step;


void cmd_test(uint8_t *udata,uint16_t ulen)
{
	OS_ERR      err;
//	char aaa[3];
	
	if (0 == strncmp("UR3 RST0", (char *)udata,8))
	{
		printf("\r\nur3复位0");
		HAL_UART_Abort_IT(&huart3);
		MX_USART3_UART_Init();
		HAL_UART_Receive_IT(&huart3,(unsigned char *)urd3,lenmax3);
		huart3.GetPoint=0;//huart->NowPoint;
		huart3.ReadPoint=0;//=huart->NowPoint;
		huart3.NowPoint=0;
	}
	if (0 == strncmp("UR3 RST1", (char *)udata,8))
	{
		printf("\r\nur3复位1");
		huart3.GetPoint=0;//huart->NowPoint;
		huart3.ReadPoint=0;//=huart->NowPoint;
		huart3.NowPoint=0;
	}
	if (0 == strncmp("UR3 RST2", (char *)udata,8))
	{
		printf("\r\nur3复位2");
		HAL_UART_Receive_IT(&huart3,(unsigned char *)urd3,lenmax3);
	}
	if (0 == strncmp("UR3 RST3", (char *)udata,8))
	{
		printf("\r\nur3复位3");
		HAL_UART_Abort_IT(&huart3);
		HAL_UART_Receive_IT(&huart3,(unsigned char *)urd3,lenmax3);
	}
	if (0 == strncmp("UR3 RST4", (char *)udata,8))
	{
		printf("\r\nur3复位4");
		MX_USART3_UART_Init();
		HAL_UART_Receive_IT(&huart3,(unsigned char *)urd3,lenmax3);
	}
	if (0 == strncmp("DTU RST", (char *)udata,7))
	{
		printf("\r\nDTU复位");
		DTU_reset();
	}
	if (0 == strncmp("UR3 DTU RST", (char *)udata,11))
	{
		printf("\r\nUR3DTU复位");
		DTU_reset();
		HAL_UART_Abort_IT(&huart3);
		MX_USART3_UART_Init();
		HAL_UART_Receive_IT(&huart3,(unsigned char *)urd3,lenmax3);
		huart3.GetPoint=0;//huart->NowPoint;
		huart3.ReadPoint=0;//=huart->NowPoint;
		huart3.NowPoint=0;
	}
	
	if (0 == strncmp("CDBF", (char *)udata,4))
	{
		data_read();
	}
	if (0 == strncmp("delete ph", (char *)udata,9))
	{
		if(read_data_ph(ph_sent_cnt,ctl_buf)==0)	 
		{
			Sphoto_step=0;
			index_ph_dec(ctl_buf); //跳过此包
//			printf("\r\nwr0");
		}
//			printf("\r\nwr1");
	}
	if (0 == strncmp("sleep", (char *)udata,5))
	{
		wr_sleep=1;
//			printf("\r\nwr1");
	}
	if (0 == strncmp("heart", (char *)udata,5))
	{
		sent_HeartBeat();
	}
	
	if(0 == strncmp("task", (char *)udata,4))
	{
		extern OS_TCB   TaskDSP_TCB;			//任务控制块
		extern OS_MEM  mem; 
		char *   p_mem_blk;
		
		
//		OSTaskSemPost((OS_TCB  *)&TaskDSP_TCB,   //目标任务
//									(OS_OPT   )OS_OPT_POST_NONE, //没选项要求
//									(OS_ERR  *)&err);            //返回错误类型	
		if((udata[4]>='0')&&(udata[4]<='9'))
		{
			/* 从内存分区 mem 获取一个内存块 */
			p_mem_blk = OSMemGet((OS_MEM      *)&mem,
										   (OS_ERR      *)&err);
			
			* p_mem_blk = udata[4]-'0';
			
			OSTaskQPost ((OS_TCB      *)&TaskDSP_TCB,      //目标任务的控制块
									 (void        *)p_mem_blk,             //消息内容的首地址
									 (OS_MSG_SIZE  )1,                     //消息长度
									 (OS_OPT       )OS_OPT_POST_FIFO,      //发布到任务消息队列的入口端
									 (OS_ERR      *)&err);                 //返回错误类型
		}
	}
	
		if(0 == strncmp("updata", (char *)udata,strlen("updata")))
		{
			updata_flag[0]=0xaa;
			HAL_NVIC_SystemReset(); 
		}
	
	
//	if (0 == strncmp("heart2", (char *)udata,5))
//	{
//		struct nw_ctl abc;
//			
//		senttask_Pend |= data_flag05;
//		aaa.cmd=HeartBeat;
//		aaa.len=8;
//		memcpy(aaa.data,systime,6);
//		
//		if(check_signel()==0) aaa.data[6]=signal;
//		else aaa.data[6]=0;

//		if(Get_Volt()==0) nw_sc.data[7]=volt;
//		else nw_sc.data[7]=0;

//		nw_sent_st(&nw_sc);
//	}
//	printf("\r\nwr2");
}


