

#include "global.h"
#include "sram.h" 
#include "fatfs.h" 
#include "hk.h"
void power_set_cam1(uint8_t flag);

uint8_t preset_now[2];

//extern unsigned char WidthxHeight,bLevel,cLevel,sLevel;
uint8_t hk_state,http_sent_flag;
extern unsigned char http_linkd;
unsigned char power_buf_cam1;
extern int work_p100ms;
int wt_start,hk_wait_time,hk_tast_step;
extern uint8_t http_client_flag;		//TCP客户端数据发送标志位
extern OS_SEM http_rev_sem,http_data_sem;

int camera_authorization(char *sttp_buf,const char *method,const char *uri,const char ch);
int GET_photo(char *sttp_buf,char ch);
int GET_Streaming(char *sttp_buf,char ch);//查询图像大小
int GET_Streaming_capabilities(char *sttp_buf,char ch);// 查询可以配置的图像大小规格
int GET_Image(char *sttp_buf,char ch);//查询图像亮度,对比度,饱和度
int PTZCtrl_continuous(char *sttp_buf,signed char x,signed char y,signed char z,char ch);//Pan_Tilt_Zoom 上下左右大小移动
int PTZCtrl_presets(char *sttp_buf,char presetN,char ch);	//设置预置位
int PTZCtrl_presets_goto(char *sttp_buf,char presetN,char ch); //转到预置位
int PTZ(char *sttp_buf,char *command,char *mode,char speed,int waitime,char ch);//,char *mode,char speed,上下左右转的简单方式
int PTZ_Image(char *sttp_buf,signed char brightnessLevel,signed char contrastLevel,signed char saturationLevel,char ch);//更改图像亮度,对比度,饱和度
int PTZ_Streaming(char *sttp_buf,uint8_t hWidthxHeight,char ch);//更改图像大小参数，大小，色彩选择等

void get_digest_info(char *in);
int get_color_info(char *in);
int Content_Length(char **in);
int get_Width_Height_info(char *in);

void  nw_sent_88(uint8_t *t_data,uint8_t ch);


uint32_t data_len,data_rlen;
uint8_t waiting_data,recv_200,recv_401,recv_htty_end,recv_htty_err,http_linkd_old;//recv_htty_my,
int hk_last_stime;

extern unsigned char systime[6]; //20 (19年 9月 29日 9点 20分 47秒)
uint8_t sent_cmd;
extern uint8_t sd_sta_h;
//extern char write_ph(unsigned char *time,unsigned char *in,int len);

int f_open_s(int name);
int f_write_s(char * in,uint32_t len);
int f_close_s(void);
int f_lseek_s(void);

//extern uint16_t	ph_sent_cnt,ph_gain_cnt;

int index_ph_inc(uint8_t *in);
extern	FIL fp; 
extern	UINT bw;
extern	UINT br;
int flash_save(uint8_t *ADD,uint8_t *buf,int len);
extern uint8_t	STM32_reseting,STM32_sleeping;
extern int int_time_to_char_time(uint8_t *out,uint32_t sec); //从2000-1-1 0:0:0 分开始算
int last_ph_time;

