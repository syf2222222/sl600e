/****************************************Copyright (c)****************************************************
**
**                                 http://www.
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               .c
** Last modified Date:      2018-02-07
** Last Version:            1.0.0
** Descriptions:
**
**--------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
//#include <time.h>
#include "stm32l4xx_hal.h"

#include "lwip/api.h"
#include "md5.h"
#include "camera_authorization.h"

extern UART_HandleTypeDef huart1;

#define MD5_SESS_STR					"MD5-sess"
#define AUTH_STR						"auth"
#define AUTH_INT_STR					"auth-int"
#define NC_STR							"00000001"

#define USERNAME_LEN					(sizeof(USERNAME_STR) - 1)
#define PASSWORD_LEN					(sizeof(PASSWORD_STR) - 1)
#define NC_LEN							(sizeof(NC_STR) - 1)
#define MD5_LEN							32
#define CNONCE_LEN						32

#define HTTP_GET_IMAGE_STR \
	"GET /onvif-http/snapshot?Profile_1 HTTP/1.1\r\n" \
	"Accept: image/jpeg\r\n" \
	"\r\n" \
	"\r\n"

#define UNAUTHORIZED_401_HEAD_STR		"HTTP/1.1 401"
#define CONTENT_LENGTH_HEAD_STR			"Content-Length: "
#define AUTHENTICATE_HEAD_STR			"WWW-Authenticate: Digest "
#define RN_2_STR						"\r\n\r\n"


uint8_t http_buf[1024];


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

static const uint8_t * key_find(const uint8_t *key, const uint8_t *in, uint16_t in_len)
{
	const uint8_t *p;
	const uint8_t *in_end;
	uint8_t key_len;

	if ((NULL == key)
		|| (NULL == in)
			|| (0 == in_len))
	{
		return NULL;
	}

	key_len = strlen((char *)key);
	if (0 == key_len)
	{
		return NULL;
	}

	in_end = in + in_len - 1;
	for (p = in; p <= in_end; p++)
	{
		//key=valuern
		if (('\r' == *p)
				|| ('\n' == *p)
					|| ((p + key_len + 1 + 2 - 1) > in_end))
		{
			return NULL;
		}

		if (((' ' == *(p - 1)) || (',' == *(p - 1)))
				&& (0 == memcmp(p, key, key_len))
					&& ('=' == *(p + key_len)))
		{
			//printf("Found %s\r\n", key);
			return p;
		}
	}

	return NULL;
}

static int key_find_value(const uint8_t *key,
					const uint8_t *in_begin, const uint8_t *in_end,
					const uint8_t **p_out, uint16_t *p_out_len)
{
	const uint8_t *curr;

	if ((NULL == key)
			|| (NULL == in_begin)
				|| (in_end <= in_begin)
					|| (NULL == p_out)
						|| (NULL == p_out_len))
	{
		return -1;
	}

	//0123456789a
	//key="value",
	curr = key_find(key, in_begin, in_end - in_begin + 1);
	if (NULL == curr)
	{
		return -1;
	}

	curr += strlen((char *)key) + 1;
	if ('\"' == *curr)
	{
		const uint8_t *p;

		curr++;
		for (p = curr; p <= in_end; p++)
		{
			if ('\"' == *p)
			{
				if (p == curr)
				{
					return -1;
				}

				*p_out = curr;
				*p_out_len = p - 1 - curr + 1;					
				return 0;
			}
			else if (('\r' == *p)
						|| ('\n' == *p))
			{
				return -1;
			}
		}
	}

	return -1;
}

int method_head_pkg(const uint8_t *method, uint16_t method_len,
					const uint8_t *uri, uint16_t uri_len,
					const uint8_t *ip_host, uint8_t ip_host_len,
					const uint8_t *str, uint16_t str_len,
					uint8_t *out, uint16_t *p_out_len)
{
	uint16_t len_total = 0;

	if ((NULL == method)
			|| (0 == method_len)
				|| (NULL == uri)
					|| (0 == uri_len)
						|| (NULL == ip_host)
							|| (0 == ip_host_len)
								|| (NULL == out)
									|| (NULL == p_out_len))
	{
		return -1;
	}

	memcpy(out, method, method_len);
	len_total = method_len;
	out[len_total++] = ' ';

	memcpy(out + len_total, uri, uri_len);
	len_total += uri_len;

	memcpy(out + len_total, " HTTP/1.1\r\nHost: ", sizeof(" HTTP/1.1\r\nHost: ") - 1);
	len_total += sizeof(" HTTP/1.1\r\nHost: ") - 1;
	memcpy(out + len_total, ip_host, ip_host_len);
	len_total += ip_host_len;
	
	memcpy(out + len_total, "\r\nConnection: Keep-Alive\r\n", sizeof("\r\nConnection: Keep-Alive\r\n") - 1);
	len_total += sizeof("\r\nConnection: Keep-Alive\r\n") - 1;

	if ((NULL != str)
			&& (0 != str_len))
	{
		memcpy(out + len_total, str, str_len);
		len_total += str_len;
	}

	if (NULL != p_out_len)
	{
		*p_out_len = len_total;
	}

	return 0;
}
/*
SETUP rtsp://192.168.1.11/Streaming/Channels/101/trackID=1 RTSP
CSeq: 3
Transport: RTP/AVP;unicast;client_port=1094-49653
User-Agent: Hikplayer


RTSP/1.0 401 Unauthorized
CSeq: 3
WWW-Authenticate: Digest realm="IP Camera(C5654)", nonce="49072c70eaf2a3844d5a3028238893a3", stale="FALSE"
Date:  Fri, Sep 20 2019 19:20:06 GMT

GET /dir/index.html HTTP/1.0
Host: localhost


Authorization: Digest username="Mufasa",
                     realm="testrealm@host.com",
                     nonce="dcd98b7102dd2f0e8b11d0f600bfb0c093",
                     uri="/dir/index.html",
                     qop=auth,
                     nc=00000001,
                     cnonce="0a4f113b",
                     response="6629fae49393a05397450978507c4ef1",
                     opaque="5ccc069c403ebaf9f0171e9517f40e41"
										 
										 
A1 = username:realm:password = admin:IP Camera(C5654):scisun3206
A2 = mthod:uri

*/
extern char Auth_str[200];

