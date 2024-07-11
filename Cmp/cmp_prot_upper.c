#include "cmp_prot_upper.h"

static cmp_prot_upper_src_stu upper_conf;


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
static void cmp_prot_upper_genr_tx_id(uint32_t id)
{
    drv_bcd_datetime_stu curr_time;
    upper_conf.clock->drv_get_bcd_datetime(&curr_time);
    id = curr_time.hour*60*60 + curr_time.min*60 + curr_time.sec;
}

static void cmp_prot_upper_save_ble_conn(uint8_t *msg)
{
    cmp_prot_upper_tx_msg_conn_ble_stu *upper_ble_conn;
    cmp_ble_to_server_msg_ble_conn_sta_stu ble_conn_sta;
    memcpy(&ble_conn_sta, msg, sizeof(cmp_ble_to_server_msg_ble_conn_sta_stu));
    uint32_t id;

    upper_ble_conn->ble_conn_sta_flag = ble_conn_sta.ble_sta;

    RyanJson_t head = upper_conf.ryjson->ryjson_api_creat_ryjson();
    upper_conf.ryjson->ryjson_api_add_stringItem("target", "Controller", &head);
    upper_conf.ryjson->ryjson_api_add_stringItem("id", id, &head);
    upper_conf.ryjson->ryjson_api_add_stringItem("title", "CheckTimeRes", &head);
    cmp_prot_upper_send_msg(head, NULL);

}

static void cmp_prot_upper_save_w5500_conn(uint8_t *msg)
{
    cmp_prot_w5500_param_stu w5500_param;
    RyanJson_t head = upper_conf.ryjson->ryjson_api_creat_ryjson();
    uint32_t id;

    if(w5500_param.w5500_conn_status == 1)
    {
        cmp_prot_upper_genr_tx_id(id);
        upper_conf.ryjson->ryjson_api_add_stringItem("target", "Controller", &head);
        upper_conf.ryjson->ryjson_api_add_intItem("id", id, &head);
        upper_conf.ryjson->ryjson_api_add_stringItem("title", "LoginReq", &head);
        cmp_prot_upper_send_msg(head, NULL);
    }
}

static void  cmp_prot_upper_save_param(UPPER_PROT_SAVE_TYPE type, uint8_t *msg)
{
    switch (type)
    {
    case UPPER_PROT_BLE_CONN_STA: cmp_prot_upper_save_ble_conn(msg);
        break;
    case UPPER_PROT_W5500_CONN_STA: cmp_prot_upper_save_w5500_conn(msg);
        break;
    default:
        break;
    }
}


static void cmp_prot_upper_send_msg(RyanJson_t head, RyanJson_t body)
{
    char *tx_msg = NULL;

    RyanJson_t tx_json = upper_conf.ryjson->ryjson_api_creat_ryjson();
    upper_conf.ryjson->ryjson_api_add_objectItem("head", head, &tx_json);
    upper_conf.ryjson->ryjson_api_add_objectItem("body", body, &tx_json);

    tx_msg = upper_conf.ryjson->ryjson_api_ryjson_to_string(&tx_json);
    upper_conf.ryjson->ryjson_api_ryjson_delet(&tx_json);
}

static void prot_upper_check_time(uint32_t id, RyanJson_t *body)
{
    drv_bcd_datetime_stu *bcd_date;
    char *check_time = NULL;
    const char *time_format = ":%d:%d:%d:%d:%d:%d";
    uint8_t year, month, day, hour, minute, second;

    if(upper_conf.ryjson->ryjson_api_get_string_byKey("systemTime",&check_time,body) > 0)
    {
        return;
    }

    if (sscanf(check_time, time_format, &year, &month, &day, &hour, &minute, &second) == 6)
    {
        bcd_date->year = year;
        bcd_date->month = month;
        bcd_date->day = day;
        bcd_date->hour = hour;
        bcd_date->min = minute;
        bcd_date->sec = second;
    }
    upper_conf.clock->drv_clock_syn_bcd(bcd_date);

    RyanJson_t head = upper_conf.ryjson->ryjson_api_creat_ryjson();
    upper_conf.ryjson->ryjson_api_add_stringItem("target", "Controller", &head);
    upper_conf.ryjson->ryjson_api_add_intItem("id", id, &head);
    upper_conf.ryjson->ryjson_api_add_stringItem("title", "CheckTimeRes", &head);
    cmp_prot_upper_send_msg(head, NULL);
}

