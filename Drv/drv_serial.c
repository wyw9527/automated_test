#include "drv_serial.h"


#define DRV_SERV_RX_BUFF_SIZE 1152
#define DRV_SERV_TX_BUFF_SIZE 320
#define DRV_SERV_RX_RING_BUFF_SIZE 2304

#define DRV_LOAD_RX_BUFF_SIZE 1152
#define DRV_LOAD_TX_BUFF_SIZE 320
#define DRV_LOAD_RX_RING_BUFF_SIZE 1152

#define DRV_RFID_RX_BUFF_SIZE 128
#define DRV_RFID_TX_BUFF_SIZE 128
#define DRV_RFID_RX_RING_BUFF_SIZE 128

#define DRV_POWER_RX_BUFF_SIZE 32
#define DRV_POWER_TX_BUFF_SIZE 32
#define DRV_POWER_RX_RING_BUFF_SIZE 32

static drv_serial_pub_stu serial_pub;
static drv_serial_stu serial_conf[DEV_SERIAL_NUM];

static uint8_t drv_serv_rx_buff[DRV_SERV_RX_BUFF_SIZE];
static uint8_t drv_serv_tx_buff[DRV_SERV_TX_BUFF_SIZE];
static uint8_t drv_serv_ring[DRV_SERV_RX_RING_BUFF_SIZE];

static uint8_t drv_load_rx_buff[DRV_LOAD_RX_BUFF_SIZE];
static uint8_t drv_load_tx_buff[DRV_LOAD_TX_BUFF_SIZE];
static uint8_t drv_load_ring[DRV_LOAD_RX_RING_BUFF_SIZE];

static uint8_t drv_rfid_rx_buff[DRV_RFID_RX_BUFF_SIZE];
static uint8_t drv_rfid_tx_buff[DRV_RFID_TX_BUFF_SIZE];
static uint8_t drv_rfid_ring[DRV_RFID_RX_RING_BUFF_SIZE];

static uint8_t drv_power_rx_buff[DRV_POWER_RX_BUFF_SIZE];
static uint8_t drv_power_tx_buff[DRV_POWER_TX_BUFF_SIZE];
static uint8_t drv_power_ring[DRV_POWER_RX_RING_BUFF_SIZE];

/***************************************************************************************
 * @Function    : drv_msg_buff_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/29
 ****************************************************************************************/
static void drv_msg_buff_init(drv_serial_stu *serial, uint8_t *buff, uint16_t buff_size)
{
    memset((uint8_t *)&serial->msg_buff, 0, sizeof(serial->msg_buff));
    serial->msg_buff.buff = buff;
    serial->msg_buff.buff_size = buff_size;
}

/***************************************************************************************
 * @Function    : drv_msg_buff_state()
 *
 * @Param       :
 *
 * @Return      : 0 EMPTY  1 HALFFULL  2 FULL
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/30
 ****************************************************************************************/
static uint8_t drv_msg_buff_state(drv_serial_stu *serial)
{
    if (serial->msg_buff.wr_idx == serial->msg_buff.rd_idx)
    {
        if (serial->msg_buff.wr_mirror == serial->msg_buff.rd_mirror)
        {
            if (serial->msg_buff.wr_idx == 0)
            {
                return 0;
            }
            return 1;
        }
        return 2;
    }
    return 1;
}

/***************************************************************************************
 * @Function    : drv_msg_buff_get_write_idx()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/30
 ****************************************************************************************/
static uint8_t drv_msg_buff_get_write_idx(drv_serial_stu *serial, uint16_t len, uint16_t *start_idx, uint8_t *mirror)
{
    uint8_t sta;
    uint16_t left_capa;

    sta = drv_msg_buff_state(serial);
    /* 缓冲区为空 */
    if (sta == 0)
    {
        left_capa = serial->msg_buff.buff_size;
        if (len > left_capa)
            return RT_ERROR;
        *start_idx = 0;
        *mirror = 0;
        return RT_EOK;
    }

    /* 缓冲区为满 */
    if (sta == 2)
    {
        return RT_EFULL;
    }

    /* 缓冲区部分有数据 */
    if (serial->msg_buff.wr_mirror != serial->msg_buff.rd_mirror)
    {
        /* 不在一页 */
        left_capa = serial->msg_buff.rd_idx - serial->msg_buff.wr_idx;
        if (left_capa < len)
            return RT_EFULL;
        *start_idx = serial->msg_buff.wr_idx;
        return RT_EOK;
    }

    /* 在一页 尾部*/
    left_capa = serial->msg_buff.buff_size - serial->msg_buff.wr_idx;
    if (left_capa >= len)
    {
        *start_idx = serial->msg_buff.wr_idx;
        return RT_EOK;
    }

    /* 头部 */
    if (serial->msg_buff.rd_idx >= len)
    {
        *start_idx = 0;
        *mirror = 1;
        return RT_EOK;
    }
    return RT_EFULL;
}

