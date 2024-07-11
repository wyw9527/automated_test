#include "w5500_conf.h"
#include "w5500.h"
#include "dhcp.h"
#include "rtthread.h"



CONFIG_MSG  ConfigMsg;

uint8 local_ip[4]={192,168,1,25};	

uint8 subnet[4]={255,255,255,0};	
uint8 gateway[4]={192,168,1,1};	
uint8 dns_server[4]={114,114,114,114};

uint16 local_port=8080;	

/***************************************************************************
* FuncName     : set_w5500_ip()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : RGxiang         Version:    V1.0      Date:    2022/8/15
***************************************************************************/
void set_w5500_ip(void)
{			
    memcpy(ConfigMsg.lip, local_ip,4);
    memcpy(ConfigMsg.sub, subnet,4);
    memcpy(ConfigMsg.gw, gateway,4);
    memcpy(ConfigMsg.dns, dns_server,4);		

    setSUBR(ConfigMsg.sub);
    setGAR(ConfigMsg.gw);
    setSIPR(ConfigMsg.lip);
}

/***************************************************************************
* FuncName     : set_w5500_mac()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : RGxiang         Version:    V1.0      Date:    2022/8/15
***************************************************************************/
void set_w5500_mac(uint8* mac)
{
    memcpy(ConfigMsg.mac, mac, 6);
    setSHAR(ConfigMsg.mac);	/**/
}

#define  SPI_W5500_CS_LOW()            gpio_bit_reset(GPIOA, GPIO_PIN_4)
#define  SPI_W5500_CS_HIGH()           gpio_bit_set(GPIOA, GPIO_PIN_4)

//static spi_delay_func spi_delay = NULL;
/***************************************************************************
* FuncName     : W5500_pin_init()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : RGxiang         Version:    V1.0      Date:    2022/8/15
***************************************************************************/
static void W5500_pin_init()
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_AF);
    /* INT PC7*/
    gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
    /* W5500_RST(PC6) GPIO pin configuration */
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    /* SPI0_CS(PA4) GPIO pin configuration */
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
    
    /* SPI0_SCK(PA5)and SPI1_MOSI(PA7) GPIO pin configuration */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
    
    /* SPI1_MISO(PA6) GPIO pin configuration */
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    
}

/***************************************************************************
* FuncName     : spi_config()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : RGxiang         Version:    V1.0      Date:    2022/8/15
***************************************************************************/
static void spi_config()
{
    spi_parameter_struct spi_stu;
    
    SPI_W5500_CS_HIGH();
    rcu_periph_clock_enable(RCU_SPI0);
    
    spi_stu.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
    spi_stu.device_mode = SPI_MASTER;
    spi_stu.frame_size = SPI_FRAMESIZE_8BIT;
    spi_stu.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_stu.nss = SPI_NSS_SOFT;
    spi_stu.prescale = SPI_PSC_256;
    spi_stu.endian = SPI_ENDIAN_MSB;
    spi_init(SPI0,&spi_stu);
    
    spi_enable(SPI0);
    
}

/***************************************************************************
* FuncName     : bsp_W5500_init()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : RGxiang         Version:    V1.0      Date:    2022/8/15
***************************************************************************/
void bsp_W5500_init(void)
{
    W5500_pin_init();
    
    spi_config();
}

/***************************************************************************
* FuncName     : wiz_cs()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : RGxiang         Version:    V1.0      Date:    2022/8/15
***************************************************************************/
void wiz_cs(uint8_t val)
{
    if (val == LOW)
    {
        SPI_W5500_CS_LOW();
    }
    else if (val == HIGH)
    {
        SPI_W5500_CS_HIGH();
    }	
}

/***************************************************************************
* FuncName     : iinchip_csoff()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : RGxiang         Version:    V1.0      Date:    2022/8/15
***************************************************************************/
void iinchip_csoff(void)
{
	wiz_cs(LOW);
}

/***************************************************************************
* FuncName     : iinchip_cson()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : RGxiang         Version:    V1.0      Date:    2022/8/15
***************************************************************************/
void iinchip_cson(void)
{	
   wiz_cs(HIGH);
}

