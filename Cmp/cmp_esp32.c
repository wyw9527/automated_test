#include "cmp_esp32.h"
#include "SEGGER_RTT.h"

static cmp_esp32_stu esp32_conf;

/***************************************************************************************
 * @Function    : cmp_esp32_rx_dst_rsp()
 *
 * @Param       :
 *
 * @Return      : 0 no msg  1 succ  >1 fail reason
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/5
 ****************************************************************************************/
static uint8_t cmp_esp32_rx_dst_rsp(const char *dst, char *err_dst)
{
    uint8_t len, res;
    drv_serial_rx_stu rx_msg;

    if (esp32_conf.serial->dev_get_msg(&rx_msg, 0) == RT_EOK)
    {
        if (rx_msg.len > 0)
        {
            if (strstr((char *)rx_msg.buff, dst) != RT_NULL)
            {
                return 1;
            }

            if ((err_dst != RT_NULL) && ((err_dst = strstr((char *)rx_msg.buff, err_dst)) != RT_NULL))
            {
                len = strlen(err_dst);
                res = atoi(&err_dst[len]);
                if (res > 0 && res < 5)
                {
                    return res + 1;
                }
                return 6;
            }
            if (strstr((char *)rx_msg.buff, "ERROR") != RT_NULL)
            {
                return 0xff;
            }
        }
    }
    return 0;
}

/***************************************************************************************
 * @Function    : cmp_esp32_send_cmd()
 *
 * @Param       :
 *
 * @Return      : 0:SUCC 1 TMOUT  >=2 fail(0xfe:error)
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/5
 ****************************************************************************************/
static uint8_t cmp_esp32_send_cmd(char *cmd, char *resp, char *err_dst, uint16_t tmout_ms)
{
    uint8_t res;
    uint16_t delay_count = 0;

    if (cmd != RT_NULL)
    {
        esp32_conf.serial->dev_send_msg((uint8_t *)cmd, strlen(cmd));
        esp32_conf.serial->dev_rx_buff_clear();
    }

    while (delay_count < tmout_ms)
    {
        if ((res = cmp_esp32_rx_dst_rsp(resp, err_dst)) > 0)
        {
            return (res - 1);
        }
        delay_count += 50;
        rt_thread_mdelay(50);
    }

    return 1;
}

/***************************************************************************************
 * @Function    : cmp_esp32_hardware_reset()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/5
 ****************************************************************************************/
static uint8_t cmp_esp32_hardware_reset(void)
{
    rt_thread_mdelay(500);
    esp32_conf.reset_io->drv_do_off();
    rt_thread_mdelay(2000);
    esp32_conf.reset_io->drv_do_on();
    rt_thread_mdelay(500);

    return cmp_esp32_send_cmd(RT_NULL, "ready", RT_NULL, 4000);
}

/***************************************************************************************
 * @Function    : cmp_esp32_mod_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/5
 ****************************************************************************************/
static uint8_t cmp_esp32_mod_init(void)
{
    uint8_t ret = 0;

    if ((ret = cmp_esp32_send_cmd("ATE0\r\n", "OK", RT_NULL, 500)) > 0)
    {
        return ret;
    }

    if ((ret = cmp_esp32_send_cmd("AT+SYSMSG=5\r\n", "OK", RT_NULL, 500)) > 0)
    {
        return ret;
    }

    return ret;
}

/***************************************************************************************
 * @Function    : cmp_esp32_wifi_conn()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/5
 ****************************************************************************************/
static uint8_t cmp_esp32_wifi_conn(void)
{
    uint8_t ret = 0;
    char *buff_p;
    char send_buff[96];
    char *err_str = "+CWJAP:";

    if ((ret = cmp_esp32_send_cmd("AT+CWMODE=1,0\r\n", "OK", RT_NULL, 500)) > 0)
    {
        return ret;
    }

    buff_p = send_buff;
    memset(buff_p, 0, sizeof(send_buff));
    strcpy(buff_p, "AT+CWJAP=\"");
    strncat(buff_p, esp32_conf.param.ssid, 32);
    strcat(buff_p, "\",\"");
    strncat(buff_p, esp32_conf.param.pwd, 32);
    strcat(buff_p, "\",,,,,,5\r\n");

    return cmp_esp32_send_cmd(send_buff, "WIFI GOT IP", err_str, 10000);
}

