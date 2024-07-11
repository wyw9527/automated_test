#ifndef _CMP_PROT_MT_H_
#define _CMP_PROT_MT_H_

// #include "cmp_store.h"
#include "cmp_prot_upper.h"
#include "cmp_ble.h"
#include "drv_clock.h"
#include "drv_serial.h"

typedef enum
{
    MT_PROT_UPPER_BLE_CONN_STA = 0x0,
    MT_PROT_UPPER_LOGIN_PROFILE,

}MT_PROT_UPPER_TYPE;

typedef enum
{
    PROT_MT_CMD_LOGIN = 0,
    PROT_MT_HEART_BEAT,
    PROT_MT_READ_CARD_RES,
    PROT_MT_CONFIG_CARD_RES,
    PROT_MT_QUERY_CARD_RES,
    // PROT_MT_AUTH_CARD_RES,
    PROT_MT_ALARM_STATUS,
    PROT_MT_OPERA_DATA_RES,
    PROT_MT_GUN_STATUS,
    PROT_MT_START_CHRG_NOTICE,
    PROT_MT_STOP_CHRG_NOTICE,
    PROT_MT_REMOTE_START_RES,
    PROT_MT_REMOTE_STOP_RES,
    PROT_MT_CHRG_DATA,
    PROT_MT_RESERVE_SET_RES,
    PROT_MT_RESERVE_CONTROL_RES,
    PROT_MT_RESERVE_QUERY_RES,
    PROT_MT_ORDER_UP,
    PROT_MT_LOAD_BALENCE_UP,
    PROT_MT_STATION_LB_CURR_SET_RES,
    PROT_MT_OFFPEAK_SET_RES,
    PROT_MT_REMOTE_TRIGGER_RES,
    PROT_MT_REMOTE_PARAM_SET_RES,
    PROT_MT_REMOTE_PARAM_QUERY_RES,
    PROT_MT_BILLING_MODEL_SET_RES,
    PROT_MT_BILLING_MODEL_QUERY_RES,
    // PROT_MT_DIAGNOSTIC_CONTROL_RES,
    // PROT_MT_DIAGNOSTIC_INFO_UP,
    PROT_MT_PBOX_SWITCH_PHASE_RES,
    PROT_MT_PARAM_CHANGE_NOTICE,
    // PROT_MT_OTA_DOWNLOAD_RES,
    // PROT_MT_OTA_DATA_QUERY,
    // PROT_MT_OTA_STATUS_QUERY,
    // PROT_MT_FILE_UPGRADE_RES,
    // PROT_MT_FILE_UPDATE_STATUS,
    PROT_MT_CMD_NUM
} CMP_PROT_MT_CMD_TYPE;

typedef struct
{
    uint16_t period;
    uint16_t tmout;
    uint16_t rx_cmd;
    uint16_t tx_cmd;
    void (*cmp_prot_mt_cmd_pase_func)(uint8_t *msg, uint16_t msg_len);
    void (*load_data)(uint16_t cmd);
} cmp_prot_mt_param_default_stu;

typedef struct 
{
    /* data */
    uint16_t period_tmr;
    uint16_t tmout_tmr;
    uint8_t tx_flag;
    // uint8_t tx_rpy_flag;
}cmp_prot_tx_ctrl_stu;

typedef struct
{
    char ble_name[28];
    uint8_t ble_sta;

}cmp_prot_ble_conn_conf;

typedef struct
{
    uint8_t login_res;  //0 accept  1 reject
    uint8_t interval[2];

}cmp_prot_login_conf;

typedef struct
{
    uint8_t send_cmd;   //0 send  1 not send
    uint8_t period;
    
}cmp_prot_heart_beat_conf;

typedef struct
{
    uint8_t res;  //0 accept  1 reject
    
}cmp_prot_alarm_status_conf;

typedef struct
{
    uint8_t res;  //0 accept  1 reject

}cmp_prot_gun_status_conf;

typedef struct
{
    uint8_t res;  //0 accept  1 reject

}cmp_prot_start_chrg_notice_conf;

typedef struct
{
    uint8_t res;  //0 accept  1 reject

}cmp_prot_stop_chrg_notice_conf;

typedef struct
{
    uint8_t res;  //0 accept  1 reject

}cmp_prot_order_up_conf;


typedef struct 
{
    uint8_t id[2];
    uint8_t order_id[8];
    
}cmp_prot_remote_start_save;

typedef struct 
{
    uint8_t id[2];

}cmp_prot_login_res_save;

