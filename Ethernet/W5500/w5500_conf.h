#ifndef _W5500_CONF_H_
#define _W5500_CONF_H_

#include "utility.h"
#include "gd32f30x.h"
#define __GNUC__

typedef  void (*pFunction)(void);

extern uint16	local_port;/*���屾�ض˿�*/

#define HIGH	           	 			1
#define LOW		             			0

#pragma pack(1)
/*�˽ṹ�嶨����W5500�ɹ����õ���Ҫ����*/
typedef struct _CONFIG_MSG											
{
  uint8 mac[6];																						 /*MAC��ַ*/
  uint8 lip[4];																						 /*local IP����IP��ַ*/
  uint8 sub[4];																						 /*��������*/
  uint8 gw[4];																						 /*����*/	
  uint8 dns[4];																						 /*DNS��������ַ*/
  uint8 rip[4];																						 /*remote IPԶ��IP��ַ*/
}CONFIG_MSG;
#pragma pack()

extern CONFIG_MSG  	ConfigMsg;

/*MCU������غ���*/
void bsp_W5500_init(void);

/*W5500SPI��غ���*/
void IINCHIP_WRITE( uint32 addrbsb,  uint8 data);			/*д��һ��8λ���ݵ�W5500*/
uint8 IINCHIP_READ(uint32 addrbsb);													/*��W5500����һ��8λ����*/
uint16 wiz_write_buf(uint32 addrbsb,uint8* buf,uint16 len);	/*��W5500д��len�ֽ�����*/
uint16 wiz_read_buf(uint32 addrbsb, uint8* buf,uint16 len);	/*��W5500����len�ֽ�����*/

/*W5500����������غ���*/
void reset_w5500(void);				/*Ӳ��λW5500*/
void set_w5500_mac(uint8* mac);		/*����W5500��MAC��ַ*/
void set_w5500_ip(void);			        /*����W5500��IP��ַ*/
void setKPALVTR(SOCKET i,uint8 val);
void timer_counter_process(void);
#endif
