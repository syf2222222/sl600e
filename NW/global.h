

#ifndef	__GLOBAL_H
#define	__GLOBAL_H

#ifdef   NW_MODULE
#define  GLOBAL_EXT
#else
#define  GLOBAL_EXT  extern
#endif



#include 	"stdint.h"		//uint 定义
#include "mydefine.h"
#include <stdio.h>			//printf();
#include <string.h>			//字符操作
#include  <os.h>
#include <main.h>			//包含那些HAL功能
#include <power_ctrl.h>			

//#include "stm32l4xx_hal_def.h"
//extern void UART_data_clear(UART_HandleTypeDef *huart);	
//char UART_Receive(UART_HandleTypeDef *huart,uint8_t **pack_point, uint16_t *pack_len,int wait_time);
//char UART_Receive_s(UART_HandleTypeDef *huart,uint8_t **pack_point, uint16_t *pack_len,int time_out);

#define	data_flag00				0x0001
#define	data_flag01				0x0002
#define	data_flag05				0x0004
#define	data_flag05_ok		0x0008

#define	data_flag06				0x0010
#define	data_flag0a				0x0020
#define	data_flag22				0x0040
#define	data_flag25				0x0080

#define	data_flag84_1		0x0100		//请求上送
#define	data_flag86_1		0x0200		//上送结束
#define	data_flag84_2		0x0400		//请求上送
#define	data_flag86_2		0x0800		//上送结束

#define	data_flag81_1		0x1000		//配置图像参数
#define	data_flag81_2		0x2000		//配置图像参数


GLOBAL_EXT uint32_t	senttask_Asim;	//assignment  被分派的任务
GLOBAL_EXT uint32_t	senttask_Pend;	//等待结果


#define	gat_data_time			0x0001		//
#define	gat_a_ll_req			0x0002		//
GLOBAL_EXT uint32_t	datagettask_Asim;

GLOBAL_EXT uint16_t ll_sent_cnt;
GLOBAL_EXT uint16_t ll_gain_cnt;

GLOBAL_EXT uint16_t qx_sent_cnt;
GLOBAL_EXT uint16_t qx_gain_cnt;

GLOBAL_EXT uint16_t ph_sent_cnt;
GLOBAL_EXT uint16_t ph_gain_cnt;



GLOBAL_EXT uint32_t	hktask_Pend;	//等待结果

#define HK_post_max	520
GLOBAL_EXT uint8_t HK_post_buf[HK_post_max+10];//
GLOBAL_EXT uint16_t HK_post_cnt;

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
//11		光圈放大1个单位
//12		光圈缩小1个单位
//13		聚焦增加1个单位
//14		聚焦减少1个单位
//15	巡航号	开始巡航
//16	巡航号	停止巡航
//17	辅助开关号	打开辅助开关
//18	辅助开关号	关闭辅助开关
//19	0	开始自动扫描
//20	0	停止自动扫描
//21	0	开始随机扫描
//22	0	停止随机扫描
//23	0	红外灯全开
//24	0	红外灯半开
//25	0	红外灯关闭
//26 	所需预置位号	删除预置位号
//27	0	设置自动扫描左边界
//28	0	设置自动扫描右边界
//29	0	设置自动扫描速度（1-10量化）
//30	每单位的步长	设置云台上下左右调节时，“1个单位”的步长。
//31	巡检组号	开始巡检
//32	巡检组号	停止巡检
//48	0	停止摄像机动作
//49	速度	开始摄像机向上运动
//50	速度	开始摄像机向下运动
//51	速度	开始摄像机向左运动
//52	速度	开始摄像机向右运动
//53	速度	开始摄像机向左上运动
//54	速度	开始摄像机向右上运动
//55	速度	开始摄像机向左下运动
//56	速度	开始摄像机向右下运动
//57	0	开始摄像机焦距向远方调节（镜头变倍放大）
//58	0	开始摄像机焦距向近处调节（镜头变倍缩小） 
//59	速度	开始摄像机光圈放大
//60	速度	开始摄像机光圈缩小
//61	速度	开始摄像机聚焦增加
//62	速度	开始摄像机聚焦减少


//密码	通道号	动作指令	指令参数
//4字节	1字节		1字节			1字节

//0x88指令下的 指令 1-62
#define		hk_pwr_on					1
#define		hk_preset_goto		2
#define		hk_up							3
#define		hk_down						4
#define		hk_left						5
#define		hk_right					6
#define		hk_pwr_off				10
#define		hk_preset					9


#define		hk_par_cfg				0x81		//parameter configuration
#define		hk_get_ph					0x83		//parameter configuration





GLOBAL_EXT OS_Q 			hktask_Asim[2];				//海康等待任务
GLOBAL_EXT OS_Q 			http_recv[2];					//海康等待HTTP接收数据


//uint32_t ask_data_flag;
//uint32_t rep_data_flag;
//typedef 

typedef enum
{
  bempty = 0,
	bpre_bready,
  bready,
	
  bpre_load_hk,
	bloading_hk,
	
	bpre_sent_buf,
  bsenting_buf,
	
} buf_state;


//enum
//{
//  sys_normal = 0,
//	sys_reset_wait,
//  sys_reset_ing,
//	sys_sleep_wait,
//	sys_sleep_ing
//		
//} sys_state;

GLOBAL_EXT buf_state		PH_sram_buf;


typedef struct __NWD_HandleTypeDef
{
	uint16_t        sav_nb;     
	uint16_t        sav_Pt;  
	uint16_t				sav_add;
	buf_state				state;
	
}NWD_HandleTypeDef;

GLOBAL_EXT NWD_HandleTypeDef	hll[3],hqx,hph;

//#define test_mode
//#define test_mode2

#ifdef test_mode
#define		hk_power_on_time	200
#define testdvi	5
#else
#define		hk_power_on_time	10

#ifdef test_mode2
#define testdvi	5
#else
#define testdvi	1
#endif

#endif

#define delayT	10

#define		nw_pk_len		1024 //1460 1040-16
#define		nw_pk_max		200000

#define		y_min		19
#define		y_max		50
#endif	/*__GLOBAL_H*/


#define ur3_rdma


GLOBAL_EXT unsigned char signal,volt;//nw_sent_buf[50],nw_recv_buf[50],
//GLOBAL_EXT unsigned char function_buf[20],function_nub;


int UART_Receive_s(UART_HandleTypeDef *huart,uint8_t **pack_point, uint16_t *pack_len,int time_out);


extern unsigned int runningtime;