typedef struct 
{
    uint8_t cmd_type;
    uint8_t res;
    uint8_t card_id[4];

}cmp_prot_read_card_res_save;

typedef struct 
{
    uint8_t cmd_type;
    uint8_t res;
    uint8_t card_id[4];

}cmp_prot_config_card_res_save;

typedef struct 
{
    uint8_t res;
    uint8_t card_num;
    uint8_t *card_id;

}cmp_prot_query_card_res_save;

// typedef struct 
// {
//     uint8_t res;
//     uint8_t card_num;
//     uint8_t *card_id;

// }cmp_prot_auth_card_res_save;

typedef struct 
{
    uint8_t id[2];
    uint8_t alarm_num;
    uint8_t *data;

}cmp_prot_alarm_status_save;

typedef struct 
{
    uint8_t data_num;
    uint8_t *data;

}cmp_prot_opera_data_res_save;

typedef struct 
{
    uint8_t id[2];
    uint8_t gun_id;
    uint8_t gun_sta;

}cmp_prot_gun_status_save;

typedef struct 
{
    uint8_t id[2];
    uint8_t gun_id;
    uint8_t order_id[8];
    uint8_t start_res;
    uint8_t rate[2];
    uint8_t start_type;
    uint8_t card_id[4];

}cmp_prot_start_chrg_notice_save;

typedef struct 
{
    uint8_t id[2];
    uint8_t gun_id;
    uint8_t order_id[8];
    uint8_t stop_reason;

}cmp_prot_stop_chrg_notice_save;

typedef struct 
{
    uint8_t res;
    uint8_t gun_id;
    uint8_t order_id[8];

}cmp_prot_remote_start_res_save;

typedef struct 
{
    uint8_t res;
    uint8_t gun_id;
    uint8_t order_id[8];

}cmp_prot_remote_stop_res_save;

typedef struct 
{
    uint8_t gun_id;
    uint8_t order_id[8];
    uint8_t rate[2];
    uint8_t chrg_Va[2];
    uint8_t chrg_Vb[2];
    uint8_t chrg_Vc[2];
    uint8_t chrg_Aa[2];
    uint8_t chrg_Ab[2];
    uint8_t chrg_Ac[2];
    uint8_t chrg_power[4];
    uint8_t chrg_energy[4];
    uint8_t chrg_time[2];
    uint8_t chrg_money[4];

}cmp_prot_chrg_data_save;

typedef struct 
{
    uint8_t gun_id;
    uint8_t reserve_id[8];
    uint8_t res;

}cmp_prot_reserve_set_res_save;

typedef struct 
{
    uint8_t gun_id;
    uint8_t reserve_id[8];
    uint8_t res;

}cmp_prot_reserve_control_res_save;

typedef struct 
{
    uint8_t reserve_num;
    uint8_t *data;

}cmp_prot_reserve_query_res_save;

typedef struct 
{
    uint8_t id[2];
    uint8_t gun_id;
    uint8_t order_id[8];
    uint8_t stop_reason;
    uint8_t start_type;
    uint8_t card_type;
    uint8_t card_id[4];
    uint8_t start_time[6];
    uint8_t stop_time[6];
    uint8_t chrg_time[2];
    uint8_t start_energy[4];
    uint8_t stop_energy[4];
    uint8_t chrg_energy[4];
    uint8_t billing_type;
    uint8_t chrg_server_money[4];
    uint8_t chrg_electric_money[4];
    uint8_t billing_model_period_num;
    uint8_t chrg_period_num;
    uint8_t *chrg_period_data;
    
}cmp_prot_order_up_save;

typedef struct 
{
    uint8_t meter_Va[2];
    uint8_t meter_Vb[2];
    uint8_t meter_Vc[2];
    uint8_t meter_Aa[4];
    uint8_t meter_Ab[4];
    uint8_t meter_Ac[4];
    uint8_t meter_power[4];
    uint8_t meter_energy[4];

}cmp_prot_load_balence_up_save;

typedef struct 
{
    uint8_t res;
    uint16_t current_curr;

}cmp_prot_station_lb_curr_set_res_save;

typedef struct 
{
    uint8_t gun_id;
    uint8_t res;

}cmp_prot_offpeak_set_res_save;

typedef struct 
{
    uint8_t trigger_type;
    uint8_t res;

}cmp_prot_remote_trigger_res_save;

typedef struct 
{
    uint8_t param_num;
    uint8_t* data;

}cmp_prot_remote_param_set_res_save;

typedef struct 
{
    uint8_t param_num;
    uint8_t* data;

}cmp_prot_remote_param_query_res_save;

