#ifndef _CMP_BLE_H_
#define _CMP_BLE_H_

#include "cmp.h"
#include "drv_serial.h"

typedef struct cmp_ble_sta_stu_s cmp_ble_sta_stu;

typedef enum
{
    CMP_BLE_CONN_STA,
    CMP_BLE_RX_MSG,
    
} CMP_BLE_CONN_STA_TYPE;

typedef struct
{
    uint8_t ble_name[28];
    uint8_t ble_sta;  //0 unconnected  1 connceted  2 Not find device  3 Not find Service  
                    // 4 Not find Char  5 ERROR

} cmp_ble_to_server_msg_ble_conn_sta_stu;


typedef void (*ble_conn_sta_hook_func)(CMP_BLE_CONN_STA_TYPE cmd, void *data);


typedef enum
{
    RESTART_BLE = 0,
    SET_CLIENT,
    SCAN_BROADCAST_PARAM,
    SCAN_BLE_NAME,
    CONN_TARGET_BLE,
    CHANGE_TRANS_LEN,
    GET_DEVICE_SERVER,
    GET_SERVER_CHAR,
    WRITE_SERVER_CHAR,
    CONFIG_SPP,
    ENTER_SPP_MODE,
    CONN_BLE_STEP_NUM

} CONN_BLE_STEP_TYPE;

typedef struct
{
    CONN_BLE_STEP_TYPE conn_ble_step;
    uint16_t wait_count;
    uint8_t conn_ble_flag;  //0 unconnect  1 connected

}cmp_ble_procc_data_stu;

typedef struct 
{
    uint16_t comm_flag;
    uint16_t count;

}cmp_ble_comm_status_stu;



typedef struct
{
    cmp_ble_procc_data_stu data;
    char conn_ble_name[28];
    uint8_t conn_ble_cmd_flag; 
    uint8_t conn_ble_count; 
    uint8_t conn_dev_count; 
    char ble_mac[17];
    char char_rx_id[1];
    char char_tx_id[1];

    cmp_ble_comm_status_stu comm;

}cmp_ble_param_stu;

typedef struct
{
    drv_serial_dev_stu* ble_serial;

    cmp_ble_param_stu procc;

    ble_conn_sta_hook_func ble_callback;
    
} cmp_ble_stu;

typedef struct
{
    void (*entr_conn_ble_func)(uint8_t period);
    void (*exec_conn_ble_func)(uint8_t period);
    void (*eixt_conn_ble_func)(uint8_t period);
    
} conn_ble_fsm_type;


typedef struct 
{
    void (*cmp_ble_work)(uint8_t period);
    void (*cmp_get_conn_ble_cmd)(uint8_t *msg);
    void (*ble_conn_sta_hook)(ble_conn_sta_hook_func ble_hook);

} cmp_ble_dev_stu;



#endif