void http_data_post(char * in,uint16_t len)//pending_http_data(char *in,int len,uint8_t sent_cmd,uint8_t ch)
{
//	int res;
	char *end;
	int i;
		
//		printf("收到\r\n%s\r\n",in); //收到数据http		
		
	if(strncmp("HTTP/1.1 401", (char *)in,12)==0)
	{
		printf("\r\nHTTP/1.1 401"); //收到数据http
		get_digest_info((char *)in); 	//取认证数据
		recv_401=1;
	}
	else	if(strncmp("HTTP/1.1 200 OK", (char *)in,15)==0)
	{
		printf("\r\nHTTP/1.1 200 OK"); //收到数据http
		
		end=in+15;
						
		if(sent_cmd==0x8b)	//查询亮度,对比度,饱和度
		{
//				printf("\r\n收到%s\r\n",in); //收到数据http		
			if(get_color_info(end)==0)	//找到值
			{
				recv_200=1;
				flash_save(bLevel,bLevel,3);
			}
			else
			{
				recv_200=1;
				printf("\r\n没找到颜色值"); 	
			}
		}
		else	if(sent_cmd==0x8a)	//查询图像大小
		{
//				printf("\r\n收到%s\r\n",in); //收到数据http	
			if(get_Width_Height_info(end)==0)
			{
				recv_200=1;
				flash_save(WidthxHeight,WidthxHeight,1);
			}
			else
			{
				recv_200=1;
				printf("\r\n没找到大小值"); 	
			}
		}
		else	if(sent_cmd==0x83)	//拍照
		{
			data_len=Content_Length(&end);
			data_rlen=0;
			PH_LEN[0]=0;
			PH_LEN[1]=0;
			PH_LEN[2]=0;
			PH_LEN[3]=0;
			
			if(last_ph_time>=runningtime) last_ph_time=last_ph_time+1;
			else last_ph_time=runningtime;
			int_time_to_char_time(PH_GET_T,last_ph_time); 
//			memcpy(PH_GET_T,systime,6);
			*PH_GET_CH=1;//ch
			*PH_GET_PS=preset_now[0];			
			recv_200=1;
			waiting_data=1;
		}
		else recv_200=1;
	}
	else if((waiting_data==1)&&(sent_cmd==0x83))
	{	
		memcpy(PH_DATA+data_rlen,in,len);
		data_rlen +=len;
		
		if(data_rlen>=data_len)
		{
			waiting_data=0;	
			PH_LEN[0]=(data_len>>0);
			PH_LEN[1]=(data_len>>8);
			PH_LEN[2]=(data_len>>16);
			PH_LEN[3]=(data_len>>24);
			recv_htty_end=1;
		}
		
		if(data_rlen==len)	//起始包
		{
			printf("\r\n");
			for(i=0;i<5;i++)	printf("%02x",in[i]); //打印前几个
			printf("\r\n");
		}
		else if(recv_htty_end) //结束包
		{
			printf("\r\n");
			for(i=len-5;i<len;i++)	printf("%02x",in[i]); //打印后几个				
			printf("\r\nphoto data rev complete len=%d",data_rlen);	
		}
		else
		{
			printf(".");
		}
	}
}


	
uint8_t hk_step;	
uint8_t sent_times,hk_tast_old,HK_Task_free=0;
extern uint8_t NBLOCK_flag,netconn_connect_time;
uint8_t netconn_err;	
extern OS_TCB   http_hk_thread_TCB;															//任务控制块
void Create_hk_thread(void);
extern uint16_t cam_on_time_detect;

extern uint8_t function84_flag;

