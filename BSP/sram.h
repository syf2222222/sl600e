#ifndef __SRAM__H
#define __SRAM__H

#include "stm32l4xx_hal.h"



#define sys_buf_sram			(( uint8_t*) (FMC_BANK1_1)) 	//起始码 0x68
#define updata_flag				(( uint8_t*) (FMC_BANK1_1))
//升级标志备用

#define STAA			(( uint8_t*) (sys_buf_sram+0x09)) 	//起始码 0x68
#define TID				(( uint8_t*) (STAA+1))	//ID码 CC0196,  TID 偶数地址开始

#define CHW				(( uint8_t*) (TID+6))		//密文验证："1234"
#define PSW				(( uint8_t*) (CHW+4))		//终端密码  "1234"
#define SV				(( uint8_t*) (PSW+4))		//电压修正参数?版本号?

#define HBT				(( uint16_t*) ((( uint16_t*)SV)+1))		//心跳时间
#define GDT				(( uint16_t*) (HBT+1))	//采样时间
#define SLT				(( uint16_t*) (GDT+1))	//睡眠时间
#define OLT				(( uint16_t*) (SLT+1))	//在线时间
#define RST				(( uint8_t*) (OLT+1))	  //复位时间

#define HostIP			(( uint8_t*) (RST+4))				//服务器IP
#define HostPort		(( uint16_t*) ((( uint16_t*)HostIP)+2))		//服务器端口
#define LoclIP			(( uint8_t*) (HostPort+1))	//本地IP
#define GWIP				(( uint8_t*) (LoclIP+4))		//
#define CH1IP				(( uint8_t*) (GWIP+4))			//通道1IP
#define CH2IP				(( uint8_t*) (CH1IP+4))			//通道2IP
#define NetMst			(( uint8_t*) (CH2IP+4))			//

#define WidthxHeight	(( uint8_t*) (NetMst+4))	
#define bLevel	(( uint8_t*) (WidthxHeight+1))
#define cLevel	(( uint8_t*) (bLevel+1))
#define sLevel	(( uint8_t*) (cLevel+1))

#define runningtime_buf (( uint8_t*) (sLevel+1))  //

#define function_nub	(( uint8_t*) (runningtime_buf+4))  //
#define function_buf	(( uint8_t*) (function_nub+1))  //

//GLOBAL_EXT unsigned char function_buf[20],function_nub;



//unsigned char WidthxHeight,bLevel,cLevel,sLevel;

//前面256字节

#define aut_point_group1			(( uint8_t*)  (FMC_BANK1_1+0x100)) //自动拍照时间1
#define aut_get_point1			((struct aut_get_photo*) (aut_point_group1+1)) //3*256
	
#define aut_point_group2			(( uint8_t*)  (aut_point_group1+0x400))//自动拍照时间2
#define aut_get_point2			((struct aut_get_photo*) (aut_point_group2+1)) //3*256
	
#define buf_ll			( (FMC_BANK1_1+0x100+0x400+0x400))			//( uint8_t*) 
#define buf_qx			( (buf_ll+0xc00))		//( uint8_t*) 

//#define aut_point_group1			(( uint8_t*)  (buf_qx+0x400)) //自动拍照时间1
//#define aut_get_point1			((struct aut_get_photo*) (aut_point_group1+1)) //3*256
//	
//#define aut_point_group2			(( uint8_t*)  (aut_point_group1+0x400))//自动拍照时间2
//#define aut_get_point2			((struct aut_get_photo*) (aut_point_group2+1)) //3*256
	
#define urd1						(( uint8_t*)  (buf_qx+0x400))
#define urd2						(( uint8_t*)  (urd1+0x200))
#define urd3						(( uint8_t*)  (urd2+0x200))
#define urd5						(( uint8_t*)  (urd3+0x800))

#define http_sendbuf		(( uint8_t*)  (urd5+0x400))
//#define http_recvbuf		(( uint8_t*)  (http_sendbuf+0x800))





//#define PH_flag			(( uint8_t*) (http_sendbuf+0x800))	//
#define PH_GET_T		(( uint8_t*) (http_sendbuf+0x800))
#define PH_GET_CH		(( uint8_t*) (PH_GET_T+6))
#define PH_GET_PS		(( uint8_t*) (PH_GET_CH+1))
#define PH_LEN			(( uint8_t*) (PH_GET_PS+1))
#define PH_DATA			(( uint8_t*) (PH_LEN+10))


#define PH_DATA2			(( uint8_t*) (PH_DATA+4))



#endif  /* __SRAM__H */