/***************************************************************************************
 * @Function    : cmp_esp32_serv_conn()
 *
 * @Param       :
 *
 * @Return      : 1 CIPSTART tmout 2 CIPSTART error 3 CIPMODE tmout  4 CIPSEND tmout
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/5
 ****************************************************************************************/
static uint8_t cmp_esp32_serv_conn(void)
{
    uint8_t ret = 0;
    char *buff_p;
    char send_buff[96];

    memset(send_buff, 0, sizeof(send_buff));
    buff_p = send_buff;
    strcpy(buff_p, "AT+CIPSTART=\"TCP\",\"");
    strncat(buff_p, esp32_conf.param.serv_addr, 48);
    sprintf(buff_p, "%s\",%d\r\n", send_buff, esp32_conf.param.port);

    if ((ret = cmp_esp32_send_cmd(send_buff, "CONNECT\r\n", RT_NULL, 3000)) > 0)
    {
        return ret;
    }

    if ((ret = cmp_esp32_send_cmd("AT+CIPMODE=1\r\n", "OK", RT_NULL, 500)) > 0)
    {
        return ret;
    }

    return cmp_esp32_send_cmd("AT+CIPSEND\r\n", "OK", RT_NULL, 500);
}

/***************************************************************************************
 * @Function    : cmp_esp32_ble_config()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/5
 ****************************************************************************************/
static uint8_t cmp_esp32_ble_config(void)
{
    uint8_t ret = 0;
    char *buff_p;
    char send_buff[96];

    memset(send_buff, 0, sizeof(send_buff));
    if ((ret = cmp_esp32_send_cmd("AT+BLEINIT=2\r\n", "OK", RT_NULL, 500)) > 0)
    {
        return ret;
    }

    if ((ret = cmp_esp32_send_cmd("AT+BLEGATTSSRVCRE\r\n", "OK", RT_NULL, 500)) > 0)
    {
        return ret;
    }

    if ((ret = cmp_esp32_send_cmd("AT+BLEGATTSSRVSTART=1\r\n", "OK", RT_NULL, 500)) > 0)
    {
        return ret;
    }

    buff_p = send_buff;
    strcpy(buff_p, "AT+BLEADVDATAEX=\"");
    strncat(buff_p, esp32_conf.param.pole_sn, 28);
    strcat(buff_p, "\",\"A002\",\"020102\",1\r\n");
    if ((ret = cmp_esp32_send_cmd(send_buff, "OK", RT_NULL, 500)) > 0)
    {
        return ret;
    }

    return cmp_esp32_send_cmd("AT+BLEADVSTART\r\n", "OK", RT_NULL, 500);
}

/***************************************************************************************
 * @Function    : cmp_esp32_ble_conn_query()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/7
 ****************************************************************************************/
static uint8_t cmp_esp32_ble_conn_query(void)
{
    return cmp_esp32_send_cmd("AT+BLECONN?\r\n", "+BLECONN:0,", RT_NULL, 1000);
}

/***************************************************************************************
 * @Function    : cmp_esp32_ble_conv_spp()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/5
 ****************************************************************************************/
static uint8_t cmp_esp32_ble_conv_spp(void)
{
    uint8_t ret = 0;

    if ((ret = cmp_esp32_send_cmd("AT+BLECONNPARAM=0,24,24,0,600\r\n", "OK", RT_NULL, 200)) > 0)
    {
        return ret;
    }

    if ((ret = cmp_esp32_send_cmd("AT+BLESPPCFG=1,1,6,1,5,0\r\n", "OK", RT_NULL, 200)) > 0)
    {
        return ret;
    }

    cmp_esp32_send_cmd(RT_NULL, "+WRITE:0,1,6,1,2,", RT_NULL, 2000);

    return cmp_esp32_send_cmd("AT+BLESPP\r\n", "OK", RT_NULL, 200);
}

/***************************************************************************************
 * @Function    : cmp_esp32_wifi_exit_spp()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/5
 ****************************************************************************************/
static uint8_t cmp_esp32_wifi_exit_spp(void)
{
    rt_thread_mdelay(30);
    esp32_conf.serial->dev_send_msg("+++", strlen("+++"));
    rt_thread_mdelay(1030);

    return cmp_esp32_send_cmd("AT+CIPCLOSE\r\n", "CLOSED", RT_NULL, 200);
}

/***************************************************************************************
 * @Function    : cmp_esp32_ble_exit_spp()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/5
 ****************************************************************************************/
static void cmp_esp32_ble_exit_spp(void)
{
    rt_thread_mdelay(1000);
    esp32_conf.serial->dev_send_msg("+++", strlen("+++"));
}

