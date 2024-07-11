#include "cmp_ble.h"



static cmp_ble_stu ble_conf;

static void conn_ble_sw_step(CONN_BLE_STEP_TYPE new_step);

static void cmp_ble_pub_dev_init()
{
    ble_conf.ble_serial = drv_dev_find(DRV_DEV_SERIAL_SERV);
    if (RT_NULL == ble_conf.ble_serial)
    {
        return;
    }
    drv_serial_init_param_stu param;
    param.baut =  SERIAL_BAUT_115200;
    param.dma_sta = SERIAL_DMA_ENABLE;
    param.parity = SERIAL_PARITY_NONE;
    param.stb = SERIAL_STB_1BIT;
    param.wl = SERIAL_WL_8BIT;

    ble_conf.ble_serial->dev_param_init(&param);
}

static void cmp_get_conn_ble_cmd(uint8_t *msg)
{
    memcpy(ble_conf.procc.conn_ble_name, msg, 28);
    ble_conf.procc.conn_ble_cmd_flag = msg[28];
    ble_conf.procc.conn_ble_count = 0;
    ble_conf.procc.conn_dev_count = 0;
}

static void exec_restart_ble_func(uint8_t period)
{
    drv_serial_rx_stu rx_msg;
    cmp_ble_to_server_msg_ble_conn_sta_stu ble_conn_sta;
    
    if(ble_conf.procc.data.wait_count == 0)
    {
        if(ble_conf.procc.conn_ble_count > 5)
        {
            ble_conf.procc.conn_ble_cmd_flag = 0;
            ble_conn_sta.ble_sta = 5;
            ble_conf.ble_callback(CMP_BLE_CONN_STA, &ble_conn_sta);
        }
        ble_conf.procc.conn_ble_count += 1;

        ble_conf.ble_serial->dev_rx_buff_clear();
        char tx_msg[32];
        memset(tx_msg,0,32);
        strcat(tx_msg,"AT+RST\r\n");
        ble_conf.ble_serial->dev_send_msg((uint8_t *)tx_msg, strlen(tx_msg));
    }

    if(ble_conf.procc.data.wait_count <= 2000)
    {
        ble_conf.procc.data.wait_count += period;
        
        if(ble_conf.ble_serial->dev_get_msg(&rx_msg,0) == RT_EOK) 
        {
            if(strstr((char *)rx_msg.buff,"ready") != RT_NULL)
            {
                conn_ble_sw_step(SET_CLIENT);
            }
            else
            {
                return ;
            }
        }
    }
    else if(ble_conf.procc.data.wait_count > 2000)
    {
        ble_conf.procc.data.wait_count = 0;
        return ;
    }
}

static void exec_set_client_func(uint8_t period)
{
    drv_serial_rx_stu rx_msg;
    
    if(ble_conf.procc.data.wait_count == 0)
    {
        ble_conf.ble_serial->dev_rx_buff_clear();
        char tx_msg[32];
        memset(tx_msg,0,32);
        strcat(tx_msg,"AT+BLEINIT=1\r\n");
        ble_conf.ble_serial->dev_send_msg((uint8_t *)tx_msg, strlen(tx_msg));
    }

    if(ble_conf.procc.data.wait_count <= 1000)
    {
        ble_conf.procc.data.wait_count += period;

        if(ble_conf.ble_serial->dev_get_msg(&rx_msg,0) == RT_EOK) 
        {
            if(strstr((char *)rx_msg.buff,"OK") != RT_NULL && strstr((char *)rx_msg.buff,"ERROR") == RT_NULL)
            {
                conn_ble_sw_step(SCAN_BROADCAST_PARAM);
            }
            else if(strstr((char *)rx_msg.buff,"ERROR") != RT_NULL)
            {
                conn_ble_sw_step(RESTART_BLE);
            }
            else
            {
                return;
            }
        }
    }
    else if(ble_conf.procc.data.wait_count > 1000)
    {
        conn_ble_sw_step(RESTART_BLE);
    }
}

