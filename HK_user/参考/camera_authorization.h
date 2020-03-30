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
**--------------------------------------------------------------------------------------------------------
** Created by:              feiyun.wu
** Created date:            2018-02-07
** Version:                 1.0.0
** Descriptions:            Í·ÎÄ¼þ
**
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Descriptions:
**
*********************************************************************************************************/
#ifndef	__CAMERA_AUTHORIZATION_H_
#define	__CAMERA_AUTHORIZATION_H_

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#include <stdint.h>
#include "lwip/api.h"


#define USERNAME_STR					"admin"
#define PASSWORD_STR					"scisun3206"


extern int method_head_pkg(const uint8_t *method, uint16_t method_len,
					const uint8_t *uri, uint16_t uri_len,
					const uint8_t *ip_host, uint8_t ip_host_len,
					const uint8_t *str, uint16_t str_len,
					uint8_t *out, uint16_t *p_out_len);

extern int camera_authorization(const uint8_t *in, uint16_t in_len,
								const uint8_t *method, uint16_t method_len,
								const uint8_t *uri, uint16_t uri_len,
								const uint8_t *ip_host, uint8_t ip_host_len,
								const uint8_t *str, uint16_t str_len,
								struct netconn *conn);
//extern void camera_authorization_test(void);


#ifdef __cplusplus
}  //extern "C"
#endif //__cplusplus


#endif                                                                  /* __CAMERA_AUTHORIZATION_H_ */
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