/***************************************************************************************
 * @Function    : cmp_esp32_param_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/5
 ****************************************************************************************/
static void cmp_esp32_param_init(void)
{
    memset((uint8_t *)&esp32_conf.param, 0, sizeof(esp32_conf));

    esp32_conf.serial = RT_NULL;
}

/***************************************************************************************
 * @Function    : cmp_esp32_dido_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/5
 ****************************************************************************************/
static void cmp_esp32_dido_init(void)
{
    esp32_conf.reset_io = drv_dev_find(DRV_DEV_DIDO_ESP_RESET);
}

/***************************************************************************************
 * @Function    : cmp_esp32_set_param()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/5
 ****************************************************************************************/
static void cmp_esp32_set_param(cmp_esp32_param_stu *param)
{
    memcpy((uint8_t *)&esp32_conf.param, (uint8_t *)param, sizeof(cmp_esp32_param_stu));

    memset((uint8_t *)&esp32_conf.procc, 0, sizeof(esp32_conf.procc));

    if (strlen(esp32_conf.param.ssid) > 0 && strlen(esp32_conf.param.pwd) > 0 && strlen(esp32_conf.param.serv_addr) > 0)
    {
        esp32_conf.procc.wifi_state = 1;
    }

    if (strlen(esp32_conf.param.pole_sn) > 0)
    {
        esp32_conf.procc.ble_state = 1;
    }
}

/***************************************************************************************
 * @Function    : cmp_esp32_work_mod_reset()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/6
 ****************************************************************************************/
static void cmp_esp32_work_mod_reset(void)
{
    if (cmp_esp32_hardware_reset() == 0)
    {
        esp32_conf.procc.step++;
        return;
    }

    esp32_conf.status.mod = ESP_MOD_STATE_RESET_ABN;
}

/***************************************************************************************
 * @Function    : cmp_esp32_work_mod_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description : wifi 参数已经配置则进入连接路由器，否则进入蓝牙配置
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/6
 ****************************************************************************************/
static void cmp_esp32_work_mod_init(void)
{
    uint8_t ret;

    if ((ret = cmp_esp32_mod_init()) == 0)
    {
        esp32_conf.status.mod = ESP_MOD_STATE_OK;
        esp32_conf.status.router = ESP_ROUTER_STATE_PARAM_INIT;
        esp32_conf.procc.cmd_send_cnt = 0;
        if (esp32_conf.procc.wifi_state > 0)
        {
            esp32_conf.procc.step = ESP_WORK_STEP_WIFI_CONN;
        }
        else
        {
            esp32_conf.procc.step = ESP_WORK_STEP_BLE_CONFIG;
        }
        return;
    }

    /* ERROR */
    if (ret == 0xFE)
    {
        esp32_conf.status.mod = ESP_MOD_STATE_INIT_ABN;
        esp32_conf.procc.step = ESP_WORK_STEP_MOD_RESET;
        return;
    }

    /* 超时3次重置 */
    if (++esp32_conf.procc.cmd_send_cnt > 3)
    {
        esp32_conf.status.mod = ESP_MOD_STATE_INIT_ABN;
        esp32_conf.procc.step = ESP_WORK_STEP_MOD_RESET;
    }
}

/***************************************************************************************
 * @Function    : cmp_esp32_work_wifi_conn()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/6
 ****************************************************************************************/
static void cmp_esp32_work_wifi_conn(void)
{
    uint8_t ret;

    // SUCC
    if (++esp32_conf.procc.wifi_conn_cnt > 10)
    {
        esp32_conf.procc.wifi_conn_cnt = 10;
    }
    if ((ret = cmp_esp32_wifi_conn()) == 0)
    {
        esp32_conf.status.router = ESP_ROUTER_STATE_OK;
        esp32_conf.status.wifi_use = CMP_ESP_MOUDLE_ATE_USEFUL;
        esp32_conf.status.server = ESP_SERV_STATE_WAIT_INIT;
        esp32_conf.procc.cmd_send_cnt = 0;
        esp32_conf.procc.step++;
        return;
    }

    /* ERROR */
    if (ret == 0xFE)
    {
        esp32_conf.status.router = ESP_ROUTER_STATE_CONF_ERR;
        esp32_conf.procc.step = ESP_WORK_STEP_MOD_RESET;
        return;
    }

    /* MSG TMOUT */
    if (ret == 0x01 || ret == 0x02)
    {
        if (++esp32_conf.procc.cmd_send_cnt <= 3)
            return;
        esp32_conf.status.router = ESP_ROUTER_STATE_CONN_TMOUT;
        esp32_conf.procc.step = ESP_WORK_STEP_MOD_RESET;
        return;
    }

    /* OTHER REASON */
    switch (ret)
    {
    case 0x03:
        esp32_conf.status.router = ESP_ROUTER_STATE_PWD_ERR;
        break;
    case 0x04:
        esp32_conf.status.router = ESP_ROUTER_STATE_SSID_ERR;
        break;
    case 0x05:
        esp32_conf.status.router = ESP_ROUTER_STATE_CONN_FAIL;
        break;
    default:
        esp32_conf.status.router = ESP_ROUTER_STATE_OTH_ABN;
        break;
    }

    esp32_conf.procc.step = ESP_WORK_STEP_BLE_CONFIG;
    esp32_conf.procc.cmd_send_cnt = 0;
}

