
#include "main.h"
#include "lwip.h"
#include <stdio.h>
#include <string.h>

#include "lwip/timeouts.h"
#include "netif/etharp.h"
#include "ethernetif.h"
#include "MD5.h"
#include "fatfs.h"
//#include "camera.h"

#define ctl_step_time	1000

extern unsigned int testtime;

extern char method[10];
extern char uri[120];
//extern char ip_host[20]
extern char strA[100];
extern char body[500];

extern int sent_to_camera(char *sentbuf,char *method,char *uri,char *str,char *body);//const uint8_t *str,
extern uint8_t hk_power_delay,http_link;

void get_digest_info(char *in);
extern int Content_Length(char **in);
	
OS_SEM http_rev_sem,http_200_sem,http_401_sem,http_data_sem;
//extern uint8_t cnt,r200,step,Keep_Alive_Time;
//extern uint8_t havcmd,cmdcnt,rnumb,cnumb;
//extern uint32_t datalen_buf,datalen_r;

#define WaitingImage			1
#define WaitingStreaming	2

char waiting_data,HTTP_GET;
uint32_t data_len,data_rlen,data_pack_nb;

void dm9k_reset(void);
int get_color_info(char *in);
int get_Width_Height_info(char *in);

extern uint8_t preset_now[2];

void http_data_post(char * in,uint16_t len)
{
	int i;
	OS_ERR      err;
	char *end;
	

	if(strncmp("HTTP/1.1 401", (char *)in,12)==0)
	{
//		printf("收到\r\n%s\r\n",in); //收到数据http
		printf("\r\nHTTP/1.1 401"); //收到数据http
		get_digest_info(in);
		sent_to_camera((char *)http_client_sendbuf,method,uri,strA,body); //更新认证信息重发
	}
	else	if(strncmp("HTTP/1.1 200 OK", (char *)in,15)==0) //数据OK
	{
		  //处理接收到数据帧 
//		printf("收到\r\n%s\r\n",in); //收到数据http
		printf("\r\nHTTP/1.1 200 OK"); //收到数据http
		end=in+15;
		data_len=Content_Length(&end);
		
		if(HTTP_GET==WaitingImage)
		{
			if(get_color_info(end)==0)
			{
				HTTP_GET=0;
			}
		}
		if(HTTP_GET==WaitingStreaming)
		{
			if(get_Width_Height_info(end)==0)
			{
				HTTP_GET=0;
			}
		}
		
		OSSemPost (&http_200_sem, OS_OPT_POST_ALL, &err);  

		
//		printf("等待接收长度=%d",data_len);	
		data_rlen=0;
		if(data_len>0)//(strlen(getbody)-4))  //暂时以此为拍照判断
		{
			PH_flag[0]=0;
			PH_LEN[0]=0;
			PH_LEN[1]=0;
			waiting_data=1;
			data_pack_nb=0;
			
			memcpy(PH_GET_T,systime,6);
			*PH_GET_CH=1;
			*PH_GET_PS=preset_now[0];
			
//#define PH_GET_T		(( uint8_t*) (PH_flag+1))
//#define PH_GET_CH		(( uint8_t*) (PH_GET_T+6))
//#define PH_GET_PS		(( uint8_t*) (PH_GET_CH+1))
//#define PH_LEN			(( uint16_t*) (PH_GET_PS+1))
		//拍照时间	通道号	预置位号	包数高位	包数低位
		//6字节	1字节	1字节	1字节	1字节
		}
	}
	else if(waiting_data==1)
	{	
//		printf("\r\nph_rev,peket=%d,len=%d,data=",data_pack_nb++,len);	
//		for(i=0;i<5;i++)	printf("%02x",in[i]); printf("..."); for(i=len-5;i<len;i++)	printf("%02x",in[i]); //打印前后几个		
		
		memcpy(PH_DATA+data_rlen,in,len);
		data_rlen +=len;
		if(data_rlen>=data_len)
		{
			printf("\r\nphoto data rev complete len=%d",data_rlen);	
			waiting_data=0;	
			if(data_rlen==data_len)	PH_flag[0]=1; //图片数据完整
			PH_LEN[0]=(data_len>>16);
			PH_LEN[1]=data_len;
			OSSemPost (&http_data_sem, OS_OPT_POST_ALL, &err);    //处理接收到数据帧 
		}
	}
	else
	{
		printf("收到\r\n%s\r\n",in); //收到数据http
	}

	
}


