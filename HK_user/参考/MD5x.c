
#include "main.h"

#include <stdio.h>
#include <string.h>

#include "md5.h"

//512/8=64 字节一组
//长度值 64位=8字节
//64/16=4 字节一个子分组
//结果为 128位 16字节


#define staA		0x01234567	//4字节
#define staB		0x89abcdef
#define staC		0xfedcba98
#define staD		0x76543210

//#define staA  0x67452301 
//#define staB  0xEFCDAB89
//#define staC  0x98BADCFE 
//#define staD  0x10325476




//#define F(X,Y,Z)	((X&Y)|((~X)&Z))
//#define G(X,Y,Z)	((X&Z)|(Y&(~Z)))
//#define H(X,Y,Z)	(X^Y^Z)
//#define I(X,Y,Z)	(Y^(X|(~Z)))

#define leftrot(x,n)	((x << n) | (x >> (32-n)))

//uint32_t leftrot(uint32_t inDat,uint32_t num)
//{
//	uint32_t outDat;
//	outDat=((inDat << num) | (inDat >> (32-num)));
//	return outDat;
//}


//#define FF(a,b,c,d,Mj,s,ti)		a=b+leftrot((a+F(b,c,d)+Mj+ti),s);
//#define GG(a,b,c,d,Mj,s,ti)		a=b+leftrot((a+G(b,c,d)+Mj+ti),s);
//#define HH(a,b,c,d,Mj,s,ti)		a=b+leftrot((a+H(b,c,d)+Mj+ti),s);
//#define II(a,b,c,d,Mj,s,ti)		a=b+leftrot((a+I(b,c,d)+Mj+ti),s);

//#define FF(a, b, c, d, x, s, ac) { \
// (a) += F ((b), (c), (d)) + (x) + (uint32_t)(ac); \
// (a) = ROTATE_LEFT ((a), (s)); \
// (a) += (b); \
//}
//#define GG(a, b, c, d, x, s, ac) { \
// (a) += G ((b), (c), (d)) + (x) + (uint32_t)(ac); \
// (a) = ROTATE_LEFT ((a), (s)); \
// (a) += (b); \
//}
//#define HH(a, b, c, d, x, s, ac) { \
// (a) += H ((b), (c), (d)) + (x) + (uint32_t)(ac); \
// (a) = ROTATE_LEFT ((a), (s)); \
// (a) += (b); \
//}
//#define II(a, b, c, d, x, s, ac) { \
// (a) += I ((b), (c), (d)) + (x) + (uint32_t)(ac); \
// (a) = ROTATE_LEFT ((a), (s)); \
// (a) += (b); \
//}

//void FF(uint32_t *a0,uint32_t b0,uint32_t c0,uint32_t d0,uint32_t Mj,uint8_t s0,uint32_t ti)
//{
//	uint32_t bbb,ccc;//
//	
////	printf("\r\nF(%x,%x,%x)=",b0,c0,d0);	
////	bbb=F(b0,c0,d0);
////	printf("%x",bbb);
////	
////	printf("\r\n%x+%x+%x+%x=",*a0,bbb,Mj,ti);
////	bbb=*a0+bbb+Mj+ti;
////	printf("%x",bbb);

////	printf("\r\n%x<<<%x=",bbb,s0);
////	bbb=leftrot(bbb,s0);
////	printf("%x",bbb);
////	
////	printf("\r\n%x+%x=",b0,bbb);
////	bbb=bbb+b0;
////	printf("%x",bbb);
//	*a0=b0 + leftrot((*a0+F(b0,c0,d0)+Mj+ti),s0);
//}

//void GG(uint32_t *a0,uint32_t b0,uint32_t c0,uint32_t d0,uint32_t Mj,uint8_t s0,uint32_t ti)
//{
//	*a0=b0 + leftrot((*a0+G(b0,c0,d0)+Mj+ti),s0);
//}

