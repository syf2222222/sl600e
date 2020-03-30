#ifndef __DM9000_H
#define __DM9000_H

#include "lwip/pbuf.h"

unsigned char check_rx(void);
void tcp_echoserver_connect(void);

#define RST_H()		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET)
#define RST_L()		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET)

//#define PWREN_ETH_ON()		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET)
//#define PWREN_ETH_OFF()		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET)

#define IRQ_Read()		HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2)

//#define PWREN_CAMERA1_H()		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_9, GPIO_PIN_SET)
//#define PWREN_CAMERA1_L()		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_9, GPIO_PIN_RESET)

//#define PWREN_CAMERA2_H()		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_SET)
//#define PWREN_CAMERA2_L()		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_RESET)

//void             HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
//void              HAL_GPIO_TogglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
//https://baike.baidu.com/item/dm9000

#define NCR 	0x00
//（00H）：网络控制寄存器（Network Control Register ）
//7：EXT_PHY：1选择外部PHY，0选择内部PHY，不受软件复位影响。
//6：WAKEEN：事件唤醒使能，1使能，0禁止并清除事件唤醒状态，不受软件复位影响。
//5：保留。
//4：FCOL：1强制冲突模式，用于用户测试。
//3：FDX：全双工模式。内部PHY模式下只读，外部PHY下可读写。
//2-1：LBK：回环模式（Loopback）00通常，01MAC内部回环，10内部PHY 100M模式数字回环，11保留。
//0：RST：1软件复位，10us后自动清零。