/***************************************************************************************
 * @Function    : drv_msg_buff_write()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/29
 ****************************************************************************************/
static uint8_t drv_msg_buff_write(drv_serial_stu *serial, uint8_t *buff, uint16_t len)
{
    uint8_t ret, mirror = 0;
    drv_serial_mq_data_stu mq_data;

    mq_data.ele.len = len;
    if ((ret = drv_msg_buff_get_write_idx(serial, len, &mq_data.ele.start, &mirror)) != RT_EOK)
    {
        return ret;
    }

    memcpy(&serial->msg_buff.buff[mq_data.ele.start], buff, len);
    serial->msg_buff.wr_idx = mq_data.ele.start + len;
    if (mirror > 0)
    {
        serial->msg_buff.wr_mirror = (serial->msg_buff.wr_mirror > 0) ? 0 : 1;
    }
    rt_mb_send(&serial->mb_rx, mq_data.val);

    return RT_EOK;
}

/***************************************************************************************
 * @Function    : drv_msg_buff_write()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/29
 ****************************************************************************************/
static uint8_t drv_msg_buff_read(drv_serial_stu *serial, drv_serial_rx_stu *rx_msg, uint32_t tmout)
{
    drv_serial_mq_data_stu mq_data;

    /* 读取邮箱数据 */
    if (rt_mb_recv(&serial->mb_rx, (rt_ubase_t *)&mq_data.val, tmout) != RT_EOK)
        return RT_EEMPTY;

    /* 数据长度校验 */
    if (mq_data.ele.start + mq_data.ele.len >= serial->msg_buff.buff_size)
    {
        return RT_ERROR;
    }

    /* 换页处理 */
    if (mq_data.ele.start < serial->msg_buff.rd_idx)
    {
        serial->msg_buff.rd_mirror = (serial->msg_buff.rd_mirror > 0) ? 0 : 1;
    }
    serial->msg_buff.rd_idx = mq_data.ele.start + mq_data.ele.len;

    rx_msg->buff = &serial->msg_buff.buff[mq_data.ele.start];
    rx_msg->len = mq_data.ele.len;

    return RT_EOK;
}

/***************************************************************************************
 * @Function    : drv_serial_get_uart_param()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/8
 ****************************************************************************************/
static void drv_serial_get_uart_param(drv_serial_init_param_stu *drv_param, bsp_uart_config_param_stu *bsp_param)
{
    if (drv_param == RT_NULL)
    {
        bsp_param->baut = 9600;
        bsp_param->wd_len = USART_WL_8BIT;
        bsp_param->stop_bits = USART_STB_1BIT;
        bsp_param->parity = USART_PM_NONE;
        bsp_param->dma_flag = 1;
        return;
    }

    switch (drv_param->baut)
    {
    case SERIAL_BAUT_2400:
        bsp_param->baut = 2400;
        break;
    case SERIAL_BAUT_4800:
        bsp_param->baut = 4800;
        break;
    case SERIAL_BAUT_115200:
        bsp_param->baut = 115200;
        break;
    default:
        bsp_param->baut = 9600;
        break;
    }

    switch (drv_param->wl)
    {
    case SERIAL_WL_9BIT:
        bsp_param->wd_len = USART_WL_9BIT;
        break;
    default:
        bsp_param->wd_len = USART_WL_8BIT;
    }

    switch (drv_param->stb)
    {
    case SERIAL_STB_2BIT:
        bsp_param->stop_bits = USART_STB_2BIT;
        break;
    case SERIAL_STB_0_5BIT:
        bsp_param->stop_bits = USART_STB_0_5BIT;
        break;
    case SERIAL_STB_1_5BIT:
        bsp_param->stop_bits = USART_STB_1_5BIT;
        break;
    default:
        bsp_param->stop_bits = USART_STB_1BIT;
        break;
    }

    switch (drv_param->parity)
    {
    case SERIAL_PARITY_ODD:
        bsp_param->parity = USART_PM_ODD;
        break;
    case SERIAL_PARITY_EVEN:
        bsp_param->parity = USART_PM_EVEN;
        break;
    default:
        bsp_param->parity = USART_PM_NONE;
        break;
    }

    if (drv_param->dma_sta == SERIAL_DMA_DISABLE)
    {
        bsp_param->dma_flag = 0;
    }
    else
    {
        bsp_param->dma_flag = 1;
    }
}

