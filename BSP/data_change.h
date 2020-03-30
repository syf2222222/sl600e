
#ifndef  __DATA_CHANGE_H
#define  __DATA_CHANGE_H

#include "stdint.h"


//static uint8_t num_to_char(uint8_t ch, uint8_t lower_upper)
extern uint8_t u4_to_c8(uint8_t ch, uint8_t lower_upper);	//16����ת�ַ���4λ��8λ��lower_upper����Сд


//static void byte16_to_char32(const uint8_t *in, uint8_t *out, uint8_t lower_upper)
extern void u8_to_c8(const uint8_t *in, uint8_t *out,uint8_t len, uint8_t lower_upper); //16����ת�ַ���8λ���鵽8λ���飬lenΪ���볤��


extern int c8_to_d8(const uint8_t *in, uint8_t *out,uint8_t len); //ת����������������

extern int c8_to_u32_d(const uint8_t *in,uint8_t len);
extern int c8_to_u32_x(const uint8_t *in,uint8_t len);

extern int c8_to_u8(const uint8_t *in, uint8_t *out,uint8_t len); //�ַ�ת16���ƣ�8λ���鵽8λ���飬lenΪ���볤��
	
#endif /* __DATA_CHANGE_H */

