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
//��00H����������ƼĴ�����Network Control Register ��
//7��EXT_PHY��1ѡ���ⲿPHY��0ѡ���ڲ�PHY�����������λӰ�졣
//6��WAKEEN���¼�����ʹ�ܣ�1ʹ�ܣ�0��ֹ������¼�����״̬�����������λӰ�졣
//5��������
//4��FCOL��1ǿ�Ƴ�ͻģʽ�������û����ԡ�
//3��FDX��ȫ˫��ģʽ���ڲ�PHYģʽ��ֻ�����ⲿPHY�¿ɶ�д��
//2-1��LBK���ػ�ģʽ��Loopback��00ͨ����01MAC�ڲ��ػ���10�ڲ�PHY 100Mģʽ���ֻػ���11������
//0��RST��1�����λ��10us���Զ����㡣

#define NSR 0x01
//��01H��������״̬�Ĵ�����Network Status Register ��
//7��SPEED��ý���ٶȣ����ڲ�PHYģʽ�£�0Ϊ100Mbps��1Ϊ10Mbps����LINKST=0ʱ����λ���á�
//6��LINKST������״̬�����ڲ�PHYģʽ�£�0Ϊ����ʧ�ܣ�1Ϊ�����ӡ�
//5��WAKEST�������¼�״̬����ȡ��д1�������λ�����������λӰ�졣
//4��������
//3��TX2END��TX�����ͣ����ݰ�2��ɱ�־����ȡ��д1�������λ�����ݰ�ָ��2������ɡ�
//2��TX2END��TX�����ͣ����ݰ�1��ɱ�־����ȡ��д1�������λ�����ݰ�ָ��1������ɡ�
//1��RXOV��RX�����գ�FIFO���Ƚ��ȳ����棩�����־��
//0��������
#define TCR 0x02
//��02H�������Ϳ��ƼĴ�����TX Control Register��
//7��������
//6��TJDIS��Jabber����ʹ�ܡ�1ʹ��Jabber���䶨ʱ����2048�ֽڣ���0��ֹ��
//ע�ͣ�Jabber��һ����CRC����ĳ�֡������1518byte��С��6000byte���������ݰ��������ԭ�������ܵ������綪��������������վ��Ӳ�����������
//5��EXCECM�������ͻģʽ���ơ�0������ĳ�ͻ��������15����ֹ�������ݰ���1ʼ�ճ��Է����ͱ������ݰ���
//4��PAD_DIS2����ֹΪ���ݰ�ָ��2���PAD��
//3��CRC_DIS2����ֹΪ���ݰ�ָ��2���CRCУ�顣
//2��PAD_DIS2����ֹΪ���ݰ�ָ��1���PAD��
//1��CRC_DIS2����ֹΪ���ݰ�ָ��1���CRCУ�顣
//0��TXREQ��TX�����ͣ����󡣷�����ɺ��Զ������λ��
#define TSR_I 0x03
//��03H�������ݰ�ָ��1�ķ���״̬�Ĵ���1��TX Status Register I��
//7��TJTO��Jabber���䳬ʱ����λ��λ��ʾ���ڶ���2048�ֽ����ݱ��������������֡���ص���
//6��LC���ز��źŶ�ʧ����λ��λ��ʾ��֡����ʱ�������ز��źŶ�ʧ�����ڲ��ػ�ģʽ�¸�λ��Ч��
//5��NC�����ز��źš���λ��λ��ʾ��֡����ʱ���ز��źš����ڲ��ػ�ģʽ�¸�λ��Ч��
//4��LC����ͻ�ӳ١���λ��λ��ʾ��64�ֽڵĳ�ͻ���ں��ַ�����ͻ��
//3��COL�����ݰ���ͻ����λ��λ��ʾ��������з�����ͻ��
//2��EC�������ͻ����λ��λ��ʾ���ڷ����˵�16�γ�ͻ���������ͻ���󣬴��ͱ���ֹ��
//1-0��������
#define TSR_II 0x04
//��04H�������ݰ�ָ��2�ķ���״̬�Ĵ���2��TX Status Register II��
//ͬTSR_I
//�ԡ�
#define RCR 0x05
//��05H�������տ��ƼĴ�����RX Control Register ��
//7��������
//6��WTDIS�����Ź���ʱ����ֹ��1��ֹ��0ʹ�ܡ�
//5��DIS_LONG�����������ݰ���1Ϊ�������ݰ����ȳ���1522�ֽڵ����ݰ���
//4��DIS_CRC������CRCУ���������ݰ���
//3��ALL���������ж�㴫�͡�
//2��RUNT�����Բ����������ݰ���
//1��PRMSC������ģʽ��Promiscuous Mode��
//0��RXEN������ʹ�ܡ�
#define RSR 0x06
//��06H��������״̬�Ĵ�����RX Status Register ��
//7��RF������������֡����λ��λ��ʾ���յ�С��64�ֽڵ�֡��
//6��MF����㴫��֡����λ��λ��ʾ���յ�֡������㴫�͵�ַ��
//5��LCS����ͻ�ӳ١���λ��λ��ʾ��֡���չ����з�����ͻ�ӳ١�
//4��RWTO�����տ��Ź���ʱ�������λ��λ��ʾ���յ�����2048�ֽ�����֡��
//3��PLE���������󡣸�λ��λ��ʾ��֡���չ����з�����������
//2��AE���������Alignment������λ��λ��ʾ���յ���֡��β�������ֽڶ��룬���������ֽ�Ϊ�߽���롣
//1��CE��CRCУ����󡣸�λ��λ��ʾ���յ���֡CRCУ�����
//0��FOE������FIFO�����������λ��λ��ʾ��֡����ʱ����FIFO�����
#define ROCR 0x07
//��07H����������������Ĵ�����Receive Overflow Counter Register��
//7��RXFU����������������������λ��λ��ʾROC��������������������������
//6-0��ROC������������������ü�����Ϊ��̬��������ָʾFIFO����󣬵�ǰ����������ĸ�����
#define BPTR 0x08
//��08H������ѹ���޼Ĵ�����Back Pressure Threshold Register��
//7-4��BPHW����ѹ�������ֵ��������SRAM���пռ���ڸ�����ֵ����MAC������һ��ӵ��״̬��1=1K�ֽڡ�Ĭ��ֵΪ3H����3K�ֽڿ��пռ䡣��Ҫ����SRAM��С��
//3-0��JPT��ӵ��״̬ʱ�䡣Ĭ��Ϊ200us��0000 Ϊ5us��0001Ϊ10us��0010Ϊ15us��0011Ϊ25us��0100Ϊ50us��0101Ϊ100us��0110Ϊ150us��0111Ϊ 200us��1000Ϊ250us��1001Ϊ300us��1010Ϊ350us��1011Ϊ400us��1100Ϊ450us��1101Ϊ500us�� 1110Ϊ550us��1111Ϊ600us��
#define FCTR 0x09
//��09H��������������޼Ĵ�����Flow Control Threshold Register��
//7-4��HWOT������FIFO��������������ֵ��������SRAM���пռ�С�ڸ�����ֵ������һ����ͣʱ�䣨pause_time��ΪFFFFH����ͣ��������ֵΪ0�����޽��տ��пռ䡣1=1K�ֽڡ�Ĭ��ֵΪ3H����3K�ֽڿ��пռ䡣��Ҫ����SRAM��С��
//3-0��LWOT������FIFO��������������ֵ��������SRAM���пռ���ڸ�����ֵ������һ����ͣʱ�䣨pause_time��Ϊ0000H����ͣ����������������ֵ����ͣ������֮������������ֵ����ͣ������Ч��Ĭ��ֵΪ8K�ֽڡ���Ҫ����SRAM��С��
#define RTFCR 0x0a
//��0AH��������/����������ƼĴ�����RX/TX Flow Control Register��
//7��TXP0��1������ͣ����������ɺ��Զ����㣬������TX��ͣ��ʱ��Ϊ0000H��
//6��TXPF��1������ͣ����������ɺ��Զ����㣬������TX��ͣ��ʱ��ΪFFFFH��
//5��TXPEN��ǿ�Ʒ�����ͣ��ʹ�ܡ�������������ֵʹ�ܷ�����ͣ����
//4��BKPA����ѹģʽ����ģʽ���ڰ�˫��ģʽ����Ч��������SRAM����BPHW���ҽ��������ݰ�ʱ������һ��ӵ��״̬��
//3��BKPM����ѹģʽ����ģʽ���ڰ�˫��ģʽ����Ч��������SRAM����BPHW�����ݰ�DAƥ��ʱ������һ��ӵ��״̬��
//2��RXPS��������ͣ��״̬��ֻ����������
//1��RXPCS��������ͣ����ǰ״̬��
//0��FLCE���������ʹ�ܡ�1����ʹ���������ģʽ��
#define EPCR_PHY_CR 0x0b
//��0BH����EEPROM��PHY���ƼĴ�����EEPROM & PHY Control Register��
//7-6��������
//5��REEP�����¼���EEPROM������������Ҫ�ڸò�����ɺ������λ��
//4��WEP��EEPROMдʹ�ܡ�
//3��EPOS��EEPROM��PHY����ѡ��λ��0ѡ��EEPROM��1ѡ��PHY��
//2��ERPRR��EEPROM������PHY�Ĵ������������������Ҫ�ڸò�����ɺ������λ��
//1��ERPRW��EEPROMд����PHY�Ĵ���д�������������Ҫ�ڸò�����ɺ������λ��
//0��ERRE��EEPROM��PHY�ķ���״̬��1��ʾEEPROM��PHY���ڱ����ʡ�
#define EPAR_PHY_AR 0x0c
//��0CH����EEPROM��PHY��ַ�Ĵ�����EEPROM & PHY Address Register��
//7-6��PHY_ADR��PHY��ַ�ĵ���λ��bit1��bit0������PHY��ַ��bit[4:2]ǿ��Ϊ000�����Ҫѡ���ڲ�PHY����ô��2λǿ��Ϊ01��ʵ��Ӧ����Ҫǿ��Ϊ01��
//5-0��EROA��EEPROM�ֵ�ַ��PHY�Ĵ�����ַ��
#define EPDRL_PHY_DRL 0x0d
//��0DH����EEPROM��PHY���ݼĴ����Ͱ��ֽڣ�EEPROM & PHY Low Byte Data Register��
//7-0��EE_PHY_L
#define EPDRL_PHY_DRH 0x0e
//��0EH����EEPROM��PHY���ݼĴ����߰��ֽڣ�EEPROM & PHY High Byte Data Register��
//7-0��EE_PHY_H
#define WUCR 0x0f
//��0FH�������ѿ��ƼĴ�����Wake Up Control Register��
//7-6��������
//5��LINKEN��1ʹ�ܡ�����״̬�ı䡱�����¼�����λ���������λӰ�졣
//4��SAMPLEEN��1ʹ�ܡ�Sample֡�������¼�����λ���������λӰ�졣
//3��MAGICEN��1ʹ�ܡ�Magic Packet�������¼�����λ���������λӰ�졣
//2��LINKST��1��ʾ���������Ӹı��¼�������״̬�ı��¼�����λ���������λӰ�졣
//1��SAMPLEST��1��ʾ���յ���Sample֡���ͷ����ˡ�Sample֡���¼�����λ���������λӰ�졣
//0��MAGICST��1��ʾ���յ���Magic Packet���ͷ����ˡ�Magic Packet���¼�����λ���������λӰ�졣
#define PAR		0x10
#define PAR1	0x10
#define PAR2	0x11
#define PAR3	0x12
#define PAR4	0x13
#define PAR5	0x14
#define PAR6	0x15
//��10H -- 15H���������ַ��MAC���Ĵ�����Physical Address Register��
//7-0��PAD0 -- PAD5�������ַ�ֽ�0 -- �ֽ�5��10H -- 15H������������6���ֽڵ�MAC��ַ��
#define MAR	0x16
//��16H -- 1DH������㷢�͵�ַ�Ĵ�����Multicast Address Register ��
//7-0��MAB0 -- MAB7����㷢�͵�ַ�ֽ�0 -- �ֽ�7��16H --1DH����
#define GPCR	0x1f
//��1FH����GPIO���ƼĴ�����General Purpose Control Register��
//7-4��������
//3-0��GEP_CNTL��GPIO���ơ�����GPIO�������������1Ϊ�����0Ϊ���롣GPIO0Ĭ��Ϊ�����POWER_DOWN���ܡ�����Ĭ��Ϊ���롣���Ĭ��ֵΪ0001��
#define GPR	0x1f
//��1FH����GPIO�Ĵ�����General Purpose Register��
//7-4��������
//3-1��GEPIO3-1��GPIOΪ���ʱ�����λ���ƶ�ӦGPIO�˿�״̬��GPIOΪ����ʱ�����λ��ӳ��ӦGPIO�˿�״̬���������ڵ�Ƭ����IO�˿ڵĿ��ƣ���
//0��GEPIO0������ͬ�ϡ���λĬ��Ϊ���1��POWER_DEWN�ڲ�PHY����ϣ������PHY��������������Ҫͨ��д��0����PWER_DOWN�ź����㡣��λĬ��ֵ��ͨ��EEPROM��̵õ����ο�EEPROM���������
#define TRPAL
//��22H��������SRAM��ָ���ַ�Ͱ��ֽڣ�TX SRAM Read Pointer Address Low Byte��
//7-0��TRPAL
#define TRPAH	
//��23H��������SRAM��ָ���ַ�߰��ֽڣ�TX SRAM Read Pointer Address High Byte ��
//7-0��TRPAH
#define RWPAL
//��24H��������SRAMָ���ַ�Ͱ��ֽڣ�RX SRAM Write Pointer Address Low Byte��
//7-0��RWPAL
#define RWPAH
//��25H��������SRAMָ���ַ�߰��ֽڣ�RX SRAM Write Pointer Address High Byte��
//7-0:RWPAH
#define VIDL	0x28
#define VIDH	0x29
//��28H -- 29H���������������кţ�Vendor ID��
//7-0��VIDL���Ͱ��ֽڣ�28H����ֻ����Ĭ��46H��
//7-0��VIDH���߰��ֽڣ�29H����ֻ����Ĭ��0AH��
#define PID
//��2AH --2BH������Ʒ���кţ�Product ID��
//7-0��PIDL���Ͱ��ֽڣ�2AH����ֻ����Ĭ��00H��
//7-0��PIDH���߰��ֽڣ�2BH����ֻ����Ĭ��90H��
#define CHIPR
//��2CH����оƬ�޶��汾��CHIP Revision��
//7-0��PIDH��ֻ����Ĭ��00H��
#define TCR2 0x2D
//��2DH����������ƼĴ���2��TX Control Register 2��
//7��LED��LEDģʽ��1����LED����Ϊģʽ1��0����LED����Ϊģʽ0�����EEPROM���趨��
//6��RLCP��1���·����г�ͻ�ӳٵ����ݰ���
//5��DTU��1��ֹ���·��͡�underruned�����ݰ���
//4��ONEPM������ģʽ��1�������ǰ����һ�����ݰ��������ܱ�ִ�У�0�������ǰ���������������ݰ��������ܱ�ִ�С�
//3-0��IFGS��֡�������á�0XXXΪ96bit��1000Ϊ64bit��1001Ϊ72bit��1010Ϊ80bit��1011Ϊ88bit��1100Ϊ96bit��1101Ϊ104bit��1110Ϊ112bit��1111Ϊ120bit��
#define OCR
//��2EH�����������Կ��ƼĴ�����Operation Control Register��
//7-6��SCC�������ڲ�ϵͳʱ�ӡ�00Ϊ50MHz��01Ϊ20MHz��10Ϊ100MHz��11������
//5��������
//4��SOE���ڲ�SRAM���ʹ��ʼ�տ�����
//3��SCS���ڲ�SRAMƬѡʼ�տ�����
//2-0��PHYOP��Ϊ�������ڲ�PHY����ģʽ��
#define SMCR 0x2f
//��2FH��������ģʽ���ƼĴ�����Special Mode Control Register��
//7��SM_EN������ģʽʹ�ܡ�
//6-3��������
//2��FLC��ǿ�Ƴ�ͻ�ӳ١�
//1��FB1��ǿ�����Back-off��ʱ�䡣
//0��FB0��ǿ����̡�Back-off��ʱ�䡣
#define ETXCSR
//��30H��������ǰ��Early�����ơ�״̬�Ĵ�����Early Transmit Control/Status Register��
//7��ETE������ǰʹ�ܡ�
//6��ETS2������ǰ״̬2��
//5��ETS1������ǰ״̬1��
//4-2��������
//1-0��ETT������ǰ���ޡ���д������FIFO������������ֽ����ﵽ�����ޣ���ʼ���䡣00Ϊ12.5%��01Ϊ25%��10Ϊ50%��11Ϊ75%��
#define TCSCR
//��31H��������У��Ϳ��ƼĴ�����Transmit Check Sum Control Register��
//7-3��������
//2��UDPCSE��UDPУ��Ͳ���ʹ�ܡ�
//1��TCPCSE��TCP����Ͳ���ʹ�ܡ�
//0��IPCSE��IPУ��Ͳ���ʹ�ܡ�
#define RCSCSR
//��32H��������У��Ϳ���״̬�Ĵ�����Receive Check Sum Control Status Register ��
//7��UDPS��UDPУ���״̬��1��ʾUDP���ݰ�У��ʧ�ܡ�
//6��TCPS��TCPУ���״̬��1��ʾTCP���ݰ�У��ʧ�ܡ�
//5��IPS��IPУ���״̬��1��ʾIP���ݰ�У��ʧ�ܡ�
//4��UDPP��1��ʾUDP���ݰ���
//3��TCPP��1��ʾTCP���ݰ���
//2��IPP��1��ʾIP���ݰ���
//1��RCSEN�����ռ���ͼ���ʹ�ܡ�1ʹ��У���У�飬��У���״̬λ��bit7-2���洢�����ݰ��ĸ��Եı���ͷ�ĵ�һ���ֽڡ�
//0��DCSE������У��ʹ�������ݰ���1ʹ�ܶ���У��ʹ�������ݰ�����IP/TCP/UDP��У�����������������ݰ���
#define MRCMDX	0xf0
//��F0H�����洢����ַ����Ķ��������Memory Data Pre-Fetch Read Command Without Address Increment Register��
//7-0��MRCMDX���ӽ���SRAM�ж����ݣ���ȡ֮��ָ���ڲ�SRAM�Ķ�ָ�벻�䡣
#define MRCMDX1
//��F1H�����洢������ַ����Ķ��������Memory Data Read Command With Address Increment Register
//ͬ�ϡ�
#define MRCMD	0xf2
//��F2H�����洢������ַ�Զ����ӵĶ��������Memory Data Read Command With Address Increment Register��
//7-0��MRCMD���ӽ���SRAM�ж����ݣ���ȡ֮��ָ���ڲ�SRAM�Ķ�ָ���Զ�����1��2��4�����ݴ������Ĳ���ģʽ������8λ��16λ��32λ����
#define MRR  0xf4
#define MRRL  0xf4
//��F4H�����洢������ַ�Ĵ����Ͱ��ֽڣ�Memory Data Read_ address Register Low Byte��
//7-0��MDRAL
#define MRRH	0xf5
//��F5H�����洢������ַ�Ĵ����߰��ֽ�Memory Data Read_ address Register High Byte
//7-0��MDRAH����IMR��bit7=1����üĴ�������Ϊ0CH��
#define MWCMDX	0xf6
//��F6H�����洢������ַ����Ķ��������Memory Data Write Command Without Address Increment Register��
//7-0��MWCMDX��д���ݵ�����SRAM�У�֮��ָ���ڲ�SRAM��д��ַָ�벻�䡣
#define MWCMD	0xf8
//��F8H�����洢������ַ�Զ����ӵĶ��������Memory Data Write Command With Address Increment Register��
//7-0��MWCMD��д���ݵ�����SRAM�У�֮��ָ���ڲ�SRAM�Ķ�ָ���Զ�����1��2��4�����ݴ������Ĳ���ģʽ������8λ��16λ��32λ����
#define MWRL	0xfa
//��FAH�����洢��д��ַ�Ĵ����Ͱ��ֽڣ�Memory Data Write_ address Register Low Byte��
//7-0��MDRAL
#define MWRH	0xfb
//��FBH�����洢��д��ַ�Ĵ����߰��ֽڣ�Memory Data Write _ address Register High Byte��
//7-0:MDRAH
#define TXPLL	0xfc
//��FCH�����������ݰ����ȼĴ����Ͱ��ֽڣ�TX Packet Length Low Byte Register��
//7-0��TXPLL
#define TXPLH	0xfd
//��FDH�����������ݰ����ȼĴ����߰��ֽڣ�TX Packet Length High Byte Register��
//7-0��TXPLH
#define ISR	0xfe
//��FEH�����ж�״̬�Ĵ�����Interrupt Status Register��
//7-6��IOMODE��������ģʽ��00Ϊ16λģʽ��01Ϊ32λģʽ��10Ϊ8λģʽ��00������
//5��LNKCHG������״̬�ı䡣
//4��UDRUN�����䡰Underrun��
//3��ROOS��������������������
//2��ROS�����������
//1��PTS�����ݰ����䡣
//0��PRS�����ݰ����ա�
//ISR�Ĵ�����״̬д1���
#define IMR	0xff
//��FFH�����ն����μĴ�����Interrupt Mask Register��
//7��PAR��1ʹ��ָ���Զ����ء���SRAM�Ķ���дָ�볬��SRAM�Ĵ�Сʱ��ָ���Զ�������ʼλ�á���Ҫ�����������ø�λ����������REG_F5��MDRAH�����Զ�λ0CH��
//6��������
//5��LNKCHGI��1ʹ������״̬�ı��жϡ�
//4��UDRUNI��1ʹ�ܴ��䡰Underrun���жϡ�
//3��ROOI��1ʹ�ܽ����������������жϡ�
//2��ROI��1ʹ�ܽ�������жϡ�
//1��PTI��1ʹ�����ݰ������նˡ�
//0��PRI��1ʹ�����ݰ������жϡ�
//ע�ͣ���ʾ��DM9000��ʼ����Ҫ�õ��ļĴ�����
//�������ϼĴ����ķ�����ͨ�����������ķ�ʽ����ͨ����IOR��IOW��AEN��CMD�Լ�SD0--SD15��������ŵĲ�����ʵ�֡�����CMD����Ϊ�� ��ƽʱΪд�Ĵ�����ַ��Ϊ�͵�ƽʱΪд���ݵ�ָ����ַ�ļĴ����С���ϸ������ο������ֲ��С���дʱ�򡱲��֡�
//��DM9000��A���У�����һЩPHY�Ĵ�����Ҳ��֮Ϊ�����޹ؽӿ�MII�Ĵ�������Ҫ����ȥ���ʡ���Щ�Ĵ������ֶ���ģ���16λ�������г��������õ�PHY�Ĵ�����
#define BMCR 0x00
//��00H��������ģʽ���ƼĴ�����Basic Mode Control Register)
//15��reset��1PHY�����λ��0������������λ����ʹPHY�Ĵ�����ֵΪĬ��ֵ����λ������ɺ󣬸�λ�Զ����㡣
//14��loopback��1Loop-backʹ�ܣ�0����������
//13��speed selection��1Ϊ100Mbps��0Ϊ10Mbps�������ٶȼ����Ը��ݸ�λѡ��Ҳ���Ը��ݵ�12λ�����Զ�Э��ѡ�񡣵��Զ�Э��ʹ��ʱ������12λΪ1����λ���᷵���Զ�Э�̺���ٶ�ֵ��
//12��auto-negotiation enable��1�Զ�Э��ʹ�ܡ�ʹ�õ�13λ�͵�8λ��ֵ��Ӧ�Զ�Э�̺��״̬��
//11��power down��POWER_DOWNģʽ��1ΪPOWER_DOWN��0Ϊ������������POWER_DOWN״̬�£�PHYӦ����Ӧ����������ת�䵽POWER_DOWN״̬���Ѿ�������POWER_DOWN״̬��ʱ��PHY������MII�ϲ�������źš�
//10��isolate��1����һЩ�����⣬PHY����MII�и��룬0Ϊ��������������λ��λ��PHY������ӦTXD[3:0]��TX_EN��TX_ER���룬������TX_CLK��RX_CLK��RX_DV��RX_ER��RXD[3:0]�� COL��CRS�����Ϊ����̬����PHY�����룬��������Ӧ��������
//9��restart auto-aegotiation��1���³�ʼ���Զ�Э��Э�飬0Ϊ��������������12λ��ֹ�ù��ܣ����λ��Ч����ʼ�����λ�Զ����㡣
//8��duplex mode��1Ϊȫ˫��������0Ϊ��������������12λ����ֹ����0��ʱ��λ����λ������12λ����λ�����λ��Ӧ�Զ�Э�̺��״̬��
//7��collision test��1Ϊ��ͻ����ʹ�ܣ�0Ϊ��������������λ��λ������TX_EN������COL�źű�������
//6-0��������

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
//��04H�����Զ�Э�̹��Ĵ�����Auto-negotiationAdvertisement Register��
//15��NP��0��ʾ����Ч����һҳ��1��ʾ��һҳ��Ч��PHYû����һҳ�����Ը�λʼ��Ϊ0��
//14��ACK��1��ʾ���Ӷ������ݽ�����֤��0��ʾ����֤��PHY���Զ�Э��״̬�����Զ����Ƹ�λ��
//13��RF��1��ʾ�����豸���ڴ���״̬��0Ϊ�޴�����顣
//12-11��������
//10��FCS��1��ʾ������֧���������������0��ʾ��֧�֡�
//9��T4��1��ʾ�����豸֧��100BASE-T4��0��ʾ��֧�֡�PHY��֧��100BASE-T4�����Ը�λ��Զ��0��
//8��TX_FDX��1Ϊ�����豸֧��100BASE-TXȫ˫��ģʽ��0Ϊ��֧�֡�
//7��TX_HDX��1Ϊ�����豸֧��100BASE-TX��0Ϊ��֧�֡�
//6��10_FDX��1Ϊ�����豸֧��100BASE-Tȫ˫��ģʽ��0Ϊ��֧�֡�
//5��10_HDX��1Ϊ�����豸֧��100BASE-T��0Ϊ��֧�֡�
//4-0��selecter��Э��ѡ��λ��00001ΪĬ��ֵ����ʾ�豸֧��IEEE802.3CSMA/CD�������޸ġ�
#define DSCR	0x16
//��16H����DAVICOM��ϸ���üĴ�����DAVICOM Specified Configuration Register��
//15��BP_4B5B��1Ϊ�ƹ�4B5B�����5B4B���빦�ܣ�0Ϊ����4B5B��5B4B���ܡ�
//14��BP_SCR��1Ϊ�ƹ���Ƶ�ͽ��Ź��ܣ�0Ϊ����������
//13��BP_ALIGN��1Ϊ�ƹ�����ʱ�Ľ��š����Ŷ��С����빦�ܺͷ���ʱ�ķ��ű��롢��Ƶ���ܣ�0����������
//12��BP_ADPOK��1Ϊǿ���ź�̽�⹦��ʹ�ܣ�0Ϊ������������λ��Ϊ����ʹ��
//11��������
//10��TX��1��ʾ100BASE-TX������0������
//9-8��������
//7��F_LINK_100��0Ϊ����100Mbps��1Ϊǿ��100Mbps��������״̬��
//6-5��������ǿ��Ϊ0.
//4��RPDCTR-EN��1Ϊʹ���Զ���POWER_DOWN��0Ϊ��ֹ��
//3��SMRST��1Ϊ���³�ʼ��PHY��״̬������ʼ�����λ�Զ����㡣
//2��MFPSC��1��ʾMII֡�������ƿ�����0��ʾ�رա�
//1��SLEEP��˯��ģʽ����λ��λ������PHY����˯��ģʽ��ͨ������λ���㻽��˯��ģʽ���������ý���ԭΪ˯��ģʽ֮ǰ��״̬����״̬�������³�ʼ����
//0��RLOUT����λ��λ��ʹ���յ������ݷ��뷢��ͨ���С�

//����PHY�Ĵ����ķ����ǣ�
//��1���Ĵ�����ַд��EPAR/PHY_AR��0CH���Ĵ����У�ע�⽫�Ĵ�����ַ�ĵ�6λ��1����ַ��0x40�����㼴�ɣ����Ա���д����PHY��ַ��������EEPROM��ַ��
//��2�������ݸ��ֽ�д��PHY_DRH��0EH���Ĵ����С�
//��3�������ݵ��ֽ�д��PHY_DRL��0DH���Ĵ����С�
//��4������PHY����(0x0a����EPCR/PHY_CR��0BH���Ĵ����С�
//��5����ʱ5us����������0x08��EPCR/PHY_CR��0BH���Ĵ����У����PHYд������
//����ΪDM9000��A�����üĴ������ܵ���ϸ���ܣ�ͨ������Щ�Ĵ����Ĳ������ʣ����Ǳ����ʵ�ֶ�DM9000�ĳ�ʼ�������ݷ��͡����յ���ز�����
//��Ҫʵ��ARP��IP��TCP�ȹ��ܣ�����Ҫ�����Э�����⣬�ɱ�д���Э�����ֲЭ��ջ��ʵ�֡�



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
#define IMR_POI             0X04		//ʹ�ܽ�������ж�
#define IMR_PTI             0X02		//ʹ�ܷ����ж�
#define IMR_PRI             0X01		//ʹ�ܽ����ж�

#define ISR_LNKCHGS         (1<<5)
#define ISR_ROOS            (1<<3)
#define ISR_ROS             (1<<2)
#define ISR_PTS             (1<<1)
#define ISR_PRS             (1<<0)
#define ISR_CLR_STATUS      (ISR_ROOS | ISR_ROS | ISR_PTS | ISR_PRS)


//DM9000����ģʽ����
enum DM9000_PHY_mode
{
	DM9000_10MHD 	= 	0, 					//10M��˫��
	DM9000_100MHD 	= 	1,					//100M��˫��	
	DM9000_10MFD 	= 	4, 					//10Mȫ˫��
	DM9000_100MFD 	= 	5,					//100Mȫ˫��
	DM9000_AUTO  	= 	8, 					//�Զ�Э��
};

	
//DM9000���ýṹ��
struct dm9000_config
{
	enum DM9000_PHY_mode mode;				//����ģʽ
	uint8_t  imr_all;							//�ж����� 
	uint16_t queue_packet_len;					//ÿ�����ݰ���С
	uint8_t  mac_addr[6];						//MAC��ַ
	uint8_t  multicase_addr[8];					//�鲥��ַ
};
extern struct dm9000_config dm9000cfg;		//dm9000���ýṹ��


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