static void exec_scan_broadcast_param_func(uint8_t period)
{
    drv_serial_rx_stu rx_msg;
    
    if(ble_conf.procc.data.wait_count == 0)
    {
        ble_conf.ble_serial->dev_rx_buff_clear();
        char tx_msg[32];
        memset(tx_msg,0,32);
        strcat(tx_msg,"AT+BLESCANPARAM=0,0,0,100,50\r\n");
        ble_conf.ble_serial->dev_send_msg((uint8_t *)tx_msg, strlen(tx_msg));
    }

    if(ble_conf.procc.data.wait_count <= 2000)
    {
        ble_conf.procc.data.wait_count += period;

        if(ble_conf.ble_serial->dev_get_msg(&rx_msg,0) == RT_EOK) 
        {
            if(strstr((char *)rx_msg.buff,"OK") != RT_NULL && strstr((char *)rx_msg.buff,"ERROR") == RT_NULL)
            {
                conn_ble_sw_step(SCAN_BLE_NAME);
            }
            else if(strstr((char *)rx_msg.buff,"ERROR") != RT_NULL)
            {
                conn_ble_sw_step(RESTART_BLE);
            }
            else
            {
                return;
            }
        }
    }
    else if(ble_conf.procc.data.wait_count > 2000)
    {
        conn_ble_sw_step(RESTART_BLE);
    }
}

static void exec_scan_ble_name_func(uint8_t period)
{
    drv_serial_rx_stu rx_msg;

    if(ble_conf.procc.data.wait_count == 0)
    {
        ble_conf.ble_serial->dev_rx_buff_clear();
        char tx_msg[64];
        memset(tx_msg,0,64);
        strcat(tx_msg,"AT+BLESCAN=1,1,2,");
        strcat(tx_msg,"\"");
        strncat(tx_msg,ble_conf.procc.conn_ble_name,28);
        strcat(tx_msg,"\"");
        strcat(tx_msg,"\r\n");

        ble_conf.ble_serial->dev_send_msg((uint8_t *)tx_msg, strlen(tx_msg));
    }

    if(ble_conf.procc.data.wait_count <= 3000)
    {
        ble_conf.procc.data.wait_count += period;

        if(ble_conf.ble_serial->dev_get_msg(&rx_msg,0) == RT_EOK) 
        {
            if(strstr((char *)rx_msg.buff,"+BLESCAN:") != RT_NULL)
            {
                ble_conf.procc.data.wait_count = 0;
                uint8_t *ret = strstr((char *)rx_msg.buff,"+BLESCAN:");
                memcpy(ble_conf.procc.ble_mac, ret+10, 17);
                conn_ble_sw_step(CONN_TARGET_BLE);
            }
            else
            {
                return ;
            }
        }
    }
    else if(ble_conf.procc.data.wait_count > 3000)
    {
        conn_ble_sw_step(RESTART_BLE);
    }
}

static void exec_conn_target_ble_func(uint8_t period)
{
    drv_serial_rx_stu rx_msg;
    cmp_ble_to_server_msg_ble_conn_sta_stu ble_conn_sta;

    if(ble_conf.procc.data.wait_count == 0)
    {
        ble_conf.ble_serial->dev_rx_buff_clear();
        char tx_msg[64];
        memset(tx_msg,0,64);
        strcat(tx_msg,"AT+BLECONN=0,");
        strcat(tx_msg,"\"");
        strncat(tx_msg,ble_conf.procc.ble_mac,17);
        strcat(tx_msg,"\"");
        strcat(tx_msg,"\r\n");

        ble_conf.ble_serial->dev_send_msg((uint8_t *)tx_msg, strlen(tx_msg));
    }

    if(ble_conf.procc.data.wait_count <= 2000)
    {
        ble_conf.procc.data.wait_count += period;

        if(ble_conf.ble_serial->dev_get_msg(&rx_msg,0) == RT_EOK) 
        {
            if(strstr((char *)rx_msg.buff,"+BLECONN:") != RT_NULL && strstr((char *)rx_msg.buff,"ERROR") == RT_NULL)
            {
                if(ble_conf.procc.conn_dev_count > 3)
                {
                    ble_conf.procc.conn_ble_cmd_flag = 0;
                    ble_conn_sta.ble_sta = 2;
                    ble_conf.ble_callback(CMP_BLE_CONN_STA, &ble_conn_sta);
                }
                ble_conf.procc.conn_dev_count += 1;
                conn_ble_sw_step(CHANGE_TRANS_LEN);
            }
            else if(strstr((char *)rx_msg.buff,"ERROR") != RT_NULL || strstr((char *)rx_msg.buff,"+BLEDISCONN:") != RT_NULL)
            {
                conn_ble_sw_step(RESTART_BLE);
            }
            else
            {
                return;
            }
        }
    }
    else if(ble_conf.procc.data.wait_count > 2000)
    {
        conn_ble_sw_step(RESTART_BLE);
    }
}

