#include "cmp_prot_mt.h"
#include "SEGGER_RTT_Conf.h"
#include "SEGGER_RTT.h"
#include <stdio.h>

static cmp_prot_mt_stu mt_conf;

/***************************************************************************************
 * @Function    : cmp_prot_mt_genr_tx_id()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/16
 ****************************************************************************************/
static void cmp_prot_mt_genr_tx_id(uint8_t *id)
{
    static uint16_t msg_id = 0;

    if (id == RT_NULL)
        return;

    if (++msg_id == 0)
    {
        msg_id++;
    }

    memcpy(id, (uint8_t *)&msg_id, 2);
}

/***************************************************************************************
 * @Function    : cmp_prot_mt_order_num()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/16
 ****************************************************************************************/
static void cmp_prot_mt_order_num(uint8_t *order_num)
{
    static uint16_t order_idx = 1;

    drv_bcd_datetime_stu curr_time;
    mt_conf.clock->drv_get_bcd_datetime(&curr_time);
    order_num[0] = curr_time.year;
    order_num[1] = curr_time.month;
    order_num[2] = curr_time.day;
    order_num[3] = curr_time.hour;
    order_num[4] = curr_time.min;
    order_num[5] = curr_time.sec;
    uint8_t min_sec_sum = curr_time.min + curr_time.sec;
    order_num[6] = min_sec_sum / 16; // 十位
    order_num[7] = min_sec_sum % 16; // 个位
}

static void cmp_prot_mt_save_ble_conn(uint8_t *msg)
{
    cmp_ble_to_server_msg_ble_conn_sta_stu *upper_ble_conn = (cmp_ble_to_server_msg_ble_conn_sta_stu *)msg;

    memcpy(&mt_conf.procc.ble_conn_conf, upper_ble_conn, sizeof(cmp_ble_to_server_msg_ble_conn_sta_stu));

}

static void cmp_prot_mt_save_login_profile(uint8_t *msg)
{
    cmp_prot_upper_to_server_msg_login_stu *upper_login_res = (cmp_prot_upper_to_server_msg_login_stu *)msg;

    memcpy(&mt_conf.procc.login_conf, upper_login_res, sizeof(cmp_prot_upper_to_server_msg_login_stu));
}



/***************************************************************************************
 * @Function    : cmp_prot21_mt_save_conf()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : SSY            Version   : V3.0.0                Date:2024/6/27
 ****************************************************************************************/
static void cmp_prot21_mt_save_conf(MT_PROT_UPPER_TYPE type, uint8_t *msg)
{
    switch (type)
    {
    case MT_PROT_UPPER_BLE_CONN_STA: cmp_prot_mt_save_ble_conn(msg);
        break;
    case MT_PROT_UPPER_LOGIN_PROFILE: cmp_prot_mt_save_login_profile(msg);
        break;

    default:
        break;
    }

}

/***************************************************************************************
 * @Function    : cmp_prot21_mt_send_msg()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : SSY            Version   : V3.0.0                Date:2024/6/27
 ****************************************************************************************/
static void cmp_prot21_mt_send_msg(uint16_t cmd, uint8_t *msg, uint16_t data_len)
{
    uint16_t pos = 0;
    static uint8_t buff[512];

    memset(buff, 0, sizeof(buff));
    buff[pos++] = 'W';
    buff[pos++] = 'M';  //帧头
    buff[pos++] = 0x01;  //协议类型
    buff[pos++] = 0x02;
    buff[pos++] = 0x10;  //协议版本
    memcpy(&buff[pos], mt_conf.procc.ble_conn_conf.ble_name, 28);  //SN
    pos += 28;
    buff[pos++] = 0x01;  //加密标志
    memcpy(&buff[pos], &cmd, 2);  //cmd
    pos += 2;
    memcpy(&buff[pos], &data_len, 2);  //datalen
    pos += 2;
    memcpy(&buff[pos], msg, data_len);  //数据域
    pos += data_len;
    buff[pos] = xor_check(buff, pos);  //异或校验
    pos++;

    mt_conf.serial->dev_send_msg(buff, pos);
}

