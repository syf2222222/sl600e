


#include "global.h"
#include "sram.h"


unsigned char sampled_data_nub;
extern uint8_t qx_data_buf[32] __attribute__((at(buf_qx)));
extern uint8_t ll_data_buf[3][32] __attribute__((at(buf_ll)));
int read_data_ll(uint16_t rcnt,uint8_t *out);
int read_data_qx(uint16_t rcnt,uint8_t *out);
int index_ll_dec(void);
int index_qx_dec(void);

uint8_t dt_read_buf[60];


void sent_ll_data_22(unsigned char *input,unsigned char zbs);
void sent_qx_data_25(unsigned char *input,unsigned char zbs);
extern uint8_t Contact_state;

uint8_t Sdata_step=0,Sdata_state,Sdata_cnt,Sdata_err;
uint16_t Sdata_time;

uint8_t function22_flag,function25_flag;

void Sdata_polling(void)
{
//	OS_ERR      err; 
	
	switch(Sdata_step)
	{
		case 0:
			if(senttask_Asim & data_flag22)
			{
				if(function22_flag)
				{
					if((ll_sent_cnt)!=ll_gain_cnt) Sdata_step=0x22;
					else senttask_Asim &= (~data_flag22);
				}
				else
				{
					senttask_Asim &= (~data_flag22);
				}
			}
			else if(senttask_Asim & data_flag25)
			{
				if(function25_flag)
				{
					if((qx_sent_cnt)!=qx_gain_cnt) Sdata_step=0x25;
					else senttask_Asim &= (~data_flag25);
				}
				else
				{
					senttask_Asim &= (~data_flag25);
				}
			}
			else if((ll_sent_cnt)!=ll_gain_cnt)	//有数据发
			{
				if(function22_flag) Sdata_step=0x22;
			}
			else if((qx_sent_cnt)!=qx_gain_cnt)	//有数据发
			{
				if(function25_flag) Sdata_step=0x25;
			}
			break;
		case 1:

			break;
			
		case 0x22: //
			if(read_data_ll(ll_sent_cnt,dt_read_buf)==0)
			{
				sampled_data_nub++;
				sent_ll_data_22(dt_read_buf,sampled_data_nub);
				Sdata_step++;
				Sdata_cnt++;
				Sdata_time=0;
			}
			else
			{
				printf("\r\n22数据校验错误");
				index_ll_dec(); //未发包数量减一
				Sdata_step=0;
			}
			break;
			
		case 0x23:
			if((senttask_Pend & data_flag22)==0)//发完一组 或没有数据需要发
			{
				index_ll_dec(); //未发包数量减一
				Sdata_step=0;
			}
			else if(Sdata_time>=30) //3s没收到回复,如果数据异常,需删除此数据?
			{
				printf("\r\n22指令答复超时");
				Contact_state=0;
				Sdata_step=0; //超时退出
			}
			break;
				
		case 0x25:	
			if(read_data_qx(qx_sent_cnt,dt_read_buf)==0)
			{
				sampled_data_nub++;
				sent_qx_data_25(dt_read_buf,sampled_data_nub);
				Sdata_step++;
				Sdata_cnt++;
				Sdata_time=0;
			}
			else
			{
				printf("\r\n25数据校验错误");
				index_qx_dec(); //未发包数量减一
				Sdata_step=0;
			}
		break;
			
		case 0x26:
			if((senttask_Pend & data_flag25)==0)	//发完一组 或没有数据需要发
			{
				index_qx_dec(); //未发包数量减一
				Sdata_step=0;
				Contact_state=1;
			}
			else if(Sdata_time>=30) //没收到回复
			{
				printf("\r\n25指令答复超时");
				Contact_state=0;
				Sdata_step=0; //超时退出
			}
			break;
	
			default:
				Sdata_step=0;
				break;
	}
}