/***************************************************************************************
 * @Function    : drv_servial_uart_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/9
 ****************************************************************************************/
static void drv_servial_uart_init(drv_serial_stu *serial, drv_serial_init_param_stu *param, uint8_t *rx_buff, uint16_t buff_size, func_rx_msg_hook rx_hook, func_tx_fnsh_hook tx_fnsh_hook)
{
    bsp_uart_config_param_stu bsp_param;

    /* uart have already connected */
    //    if(serial->flag.conn_status != 0) return;

    serial_pub.io->bsp_io_init(serial->io_ctrl_x);

    /* serial uart config parameter */
    drv_serial_get_uart_param(param, &bsp_param);

    serial_pub.uart->bsp_uart_rx_func_regist(serial->uart_x, rx_hook, rx_buff, buff_size);
    serial_pub.uart->bsp_uart_tx_fnsh_func_regist(serial->uart_x, tx_fnsh_hook);

    /* uart param init and start */
    serial_pub.uart->bsp_uart_init(serial->uart_x, &bsp_param);

    serial->flag.tx_fnshed = 1;
    serial->flag.conn_status = 1;
}

/***************************************************************************************
 * @Function    : drv_serial_uart_rx_hook()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/9
 ****************************************************************************************/
static void drv_serial_uart_rx_hook(drv_serial_stu *serial, uint8_t *buff, uint16_t rx_len)
{
    drv_msg_buff_write(serial, buff, rx_len);
}

/***************************************************************************************
 * @Function    : drv_serial_uart_send_msg()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/9
 ****************************************************************************************/
static void drv_serial_uart_send_msg(drv_serial_stu *serial, uint8_t *buff, uint8_t *msg, uint16_t len)
{
    uint32_t tx_wait_count = 0;

    if (msg == RT_NULL || len == 0)
    {
        return;
    }

    if (serial->flag.conn_status == 0)
    {
        return;
    }

    while (serial->flag.tx_fnshed == 0)
    {
        if (++tx_wait_count > 1000000)
        {
            return;
        }
    }

    serial->flag.tx_fnshed = 0;
    if (serial_pub.io != RT_NULL)
    {
        serial_pub.io->bsp_io_set(serial->io_ctrl_x);
    }

    memcpy(buff, msg, len);
    serial_pub.uart->bsp_uart_tx_msg(serial->uart_x, buff, len);
}

/***************************************************************************************
 * @Function    : drv_dev_tx_fnsh_hook()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/13
 ****************************************************************************************/
static void drv_dev_tx_fnsh_hook(drv_serial_stu *serial)
{

    if (serial_pub.io == RT_NULL)
    {
        return;
    }

    serial_pub.io->bsp_io_reset(serial->io_ctrl_x);

    serial->flag.tx_fnshed = 1;
}

/***************************************************************************************
 * @Function    : inv_tx_fnsh_hook()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/9
 ****************************************************************************************/
static void load_tx_fnsh_hook(void)
{
    drv_dev_tx_fnsh_hook(&serial_conf[DEV_SERIAL_LOAD]);
}

/***************************************************************************************
 * @Function    : serv_tx_fnsh_hook()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/9
 ****************************************************************************************/
static void serv_tx_fnsh_hook(void)
{
    drv_dev_tx_fnsh_hook(&serial_conf[DEV_SERIAL_SERV]);
}

/***************************************************************************************
 * @Function    : rfid_tx_fnsh_hook()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/9
 ****************************************************************************************/
static void rfid_tx_fnsh_hook(void)
{
    drv_dev_tx_fnsh_hook(&serial_conf[DEV_SERIAL_RFID]);
}

/***************************************************************************************
 * @Function    : meter_tx_fnsh_hook()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/9
 ****************************************************************************************/
static void power_tx_fnsh_hook(void)
{
    drv_dev_tx_fnsh_hook(&serial_conf[DEV_SERIAL_POWER]);
}

