#include "cmp_plug.h"

static cmp_plug_stu plug_conf[BSP_POLE_PLUG_NUM];

/***************************************************************************************
 * @Function    : cmp_plug_set_chrg_curr0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/13
 ****************************************************************************************/
static void cmp_plug_set_chrg_curr0(uint16_t curr)
{
    uint16_t duty;
    if (plug_conf[0].cp == RT_NULL)
        return;
    duty = (uint16_t)(curr / 0.6);
    plug_conf[0].cp->cmp_set_pwm_duty(duty);
}

/***************************************************************************************
 * @Function    : cmp_plug_get_conn_sta0()
 *
 * @Param       :
 *
 * @Return      : 0 未连接  1 已连接
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/12
 ****************************************************************************************/
static uint8_t cmp_plug_get_conn_sta0(void)
{
    uint8_t cp_sta, cc_sta;

    if (plug_conf[0].cp == RT_NULL)
        return 0;

    cp_sta = plug_conf[0].cp->cmp_get_sta();
    if (plug_conf[0].param.sock_type > 0)
    {
        return cp_sta == 1 ? 0 : 1;
    }
    cc_sta = plug_conf[0].cc->cmp_get_sta();

    if (cc_sta == 0 || cp_sta < 2)
        return 0;

    return 1;
}

/***************************************************************************************
 * @Function    : cmp_plug_get_cp_sta0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/23
 ****************************************************************************************/
static uint8_t cmp_plug_get_cp_sta0(void)
{
    if (plug_conf[0].cp == RT_NULL)
        return 0;
    return plug_conf[0].cp->cmp_get_sta();
}

/***************************************************************************************
 * @Function    : cmp_plug_cpcc_sw0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/31
 ****************************************************************************************/
static void cmp_plug_cpcc_sw0(CMP_PLUG_CMD_TYPE cmd)
{
    if (plug_conf[0].cp_io_sw == RT_NULL)
        return;
    if (cmd == plug_conf[0].procc.cpcc_sta)
        return;
    plug_conf[0].procc.cpcc_sta = cmd;

    if (cmd == PLUG_CMD_ON)
    {
        plug_conf[0].cp_io_sw->drv_do_on();
    }
    else
    {
        plug_conf[0].cp_io_sw->drv_do_off();
    }
}

/***************************************************************************************
 * @Function    : cmp_plug_elock_ctrl0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/13
 ****************************************************************************************/
static void cmp_plug_elock_ctrl0(CMP_PLUG_CMD_TYPE cmd)
{

    if (plug_conf[0].elock == RT_NULL)
        return;
    if (cmd == PLUG_CMD_ON)
    {
        plug_conf[0].elock->drv_relay_on();
    }
    else
    {
        plug_conf[0].elock->drv_relay_off();
    }
}

/***************************************************************************************
 * @Function    : cmp_plug_contactor_ctrl0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/13
 ****************************************************************************************/
static void cmp_plug_contactor_ctrl0(CMP_PLUG_CMD_TYPE cmd)
{
    if (plug_conf[0].contactor == RT_NULL)
        return;
    if (cmd == PLUG_CMD_ON)
    {
        plug_conf[0].contactor->drv_relay_on();
    }
    else
    {
        plug_conf[0].contactor->drv_relay_off();
    }
}

/***************************************************************************
*@Function    :cmp_plug_relay_fault_handler_func
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2024-01-04
***************************************************************************/
static void cmp_plug_relay_fault_handler_func()
{
    if (plug_conf[0].contactor == RT_NULL)
        return;
    plug_conf[0].contactor->drv_relay_fault_handler();
}

/***************************************************************************************
 * @Function    : cmp_plug_stop_pwm_output0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/23
 ****************************************************************************************/
static void cmp_plug_stop_pwm_output0(void)
{
    if (plug_conf[0].cp == RT_NULL)
        return;
    plug_conf[0].cp->cmp_pwm_stop();
}

/***************************************************************************************
 * @Function    : cmp_plug_set_sock_type0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/23
 ****************************************************************************************/
static void cmp_plug_set_sock_type0(uint8_t type)
{
    if (type > 1)
        return;

    plug_conf[0].param.sock_type = type;
}

/***************************************************************************************
 * @Function    : cmp_plug_contactor_work0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/12
 ****************************************************************************************/
static void cmp_plug_contactor_work(cmp_plug_stu *plug, uint8_t period)
{
    if (plug->contactor == RT_NULL)
        return;
    plug->contactor->drv_relay_work(period);
}

/***************************************************************************************
 * @Function    : cmp_plug_elock_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/12
 ****************************************************************************************/
static void cmp_plug_elock_work(cmp_plug_stu *plug, uint8_t period)
{
    if (plug->param.sock_type > 0)
    {
        return;
    }
    if (plug->elock == RT_NULL)
    {
        return;
    }

    plug->elock->drv_relay_work(period);
}