//void HH(uint32_t *a0,uint32_t b0,uint32_t c0,uint32_t d0,uint32_t Mj,uint8_t s0,uint32_t ti)
//{
//	*a0=b0 + leftrot((*a0+H(b0,c0,d0)+Mj+ti),s0);
//}

//void II(uint32_t *a0,uint32_t b0,uint32_t c0,uint32_t d0,uint32_t Mj,uint8_t s0,uint32_t ti)
//{
//	*a0=b0 + leftrot((*a0+I(b0,c0,d0)+Mj+ti),s0);
//}

//FF(a,b,c,d,Mj,s,ti)表示a=b+((a+(F(b,c,d)+Mj+ti)<<<s)
//GG(a,b,c,d,Mj,s,ti)表示a=b+((a+(G(b,c,d)+Mj+ti)<<<s)
//HH(a,b,c,d,Mj,s,ti)表示a=b+((a+(H(b,c,d)+Mj+ti)<<<s)
//II(a,b,c,d,Mj,s,ti)表示a=b+((a+(I(b,c,d)+Mj+ti)<<<s)
	
uint32_t 	a,b,c,d;

#if 0
void MD5_computer(uint8_t *indat,uint16_t len) //len有64位 
{
	uint16_t zoremax,i;
	uint8_t 	buf8[1000];
	uint32_t 	M[1000];
	
//len=sizeof(indat);
	printf("\r\nlen=%x",len);
	
	for(i=0;i<len; i++)
	{
			buf8[i]=indat[i];
	}
	buf8[len]=0x80;
	for(i=(len+1);i<56; i +=4)
	{
		buf8[i]=0;
	}
	printf("\r\n%s",buf8);
	
	printf("\r\n");
	for(i=0;i<64; i ++)	printf("%x,",buf8[i]);
	
	for(i=0;i<56; i +=4)
	{
		M[i/4]=((uint32_t) buf8[i]<<24)+((uint32_t)buf8[i+1]<<16)+((uint32_t)buf8[i+2]<<8)+((uint32_t)buf8[i+3]<<0);
//		M[i/4]=((uint32_t) buf8[i]<<0)+((uint32_t)buf8[i+1]<<8)+((uint32_t)buf8[i+2]<<16)+((uint32_t)buf8[i+3]<<24);
	}
	M[14]=0;		//64位长度
	M[15]=len*8;//64位长度
	
	printf("\r\n");
	for(i=0;i<16; i ++)	printf("%x,",M[i]);
	
	
	a=staA;//leftrot(staA,8);//  
	b=staB;
	c=staC;
	d=staD;
	printf("\r\n%x,%x,%x,%x",a,b,c,d);
//		第一轮	
		FF(&a,b,c,d,M[0],7,0xd76aa478);
		FF(&d,a,b,c,M[1],12,0xe8c7b756);
		FF(&c,d,a,b,M[2],17,0x242070db);
		FF(&b,c,d,a,M[3],22,0xc1bdceee);
		FF(&a,b,c,d,M[4],7,0xf57c0faf);
		FF(&d,a,b,c,M[5],12,0x4787c62a);
		FF(&c,d,a,b,M[6],17,0xa8304613);
		FF(&b,c,d,a,M[7],22,0xfd469501);
		FF(&a,b,c,d,M[8],7,0x698098d8);
		FF(&d,a,b,c,M[9],12,0x8b44f7af);
		FF(&c,d,a,b,M[10],17,0xffff5bb1);
		FF(&b,c,d,a,M[11],22,0x895cd7be);
		FF(&a,b,c,d,M[12],7,0x6b901122);
		FF(&d,a,b,c,M[13],12,0xfd987193);
		FF(&c,d,a,b,M[14],17,0xa679438e);
		FF(&b,c,d,a,M[15],22,0x49b40821);
//		第二轮
		GG(&a,b,c,d,M[1],5,0xf61e2562);
		GG(&d,a,b,c,M[6],9,0xc040b340);
		GG(&c,d,a,b,M[11],14,0x265e5a51);
		GG(&b,c,d,a,M[0],20,0xe9b6c7aa);
		GG(&a,b,c,d,M[5],5,0xd62f105d);
		GG(&d,a,b,c,M[10],9,0x02441453);
		GG(&c,d,a,b,M[15],14,0xd8a1e681);
		GG(&b,c,d,a,M[4],20,0xe7d3fbc8);
		GG(&a,b,c,d,M[9],5,0x21e1cde6);
		GG(&d,a,b,c,M[14],9,0xc33707d6);
		GG(&c,d,a,b,M[3],14,0xf4d50d87);
		GG(&b,c,d,a,M[8],20,0x455a14ed);
		GG(&a,b,c,d,M[13],5,0xa9e3e905);
		GG(&d,a,b,c,M[2],9,0xfcefa3f8);
		GG(&c,d,a,b,M[7],14,0x676f02d9);
		GG(&b,c,d,a,M[12],20,0x8d2a4c8a);
//		第三轮
		HH(&a,b,c,d,M[5],4,0xfffa3942);
		HH(&d,a,b,c,M[8],11,0x8771f681);
		HH(&c,d,a,b,M[11],16,0x6d9d6122);
		HH(&b,c,d,a,M[14],23,0xfde5380c);
		HH(&a,b,c,d,M[1],4,0xa4beea44);
		HH(&d,a,b,c,M[4],11,0x4bdecfa9);
		HH(&c,d,a,b,M[7],16,0xf6bb4b60);
		HH(&b,c,d,a,M[10],23,0xbebfbc70);
		HH(&a,b,c,d,M[13],4,0x289b7ec6);
		HH(&d,a,b,c,M[0],11,0xeaa127fa);
		HH(&c,d,a,b,M[3],16,0xd4ef3085);
		HH(&b,c,d,a,M[6],23,0x04881d05);
		HH(&a,b,c,d,M[9],4,0xd9d4d039);
		HH(&d,a,b,c,M[12],11,0xe6db99e5);
		HH(&c,d,a,b,M[15],16,0x1fa27cf8);
		HH(&b,c,d,a,M[2],23,0xc4ac5665);
//		第四轮
		II(&a,b,c,d,M[0],6,0xf4292244);
		II(&d,a,b,c,M[7],10,0x432aff97);
		II(&c,d,a,b,M[14],15,0xab9423a7);
		II(&b,c,d,a,M[5],21,0xfc93a039);
		II(&a,b,c,d,M[12],6,0x655b59c3);
		II(&d,a,b,c,M[3],10,0x8f0ccc92);
		II(&c,d,a,b,M[10],15,0xffeff47d);
		II(&b,c,d,a,M[1],21,0x85845dd1);
		II(&a,b,c,d,M[8],6,0x6fa87e4f);
		II(&d,a,b,c,M[15],10,0xfe2ce6e0);
		II(&c,d,a,b,M[6],15,0xa3014314);
		II(&b,c,d,a,M[13],21,0x4e0811a1);
		II(&a,b,c,d,M[4],6,0xf7537e82);
		II(&d,a,b,c,M[11],10,0xbd3af235);
		II(&c,d,a,b,M[2],15,0x2ad7d2bb);
		II(&b,c,d,a,M[9],21,0xeb86d391);

		printf("\r\n%x,%x,%x,%x",a,b,c,d);
	a+=staA;//leftrot(staA,8);//  
	b+=staB;
	c+=staC;
	d+=staD;
	
		printf("\r\n%x,%x,%x,%x",a,b,c,d);
		
		printf("\r\n\r\n");
}
#endif
int8_t buf2[100];