/***************************************************************************
* FuncName     : reset_w5500()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : RGxiang         Version:    V1.0      Date:    2022/8/15
***************************************************************************/
void reset_w5500(void)
{
    /* W5500_RST(PC6)*/
    gpio_bit_set(GPIOC, GPIO_PIN_6);
    rt_thread_mdelay(20);
    //delay_us(50);  
    gpio_bit_reset(GPIOC, GPIO_PIN_6);
    rt_thread_mdelay(20);
    //delay_us(50);
}

/***************************************************************************
* FuncName     : spi_send_byte()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : RGxiang         Version:    V1.0      Date:    2022/8/15
***************************************************************************/
static uint8_t spi_send_byte(uint8_t byte)
{
    uint32_t w_cnt = 0;
    
    /* loop while data register in not emplty */
    while ((RESET == spi_i2s_flag_get(SPI0,SPI_FLAG_TBE))&& (w_cnt < 200000))
    {
        w_cnt++;
    }

    /* send byte through the SPI1 peripheral */
    spi_i2s_data_transmit(SPI0,byte);

    /* wait to receive a byte */
    w_cnt = 0;
    while((RESET == spi_i2s_flag_get(SPI0,SPI_FLAG_RBNE))&& (w_cnt < 200000))
    {
        w_cnt++;
    }
    
    /* return the byte read from the SPI bus */
    return(spi_i2s_data_receive(SPI0));
}

/***************************************************************************
* FuncName     : IINCHIP_SpiSendData()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : RGxiang         Version:    V1.0      Date:    2022/8/15
***************************************************************************/
uint8  IINCHIP_SpiSendData(uint8 dat)
{
	return(spi_send_byte(dat));
}

/***************************************************************************
* FuncName     : IINCHIP_WRITE()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : RGxiang         Version:    V1.0      Date:    2022/8/15
***************************************************************************/
void IINCHIP_WRITE( uint32 addrbsb,  uint8 data)
{
	iinchip_csoff();                              		
	IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);	
	IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
	IINCHIP_SpiSendData( (addrbsb & 0x000000F8) + 4);  
	IINCHIP_SpiSendData(data);                   
	iinchip_cson();                            
}

/***************************************************************************
* FuncName     : IINCHIP_READ()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : RGxiang         Version:    V1.0      Date:    2022/8/15
***************************************************************************/
uint8 IINCHIP_READ(uint32 addrbsb)
{
	uint8 data = 0;
	iinchip_csoff();                            
	IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
	IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
	IINCHIP_SpiSendData( (addrbsb & 0x000000F8))    ;
	data = IINCHIP_SpiSendData(0x00);            
	iinchip_cson();                               
	return data;    
}

/***************************************************************************
* FuncName     : wiz_write_buf()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : RGxiang         Version:    V1.0      Date:    2022/8/15
***************************************************************************/
uint16 wiz_write_buf(uint32 addrbsb,uint8* buf,uint16 len)
{
	uint16 idx = 0;
	//if(len == 0) printf(" Unexpected2 length 0\r\n");
	iinchip_csoff();                               
	IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
	IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
	IINCHIP_SpiSendData( (addrbsb & 0x000000F8) + 4); 
	for(idx = 0; idx < len; idx++)
	{
		IINCHIP_SpiSendData(buf[idx]);
	}
	iinchip_cson();                           
	return len;  
}

/***************************************************************************
* FuncName     : wiz_read_buf()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : RGxiang         Version:    V1.0      Date:    2022/8/15
***************************************************************************/
uint16 wiz_read_buf(uint32 addrbsb, uint8* buf,uint16 len)
{
  uint16 idx = 0;
  //if(len == 0)
  //{
  //  printf(" Unexpected2 length 0\r\n");
  //}
  iinchip_csoff();                                
  IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
  IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
  IINCHIP_SpiSendData( (addrbsb & 0x000000F8));    
  for(idx = 0; idx < len; idx++)                   
  {
    buf[idx] = IINCHIP_SpiSendData(0x00);
  }
  iinchip_cson();                                  
  return len;
}

void timer_counter_process(void)
{
    DHCP_Timer_Handler();
}

void setKPALVTR(SOCKET i,uint8 val)
{
  IINCHIP_WRITE(Sn_KPALVTR(i),val);
}
