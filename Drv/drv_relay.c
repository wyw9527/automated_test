#include "drv_relay.h"


#define DRV_RELAY_ALARM_DELAY 1000
#define DRV_RELAY_ACT_TMOUT 1000
#define DRV_RELAY_ACT_REMAIN 100

#define DRV_RELAY_ELOCK_FB_TYPE 0

#define DRV_RELAY_TIMER_TICK 1

static drv_relay_pub_dev_stu relay_pub;

static drv_relay_stu elock_conf[BSP_POLE_PLUG_NUM];
static drv_relay_stu conn_conf[BSP_POLE_PLUG_NUM];

static drv_relay_tphase_stu tphase_conn_io[BSP_POLE_PLUG_NUM] =
    {
        {
            BSP_SOURCE_DO_RELAY_CTRL1,
            BSP_SOURCE_DO_RELAY_CTRL2,
            BSP_SOURCE_DI_RELAY_STA1,
            BSP_SOURCE_DI_RELAY_STA2,
            BSP_SOURCE_DO_RELAY_PWR,
        }};

static drv_relay_io_stu elock_io[BSP_POLE_PLUG_NUM] =
    {
        {BSP_SOURCE_DO_ELOCK_CTRL1, BSP_SOURCE_DO_ELOCK_PWR, BSP_SOURCE_DI_ELOCK_STA1},
};

/***************************************************************************
*@Function    :drv_relay_elock_io_init
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-11-03
***************************************************************************/
static void drv_relay_elock_io_init(drv_relay_io_stu *relay, drv_relay_stu *elock)
{
    relay_pub.relay_io->bsp_io_init(relay->io_ctrl);
    relay_pub.relay_io->bsp_io_init(relay->io_pwr);
    relay_pub.relay_io->bsp_io_init(relay->io_sta);

    elock->param.relay_fb_type = ELOCK_FB_H;
    elock->param.chrg_type = CHRG_TYPE;
}

/***************************************************************************
*@Function    :drv_telay_tphase_io_init
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-11-03
***************************************************************************/
static void drv_telay_tphase_io_init(drv_relay_tphase_stu *tphase_io, drv_relay_stu *conf)
{
    relay_pub.relay_io->bsp_io_init(tphase_io->ctrl_l1);
    relay_pub.relay_io->bsp_io_init(tphase_io->ctrl_l2);
    relay_pub.relay_io->bsp_io_init(tphase_io->sta_l1);
    relay_pub.relay_io->bsp_io_init(tphase_io->sta_l2);
    relay_pub.relay_io->bsp_io_init(tphase_io->pwr);

    relay_pub.relay_io->bsp_io_init(BSP_SOURCE_DO_AC_LEAK_PROD);

    conf->param.relay_fb_type = CONN_FB_TYPE;
    conf->param.relay_act_type = CONN_ACT_TYPE;
}

/***************************************************************************************
 * @Function    : drv_relay_off()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/29
 ****************************************************************************************/
static void drv_relay_off(drv_relay_stu *relay)
{
    relay->procc.step = 0;
    relay->procc.cmd_flag = 1;

    relay->procc.cmp_type = 0;
}

/***************************************************************************************
 * @Function    : drv_relay_on()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/29
 ****************************************************************************************/
static void drv_relay_on(drv_relay_stu *relay)
{
    relay->procc.step = 0;

    relay->procc.cmp_type = 1;

    relay->procc.act_count++;

    relay->procc.cmd_flag = 1;
}

/***************************************************************************
*@Function    :drv_relay_tphase_act_norm_work
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-11-03
***************************************************************************/
static void drv_relay_tphase_act_norm_work(drv_relay_stu *relay, drv_relay_tphase_stu *io_conf, uint8_t period)
{
    if (relay->procc.cmd_flag == 0)
        return;

    if (relay->procc.step == 0)
    {
        if (relay->procc.cmp_type == 1)
        {
            relay_pub.relay_io->bsp_io_set(io_conf->ctrl_l1);
            relay_pub.relay_io->bsp_io_set(io_conf->ctrl_l2);
            relay_pub.relay_io->bsp_io_set(io_conf->pwr);
        }
        else
        {
            relay_pub.relay_io->bsp_io_reset(io_conf->ctrl_l1);
            relay_pub.relay_io->bsp_io_reset(io_conf->ctrl_l2);
        }

        relay->procc.count = 0;
        relay->procc.act_flag = 0;
        relay->procc.step++;
    }
    else if (relay->procc.step == 1)
    {
        if (relay->procc.cmp_type == relay->procc.fb_state && relay->procc.count >= DRV_RELAY_ACT_REMAIN)
        {
            relay->procc.step++;
            return;
        }
        relay->procc.count += period;
        if (relay->procc.count > DRV_RELAY_ACT_TMOUT)
        {
            relay->procc.step++;
            return;
        }
    }
    else if (relay->procc.step == 2)
    {
        relay_pub.relay_io->bsp_io_reset(io_conf->pwr);
        relay->procc.cmd_flag = 0;
    }
}