/***************************************************************************************
 * @Function    : cmp_esp32_work_serv_conn()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/7
 ****************************************************************************************/
static void cmp_esp32_work_serv_conn(void)
{
    uint8_t ret;

    /* SUCC */
    if ((ret = cmp_esp32_serv_conn()) == 0)
    {
        esp32_conf.procc.wifi_conn_cnt = 0;
        esp32_conf.procc.cmd_send_cnt = 0;
        esp32_conf.status.server = ESP_SERV_STATE_OK;
        esp32_conf.procc.step = ESP_WORK_STEP_SPP;
        esp32_conf.procc.wifi_state = 2;
        return;
    }

    /* ERROR or TMOUT: TO BLE */
    if (ret == 0xFE || (++esp32_conf.procc.cmd_send_cnt > 3))
    {
        esp32_conf.procc.cmd_send_cnt = 0;
        esp32_conf.status.server = ESP_SERV_STATE_CONN_FAIL;
        esp32_conf.procc.step = ESP_WORK_STEP_BLE_CONFIG;
    }
}

/***************************************************************************************
 * @Function    : cmp_esp32_work_ble_config()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/7
 ****************************************************************************************/
static void cmp_esp32_work_ble_config(void)
{
    uint8_t ret;

    if ((ret = cmp_esp32_ble_config()) == 0)
    {
        esp32_conf.procc.cmd_send_cnt = 0;
        esp32_conf.status.ble = ESP_BLE_STATE_WAIT_CONN;
        esp32_conf.procc.step = ESP_WORK_STEP_WAIT;
        return;
    }

    // ERROR
    if (ret == 0xFE || (++esp32_conf.procc.cmd_send_cnt > 3))
    {
        esp32_conf.procc.cmd_send_cnt = 0;
        esp32_conf.status.ble = ESP_BLE_STATE_CONF_ABN;
        esp32_conf.procc.step = ESP_WORK_STEP_MOD_RESET;
        return;
    }
}

/***************************************************************************************
 * @Function    : cmp_esp32_work_wifi_reconn()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/28
 ****************************************************************************************/
static void cmp_esp32_work_wifi_reconn(void)
{
    uint16_t reconn_period;

    if (esp32_conf.procc.wifi_state == 0)
        return;

    if (esp32_conf.procc.wifi_state == 1)
    {
        reconn_period = 30 * esp32_conf.procc.wifi_conn_cnt;
        if (++esp32_conf.procc.wait_count > reconn_period)
        {
            esp32_conf.procc.wait_count = 0;
            esp32_conf.procc.step = ESP_WORK_STEP_MOD_RESET;
        }
        return;
    }

    if (++esp32_conf.procc.wait_count > 30)
    {
        esp32_conf.procc.wait_count = 0;
        esp32_conf.procc.step = ESP_WORK_STEP_MOD_RESET;
    }
}

/***************************************************************************************
 * @Function    : cmp_esp32_work_wait()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/7
 ****************************************************************************************/