//标准头？
//PUT /PTZ/channels/1/PTZControl?command=&mode=start&speed=3 HTTP/1.1
//Host: 192.168.1.11
//Connection: Keep-Alive
//Authorization: Digest username="admin",realm="DS-2DF5130W-D",qop="auth",uri="/PTZ/channels/1/PTZControl?command=&mode=start&speed=3",nonce="4d6a4530516a4d784e447069593252695a6a5a6c4f513d3d",nc=0000001E,cnonce="181C7E12ACF1206BCBE43C26B0335FF8",response="412657c9ee76dd0a16dc6e2ee8245ed2"

//PUT /PTZCtrl/channels/1/continuous HTTP/1.1
//Host: 172.8.11.101
//Content-Type: text/xml
//Authorization: Basic YWRtaW46MTIzNDU=
//Content-Length: 95

//<?xml version="1.0" encoding="UTF-8"?>	//38
//<PTZData>						//47
//<pan>-60</pan>			//61
//<tilt>0</tilt>			//75
//</PTZData>					//85

//<?xml version="1.0" encoding="UTF-8"?>
//<PTZPreset>
//<id>1</id>
//<enabled>true</enabled>
//<presetName>Preset-1</presetName>
//</PTZPreset>


int sent_to_camera(char *sentbuf,char *method,char *uri,char *str,char *body);

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