static void cmp_prot21_mt_remote_start()
{
    uint16_t pos = 0;
    static uint8_t buff[512];
    uint8_t order[8];
    uint8_t id[2];
    uint8_t stop_id[6] = {0x00,0x00,0x00,0x00,0x00,0x00};
    uint8_t stop_condition[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};  
    cmp_prot_order_up_save order_info;
    cmp_prot_remote_start_save start_info;

    memset(buff, 0, sizeof(buff));
    cmp_prot_mt_genr_tx_id(id);
    memcpy(&buff[pos], id, sizeof(id));  
    pos += 2;
    buff[pos++] = 0x00; 
    cmp_prot_mt_order_num(order);
    memcpy(&buff[pos], order, sizeof(order));  
    pos += 8;
    memcpy(&buff[pos], stop_id, sizeof(stop_id));  
    pos += 6;
    memcpy(&buff[pos], stop_condition, sizeof(stop_condition));
    pos += 6;
    buff[pos++] = 0x00; 

    memcpy(start_info.id, id, 2);
    memcpy(start_info.order_id, order, 8);
    memcpy(order_info.order_id, order, 8);
    cmp_prot21_mt_send_msg(0x8013, buff, pos);
}

static void cmp_prot21_mt_remote_stop()
{
    uint16_t pos = 0;
    static uint8_t buff[512];
    cmp_prot_order_up_save order_info;
    cmp_prot_remote_start_save start_info;

    memset(buff, 0, sizeof(buff));
    memcpy(&buff[pos], start_info.id, 2);  
    pos += 2;
    buff[pos++] = 0x00; 
    memcpy(&buff[pos], order_info.order_id, 8);  
    pos += 8;
  
    cmp_prot21_mt_send_msg(0x8014, buff, pos);
}

/*
    接收函数
*/
static void cmd_pase_login_res(uint8_t *data, uint16_t len)
{
    memcpy(mt_conf.procc.login_res.id,data,2);
    mt_conf.procc.login_conf.login_res = 0x01;
    mt_conf.procc.login_conf.interval[0] = 0x0A;
    mt_conf.procc.login_conf.interval[1] = 0x00;

    mt_conf.procc.tx_ctrl[PROT_MT_CMD_LOGIN].tx_flag = 1;
    mt_conf.procc.tx_ctrl[PROT_MT_CMD_LOGIN].period_tmr = 900;
}

static void cmd_pase_evcc_heart_beat(uint8_t *data, uint16_t len)
{
    mt_conf.procc.tx_ctrl[PROT_MT_HEART_BEAT].tx_flag = 1;
    mt_conf.procc.tx_ctrl[PROT_MT_HEART_BEAT].period_tmr = 900;
}

static void cmd_pase_read_card_res(uint8_t *data, uint16_t len)
{
    mt_conf.procc.read_card_res.cmd_type = data[2];
    mt_conf.procc.read_card_res.res = data[3];
    memcpy(mt_conf.procc.read_card_res.card_id, &data[4], 4);
}

static void cmd_pase_config_card_res(uint8_t *data, uint16_t len)
{
    mt_conf.procc.config_card_res.cmd_type = data[2];
    mt_conf.procc.config_card_res.res = data[3];
    memcpy(mt_conf.procc.config_card_res.card_id, &data[4], 4);
}

static void cmd_pase_query_card_res(uint8_t *data, uint16_t len)
{
    mt_conf.procc.query_card_res.res = data[2];
    mt_conf.procc.query_card_res.card_num = data[3];
    if(len == (4+4*mt_conf.procc.query_card_res.card_num))
    {
        for(int i = 0; i < mt_conf.procc.query_card_res.card_num; i++)
        {
            memcpy(mt_conf.procc.query_card_res.card_id+i*4, &data[4+i*4], 4);
        }
    }

}