static void exec_change_trans_len_func(uint8_t period)
{
        drv_serial_rx_stu rx_msg;
    
    if(ble_conf.procc.data.wait_count == 0)
    {
        ble_conf.ble_serial->dev_rx_buff_clear();
        char tx_msg[32];
        memset(tx_msg,0,32);
        strcat(tx_msg,"AT+BLECFGMTU=0,512\r\n");
        ble_conf.ble_serial->dev_send_msg((uint8_t *)tx_msg, strlen(tx_msg));
    }

    if(ble_conf.procc.data.wait_count <= 2000)
    {
        ble_conf.procc.data.wait_count += period;

        if(ble_conf.ble_serial->dev_get_msg(&rx_msg,0) == RT_EOK) 
        {
            if(strstr((char *)rx_msg.buff,"+BLECFGMTU:") != RT_NULL && strstr((char *)rx_msg.buff,"ERROR") == RT_NULL)
            {
                conn_ble_sw_step(GET_DEVICE_SERVER);
            }
            else if(strstr((char *)rx_msg.buff,"ERROR") != RT_NULL || strstr((char *)rx_msg.buff,"+BLEDISCONN:") != RT_NULL)
            {
                conn_ble_sw_step(RESTART_BLE);
            }
            else
            {
                return;
            }
        }
    }
    else if(ble_conf.procc.data.wait_count > 2000)
    {
        conn_ble_sw_step(RESTART_BLE);
    }
}

static void exec_get_device_server_func(uint8_t period)
{
    drv_serial_rx_stu rx_msg;
    cmp_ble_to_server_msg_ble_conn_sta_stu ble_conn_sta;
    
    if(ble_conf.procc.data.wait_count == 0)
    {
        ble_conf.ble_serial->dev_rx_buff_clear();
        char tx_msg[32];
        memset(tx_msg,0,32);
        strcat(tx_msg,"AT+BLEGATTCPRIMSRV=0\r\n");
        ble_conf.ble_serial->dev_send_msg((uint8_t *)tx_msg, strlen(tx_msg));
    }

    if(ble_conf.procc.data.wait_count <= 2000)
    {
        ble_conf.procc.data.wait_count += period;

        if(ble_conf.ble_serial->dev_get_msg(&rx_msg,0) == RT_EOK) 
        {
            if(strstr((char *)rx_msg.buff,"+BLEGATTCPRIMSRV:") != RT_NULL && strstr((char *)rx_msg.buff,"ERROR") == RT_NULL)
            {
                if(strstr((char *)rx_msg.buff,"0xA002") != RT_NULL)
                {
                    conn_ble_sw_step(GET_SERVER_CHAR);
                }
                else
                {
                    ble_conf.procc.conn_ble_cmd_flag = 0;
                    ble_conn_sta.ble_sta = 3;
                    ble_conf.ble_callback(CMP_BLE_CONN_STA, &ble_conn_sta);
                }
            }
            else if(strstr((char *)rx_msg.buff,"ERROR") != RT_NULL || strstr((char *)rx_msg.buff,"+BLEDISCONN:") != RT_NULL)
            {
                conn_ble_sw_step(RESTART_BLE);
            }
            else
            {
                return;
            }
        }
    }
    else if(ble_conf.procc.data.wait_count > 2000)
    {
        conn_ble_sw_step(RESTART_BLE);
    }
}