void  HK_Task_ch1 ( void * p_arg )
{
	OS_ERR         err;
	OS_MSG_SIZE    msg_size;
//	char *in;
	
	uint8_t pMsg[5],*Get_Msg;
	
	int i;
	#define ch 1
		
	(void)p_arg;

					 
//  cpu_clk_freq = BSP_CPU_ClkFreq();                 //获取CPU时钟，时间戳是以该时钟计数

	hk_state=0;
	hk_wait_time=0;
	while (DEF_TRUE) {                                           //任务体
		OSTimeDly( 100, OS_OPT_TIME_DLY, & err);
		
		if(hk_tast_old != hk_tast_step)
		{
			hk_tast_old=hk_tast_step;
			hk_step=0;
			hk_wait_time=0;
//			sent_cnt=0;
		}
		if(hk_tast_step==2) //无卡状态,等待发完才进行下一张拍照
		{
			OSTimeDly ( 300, OS_OPT_TIME_DLY, & err ); 
			if(PH_sram_buf==bempty) hk_tast_step=1;
		}
		
		if(hk_tast_step==0)
		{			
			Get_Msg=OSQPend((OS_Q*			)&hktask_Asim[0],   
											(OS_TICK		)1000,		//6000,	//							
											(OS_OPT			)OS_OPT_PEND_BLOCKING,
											(OS_MSG_SIZE*	)&msg_size,		
											(CPU_TS*		)0,
											(OS_ERR*		)&err);
			
			
			if((err==OS_ERR_NONE)&&(function84_flag))		//收到南网消息
			{
				HK_Task_free=0;
				hk_wait_time=0;
				for(i=0;i<5;i++) pMsg[i]=Get_Msg[i];
				printf("\r\ncamera got command %02x",pMsg[0]);
				if(pMsg[0]==1)	//开电源
				{
					power_set_cam1(1);	//cam1 开关同时控制ETH	
					power_buf_cam1=1;
					nw_sent_88(pMsg,ch); //原命令返回
//					printf("打开球机电源");
				}
				else if(pMsg[0]==10)//关电源
				{
					power_set_cam1(0);
					
						nw_sent_88(pMsg,ch); //原命令返回
					power_buf_cam1=0;
//					printf("关闭球机电源");
				}
				else			
				{
					if((pMsg[0]==0x83)&&(sd_sta_h==0)&&((PH_sram_buf==bpre_bready)||(PH_sram_buf==bready)))
					{
						PH_sram_buf=bready;
						senttask_Asim |= data_flag84_1;	//照片OK，上传或者保存由主任务完成
						hk_tast_step=2;
					}
					else
					{
						if(power_flag_cam1==0)
						{
							power_set_cam1(1); //打开球机电源
						}
						
						if((pMsg[0]!=0x83)||(sd_sta_h==0))
						{
							if(PH_sram_buf==bpre_bready)
							{
								PH_sram_buf=bready;
								senttask_Asim |= data_flag84_1;	//照片OK，上传或者保存由主任务完成
	//							printf("\r\n照片传递给发送");
							}
							if((PH_sram_buf!=bpre_sent_buf)&&(PH_sram_buf!=bsenting_buf)&&(PH_sram_buf!=bready))//&&(PH_sram_buf!=bpre_sent_sd)&&(PH_sram_buf!=bsenting_sd)
							{
								PH_sram_buf=bempty;
							}
						}					
						hk_tast_step=1;
						http_sendbuf[0]=0;
						sent_cmd=pMsg[0];
						sent_times=0;
						hk_step=0;
						recv_401=0;
						recv_200=0;
						power_buf_cam1=1;
					}					
				}
			}
			else 	//没任务
			{
//				if((STM32_reseting)||(STM32_sleeping))
				if(STM32_reseting)
				{
					if((PH_sram_buf==bpre_bready)||(PH_sram_buf==bready)||(PH_sram_buf==bpre_sent_buf)||(PH_sram_buf==bsenting_buf)) //正在发的怎么处理?
					{
						PH_sram_buf=bpre_load_hk;
						if(index_ph_inc(PH_GET_T)==0)
						{
							PH_sram_buf=bempty;
							HK_Task_free=1;
						}
					}
				}
				else if(PH_sram_buf==bpre_bready) //睡眠前先发照片
				{
					PH_sram_buf=bready;
					senttask_Asim |= data_flag84_1;	//照片OK，上传或者保存由主任务完成
//					NW_Task_step =0;
				}
				else
				{
					HK_Task_free=1;
				}
				
				if((PH_sram_buf!=bpre_sent_buf)&&(PH_sram_buf!=bsenting_buf)&&(PH_sram_buf!=bready))//&&(PH_sram_buf!=bpre_sent_sd)&&(PH_sram_buf!=bsenting_sd)
				{
					PH_sram_buf=bempty;
				}
				if(((hk_wait_time)>600)&&(power_flag_cam1)) //超时没动作自动关闭
				{
					printf("\r\ncamera off by timeout");
					power_set_cam1(0); //打开球机电源
					power_buf_cam1=0; 
				}
			}
		}
		
		
		else	if(hk_tast_step==1)
		{
//			signed char x,y,z;
//			if(http_sent_flag)
//			{
//			}
			
			if(recv_401) //重新开始任务
			{
//				printf("\r\n重新认证");
				recv_401=0;
				hk_step=0;
				sent_cmd=pMsg[0];
				hk_wait_time=0;
			}
			
			if(http_linkd==0)
			{
				http_linkd_old=0;
				if(hk_wait_time>(hk_power_on_time+1800))//&&(http_sent_flag)) 
				{
					printf("\r\nhttp连接超时,球机复位重连");
					power_set_cam1(0);
					OSTimeDly ( 5000, OS_OPT_TIME_DLY, & err ); 
					power_set_cam1(1);
					hk_wait_time=0;
					hk_tast_step=0;
				}
				else
				{
//					printf("1");
					OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err ); 
					if(NBLOCK_flag) 
					{
						hk_wait_time=0;
						if(NBLOCK_flag==1)
						{
							printf("?"); //正与球机建立网络连接 netconn_connect
							netconn_err++;
//							if(netconn_err==10)
//							{
//								printf("球机连接异常,删除任务重建");
//								OSTaskDel(&http_hk_thread_TCB,& err );
//								OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err ); 
//								Create_hk_thread();
//							}
//							else 
								if(netconn_err>=180)
							{
								netconn_err=0;
								printf("球机连接异常，重启球机,删除任务重建");
								power_set_cam1(0);
								OSTimeDly ( 3000, OS_OPT_TIME_DLY, & err ); 
								power_set_cam1(1);
//								OSTaskDel(&http_hk_thread_TCB,& err );
//								OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err ); 
//								Create_hk_thread();
							}
//							HAL_NVIC_SystemReset(); //重启系统
						}
						if(NBLOCK_flag==2)
						{
							netconn_err=0;
							printf("*");
						}
						netconn_connect_time++;
					}
					else netconn_err=0;
				}
			}
			else //if(http_sent_flag==1)
			{
				netconn_err=0;
				if(http_linkd_old==0)
				{
					http_linkd_old=1;
					hk_wait_time=0;
				}
				if(hk_wait_time>100)
				{
					if(sent_times<8)
					{
						sent_times++;
						http_sent_flag=1; //重发
						hk_wait_time=0;
					}
					else
					{
						printf("\r\nTCP发超时,指令丢失");
						hk_tast_step=0;
					}
				}
			}
			
			
			
			
			
			switch(pMsg[0])//sent_cmd
			{					
				case 3:
				case 4:
				case 5:
				case 6:
				case 7://焦距向远方调节1个单位（镜头变倍放大）
				case 8://焦距向近处调节1个单位（镜头变倍缩小）
					if(hk_step==0)
					{
//						if(pMsg[0]==3)		{x=0;y=28;z=0;}
//						if(pMsg[0]==4)		{x=0;y=-28;z=0;}
//						if(pMsg[0]==5)		{x=-28;y=0;z=0;}					
//						if(pMsg[0]==6)		{x=28;y=0;z=0;}
//						if(pMsg[0]==7)		{x=0;y=0;z=-28;}					
//						if(pMsg[0]==8)		{x=0;y=0;z=28;}
						
						if(pMsg[0]==3) PTZCtrl_continuous((char *)http_sendbuf,0,28,120,ch);
						if(pMsg[0]==4) PTZCtrl_continuous((char *)http_sendbuf,0,-28,120,ch);
						if(pMsg[0]==5) PTZCtrl_continuous((char *)http_sendbuf,-28,0,120,ch);
						if(pMsg[0]==6) PTZCtrl_continuous((char *)http_sendbuf,28,0,120,ch);
						if(pMsg[0]==7) PTZCtrl_continuous((char *)http_sendbuf,120,120,-28,ch);
						if(pMsg[0]==8) PTZCtrl_continuous((char *)http_sendbuf,120,120,28,ch);
						hk_step++;
						http_sent_flag=1;
						preset_now[0]=0; //预置位清零
					}
					if(hk_step==1)
					{
						if(recv_200)
						{
							OSTimeDly ( 300, OS_OPT_TIME_DLY, & err ); 
							PTZCtrl_continuous((char *)http_sendbuf,0,0,0,ch);	//停止转动
							hk_step++;
							http_sent_flag=1;
							recv_200=0;
						}
					}
					if(hk_step==2)
					{
						if(recv_200)
						{
							nw_sent_88(pMsg,ch); //原命令返回
							hk_tast_step=0;
							recv_200=0;
						}
					}					
				break;
				
				case 2://摄像机调节到指定预置点
				case 9://保存当前位置为某预置点		
				if(hk_step==0)
					{
						if(pMsg[0]==2) PTZCtrl_presets_goto((char *)http_sendbuf,pMsg[1],ch); //转到预置位
						if(pMsg[0]==9) PTZCtrl_presets((char *)http_sendbuf,pMsg[1],ch);	//保存当前位置为某预置点		
						hk_step++;
						http_sent_flag=1;
						preset_now[0]=0;	//更改当前预置位
					}
					if(hk_step==1)
					{
						if(recv_200)
						{
							preset_now[0]=pMsg[1];	//更改当前预置位
							nw_sent_88(pMsg,ch); //原命令返回
							hk_tast_step=0;
							recv_200=0;
						}
					}
				break;
				
				case 0x81: 	//更改图像参数  //配置图片参数原命令返回 @@
					if(hk_step==0)
					{
						PTZ_Streaming((char *)http_sendbuf,pMsg[1],ch); //更改图像大小参数
						hk_step++;
						http_sent_flag=1;
					}
					if(hk_step==1)
					{
						if(recv_200)
						{
							PTZ_Image((char *)http_sendbuf,pMsg[2],pMsg[3],pMsg[4],ch); //更改图像亮度,对比度,饱和度
							hk_step++;
							http_sent_flag=1;
							recv_200=0;
						}
					}
					if(hk_step==2)
					{
						if(recv_200) 
						{
							senttask_Asim |= data_flag81_1;	 //原命令返回，需要通道1、2同时成功
							hk_step++;
							recv_200=0;
						}
					}
//				case  0x8a:	//查询图像大小
					if(hk_step==3)
					{
						sent_cmd=0x8a;
						printf("\r\n查询图像大小");
						GET_Streaming((char *)http_sendbuf,ch);//查询图像大小
						hk_step++;
						http_sent_flag=1;
					}
					if(hk_step==4)
					{
						if(recv_200) 
						{
//							printf("\r\n查询图像大小OK");
							hk_step++;
							recv_200=0;
						}
					}
//				case  0x8b:	//查询亮度,对比度,饱和度
					if(hk_step==5)
					{
						sent_cmd=0x8b;
						printf("\r\n查询图像亮度");
						GET_Image((char *)http_sendbuf,ch);
						hk_step++;
						http_sent_flag=1;
					}
					if(hk_step==6)
					{
						if(recv_200) 
						{
							hk_tast_step=0;
							recv_200=0;
						}
					}
				break;
								
				case 0x83://拍照
					if(hk_step==0)
					{
						if((pMsg[1] != preset_now[0])&&(pMsg[1] != 0))
						{
							sent_cmd=0x02;
							PTZCtrl_presets_goto((char *)http_sendbuf,pMsg[1],ch); //转到预置位
							hk_step++;
							http_sent_flag=1;
//							printf("\r\n转到预置位%d",pMsg[1]);
						}
						else
						{
//							printf("\r\n预置位正确");
							hk_step=2;
						}
					}
					if(hk_step==1) //等待预置位设好
					{
						if(recv_200) 
						{
							hk_step++;
							recv_200=0;
							preset_now[0]=pMsg[1];
							printf("\r\nin preset %d",preset_now[0]);
						}
					}
						
					if(hk_step==2)
					{
						if((PH_sram_buf==bempty)||(PH_sram_buf==bpre_load_hk)||(PH_sram_buf==bloading_hk))	//拍照缓存为空
						{
							PH_sram_buf=bpre_load_hk;//PH_flag[0]=0x10; //正在拍照
							sent_cmd=0x83;
							GET_photo((char *)http_sendbuf,ch);
							hk_step++;
							http_sent_flag=1;
							recv_htty_end=0;
							recv_htty_err=0;
							recv_200=0;
							printf("\r\ngoing to take a photo");
						}						
						else if((PH_sram_buf==bready)||(PH_sram_buf==bpre_bready))	//前次拍完,主流程还未处理
						{
							if(sd_sta_h)
							{
								PH_sram_buf=bpre_load_hk;//PH_flag[0]=0x10; //正在拍照
								
								if(index_ph_inc(PH_GET_T)==0)
								{
									sent_cmd=0x83;
									GET_photo((char *)http_sendbuf,ch);
									hk_step++;
									http_sent_flag=1;
									recv_htty_end=0;
									recv_htty_err=0;
									recv_200=0;
									printf("\r\nsave ph in SD,going to take a photo");
								}
								else
								{
									PH_sram_buf=bready;//PH_flag[0]=1; //恢复有照片待发状态
									printf("\r\nsave to SD fail");
									hk_tast_step=0; //退出,暂不处理
									OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err ); 
								}
							}
							else
							{
								PH_sram_buf=bready;//PH_flag[0]=1; //恢复有照片待发状态
								printf("\r\nsave to SD fail");
								hk_tast_step=0; //退出,暂不处理
								OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err ); 
							}
						}
						else if((PH_sram_buf==bpre_sent_buf)||(PH_sram_buf==bsenting_buf))//  (PH_flag[0]==2)	//正在上传缓存照片
						{
							printf("\r\n正在上传照片,拍照等待");
							hk_wait_time=0;
							OSTimeDly( 2000, OS_OPT_TIME_DLY, & err);
						}
//						else if((PH_sram_buf==bpre_sent_sd)||(PH_sram_buf==bsenting_sd))//(PH_flag[0]==0x84)	//正在上传存储照片
//						{
//							printf("\r\n正在上传存储照片,拍照等待");
//							OSTimeDly( 2000, OS_OPT_TIME_DLY, & err);
//						}
						else
						{
							PH_sram_buf=bempty;
							hk_tast_step=0;
							printf("\r\nbuf数据异常");
						}
					}
					if(hk_step==3)
					{
						if(recv_htty_end)	
						{
							PH_sram_buf=bpre_bready;//PH_flag[0]=1; //拍完照,缓存有数据
							printf("\r\ngain a photo");
							hk_tast_step=0;
							recv_htty_end=0;
							recv_200=0;
						}
						if(recv_htty_err)	//数据错误
						{
							hk_tast_step=0;
						}
					}
					else OSTimeDly( 100, OS_OPT_TIME_DLY, & err);
					
				break;
				
				case  0x8a:	//查询图像大小
					if(hk_step==0)
					{
						GET_Streaming((char *)http_sendbuf,ch);//查询图像大小
						hk_step++;
						http_sent_flag=1;
					}
					if(hk_step==1)
					{
						if(recv_200) 
						{
							hk_tast_step=0;
							recv_200=0;
						}
					}
					break;
			
				case  0x8b:	//查询亮度,对比度,饱和度
					if(hk_step==0)
					{
						GET_Image((char *)http_sendbuf,ch);
						hk_step++;
						http_sent_flag=1;
					}
					if(hk_step==1)
					{
						if(recv_200) 
						{
							hk_tast_step=0;
							recv_200=0;
						}
					}
					break;
					
					default:hk_tast_step=0; break;
				}
			
		}
	}//while (DEF_TRUE)
}