static void cmp_esp32_work_wait(void)
{
    uint8_t ret;

    if ((ret = cmp_esp32_ble_conn_query()) == 0)
    {
        esp32_conf.procc.wait_count = 0;
        esp32_conf.procc.step = ESP_WORK_STEP_BLE_CONV;
        return;
    }

    // ERROR
    if (ret == 0xFE)
    {
        esp32_conf.procc.cmd_send_cnt = 0;
        esp32_conf.status.ble = ESP_BLE_STATE_WAIT_CONN_ABN;
        esp32_conf.procc.step = ESP_WORK_STEP_MOD_RESET;
        return;
    }

    // TMOUT
    if (ret == 0x01 && (++esp32_conf.procc.cmd_send_cnt > 15))
    {
        esp32_conf.procc.cmd_send_cnt = 0;
        esp32_conf.status.ble = ESP_BLE_STATE_WAIT_TMOUT;
        esp32_conf.procc.step = ESP_WORK_STEP_MOD_RESET;
        return;
    }

    // hndle wifi reconnect
    cmp_esp32_work_wifi_reconn();

    esp32_conf.procc.cmd_send_cnt = 0;
}

/***************************************************************************************
 * @Function    : cmp_esp32_work_ble_conv()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/7
 ****************************************************************************************/
static void cmp_esp32_work_ble_conv(void)
{
    uint8_t ret;

    if ((ret = cmp_esp32_ble_conv_spp()) == 0)
    {
        esp32_conf.procc.wait_count = 0;
        esp32_conf.procc.cmd_send_cnt = 0;
        esp32_conf.status.ble = ESP_BLE_STATE_SPP;
        esp32_conf.status.ble_use = CMP_ESP_MOUDLE_ATE_USEFUL;
        esp32_conf.procc.step = ESP_WORK_STEP_SPP;
        return;
    }

    if (++esp32_conf.procc.cmd_send_cnt > 3)
    {
        esp32_conf.procc.cmd_send_cnt = 0;
        esp32_conf.status.ble = ESP_BLE_STATE_SPP_ABN;
        if (ret == 0xfe)
        {
            esp32_conf.procc.step = ESP_WORK_STEP_MOD_RESET;
        }
        else
        {
            esp32_conf.procc.step = ESP_WORK_STEP_WAIT;
        }
    }
}

/***************************************************************************************
 * @Function    : cmp_esp_work_spp_exit_check()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/7
 ****************************************************************************************/
static uint8_t cmp_esp_work_spp_exit_check(drv_serial_rx_stu *rx_msg)
{
    if (strstr((char *)rx_msg->buff, "+QUITT") != RT_NULL) // ble
    {
        cmp_esp32_ble_exit_spp();
        esp32_conf.procc.step = ESP_WORK_STEP_MOD_RESET;
        return 1;
    }

    if (strstr((char *)rx_msg->buff, "CLOSED") != RT_NULL) // wifi
    {
        cmp_esp32_wifi_exit_spp();
        esp32_conf.procc.step = ESP_WORK_STEP_SERV_CONN;
        return 1;
    }
    return 0;
}

/***************************************************************************************
 * @Function    : cmp_esp32_work_spp_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/7
 ****************************************************************************************/
static void cmp_esp32_work_spp_work(void)
{
    if (esp32_conf.serial->dev_get_msg(&esp32_conf.msg.rx_msg, 0) != RT_EOK)
        return;
    if (esp32_conf.msg.rx_msg.len > 0)
    {
        if (cmp_esp_work_spp_exit_check(&esp32_conf.msg.rx_msg) > 0)
            return;

        esp32_conf.msg.rx_msg_flag = 1;
    }
}

/***************************************************************************************
 * @Function    : cmp_esp32_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/6
 ****************************************************************************************/
static void cmp_esp32_work(uint8_t period)
{
    if (esp32_conf.procc.wifi_state == 0 && esp32_conf.procc.ble_state == 0)
    {
        return;
    }

    switch (esp32_conf.procc.step)
    {
    case ESP_WORK_STEP_MOD_RESET:
        cmp_esp32_work_mod_reset();
        break;

    case ESP_WORK_STEP_MOD_INIT:
        cmp_esp32_work_mod_init();
        break;

    case ESP_WORK_STEP_WIFI_CONN:
        cmp_esp32_work_wifi_conn();
        break;

    case ESP_WORK_STEP_SERV_CONN:
        cmp_esp32_work_serv_conn();
        break;

    case ESP_WORK_STEP_BLE_CONFIG:
        cmp_esp32_work_ble_config();
        break;

    case ESP_WORK_STEP_WAIT:
        cmp_esp32_work_wait();
        break;

    case ESP_WORK_STEP_BLE_CONV:
        cmp_esp32_work_ble_conv();
        break;

    case ESP_WORK_STEP_SPP:
        cmp_esp32_work_spp_work();
        break;
    }
}

/***************************************************************************************
 * @Function    : cmp_esp32_set_serial()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/8
 ****************************************************************************************/