static void prot_upper_login_profile(uint32_t id, RyanJson_t *body)
{
    char *res = NULL;
    int32_t interval;
    cmp_prot_upper_to_server_msg_login_stu login_res;

    if(upper_conf.ryjson->ryjson_api_get_string_byKey("res",&res,body) > 0 ||
        upper_conf.ryjson->ryjson_api_get_int_byKey("interval",&interval,body) > 0)
    {
        return;
    }
    if (strcmp(res, "accepted") == 0) 
    {
        login_res.login_res = 0;
    } 
    else if (strcmp(res, "rejected") == 0) 
    {
        login_res.login_res = 1;
    }
    login_res.login_interval = (uint8_t)interval;
    upper_conf.cmd_callback(PROT21_MT_HOOK_CMD_LOGIN_PROFILE, &login_res);
    
    RyanJson_t head = upper_conf.ryjson->ryjson_api_creat_ryjson();
    upper_conf.ryjson->ryjson_api_add_stringItem("target", "Controller", &head);
    upper_conf.ryjson->ryjson_api_add_intItem("id", id, &head);
    upper_conf.ryjson->ryjson_api_add_stringItem("title", "SetLogProfileRes", &head);
    cmp_prot_upper_send_msg(head, NULL);
}

static void prot_upper_connect_EVCC(uint32_t id, RyanJson_t *body)
{
    char *bleName = NULL;
    int32_t connectFlag;
    cmp_prot_upper_to_server_msg_conn_ble_stu conn_ble;

    if(upper_conf.ryjson->ryjson_api_get_string_byKey("bleName",&bleName,body) > 0 ||
        upper_conf.ryjson->ryjson_api_get_int_byKey("connectFlag",&connectFlag,body) > 0)
    {
        return;
    }
    strcpy(conn_ble.ble_name,(uint8_t *)bleName);
    conn_ble.conn_flag = (uint8_t)connectFlag;
    upper_conf.cmd_callback(PROT21_MT_HOOK_CMD_CONNECT_EVCC, &conn_ble);
    
    RyanJson_t head = upper_conf.ryjson->ryjson_api_creat_ryjson();
    upper_conf.ryjson->ryjson_api_add_stringItem("target", "Controller", &head);
    upper_conf.ryjson->ryjson_api_add_intItem("id", id, &head);
    upper_conf.ryjson->ryjson_api_add_stringItem("title", "ConnEVCCRes", &head);
    cmp_prot_upper_send_msg(head, NULL);
}

static void prot_upper_control_power_switch(uint32_t id, RyanJson_t *body)
{
    char *command = NULL;
    cmp_prot_upper_to_server_msg_power_switch_stu power_sw;

    if(upper_conf.ryjson->ryjson_api_get_string_byKey("command",&command,body) > 0)
    {
        return;
    }
    if (strcmp(command, "start") == 0) 
    {
        power_sw.command = 0;
    } 
    else if (strcmp(command, "stop") == 0) 
    {
        power_sw.command = 1;
    }
    upper_conf.cmd_callback(PROT21_MT_HOOK_CMD_CONTROL_POWER_SWITCH, &power_sw);
    
    RyanJson_t head = upper_conf.ryjson->ryjson_api_creat_ryjson();
    upper_conf.ryjson->ryjson_api_add_stringItem("target", "Power", &head);
    upper_conf.ryjson->ryjson_api_add_intItem("id", id, &head);
    upper_conf.ryjson->ryjson_api_add_stringItem("title", "CtrlPowerSwRes", &head);
    cmp_prot_upper_send_msg(head, NULL);
}

static void prot_upper_control_power_valtage(uint32_t id, RyanJson_t *body)
{
    int32_t input_Va, input_Vb, input_Vc;
    cmp_prot_upper_to_server_msg_power_val_stu power_val;

    if(upper_conf.ryjson->ryjson_api_get_int_byKey("Va",&input_Va,body) > 0 ||
        upper_conf.ryjson->ryjson_api_get_int_byKey("Vb",&input_Vb,body) > 0 ||
        upper_conf.ryjson->ryjson_api_get_int_byKey("Vc",&input_Vc,body) > 0)
    {
        return;
    }
    power_val.input_Va = input_Va;
    power_val.input_Vb = input_Vb;
    power_val.input_Vc = input_Vc;
    upper_conf.cmd_callback(PROT21_MT_HOOK_CMD_CONTROL_POWER_VALTAGE, &power_val);
    
    RyanJson_t head = upper_conf.ryjson->ryjson_api_creat_ryjson();
    upper_conf.ryjson->ryjson_api_add_stringItem("target", "Power", &head);
    upper_conf.ryjson->ryjson_api_add_intItem("id", id, &head);
    upper_conf.ryjson->ryjson_api_add_stringItem("title", "CtrlPowerValRes", &head);
    cmp_prot_upper_send_msg(head, NULL);
}