char Enonce[49];
char Ecnonce[33];

int camera_authorization(const uint8_t *in, uint16_t in_len,
							const uint8_t *method, uint16_t method_len,
							const uint8_t *uri, uint16_t uri_len,
							const uint8_t *ip_host, uint8_t ip_host_len,
							const uint8_t *str, uint16_t str_len,
							struct netconn *conn)
{
	int flag = -1;
	uint8_t step = 0;

	const uint8_t *curr;
	const uint8_t *addr_end;

	uint16_t len_total = 0;

	const uint8_t *qop = NULL;
	uint16_t qop_len = 0;

	const uint8_t *realm = NULL;
	uint16_t realm_len = 0;

	const uint8_t *nonce = NULL;
	uint16_t nonce_len = 0;

	const uint8_t *algorithm = NULL;
	uint16_t algorithm_len = 0;

	MD5_CTX md5;
	uint8_t md5_out_str[33];
	uint8_t cnonce[33];

	if ((NULL == in)
			|| (0 == in_len)
				|| (NULL == method)
					|| (0 == method_len)
						|| (NULL == uri)
							|| (0 == uri_len)
								|| (NULL == ip_host)
									|| (0 == ip_host_len))
	{
		return -1;
	}

	//HTTP/1.1 401\r\n
	curr = in;
	if (0 == memcmp(curr, UNAUTHORIZED_401_HEAD_STR, sizeof(UNAUTHORIZED_401_HEAD_STR) - 1))
	{
		curr += sizeof(UNAUTHORIZED_401_HEAD_STR) - 1;
		//printf("Found %s\r\n", UNAUTHORIZED_401_HEAD_STR);
	}
	else
	{
		return -1;
	}

