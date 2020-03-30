

#include 	"stdint.h"		//uint ����
#include <stdio.h>
#include <string.h>

//extern char uri[120];
//extern char ip_host[20]
//extern char strA[100];
//extern char body[500];


//��׼ͷ��
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


int camera_authorization(char *sttp_buf,const char *method,const char *uri,const char ch);

int get_color_info(char *in);
extern unsigned char bLevel,cLevel,sLevel;



//int load_get_buf()
//{
//	;
//}

//#define HTTP_GET_IMAGE_STR \
//	"GET /onvif-http/snapshot?Profile_1 HTTP/1.1\r\n" \
//	"Accept: image/jpeg\r\n" \
//	"\r\n" \
//	"\r\n"

//#define	GET_photo_line1	"/onvif-http/snapshot?Profile_1"

//const char GET_photo_line1[]="/onvif-http/snapshot?Profile_1";
//const char GET_photo_line2[]="Accept: image/jpeg\r\n\r\n";

int GET_photo(char *sttp_buf,char ch)
{
	int len_total;
	
	len_total = camera_authorization(sttp_buf,"GET","/onvif-http/snapshot?Profile_1",ch);	//��֤ͷ
	len_total += sprintf(&sttp_buf[len_total],"Accept: image/jpeg\r\n\r\n"); 
	
	return len_total;
}
int GET_Streaming(char *sttp_buf,char ch)//��ѯͼ���С
{
	int len_total;
	
	len_total = camera_authorization(sttp_buf,"GET","/Streaming/channels/1",ch);	//��֤ͷ
	len_total += sprintf(&sttp_buf[len_total],"\r\n"); 

	return len_total;
}
int GET_Streaming_capabilities(char *sttp_buf,char ch)// ��ѯ�������õ�ͼ���С���
{
	int len_total;
	
	len_total = camera_authorization(sttp_buf,"GET","/Streaming/channels/1/capabilities",ch);	//��֤ͷ
	len_total += sprintf(&sttp_buf[len_total],"\r\n"); 

	return len_total;
}

int GET_Image(char *sttp_buf,char ch)//��ѯͼ������,�Աȶ�,���Ͷ�
{
	int len_total;
	
	len_total = camera_authorization(sttp_buf,"GET","/Image/channels/1/Color",ch);	//��֤ͷ
	len_total += sprintf(&sttp_buf[len_total],"\r\n"); 
	return len_total;
}