static void cmd_pase_alarm_status(uint8_t *data, uint16_t len)
{
    memcpy(mt_conf.procc.alarm_status.id, data, 2);
    mt_conf.procc.alarm_status.alarm_num = data[2];
    if(len == 3+4*mt_conf.procc.alarm_status.alarm_num)
    {
        for(int i = 0; i < mt_conf.procc.alarm_status.alarm_num; i++)
        {
            memcpy(mt_conf.procc.alarm_status.data+i*4, &data[3+i*4], 4);
        }    
    }

    mt_conf.procc.tx_ctrl[PROT_MT_ALARM_STATUS].tx_flag = 1;
    mt_conf.procc.tx_ctrl[PROT_MT_ALARM_STATUS].period_tmr = 900;
}

static void cmd_pase_opera_data_res(uint8_t *data, uint16_t len)
{
    memcpy(&mt_conf.procc.opera_data_res, &data[2], len-2);
}

static void cmd_pase_gun_status(uint8_t *data, uint16_t len)
{
    memcpy(&mt_conf.procc.gun_status, data, len);

    mt_conf.procc.tx_ctrl[PROT_MT_GUN_STATUS].tx_flag = 1;
    mt_conf.procc.tx_ctrl[PROT_MT_GUN_STATUS].period_tmr = 900;
}

static void cmd_pase_start_chrg_notice(uint8_t *data, uint16_t len)
{
    memcpy(&mt_conf.procc.start_chrg_notice, &data[2], len-2);

    mt_conf.procc.tx_ctrl[PROT_MT_START_CHRG_NOTICE].tx_flag = 1;
    mt_conf.procc.tx_ctrl[PROT_MT_START_CHRG_NOTICE].period_tmr = 900;
}

static void cmd_pase_stop_chrg_notice(uint8_t *data, uint16_t len)
{
    memcpy(&mt_conf.procc.stop_chrg_notice, &data[2], len-2);

    mt_conf.procc.tx_ctrl[PROT_MT_STOP_CHRG_NOTICE].tx_flag = 1;
    mt_conf.procc.tx_ctrl[PROT_MT_STOP_CHRG_NOTICE].period_tmr = 900;
}

static void cmd_pase_remote_start_res(uint8_t *data, uint16_t len)
{
    memcpy(&mt_conf.procc.remote_start_res, &data[2], len-2);
}

static void cmd_pase_remote_stop_res(uint8_t *data, uint16_t len)
{
    memcpy(&mt_conf.procc.remote_stop_res, &data[2], len-2);
}

static void cmd_pase_chrg_data(uint8_t *data, uint16_t len)
{
    memcpy(&mt_conf.procc.chrg_data, data, len);
}

static void cmd_pase_reserve_set_res(uint8_t *data, uint16_t len)
{
    memcpy(&mt_conf.procc.reserve_set_res, &data[2], len-2);
}

static void cmd_pase_reserve_control_res(uint8_t *data, uint16_t len)
{
    memcpy(&mt_conf.procc.reserve_control_res, &data[2], len-2);
}

static void cmd_pase_reserve_query_res(uint8_t *data, uint16_t len)
{
    memcpy(&mt_conf.procc.reserve_query_res, &data[2], len-2);
        
    mt_conf.procc.reserve_query_res.reserve_num = data[2];
    if(len == 3+15*mt_conf.procc.reserve_query_res.reserve_num)
    {
        for(int i = 0; i < mt_conf.procc.reserve_query_res.reserve_num; i++)
        {
            memcpy(mt_conf.procc.reserve_query_res.data+i*15, &data[3+i*15], 15);
        }    
    }
}

static void cmd_pase_order_up(uint8_t *data, uint16_t len)
{
    memcpy(&mt_conf.procc.order_up, data, 58);
    memcpy(&mt_conf.procc.order_up.chrg_period_data, &data[58], mt_conf.procc.order_up.chrg_period_num*3);

    mt_conf.procc.tx_ctrl[PROT_MT_ORDER_UP].tx_flag = 1;
    mt_conf.procc.tx_ctrl[PROT_MT_ORDER_UP].period_tmr = 900;
}

static void cmd_pase_load_balence_up(uint8_t *data, uint16_t len)
{
    memcpy(&mt_conf.procc.load_balence_up, data, len);

    mt_conf.procc.tx_ctrl[PROT_MT_LOAD_BALENCE_UP].tx_flag = 1;
    mt_conf.procc.tx_ctrl[PROT_MT_LOAD_BALENCE_UP].period_tmr = 900;
}