/***************************************************************************************
 * @Function    : inv_rx_hook()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/9
 ****************************************************************************************/
static void load_rx_hook(uint16_t rx_len)
{
    drv_serial_uart_rx_hook(&serial_conf[DEV_SERIAL_LOAD], drv_load_rx_buff, rx_len);
}

/***************************************************************************************
 * @Function    : serv_rx_hook()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/9
 ****************************************************************************************/
static void serv_rx_hook(uint16_t rx_len)
{
    drv_serial_uart_rx_hook(&serial_conf[DEV_SERIAL_SERV], drv_serv_rx_buff, rx_len);
}

/***************************************************************************************
 * @Function    : rfid_rx_hook()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/9
 ****************************************************************************************/
static void rfid_rx_hook(uint16_t rx_len)
{
    drv_serial_uart_rx_hook(&serial_conf[DEV_SERIAL_RFID], drv_rfid_rx_buff, rx_len);
}

/***************************************************************************************
 * @Function    : meter_rx_hook()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/9
 ****************************************************************************************/
static void power_rx_hook(uint16_t rx_len)
{
    drv_serial_uart_rx_hook(&serial_conf[DEV_SERIAL_POWER], drv_power_rx_buff, rx_len);
}

/***************************************************************************************
 * @Function    : inv_param_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/9
 ****************************************************************************************/
static void load_param_init(drv_serial_init_param_stu *param)
{
    serial_conf[DEV_SERIAL_LOAD].uart_x = BSP_UART3;//BSP_UART3
    serial_conf[DEV_SERIAL_LOAD].io_ctrl_x = BSP_SOURCE_DO_LOAD_RS485_CTRL;//BSP_SOURCE_DO_INV_RS485_CTRL
    // serial_conf[DEV_SERIAL_INV].io_ctrl_x = BSP_IO_NUM;

    rt_mb_init(&serial_conf[DEV_SERIAL_LOAD].mb_rx, "load_rx", &serial_conf[DEV_SERIAL_LOAD].mb_poll[0], sizeof(serial_conf[DEV_SERIAL_LOAD].mb_poll) / 4, RT_IPC_FLAG_FIFO);

    drv_msg_buff_init(&serial_conf[DEV_SERIAL_LOAD], drv_load_ring, DRV_LOAD_RX_RING_BUFF_SIZE);

    drv_servial_uart_init(&serial_conf[DEV_SERIAL_LOAD], param, drv_load_rx_buff, DRV_LOAD_RX_BUFF_SIZE, load_rx_hook, load_tx_fnsh_hook);
}

/***************************************************************************************
 * @Function    : serv_param_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/9
 ****************************************************************************************/
static void serv_param_init(drv_serial_init_param_stu *param)
{
    serial_conf[DEV_SERIAL_SERV].uart_x = BSP_UART0;
    serial_conf[DEV_SERIAL_SERV].io_ctrl_x = BSP_SOURCE_NUM;

    rt_mb_init(&serial_conf[DEV_SERIAL_SERV].mb_rx, "serv_rx", &serial_conf[DEV_SERIAL_SERV].mb_poll[0], sizeof(serial_conf[DEV_SERIAL_SERV].mb_poll) / 4, RT_IPC_FLAG_FIFO);

    drv_msg_buff_init(&serial_conf[DEV_SERIAL_SERV], drv_serv_ring, DRV_SERV_RX_RING_BUFF_SIZE);

    drv_servial_uart_init(&serial_conf[DEV_SERIAL_SERV], param, drv_serv_rx_buff, DRV_SERV_RX_BUFF_SIZE, serv_rx_hook, serv_tx_fnsh_hook);
}

/***************************************************************************************
 * @Function    : rfid_param_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/9
 ****************************************************************************************/
static void rfid_param_init(drv_serial_init_param_stu *param)
{
    serial_conf[DEV_SERIAL_RFID].uart_x = BSP_UART1;
    serial_conf[DEV_SERIAL_RFID].io_ctrl_x = BSP_SOURCE_NUM;

    rt_mb_init(&serial_conf[DEV_SERIAL_RFID].mb_rx, "rfid_rx", &serial_conf[DEV_SERIAL_RFID].mb_poll[0], sizeof(serial_conf[DEV_SERIAL_RFID].mb_poll) / 4, RT_IPC_FLAG_FIFO);

    drv_msg_buff_init(&serial_conf[DEV_SERIAL_RFID], drv_rfid_ring, DRV_RFID_RX_RING_BUFF_SIZE);

    drv_servial_uart_init(&serial_conf[DEV_SERIAL_RFID], param, drv_rfid_rx_buff, DRV_RFID_RX_BUFF_SIZE, rfid_rx_hook, rfid_tx_fnsh_hook);
}