int PTZCtrl_continuous(char *sttp_buf,signed char x,signed char y,signed char z,char ch)//Pan_Tilt_Zoom �������Ҵ�С�ƶ�
{
	int len_body,len_total;
	char *body;
			
	//�ٶȷ�Χ -100-100;
	
	len_total = camera_authorization(sttp_buf,"PUT","/PTZCtrl/channels/1/continuous",ch);	//��֤ͷ
	
	len_total += sprintf(&sttp_buf[len_total],"Content-Type: text/xml\r\n");
	len_total += sprintf(&sttp_buf[len_total],"Content-Length: ");
//////////////////////////////////////////////////////////////////////////////////////	
	body= &sttp_buf[len_total+30];//����sttp_buf �������body
	len_body = sprintf(body,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
	len_body += sprintf(&body[len_body],"<PTZData>\r\n");
	if(x!=120) len_body += sprintf(&body[len_body],"<pan>%d</pan>\r\n",x);	
	if(y!=120) len_body += sprintf(&body[len_body],"<tilt>%d</tilt>\r\n",y);
	if(z!=120) len_body += sprintf(&body[len_body],"<zoom>%d</zoom>\r\n",z);	
	len_body += sprintf(&body[len_body],"</PTZData>\r\n");
//////////////////////////////////////////////////////////////////////////////////////	
	len_total += sprintf(&sttp_buf[len_total],"%d\r\n\r\n",len_body); //HTTP ͷ
	len_total += sprintf(&sttp_buf[len_total],"%s",body);
	
	return len_total;
}

	
//<?xml version="1.0" encoding="UTF-8"?>
//<PTZPreset>
//<id>1</id>
//<enabled>true</enabled>
//<presetName>Preset-1</presetName>
//</PTZPreset>
int PTZCtrl_presets(char *sttp_buf,char presetN,char ch)	//����Ԥ��λ
{
	int len_body,len_total;
	char *body;
	char *uri;
	uri=&sttp_buf[900];
	sprintf(uri,"/PTZCtrl/channels/1/presets/%d",presetN);
	len_total = camera_authorization(sttp_buf,"PUT",uri,ch);	//��֤ͷ
	
	len_total += sprintf(&sttp_buf[len_total],"Content-Type: text/xml\r\n");
	len_total += sprintf(&sttp_buf[len_total],"Content-Length: ");
///////////////////////////////////////////////////////////////////////////////////	
	body=&sttp_buf[len_total+30];  //ʹ�õ� sttp_buf���
		
	len_body = sprintf(body,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
	len_body += sprintf(&body[len_body],"<PTZPreset>\r\n");
//	len_body += sprintf(&body[len_body],"<PTZPreset>\r\n");
	len_body += sprintf(&body[len_body],"<id>%d</id>\r\n",presetN);
	len_body += sprintf(&body[len_body],"<enabled>true</enabled>\r\n");
	len_body += sprintf(&body[len_body],"<presetName>Preset-%d</presetName>\r\n",presetN);
	len_body += sprintf(&body[len_body],"</PTZPreset>\r\n");
///////////////////////////////////////////////////////////////////////////////////	
	
	len_total += sprintf(&sttp_buf[len_total],"%d\r\n\r\n",len_body); //HTTP ͷ
	len_total += sprintf(&sttp_buf[len_total],"%s",body);
	
	return len_total;
}

int PTZCtrl_presets_goto(char *sttp_buf,char presetN,char ch) //ת��Ԥ��λ
{
	int len_total;
	char *uri;
	uri=&sttp_buf[900];
	
	sprintf(uri,"/PTZCtrl/channels/1/presets/%d/goto",presetN);
	len_total = camera_authorization(sttp_buf,"PUT",uri,ch);	//��֤ͷ
	len_total += sprintf(&sttp_buf[len_total],"\r\n");
	return len_total;
}

//PAN_LEFT ��ˮƽ����ת��
//PAN_RIGHT��ˮƽ����ת��
//TILT_UP����ֱ�����˶�
//TILT_DOWN����ֱ�����˶�
//ZOOM_IN����ͷ��Զ
//ZOOM_OUT����ͷ����
//UP_LEFT: ����ת���˶�
//UP_RIGHT: ����ת���˶�
//DOWN_LEFT: ����ת���˶�
//DOWN_RIGHT: ����ת���˶�
//PAN_AUTO: �Զ�Ѳ��
//mode��ѡ����������start��stop
//speedָʾ������̨ת���ٶȣ��ٶȵ���Сֵ��1�����ֵΪ7

int PTZ(char *sttp_buf,char *command,char *mode,char speed,int waitime,char ch)//,char *mode,char speed,��������ת�ļ򵥷�ʽ
{
	int len_total;
	char *uri;

	if((speed<1)||(speed>7)) return -1;

	uri=&sttp_buf[900];
	sprintf(uri,"/PTZ/channels/1/PTZControl?command=%s&mode=%s&speed=%d",command,mode,speed);
	len_total = camera_authorization(sttp_buf,"PUT",uri,ch);	//��֤ͷ
	len_total += sprintf(&sttp_buf[len_total],"\r\n");
	return len_total;
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

int PTZ_Image(char *sttp_buf,signed char brightnessLevel,signed char contrastLevel,signed char saturationLevel,char ch)//����ͼ������,�Աȶ�,���Ͷ�
{
	int len_body,len_total;
	char *body;
	
	len_total = camera_authorization(sttp_buf,"PUT","/Image/channels/1/Color",ch);	//��֤ͷ
	
	len_total += sprintf(&sttp_buf[len_total],"Content-Type: text/xml\r\n");
	len_total += sprintf(&sttp_buf[len_total],"Content-Length: ");
///////////////////////////////////////////////////////////////////////////////////	
	body=&sttp_buf[len_total+30];
	len_body = sprintf(body,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
	len_body += sprintf(&body[len_body],"<Color version=\"1.0\" xmlns=\"http://www.std-cgi.com/ver10/XMLSchema\">\r\n");

	len_body += sprintf(&body[len_body],"<brightnessLevel>%d</brightnessLevel>\r\n",brightnessLevel);	
	len_body += sprintf(&body[len_body],"<contrastLevel>%d</contrastLevel>\r\n",contrastLevel);
	len_body += sprintf(&body[len_body],"<saturationLevel>%d</saturationLevel>\r\n",saturationLevel);
	len_body += sprintf(&body[len_body],"</Color>\r\n");	
///////////////////////////////////////////////////////////////////////////////////	
	
	len_total += sprintf(&sttp_buf[len_total],"%d\r\n\r\n",len_body); //HTTP ͷ
	len_total += sprintf(&sttp_buf[len_total],"%s",body);
	
	return len_total;
}

const uint16_t Width[21]= {0,320,640,704,800,1024,1280,1280,1920,960,1280,1600,2048,2592,2592,3072,3840,4000,4608,3200,4224};
const uint16_t Height[21]={0,240,480,576,600,768, 1024,720, 1080,576,960, 1200,1536,1520,1944,2048,2160,3000,3456,2400,3136};
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
int PTZ_Streaming(char *sttp_buf,uint8_t hWidthxHeight,char ch)//����ͼ���С��������С��ɫ��ѡ���
{
	int len_body,len_total;
	char *body;
	
	if((hWidthxHeight>20)||(hWidthxHeight==0)) return -2;

	len_total = camera_authorization(sttp_buf,"PUT","/Streaming/channels/1",ch);	//��֤ͷ
	
	len_total += sprintf(&sttp_buf[len_total],"Content-Type: text/xml\r\n"); 
	len_total += sprintf(&sttp_buf[len_total],"Content-Length: ");
///////////////////////////////////////////////////////////////////////////////////////////////////	
	body=&sttp_buf[len_total+30];
//<videoResolutionWidth>��<videoResolutionHeight>��<videoQualityControlType>��<maxFrameRate>	�ĸ���ǩ�������
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
///////////////////////////////////////////////////////////////////////////////////////////////////	
	len_total += sprintf(&sttp_buf[len_total],"%d\r\n\r\n",len_body); //HTTP ͷ
	len_total += sprintf(&sttp_buf[len_total],"%s",body);		//����body
	return len_total;
	
}