static void cmd_pase_station_lb_curr_set_res(uint8_t *data, uint16_t len)
{
    memcpy(&mt_conf.procc.station_lb_curr_set_res, &data[2], len-2);
}

static void cmd_pase_offpeak_set_res(uint8_t *data, uint16_t len)
{
    memcpy(&mt_conf.procc.offpeak_set_res, &data[2], len-2);
}

static void cmd_pase_remote_trigger_res(uint8_t *data, uint16_t len)
{
    memcpy(&mt_conf.procc.remote_trigger_res, &data[2], len-2);
}

static void cmd_pase_remote_param_set_res(uint8_t *data, uint16_t len)
{
    mt_conf.procc.remote_param_set_res.param_num = data[2];
    memcpy(mt_conf.procc.remote_param_set_res.data, &data[3], len-3);
}

static void cmd_pase_remote_param_query_res(uint8_t *data, uint16_t len)
{
    mt_conf.procc.remote_param_query_res.param_num = data[2];
    memcpy(mt_conf.procc.remote_param_query_res.data, &data[3], len-3);
}

static void cmd_pase_billing_model_set_res(uint8_t *data, uint16_t len)
{
    memcpy(&mt_conf.procc.billing_model_set_res, &data[2], len-2);
}

static void cmd_pase_billing_model_query_res(uint8_t *data, uint16_t len)
{
    memcpy(&mt_conf.procc.billing_model_query_res, &data[2], 3);
    memcpy(mt_conf.procc.billing_model_query_res.data, &data[5], len-5);
}

static void cmd_pase_pbox_switch_phase_res(uint8_t *data, uint16_t len)
{
    memcpy(&mt_conf.procc.pbox_switch_phase_res, &data[2], len-2);
}

static void cmd_pase_param_change_notice(uint8_t *data, uint16_t len)
{
    memcpy(&mt_conf.procc.param_change_notice, data, 4);
    memcpy(mt_conf.procc.param_change_notice.data, &data[4], len-4);

    mt_conf.procc.tx_ctrl[PROT_MT_PARAM_CHANGE_NOTICE].tx_flag = 1;
    mt_conf.procc.tx_ctrl[PROT_MT_PARAM_CHANGE_NOTICE].period_tmr = 900;
}

// static void cmd_pase_ota_download_res(uint8_t *data, uint16_t len)
// {
//     memcpy(&mt_conf.procc.ota_download_res, data, len);
// }

// static void cmd_pase_ota_data_query(uint8_t *data, uint16_t len)
// {
//     uint8_t tmp[2];
//     memcpy(&mt_conf.procc.ota_data_query, data, 2);
//     memcpy(tmp, &data[2], len-2);
//     mt_conf.procc.ota_data_query.pack_id = (uint16_t)tmp[0] | ((uint16_t)tmp[1] << 8);

//     mt_conf.procc.tx_ctrl[PROT_MT_OTA_DATA_QUERY].tx_flag = 1;
//     mt_conf.procc.tx_ctrl[PROT_MT_OTA_DATA_QUERY].period_tmr = 900;
// }

// static void cmd_pase_ota_status_query(uint8_t *data, uint16_t len)
// {
//     memcpy(&mt_conf.procc.ota_status_query, data, len);

//     mt_conf.procc.tx_ctrl[PROT_MT_OTA_STATUS_QUERY].tx_flag = 1;
//     mt_conf.procc.tx_ctrl[PROT_MT_OTA_STATUS_QUERY].period_tmr = 900;
// }






/*
    发送函数
*/
static void load_data_login_res(uint16_t cmd)
{
    uint8_t data[11];
    drv_clock_calendar_stu curr_time;
    mt_conf.clock->drv_clock_get(&curr_time);

    memcpy(data, mt_conf.procc.login_res.id, 2);
    data[2] = mt_conf.procc.login_conf.login_res;
    data[3] = curr_time.year;
    data[4] = curr_time.month;
    data[5] = curr_time.day;
    data[6] = curr_time.hour;
    data[7] = curr_time.min;
    data[8] = curr_time.second;
    memcpy(&data[9], mt_conf.procc.login_conf.interval, 2);

    cmp_prot21_mt_send_msg(cmd, data, sizeof(data));
}


