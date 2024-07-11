#include "cmp_cp.h"

static cmp_cp_stu cp_conf[BSP_POLE_PLUG_NUM];

/***************************************************************************************
 * @Function    : cmp_cp_pwm_stop0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/22
 ****************************************************************************************/
static void cmp_cp_pwm_stop0(void)
{
    cp_conf[0].pwm->drv_dev_stop();
    cp_conf[0].procc.pwm_duty = 1000;
}

/***************************************************************************************
 * @Function    : cmp_plug_cp_status_conv()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/12
 ****************************************************************************************/
static void cmp_plug_cp_status_conv(cmp_cp_stu *cp, uint8_t period)
{
    uint16_t cp_val;

    cp_val = cp->procc.cp_val;
    if (cp_val > CMP_CP_12V_DOWN && cp_val < CMP_CP_12V_UP)
    {
        if (cp->procc.sta_pre != 1)
        {
            cp->procc.count = 0;
        }
        cp->procc.sta_pre = 1;
    }
    else if (cp_val > CMP_CP_9V_DOWN && cp_val < CMP_CP_9V_UP)
    {
        if (cp->procc.sta_pre != 2)
        {
            cp->procc.count = 0;
        }
        cp->procc.sta_pre = 2;
    }
    else if (cp_val > CMP_CP_6V_DOWN && cp_val < CMP_CP_6V_UP)
    {
        if (cp->procc.sta_pre != 3)
        {
            cp->procc.count = 0;
        }
        cp->procc.sta_pre = 3;
    }
    else
    {
        if (cp->procc.sta_pre != 0)
        {
            cp->procc.count = 0;
        }
        cp->procc.sta_pre = 0;
    }

    if (cp->procc.sta == cp->procc.sta_pre)
    {
        if (cp->procc.cp_fault == 1)
        {
            return;
        }
    }

    if (cp->procc.count < 20)
    {
        cp->procc.count += period;
        return;
    }
    cp->procc.count = 0;
    cp->procc.sta = cp->procc.sta_pre;
    if (cp->procc.sta == 0)
    {
        bsp_alarm_mb_send(0, 6, 0, 2);
        cp->procc.cp_fault = 1;
        cp->procc.abn_tmr++;
        return;
    }
    cp->procc.cp_fault = 0;
    bsp_alarm_mb_send(0, 6, 0, 0);
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
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/12
 ****************************************************************************************/
static void cmp_plug_cp_work(cmp_cp_stu *cp, uint8_t period)
{
    cp->procc.cp_val = cp->samp->drv_analog_get_real_val();

    cmp_plug_cp_status_conv(cp, period);
}

/***************************************************************************************
 * @Function    : cmp_cp_work0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/22
 ****************************************************************************************/
static void cmp_cp_work0(uint8_t period)
{
    cmp_plug_cp_work(&cp_conf[0], period);
}

/***************************************************************************************
 * @Function    : cmp_cp_get_sta0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/22
 ****************************************************************************************/
static uint8_t cmp_cp_get_sta0(void)
{
    return cp_conf[0].procc.sta;
}

/***************************************************************************************
 * @Function    : cmp_cp_set_pwm_duty0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/22
 ****************************************************************************************/
static void cmp_cp_set_pwm_duty0(uint16_t duty)
{
    cp_conf[0].procc.pwm_duty = duty;
    cp_conf[0].pwm->drv_dev_set_duty(duty);

    cp_conf[0].samp->drv_analog_set_start_duty(duty);
}

/***************************************************************************************
 * @Function    : cmp_cp_conf_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/22
 ****************************************************************************************/
static void cmp_cp_conf_init(void)
{
    cp_conf[0].pwm = drv_dev_find(DRV_DEV_PWM_CP);

    cp_conf[0].samp = drv_dev_find(DRV_DEV_ANALOG_CP);
}

/***************************************************************************************
 * @Function    : cmp_cp_local_dev_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/22
 ****************************************************************************************/
static void cmp_cp_local_dev_init(void)
{
    static cmp_cp_dev_stu cp_dev[BSP_POLE_PLUG_NUM];

    cp_dev[0].cmp_get_sta = cmp_cp_get_sta0;

    cp_dev[0].cmp_dev_work = cmp_cp_work0;

    cp_dev[0].cmp_set_pwm_duty = cmp_cp_set_pwm_duty0;

    cp_dev[0].cmp_pwm_stop = cmp_cp_pwm_stop0;

    cmp_dev_append(CMP_DEV_NAME_CP, cp_dev);
}

/***************************************************************************************
 * @Function    : cmp_cp_oper_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/22
 ****************************************************************************************/
static void cmp_cp_oper_init(void)
{
    bsp_bind_oper_data_dev(0x0, 6, 1, (uint8_t *)&cp_conf[0].procc.cp_val, 1);
    bsp_bind_oper_data_dev(0x0, 6, 2, &cp_conf[0].procc.sta, 1);
    bsp_bind_oper_data_dev(0x0, 6, 3, (uint8_t *)&cp_conf[0].procc.abn_tmr, 2);
    bsp_bind_oper_data_dev(0x0, 6, 4, (uint8_t *)&cp_conf[0].procc.pwm_duty, 2);
}

/***************************************************************************************
 * @Function    : cmp_cp_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/22
 ****************************************************************************************/
static int cmp_cp_dev_create()
{
    cmp_cp_conf_init();

    cmp_cp_local_dev_init();

    cmp_cp_oper_init();

    return 0;
}
INIT_ENV_EXPORT(cmp_cp_dev_create);