	addr_end = in + in_len - 1;
	for (; curr <= addr_end; curr++)
	{
		if ((curr + sizeof(AUTHENTICATE_HEAD_STR) - 1 + 2 - 1) > addr_end)
		{
			return -1;
		}
		else
		{
			if (0 == memcmp(curr, AUTHENTICATE_HEAD_STR, sizeof(AUTHENTICATE_HEAD_STR) - 1))
			{
				step = 1;
				curr += sizeof(AUTHENTICATE_HEAD_STR) - 1;
				//printf("Found %s\r\n", AUTHENTICATE_HEAD_STR);

				break;
			}
		}
	}

	if (0 == step)
	{
		return -1;
	}

	if (0 == key_find_value("realm", curr, addr_end, &realm, &realm_len))
	{
		//const uint8_t *p;
		//const uint8_t *realm_end;

		//realm_end = realm + realm_len - 1;
		/*printf("realm=");
		for (p = realm; p <= realm_end; p++)
		{
			printf("%c", *p);
		}
		printf("\r\n");*/
	}
	else
	{
		return -1;
	}

	if (0 == key_find_value("nonce", curr, addr_end, &nonce, &nonce_len))
	{
		//const uint8_t *p;
		//const uint8_t *nonce_end;

		//nonce_end = nonce + nonce_len - 1;
		/*printf("nonce=");
		for (p = nonce; p <= nonce_end; p++)
		{
			printf("%c", *p);
		}
		printf("\r\n");*/
		
		strncpy(Enonce,(char *)nonce,nonce_len);
		printf("Enonce=%s",Enonce);
	}
	else
	{
		return -1;
	}

	if (0 == key_find_value("qop", curr, addr_end, &qop, &qop_len))
	{
		//const uint8_t *p;
		//const uint8_t *qop_end;

		//qop_end = qop + qop_len - 1;
		/*printf("qop=");
		for (p = qop; p <= qop_end; p++)
		{
			printf("%c", *p);
		}
		printf("\r\n");*/
	}

	if (0 == key_find_value("algorithm", curr, addr_end, &algorithm, &algorithm_len))
	{
		//const uint8_t *p;
		//const uint8_t *algorithm_end;

		//algorithm_end = algorithm + algorithm_len - 1;
		/*printf("algorithm=");
		for (p = algorithm; p <= algorithm_end; p++)
		{
			printf("%c", *p);
		}
		printf("\r\n");*/
	}
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
	
	
//HTTP/1.1 401 Unauthorized
//Date: Fri, 27 Sep 2019 14:21:46 GMT
//Server: webserver
//Content-Length: 255
//Content-Type: text/html
//Connection: keep-alive
//Keep-Alive: timeout=10, max=99
//WWW-Authenticate: Digest qop="auth", realm="IP Camera(C5654)", nonce="4d3249794e324d305a6d4d364e7a4d784f5459795a47493d", stale="FALSE"
	
	
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