static void load_data_heart_beat(uint16_t cmd)
{
    uint8_t data[6];
    drv_clock_calendar_stu curr_time;
    mt_conf.clock->drv_clock_get(&curr_time);

    data[0] = curr_time.year;
    data[1] = curr_time.month;
    data[2] = curr_time.day;
    data[3] = curr_time.hour;
    data[4] = curr_time.min;
    data[5] = curr_time.second;

    cmp_prot21_mt_send_msg(cmd, data, sizeof(data));
}

static void load_data_alarm_status(uint16_t cmd)
{
    uint8_t data[3];

    memcpy(data, mt_conf.procc.alarm_status.id, 2);
    data[2] = mt_conf.procc.alarm_status_conf.res;

    cmp_prot21_mt_send_msg(cmd, data, sizeof(data));
}

static void load_data_gun_status(uint16_t cmd)
{
    uint8_t data[4];

    memcpy(data, mt_conf.procc.gun_status.id, 2);
    data[2] = mt_conf.procc.gun_status.gun_id;
    data[3] = mt_conf.procc.gun_status_conf.res;

    cmp_prot21_mt_send_msg(cmd, data, sizeof(data));
}

static void load_data_start_chrg_notice(uint16_t cmd)
{
    uint8_t data[4];

    memcpy(data, mt_conf.procc.start_chrg_notice.id, 2);
    data[2] = mt_conf.procc.start_chrg_notice_conf.res;
    data[3] = mt_conf.procc.start_chrg_notice.gun_id;
    memcpy(&data[4], mt_conf.procc.start_chrg_notice.order_id, 8);

    cmp_prot21_mt_send_msg(cmd, data, sizeof(data));
}

static void load_data_stop_chrg_notice(uint16_t cmd)
{
    uint8_t data[4];

    memcpy(data, mt_conf.procc.stop_chrg_notice.id, 2);
    data[2] = mt_conf.procc.stop_chrg_notice_conf.res;
    data[3] = mt_conf.procc.stop_chrg_notice.gun_id;
    memcpy(&data[4], mt_conf.procc.stop_chrg_notice.order_id, 8);

    cmp_prot21_mt_send_msg(cmd, data, sizeof(data));
}

static void load_data_order_up(uint16_t cmd)
{
    uint8_t data[12];

    memcpy(data, mt_conf.procc.order_up.id, 2);
    data[2] = mt_conf.procc.order_up.gun_id;
    memcpy(&data[3], mt_conf.procc.order_up.order_id, 8);
    data[11] = mt_conf.procc.order_up_conf.res;

    cmp_prot21_mt_send_msg(cmd, data, sizeof(data));
}

static void load_data_param_change_notice(uint16_t cmd)
{
    uint8_t data[2];
    memcpy(data, mt_conf.procc.param_change_notice.id, 2);

    cmp_prot21_mt_send_msg(cmd, data, sizeof(data));
}

// static void load_data_ota_data_query(uint16_t cmd)
// {


// }

// static void load_data_ota_status_query(uint16_t cmd)
// {


// }


/*
 *  协议固定架构
 *
 */
