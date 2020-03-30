
#include <stdio.h>
#include <string.h>
#include "md5.h"
#include "lwip/api.h"
//#include "camera_authorization.h"

//Keep-Alive: timeout=10, max=99
//timeout������ʱ��10�루��Ӧhttpd.conf��Ĳ����ǣ�KeepAliveTimeout��
//max�����99������ǿ�ƶϵ����ӡ�������timeoutʱ���������µ����ӹ�����ͬʱmax���Զ���1��ֱ��Ϊ0��ǿ�ƶϵ���

//username: �û�������վ���壩 
//password: ���� 
//realm:�����������ص�realm,һ�������� 
//method: ����ķ��� ��ָ��GET��/"POST"����httpͷ��ָ���Ļ�ȡ��Դ�ķ�ʽ
//nonce: �����������ͻ��˵�������ַ��� 
//nc(nonceCount):����Ĵ��������ڱ�ǣ���������ֹ�طŹ��� 
//cnonce(clinetNonce): �ͻ��˷��͸�������������ַ���
//qop: ������������,һ����auth,��auth-int,���Ӱ��ժҪ���㷨��
//qop����֤��(У��)��ʽ
//uri: �����uri(ֻ�ǣ�����)
//response:���ͻ��˸����㷨�����ժҪֵ
	
	//����response��response=md5(md5(A1):HD:md5(A2))

	//1������A1��
	//1.1�����"algorithm"ֵ��"MD5",��û��ָ������A1=username:realm:password;
	//1.2�����"algorithm"ֵ��"MD5-sess",��A1=md5(username:realm:password):nonce:cnonce;

	//2������HD��
	//2.1�����"qop"ֵ��"auth" ��"auth-int"����HD=nonce:nc:cnonce:qop;
	//2.2�����"qop"ֵûָ������HD=nonce;

	//3������A2��
	//3.1�����"qop"ֵ��"auth"����û��������A2=method:uri;
	//3.2�����"qop"ֵ��"auth-int", ��A2=method:uri:md5(body)

	//4������response��response=md5(md5(A1):HD:md5(A2))
	
//#define USERNAME_STR					"admin"
//#define PASSWORD_STR					"scisun3206"


//HTTP/1.1 401 Unauthorized
//Date: Fri, 27 Sep 2019 14:21:46 GMT
//Server: webserver
//Content-Length: 255
//Content-Type: text/html
//Connection: keep-alive
//Keep-Alive: timeout=10, max=99
//WWW-Authenticate: Digest qop="auth", realm="IP Camera(C5654)", nonce="4d3249794e324d305a6d4d364e7a4d784f5459795a47493d", stale="FALSE"


//HTTP/1.1 401 Unauthorized
//Date: Mon, 23 Jan 2017 00:08:16 GMT
//Server: App-webs/
//Content-Length: 218
//Content-Type: text/html
//Connection: keep-alive
//Keep-Alive: timeout=60, max=33
//WWW-Authenticate: Basic realm="API"


const char algorithm[9]="\0";
const char username[10]="admin";
const char password[20]="scisun3206";//scisun3206

char qop[9]="auth";
char realm[30]="IP Camera(C5654)";//"DS-2DF5130W-D";//
char nonce[50]="";//"596a686c5a6a6c6d4e4455365a6a42684d7a4d315a57553d";//
int nc_d=1;
//char nc[9]="00000001";
char cnonce[33];
extern void rand_num_create_str32(uint8_t *out);


//#define check_md5
//typedef struct
//{
//	unsigned int count[2];
//	unsigned int state[4];
//	unsigned char buffer[64];   
//}MD5_CTX;
//								ת���ݴ�						����ֵ						����					���ֵ					��Сд
//void md5_char32(MD5_CTX *p_md5, const uint8_t *in, uint32_t in_len, uint8_t *out, uint8_t lower_upper)

MD5_CTX md5;