#define NSR 0x01
//（01H）：网络状态寄存器（Network Status Register ）
//7：SPEED：媒介速度，在内部PHY模式下，0为100Mbps，1为10Mbps。当LINKST=0时，此位不用。
//6：LINKST：连接状态，在内部PHY模式下，0为连接失败，1为已连接。
//5：WAKEST：唤醒事件状态。读取或写1将清零该位。不受软件复位影响。
//4：保留。
//3：TX2END：TX（发送）数据包2完成标志，读取或写1将清零该位。数据包指针2传输完成。
//2：TX2END：TX（发送）数据包1完成标志，读取或写1将清零该位。数据包指针1传输完成。
//1：RXOV：RX（接收）FIFO（先进先出缓存）溢出标志。
//0：保留。
#define TCR 0x02
//（02H）：发送控制寄存器（TX Control Register）
//7：保留。
//6：TJDIS：Jabber传输使能。1使能Jabber传输定时器（2048字节），0禁止。
//注释：Jabber是一个有CRC错误的长帧（大于1518byte而小于6000byte）或是数据包重组错误。原因：它可能导致网络丢包。多是由于作站有硬件或软件错误。
//5：EXCECM：额外冲突模式控制。0当额外的冲突计数多于15则终止本次数据包，1始终尝试发发送本次数据包。
//4：PAD_DIS2：禁止为数据包指针2添加PAD。
//3：CRC_DIS2：禁止为数据包指针2添加CRC校验。
//2：PAD_DIS2：禁止为数据包指针1添加PAD。
//1：CRC_DIS2：禁止为数据包指针1添加CRC校验。
//0：TXREQ：TX（发送）请求。发送完成后自动清零该位。
#define TSR_I 0x03
//（03H）：数据包指针1的发送状态寄存器1（TX Status Register I）
//7：TJTO：Jabber传输超时。该位置位表示由于多于2048字节数据被传输而导致数据帧被截掉。
//6：LC：载波信号丢失。该位置位表示在帧传输时发生红载波信号丢失。在内部回环模式下该位无效。
//5：NC：无载波信号。该位置位表示在帧传输时无载波信号。在内部回环模式下该位无效。
//4：LC：冲突延迟。该位置位表示在64字节的冲突窗口后又发生冲突。
//3：COL：数据包冲突。该位置位表示传输过程中发生冲突。
//2：EC：额外冲突。该位置位表示由于发生了第16次冲突（即额外冲突）后，传送被终止。
//1-0：保留。
#define TSR_II 0x04
//（04H）：数据包指针2的发送状态寄存器2（TX Status Register II）
//同TSR_I
//略。
#define RCR 0x05
//（05H）：接收控制寄存器（RX Control Register ）
//7：保留。
//6：WTDIS：看门狗定时器禁止。1禁止，0使能。
//5：DIS_LONG：丢弃长数据包。1为丢弃数据包长度超过1522字节的数据包。
//4：DIS_CRC：丢弃CRC校验错误的数据包。
//3：ALL：忽略所有多点传送。
//2：RUNT：忽略不完整的数据包。
//1：PRMSC：混杂模式（Promiscuous Mode）
//0：RXEN：接收使能。
#define RSR 0x06
//（06H）：接收状态寄存器（RX Status Register ）
//7：RF：不完整数据帧。该位置位表示接收到小于64字节的帧。
//6：MF：多点传送帧。该位置位表示接收到帧包含多点传送地址。
//5：LCS：冲突延迟。该位置位表示在帧接收过程中发生冲突延迟。
//4：RWTO：接收看门狗定时溢出。该位置位表示接收到大于2048字节数据帧。
//3：PLE：物理层错误。该位置位表示在帧接收过程中发生物理层错误。
//2：AE：对齐错误（Alignment）。该位置位表示接收到的帧结尾处不是字节对齐，即不是以字节为边界对齐。
//1：CE：CRC校验错误。该位置位表示接收到的帧CRC校验错误。
//0：FOE：接收FIFO缓存溢出。该位置位表示在帧接收时发生FIFO溢出。
#define ROCR 0x07
//（07H）：接收溢出计数寄存器（Receive Overflow Counter Register）
//7：RXFU：接收溢出计数器溢出。该位置位表示ROC（接收溢出计数器）发生溢出。
//6-0：ROC：接收溢出计数器。该计数器为静态计数器，指示FIFO溢出后，当前接收溢出包的个数。
#define BPTR 0x08
//（08H）：背压门限寄存器（Back Pressure Threshold Register）
//7-4：BPHW：背压门限最高值。当接收SRAM空闲空间低于该门限值，则MAC将产生一个拥挤状态。1=1K字节。默认值为3H，即3K字节空闲空间。不要超过SRAM大小。
//3-0：JPT：拥挤状态时间。默认为200us。0000 为5us，0001为10us，0010为15us，0011为25us，0100为50us，0101为100us，0110为150us，0111为 200us，1000为250us，1001为300us，1010为350us，1011为400us，1100为450us，1101为500us， 1110为550us，1111为600us。
#define FCTR 0x09
//（09H）：溢出控制门限寄存器（Flow Control Threshold Register）
//7-4：HWOT：接收FIFO缓存溢出门限最高值。当接收SRAM空闲空间小于该门限值，则发送一个暂停时间（pause_time）为FFFFH的暂停包。若该值为0，则无接收空闲空间。1=1K字节。默认值为3H，即3K字节空闲空间。不要超过SRAM大小。
//3-0：LWOT：接收FIFO缓存溢出门限最低值。当接收SRAM空闲空间大于该门限值，则发送一个暂停时间（pause_time）为0000H的暂停包。当溢出门限最高值的暂停包发送之后，溢出门限最低值的暂停包才有效。默认值为8K字节。不要超过SRAM大小。
#define RTFCR 0x0a
//（0AH）：接收/发送溢出控制寄存器（RX/TX Flow Control Register）
//7：TXP0：1发送暂停包。发送完成后自动清零，并设置TX暂停包时间为0000H。
//6：TXPF：1发送暂停包。发送完成后自动清零，并设置TX暂停包时间为FFFFH。
//5：TXPEN：强制发送暂停包使能。按溢出门限最高值使能发送暂停包。
//4：BKPA：背压模式。该模式仅在半双工模式下有效。当接收SRAM超过BPHW并且接收新数据包时，产生一个拥挤状态。
//3：BKPM：背压模式。该模式仅在半双工模式下有效。当接收SRAM超过BPHW并数据包DA匹配时，产生一个拥挤状态。
//2：RXPS：接收暂停包状态。只读清零允许。
//1：RXPCS：接收暂停包当前状态。
//0：FLCE：溢出控制使能。1设置使能溢出控制模式。
#define EPCR_PHY_CR 0x0b
//（0BH）：EEPROM和PHY控制寄存器（EEPROM & PHY Control Register）
//7-6：保留。
//5：REEP：重新加载EEPROM。驱动程序需要在该操作完成后清零该位。
//4：WEP：EEPROM写使能。
//3：EPOS：EEPROM或PHY操作选择位。0选择EEPROM，1选择PHY。
//2：ERPRR：EEPROM读，或PHY寄存器读命令。驱动程序需要在该操作完成后清零该位。
//1：ERPRW：EEPROM写，或PHY寄存器写命令。驱动程序需要在该操作完成后清零该位。
//0：ERRE：EEPROM或PHY的访问状态。1表示EEPROM或PHY正在被访问。
#define EPAR_PHY_AR 0x0c
//（0CH）：EEPROM或PHY地址寄存器（EEPROM & PHY Address Register）
//7-6：PHY_ADR：PHY地址的低两位（bit1，bit0），而PHY地址的bit[4:2]强制为000。如果要选择内部PHY，那么此2位强制为01，实际应用中要强制为01。
//5-0：EROA：EEPROM字地址或PHY寄存器地址。
#define EPDRL_PHY_DRL 0x0d
//（0DH）：EEPROM或PHY数据寄存器低半字节（EEPROM & PHY Low Byte Data Register）
//7-0：EE_PHY_L
#define EPDRL_PHY_DRH 0x0e
//（0EH）：EEPROM或PHY数据寄存器高半字节（EEPROM & PHY High Byte Data Register）
//7-0：EE_PHY_H
#define WUCR 0x0f
//（0FH）：唤醒控制寄存器（Wake Up Control Register）
//7-6：保留。
//5：LINKEN：1使能“连接状态改变”唤醒事件。该位不受软件复位影响。
//4：SAMPLEEN：1使能“Sample帧”唤醒事件。该位不受软件复位影响。
//3：MAGICEN：1使能“Magic Packet”唤醒事件。该位不受软件复位影响。
//2：LINKST：1表示发生了连接改变事件和连接状态改变事件。该位不受软件复位影响。
//1：SAMPLEST：1表示接收到“Sample帧”和发生了“Sample帧”事件。该位不受软件复位影响。
//0：MAGICST：1表示接收到“Magic Packet”和发生了“Magic Packet”事件。该位不受软件复位影响。
#define PAR		0x10
#define PAR1	0x10
#define PAR2	0x11
#define PAR3	0x12
#define PAR4	0x13
#define PAR5	0x14
#define PAR6	0x15
//（10H -- 15H）：物理地址（MAC）寄存器（Physical Address Register）
//7-0：PAD0 -- PAD5：物理地址字节0 -- 字节5（10H -- 15H）。用来保存6个字节的MAC地址。
#define MAR	0x16
//（16H -- 1DH）：多点发送地址寄存器（Multicast Address Register ）
//7-0：MAB0 -- MAB7：多点发送地址字节0 -- 字节7（16H --1DH）。
#define GPCR	0x1f
//（1FH）：GPIO控制寄存器（General Purpose Control Register）
//7-4：保留。
//3-0：GEP_CNTL：GPIO控制。定义GPIO的输入输出方向。1为输出，0为输入。GPIO0默认为输出做POWER_DOWN功能。其它默认为输入。因此默认值为0001。
#define GPR	0x1f
//（1FH）：GPIO寄存器（General Purpose Register）
//7-4：保留。
//3-1：GEPIO3-1：GPIO为输出时，相关位控制对应GPIO端口状态，GPIO为输入时，相关位反映对应GPIO端口状态。（类似于单片机对IO端口的控制）。
//0：GEPIO0：功能同上。该位默认为输出1到POWER_DEWN内部PHY。若希望启用PHY，则驱动程序需要通过写“0”将PWER_DOWN信号清零。该位默认值可通过EEPROM编程得到。参考EEPROM相关描述。
#define TRPAL
//（22H）：发送SRAM读指针地址低半字节（TX SRAM Read Pointer Address Low Byte）
//7-0：TRPAL
#define TRPAH	
//（23H）：发送SRAM读指针地址高半字节（TX SRAM Read Pointer Address High Byte ）
//7-0：TRPAH
#define RWPAL
//（24H）：接收SRAM指针地址低半字节（RX SRAM Write Pointer Address Low Byte）
//7-0：RWPAL
#define RWPAH
//（25H）：接收SRAM指针地址高半字节（RX SRAM Write Pointer Address High Byte）
//7-0:RWPAH
#define VIDL	0x28
#define VIDH	0x29
//（28H -- 29H）：生产厂家序列号（Vendor ID）
//7-0：VIDL：低半字节（28H），只读，默认46H。
//7-0：VIDH：高半字节（29H），只读，默认0AH。
#define PID
//（2AH --2BH）：产品序列号（Product ID）
//7-0：PIDL：低半字节（2AH），只读，默认00H。
//7-0：PIDH：高半字节（2BH），只读，默认90H。
#define CHIPR
//（2CH）：芯片修订版本（CHIP Revision）
//7-0：PIDH：只读，默认00H。
#define TCR2 0x2D
//（2DH）：传输控制寄存器2（TX Control Register 2）
//7：LED：LED模式。1设置LED引脚为模式1，0设置LED引脚为模式0或根据EEPROM的设定。
//6：RLCP：1重新发送有冲突延迟的数据包。
//5：DTU：1禁止重新发送“underruned”数据包。
//4：ONEPM：单包模式。1发送完成前发送一个数据包的命令能被执行，0发送完成前发送两个以上数据包的命令能被执行。
//3-0：IFGS：帧间间隔设置。0XXX为96bit，1000为64bit，1001为72bit，1010为80bit，1011为88bit，1100为96bit，1101为104bit，1110为112bit，1111为120bit。
#define OCR
//（2EH）：操作测试控制寄存器（Operation Control Register）
//7-6：SCC：设置内部系统时钟。00为50MHz，01为20MHz，10为100MHz，11保留。
//5：保留。
//4：SOE：内部SRAM输出使能始终开启。
//3：SCS：内部SRAM片选始终开启。
//2-0：PHYOP：为测试用内部PHY操作模式。
#define SMCR 0x2f
//（2FH）：特殊模式控制寄存器（Special Mode Control Register）
//7：SM_EN：特殊模式使能。
//6-3：保留。
//2：FLC：强制冲突延迟。
//1：FB1：强制最长“Back-off”时间。
//0：FB0：强制最短“Back-off”时间。
#define ETXCSR
//（30H）：传输前（Early）控制、状态寄存器（Early Transmit Control/Status Register）
//7：ETE：传输前使能。
//6：ETS2：传输前状态2。
//5：ETS1：传输前状态1。
//4-2：保留。
//1-0：ETT：传输前门限。当写到发送FIFO缓存里的数据字节数达到该门限，则开始传输。00为12.5%，01为25%，10为50%，11为75%。
#define TCSCR
//（31H）：传输校验和控制寄存器（Transmit Check Sum Control Register）
//7-3：保留。
//2：UDPCSE：UDP校验和产生使能。
//1：TCPCSE：TCP检验和产生使能。
//0：IPCSE：IP校验和产生使能。
#define RCSCSR
//（32H）：接收校验和控制状态寄存器（Receive Check Sum Control Status Register ）
//7：UDPS：UDP校验和状态。1表示UDP数据包校验失败。
//6：TCPS：TCP校验和状态。1表示TCP数据包校验失败。
//5：IPS：IP校验和状态。1表示IP数据包校验失败。
//4：UDPP：1表示UDP数据包。
//3：TCPP：1表示TCP数据包。
//2：IPP：1表示IP数据包。
//1：RCSEN：接收检验和检验使能。1使能校验和校验，将校验和状态位（bit7-2）存储到数据包的各自的报文头的第一个字节。
//0：DCSE：丢弃校验和错误的数据包。1使能丢弃校验和错误的数据包，若IP/TCP/UDP的校验和域错误，则丢弃该数据包。
#define MRCMDX	0xf0
//（F0H）：存储器地址不变的读数据命令（Memory Data Pre-Fetch Read Command Without Address Increment Register）
//7-0：MRCMDX：从接收SRAM中读数据，读取之后，指向内部SRAM的读指针不变。
#define MRCMDX1
//（F1H）：存储器读地址不变的读数据命令（Memory Data Read Command With Address Increment Register
//同上。
#define MRCMD	0xf2
//（F2H）：存储器读地址自动增加的读数据命令（Memory Data Read Command With Address Increment Register）
//7-0：MRCMD：从接收SRAM中读数据，读取之后，指向内部SRAM的读指针自动增加1、2或4，根据处理器的操作模式而定（8位、16位或32位）。
#define MRR  0xf4
#define MRRL  0xf4
//（F4H）：存储器读地址寄存器低半字节（Memory Data Read_ address Register Low Byte）
//7-0：MDRAL
#define MRRH	0xf5
//（F5H）：存储器读地址寄存器高半字节Memory Data Read_ address Register High Byte
//7-0：MDRAH：若IMR的bit7=1，则该寄存器设置为0CH。
#define MWCMDX	0xf6
//（F6H）：存储器读地址不变的读数据命令（Memory Data Write Command Without Address Increment Register）
//7-0：MWCMDX：写数据到发送SRAM中，之后指向内部SRAM的写地址指针不变。
#define MWCMD	0xf8
//（F8H）：存储器读地址自动增加的读数据命令（Memory Data Write Command With Address Increment Register）
//7-0：MWCMD：写数据到发送SRAM中，之后指向内部SRAM的读指针自动增加1、2或4，根据处理器的操作模式而定（8位、16位或32位）。
#define MWRL	0xfa
//（FAH）：存储器写地址寄存器低半字节（Memory Data Write_ address Register Low Byte）
//7-0：MDRAL
#define MWRH	0xfb
//（FBH）：存储器写地址寄存器高半字节（Memory Data Write _ address Register High Byte）
//7-0:MDRAH
#define TXPLL	0xfc
//（FCH）：发送数据包长度寄存器低半字节（TX Packet Length Low Byte Register）
//7-0：TXPLL
#define TXPLH	0xfd
//（FDH）：发送数据包长度寄存器高半字节（TX Packet Length High Byte Register）
//7-0：TXPLH
#define ISR	0xfe
//（FEH）：中断状态寄存器（Interrupt Status Register）
//7-6：IOMODE：处理器模式。00为16位模式，01为32位模式，10为8位模式，00保留。
//5：LNKCHG：连接状态改变。
//4：UDRUN：传输“Underrun”
//3：ROOS：接收溢出计数器溢出。
//2：ROS：接收溢出。
//1：PTS：数据包传输。
//0：PRS：数据包接收。
//ISR寄存器各状态写1清除
#define IMR	0xff
//（FFH）：终端屏蔽寄存器（Interrupt Mask Register）
//7：PAR：1使能指针自动跳回。当SRAM的读、写指针超过SRAM的大小时，指针自动跳回起始位置。需要驱动程序设置该位，若设置则REG_F5（MDRAH）将自动位0CH。
//6：保留。
//5：LNKCHGI：1使能连接状态改变中断。
//4：UDRUNI：1使能传输“Underrun”中断。
//3：ROOI：1使能接收溢出计数器溢出中断。
//2：ROI：1使能接收溢出中断。
//1：PTI：1使能数据包传输终端。
//0：PRI：1使能数据包接收中断。
//注释：表示在DM9000初始化中要用到的寄存器。
//访问以上寄存器的方法是通过总线驱动的方式，即通过对IOR、IOW、AEN、CMD以及SD0--SD15等相关引脚的操作来实现。其中CMD引脚为高 电平时为写寄存器地址，为低电平时为写数据到指定地址的寄存器中。详细过程请参考数据手册中“读写时序”部分。
//在DM9000（A）中，还有一些PHY寄存器，也称之为介质无关接口MII寄存器，需要我们去访问。这些寄存器是字对齐的，即16位宽。下面列出三个常用的PHY寄存器。
#define BMCR 0x00
//（00H）：基本模式控制寄存器（Basic Mode Control Register)
//15：reset：1PHY软件复位，0正常操作。复位操作使PHY寄存器的值为默认值。复位操作完成后，该位自动清零。
//14：loopback：1Loop-back使能，0正常操作。
//13：speed selection：1为100Mbps，0为10Mbps。连接速度即可以根据该位选择，也可以根据第12位，即自动协商选择。当自动协商使能时，即第12位为1，该位将会返回自动协商后的速度值。
//12：auto-negotiation enable：1自动协商使能。使得第13位和第8位的值反应自动协商后的状态。
//11：power down：POWER_DOWN模式。1为POWER_DOWN，0为正常操作。在POWER_DOWN状态下，PHY应当响应操作处理。在转变到POWER_DOWN状态或已经运行在POWER_DOWN状态下时，PHY不会在MII上产生虚假信号。
//10：isolate：1除了一些操作外，PHY将从MII中隔离，0为正常操作。当该位置位，PHY不会响应TXD[3:0]，TX_EN和TX_ER输入，并且在TX_CLK，RX_CLK，RX_DV，RX_ER，RXD[3:0]， COL和CRS输出上为高阻态。当PHY被隔离，则它将响应操作处理。
//9：restart auto-aegotiation：1重新初始化自动协商协议，0为正常操作。当第12位禁止该功能，则该位无效。初始化后该位自动清零。
//8：duplex mode：1为全双工操作，0为正常操作。当第12位被禁止（置0）时该位被置位，若第12位被置位，则该位反应自动协商后的状态。
//7：collision test：1为冲突测试使能，0为正常操作。若该位置位，声明TX_EN将引起COL信号被声明。
//6-0：保留。

