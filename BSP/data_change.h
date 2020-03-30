
#ifndef  __DATA_CHANGE_H
#define  __DATA_CHANGE_H

#include "stdint.h"


//static uint8_t num_to_char(uint8_t ch, uint8_t lower_upper)
extern uint8_t u4_to_c8(uint8_t ch, uint8_t lower_upper);	//16进制转字符，4位到8位，lower_upper定大小写


//static void byte16_to_char32(const uint8_t *in, uint8_t *out, uint8_t lower_upper)
extern void u8_to_c8(const uint8_t *in, uint8_t *out,uint8_t len, uint8_t lower_upper); //16进制转字符，8位数组到8位数组，len为输入长度


extern int c8_to_d8(const uint8_t *in, uint8_t *out,uint8_t len); //转南网的年月日数据

extern int c8_to_u32_d(const uint8_t *in,uint8_t len);
extern int c8_to_u32_x(const uint8_t *in,uint8_t len);

extern int c8_to_u8(const uint8_t *in, uint8_t *out,uint8_t len); //字符转16进制，8位数组到8位数组，len为输入长度
	
#endif /* __DATA_CHANGE_H */

