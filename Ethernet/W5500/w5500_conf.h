#ifndef _W5500_CONF_H_
#define _W5500_CONF_H_

#include "utility.h"
#include "gd32f30x.h"
#define __GNUC__

typedef  void (*pFunction)(void);

extern uint16	local_port;/*定义本地端口*/

#define HIGH	           	 			1
#define LOW		             			0

#pragma pack(1)
/*此结构体定义了W5500可供配置的主要参数*/
typedef struct _CONFIG_MSG											
{
  uint8 mac[6];																						 /*MAC地址*/
  uint8 lip[4];																						 /*local IP本地IP地址*/
  uint8 sub[4];																						 /*子网掩码*/
  uint8 gw[4];																						 /*网关*/	
  uint8 dns[4];																						 /*DNS服务器地址*/
  uint8 rip[4];																						 /*remote IP远程IP地址*/
}CONFIG_MSG;
#pragma pack()

extern CONFIG_MSG  	ConfigMsg;

/*MCU配置相关函数*/
void bsp_W5500_init(void);

/*W5500SPI相关函数*/
void IINCHIP_WRITE( uint32 addrbsb,  uint8 data);			/*写入一个8位数据到W5500*/
uint8 IINCHIP_READ(uint32 addrbsb);													/*从W5500读出一个8位数据*/
uint16 wiz_write_buf(uint32 addrbsb,uint8* buf,uint16 len);	/*向W5500写入len字节数据*/
uint16 wiz_read_buf(uint32 addrbsb, uint8* buf,uint16 len);	/*从W5500读出len字节数据*/

/*W5500基本配置相关函数*/
void reset_w5500(void);				/*硬复位W5500*/
void set_w5500_mac(uint8* mac);		/*配置W5500的MAC地址*/
void set_w5500_ip(void);			        /*配置W5500的IP地址*/
void setKPALVTR(SOCKET i,uint8 val);
void timer_counter_process(void);
#endif