#define BMSR 0x01

#define PHYID1 0x02
#define PHYID2 0x03

#define ANLPAR 	0x05
#define ANER		0x06

#define DSCSR						0x17
#define BTCSR10					0x18
#define PWDOR						0x19
#define specified_conf	0x20



#define ANAR	0x04
//（04H）：自动协商广告寄存器（Auto-negotiationAdvertisement Register）
//15：NP：0表示无有效的下一页，1表示下一页有效。PHY没有下一页，所以该位始终为0。
//14：ACK：1表示连接对象数据接收认证，0表示无认证。PHY的自动协商状态机会自动控制该位。
//13：RF：1表示本地设备处于错误状态，0为无错误检验。
//12-11：保留。
//10：FCS：1表示处理器支持溢出控制能力，0表示不支持。
//9：T4：1表示本地设备支持100BASE-T4，0表示不支持。PHY不支持100BASE-T4，所以该位永远是0。
//8：TX_FDX：1为本地设备支持100BASE-TX全双工模式，0为不支持。
//7：TX_HDX：1为本地设备支持100BASE-TX，0为不支持。
//6：10_FDX：1为本地设备支持100BASE-T全双工模式，0为不支持。
//5：10_HDX：1为本地设备支持100BASE-T，0为不支持。
//4-0：selecter：协议选择位，00001为默认值，表示设备支持IEEE802.3CSMA/CD，不用修改。
#define DSCR	0x16
//（16H）：DAVICOM详细配置寄存器（DAVICOM Specified Configuration Register）
//15：BP_4B5B：1为绕过4B5B编码和5B4B解码功能，0为正草4B5B和5B4B功能。
//14：BP_SCR：1为绕过扰频和解扰功能，0为正常操作。
//13：BP_ALIGN：1为绕过接收时的解扰、符号队列、解码功能和发送时的符号编码、扰频功能，0正常操作。
//12：BP_ADPOK：1为强制信号探测功能使能，0为正常操作。该位仅为调试使用
//11：保留。
//10：TX：1表示100BASE-TX操作，0保留。
//9-8：保留。
//7：F_LINK_100：0为正常100Mbps，1为强制100Mbps良好连接状态。
//6-5：保留，强制为0.
//4：RPDCTR-EN：1为使能自动简化POWER_DOWN，0为禁止。
//3：SMRST：1为重新初始化PHY的状态机，初始化后该位自动清零。
//2：MFPSC：1表示MII帧引导抑制开启，0表示关闭。
//1：SLEEP：睡眠模式。该位置位将导致PHY进入睡眠模式，通过将该位清零唤醒睡眠模式，其中配置将还原为睡眠模式之前的状态，但状态机将重新初始化。
//0：RLOUT：该位置位将使接收到的数据放入发送通道中。

