#ifndef _CMP_PROT_UPPER_H_
#define _CMP_PROT_UPPER_H_

#include "cmp.h"
#include "drv_clock.h"
#include "drv_serial.h"
#include "cmp_w5500.h"
#include "cmp_ble.h"
#include "RyJsonAPI.h"

typedef struct cmp_prot_upper_stu_s cmp_prot_upper_stu;

typedef enum
{
    CMD_CHECK_TIME = 0x0,
    CMD_CONNECT_EVCC,
    CMD_LOGIN_PROFILE,
    CMD_CONTROL_POWER_SWITCH,
    CMD_CONTROL_POWER_VALTAGE,
    CMD_CONTROL_LOAD_SWITCH,
    CMD_REMOTE_CHARGE,
    CMD_CONTROL_LOAD_CURRENT,
    CMD_UNKNOWN,

}UPPER_CMD_TYPE;

typedef enum
{
    PROT21_MT_HOOK_CMD_LOGIN_PROFILE,
    PROT21_MT_HOOK_CMD_CONNECT_EVCC,
    PROT21_MT_HOOK_CMD_CONTROL_POWER_SWITCH,
    PROT21_MT_HOOK_CMD_CONTROL_POWER_VALTAGE,
    PROT21_MT_HOOK_CMD_CONTROL_LOAD_SWITCH,
    PROT21_MT_HOOK_CMD_REMOTE_CHARGE,
    PROT21_MT_HOOK_CMD_CONTROL_LOAD_CURRENT,
    
} CMP_PROT_UPPER_HOOK_CMD_TYPE;


typedef void (*upper_set_cmd_hook_func)(CMP_PROT_UPPER_HOOK_CMD_TYPE cmd, void *data);


typedef enum
{
    UPPER_PROT_BLE_CONN_STA = 0x0,
    UPPER_PROT_W5500_CONN_STA,

}UPPER_PROT_SAVE_TYPE;



typedef struct
{
    uint8_t ble_conn_sta_flag;

} cmp_prot_upper_tx_msg_conn_ble_stu;

typedef struct
{
    cmp_prot_upper_tx_msg_conn_ble_stu conn_ble;

}cmp_prot_upper_tx_msg_stu;



typedef struct
{
    uint8_t login_res;  //0 accept  1 reject
    uint8_t login_interval;

} cmp_prot_upper_to_server_msg_login_stu;

typedef struct
{
    uint8_t ble_name[28];
    uint8_t conn_flag;  //0 disconnect  1 connect

} cmp_prot_upper_to_server_msg_conn_ble_stu;

typedef struct
{
    uint8_t command;  //0 start  1 stop

} cmp_prot_upper_to_server_msg_power_switch_stu;

typedef struct
{
    uint32_t input_Va; 
    uint32_t input_Vb; 
    uint32_t input_Vc; 

} cmp_prot_upper_to_server_msg_power_val_stu;

typedef struct
{
    uint8_t command;  //0 start  1 stop

} cmp_prot_upper_to_server_msg_remote_chrg_stu;

typedef struct
{
    uint16_t Ia; 
    uint16_t Ib; 
    uint16_t Ic; 

} cmp_prot_upper_to_server_msg_load_curr_stu;



typedef struct
{
    uint8_t w5500_conn_status;  //0: disconnect(AT), 1: connect(SPP)
}cmp_prot_w5500_param_stu;


typedef struct
{
    // cmp_store_dev_stu *store;
    
    drv_serial_dev_stu *serial;

    drv_clock_dev_stu *clock;

    cmp_w5500_dev_stu *w5500;

    ryjson_api_dev_stu *ryjson;

    //cmp_prot_upper_stu *prot;

    upper_set_cmd_hook_func cmd_callback;

    cmp_prot_upper_tx_msg_stu tx_msg;

    cmp_prot_w5500_param_stu w5500_param;

} cmp_prot_upper_src_stu;


// struct cmp_prot_upper_stu_s
// {
//     upper_set_cmd_hook_func cmd_callback;
// };

typedef struct
{
    void (*cmp_prot_upper_save_param)(UPPER_PROT_SAVE_TYPE type, uint8_t *msg);

    void (*cmp_prot_upper_set_msg)(uint8_t *msg, uint16_t datalen);

    // void (*cmp_prot_upper_recieve_msg)(uint8_t period);

    void (*cmp_prot_upper_work)(uint8_t period);

    void (*upper_set_cmd_hook)(upper_set_cmd_hook_func cmd_hook);

} cmp_prot_upper_dev_stu;





#endif