static void prot_upper_control_load_switch(uint32_t id, RyanJson_t *body)
{

    
    RyanJson_t head = upper_conf.ryjson->ryjson_api_creat_ryjson();
    upper_conf.ryjson->ryjson_api_add_stringItem("target", "Power", &head);
    upper_conf.ryjson->ryjson_api_add_intItem("id", id, &head);
    upper_conf.ryjson->ryjson_api_add_stringItem("title", "CtrlLoadSwRes", &head);
    cmp_prot_upper_send_msg(head, NULL);
}

static void prot_upper_remote_charge(uint32_t id, RyanJson_t *body)
{
    char *command = NULL;
    cmp_prot_upper_to_server_msg_remote_chrg_stu remote_chrg;

    if(upper_conf.ryjson->ryjson_api_get_string_byKey("command",&command,body) > 0)
    {
        return;
    }
    if (strcmp(command, "start") == 0) 
    {
        remote_chrg.command = 0;
    } 
    else if (strcmp(command, "stop") == 0) 
    {
        remote_chrg.command = 1;
    }
    upper_conf.cmd_callback(PROT21_MT_HOOK_CMD_REMOTE_CHARGE, &remote_chrg);
    
    RyanJson_t head = upper_conf.ryjson->ryjson_api_creat_ryjson();
    upper_conf.ryjson->ryjson_api_add_stringItem("target", "EVCharger", &head);
    upper_conf.ryjson->ryjson_api_add_intItem("id", id, &head);
    upper_conf.ryjson->ryjson_api_add_stringItem("title", "RemoteChrgRes", &head);
    cmp_prot_upper_send_msg(head, NULL);
}

static void prot_upper_control_load_current(uint32_t id, RyanJson_t *body)
{
    int32_t Ia, Ib, Ic;
    cmp_prot_upper_to_server_msg_load_curr_stu load_curr;

    if(upper_conf.ryjson->ryjson_api_get_int_byKey("Ia",&Ia,body) > 0 ||
        upper_conf.ryjson->ryjson_api_get_int_byKey("Ib",&Ib,body) > 0 ||
        upper_conf.ryjson->ryjson_api_get_int_byKey("Ic",&Ic,body) > 0)
    {
        return;
    }
    load_curr.Ia = (uint16_t)Ia;
    load_curr.Ib = (uint16_t)Ib;
    load_curr.Ic = (uint16_t)Ic;
    upper_conf.cmd_callback(PROT21_MT_HOOK_CMD_CONTROL_LOAD_CURRENT, &load_curr);
    
    RyanJson_t head = upper_conf.ryjson->ryjson_api_creat_ryjson();
    upper_conf.ryjson->ryjson_api_add_stringItem("target", "Load", &head);
    upper_conf.ryjson->ryjson_api_add_intItem("id", id, &head);
    upper_conf.ryjson->ryjson_api_add_stringItem("title", "CtrlLoadCurrRes", &head);
    cmp_prot_upper_send_msg(head, NULL);
}

/***************************************************************************************
 * @Function    : cmp_prot1_mt_set_msg()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/16
 ****************************************************************************************/
static void cmp_prot_upper_cmd_pase(uint32_t id, char *title, RyanJson_t *body)
{
    uint8_t cmd;

    if (strcmp(title, "CheckTimeReq") == 0) {
        cmd = CMD_CHECK_TIME;
    } 
    else if (strcmp(title, "ConnEVCCReq") == 0) {
        cmd = CMD_CONNECT_EVCC;
    } 
    else if (strcmp(title, "SetLogProfileReq") == 0) {
        cmd = CMD_LOGIN_PROFILE;
    } 
    else if (strcmp(title, "CtrlPowerSwReq") == 0) {
        cmd = CMD_CONTROL_POWER_SWITCH;
    }     
    else if (strcmp(title, "CtrlPowerValReq") == 0) {
        cmd = CMD_CONTROL_POWER_VALTAGE;
    } 
    else if (strcmp(title, "CtrlLoadSwReq") == 0) {
        cmd = CMD_CONTROL_LOAD_SWITCH;
    } 
    else if (strcmp(title, "RemoteChrgReq") == 0) {
        cmd = CMD_REMOTE_CHARGE;
    } 
    else if (strcmp(title, "CtrlLoadCurrReq") == 0) {
        cmd = CMD_CONTROL_LOAD_CURRENT;
    } 
    else {
        cmd = CMD_UNKNOWN; return;
    }

    switch (cmd)
    {
        case CMD_CHECK_TIME: prot_upper_check_time(id,body); break;  
        case CMD_CONNECT_EVCC: prot_upper_connect_EVCC(id,body); break;
        case CMD_LOGIN_PROFILE: prot_upper_login_profile(id,body); break;
        case CMD_CONTROL_POWER_SWITCH: prot_upper_control_power_switch(id,body); break;
        case CMD_CONTROL_POWER_VALTAGE: prot_upper_control_power_valtage(id,body); break;
        case CMD_CONTROL_LOAD_SWITCH: prot_upper_control_load_switch(id,body); break;
        case CMD_REMOTE_CHARGE: prot_upper_remote_charge(id,body); break;
        case CMD_CONTROL_LOAD_CURRENT: prot_upper_control_load_current(id,body); break;

    }
}

