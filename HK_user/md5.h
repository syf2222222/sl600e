/****************************************Copyright (c)****************************************************
**
**                                 http://www.
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               .h
** Last modified Date:      2018-02-07
** Last Version:            1.0.0
** Descriptions:            
**
** Modified by:
** Modified date:
** Version:
** Descriptions:
**
*********************************************************************************************************/
#ifndef __MD5_H_
#define __MD5_H_

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#include <stdint.h>


typedef struct
{
	unsigned int count[2];
	unsigned int state[4];
	unsigned char buffer[64];   
}MD5_CTX;

#define F(x,y,z) ((x & y) | (~x & z))
#define G(x,y,z) ((x & z) | (y & ~z))
#define H(x,y,z) (x^y^z)
#define I(x,y,z) (y ^ (x | ~z))
#define ROTATE_LEFT(x,n) ((x << n) | (x >> (32-n)))

#define FF(a,b,c,d,x,s,ac) \
{ \
	a += F(b,c,d) + x + ac; \
	a = ROTATE_LEFT(a,s); \
	a += b; \
}

#define GG(a,b,c,d,x,s,ac) \
{ \
	a += G(b,c,d) + x + ac; \
	a = ROTATE_LEFT(a,s); \
	a += b; \
}

#define HH(a,b,c,d,x,s,ac) \
{ \
	a += H(b,c,d) + x + ac; \
	a = ROTATE_LEFT(a,s); \
	a += b; \
}

#define II(a,b,c,d,x,s,ac) \
{ \
	a += I(b,c,d) + x + ac; \
	a = ROTATE_LEFT(a,s); \
	a += b; \
}


extern void MD5Init(MD5_CTX *context);
extern void MD5Update(MD5_CTX *context,unsigned char *input,unsigned int inputlen);
extern void MD5Final(MD5_CTX *context,unsigned char digest[16]);
extern void MD5Transform(unsigned int state[4],unsigned char block[64]);

//extern void MD5Encode(unsigned char *output,unsigned int *input,unsigned int len);
//extern void MD5Decode(unsigned int *output,unsigned char *input,unsigned int len);
extern void u32_to_u8(unsigned int *input, unsigned char *output, unsigned int len);
extern void u8_to_u32(unsigned char *input, unsigned int *output, unsigned int len);
extern void md5_char32(MD5_CTX *p_md5,const uint8_t *in, uint32_t in_len,	uint8_t *out,	uint8_t lower_upper);


#ifdef __cplusplus
}  //extern "C"
#endif //__cplusplus


#endif                                                                  /* __MD5_H_ */
/*********************************************************************************************************
END FILE
*********************************************************************************************************/
