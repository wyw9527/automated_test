#ifndef _BSP_UART_H_
#define _BSP_UART_H_


#include "bsp.h"

typedef void (*func_rx_msg_hook)(uint16_t rx_len);
typedef void (*func_tx_fnsh_hook)(void);

/* The serial port enumeration types owned by the chip */
typedef enum
{
    BSP_UART0 = 0,
    BSP_UART1,
    BSP_UART2,
    BSP_UART3,
    BSP_UART4,
    BSP_UART_NUM
}BSP_UART_TYPE;

/* Serial port transceiver pin information */
typedef struct
{
    uint32_t tx_gpio_periph;
    uint32_t tx_gpio_pin;
    uint32_t rx_gpio_periph;
    uint32_t rx_gpio_pin;
}bsp_uart_pin_stu;

/* Serial port configuration parameter information */
typedef struct
{
    uint32_t baut;
    uint32_t wd_len;
    uint32_t stop_bits;
    uint32_t parity;
    uint8_t dma_flag;
}bsp_uart_config_param_stu;

/* Serial DMA configuration parameter information */
typedef struct
{
    uint32_t periph;
    dma_channel_enum tx_chann;
    dma_channel_enum rx_chann;
}bsp_uart_dma_stu;

typedef struct
{
    uint32_t periph;
    uint32_t remap;
}bsp_uart_param_stu;


typedef struct
{
    uint8_t use_dma_flag;
    uint32_t dma_periph;
    dma_channel_enum dma_tx_chann;
    dma_channel_enum dma_rx_chann;
    
    uint8_t* rx_buff;
    uint16_t rx_buff_size;
    uint16_t irq_rx_pos;
    
    uint8_t* tx_buff;
    uint16_t tx_len;
    uint8_t  tx_pos;
    
    func_rx_msg_hook rx_hook;
    func_tx_fnsh_hook tx_fnsh_hook;
    uint8_t tx_fnsh_flag;
}bsp_uart_stu;

typedef struct
{
    uint8_t enable;
    uint8_t rx_tmr;
    uint8_t tx_tmr;
}bsp_uart_oper_stu;

typedef struct
{
    /* Serial port initialization function */
    uint8_t (*bsp_uart_init)(BSP_UART_TYPE uart_x,bsp_uart_config_param_stu* param);
    
    /* Set the serial port receive callback function */
    uint8_t (*bsp_uart_rx_func_regist)(BSP_UART_TYPE uart_x,func_rx_msg_hook rx_func,uint8_t* rx_buff,uint16_t buff_size);
    
    /* Set serial port sending function */
    uint8_t (*bsp_uart_tx_msg)(BSP_UART_TYPE uart_x,uint8_t* tx_buff,uint16_t tx_len);
    
    /* Set serial port send completion callback function*/
    void (*bsp_uart_tx_fnsh_func_regist)(BSP_UART_TYPE uart_x,func_tx_fnsh_hook tx_fnsh_func);
    
}bsp_uart_dev_stu;



#endif