typedef struct 
{
    uint8_t rate_num;
    uint8_t res;

}cmp_prot_billing_model_set_res_save;

typedef struct 
{
    uint8_t res;
    uint8_t rate_num;
    uint8_t *data;

}cmp_prot_billing_model_query_res_save;

typedef struct 
{
    uint8_t res;

}cmp_prot_pbox_switch_phase_res_save;

typedef struct 
{
    uint8_t id[2];
    uint8_t change_source;
    uint8_t param_num;
    uint8_t *data;

}cmp_prot_param_change_notice_save;

typedef struct 
{
    uint8_t res;

}cmp_prot_ota_download_res_save;

typedef struct 
{
    uint8_t id[2];
    uint16_t pack_id;

}cmp_prot_ota_data_query_save;

typedef struct 
{
    uint8_t id[2];
    uint8_t res;

}cmp_prot_ota_status_query_save;

typedef struct
{
    uint8_t store_sta;
    
    cmp_prot_tx_ctrl_stu tx_ctrl[PROT_MT_CMD_NUM];

    cmp_prot_ble_conn_conf ble_conn_conf;

    cmp_prot_login_conf login_conf;

    cmp_prot_heart_beat_conf heart_beat_conf;

    cmp_prot_alarm_status_conf alarm_status_conf;

    cmp_prot_gun_status_conf gun_status_conf;

    cmp_prot_start_chrg_notice_conf start_chrg_notice_conf;

    cmp_prot_stop_chrg_notice_conf stop_chrg_notice_conf;

    cmp_prot_order_up_conf order_up_conf;

    cmp_prot_remote_start_save remote_start;

    cmp_prot_login_res_save login_res;

    cmp_prot_read_card_res_save read_card_res; 

    cmp_prot_config_card_res_save config_card_res; 

    cmp_prot_query_card_res_save query_card_res; 

    // cmp_prot_auth_card_res_save auth_card_res; 

    cmp_prot_alarm_status_save alarm_status; 

    cmp_prot_opera_data_res_save opera_data_res;

    cmp_prot_gun_status_save gun_status;

    cmp_prot_start_chrg_notice_save start_chrg_notice;

    cmp_prot_stop_chrg_notice_save stop_chrg_notice;

    cmp_prot_remote_start_res_save remote_start_res;

    cmp_prot_remote_stop_res_save remote_stop_res;

    cmp_prot_chrg_data_save chrg_data;

    cmp_prot_reserve_set_res_save reserve_set_res;

    cmp_prot_reserve_control_res_save reserve_control_res;

    cmp_prot_reserve_query_res_save reserve_query_res;

    cmp_prot_order_up_save order_up;

    cmp_prot_load_balence_up_save load_balence_up;

    cmp_prot_station_lb_curr_set_res_save station_lb_curr_set_res;

    cmp_prot_offpeak_set_res_save offpeak_set_res;

    cmp_prot_remote_trigger_res_save remote_trigger_res;

    cmp_prot_remote_param_set_res_save remote_param_set_res;

    cmp_prot_remote_param_query_res_save remote_param_query_res;

    cmp_prot_billing_model_set_res_save billing_model_set_res;

    cmp_prot_billing_model_query_res_save billing_model_query_res;

    cmp_prot_pbox_switch_phase_res_save pbox_switch_phase_res;

    cmp_prot_param_change_notice_save param_change_notice;

    // cmp_prot_ota_download_res_save ota_download_res;

    // cmp_prot_ota_data_query_save ota_data_query;

    // cmp_prot_ota_status_query_save ota_status_query;

} cmp_prot_mt_src_procc_stu;

typedef struct
{
    const char *sn;
} cmp_prot_mt_src_param_stu;

typedef struct
{
    // cmp_store_dev_stu *store;
    
    drv_serial_dev_stu *serial;
    
    drv_clock_dev_stu *clock;

    cmp_prot_mt_src_procc_stu procc;

    cmp_prot_mt_src_param_stu param;

} cmp_prot_mt_stu;


typedef struct
{
    void (*cmp_prot21_mt_save_conf)(MT_PROT_UPPER_TYPE type, uint8_t *msg);

    void (*cmp_prot21_mt_send_msg)(uint16_t cmd, uint8_t *msg, uint16_t msg_len);

    uint8_t (*cmp_prot21_mt_set_msg)(uint8_t *msg, uint16_t len);

    void (*cmp_prot21_mt_work)(uint8_t period);

    void (*cmp_prot21_mt_remote_start)(void);

    void (*cmp_prot21_mt_remote_stop)(void);

} cmp_prot_mt_dev_stu;

#endif