static void cmp_esp32_set_serial(drv_serial_dev_stu *serial)
{
    esp32_conf.serial = serial;
}

/***************************************************************************************
 * @Function    : cmp_esp32_get_usabel_state()
 *
 * @Param       :
 *
 * @Return      : RT_EOK:find device   RT_EEMPTY:no device find
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/9
 ****************************************************************************************/
static uint8_t cmp_esp32_get_usabel_state()
{
    if (cmp_esp32_hardware_reset() == 0)
    {
        esp32_conf.procc.step = ESP_WORK_STEP_MOD_INIT;
        return RT_EOK;
    }

    return RT_EEMPTY;
}

/***************************************************************************************
 * @Function    : cmp_esp32_query_rx_app_msg()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/17
 ****************************************************************************************/
static uint8_t cmp_esp32_query_rx_app_msg(drv_serial_rx_stu *rx_msg)
{
    if (esp32_conf.msg.rx_msg_flag == 0)
        return 0;

    memcpy((uint8_t *)rx_msg, (uint8_t *)&esp32_conf.msg.rx_msg, sizeof(drv_serial_rx_stu));

    esp32_conf.msg.rx_msg_flag = 0;
    return 1;
}

/***************************************************************************************
 * @Function    : cmp_esp32_send_app_msg()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/17
 ****************************************************************************************/
static void cmp_esp32_send_app_msg(uint8_t *data, uint16_t len)
{
    if (esp32_conf.procc.step == ESP_WORK_STEP_SPP)
    {
        esp32_conf.serial->dev_send_msg(data, len);
    }
}

/***************************************************************************************
 * @Function    : cmp_esp32_get_spp_mode()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/26
 ****************************************************************************************/
static uint8_t cmp_esp32_get_spp_mode(void)
{
    if (esp32_conf.procc.step == ESP_WORK_STEP_SPP)
        return 1;

    return 0;
}

/***************************************************************************************
 * @Function    : cmp_esp32_dev_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/5
 ****************************************************************************************/
static void cmp_esp32_dev_init(void)
{
    static cmp_esp32_dev_stu esp32_dev;

    esp32_dev.esp32_set_param = cmp_esp32_set_param;
    esp32_dev.esp32_set_serial = cmp_esp32_set_serial;
    esp32_dev.esp32_work = cmp_esp32_work;
    esp32_dev.esp32_get_usable_state = cmp_esp32_get_usabel_state;
    esp32_dev.esp32_query_rx_app_msg = cmp_esp32_query_rx_app_msg;
    esp32_dev.esp32_send_app_msg = cmp_esp32_send_app_msg;
    esp32_dev.esp32_get_spp_mode = cmp_esp32_get_spp_mode;

    cmp_dev_append(CMP_DEV_NAME_ESP32, &esp32_dev);
}

/***************************************************************************************
 * @Function    : cmp_esp32_oper_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/8
 ****************************************************************************************/
static void cmp_esp32_oper_init(void)
{
    // 模块当前状态
    bsp_bind_oper_data_dev(0xff, 12, 0, (uint8_t *)&esp32_conf.procc.step, 1);

    // 模块初始化状态
    bsp_bind_oper_data_dev(0xff, 12, 1, (uint8_t *)&esp32_conf.status.mod, 1);

    // 路由状态
    bsp_bind_oper_data_dev(0xff, 12, 2, (uint8_t *)&esp32_conf.status.router, 1);

    // 服务器状态
    bsp_bind_oper_data_dev(0xff, 12, 3, (uint8_t *)&esp32_conf.status.server, 1);

    // 蓝牙状态
    bsp_bind_oper_data_dev(0xff, 12, 4, (uint8_t *)&esp32_conf.status.ble, 1);

    // ATE_BLE状态
    bsp_bind_oper_data_dev(0xff, 12, 5, (uint8_t *)&esp32_conf.status.ble_use, 1);

    // ATE_WIFI状态
    bsp_bind_oper_data_dev(0xff, 12, 6, (uint8_t *)&esp32_conf.status.wifi_use, 1);
}

/***************************************************************************************
 * @Function    : cmp_esp32_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/5
 ****************************************************************************************/
static int cmp_esp32_dev_create(void)
{
    cmp_esp32_param_init();

    cmp_esp32_dido_init();

    cmp_esp32_dev_init();

    cmp_esp32_oper_init();

    return 0;
}
INIT_ENV_EXPORT(cmp_esp32_dev_create);