//访问PHY寄存器的方法是：
//（1）寄存器地址写到EPAR/PHY_AR（0CH）寄存器中，注意将寄存器地址的第6位置1（地址与0x40或运算即可），以表明写的是PHY地址，而不是EEPROM地址。
//（2）将数据高字节写到PHY_DRH（0EH）寄存器中。
//（3）将数据低字节写到PHY_DRL（0DH）寄存器中。
//（4）发送PHY命令(0x0a）到EPCR/PHY_CR（0BH）寄存器中。
//（5）延时5us，发送命令0x08到EPCR/PHY_CR（0BH）寄存器中，清除PHY写操作。
//以上为DM9000（A）常用寄存器功能的详细介绍，通过对这些寄存器的操作访问，我们便可以实现对DM9000的初始化、数据发送、接收等相关操作。
//而要实现ARP、IP、TCP等功能，则需要对相关协议的理解，由编写相关协议或移植协议栈来实现。



#define NCR_RST             0X01
#define NSR_SPEED           0X80
#define NSR_LINKST         	0X40
#define NSR_WAKEST          0X20
#define NSR_TX2END          0X08
#define NSR_TX1END          0X04
#define NSR_RXOV            0X02

#define RCR_DIS_LONG        0X20
#define RCR_DIS_CRC         0X10
#define RCR_ALL             0X08
#define RCR_RXEN            0X01