void MD5Transform(unsigned int state[4], unsigned char block[64]);

void MD5Update(MD5_CTX *context, unsigned char *input, unsigned int inputlen);
void MD5Init(MD5_CTX *context);
void MD5Final(MD5_CTX *context, unsigned char digest[16]);

uint32_t st[4]={
0x67452301,
0xEFCDAB89,
0x98BADCFE,
0x10325476,
};

void MD5CC(MD5_CTX *context);
void md5_char32(MD5_CTX *p_md5, const uint8_t *in, uint32_t in_len, uint8_t *out, uint8_t lower_upper);
	
void MD5_TEST(void)
{
	unsigned int state[4];
	unsigned char block[64],i;
	
	MD5_CTX p_md5;
	
	
	
	
	
	
		
	printf("\r\n计算MD5");
	strcpy((char *)buf2,"1");//23456
	
//	md5_char32(&p_md5, (uint8_t *)buf2, 1, block, 0);
//	printf("\r\n%s",block);
	
	
	
	MD5Init(&p_md5);  

	printf("\r\n");
	for(i=0;i<4;i++)	printf("%x,",p_md5.state[i]);
	
	for(i=0;i<1;i++) p_md5.buffer[i]=buf2[i];
	p_md5.count[0]=8;
	
	MD5Final(&p_md5, block);
		
	printf("\r\n");
	for(i=0;i<4;i++)	printf("%x,",p_md5.state[i]);
	
	
	
	
//	MD5_computer((uint8_t *)buf2,(uint16_t )strlen((char *)buf2));
	for(i=0;i<4;i++)	state[i]=st[i];
	
	for(i=0;i<1;i++) block[i]=buf2[i]; //数据
	block[1]=0x80;				//补1
	for(i=2;i<56;i++) block[i] = 0; //补0
	
	for(i=56;i<64;i++) block[i] = 0; //补长度
	block[56] = 8;//补长度


	MD5Transform(state, block);
	
	printf("\r\n%x,%x,%x,%x",state[0],state[1],state[2],state[3]);
}