/***************************************************************************************
 * @Function    : meter_param_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/9
 ****************************************************************************************/
static void power_param_init(drv_serial_init_param_stu *param)
{
    serial_conf[DEV_SERIAL_POWER].uart_x = BSP_UART2;
    //    serial_conf[DEV_SERIAL_METER].io_ctrl_x = BSP_DO_METER_RS485_CTRL;
    serial_conf[DEV_SERIAL_POWER].io_ctrl_x = BSP_SOURCE_NUM;

    rt_mb_init(&serial_conf[DEV_SERIAL_POWER].mb_rx, "power_rx", &serial_conf[DEV_SERIAL_POWER].mb_poll[0], sizeof(serial_conf[DEV_SERIAL_POWER].mb_poll) / 4, RT_IPC_FLAG_FIFO);

    drv_msg_buff_init(&serial_conf[DEV_SERIAL_POWER], drv_power_ring, DRV_POWER_RX_RING_BUFF_SIZE);

    drv_servial_uart_init(&serial_conf[DEV_SERIAL_POWER], param, drv_power_rx_buff, DRV_POWER_RX_BUFF_SIZE, power_rx_hook, power_tx_fnsh_hook);
}

/***************************************************************************************
 * @Function    : inv_get_msg()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description : return rx date length
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/9
 ****************************************************************************************/
static uint8_t load_get_msg(drv_serial_rx_stu *rx_msg, uint32_t tmout)
{
    return drv_msg_buff_read(&serial_conf[DEV_SERIAL_LOAD], rx_msg, tmout);
}

/***************************************************************************************
 * @Function    : serv_get_msg()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/9
 ****************************************************************************************/
static uint8_t serv_get_msg(drv_serial_rx_stu *rx_msg, uint32_t tmout)
{
    return drv_msg_buff_read(&serial_conf[DEV_SERIAL_SERV], rx_msg, tmout);
}

/***************************************************************************************
 * @Function    : rfid_get_msg()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/9
 ****************************************************************************************/
static uint8_t rfid_get_msg(drv_serial_rx_stu *rx_msg, uint32_t tmout)
{
    return drv_msg_buff_read(&serial_conf[DEV_SERIAL_RFID], rx_msg, tmout);
}

/***************************************************************************************
 * @Function    : meter_get_msg()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/9
 ****************************************************************************************/
static uint8_t power_get_msg(drv_serial_rx_stu *rx_msg, uint32_t tmout)
{
    return drv_msg_buff_read(&serial_conf[DEV_SERIAL_POWER], rx_msg, tmout);
}

/***************************************************************************************
 * @Function    : inv_send_msg()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/9
 ****************************************************************************************/
static void load_send_msg(uint8_t *msg, uint16_t len)
{
    drv_serial_uart_send_msg(&serial_conf[DEV_SERIAL_LOAD], drv_load_tx_buff, msg, len);
}

/***************************************************************************************
 * @Function    : serv_send_msg()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/9
 ****************************************************************************************/
static void serv_send_msg(uint8_t *msg, uint16_t len)
{
    drv_serial_uart_send_msg(&serial_conf[DEV_SERIAL_SERV], drv_serv_tx_buff, msg, len);
}

/***************************************************************************************
 * @Function    : rfid_send_msg()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/9
 ****************************************************************************************/
static void rfid_send_msg(uint8_t *msg, uint16_t len)
{
    drv_serial_uart_send_msg(&serial_conf[DEV_SERIAL_RFID], drv_rfid_tx_buff, msg, len);
}

/***************************************************************************************
 * @Function    : meter_send_msg()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/9
 ****************************************************************************************/
static void power_send_msg(uint8_t *msg, uint16_t len)
{
    drv_serial_uart_send_msg(&serial_conf[DEV_SERIAL_POWER], drv_power_tx_buff, msg, len);
}

/***************************************************************************************
 * @Function    : drv_serial_uart_rx_buff_clear()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/6
 ****************************************************************************************/
