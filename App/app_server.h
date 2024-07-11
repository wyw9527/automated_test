#ifndef _APP_SERVER_H_
#define _APP_SERVER_H_

#include "app_system.h"

#define PPM_INFO_BAK_FLAG 0xA55AA55A

/* 服务协议选择 */
typedef struct
{
    uint32_t count; // 计数值

    uint8_t item_type; // 0 待定  1  ATE业务
} app_serv_procc_item_stu;

typedef struct
{
    uint8_t chrg_data_upt_flag[APP_PLUG_NUM];
    uint8_t *chrg_order_completed_flag[APP_PLUG_NUM];
    uint8_t chrg_sta[APP_PLUG_NUM];
    uint16_t chrg_data_period;
} app_serv_procc_data_stu;


typedef struct
{
    app_serv_procc_data_stu data;
    app_serv_procc_item_stu item;

} app_serv_procc_stu;

typedef struct
{
    char rx_buff[1024];
    uint16_t rx_len;
    
}app_net_uart_comm_stu;

typedef struct
{
    drv_serial_dev_stu *serial;

    cmp_prot_upper_dev_stu *upper;

    cmp_prot_mt_dev_stu *mt;

    cmp_ble_dev_stu *ble;

    cmp_power_dev_stu *power;

    cmp_load_dev_stu *load;

    cmp_w5500_dev_stu *w5500;

    app_net_uart_comm_stu w5500_comm;

    app_serv_procc_stu procc;


} app_serv_stu;

#endif