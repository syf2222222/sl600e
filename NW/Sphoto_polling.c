



#include "global.h"
#include "nw.h"
#include "sram.h"

void nw_sent_st(struct nw_ctl *t_data);
unsigned short tt_pk_nb;//pk_nb,
unsigned int tt_len;


extern unsigned short sentdelay;
extern uint8_t waiting_data;

int read_data_ph(uint16_t rcnt,uint8_t *out);
int index_ph_dec(unsigned char *index);
int read_ph(unsigned char *in,unsigned char *out,int len);
//void  nw_sent_ph_time(uint8_t ch);
void  nw_sent_ph(unsigned char *input,uint16_t len,uint8_t ch,uint8_t preset,uint16_t peket);
int sent_ph_sd(uint8_t *index,uint8_t *data_in,int tt_len);


uint8_t ph_index_buf[11];

#define	sent_wait		0
#define	sent_sd			1
#define	sent_sram		2


extern uint8_t Contact_state;

uint8_t Sphoto_step=0,Sphoto_state,Sphoto_cnt,Sphoto_err,ReSphoto_cnt;
uint16_t Sphoto_time;

extern uint8_t sd_sta_h;

uint8_t function84_flag;

void resent_ph(uint8_t *data_in,uint16_t data_len)
{
	OS_ERR      err; 
	uint16_t i;
	
	if(Sphoto_state==sent_sd)
	{
//		printf("\r\nre_send photo,pk_nb=%d,tt_len=%d,",tt_pk_nb,tt_len);
//		sent_ph_sd(ph_index_buf,data_in,tt_len);
		if(sent_ph_sd(ph_index_buf,data_in,tt_len)==0)
		{
			sentdelay=delayT;
			Sphoto_time=10; //赋值1s, 3s后发
			ReSphoto_cnt=0;	//重发次数清零
			Sphoto_step=7;	
		}
		else
		{
			printf("\r\n重发打开文件失败，跳过此包");
			index_ph_dec(ph_index_buf); //未发包数量减一
			Sphoto_step=0;
		}
	}
	else
	{
		unsigned short pk_nb;
		for(i=0;i<data_in[2];i++) //data_in[2] 需补包的数量
		{			
			OSTimeDly( 20, OS_OPT_TIME_DLY, & err);
			pk_nb=(data_in[2*i+3]<<8)+data_in[2*i+4]; //补包的 包值
			if(pk_nb==tt_pk_nb) nw_sent_ph(PH_DATA+((pk_nb-1)*nw_pk_len),tt_len-((pk_nb-1)*nw_pk_len),*PH_GET_CH,*PH_GET_PS,pk_nb);  //结尾包,长度为 tt_len-((pk_nb-1)*nw_pk_len
			else	nw_sent_ph(PH_DATA+((pk_nb-1)*nw_pk_len),nw_pk_len,*PH_GET_CH,*PH_GET_PS,pk_nb); //非结尾包 长度为 nw_pk_len
			printf("%d ",pk_nb);
		}
		sentdelay=delayT;
		Sphoto_time=10; //赋值1s, 3s后发
		ReSphoto_cnt=0; //重发次数清零
		Sphoto_step=7;	
	}

}