static cmp_prot_mt_param_default_stu mt_param_default[PROT_MT_CMD_NUM] =
{
    {1000, 1000, 0x0001, 0x8001, cmd_pase_login_res, load_data_login_res}, 
    {1000, 1000, 0x0002, 0x8002, cmd_pase_evcc_heart_beat, load_data_heart_beat}, 
    {1000, 1000, 0x0005, 0, cmd_pase_read_card_res, RT_NULL}, 
    {1000, 1000, 0x0006, 0, cmd_pase_config_card_res, RT_NULL}, 
    {1000, 1000, 0x0007, 0, cmd_pase_query_card_res, RT_NULL}, 
    // {1000, 1000, 0x0008, 0, cmd_pase_auth_card_res, RT_NULL}, 
    {1000, 1000, 0x000A, 0x800A, cmd_pase_alarm_status, load_data_alarm_status}, 
    {1000, 1000, 0x000B, 0, cmd_pase_opera_data_res, RT_NULL}, 
    {1000, 1000, 0x0010, 0x8010, cmd_pase_gun_status, load_data_gun_status}, 
    {1000, 1000, 0x0011, 0x8011, cmd_pase_start_chrg_notice, load_data_start_chrg_notice}, 
    {1000, 1000, 0x0012, 0x8012, cmd_pase_stop_chrg_notice, load_data_stop_chrg_notice}, 
    {1000, 1000, 0x0013, 0, cmd_pase_remote_start_res, RT_NULL},
    {1000, 1000, 0x0014, 0, cmd_pase_remote_stop_res, RT_NULL}, 
    {1000, 1000, 0x0015, 0, cmd_pase_chrg_data, RT_NULL},
    {1000, 1000, 0x0016, 0, cmd_pase_reserve_set_res, RT_NULL},
    {1000, 1000, 0x0017, 0, cmd_pase_reserve_control_res, RT_NULL},
    {1000, 1000, 0x0018, 0, cmd_pase_reserve_query_res, RT_NULL},
    {1000, 1000, 0x0019, 0x8019, cmd_pase_order_up, load_data_order_up},
    {1000, 1000, 0x001A, 0, cmd_pase_load_balence_up, RT_NULL},
    {1000, 1000, 0x001B, 0, cmd_pase_station_lb_curr_set_res, RT_NULL}, 
    {1000, 1000, 0x001C, 0, cmd_pase_offpeak_set_res, RT_NULL},
    {1000, 1000, 0x0020, 0, cmd_pase_remote_trigger_res, RT_NULL}, 
    {1000, 1000, 0x0021, 0, cmd_pase_remote_param_set_res, RT_NULL},
    {1000, 1000, 0x0022, 0, cmd_pase_remote_param_query_res, RT_NULL},
    {1000, 1000, 0x0023, 0, cmd_pase_billing_model_set_res, RT_NULL}, 
    {1000, 1000, 0x0024, 0, cmd_pase_billing_model_query_res, RT_NULL},
    // {1000, 1000, 0x0025, 0, cmd_pase_diagnostic_control_res, RT_NULL}, 
    // {1000, 1000, 0x0026, 0, cmd_pase_diagnostic_info_up, RT_NULL}, 
    {1000, 1000, 0x0027, 0, cmd_pase_pbox_switch_phase_res, RT_NULL},
    {1000, 1000, 0x0028, 0x8028, cmd_pase_param_change_notice, load_data_param_change_notice}, 
    // {1000, 1000, 0x0030, 0, cmd_pase_ota_download_res, RT_NULL}, 
    // {1000, 1000, 0x0031, 0x8031, cmd_pase_ota_data_query, load_data_ota_data_query}, 
    // {1000, 1000, 0x0032, 0x8032, cmd_pase_ota_status_query, load_data_ota_status_query}, 
    // {1000, 1000, 0x0033, 0, cmd_pase_file_upgrade_res, RT_NULL},
    // {1000, 1000, 0x0034, 0, cmd_pase_file_update_status, RT_NULL}, 

};



static void cmp_prot_mt_tx_ctrl_work(cmp_prot_mt_src_procc_stu *procc,uint8_t period)
{
    for(uint8_t i = 0; i < PROT_MT_CMD_NUM; i++)
    {
        if(procc->tx_ctrl[i].tx_flag != 1)
        {
            continue;
        }
        if(procc->tx_ctrl[i].period_tmr < mt_param_default[i].period)
        {
            procc->tx_ctrl[i].period_tmr += period;
            continue;
        }
        procc->tx_ctrl[i].period_tmr = 0;
        mt_param_default[i].load_data(mt_param_default[i].tx_cmd);
    }
}

static uint8_t cmd_pase(uint16_t cmd, uint8_t *data, uint16_t len)
{
    for (uint8_t i = 0; i < PROT_MT_CMD_NUM; i++)
    {
        if (cmd != mt_param_default[i].rx_cmd)
            continue;
        if (mt_param_default[i].cmp_prot_mt_cmd_pase_func != RT_NULL)
        {
            mt_param_default[i].cmp_prot_mt_cmd_pase_func(data, len);
            return 1;
        }
    }
    return 0;
}