static void exec_get_server_char_func(uint8_t period)
{
    drv_serial_rx_stu rx_msg;
    cmp_ble_to_server_msg_ble_conn_sta_stu ble_conn_sta;

    if(ble_conf.procc.data.wait_count == 0)
    {
        ble_conf.ble_serial->dev_rx_buff_clear();
        char tx_msg[32];
        memset(tx_msg,0,32);
        strcat(tx_msg,"AT+BLEGATTCCHAR=0,3\r\n");
        ble_conf.ble_serial->dev_send_msg((uint8_t *)tx_msg, strlen(tx_msg));
    }

    if(ble_conf.procc.data.wait_count <= 3000)
    {
        ble_conf.procc.data.wait_count += period;

        if(ble_conf.ble_serial->dev_get_msg(&rx_msg,0) == RT_EOK) 
        {
            if(strstr((char *)rx_msg.buff,"+BLEGATTCCHAR:") != RT_NULL && strstr((char *)rx_msg.buff,"ERROR") == RT_NULL)
            {
                if(strstr((char *)rx_msg.buff,"0xC305") != RT_NULL && strstr((char *)rx_msg.buff,"0xC304") != RT_NULL )
                {
                    uint8_t *ret1 = strstr((char *)rx_msg.buff,"0xC305");
                    memcpy(ble_conf.procc.char_rx_id, ret1-2, 1);
                    uint8_t *ret2 = strstr((char *)rx_msg.buff,"0xC304");
                    memcpy(ble_conf.procc.char_tx_id, ret2-2, 1);
                    conn_ble_sw_step(WRITE_SERVER_CHAR);                
                }
                else
                {
                    ble_conf.procc.conn_ble_cmd_flag = 0;
                    ble_conn_sta.ble_sta = 4;
                    ble_conf.ble_callback(CMP_BLE_CONN_STA, &ble_conn_sta);
                }
            }
            else if(strstr((char *)rx_msg.buff,"ERROR") != RT_NULL || strstr((char *)rx_msg.buff,"+BLEDISCONN:") != RT_NULL)
            {
                conn_ble_sw_step(RESTART_BLE);
            }
            else
            {
                return;
            }
        }
    }
    else if(ble_conf.procc.data.wait_count > 3000)
    {
        conn_ble_sw_step(RESTART_BLE);
    }
}

static void exec_write_server_char_func(uint8_t period)
{
    drv_serial_rx_stu rx_msg;
    
    if(ble_conf.procc.data.wait_count == 0)
    {
        ble_conf.ble_serial->dev_rx_buff_clear();
        char tx_msg[32];
        memset(tx_msg,0,32);
        strcat(tx_msg,"AT+BLEGATTCWR=0,3,");
        strncat(tx_msg,ble_conf.procc.char_rx_id,1);
        strcat(tx_msg,",1,1\r\n");

        ble_conf.ble_serial->dev_send_msg((uint8_t *)tx_msg, strlen(tx_msg));
    }

    if(ble_conf.procc.data.wait_count <= 1000)
    {
        ble_conf.procc.data.wait_count += period;

        if(ble_conf.ble_serial->dev_get_msg(&rx_msg,0) == RT_EOK) 
        {
            if(strstr((char *)rx_msg.buff,">") != RT_NULL && strstr((char *)rx_msg.buff,"ERROR") == RT_NULL)
            {
                conn_ble_sw_step(CONFIG_SPP);
            }
            else if(strstr((char *)rx_msg.buff,"ERROR") != RT_NULL || strstr((char *)rx_msg.buff,"+BLEDISCONN:") != RT_NULL)
            {
                conn_ble_sw_step(RESTART_BLE);
            }
            else
            {
                return;
            }
        }
    }
    else if(ble_conf.procc.data.wait_count > 1000)
    {
        conn_ble_sw_step(RESTART_BLE);
    }
}

