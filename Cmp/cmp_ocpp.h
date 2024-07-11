#ifndef _CMP_OCPP_H_
#define _CMP_OCPP_H_

#include "cmp_store.h"
#include "cmp_measure.h"

typedef void (*occp_transmit_msg_func_type)(uint8_t* data,uint16_t len);

typedef enum
{
    OCPP_CMD_UNLOCK,
    OCPP_CMD_REMOTE_START,
    OCPP_CMD_REMOTE_STOP,
    OCPP_CMD_CHANGE_AVAI,
    OCPP_CMD_RESET,
    OCPP_CMD_CHRG_LIMIT,
    OCPP_CMD_CHANGE_CONF,
    OCPP_CMD_CARD_AUTH,
}CMP_PROT_OCPP_CMD_TYPE;

typedef void (*ocpp_cmd_msg_func_type)(CMP_PROT_OCPP_CMD_TYPE cmd,uint32_t data);

typedef enum
{
    OCPP_MSG_INIT=0,
    OCPP_MSG_NET,
    OCPP_MSG_SERV,
    OCPP_MSG_HB,
    OCPP_MSG_RESV,
    OCPP_MSG_RESV_CAL,
    OCPP_MSG_PLUG_STA,
    OCPP_MSG_NUM
}CMP_PROT_OCPP_MSG_TYPE;

typedef struct
{
    uint8_t ver_len;
    uint8_t verion[16];
    uint8_t sim_len;
    uint8_t sim[24];
}cmp_prot_ocpp_rx_init_stu;

typedef struct
{
    uint8_t plug_stat;
    uint8_t error_code;
}cmp_prot_ocpp_tx_status_stu;

typedef struct
{
    uint8_t ocpp_sta;/* 0 wait init  1 net config  2 serv conf 3 spp */
    uint8_t comm_sta;/* 0 fail 1 narmal */   
    uint8_t csms_sta;/* 0 not conn 1 conned */
    uint8_t tcp_sta;/* 0 tcp serv not conn 1 conned */
    uint16_t ocpp_count;
    uint16_t hb_count;
    uint8_t resv_result;
    uint8_t cmd_result;
    uint16_t plug_sta_count;
    
    occp_transmit_msg_func_type ocpp_rx_transparent_func;
    ocpp_cmd_msg_func_type cmd_hook_func;
    
    cmp_prot_ocpp_tx_status_stu tx_status[BSP_POLE_PLUG_NUM];
}cmp_prot_ocpp_procc_stu;

typedef struct
{
    uint8_t net_type;/* 0 eth  1 wifi  2 lte */
    uint8_t wifi_secuity_type;
}cmp_prot_ocpp_param_stu;

typedef struct
{
    char ppm_ver[16];
    char ppm_sim_no[24];
}cmp_prot_ocpp_auth_info_stu;

typedef struct 
{
    uint32_t resv_time;
    uint8_t resv_flag;
}cmp_prot_ocpp_resv_stu;

typedef struct
{
    cmp_prot_ocpp_auth_info_stu init;
    
    cmp_prot_ocpp_resv_stu resv[BSP_POLE_PLUG_NUM];
}cmp_prot_ocpp_info_stu;

typedef struct
{
    uint8_t  tx_flag;
}cmp_prot_ocpp_tx_ctrl_stu;

typedef struct
{
    const cmp_store_dev_stu* store;
    drv_serial_dev_stu* serial;
    drv_dido_do_dev_stu* reset_io;
}cmp_prot_ocpp_pub_stu;

typedef struct
{
    cmp_prot_ocpp_pub_stu pub_dev;
    
    cmp_prot_ocpp_param_stu param;
    
    cmp_prot_ocpp_procc_stu procc;
    
    cmp_prot_ocpp_info_stu ocpp_info;
    
    cmp_prot_ocpp_tx_ctrl_stu tx_ctrl[OCPP_MSG_NUM];
}cmp_prot_ocpp_stu;


typedef struct
{
    void (*ocpp_set_serial)(drv_serial_dev_stu* serial);
    
    uint8_t (*ocpp_get_uasable_state)(void);
    
    void (*ocpp_cyc_work)(uint8_t period);
    
    void (*ocpp_set_cmd_msg_hook)(ocpp_cmd_msg_func_type cmd_func);
    
    void (*ocpp_set_cmd_result)(CMP_PROT_OCPP_CMD_TYPE type,uint8_t result);
    
    void (*ocpp_set_auth_info)(uint8_t plug,uint32_t uid,uint8_t type);/* 充电卡鉴权 */
    
    void (*ocpp_set_chrg_notify)(uint8_t plug,uint8_t type);
    
    void (*ocpp_set_transparent_data)(uint8_t* data,uint8_t len);/* 麦田协议数据透传 */
    
    void (*ocpp_set_transparent_hook)(occp_transmit_msg_func_type transparent_func);
    
}cmp_prot_ocpp_dev_stu;



























#endif