	//1、计算 md5(A1)
	{
		//1、计算A1：
		//1.1、如果"algorithm"值是"MD5",或没有指定，则A1=username:realm:password;
		//1.2、如果"algorithm"值是"MD5-sess",则A1=md5(username:realm:password):nonce:cnonce;

		//username:realm:password
		memcpy(http_buf, USERNAME_STR, USERNAME_LEN);
		len_total += USERNAME_LEN;

		http_buf[len_total++] = ':';

		memcpy(http_buf + len_total, realm, realm_len);
		len_total += realm_len;

		http_buf[len_total++] = ':';

		memcpy(http_buf + len_total, PASSWORD_STR, PASSWORD_LEN);
		len_total += PASSWORD_LEN;

		rand_num_create_str32(cnonce);		
		cnonce[CNONCE_LEN] = '\0';
		
		strcpy(Ecnonce,(char *)cnonce);
		printf("Ecnonce=%s",Ecnonce);
		
		//printf("cnonce=%s\r\n", cnonce);
//HTTP/1.1 401 Unauthorized
//Date: Fri, 27 Sep 2019 14:21:46 GMT
//Server: webserver
//Content-Length: 255
//Content-Type: text/html
//Connection: keep-alive
//Keep-Alive: timeout=10, max=99
//WWW-Authenticate: Digest qop="auth", realm="IP Camera(C5654)", nonce="4d3249794e324d305a6d4d364e7a4d784f5459795a47493d", stale="FALSE"
		if ((NULL != algorithm)
			&& (algorithm_len == (sizeof(MD5_SESS_STR) - 1))) //"MD5-sess"
		{
			if (0 == memcmp(algorithm, MD5_SESS_STR, sizeof(MD5_SESS_STR) - 1))
			{
				//1.2、如果"algorithm"值是"MD5-sess",则A1=md5(username:realm:password):nonce:cnonce;
				md5_char32(&md5, http_buf, len_total, http_buf, 0);
				len_total = MD5_LEN;

				http_buf[len_total++] = ':';

				memcpy(http_buf + len_total, nonce, nonce_len);
				len_total += nonce_len;

				http_buf[len_total++] = ':';

				memcpy(http_buf + len_total, cnonce, CNONCE_LEN);
				len_total += CNONCE_LEN;
			}
		}

		http_buf[len_total] = '\0';
		//printf("\r\na=%s\r\n", http_buf);

		//md5(A1)
		printf("\r\nA1=%s",http_buf);
		md5_char32(&md5, http_buf, len_total, http_buf, 0);
		
		len_total = MD5_LEN;
		http_buf[len_total] = '\0';
		printf("\r\nmd5(A1)=%s",http_buf);
		//printf("md5(a)=%s\r\n", http_buf);
	}
	
	//判断qop值
	if (NULL != qop)
	{
		if (qop_len == (sizeof(AUTH_STR) - 1))
		{
			if (0 == memcmp(qop, AUTH_STR, sizeof(AUTH_STR) - 1))
			{
				//printf("value=%s\r\n", AUTH_STR);
				flag = 0;
			}
		}
		else if (qop_len == (sizeof(AUTH_INT_STR) - 1))
		{
			if (0 == memcmp(qop, AUTH_INT_STR, sizeof(AUTH_INT_STR) - 1))
			{
				//printf("value=%s\r\n", AUTH_INT_STR);
				flag = 1;
			}
		}
	}

	//2、计算HD：
	//2.1、如果"qop"值是"auth" 或"auth-int"，则HD=nonce:nc:cnonce:qop;
	//2.2、如果"qop"值没指定，则HD=nonce;
	{
		//md5(A1):HD:md5(A2)
		http_buf[len_total++] = ':';

		memcpy(http_buf + len_total, nonce, nonce_len);
		len_total += nonce_len;

		//计算HD
		if (flag >= 0)
		{
			//2.1、如果"qop"值是"auth" 或"auth-int"，则HD=nonce:nc:cnonce:qop
			//nc=00000001
			http_buf[len_total++] = ':';

			memcpy(http_buf + len_total, NC_STR, NC_LEN);
			len_total += NC_LEN;

			http_buf[len_total++] = ':';

			memcpy(http_buf + len_total, cnonce, CNONCE_LEN);
			len_total += CNONCE_LEN;

			http_buf[len_total++] = ':';

			memcpy(http_buf + len_total, qop, qop_len);
			len_total += qop_len;
		}

		http_buf[len_total] = '\0';
		//printf("\r\nhd=%s\r\n", http_buf);
	}