static void exec_config_SPP_func(uint8_t period)
{
    drv_serial_rx_stu rx_msg;
    
    if(ble_conf.procc.data.wait_count == 0)
    {
        ble_conf.ble_serial->dev_rx_buff_clear();
        char tx_msg[32];
        memset(tx_msg,0,32);
        strcat(tx_msg,"AT+BLESPPCFG=1,3,");
        strncat(tx_msg,ble_conf.procc.char_tx_id,1);
        strcat(tx_msg,",3,");
        strncat(tx_msg,ble_conf.procc.char_rx_id,1);
        strcat(tx_msg,",0\r\n");
        
        ble_conf.ble_serial->dev_send_msg((uint8_t *)tx_msg, strlen(tx_msg));
    }

    if(ble_conf.procc.data.wait_count <= 1000)
    {
        ble_conf.procc.data.wait_count += period;

        if(ble_conf.ble_serial->dev_get_msg(&rx_msg,0) == RT_EOK) 
        {
            if(strstr((char *)rx_msg.buff,"OK") != RT_NULL && strstr((char *)rx_msg.buff,"ERROR") == RT_NULL)
            {
                conn_ble_sw_step(ENTER_SPP_MODE);
            }
            else if(strstr((char *)rx_msg.buff,"ERROR") != RT_NULL)
            {
                ble_conf.procc.data.wait_count = 0;
                conn_ble_sw_step(CONFIG_SPP);
            }
            else if(strstr((char *)rx_msg.buff,"+BLEDISCONN:") != RT_NULL)
            {
                conn_ble_sw_step(RESTART_BLE);
            }
            else
            {
                return;
            }
        }
    }
    else if(ble_conf.procc.data.wait_count > 1000)
    {
        ble_conf.procc.data.wait_count = 0;
        conn_ble_sw_step(CONFIG_SPP);
    }
}

static void exec_enter_SPP_mode_func(uint8_t period)
{
    drv_serial_rx_stu rx_msg;
    
    if(ble_conf.procc.data.wait_count == 0)
    {
        ble_conf.ble_serial->dev_rx_buff_clear();
        char tx_msg[32];
        memset(tx_msg,0,32);
        strcat(tx_msg,"AT+BLESPP\r\n");
        ble_conf.ble_serial->dev_send_msg((uint8_t *)tx_msg, strlen(tx_msg));
    }

    if(ble_conf.procc.data.wait_count <= 1000)
    {
        ble_conf.procc.data.wait_count += period;

        if(ble_conf.ble_serial->dev_get_msg(&rx_msg,0) == RT_EOK) 
        {
            if(strstr((char *)rx_msg.buff,">") != RT_NULL && strstr((char *)rx_msg.buff,"ERROR") == RT_NULL)
            {
                cmp_ble_to_server_msg_ble_conn_sta_stu ble_conn_sta;

                ble_conf.procc.conn_ble_cmd_flag = 0;
                ble_conf.procc.data.conn_ble_flag = 1;
                ble_conn_sta.ble_sta = 1;
                memcpy(ble_conn_sta.ble_name,ble_conf.procc.conn_ble_name,28);
                ble_conf.ble_callback(CMP_BLE_CONN_STA, &ble_conn_sta);
            }
            else if(strstr((char *)rx_msg.buff,"ERROR") != RT_NULL)
            {
                conn_ble_sw_step(CONFIG_SPP);
            }
            else if(strstr((char *)rx_msg.buff,"+BLEDISCONN:") != RT_NULL)
            {
                conn_ble_sw_step(RESTART_BLE);
            }
            else
            {
                return;
            }
        }
    }
    else if(ble_conf.procc.data.wait_count > 1000)
    {
        conn_ble_sw_step(CONFIG_SPP);
    }
}