/***************************************************************************
*@Function    :drv_relay_thase_act_work
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-11-03
***************************************************************************/
static void drv_relay_thase_act_work(drv_relay_stu *relay, drv_relay_tphase_stu *io_conf, uint8_t period)
{
    drv_relay_tphase_act_norm_work(relay, io_conf, period);
}

/***************************************************************************
*@Function    :drv_relay_thase_state_work
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-11-03
***************************************************************************/
static void drv_relay_thase_state_work(drv_relay_stu *relay, drv_relay_tphase_stu *io_conf)
{
    uint8_t di;

    uint8_t l1_di = relay_pub.relay_io->bsp_io_get_value(io_conf->sta_l1);
    uint8_t l2_di = relay_pub.relay_io->bsp_io_get_value(io_conf->sta_l2);

    if ((l1_di && l2_di) == 1)
    {
        di = 1;
    }
    else if ((l1_di || l2_di) == 0)
    {
        di = 0;
    }
    else
    {
        if (relay->procc.cmd_flag == 0)
        {
            relay->procc.fb_state = relay->procc.cmp_type == 0 ? 1 : 0;
            return;
        }
    }

    relay->procc.fb_state = (di == relay->param.relay_fb_type) ? 1 : 0;
}

/***************************************************************************************
 * @Function    : drv_relay_status_work()
 *
 * @Param       :
 *
 * @Return      : 0 无变化  1 告警恢复  2 告警产生
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/29
 ****************************************************************************************/
static uint8_t drv_relay_status_work(drv_relay_stu *relay, uint8_t period)
{
    if (relay->procc.cmp_type == relay->procc.fb_state)
    {
        relay->procc.fault_count = 0;
        if (relay->procc.relay_alarm > 0)
        {
            relay->procc.relay_alarm = 0;
            return 1;
        }
        return 0;
    }

    if (relay->procc.relay_alarm > 0)
    {
        return 0;
    }

    relay->procc.fault_count += period;
    if (relay->procc.fault_count > DRV_RELAY_ALARM_DELAY)
    {
        relay->procc.fault_count++;
        relay->procc.relay_alarm = 1;
        return 2;
    }
    return 0;
}

/***************************************************************************
*@Function    :drv_relay_tphare_work
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-11-03
***************************************************************************/
static uint8_t drv_relay_tphare_work(drv_relay_stu *relay, drv_relay_tphase_stu *io_conf, uint8_t period)
{
    drv_relay_thase_state_work(relay, io_conf);

    drv_relay_thase_act_work(relay, io_conf, period);

    return drv_relay_status_work(relay, period);
}

/***************************************************************************************
 * @Function    : drv_relay_off0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/29
 ****************************************************************************************/
static void drv_relay_contactor_off0(void)
{
    drv_relay_off(&conn_conf[0]);
}

/***************************************************************************************
 * @Function    : drv_relay_contactor_on0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/29
 ****************************************************************************************/
static void drv_relay_contactor_on0(void)
{
    drv_relay_on(&conn_conf[0]);
}

/***************************************************************************************
 * @Function    : drv_relay_contactor_work0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/29
 ****************************************************************************************/
static void drv_relay_contactor_work0(uint8_t period)
{
    uint8_t ret;
    static uint16_t count = 0;
    drv_relay_stu *relay = &conn_conf[0];

    ret = drv_relay_tphare_work(&conn_conf[0], &tphase_conn_io[0], period);

    if (ret == 2)
    {
        if (count < 3000)
        {
            count += period;
            return;
        }
        count = 0;
        if (relay->judg_hook_func(2) < 700)
            return;
        if (relay->judg_hook_func(1) < 700)
            return;
        if (relay->judg_hook_func(0) < 700)
            return;
    }
    if (ret == 1)
    {
        bsp_alarm_mb_send(0, 4, 0, 0);
    }
    else if (ret == 2)
    {
        if (relay->procc.cmp_type == 0)
        {
            if (conn_conf[0].callback_func != RT_NULL)
            {
                conn_conf[0].callback_func();
            }
        }

        bsp_alarm_mb_send(0, 4, 0, 2);
    }
}

/***************************************************************************
*@Function    :
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2024-01-04
***************************************************************************/
// void drv_relay_fault_handler_func()
// {
//     relay_pub.relay_io->bsp_io_reset(tphase_conn_io[0].ctrl_l1);
//     relay_pub.relay_io->bsp_io_reset(tphase_conn_io[0].ctrl_l2);
// }

/***************************************************************************
*@Function    :drv_relay_bonding_register_func
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-11-08
***************************************************************************/
static void drv_relay_bonding_register_func(func_relay_bond_handler callback_func)
{
    conn_conf[0].callback_func = callback_func;
}

/***************************************************************************
*@Function    :drv_relay_judgment_func
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-12-28
***************************************************************************/
static void drv_relay_judgment_func(func_relay_judg_other_hook callback_func)
{
    conn_conf[0].judg_hook_func = callback_func;
}