	//3、计算A2：
	//3.1、如果"qop"值是"auth"或者没给出，则A2=method:uri;
	//3.2、如果"qop"值是"auth-int", 则A2=method:uri:md5(body)
	{
		uint16_t len_a2_before = 0;

		//method_len = strlen((const char *)method);
		//uri_len = strlen((const char *)uri);

		//md5(md5(A1):HD:md5(A2))
		http_buf[len_total++] = ':';
		
		http_buf[len_total]=0;
		printf("\r\nmd5(A1):HD:=%s",http_buf);
		
		len_a2_before = len_total;

		memcpy(http_buf + len_total, method, method_len);
		len_total += method_len;

		http_buf[len_total++] = ':';
		
		memcpy(http_buf + len_total, uri, uri_len);
		len_total += uri_len;
		
		if (1 == flag)
		{
			//3.2、如果"qop"值是"auth-int", 则A2=method:uri:md5(body)
			const uint8_t *body = NULL;
			uint16_t body_len = 0;
					
			const uint8_t *p;

			//Content-Length: 6rn
			for (p = in; p <= addr_end; p++)
			{
				if ((p + sizeof(CONTENT_LENGTH_HEAD_STR) - 1 + 2 - 1) > addr_end)
				{
					break;
				}
						
				if (0 == memcmp(p, CONTENT_LENGTH_HEAD_STR, sizeof(CONTENT_LENGTH_HEAD_STR) - 1))
				{
					unsigned int temp = 0;

					p += sizeof(CONTENT_LENGTH_HEAD_STR) - 1;
					//printf("%s", CONTENT_LENGTH_HEAD_STR);

					if (1 == sscanf((const char *)p, "%u", &temp))
					{
						body_len = temp;
						//printf("%u\r\n", body_len);

						//rnrn123456
						for (; p <= addr_end; p++)
						{
							if ((p + sizeof(RN_2_STR) - 1 + 2 - 1) > addr_end)
							{
								break;
							}

							if (0 == memcmp(p, RN_2_STR, sizeof(RN_2_STR) - 1))
							{
								body = p + sizeof(RN_2_STR) - 1;
								break;
							}
						}	
					}

					break;
				}
			}

			if ((NULL != body)
					&& (0 != body_len))
			{

				//3.2、如果"qop"值是"auth-int", 则A2=method:uri:md5(body)
				http_buf[len_total++] = ':';

				md5_char32(&md5, body, body_len, http_buf + len_total, 0); //MD5(body)
				len_total += MD5_LEN;
			}
		}

		http_buf[len_total] = '\0';
		//printf("\r\na2=%s\r\n", http_buf);

		//md5(A2)
		//md5(md5(A1):HD:md5(A2))		
		printf("\r\nA2=%s",http_buf + len_a2_before);
		
		md5_char32(&md5, http_buf + len_a2_before, len_total - len_a2_before, http_buf + len_a2_before, 0); //MD5(A2)
		len_total = len_a2_before + MD5_LEN;
		http_buf[len_total] = '\0';
		
		printf("\r\nmd5(a2)=%s\r\n", http_buf + len_a2_before);
	}

