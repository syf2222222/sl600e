
#include <stdio.h>
#include <string.h>
#include "md5.h"
#include "lwip/api.h"
//#include "camera_authorization.h"

//Keep-Alive: timeout=10, max=99
//timeout：过期时间10秒（对应httpd.conf里的参数是：KeepAliveTimeout）
//max：最多99次请求，强制断掉连接。就是在timeout时间内又有新的连接过来，同时max会自动减1，直到为0，强制断掉。

//username: 用户名（网站定义） 
//password: 密码 
//realm:　服务器返回的realm,一般是域名 
//method: 请求的方法 是指“GET”/"POST"，即http头中指定的获取资源的方式
//nonce: 服务器发给客户端的随机的字符串 
//nc(nonceCount):请求的次数，用于标记，计数，防止重放攻击 
//cnonce(clinetNonce): 客户端发送给服务器的随机字符串
//qop: 保护质量参数,一般是auth,或auth-int,这会影响摘要的算法，
//qop是认证的(校验)方式
//uri: 请求的uri(只是ｐａｔｈ)
//response:　客户端根据算法算出的摘要值
	
	//计算response，response=md5(md5(A1):HD:md5(A2))

	//1、计算A1：
	//1.1、如果"algorithm"值是"MD5",或没有指定，则A1=username:realm:password;
	//1.2、如果"algorithm"值是"MD5-sess",则A1=md5(username:realm:password):nonce:cnonce;

	//2、计算HD：
	//2.1、如果"qop"值是"auth" 或"auth-int"，则HD=nonce:nc:cnonce:qop;
	//2.2、如果"qop"值没指定，则HD=nonce;

	//3、计算A2：
	//3.1、如果"qop"值是"auth"或者没给出，则A2=method:uri;
	//3.2、如果"qop"值是"auth-int", 则A2=method:uri:md5(body)

	//4、计算response，response=md5(md5(A1):HD:md5(A2))
	
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
//								转换暂存						输入值						长度					输出值					大小写
//void md5_char32(MD5_CTX *p_md5, const uint8_t *in, uint32_t in_len, uint8_t *out, uint8_t lower_upper)

MD5_CTX md5;

int  camput_response(char *m5_buf,char *method,char *uri) //需要互斥锁防止被重复调用
{
	int len_total,A2_len;
	
	len_total=0;
	if(strcmp(algorithm,"\0")==0) //algorithm为空 A1=username:realm:password;
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
			
//	if(aut)	//加认证字段
//	if(strlen(nonce)>=32)
	{		
		nc_d++;
	//	sprintf(nc,"%08X",nc_d);
		rand_num_create_str32((unsigned char *)cnonce); //生成随机码
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
		
		camput_response(&sttp_buf[len_total],(char *)method,(char *)uri); //计算 response值	
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
							
							
