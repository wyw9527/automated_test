#include "app_server.h"

#define SERV_QUE_PEND_TMOUT 10

static void app_serv_init(void);
static void app_serv_work(uint8_t period);

static app_serv_stu serv_info;


/***************************************************************************************
 * @Function    : app_server()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2023/6/12
 ****************************************************************************************/
void app_server(void *parameter)
{
    uint8_t *p_msg;
    uint32_t tick_rec, tick_curr = 0, qmout = SERV_QUE_PEND_TMOUT, tm_accu = 0;

    app_serv_init();

    while (1)
    {
        tick_rec = rt_tick_get();
        if (RT_EOK == rt_mb_recv(&mb_serv, (rt_ubase_t *)&p_msg, qmout))
        {
            switch (*p_msg)
            {

            }

            tick_curr = rt_tick_get();
            if (tick_curr < tick_rec)
            {
                qmout = (0xFFFFFFFFu - tick_rec) + tick_curr;
            }
            else
            {
                qmout = tick_curr - tick_rec;
            }

            tm_accu += qmout;
            if (tm_accu >= SERV_QUE_PEND_TMOUT)
            {
                tm_accu = 0;

                app_serv_work(SERV_QUE_PEND_TMOUT);
            }
            qmout = SERV_QUE_PEND_TMOUT - tm_accu;
        }
        else
        {
            app_serv_work(SERV_QUE_PEND_TMOUT);

            qmout = SERV_QUE_PEND_TMOUT;

            tm_accu = 0;
        }
    }
}


static void app_serv_hnd_prot21_login_profile_cmd(uint8_t *login_profile)
{
    serv_info.mt->cmp_prot21_mt_save_conf(MT_PROT_UPPER_LOGIN_PROFILE,(uint8_t *)login_profile);
}
static void app_serv_hnd_prot21_conn_ble_cmd(uint8_t *conn_ble)
{
    serv_info.ble->cmp_get_conn_ble_cmd((uint8_t *)conn_ble);
}

static void app_serv_hnd_prot21_power_switch_cmd(uint8_t *power_switch)
{
    cmp_prot_upper_to_server_msg_power_switch_stu *power_sw = (cmp_prot_upper_to_server_msg_power_switch_stu *)power_switch;
    if(power_sw->command == 0)
    {
        serv_info.power->cmp_set_power_box_start();
    }
    else if(power_sw->command == 1)
    {
        serv_info.power->cmp_set_power_box_stop();
    }
}

static void app_serv_hnd_prot21_power_valtage_cmd(uint8_t *power_val)
{
    cmp_prot_upper_to_server_msg_power_val_stu *val = (cmp_prot_upper_to_server_msg_power_val_stu *)power_val;
    serv_info.power->cmp_set_power_box_voltage(val->input_Va,val->input_Vb,val->input_Vc);
}

static void app_serv_hnd_prot21_load_switch_cmd(uint8_t *load_switch)
{

}

static void app_serv_hnd_prot21_remote_charge_cmd(uint8_t *remote_chrg)
{
    cmp_prot_upper_to_server_msg_remote_chrg_stu *chrg = (cmp_prot_upper_to_server_msg_remote_chrg_stu *)remote_chrg;
    if(chrg->command == 0)
    {
        serv_info.mt->cmp_prot21_mt_remote_start();
    }
    else if(chrg->command == 1)
    {
        serv_info.mt->cmp_prot21_mt_remote_stop();
    }
}

static void app_serv_hnd_prot21_load_current_cmd(uint8_t *load_curr)
{
    cmp_prot_upper_to_server_msg_load_curr_stu *val = (cmp_prot_upper_to_server_msg_load_curr_stu *)load_curr;
    serv_info.load->cmp_load_set_current(val->Ia,val->Ib,val->Ic);
}

/***************************************************************************************
 * @Function    : app_serv_prot_cmd_hook()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2023/6/26
 ****************************************************************************************/
static void app_serv_upper_prot_cmd_hook(CMP_PROT_UPPER_HOOK_CMD_TYPE cmd, void *data)
{
    switch (cmd)
    {
    case PROT21_MT_HOOK_CMD_LOGIN_PROFILE:
        app_serv_hnd_prot21_login_profile_cmd(data);
        break;
    case PROT21_MT_HOOK_CMD_CONNECT_EVCC:
        app_serv_hnd_prot21_conn_ble_cmd(data);
        break;    
    case PROT21_MT_HOOK_CMD_CONTROL_POWER_SWITCH:
        app_serv_hnd_prot21_power_switch_cmd(data);
        break;
    case PROT21_MT_HOOK_CMD_CONTROL_POWER_VALTAGE:
        app_serv_hnd_prot21_power_valtage_cmd(data);
        break;
    case PROT21_MT_HOOK_CMD_CONTROL_LOAD_SWITCH:
        app_serv_hnd_prot21_load_switch_cmd(data);
        break;
    case PROT21_MT_HOOK_CMD_REMOTE_CHARGE:
        app_serv_hnd_prot21_remote_charge_cmd(data);
        break;
    case PROT21_MT_HOOK_CMD_CONTROL_LOAD_CURRENT:
        app_serv_hnd_prot21_load_current_cmd(data);
        break;
    }
}

static void app_serv_hnd_ble_conn_sta(uint8_t *ble_conn)
{
    serv_info.mt->cmp_prot21_mt_save_conf(MT_PROT_UPPER_BLE_CONN_STA,(uint8_t *)ble_conn);
    serv_info.upper->cmp_prot_upper_save_param(UPPER_PROT_BLE_CONN_STA,(uint8_t*)ble_conn);
}