//		if(hk_tast_step==2)
//		{
//			OSTimeDly( 100, OS_OPT_TIME_DLY, & err);
//			if(recv_401)
//			{
//				printf("\r\n任务收到401");
////				load_http_sendbuf((char *)http_sendbuf,sent_cmd,&pMsg[1],ch);
//				recv_401=0;
//				http_sent_flag=1;
//				hk_last_stime=worktime;
//			}
//			else if(recv_200)
//			{
//				printf("\r\n任务收到200");
//				if(pMsg[0]<=62)
//				{
//					nw_sent_88(pMsg,ch); //原命令返回
//					hk_tast_step=0;
//				}
//				else if(pMsg[0]==0x81) 
//				{
//					if(sent_cmd==0x81)
//					{
//						sent_cmd=0x82;
//						sent_times=0;
//					}
//					else if(sent_cmd==0x82)
//					{
//						hk_tast_step=0;
//						senttask_Asim |= data_flag81_1;	 //原命令返回，需要通道1、2同时成功
//					}
//				}
//				else if(pMsg[0]==0x83) 
//				{
//					if(sent_cmd==2)
//					{
//						sent_cmd=0x83;
//						sent_times=0;
//					}
//					else if(sent_cmd==0x83)
//					{
//						hk_tast_step=0;
//						senttask_Asim |= data_flag84_1;	//照片OK，上传或者保存由主任务完成
//					}
//				}
//				
//				recv_200=0;
//				hk_tast_step=0;
//			}
//			else if((worktime-hk_last_stime)>=4)
//			{
//				printf("\r\n任务重新发送");
//				http_sent_flag=1;
//				hk_last_stime=worktime;
//			}
//		}
//			
//		if(hk_tast_step==1)
//		{
//			wt_start=worktime;
//			if(pMsg[0]==0x83) 
//			{
//				if((preset_now[0]!=pMsg[1])&&(pMsg[1] != 0))
//				{
//					sent_cmd=2;//摄像机调节到指定预置点
//				}
//			}
//			printf("\r\nHK_Task_ch1准备sent数据");
////			load_http_sendbuf((char *)http_sendbuf,sent_cmd,&pMsg[1],ch);
//			
//			if(http_sendbuf[0])
//			{
//				if(power_flag_cam1==0)
//				{
//					camera_on(1);
//					printf("\r\nHK_Task_ch1球机上电,等待30s");
//					OSTimeDly( 30000, OS_OPT_TIME_DLY, & err);
//				}
//				
//				printf("\r\nhttp_sent_flag置位,准备发");
//				http_sent_flag=1;
//				
//				recv_200=0;		
//				recv_401=0;	
//				hk_tast_step=2;
//				
//			}
//		}		
//	}
//}
//		if(hk_tast_step==2)
//		{
//			in=OSQPend((OS_Q*			)&http_recv[ch-1],   
//											(OS_TICK		)3000,					
//											(OS_OPT			)OS_OPT_PEND_BLOCKING,
//											(OS_MSG_SIZE*	)&in_size,		
//											(CPU_TS*		)0,
//											(OS_ERR*		)&err);
//				
//				if(err==OS_ERR_NONE)	//收到数据，数据处理
//				{
//					printf("收到\r\n%s\r\n",in); //收到数据http
////					printf("\r\nHK_Task_ch1收到数据");
//					pending_http_data(in,in_size,sent_cmd,ch);
//					if(recv_200==1)
//					{
//						if(pMsg[0]<=62)
//						{
//							nw_sent_88(pMsg,ch); //原命令返回
//							hk_tast_step=0;
//						}
//						else if(pMsg[0]==0x81) 
//						{
//							if(sent_cmd==0x81)
//							{
//								sent_cmd=0x82;
//								sent_times=0;
//							}
//							else if(sent_cmd==0x82)
//							{
//								hk_tast_step=0;
//								senttask_Asim |= data_flag81_1;	 //原命令返回，需要通道1、2同时成功
//							}
//						}
//						else if(pMsg[0]==0x83) 
//						{
//							if(sent_cmd==2)
//							{
//								sent_cmd=0x83;
//								sent_times=0;
//							}
//							else if(sent_cmd==0x83)
//							{
//								hk_tast_step=0;
//								senttask_Asim |= data_flag84_1;	//照片OK，上传或者保存由主任务完成
//							}
//						}
//					}
//				}
//				else
//				{
//					printf("\r\nHK_Task_ch1超时");
//					if(sent_times>6) //连续失败6次
//					{
//						printf("\r\n海康球机操作失败");
//						hk_tast_step=0;
//					}
//				}
//		}
#define  http_hk_thread_SIZE                     600
#define  http_hk_thread_PRIO                         6
OS_TCB   http_hk_thread_TCB;															//任务控制块
CPU_STK  http_hk_thread_Stk [ http_hk_thread_SIZE ];	//任务堆栈