//uint32_t t[4][16]=
//{
//0xd76aa478,0xe8c7b756,0x242070db,0xc1bdceee,0xf57c0faf,0x4787c62a,0xa8304613,0xfd469501,
//0x698098d8,0x8b44f7af,0xffff5bb1,0x895cd7be,0x6b901122,0xfd987193,0xa679438e,0x49b40821,
//	
//0xf61e2562,0xc040b340,0x265e5a51,0xe9b6c7aa,0xd62f105d,0x02441453,0xd8a1e681,0xe7d3fbc8,
//0x21e1cde6,0xc33707d6,0xf4d50d87,0x455a14ed,0xa9e3e905,0xfcefa3f8,0x676f02d9,0x8d2a4c8a,	
//		
//0xfffa3942,0x8771f681,0x6d9d6122,0xfde5380c,0xa4beea44,0x4bdecfa9,0xf6bb4b60,0xbebfbc70,
//0x289b7ec6,0xeaa127fa,0xd4ef3085,0x04881d05,0xd9d4d039,0xe6db99e5,0x1fa27cf8,0xc4ac5665,
//		
//0xf4292244,0x432aff97,0xab9423a7,0xfc93a039,0x655b59c3,0x8f0ccc92,0xffeff47d,0x85845dd1,
//0x6fa87e4f,0xfe2ce6e0,0xa3014314,0x4e0811a1,0xf7537e82,0xbd3af235,0x2ad7d2bb,0xeb86d391,
//};
////		第一轮	
//		a=FF(a,b,c,d,M[0],7,0xd76aa478);
//		d=FF(d,a,b,c,M[1],12,0xe8c7b756);
//		c=FF(c,d,a,b,M[2],17,0x242070db);
//		b=FF(b,c,d,a,M[3],22,0xc1bdceee);
//		a=FF(a,b,c,d,M[4],7,0xf57c0faf);
//		d=FF(d,a,b,c,M[5],12,0x4787c62a);
//		c=FF(c,d,a,b,M[6],17,0xa8304613);
//		b=FF(b,c,d,a,M[7],22,0xfd469501);
//		a=FF(a,b,c,d,M[8],7,0x698098d8);
//		d=FF(d,a,b,c,M[9],12,0x8b44f7af);
//		c=FF(c,d,a,b,M[10],17,0xffff5bb1);
//		b=FF(b,c,d,a,M[11],22,0x895cd7be);
//		a=FF(a,b,c,d,M[12],7,0x6b901122);
//		d=FF(d,a,b,c,M[13],12,0xfd987193);
//		c=FF(c,d,a,b,M[14],17,0xa679438e);
//		b=FF(b,c,d,a,M[15],22,0x49b40821);
////		第二轮
//		a=GG(a,b,c,d,M[1],5,0xf61e2562);
//		d=GG(d,a,b,c,M[6],9,0xc040b340);
//		c=GG(c,d,a,b,M[11],14,0x265e5a51);
//		b=GG(b,c,d,a,M[0],20,0xe9b6c7aa);
//		a=GG(a,b,c,d,M[5],5,0xd62f105d);
//		d=GG(d,a,b,c,M[10],9,0x02441453);
//		c=GG(c,d,a,b,M[15],14,0xd8a1e681);
//		=GG(b,c,d,a,M[4],20,0xe7d3fbc8);
//		=GG(a,b,c,d,M[9],5,0x21e1cde6);
//		=GG(d,a,b,c,M[14],9,0xc33707d6);
//		=GG(c,d,a,b,M[3],14,0xf4d50d87);
//		=GG(b,c,d,a,M[8],20,0x455a14ed);
//		=GG(a,b,c,d,M[13],5,0xa9e3e905);
//		=GG(d,a,b,c,M[2],9,0xfcefa3f8);
//		=GG(c,d,a,b,M[7],14,0x676f02d9);
//		=GG(b,c,d,a,M[12],20,0x8d2a4c8a);
////		第三轮
//		=HH(a,b,c,d,M[5],4,0xfffa3942);
//		=HH(d,a,b,c,M[8],11,0x8771f681);
//		=HH(c,d,a,b,M[11],16,0x6d9d6122);
//		=HH(b,c,d,a,M[14],23,0xfde5380c);
//		=HH(a,b,c,d,M[1],4,0xa4beea44);
//		=HH(d,a,b,c,M[4],11,0x4bdecfa9);
//		=HH(c,d,a,b,M[7],16,0xf6bb4b60);
//		=HH(b,c,d,a,M[10],23,0xbebfbc70);
//		=HH(a,b,c,d,M[13],4,0x289b7ec6);
//		=HH(d,a,b,c,M[0],11,0xeaa127fa);
//		=HH(c,d,a,b,M[3],16,0xd4ef3085);
//		=HH(b,c,d,a,M[6],23,0x04881d05);
//		=HH(a,b,c,d,M[9],4,0xd9d4d039);
//		=HH(d,a,b,c,M[12],11,0xe6db99e5);
//		=HH(c,d,a,b,M[15],16,0x1fa27cf8);
//		=HH(b,c,d,a,M[2],23,0xc4ac5665);
////		第四轮
//		=II(a,b,c,d,M[0],6,0xf4292244);
//		=II(d,a,b,c,M[7],10,0x432aff97);
//		=II(c,d,a,b,M[14],15,0xab9423a7);
//		=II(b,c,d,a,M[5],21,0xfc93a039);
//		=II(a,b,c,d,M[12],6,0x655b59c3);
//		=II(d,a,b,c,M[3],10,0x8f0ccc92);
//		=II(c,d,a,b,M[10],15,0xffeff47d);
//		=II(b,c,d,a,M[1],21,0x85845dd1);
//		=II(a,b,c,d,M[8],6,0x6fa87e4f);
//		=II(d,a,b,c,M[15],10,0xfe2ce6e0);
//		=II(c,d,a,b,M[6],15,0xa3014314);
//		=II(b,c,d,a,M[13],21,0x4e0811a1);
//		=II(a,b,c,d,M[4],6,0xf7537e82);
//		=II(d,a,b,c,M[11],10,0xbd3af235);
//		=II(c,d,a,b,M[2],15,0x2ad7d2bb);
//		=II(b,c,d,a,M[9],21,0xeb86d391);