	//4、计算response
	//response=md5(md5(A1):HD:md5(A2))
	{
		md5_char32(&md5, http_buf, len_total, md5_out_str, 0);
		md5_out_str[MD5_LEN] = '\0';
		//printf("\r\nresponse=%s\r\n", md5_out_str);
	}
	
	
	//5、打包数据
	{
		err_t err;
		uint16_t len_temp = 0;

		//GET /onvif-http/snapshot?Profile_1 HTTP/1.1\r\n
		//Host: 192.168.146.11\r\n
		//Connection: Keep-Alive\r\n
		//Accept: image/jpeg\r\n\r\n
				
		if (0 != method_head_pkg(method, method_len,
								uri, uri_len,
								ip_host, ip_host_len,
								NULL, 0,
								http_buf, &len_temp))
		{
			return -1;
		}
		len_total = len_temp;

		memcpy(http_buf + len_total, "Authorization: Digest username=\"", sizeof("Authorization: Digest username=\"") - 1);
		len_total += sizeof("Authorization: Digest username=\"") - 1;
		memcpy(http_buf + len_total, USERNAME_STR, sizeof(USERNAME_STR) - 1);
		len_total += sizeof(USERNAME_STR) - 1;

		memcpy(http_buf + len_total, "\",realm=\"", sizeof("\",realm=\"") - 1);
		len_total += sizeof("\",realm=\"") - 1;
		memcpy(http_buf + len_total, realm, realm_len);
		len_total += realm_len;
//GET /dir/index.html HTTP/1.0
//Host: localhost
//Authorization: Digest username="Mufasa",
//                     realm="testrealm@host.com",
//                     nonce="dcd98b7102dd2f0e8b11d0f600bfb0c093",
//                     uri="/dir/index.html",
//                     qop=auth,
//                     nc=00000001,
//                     cnonce="0a4f113b",
//                     response="6629fae49393a05397450978507c4ef1",
//                     opaque="5ccc069c403ebaf9f0171e9517f40e41"

//int camera_authorization(const uint8_t *in, uint16_t in_len,
//							const uint8_t *method, uint16_t method_len,
//							const uint8_t *uri, uint16_t uri_len,
//							const uint8_t *ip_host, uint8_t ip_host_len,
//							const uint8_t *str, uint16_t str_len,
//							struct netconn *conn)	
		if ((NULL != qop)
				&& (0 != qop_len))
		{
			memcpy(http_buf + len_total, "\",qop=\"", sizeof("\",qop=\"") - 1);
			len_total += sizeof("\",qop=\"") - 1;
			memcpy(http_buf + len_total, qop, qop_len);
			len_total += qop_len;
		}

		if ((NULL != algorithm)
				&& (0 != algorithm_len))
		{
			memcpy(http_buf + len_total, "\",algorithm=\"", sizeof("\",algorithm=\"") - 1);
			len_total += sizeof("\",algorithm=\"") - 1;
			memcpy(http_buf + len_total, algorithm, algorithm_len);
			len_total += algorithm_len;
		}
		
		memcpy(http_buf + len_total, "\",uri=\"", sizeof("\",uri=\"") - 1);
		len_total += sizeof("\",uri=\"") - 1;
		memcpy(http_buf + len_total, uri, uri_len);
		len_total += uri_len;

		memcpy(http_buf + len_total, "\",nonce=\"", sizeof("\",nonce=\"") - 1);
		len_total += sizeof("\",nonce=\"") - 1;
		memcpy(http_buf + len_total, nonce, nonce_len);
		len_total += nonce_len;

		memcpy(http_buf + len_total, "\",nc=", sizeof("\",nc=") - 1);
		len_total += sizeof("\",nc=") - 1;
		memcpy(http_buf + len_total, NC_STR, sizeof(NC_STR) - 1);
		len_total += sizeof(NC_STR) - 1;

		memcpy(http_buf + len_total, ",cnonce=\"", sizeof(",cnonce=\"") - 1);
		len_total += sizeof(",cnonce=\"") - 1;
		memcpy(http_buf + len_total, cnonce, CNONCE_LEN);
		len_total += CNONCE_LEN;

		memcpy(http_buf + len_total, "\",response=\"", sizeof("\",response=\"") - 1);
		len_total += sizeof("\",response=\"") - 1;
		memcpy(http_buf + len_total, md5_out_str, MD5_LEN);
		len_total += MD5_LEN;
		http_buf[len_total++] = '\"';
		http_buf[len_total++] = '\r';
		http_buf[len_total++] = '\n';
		
//		http_buf[len_total] = '\0';
//		strcpy(Auth_str, (char *)&http_buf[len_temp]);
//		printf("Auth_str=%s",Auth_str);

		if ((NULL != str)
				&& (0 != str_len))
		{
			memcpy(http_buf + len_total, str, str_len);
			len_total += str_len;
			printf("有str");
		}

		http_buf[len_total++] = '\r';
		http_buf[len_total++] = '\n';
		
		//printf("\r\n333 send_len = %u\r\n", len_total);
		http_buf[len_total] = '\0';
		//printf("http_buf =\r\n%s\r\n", http_buf);

//		HAL_UART_Transmit(&huart1,http_buf,len_total,10);//test
		err = netconn_write(conn, http_buf, len_total, NETCONN_COPY);
		
		//printf("err = %d\r\n", err);
		if (ERR_OK != err)
		{
			printf("Error: tcp write 2\r\n");
			return -1;
		}
		
//		printf("准备发送1\r\n");
//		err = netconn_write(tcp_clientconn ,http_buf,len_total,NETCONN_COPY); //发送tcp_server_sentbuf中的数据
//		if(err != ERR_OK)
//		{
//			printf("发送失败\r\n");
//		}
	}

	return 0;
}

/*********************************************************************************************************
END FILE
*********************************************************************************************************/
