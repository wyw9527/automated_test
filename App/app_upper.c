#include "app_upper.h"
// #include "SEGGER_RTT_Conf.h"
// #include "SEGGER_RTT.h"
// #include <stdio.h>

#define CHRG_QUE_PEND_TMOUT 9
#define CHRG_PAUSE_TMOUT 1800000

static void app_upper_init(void);
static void app_upper_work(uint8_t period);
// static void app_upper_procc_serv_mb_msg(void *msg);

static app_upper_stu upper_conf;

/***************************************************************************************
 * @Function    : app_upper()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : SSY            Version   : V3.0.0                Date:2024/6/20
 ****************************************************************************************/
void app_upper(void *parameter)
{
    uint8_t *p_msg;
    uint32_t tick_rec, tick_curr = 0, qmout = CHRG_QUE_PEND_TMOUT, tm_accu = 0;

    app_upper_init();

    while (1)
    {
        tick_rec = rt_tick_get();
        if (RT_EOK == rt_mb_recv(&mb_sys, (rt_ubase_t *)&p_msg, qmout))
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
            if (tm_accu >= CHRG_QUE_PEND_TMOUT)
            {
                tm_accu = 0;

                app_upper_work(CHRG_QUE_PEND_TMOUT);
            }
            qmout = CHRG_QUE_PEND_TMOUT - tm_accu;
        }
        else
        {
            app_upper_work(CHRG_QUE_PEND_TMOUT);

            qmout = CHRG_QUE_PEND_TMOUT;

            tm_accu = 0;
        }
    }
}


/***************************************************************************************
 * @Function    : app_upper_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : SSY            Version   : V3.0.0                Date:2024/6/20
 ****************************************************************************************/
static void app_upper_init()
{
    upper_conf.upper = cmp_dev_find(CMP_DEV_NAME_PROT_UPPER);
    upper_conf.w5500 = cmp_dev_find(CMP_DEV_NAME_W5500);
    if (NULL == upper_conf.upper || NULL == upper_conf.w5500)
    {
        return;
    }
    
}

// static void ble_test()
// {
//     static uint8_t test = 0;
//     if(test == 1)
//     {
//         cmp_prot_upper_to_server_msg_conn_ble_stu ble_conn_msg;
//         int8_t ble_name[28] = {0x36, 0x30, 0x41, 0x4C, 0x44, 0x33, 0x32, 0x4B, 0x34, 0x33, 0x32, 0x57, 0x30, 0x31, 0x37, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//         memcpy(ble_conn_msg.ble_name, ble_name, 28);
//         ble_conn_msg.conn_flag = 0x01;

//         upper_conf.upper->cmp_prot_upper_set_msg(0x01, (uint8_t *)&ble_conn_msg, 29);
//         test = 0;
//     }
//     else if(test == 2)
//     {
//         cmp_prot_upper_to_server_msg_login_stu login_res_msg;
//         login_res_msg.login_res = 0x00;
//         login_res_msg.login_interval = 0x0A;

//         upper_conf.upper->cmp_prot_upper_set_msg(0x02, (uint8_t *)&login_res_msg, 2);
//         test = 0;
//     }
// }

static void net_hnd_w5500_rx_msg()
{        
    memset(upper_conf.w5500_comm.rx_buff,0,sizeof(upper_conf.w5500_comm.rx_buff));
    upper_conf.w5500_comm.rx_len = upper_conf.w5500->func_rx_msg((uint8_t*)upper_conf.w5500_comm.rx_buff,sizeof(upper_conf.w5500_comm.rx_buff));
    if(upper_conf.w5500_comm.rx_len  < 2)
    {
        return;
    }
    
    // upper_conf.w5500->func_rx_msg_hnd((uint8_t*)upper_conf.w5500_comm.rx_buff,upper_conf.w5500_comm.rx_len);

    // upper_conf.upper->cmp_prot_upper_set_msg(upper_conf.w5500_comm.rx_buff, upper_conf.w5500_comm.rx_len);

}

/***************************************************************************************
* @Function    : net_dev_work()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V2.0.0                Date:2023/2/28
****************************************************************************************/
static void net_dev_work()
{
    upper_conf.w5500->func_dev_work();
}

/***************************************************************************************
 * @Function    : app_upper_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : SSY            Version   : V3.0.0                Date:2024/6/20
 ****************************************************************************************/
static void app_upper_work(uint8_t period)
{
    net_hnd_w5500_rx_msg();
        
    net_dev_work();
}