static void app_serv_hnd_ble_rx_msg(uint8_t* msg)
{
    drv_serial_rx_stu rx_msg;
    memcpy(&rx_msg,msg,sizeof(drv_serial_rx_stu));
    serv_info.mt->cmp_prot21_mt_set_msg(rx_msg.buff,rx_msg.len);
}

/***************************************************************************************
 * @Function    : app_serv_ble_conn_sta_hook()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2023/6/26
 ****************************************************************************************/
static void app_serv_ble_conn_sta_hook(CMP_BLE_CONN_STA_TYPE cmd, void *data)
{
    switch (cmd)
    {
    case CMP_BLE_CONN_STA:
        app_serv_hnd_ble_conn_sta(data);
        break;
    case CMP_BLE_RX_MSG:
        app_serv_hnd_ble_rx_msg(data);
        break;
    default:
        break;
    }
}


/***************************************************************************************
 * @Function    : app_serv_prot_cmd_hook()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2023/6/26
 ****************************************************************************************/
static void app_serv_w5500_conn_sta_hook(CMP_W5500_CONN_STA_TYPE cmd, void *data)
{
    cmp_prot_w5500_param_stu status;
	cmp_w5500_rx_stu* rx = data;

    switch (cmd)
    {
    case W5500_CALLBACK_CMD_SPP:
        status.w5500_conn_status = 1;
        serv_info.upper->cmp_prot_upper_save_param(UPPER_PROT_W5500_CONN_STA,(uint8_t*)&status);
        break;
    case CMP_W5500_RX_MSG:
        serv_info.upper->cmp_prot_upper_set_msg(rx->rx_buff,rx->rx_len);
        break;
    case W5500_CALLBACK_CMD_AT:
        status.w5500_conn_status = 0;
        serv_info.upper->cmp_prot_upper_save_param(UPPER_PROT_W5500_CONN_STA,(uint8_t*)&status);
        break;
    default:
        break;
    }
}


/***************************************************************************************
 * @Function    : app_server_serial_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2023/6/8
 ****************************************************************************************/
static void app_server_serial_init(void)
{
    drv_serial_init_param_stu param;

    serv_info.serial = drv_dev_find(DRV_DEV_SERIAL_SERV);

    param.baut = SERIAL_BAUT_115200;
    param.dma_sta = SERIAL_DMA_ENABLE;
    param.parity = SERIAL_PARITY_NONE;
    param.stb = SERIAL_STB_1BIT;
    param.wl = SERIAL_WL_8BIT;

    serv_info.serial->dev_param_init(&param);
}

/***************************************************************************************
 * @Function    : app_serv_dev_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2023/6/16
 ****************************************************************************************/
static void app_serv_dev_init(void)
{
    serv_info.upper = cmp_dev_find(CMP_DEV_NAME_PROT_UPPER);
    serv_info.upper->upper_set_cmd_hook(app_serv_upper_prot_cmd_hook);
    serv_info.ble = cmp_dev_find(CMP_DEV_NAME_BLE);
    serv_info.ble->ble_conn_sta_hook(app_serv_ble_conn_sta_hook);
    serv_info.mt = cmp_dev_find(CMP_DEV_NAME_PROT21_MT);
    serv_info.w5500 = cmp_dev_find(CMP_DEV_NAME_W5500);
    serv_info.w5500->w5500_conn_sta_hook(app_serv_w5500_conn_sta_hook);
    serv_info.power = cmp_dev_find(CMP_DEV_NAME_POWER);
    serv_info.load = cmp_dev_find(CMP_DEV_NAME_LOAD);
    if(NULL == serv_info.upper || NULL == serv_info.ble || NULL == serv_info.mt || NULL == serv_info.w5500 || NULL == serv_info.power || NULL == serv_info.load)
    {
        return;
    }

    serv_info.procc.item.item_type = 0;
}


/***************************************************************************************
 * @Function    : app_serv_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2023/6/12
 ****************************************************************************************/
static void app_serv_init(void)
{
    app_serv_dev_init();

    app_server_serial_init();


}


/***************************************************************************
* FuncName     : net_hnd_w5500_rx_msg()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : chenbao         Version:    V1.0      Date:    2022/7/1
***************************************************************************/
static void net_hnd_w5500_rx_msg()
{    
    memset(serv_info.w5500_comm.rx_buff,0,sizeof(serv_info.w5500_comm.rx_buff));
    serv_info.w5500_comm.rx_len = serv_info.w5500->func_rx_msg((uint8_t*)serv_info.w5500_comm.rx_buff,sizeof(serv_info.w5500_comm.rx_buff));
    if(serv_info.w5500_comm.rx_len  < 2)
    {
        return;
    }
    
    serv_info.w5500->func_rx_msg_hnd((uint8_t*)serv_info.w5500_comm.rx_buff,serv_info.w5500_comm.rx_len);

    serv_info.w5500->func_tx_msg((uint8_t*)serv_info.w5500_comm.rx_buff,serv_info.w5500_comm.rx_len);
}


/***************************************************************************************
 * @Function    : app_serv_prot_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2023/6/16
 ****************************************************************************************/
static void app_serv_prot_work(uint8_t period)
{
    serv_info.mt->cmp_prot21_mt_work(period);
    serv_info.ble->cmp_ble_work(period);
    serv_info.w5500->cmp_w5500_work(period);
    net_hnd_w5500_rx_msg();

}


/***************************************************************************************
 * @Function    : app_serv_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2023/6/12
 ****************************************************************************************/
static void app_serv_work(uint8_t period)
{
    app_serv_prot_work(period);

}