void Sphoto_polling(void)
{
//	OS_ERR      err; 
	
	switch(Sphoto_step)
	{
		case 0:
			if((ph_sent_cnt!=ph_gain_cnt)&&(sd_sta_h==1)&&(function84_flag))
			{
				Sphoto_state=sent_sd;
//				printf("\r\n准备发存储照片");
				Sphoto_step=2;
			}
			else if((PH_sram_buf==bready)&&(function84_flag))//数据准备好
			{
				PH_sram_buf=bpre_sent_buf;
				Sphoto_state=sent_sram;
//				printf("\r\n准备发缓存照片");
				Sphoto_step=3;
			}
			else
			{
				Sphoto_state=sent_wait;
				if((PH_sram_buf==bpre_sent_buf)||(PH_sram_buf==bsenting_buf))
				{
					PH_sram_buf=bempty;
				}
			}
			break;
		case 2:
			if(read_data_ph(ph_sent_cnt,ph_index_buf)==0)	//读取索引数据正确
			{
				Sphoto_step=4;
				Sphoto_cnt=0;	
			}
			else //读取索引数据错误
			{
				printf("\r\n///丢失一张图片,84读取存储索引数据错误");
				index_ph_dec(ph_index_buf); //跳过此包
				Sphoto_step=0;
			}
			break;
		
		case 3:
			if(PH_sram_buf==bpre_sent_buf) //还是在权
			{
				Sphoto_step=4;
				Sphoto_cnt=0;
				PH_sram_buf=bsenting_buf;
				memcpy(ph_index_buf,PH_GET_T,11);
			}
			else
			{
				printf("\r\n84数据冲突,PH_sram_buf=%d",PH_sram_buf);
				Sphoto_step=0;
			}
			break;
			
		case 4://请求上送照片 1s重发,要求3s,最多5次		
__Sphoto_polling_case4:
		
			nw_sc.cmd=0x84;
			nw_sc.len=10;		

			memcpy(nw_sc.data,ph_index_buf,8);
			tt_len=((ph_index_buf[8])<<0) +(ph_index_buf[9]<<8)+(ph_index_buf[10]<<16);
			
			
			if(tt_len>nw_pk_max) tt_len=nw_pk_max;
			tt_pk_nb=(tt_len/nw_pk_len);//1024一次
			if(tt_len%nw_pk_len) tt_pk_nb +=1;
//					printf("\r\nsend photo start0,pk_nb=%d,tt_len=%d,",tt_pk_nb,tt_len);					
			
			nw_sc.data[8]=(tt_pk_nb>>8);//包数高位
			nw_sc.data[9]=tt_pk_nb;			//包数低位
			
			nw_sent_st(&nw_sc);
			senttask_Pend |= data_flag84_1;
			Sphoto_step++;
			Sphoto_cnt++;
			Sphoto_time=0;
			break;
		
		case 5://等待答复
			if((senttask_Pend & data_flag84_1)==0)	//收到答复
			{
				Sphoto_time=0;
				Sphoto_step++;
//						printf("\r\n收到南网同意上传");
			}
			else if((Sphoto_time)>=30)
			{

//					printf("\r\n超时%x",sent_cnt);
				if(Sphoto_cnt>=5)
				{
//					if(Sphoto_state==sent_sram) index_ph_inc(PH_GET_T); //未发包数量加一,保存图片
					if(Sphoto_state==sent_sd)
					{
						if((ph_index_buf[0]<y_min)||(ph_index_buf[0]>y_max)||(ph_index_buf[1]==0)||(ph_index_buf[1]>12)||(ph_index_buf[2]==0)||(ph_index_buf[2]>31)) //时间为y_min-y_max年以外错误,0月
						{
							printf("\r\n时间数据错误,返回");
						}
					}
					Sphoto_step=0;
				}
				else  //在跳的期间收到答复?
				{
					Sphoto_step--;
					goto __Sphoto_polling_case4;
				}
			}
			break;
			
		case 6:	//开始上传照片
			if(((Sphoto_time>=1)&&(waiting_data==0))||(Sphoto_time>=30))	//等待100ms
			{
				if(Sphoto_state==sent_sd) 
				{
//					printf("\r\nsend photo,pk_nb=%d,tt_len=%d,",tt_pk_nb,tt_len);
//					sent_ph_all(read_buf,tt_len);
					if(sent_ph_sd(ph_index_buf,0,tt_len)==0)
					{
						sentdelay=delayT; 
						printf("\r\nsend photo complete");
						
						Sphoto_time=0;
						Sphoto_cnt=0;
						ReSphoto_cnt=0;
						Sphoto_step++;	//发完,准备发 0x86指令等南网应答
					}
					else
					{
						printf("\r\n///丢失一张图片,打开文件失败");
						index_ph_dec(ph_index_buf); //未发包数量减一
						Sphoto_step=0;
					}
				}
				else
				{
					unsigned short pk_nb;
					printf("\r\nsend photo,pk_nb=%d,tt_len=%d,sram",tt_pk_nb,tt_len);

					for(pk_nb=1;pk_nb<=tt_pk_nb;pk_nb++) //临时测试
					{
						if(pk_nb==tt_pk_nb) nw_sent_ph(PH_DATA+((pk_nb-1)*nw_pk_len),tt_len-((pk_nb-1)*nw_pk_len),*PH_GET_CH,*PH_GET_PS,pk_nb);
						else	nw_sent_ph(PH_DATA+((pk_nb-1)*nw_pk_len),nw_pk_len,*PH_GET_CH,*PH_GET_PS,pk_nb);
	//					printf("\r\nA");
	//					OSTimeDly ( 3, OS_OPT_TIME_DLY, & err ); 
					}
					
					sentdelay=delayT; 
					printf("\r\nsend photo complete");
					
					Sphoto_time=0;
					Sphoto_cnt=0;
					ReSphoto_cnt=0;
					Sphoto_step++;	//发完,准备发 0x86指令等南网应答
				}
				
				
			}
			break;

				
		case 7:	
			if(Sphoto_time>=30) //3s后
			{
__Photo_case7:				
				nw_sc.cmd=0x86;//数据结束
				nw_sc.len=2;			
				nw_sc.data[0]=*PH_GET_CH;//通道号	
				nw_sc.data[1]=*PH_GET_PS;//预置位号
				senttask_Pend |= data_flag86_1;
			
				nw_sent_st(&nw_sc);
				Sphoto_step=8;	
				Sphoto_time=0;
				Sphoto_cnt++;	//包括补包
				ReSphoto_cnt++; //每次补包后重计数
			}
			break;
			
		case 8:
			if((senttask_Pend & data_flag86_1)==0)
				{
					printf("\r\nsend photo OK");
					if(Sphoto_state==sent_sd)	index_ph_dec(ph_index_buf); //未发包数量减一
					Sphoto_step=0;
					Sphoto_err=0;
				}
				else if((Sphoto_time)>=300/testdvi)
				{
//					if(STM32_sleeping)
//					{
//						Sphoto_step=0; //等待休眠,先结束
//						if((Sphoto_state==sent_sram)&&(PH_sram_buf==bsenting_buf)) PH_sram_buf=bready;	//拍照任务没改变状态的话, 数据重新放到准备好	
//					}
//					else 
					if((Sphoto_cnt>=20)||(ReSphoto_cnt>=5)) //加上补包后的次数,总次数大于20次,或者单算重发次数大于5次, 发包失败
					{
						Sphoto_err++;
						if(Sphoto_err>=3) //错误3次,丢弃此包
						{
							if(Sphoto_state==sent_sd)	index_ph_dec(ph_index_buf); //未发包数量减一
							if((Sphoto_state==sent_sram)&&(PH_sram_buf==bsenting_buf))	PH_sram_buf=bempty;
							
							printf("\r\n///丢失一张图片,连续上传失败");
						}
						else
						{
							if((Sphoto_state==sent_sram)&&(PH_sram_buf==bsenting_buf)) PH_sram_buf=bready;	//拍照任务没改变状态的话, 数据重新放到准备好	
							//								if(Sphoto_state==sent_sd)//不处理
						}
						Contact_state=0;	//通讯错误
						Sphoto_step=0;
					}
					else
					{
						printf("\r\n超时重发86");
						Sphoto_step--;
//						nw_sent_st(&nw_sc);
//						Sphoto_time=0;
//						Sphoto_cnt++;
						goto __Photo_case7;
					}
				}
			break;
				
		default:
			
			Sphoto_step=0;
		
			break;
				
	}
}