/***************************************************************************************
 * @Function    : cmp_prot21_mt_recieve_msg()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : SSY            Version   : V3.0.0                Date:2024/6/27
 ****************************************************************************************/
static uint8_t cmp_prot21_mt_set_msg(uint8_t *msg, uint16_t len)
{
    uint8_t check;
    uint16_t cmd, pud_len;
    uint8_t res = 0;

    while (len > 39)
    {
        if (msg[0] != 'W')
        {
            len -= 1;
            msg += 1;
            continue;
        }

        if (msg[1] != 'M')
        {
            len -= 2;
            msg += 2;
            continue;
        }

        if (strncmp(mt_conf.procc.ble_conn_conf.ble_name, (char *)&msg[5], 28) != 0)
        {
            msg += 33;
            len -= 33;
            continue;
        }

        memcpy((uint8_t *)&cmd, &msg[34], 2);
        memcpy((uint8_t *)&pud_len, &msg[36], 2);

        if (len < 39 + pud_len)
        {
            msg += 38;
            len -= 38;
            continue;
        }

        check = xor_check(msg, 38 + pud_len);
        if (check != msg[38 + pud_len])
        {
            msg = msg + 39 + pud_len;
            len = len - 39 - pud_len;
            continue;
        }

        if (cmd_pase(cmd, &msg[38], pud_len) == 0)
        {
            msg = msg + 39 + pud_len;
            len = len - 39 - pud_len;

            continue;
        }
        msg = msg + 39 + pud_len;
        len = len - 39 - pud_len;

        if (++res > 10)
            return res;
    }
    return res;
    
}

/***************************************************************************************
 * @Function    : cmp_prot21_mt_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : SSY            Version   : V3.0.0                Date:2024/6/27
 ****************************************************************************************/
static void cmp_prot21_mt_work(uint8_t period)
{
    cmp_prot_mt_tx_ctrl_work(&mt_conf.procc, period);
}

/***************************************************************************************
 * @Function    : cmp_prot_upper_src_dev_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/16
 ****************************************************************************************/
static void cmp_prot_mt_dev_init(void)
{
    mt_conf.serial = drv_dev_find(DRV_DEV_SERIAL_SERV);
    if (mt_conf.serial == RT_NULL)
    {
        return;
    }

    mt_conf.clock = drv_dev_find(DRV_DEV_CLOCK);
    if (mt_conf.clock == RT_NULL)
    {
        return;
    }

}

/***************************************************************************************
 * @Function    : cmp_prot_mt_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : SSY            Version   : V3.0.0                Date:2024/6/27
 ****************************************************************************************/
static void cmp_prot_mt_create(void)
{
    static cmp_prot_mt_dev_stu prot21_mt_dev;

    /* 协议2.1对桩方法 */
    prot21_mt_dev.cmp_prot21_mt_save_conf = cmp_prot21_mt_save_conf;
    prot21_mt_dev.cmp_prot21_mt_send_msg = cmp_prot21_mt_send_msg;
    prot21_mt_dev.cmp_prot21_mt_set_msg = cmp_prot21_mt_set_msg;
    prot21_mt_dev.cmp_prot21_mt_work = cmp_prot21_mt_work;
    prot21_mt_dev.cmp_prot21_mt_remote_start = cmp_prot21_mt_remote_start;
    prot21_mt_dev.cmp_prot21_mt_remote_stop = cmp_prot21_mt_remote_stop;

    cmp_dev_append(CMP_DEV_NAME_PROT21_MT, &prot21_mt_dev);
}

/***************************************************************************************
 * @Function    : cmp_prot_mt_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : SSY            Version   : V3.0.0                Date:2024/6/27
 ****************************************************************************************/
static int cmp_prot_mt_dev_create(void)
{
    cmp_prot_mt_dev_init();

    cmp_prot_mt_create();

    return 0;
}
INIT_APP_EXPORT(cmp_prot_mt_dev_create);