static conn_ble_fsm_type conn_ble_fsm[CONN_BLE_STEP_NUM] =
{
    NULL,
    exec_restart_ble_func,
    NULL,
    NULL,
    exec_set_client_func,
    NULL,
    NULL,
    exec_scan_broadcast_param_func,
    NULL,
    NULL,
    exec_scan_ble_name_func,
    NULL,
    NULL,
    exec_conn_target_ble_func,
    NULL,
    NULL,
    exec_change_trans_len_func,
    NULL,
    NULL,
    exec_get_device_server_func,
    NULL,
    NULL,
    exec_get_server_char_func,
    NULL,
    NULL,
    exec_write_server_char_func,
    NULL,
    NULL,
    exec_config_SPP_func,
    NULL,
    NULL,
    exec_enter_SPP_mode_func,
    NULL,
};

static void conn_ble_sw_step(CONN_BLE_STEP_TYPE new_step)
{
    if (ble_conf.procc.data.conn_ble_step == new_step)
    {
        return;
    }

    if (conn_ble_fsm[ble_conf.procc.data.conn_ble_step].eixt_conn_ble_func != NULL)
    {
        conn_ble_fsm[ble_conf.procc.data.conn_ble_step].eixt_conn_ble_func(NULL);
    }

    ble_conf.procc.data.conn_ble_step = new_step;
    ble_conf.procc.data.wait_count = 0;

    if (conn_ble_fsm[ble_conf.procc.data.conn_ble_step].entr_conn_ble_func != NULL)
    {
        conn_ble_fsm[ble_conf.procc.data.conn_ble_step].entr_conn_ble_func(NULL);
    }
}

static void cmp_conn_ble_work(uint8_t period)
{
    if(ble_conf.procc.conn_ble_cmd_flag == 1)
    {
        if (conn_ble_fsm[ble_conf.procc.data.conn_ble_step].exec_conn_ble_func != NULL)
        {
            conn_ble_fsm[ble_conf.procc.data.conn_ble_step].exec_conn_ble_func(period);
        }
    }
}

static void cmp_ble_rx_buff_msg_frame(uint8_t period)
{
    drv_serial_rx_stu rx_msg;

    if(ble_conf.procc.data.conn_ble_flag == 1)
    {
        if(ble_conf.ble_serial->dev_get_msg(&rx_msg,0) == RT_EOK) 
        {
            ble_conf.ble_callback(CMP_BLE_RX_MSG, &rx_msg);
        }    
        ble_conf.procc.comm.count = 0;
    }

}

static void cmp_ble_state_check(uint8_t period)
{
    if(ble_conf.procc.comm.count <= 50000)
    {
        ble_conf.procc.comm.count += period;
    }
    ble_conf.procc.comm.comm_flag = ble_conf.procc.comm.count>50000?1:0;
}

static void cmp_ble_conn_sta_hook(ble_conn_sta_hook_func ble_hook)
{
    ble_conf.ble_callback = ble_hook;
}

static void cmp_ble_work(uint8_t period)
{
    cmp_conn_ble_work(period);

    cmp_ble_rx_buff_msg_frame(period);
    
    cmp_ble_state_check(period);
}

static void cmp_ble_create()
{
    static cmp_ble_dev_stu ble_dev;
    ble_dev.cmp_ble_work = cmp_ble_work;
    ble_dev.cmp_get_conn_ble_cmd = cmp_get_conn_ble_cmd;
    ble_dev.ble_conn_sta_hook = cmp_ble_conn_sta_hook;


    cmp_dev_append(CMP_DEV_NAME_BLE, &ble_dev);
}

/***************************************************************************
* FuncName     : cmp_prot_can_dev_create()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : SSY         Version:    V1.0      Date:    2024/6/18
***************************************************************************/
static int cmp_ble_dev_create(void)
{
    
    cmp_ble_pub_dev_init();

    cmp_ble_create();

    return 0;

}
INIT_ENV_EXPORT(cmp_ble_dev_create);