static void drv_serial_uart_rx_buff_clear(drv_serial_stu *serial, uint8_t *buff, uint16_t size)
{
    uint32_t tmp;

    while (rt_mb_recv(&serial->mb_rx, (rt_ubase_t *)&tmp, 0) == RT_EOK)
        ;

    memset(buff, 0, size);
}

/***************************************************************************************
 * @Function    : serv_rx_buff_clear()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/6
 ****************************************************************************************/
static void serv_rx_buff_clear(void)
{
    drv_serial_uart_rx_buff_clear(&serial_conf[DEV_SERIAL_SERV], drv_serv_ring, DRV_SERV_RX_RING_BUFF_SIZE);
}

/***************************************************************************************
 * @Function    : rfid_rx_buff_clear()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/6
 ****************************************************************************************/
static void rfid_rx_buff_clear(void)
{
    drv_serial_uart_rx_buff_clear(&serial_conf[DEV_SERIAL_RFID], drv_rfid_ring, DRV_RFID_RX_RING_BUFF_SIZE);
}

/***************************************************************************************
 * @Function    : inv_rx_buff_clear()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/6
 ****************************************************************************************/
static void load_rx_buff_clear(void)
{
    drv_serial_uart_rx_buff_clear(&serial_conf[DEV_SERIAL_LOAD], drv_load_ring, DRV_LOAD_RX_RING_BUFF_SIZE);
}

/***************************************************************************************
 * @Function    : meter_rx_buff_clear()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/6
 ****************************************************************************************/
static void power_rx_buff_clear(void)
{
    drv_serial_uart_rx_buff_clear(&serial_conf[DEV_SERIAL_POWER], drv_power_ring, DRV_POWER_RX_RING_BUFF_SIZE);
}

/***************************************************************************************
 * @Function    : drv_serial_load_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/9
 ****************************************************************************************/
static void drv_serial_load_create(void)
{
    static drv_serial_dev_stu load;

    load.dev_param_init = load_param_init;

    load.dev_get_msg = load_get_msg;

    load.dev_send_msg = load_send_msg;

    load.dev_rx_buff_clear = load_rx_buff_clear;

    drv_dev_append(DRV_DEV_SERIAL_LOAD, &load);
}

/***************************************************************************************
 * @Function    : drv_serial_serv_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/9
 ****************************************************************************************/
static void drv_serial_serv_create(void)
{
    static drv_serial_dev_stu serv;

    serv.dev_param_init = serv_param_init;

    serv.dev_get_msg = serv_get_msg;

    serv.dev_send_msg = serv_send_msg;

    serv.dev_rx_buff_clear = serv_rx_buff_clear;

    drv_dev_append(DRV_DEV_SERIAL_SERV, &serv);
}

/***************************************************************************************
 * @Function    : drv_serial_rfid_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/15
 ****************************************************************************************/
static void drv_serial_rfid_create(void)
{
    static drv_serial_dev_stu rfid;

    rfid.dev_param_init = rfid_param_init;

    rfid.dev_get_msg = rfid_get_msg;

    rfid.dev_send_msg = rfid_send_msg;

    rfid.dev_rx_buff_clear = rfid_rx_buff_clear;

    drv_dev_append(DRV_DEV_SERIAL_RFID, &rfid);
}

/***************************************************************************************
 * @Function    : drv_serial_power_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/15
 ****************************************************************************************/
static void drv_serial_power_create(void)
{
    static drv_serial_dev_stu power;

    power.dev_param_init = power_param_init;

    power.dev_get_msg = power_get_msg;

    power.dev_send_msg = power_send_msg;

    power.dev_rx_buff_clear = power_rx_buff_clear;

    drv_dev_append(DRV_DEV_SERIAL_POWER, &power);
}

/***************************************************************************************
 * @Function    : drv_serial_pub_dev_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/13
 ****************************************************************************************/
static void drv_serial_pub_dev_init(void)
{
    serial_pub.uart = bsp_dev_find(BSP_DEV_NAME_UART);

    serial_pub.io = bsp_dev_find(BSP_DEV_NAME_IO);
}

/***************************************************************************************
 * @Function    : drv_serial_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/8
 ****************************************************************************************/
static int drv_serial_dev_create(void)
{
    drv_serial_pub_dev_init();

    drv_serial_serv_create();

    drv_serial_load_create();  //485

    drv_serial_rfid_create();  //232

    drv_serial_power_create(); //485

    return 0;
}

INIT_COMPONENT_EXPORT(drv_serial_dev_create);