int PTZCtrl_continuous(char chanle,signed char *x,signed char *y,signed char *z)//Pan_Tilt_Zoom
{
	OS_ERR      err;
//	char buf[23];
	int len_strA,len_body;
	
	
//	len_total=0;
//	len_total +=sprintf(&sttp_buf[len_total],"%s ",method);
	
	
	strcpy(method,"PUT");	
	sprintf(uri,"/PTZCtrl/channels/%d/continuous",chanle);
	
	len_strA = sprintf(strA,"Content-Type: text/xml\r\n");
	len_strA += sprintf(&strA[len_strA],"Content-Length: ");
	
	len_body = sprintf(body,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
	len_body += sprintf(&body[len_body],"<PTZData>\r\n");

	if(x!=NULL) len_body += sprintf(&body[len_body],"<pan>%d</pan>\r\n",*x);	
	if(y!=NULL) len_body += sprintf(&body[len_body],"<tilt>%d</tilt>\r\n",*y);
	if(z!=NULL) len_body += sprintf(&body[len_body],"<zoom>%d</zoom>\r\n",*z);

	len_body += sprintf(&body[len_body],"</PTZData>\r\n");
	
	sprintf(&strA[len_strA],"%d\r\n",len_body);
	
	sent_to_camera((char *)http_client_sendbuf,method,uri,strA,body);
	
	//								多值信号量指针	//等待超时时间	//选项 //等到信号量时的时间戳 //返回错误类型
	OSSemPend (&http_200_sem, 3000, OS_OPT_PEND_BLOCKING,0, &err); 

	return err;
}

int PTZCtrl_continuous_s(char chanle,signed char *x,signed char *y,signed char *z)//Pan_Tilt_Zoom
{
	OS_ERR      err;
	char times;
	
	if(power1_camera1==0)
	{
		printf("\r\n球机未上电");
		return -3;
	}
//	(void)chanle;//暂时不管通道
	chanle=1;

	testtime=0;
	times=0;
	while((PTZCtrl_continuous(chanle,x,y,z) != 0)&&(times++<6))
	{
		printf("\r\n没收到200 NG1");
	}
	if(times>6)
	{
		dm9k_reset();
		return -1;
	}
	

	OSTimeDly ( ctl_step_time, OS_OPT_TIME_DLY, & err ); 
	
	if(x != NULL) *x=0;
	if(y != NULL) *y=0;
	if(z != NULL) *z=0;
	
	testtime=0;
	times=0;
	while((PTZCtrl_continuous(chanle,x,y,z) != 0)&&(times++<6))
	{
		printf("\r\n没收到200 NG2");
	}
	if(times>6)
	{
		dm9k_reset();
		return -1;
	}
	
	return 0;
}
	
//<?xml version="1.0" encoding="UTF-8"?>
//<PTZPreset>
//<id>1</id>
//<enabled>true</enabled>
//<presetName>Preset-1</presetName>
//</PTZPreset>
int PTZCtrl_presets(char chanle,char presetN)
{
	OS_ERR      err;
	int len_strA,len_body;
	
	if(power1_camera1==0)
	{
		printf("\r\n球机未上电");
		return -3;
	}
	
	strcpy(method,"PUT");
	sprintf(uri,"/PTZCtrl/channels/%d/presets/%d",chanle,presetN);
	
	len_strA = sprintf(strA,"Content-Type: text/xml\r\n");
	len_strA += sprintf(&strA[len_strA],"Content-Length: ");
		
	len_body = sprintf(body,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
	len_body += sprintf(&body[len_body],"<PTZPreset>\r\n");
	
//	len_body += sprintf(&body[len_body],"<PTZPreset>\r\n");
	
	len_body += sprintf(&body[len_body],"<id>%d</id>\r\n",presetN);
	len_body += sprintf(&body[len_body],"<enabled>true</enabled>\r\n");
	len_body += sprintf(&body[len_body],"<presetName>Preset-%d</presetName>\r\n",presetN);
	
	len_body += sprintf(&body[len_body],"</PTZPreset>\r\n");
	
	sprintf(&strA[len_strA],"%d\r\n",len_body);
		
	sent_to_camera((char *)http_client_sendbuf,method,uri,strA,body);
		
	//								多值信号量指针	//等待超时时间	//选项 //等到信号量时的时间戳 //返回错误类型
	OSSemPend (&http_200_sem, 3000, OS_OPT_PEND_BLOCKING,0, &err); 

	return err;
}

int PTZCtrl_presets_s(char chanle,char presetN)
{
	OS_ERR      err;
	char times;
	
	if(power1_camera1==0)
	{
		printf("\r\n球机未上电");
		return -3;
	}
	
	times=0;
	
	while((PTZCtrl_presets(chanle,presetN) != 0)&&(times++<6))
	{
		printf("\r\n没收到200 NG3");
	}
	if(times>6)
	{
//		dm9k_reset();
		return -1;
	}

	return 0;
}

int PTZCtrl_presets_goto(char chanle,char presetN)
{
	OS_ERR      err;
	
//	PUT /PTZCtrl/channels/1/presets/1/goto HTTP/1.1
//Authorization: Basic YWRtaW46MTIzNDU=
//Host: 172.8.11.101
	strcpy(method,"PUT");
	sprintf(uri,"/PTZCtrl/channels/%d/presets/%d/goto",chanle,presetN);

	strcpy(strA,"\0");
	strcpy(body,"\0");
	sent_to_camera((char *)http_client_sendbuf,method,uri,strA,body);
	
	//								多值信号量指针	//等待超时时间	//选项 //等到信号量时的时间戳 //返回错误类型
	OSSemPend (&http_200_sem, 3000, OS_OPT_PEND_BLOCKING,0, &err); 

	return err;
}

int PTZCtrl_presets_goto_s(char chanle,char presetN)
{
	OS_ERR      err;
	char times;
	
	if(power1_camera1==0)
	{
		printf("\r\n球机未上电");
		return -3;
	}
	
	times=0;
	
	while((PTZCtrl_presets_goto(chanle,presetN) != 0)&&(times++<6))
	{
		printf("\r\n没收到200 NG3");
	}
	if(times>6)
	{
//		dm9k_reset();
		return -1;
	}
	
	return 0;
}



//PAN_LEFT ：水平向左转动
//PAN_RIGHT：水平向右转动
//TILT_UP：垂直向上运动
//TILT_DOWN：垂直向下运动
//ZOOM_IN：镜头拉远
//ZOOM_OUT：镜头拉近
//UP_LEFT: 左上转向运动
//UP_RIGHT: 右上转向运动
//DOWN_LEFT: 左下转向运动
//DOWN_RIGHT: 右下转向运动
//PAN_AUTO: 自动巡航
//mode的选项有两个：start和stop
//speed指示的是云台转向速度：速度的最小值是1，最大值为7

int PTZ(char *command,char *mode,char speed,int waitime)//,char *mode,char speed
{
	OS_ERR      err;

	if((speed<1)||(speed>7)) return -1;

	strcpy((char *)method,"PUT");
	sprintf(uri,"/PTZ/channels/1/PTZControl?command=%s&mode=%s&speed=%d",command,mode,speed);
	strcpy((char *)strA,"\0");
	strcpy((char *)body,"\0");
	sent_to_camera((char *)http_client_sendbuf,method,uri,strA,body);

//								多值信号量指针	//等待超时时间	//选项 //等到信号量时的时间戳 //返回错误类型
	OSSemPend (&http_200_sem, waitime, OS_OPT_PEND_BLOCKING,0, &err); 

	return err;
}

int PUT_PTZ_s(char chanle,const char *command)
{
	OS_ERR      err;
	char speed=2;
	char times;
	
	if(power1_camera1==0)
	{
		printf("\r\n球机未上电");
		return -3;
	}
	
	(void)chanle;//暂时不管通道

	testtime=0;
	times=0;
	while((PTZ((char *)command,"start",speed,3000) != 0)&&(times++<6))
	{
		printf("\r\n没收到200 NG3");
	}
	if(times>6)
	{
		dm9k_reset();
		return -1;
	}

	OSTimeDly ( ctl_step_time, OS_OPT_TIME_DLY, & err ); 
	
	testtime=0;
	times=0;
	while((PTZ((char *)command,"stop",speed,1000) != 0)&&(times++<6))
	{
		printf("\r\nstop没收到200");
	}
	if(times>6)
	{
		dm9k_reset();
		return -1;
	}
	return 0;
}



int get_photo(void)
{
	OS_ERR      err;
	
	if(power1_camera1==0)
	{
		printf("\r\n球机未上电");
		return -3;
	}
	strcpy((char *)method,"GET");
	strcpy((char *)uri,"/onvif-http/snapshot?Profile_1");//strcpy((char *)uri,"/PTZCtrl/channels/1/presets/1/goto");//strcpy((char *)uri,"/onvif-http/snapshot?Profile_1");
	strcpy((char *)strA,"Accept: image/jpeg\r\n");//strcpy((char *)str,"\0");//str[0]=0;//	image/jpeg
	strcpy((char *)body,"\0");
	
	sent_to_camera((char *)http_client_sendbuf,method,uri,strA,body);	
	
//								多值信号量指针	//等待超时时间	//选项 //等到信号量时的时间戳 //返回错误类型
	OSSemPend (&http_200_sem, 3000, OS_OPT_PEND_BLOCKING,0, &err); 
	if(err!=OS_ERR_NONE)
	{
		printf("\r\n没收到200");
		return -1;
	}

//	printf("\r\n等待http_data_sem信号量");
	OSSemPend (&http_data_sem, 5000, OS_OPT_PEND_BLOCKING,0, &err); 
	if(err!=OS_ERR_NONE)
	{
		printf("\r\nget_photo没收完整数据");
		return -1;
	}
	else
	{
//		printf("\r\nget_photo收到数据");
	}
	

	return 0;
	
////多值信号量控制块指针	//选项	 //返回错误类型
//			OSSemPost (&http_rev_sem, OS_OPT_POST_NONE, &err);    //处理接收到数据帧 
}
int get_photo_s(void)
{
	if(power1_camera1==0)
	{
		printf("\r\n球机未上电");
		return -3;
	}
	
	if(get_photo()!=0)
	{
		if(get_photo()!=0)
		{
			dm9k_reset();
			return -1;
		}
	}
	
	return 0;
}

int get_color_info(char *in);
extern unsigned char bLevel,cLevel,sLevel;

int GET_Image(void)//Pan_Tilt_Zoom
{
	OS_ERR      err;
//	char buf[23];
//	int len_strA,len_body;
	
	strcpy(method,"GET");	
	sprintf(uri,"/Image/channels/1/Color");
	
	strcpy((char *)strA,"\0");
//	strcpy((char *)body,"\0");
	
	sent_to_camera((char *)http_client_sendbuf,method,uri,strA,0);
	
	
//								多值信号量指针	//等待超时时间	//选项 //等到信号量时的时间戳 //返回错误类型
	OSSemPend (&http_200_sem, 3000, OS_OPT_PEND_BLOCKING,0, &err); 

	return err;
}

int GET_Image_s(void)//char chanle,signed char *brightnessLevel,signed char *contrastLevel,signed char *saturationLevel
{
//	OS_ERR      err;
	char times;
	
	if(power1_camera1==0)
	{
		printf("\r\n球机未上电");
		return -3;
	}

	testtime=0;
	times=0;
	
	HTTP_GET=WaitingImage;
	while((GET_Image() != 0)&&(times++<6))//||(HTTP_GET==WaitingImage)
	{
		printf("\r\n没收到200 NG4");
	}
	if(times>6)
	{
		dm9k_reset();
		return -1;
	}
	if(HTTP_GET!=0)
	{	
		HTTP_GET=0;
		return -2;
	}
	return 0;
}


//PUT /Image/channels/1/Color HTTP/1.1
//Authorization: Basic YWRtaW46MTIzNDU=
//Host:172.8.6.228
//Content-Type:text/xml
//Content-Length:234 

//<?xml version="1.0" encoding="UTF-8"?>
//<Color version="1.0" xmlns="http://www.std-cgi.com/ver10/XMLSchema">
//<brightnessLevel>30</brightnessLevel>
//<contrastLevel>50</contrastLevel>
//<saturationLevel>100</saturationLevel>
//</Color>

int PTZ_Image(char chanle,signed char brightnessLevel,signed char contrastLevel,signed char saturationLevel)//Pan_Tilt_Zoom
{
	OS_ERR      err;
//	char buf[23];
	int len_strA,len_body;
	
	strcpy(method,"PUT");	
	sprintf(uri,"/Image/channels/%d/Color",chanle);
	
	len_strA = sprintf(strA,"Content-Type: text/xml\r\n");
	len_strA += sprintf(&strA[len_strA],"Content-Length: ");
	
	len_body = sprintf(body,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
	len_body += sprintf(&body[len_body],"<Color version=\"1.0\" xmlns=\"http://www.std-cgi.com/ver10/XMLSchema\">\r\n");

	len_body += sprintf(&body[len_body],"<brightnessLevel>%d</brightnessLevel>\r\n",brightnessLevel);	
	len_body += sprintf(&body[len_body],"<contrastLevel>%d</contrastLevel>\r\n",contrastLevel);
	len_body += sprintf(&body[len_body],"<saturationLevel>%d</saturationLevel>\r\n",saturationLevel);

	len_body += sprintf(&body[len_body],"</Color>\r\n");
	
	sprintf(&strA[len_strA],"%d\r\n",len_body);
	
	sent_to_camera((char *)http_client_sendbuf,method,uri,strA,body);
	
	//								多值信号量指针	//等待超时时间	//选项 //等到信号量时的时间戳 //返回错误类型
	OSSemPend (&http_200_sem, 3000, OS_OPT_PEND_BLOCKING,0, &err); 

	return err;
}

int PTZ_Image_s(signed char brightnessLevel,signed char contrastLevel,signed char saturationLevel)//Pan_Tilt_Zoom
{
//	OS_ERR      err;
	char times,chanle;
	
	if(power1_camera1==0)
	{
		printf("\r\n球机未上电");
		return -3;
	}
	
//	(void)chanle;//暂时不管通道
	chanle=1;

	testtime=0;
	times=0;
	while((PTZ_Image(chanle,brightnessLevel,contrastLevel,saturationLevel) != 0)&&(times++<6))
	{
		printf("\r\n没收到200 NG5");
	}
	if(times>6)
	{
//		dm9k_reset();
		return -1;
	}
	
	return 0;
}

int GET_Streaming_capabilities(void)//Pan_Tilt_Zoom
{
	OS_ERR      err;
//	char buf[23];
//	int len_strA,len_body;
	
	strcpy(method,"GET");	
	sprintf(uri,"/Streaming/channels/1/capabilities");
	
	strcpy((char *)strA,"\0");
//	strcpy((char *)body,"\0");
	
	sent_to_camera((char *)http_client_sendbuf,method,uri,strA,0);
	
	
//								多值信号量指针	//等待超时时间	//选项 //等到信号量时的时间戳 //返回错误类型
	OSSemPend (&http_200_sem, 3000, OS_OPT_PEND_BLOCKING,0, &err); 

	return err;
}

int GET_Streaming(void)//Pan_Tilt_Zoom
{
	OS_ERR      err;
//	char buf[23];
//	int len_strA,len_body;
	
	strcpy(method,"GET");	
	sprintf(uri,"/Streaming/channels/1");
	
	strcpy((char *)strA,"\0");
//	strcpy((char *)body,"\0");
	
	sent_to_camera((char *)http_client_sendbuf,method,uri,strA,0);
	
	
//								多值信号量指针	//等待超时时间	//选项 //等到信号量时的时间戳 //返回错误类型
	OSSemPend (&http_200_sem, 3000, OS_OPT_PEND_BLOCKING,0, &err); 

	return err;
}
int GET_Streaming_s(void)//char chanle,signed char *brightnessLevel,signed char *contrastLevel,signed char *saturationLevel
{
//	OS_ERR      err;
	char times;
	
	if(power1_camera1==0)
	{
		printf("\r\n球机未上电");
		return -3;
	}

	testtime=0;
	times=0;
	
	HTTP_GET=WaitingStreaming;
	while(((GET_Streaming() != 0))&&(times++<6))//||(HTTP_GET==WaitingStreaming)
	{
		printf("\r\n没收到200 NG6");
	}
	if(times>6)
	{
		dm9k_reset();
		return -1;
	}
	
	if(HTTP_GET!=0)//WaitingStreaming
	{	
		HTTP_GET=0;
		return -2;
	}
	
	return 0;
}


const uint16_t Width[21]= {0,320,640,704,800,1024,1280,1280,1920,960,1280,1600,2048,2592,2592,3072,3840,4000,4608,3200,4224};
const uint16_t Height[21]={0,240,480,576,600,768, 1024,720, 1080,576,960, 1200,1536,1520,1944,2048,2160,3000,3456,2400,3136};
	
int PTZ_Streaming(uint8_t hWidthxHeight)//Pan_Tilt_Zoom
{
	OS_ERR      err;
//	char buf[23];
	int len_strA,len_body;
	
	if((hWidthxHeight>20)||(hWidthxHeight==0)) return -2;

	strcpy(method,"PUT");	
	sprintf(uri,"/Streaming/channels/1");
	
	len_strA = sprintf(strA,"Content-Type: text/xml\r\n");
	len_strA += sprintf(&strA[len_strA],"Content-Length: ");

//<?xml version="1.0" encoding="UTF-8"?>
//<StreamingChannel version="1.0" xmlns="http://www.hikvision.com/ver10/XMLSchema">
//<id>1</id>
//<enabled>true</enabled>
//<Transport>
//<rtspPortNo>554</rtspPortNo>
//</Transport>
//<Video>
//<videoInputChannelID>1</videoInputChannelID>
//<videoResolutionWidth>640</videoResolutionWidth>
//<videoResolutionHeight>480</videoResolutionHeight>
//<videoQualityControlType>CBR</videoQualityControlType>
//<constantBitRate>2048</constantBitRate>
//<maxFrameRate>2500</maxFrameRate>
//</Video>
//</StreamingChannel>
	
//<videoResolutionWidth>，<videoResolutionHeight>，<videoQualityControlType>，<maxFrameRate>	四个标签必须包含
	len_body = sprintf(body,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
	len_body += sprintf(&body[len_body],"<StreamingChannel version=\"1.0\" xmlns=\"http://www.hikvision.com/ver10/XMLSchema\">\r\n");
	len_body += sprintf(&body[len_body],"<id>1</id>\r\n");	
	len_body += sprintf(&body[len_body],"<enabled>true</enabled>\r\n");
	len_body += sprintf(&body[len_body],"<Transport>\r\n");
	len_body += sprintf(&body[len_body],"<rtspPortNo>554</rtspPortNo>\r\n");
	len_body += sprintf(&body[len_body],"</Transport>\r\n");
	
	len_body += sprintf(&body[len_body],"<Video>\r\n");
	len_body += sprintf(&body[len_body],"<videoInputChannelID>1</videoInputChannelID>\r\n");
	len_body += sprintf(&body[len_body],"<videoResolutionWidth>%d</videoResolutionWidth>\r\n",Width[hWidthxHeight]);
	len_body += sprintf(&body[len_body],"<videoResolutionHeight>%d</videoResolutionHeight>\r\n",Height[hWidthxHeight]);
	len_body += sprintf(&body[len_body],"<videoQualityControlType>CBR</videoQualityControlType>\r\n");
//	len_body += sprintf(&body[len_body],"<constantBitRate>2048</constantBitRate>\r\n");
	len_body += sprintf(&body[len_body],"<maxFrameRate>2500</maxFrameRate>\r\n");
	len_body += sprintf(&body[len_body],"</Video>\r\n");
	
	len_body += sprintf(&body[len_body],"</StreamingChannel>\r\n");
	
	sprintf(&strA[len_strA],"%d\r\n",len_body);
	
	sent_to_camera((char *)http_client_sendbuf,method,uri,strA,body);
	
	//								多值信号量指针	//等待超时时间	//选项 //等到信号量时的时间戳 //返回错误类型
	OSSemPend (&http_200_sem, 3000, OS_OPT_PEND_BLOCKING,0, &err); 

	return err;
}

int PTZ_Streaming_s(uint8_t hWidthxHeight)//Pan_Tilt_Zoom
{
//	OS_ERR      err;
	char times;
	
	if(power1_camera1==0)
	{
		printf("\r\n球机未上电");
		return -3;
	}
	
	if((hWidthxHeight>20)||(hWidthxHeight==0)) return -2;

	testtime=0;
	times=0;
	while((PTZ_Streaming(hWidthxHeight) != 0)&&(times++<6))
	{
		printf("\r\n没收到200 NG7");
	}
	if(times>6)
	{
		dm9k_reset();
		return -1;
	}
	
	return 0;
}


//ctl_dat[0]-通道  ctl_dat[1]-指令  ctl_dat[2]-参数
void nw_ctrl_hk(uint8_t *ctl_dat)
{
//	const	char *PTZCMD[4]={"TILT_UP","TILT_DOWN","PAN_LEFT","PAN_RIGHT"};//,"ZOOM_IN","ZOOM_OUT",""
	signed char x,y,z;
	char chanle;
	if(ctl_dat[0]==1) chanle=1;
	else if(ctl_dat[0]==2) chanle=1;
	else return;
	
	switch(ctl_dat[1])
	{
		case 1:camera_on(chanle);break;
		case 10:camera_off(chanle);break;
		case 2:
//			while(PTZCtrl_presets_goto(chanle,ctl_dat[2]) !=0);//摄像机调节到指定预置点
			PTZCtrl_presets_goto_s(chanle,ctl_dat[2]);	//摄像机调节到指定预置点
		break;
		case 3:
			x=0;y=28;
			PTZCtrl_continuous_s(chanle,&x,&y,0);break;
		case 4:
			x=0;y=-28;
			PTZCtrl_continuous_s(chanle,&x,&y,0);break;
		case 5:
			x=-28;y=0;
			PTZCtrl_continuous_s(chanle,&x,&y,0);break;
		case 6:
			x=28;y=0;
			PTZCtrl_continuous_s(chanle,&x,&y,0);break;
//			PUT_PTZ_s(chanle,PTZCMD[ctl_dat[1]-3]);break;
		
		case 7://焦距向远方调节1个单位（镜头变倍放大）
		z=-28;
		PTZCtrl_continuous_s(chanle,0,0,&z);
		break;
		
		case 8://焦距向近处调节1个单位（镜头变倍缩小）
		z=28;
		PTZCtrl_continuous_s(chanle,0,0,&z);
		break;
		
		case 9://保存当前位置为某预置点
//		PTZCtrl_presets(chanle,ctl_dat[2]);//保存当前位置为某预置点
		PTZCtrl_presets_s(chanle,ctl_dat[2]);//保存当前位置为某预置点
		break;
		
		case 11://光圈放大1个单位
	
		break;
	}
}