#define IMR_PAR             0X80	
#define IMR_ROOI            0X08	
#define IMR_POI             0X04		//使能接收溢出中断
#define IMR_PTI             0X02		//使能发送中断
#define IMR_PRI             0X01		//使能接收中断

#define ISR_LNKCHGS         (1<<5)
#define ISR_ROOS            (1<<3)
#define ISR_ROS             (1<<2)
#define ISR_PTS             (1<<1)
#define ISR_PRS             (1<<0)
#define ISR_CLR_STATUS      (ISR_ROOS | ISR_ROS | ISR_PTS | ISR_PRS)


//DM9000工作模式定义
enum DM9000_PHY_mode
{
	DM9000_10MHD 	= 	0, 					//10M半双工
	DM9000_100MHD 	= 	1,					//100M半双工	
	DM9000_10MFD 	= 	4, 					//10M全双工
	DM9000_100MFD 	= 	5,					//100M全双工
	DM9000_AUTO  	= 	8, 					//自动协商
};

	
//DM9000配置结构体
struct dm9000_config
{
	enum DM9000_PHY_mode mode;				//工作模式
	uint8_t  imr_all;							//中断类型 
	uint16_t queue_packet_len;					//每个数据包大小
	uint8_t  mac_addr[6];						//MAC地址
	uint8_t  multicase_addr[8];					//组播地址
};
extern struct dm9000_config dm9000cfg;		//dm9000配置结构体


uint8_t   DM9000_Init(void);
uint16_t  DM9000_ReadReg(uint16_t reg);
void DM9000_WriteReg(uint16_t reg,uint16_t data);
uint16_t  DM9000_PHY_ReadReg(uint16_t reg);
void DM9000_PHY_WriteReg(uint16_t reg,uint16_t data);
uint32_t  DM9000_Get_DeiviceID(void);
uint8_t   DM9000_Get_SpeedAndDuplex(void);	
void DM9000_Set_PHYMode(uint8_t mode);
void DM9000_Set_MACAddress(uint8_t *macaddr);
void DM9000_Set_Multicast(uint8_t *multicastaddr);
void DM9000_Reset(void);
void DM9000_SendPacket(struct pbuf *p);
struct pbuf *DM9000_Receive_Packet(void);
void DMA9000_ISRHandler(void);
#endif /* __DM9000_H  */