/***************************************************************************************
 * @Function    : cmp_plug_cp_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/23
 ****************************************************************************************/
static void cmp_plug_cp_work(cmp_plug_stu *plug, uint8_t period)
{
    if (!CMP_PLUG_DEV_CPSW)
    {
        plug->cp->cmp_dev_work(period);
        return;
    }
    if (plug->procc.cpcc_sta == PLUG_CMD_OFF)
        return;
    if (plug->cp == RT_NULL)
        return;
    plug->cp->cmp_dev_work(period);
}

/***************************************************************************************
 * @Function    : cmp_plug_cc_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/23
 ****************************************************************************************/
static void cmp_plug_cc_work(cmp_plug_stu *plug)
{
    if (!CMP_PLUG_DEV_CPSW)
    {
        plug->cc->cmp_dev_work();
        return;
    }
    if (plug->procc.cpcc_sta == PLUG_CMD_OFF)
        return;
    if (plug->cc == RT_NULL)
        return;

    plug->cc->cmp_dev_work();
}

/***************************************************************************************
 * @Function    : cmp_plug_temp_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/23
 ****************************************************************************************/
static void cmp_plug_temp_work(cmp_plug_stu *plug, uint8_t period)
{
    if (plug->sock_temp == RT_NULL)
        return;
    plug->sock_temp->cmp_dev_work(period);
}

/***************************************************************************************
 * @Function    : cmp_plug_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/12
 ****************************************************************************************/
static void cmp_plug_work0(uint8_t period)
{
    cmp_plug_cp_work(&plug_conf[0], period);

    cmp_plug_contactor_work(&plug_conf[0], period);

    if (plug_conf[0].param.sock_type == 0)
    {
        cmp_plug_cc_work(&plug_conf[0]);

        cmp_plug_temp_work(&plug_conf[0], period);

        cmp_plug_elock_work(&plug_conf[0], period);
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

*@Author      :WYW            Version:  V3.0.0             Date:2023-12-28
***************************************************************************/
static uint16_t cmp_plug_relay_judg_func(uint8_t phase)
{
    return plug_conf[0].messure[0].cmp_get_chrg_volt(phase);
}

/***************************************************************************************
 * @Function    : cmp_plug_conf_init0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/12
 ****************************************************************************************/
static void cmp_plug_conf_init0(void)
{
    /* contactor */
    plug_conf[0].contactor = drv_dev_find(DRV_DEV_RELAY_CONTACTOR);

    /* elock */
    plug_conf[0].elock = drv_dev_find(DRV_DEV_RELAY_ELOCK);

    /* cpcc switch*/
    plug_conf[0].cp_io_sw = drv_dev_find(DRV_DEV_DIDO_CPCC_SW);

    /* cp */
    plug_conf[0].cp = cmp_dev_find(CMP_DEV_NAME_CP);

    /* cc */
    plug_conf[0].cc = cmp_dev_find(CMP_DEV_NAME_CC);

    /* temperature */
    plug_conf[0].sock_temp = cmp_dev_find(CMP_DEV_NAME_SOCK_TEMP);

    plug_conf[0].messure = cmp_dev_find(CMP_DEV_NAME_MEASURE);

    plug_conf[0].contactor->drv_relay_judgment(cmp_plug_relay_judg_func);
    plug_conf[0].param.plug_idx = 0;
}

/***************************************************************************************
 * @Function    : cmp_plug_local_dev_create0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/12
 ****************************************************************************************/
static void cmp_plug_local_dev_create0(cmp_plug_dev_stu *plug_dev)
{
    plug_dev->plug_set_sock_type = cmp_plug_set_sock_type0;

    plug_dev->plug_work = cmp_plug_work0;

    plug_dev->plug_get_conn_sta = cmp_plug_get_conn_sta0;

    plug_dev->plug_cpcc_sw = cmp_plug_cpcc_sw0;

    plug_dev->plug_get_cp_sta = cmp_plug_get_cp_sta0;

    plug_dev->plug_set_chrg_curr = cmp_plug_set_chrg_curr0;

    plug_dev->plug_stop_pwm_output = cmp_plug_stop_pwm_output0;

    plug_dev->plug_elock_ctrl = cmp_plug_elock_ctrl0;

    plug_dev->plug_contactor_ctrl = cmp_plug_contactor_ctrl0;

    plug_dev->plug_relay_fault_handler = cmp_plug_relay_fault_handler_func;
}

/***************************************************************************************
 * @Function    : cmp_plug_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/12
 ****************************************************************************************/
static int cmp_plug_dev_create(void)
{
    static cmp_plug_dev_stu plug_dev[BSP_POLE_PLUG_NUM];

    cmp_plug_conf_init0();

    cmp_plug_local_dev_create0(&plug_dev[0]);

    cmp_dev_append(CMP_DEV_NAME_PLUG, plug_dev);

    return 0;
}
INIT_APP_EXPORT(cmp_plug_dev_create);