int  camput_response(char *m5_buf,char *method,char *uri) //��Ҫ��������ֹ���ظ�����
{
	int len_total,A2_len;
	
	len_total=0;
	if(strcmp(algorithm,"\0")==0) //algorithmΪ�� A1=username:realm:password;
	{
		len_total +=sprintf(&m5_buf[len_total],"%s:%s:%s",username,realm,password);//m5_buf=A1=username:realm:password;	
		md5_char32(&md5,(uint8_t *) m5_buf,len_total,(uint8_t *) m5_buf, 0); //m5_buf=md5(A1)
		m5_buf[32]=0;	
		len_total=32;
	}	
	if(strcmp(qop,"auth")==0)//HD=nonce:nc:cnonce:qop  A2=method:uri
	{
		len_total +=sprintf(&m5_buf[len_total],":%s:%08X:%s:%s:",nonce,nc_d,cnonce,qop);//m5_buf=md5(A1):nonce:nc:cnonce:qop:
		
		A2_len=0;
		A2_len +=sprintf(&m5_buf[len_total],"%s:%s",method,uri);	//&m5_buf[len_total]=A2=method:uri
																																															
		md5_char32(&md5,(uint8_t *) &m5_buf[len_total],A2_len,(uint8_t *) &m5_buf[len_total], 0); //&m5_buf[len_total]=md5(A2)
		//m5_buf=md5(A1):nonce:nc:cnonce:qop:md5(A2)
		len_total +=32;
		m5_buf[len_total]=0;
		
		//response=md5(md5(A1):HD:md5(A2))=md5(md5(A1):nonce:nc:cnonce:qop:md5(A2))
		md5_char32(&md5,(uint8_t *) m5_buf, len_total,(uint8_t *) m5_buf, 0); //m5_buf=md5(md5(A1):HD:md5(A2))
		m5_buf[32]=0;
//		printf("\r\nresponse=%s",m5_buf);
	}
	return 0;
}	

int camera_authorization(char *sttp_buf,const char *method,const char *uri)//,const char *ip_host,const char aut
{
	uint16_t len_total = 0;

	len_total =sprintf(&sttp_buf[len_total],"%s ",method);

	len_total +=sprintf(&sttp_buf[len_total],"%s ",uri);
	
	len_total +=sprintf(&sttp_buf[len_total],"HTTP/1.1\r\n");
//	len_total +=sprintf(&sttp_buf[len_total],"Host: %s\r\n",ip_host);
	len_total +=sprintf(&sttp_buf[len_total],"Host: %d.%d.%d.%d\r\n",LoclIP[0],LoclIP[1],LoclIP[2],LoclIP[3]);
		
	len_total +=sprintf(&sttp_buf[len_total],"Connection: Keep-Alive\r\n");	
			
//	if(aut)	//����֤�ֶ�
//	if(strlen(nonce)>=32)
	{		
		nc_d++;
	//	sprintf(nc,"%08X",nc_d);
		rand_num_create_str32((unsigned char *)cnonce); //���������
		cnonce[32]=0;
				
		len_total +=sprintf(&sttp_buf[len_total],"Authorization: Digest username=\"%s\",realm=\"%s\",",username,realm);
		
		if(strcmp(qop,"\0")!=0)
		{
			len_total +=sprintf(&sttp_buf[len_total],"qop=\"%s\",",qop);
		}
		if(strcmp(algorithm,"\0")!=0)
		{
			len_total +=sprintf(&sttp_buf[len_total],"algorithm=\"%s\",",algorithm);
		}
		
//		len_total +=sprintf(&sttp_buf[len_total],"uri=\"%s\",nonce=\"%s\",nc=%s,cnonce=\"%s\",response=\"%s\"\r\n",uri,nonce,nc,cnonce,md5_out);
		len_total +=sprintf(&sttp_buf[len_total],"uri=\"%s\",nonce=\"%s\",nc=%08X,cnonce=\"%s\",response=\"",uri,nonce,nc_d,cnonce);
		
		camput_response(&sttp_buf[len_total],(char *)method,(char *)uri); //���� responseֵ	
		len_total+=32;
		len_total +=sprintf(&sttp_buf[len_total],"\"\r\n");
		
//		md5_out[32]=0;
	}
	return len_total;
	
//		err = netconn_write(conn, sttp_buf, strlen((char *)sttp_buf), NETCONN_COPY);
}



//static 
void rand_num_create_str32(uint8_t *out)
{
	uint8_t i;
	uint8_t j;
	uint8_t *p;
	uint8_t temp_u8;
	uint32_t rand_num;
	uint32_t temp = 0;

	if (NULL == out)
	{
		return;
	}

	rand_num = HAL_GetTick();
	//rand_num = (uint32_t)time(NULL);
	p = out;

	for (i = 0; i < 4; i++)
	{
		srand(rand_num);
		temp = (uint32_t)rand();

		srand(temp);
		rand_num = (uint32_t)rand();

		temp <<= 16;
		rand_num = temp | (uint16_t)rand_num;

		for (j = 0; j < 4; j++)
		{
			temp_u8 = (uint8_t)(rand_num >> (8 * (3 - j)));
			*p = temp_u8 >> 4;
			*p++ = ((*p <= 9) ? (*p + '0') : (*p + ('A' - 0x0A)));//A

			*p = temp_u8 & 0x0F;
			*p++ = ((*p <= 9) ? (*p + '0') : (*p + ('A' - 0x0A)));
		}
	}
}							
							
							