//解析上位机报文
static void cmp_prot_upper_set_msg(uint8_t *msg, uint16_t datalen)
{
    uint32_t id;
    char *title = NULL;
    static RyanJson_t json;
    static RyanJson_t head;
    static RyanJson_t body;

    if(upper_conf.ryjson->ryjson_api_parse((char *)msg,&json) == 0)
    {
        if(upper_conf.ryjson->ryjson_api_get_object_byKey("head",&head,&json) > 0 ||
            upper_conf.ryjson->ryjson_api_get_object_byKey("body",&body,&json) > 0)
        {
            return;
        }
        if(upper_conf.ryjson->ryjson_api_get_int_byKey("id",&id,&head) > 0 || 
            upper_conf.ryjson->ryjson_api_get_string_byKey("title",&title,&head) > 0)
        {
            return;
        }
    }
    cmp_prot_upper_cmd_pase(id,title,&body);
}

/***************************************************************************************
 * @Function    : cmp_prot1_set_cmd_hook()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/26
 ****************************************************************************************/
static void cmp_prot_upper_cmd_hook(upper_set_cmd_hook_func cmd_hook)
{
    upper_conf.cmd_callback = cmd_hook;
}

/***************************************************************************************
 * @Function    : cmp_prot1_mt_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/16
 ****************************************************************************************/
static void cmp_prot_upper_work(uint8_t period)
{

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
static void cmp_prot_upper_dev_init(void)
{
    upper_conf.serial = drv_dev_find(DRV_DEV_SERIAL_SERV);
    if (upper_conf.serial == RT_NULL)
    {
        return;
    }

    upper_conf.clock = drv_dev_find(DRV_DEV_CLOCK);
    if (upper_conf.clock == RT_NULL)
    {
        return;
    }

    upper_conf.w5500 = drv_dev_find(CMP_DEV_NAME_W5500);
    if (upper_conf.w5500 == RT_NULL)
    {
        return;
    }

    upper_conf.ryjson = drv_dev_find(CMP_DEV_NAME_RYJSON);
    if (upper_conf.ryjson == RT_NULL)
    {
        return;
    }
}

/***************************************************************************************
 * @Function    : cmp_prot_mt_dev_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/16
 ****************************************************************************************/
static void cmp_prot_upper_create(void)
{
    static cmp_prot_upper_dev_stu prot_upper_dev;

    prot_upper_dev.cmp_prot_upper_save_param = cmp_prot_upper_save_param;  //save param for upper to query (eg: ble_conn_status)

    prot_upper_dev.cmp_prot_upper_set_msg = cmp_prot_upper_set_msg;  //handle msg from upper : 1.direct execution; 2.save config to cmp_prot_mt.c; 3.forward to EVCC
    // prot_mt_dev.cmp_prot21_mt_recieve_msg = cmp_prot21_mt_recieve_msg;
    prot_upper_dev.cmp_prot_upper_work = cmp_prot_upper_work;
    prot_upper_dev.upper_set_cmd_hook = cmp_prot_upper_cmd_hook;  //msg from upper(type 1 & 2) must be handled in server.c

    cmp_dev_append(CMP_DEV_NAME_PROT_UPPER, &prot_upper_dev);
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
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/15
 ****************************************************************************************/
static int cmp_prot_upper_dev_create(void)
{
    cmp_prot_upper_dev_init();

    cmp_prot_upper_create();

    return 0;
}
INIT_APP_EXPORT(cmp_prot_upper_dev_create);




