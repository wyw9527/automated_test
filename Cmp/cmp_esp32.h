#ifndef _CMP_ESP32_H_
#define _CMP_ESP32_H_

#include "stdio.h"
#include "cmp.h"

typedef uint8_t (*cmp_esp_at_cmd)(void);

typedef struct
{
    char pole_sn[28];
    char ssid[32];
    char pwd[32];
    char serv_addr[48];
    uint16_t port;
} cmp_esp32_param_stu;

typedef enum
{
    ESP_MOD_STATE_WAIT_INIT = 0,
    ESP_MOD_STATE_RESET_ABN,
    ESP_MOD_STATE_INIT_ABN,
    ESP_MOD_STATE_OK,
} CMP_ESP_MOD_STATE_TYPE;

typedef enum
{
    ESP_ROUTER_STATE_PARAM_INIT = 0,
    ESP_ROUTER_STATE_CONF_ERR,
    ESP_ROUTER_STATE_CONN_TMOUT,
    ESP_ROUTER_STATE_PWD_ERR,
    ESP_ROUTER_STATE_SSID_ERR,
    ESP_ROUTER_STATE_CONN_FAIL,
    ESP_ROUTER_STATE_OTH_ABN,
    ESP_ROUTER_STATE_OK,
} CMP_ESP_ROUTER_STATE_TYPE;

typedef enum
{
    ESP_SERV_STATE_WAIT_INIT = 0,
    ESP_SERV_STATE_CONN_FAIL,
    ESP_SERV_STATE_OK
} CMP_ESP_SERV_STATE_TYPE;

typedef enum
{
    ESP_BLE_STATE_INIT = 0,
    ESP_BLE_STATE_CONF_ABN,
    ESP_BLE_STATE_WAIT_CONN,
    ESP_BLE_STATE_WAIT_CONN_ABN,
    ESP_BLE_STATE_WAIT_TMOUT,
    ESP_BLE_STATE_SPP_ABN,
    ESP_BLE_STATE_SPP,
} CMP_ESP_BLE_STATE_TYPE;

typedef enum
{
    ESP_WORK_STEP_MOD_RESET,
    ESP_WORK_STEP_MOD_INIT,
    ESP_WORK_STEP_WIFI_CONN,
    ESP_WORK_STEP_SERV_CONN,
    ESP_WORK_STEP_BLE_CONFIG,
    ESP_WORK_STEP_WAIT,
    ESP_WORK_STEP_BLE_CONV,
    ESP_WORK_STEP_SPP,
} CMP_ESP_WORK_STEP_TYPE;

typedef enum
{
    CMP_ESP_MOUDLE_ATE_USELESS,
    CMP_ESP_MOUDLE_ATE_USEFUL,
} CMP_ESP_MOUDLE_ATE_TYPE;

typedef struct
{
    CMP_ESP_MOD_STATE_TYPE mod;       /* esp32 模块状态 */
    CMP_ESP_ROUTER_STATE_TYPE router; /* 路由器状态 */
    CMP_ESP_SERV_STATE_TYPE server;   /* 服务器状态 */
    CMP_ESP_BLE_STATE_TYPE ble;       /* BLE 状态 */
    CMP_ESP_MOUDLE_ATE_TYPE ble_use;  /* 模块ATE_BLE状态 */
    CMP_ESP_MOUDLE_ATE_TYPE wifi_use; /* 模块ATE_WIFI状态 */
} cmp_esp32_sta_stu;

typedef struct
{
    CMP_ESP_WORK_STEP_TYPE step;
    uint16_t wait_count;
    uint8_t cmd_send_cnt;

    uint8_t wifi_state;    /* 0 未配置  1 已配置  2 连接过 */
    uint8_t ble_state;     /* 0 未配置  1 已配置 */
    uint8_t wifi_conn_cnt; /* wifi 连接失败次数 */
} cmp_esp32_procc_stu;

typedef struct
{
    drv_serial_rx_stu rx_msg;
    uint8_t rx_msg_flag;
} cmp_esp32_serial_stu;

typedef struct
{
    drv_serial_dev_stu *serial;

    drv_dido_do_dev_stu *reset_io;

    cmp_esp32_serial_stu msg;

    cmp_esp32_param_stu param;

    cmp_esp32_procc_stu procc;

    cmp_esp32_sta_stu status;

} cmp_esp32_stu;

typedef struct
{
    void (*esp32_work)(uint8_t period);

    void (*esp32_set_param)(cmp_esp32_param_stu *param);

    void (*esp32_set_serial)(drv_serial_dev_stu *serial);

    uint8_t (*esp32_get_usable_state)(void);

    uint8_t (*esp32_get_spp_mode)(void);

    uint8_t (*esp32_query_rx_app_msg)(drv_serial_rx_stu *rx_msg);

    void (*esp32_send_app_msg)(uint8_t *data, uint16_t len);

} cmp_esp32_dev_stu;

#endif