/***************************************************************************
*@Function    :func_relay_bond_handler
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-11-09
***************************************************************************/
static void drv_relay_bond_handler()
{
    relay_pub.relay_io->bsp_io_set(BSP_SOURCE_DO_AC_LEAK_PROD);
}

/***************************************************************************************
 * @Function    : drv_relay_contator_local_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/24
 ****************************************************************************************/
static void drv_relay_contator_local_create(void)
{
    static drv_relay_dev_stu contactor[BSP_POLE_PLUG_NUM];

    contactor[0].drv_relay_off = drv_relay_contactor_off0;
    contactor[0].drv_relay_on = drv_relay_contactor_on0;
    contactor[0].drv_relay_work = drv_relay_contactor_work0;
    contactor[0].drv_relay_bonding_register = drv_relay_bonding_register_func;
    contactor[0].drv_relay_judgment = drv_relay_judgment_func;

    contactor[0].drv_relay_fault_handler = NULL;

    contactor[0].drv_relay_bonding_register(drv_relay_bond_handler);

    drv_dev_append(DRV_DEV_RELAY_CONTACTOR, contactor);
}

/***************************************************************************************
 * @Function    : drv_relay_contator_oper_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/24
 ****************************************************************************************/
static void drv_relay_contator_oper_init(void)
{
    bsp_bind_oper_data_dev(0, 4, 3, &conn_conf[0].procc.fb_state, 1);
    bsp_bind_oper_data_dev(0, 4, 2, (uint8_t *)&conn_conf[0].procc.fault_count, 2);
    bsp_bind_oper_data_dev(0, 4, 1, (uint8_t *)&conn_conf[0].procc.act_count, 2);
}

/***************************************************************************************
 * @Function    : drv_relay_contator_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/24
 ****************************************************************************************/
static void drv_relay_contator_dev_create(void)
{
    drv_telay_tphase_io_init(&tphase_conn_io[0], &conn_conf[0]);

    drv_relay_contator_local_create();

    drv_relay_contator_oper_init();

    conn_conf[0].procc.cmd_flag = 0;
}

/***************************************************************************************
 * @Function    : drv_relay_elock_a_off()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/30
 ****************************************************************************************/
static void drv_relay_elock_a_off(void)
{
    elock_conf[0].procc.cmp_type = 0;

    drv_relay_off(&elock_conf[0]);
}

/***************************************************************************************
 * @Function    : drv_relay_elock_on0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/30
 ****************************************************************************************/
static void drv_relay_elock_on0(void)
{
    elock_conf[0].procc.cmp_type = 1;

    drv_relay_on(&elock_conf[0]);
}

/***************************************************************************************
 * @Function    : drv_relay_elock_work0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/30
 ****************************************************************************************/
static void drv_relay_elock_work0(uint8_t period)
{
    uint8_t ret;
    if (elock_conf[0].param.chrg_type == CHRG_PLUG)
    {
        return;
    }
    ret = 0;
    if (ret == 1)
    {
        bsp_alarm_mb_send(0, 7, 0, 0);
    }
    else if (ret == 2)
    {
        bsp_alarm_mb_send(0, 7, 0, 2);
    }
}

/***************************************************************************************
 * @Function    : drv_relay_elock_local_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/24
 ****************************************************************************************/
static void drv_relay_elock_local_create(void)
{
    static drv_relay_dev_stu elock[BSP_POLE_PLUG_NUM];

    elock[0].drv_relay_off = drv_relay_elock_a_off;
    elock[0].drv_relay_on = drv_relay_elock_on0;
    elock[0].drv_relay_work = drv_relay_elock_work0;

    drv_dev_append(DRV_DEV_RELAY_ELOCK, elock);
}

/***************************************************************************************
 * @Function    : drv_relay_elock_oper_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/24
 ****************************************************************************************/
static void drv_relay_elock_oper_init(void)
{
    bsp_bind_oper_data_dev(0, 7, 3, &conn_conf[0].procc.fb_state, 1);
    bsp_bind_oper_data_dev(0, 7, 2, (uint8_t *)&conn_conf[0].procc.fault_count, 2);
    bsp_bind_oper_data_dev(0, 7, 1, (uint8_t *)&conn_conf[0].procc.act_count, 2);
}

/***************************************************************************************
 * @Function    : drv_relay_elock_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/24
 ****************************************************************************************/
static void drv_relay_elock_dev_create(void)
{
    drv_relay_elock_io_init(&elock_io[0], &elock_conf[0]);

    drv_relay_elock_local_create();

    drv_relay_elock_oper_init();
}

/***************************************************************************************
 * @Function    : drv_relay_pub_dev_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/24
 ****************************************************************************************/
static void drv_relay_pub_dev_init(void)
{
    relay_pub.relay_io = bsp_dev_find(BSP_DEV_NAME_IO);
}

/***************************************************************************************
 * @Function    : drv_relay_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/29
 ****************************************************************************************/
static int drv_relay_dev_create(void)
{
    drv_relay_pub_dev_init();

    drv_relay_contator_dev_create();

    drv_relay_elock_dev_create();

    return 0;
}
INIT_COMPONENT_EXPORT(drv_relay_dev_create);