#define  HK_Task_ch1_SIZE                     800
#define  HK_Task_ch1_PRIO                         6
OS_TCB   HK_Task_ch1_TCB;															//任务控制块
CPU_STK  HK_Task_ch1_Stk [ HK_Task_ch1_SIZE ];	//任务堆栈

void http_hk_thread(void *arg);	

//OS_SEM http_rev_sem,http_200_sem,http_401_sem,http_data_sem;

void Create_hk_thread(void)
{
	OS_ERR      err;
	int i;
	for(i=0;i<http_hk_thread_SIZE;i++)
	{
		http_hk_thread_Stk [ i ]=0;
	}
	OSTaskCreate((OS_TCB     *)&http_hk_thread_TCB,                             //任务控制块地址
                 (CPU_CHAR   *)"http_hk_thread",                             //任务名称
                 (OS_TASK_PTR ) http_hk_thread,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     )	http_hk_thread_PRIO,                         //任务的优先级
                 (CPU_STK    *)&http_hk_thread_Stk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) http_hk_thread_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) http_hk_thread_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型
}
void HKTaskStart (void *p_arg)
{
	OS_ERR      err;
	
//		多值信号量控制块指针		//多值信号量名称 //资源数目或事件是否发生标志 //返回错误类型
//	OSSemCreate (&http_rev_sem, "http_rev_sem",  0, &err); //3 	

////		多值信号量控制块指针		//多值信号量名称 //资源数目或事件是否发生标志 //返回错误类型
//	OSSemCreate (&http_200_sem, "http_200_sem",  0, &err); //3 	
//	
////		多值信号量控制块指针		//多值信号量名称 //资源数目或事件是否发生标志 //返回错误类型
//	OSSemCreate (&http_data_sem, "http_data_sem",  0, &err); //3 	

//消息队列	 //消息队列名称 //消息队列名称  //消息队列长度 //错误码
	OSQCreate (&hktask_Asim[0],"hktask_Asim[0]",256,&err);	

//消息队列	 //消息队列名称 //消息队列名称  //消息队列长度 //错误码
	OSQCreate (&http_recv[0],"http_recv[0]",1,&err);	

	
	/* 创建 任务 */
    OSTaskCreate((OS_TCB     *)&http_hk_thread_TCB,                             //任务控制块地址
                 (CPU_CHAR   *)"http_hk_thread",                             //任务名称
                 (OS_TASK_PTR ) http_hk_thread,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     )	http_hk_thread_PRIO,                         //任务的优先级
                 (CPU_STK    *)&http_hk_thread_Stk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) http_hk_thread_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) http_hk_thread_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型

								 
			/* 创建 任务 */
    OSTaskCreate((OS_TCB     *)&HK_Task_ch1_TCB,                             //任务控制块地址
                 (CPU_CHAR   *)"HK_Task_ch1",                             //任务名称
                 (OS_TASK_PTR ) HK_Task_ch1,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     )	HK_Task_ch1_PRIO,                         //任务的优先级
                 (CPU_STK    *)&HK_Task_ch1_Stk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) HK_Task_ch1_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) HK_Task_ch1_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型						 

}







