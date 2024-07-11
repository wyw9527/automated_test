#ifndef _DRV_SERIAL_H_
#define _DRV_SERIAL_H_

#include "stdio.h"
#include "drv.h"

/* 应用层与底层顺序对应，不能随意改动 */
typedef enum
{
    DEV_SERIAL_SERV = 0,
    DEV_SERIAL_RFID,
    DEV_SERIAL_POWER,
    DEV_SERIAL_LOAD,
    DEV_SERIAL_NUM,
} dev_serial_type;

typedef enum
{
    SERIAL_BAUT_2400,
    SERIAL_BAUT_4800,
    SERIAL_BAUT_9600,
    SERIAL_BAUT_115200,
} DRV_SERIAL_BAUT_TYPE;

typedef enum
{
    SERIAL_WL_8BIT,
    SERIAL_WL_9BIT,
} DRV_SERIAL_WL_TYPE;

typedef enum
{
    SERIAL_STB_1BIT,
    SERIAL_STB_2BIT,
    SERIAL_STB_0_5BIT,
    SERIAL_STB_1_5BIT,
} DRV_SERIAL_STB_TYPE;

typedef enum
{
    SERIAL_PARITY_NONE,
    SERIAL_PARITY_ODD,
    SERIAL_PARITY_EVEN,
} DRV_SERIAL_PARITY_TYPE;

typedef enum
{
    SERIAL_DMA_ENABLE,
    SERIAL_DMA_DISABLE,
} DRV_SERIAL_DMA_TYPE;

typedef struct
{
    uint16_t wr_idx;
    uint16_t rd_idx;
    uint8_t *buff;
    uint16_t buff_size;
    uint8_t wr_mirror;
    uint8_t rd_mirror;
} drv_serial_msg_buff_stu;

typedef struct
{
    uint8_t *buff;
    uint16_t len;
} drv_serial_rx_stu;

typedef union
{
    uint32_t val;
    struct
    {
        uint16_t start;
        uint16_t len;
    } ele;
} drv_serial_mq_data_stu;

typedef struct
{
    uint8_t conn_status : 1;
    uint8_t tx_fnshed : 1;
} drv_serial_flag_stu;

typedef struct
{
    DRV_SERIAL_BAUT_TYPE baut;
    DRV_SERIAL_PARITY_TYPE parity;
    DRV_SERIAL_WL_TYPE wl;
    DRV_SERIAL_STB_TYPE stb;
    DRV_SERIAL_DMA_TYPE dma_sta;
} drv_serial_init_param_stu;

typedef struct
{
    bsp_uart_dev_stu *uart; /* uart设备 */
    bsp_io_dev_stu *io;     /* 收发控制 */
} drv_serial_pub_stu;

typedef struct
{
    BSP_UART_TYPE uart_x;
    BSP_SOURCE_TYPE io_ctrl_x;
    struct rt_mailbox mb_rx;
    uint8_t mb_poll[40];
    drv_serial_msg_buff_stu msg_buff;

    drv_serial_flag_stu flag; /* 串口标志 */

} drv_serial_stu;

typedef struct
{
    void (*dev_param_init)(drv_serial_init_param_stu *param);

    void (*dev_send_msg)(uint8_t *msg, uint16_t len);

    uint8_t (*dev_get_msg)(drv_serial_rx_stu *rx_msg, uint32_t tmout);

    void (*dev_rx_buff_clear)();

} drv_serial_dev_stu;

#endif