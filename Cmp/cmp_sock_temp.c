#include "cmp_sock_temp.h"

static cmp_sock_temp_stu cc_conf[BSP_POLE_PLUG_NUM];

/***************************************************************************************
 * @Function    : cmp_plug_temp_status_conv()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/12
 ****************************************************************************************/
static void cmp_cc_temp_status_conv(cmp_sock_temp_stu *cc, uint8_t period)
{
    uint16_t tmp_val;
    uint32_t abn_period = 1200000;

    tmp_val = cc->procc.temp_val;
    if (tmp_val > CMP_TEMP_FAULT_TH)
    {
        if (cc->procc.sta_pre == 2)
        {
            abn_period = 1200000;
            return;
        }
        cc->procc.sta_pre = 2;
        cc->procc.abn_count = 0;
    }
    else if (tmp_val > CMP_TEMP_ALARM_TH)
    {
        if (cc->procc.sta_pre == 1)
        {
            abn_period = 3000000;
            return;
        }
        cc->procc.sta_pre = 1;
        cc->procc.abn_count = 0;
    }
    else
    {
        if (cc->procc.sta_pre == 0)
        {
            return;
        }
        cc->procc.sta_pre = 0;
        cc->procc.abn_count = 0;
    }

    if (cc->procc.abn_count < abn_period)
    {
        cc->procc.abn_count += period;
        return;
    }

    cc->procc.sta = cc->procc.sta_pre;
    bsp_alarm_mb_send(cc->procc.idx, 0, 0, cc->procc.sta);
}

/***************************************************************************************
 * @Function    : cmp_cc_work0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/22
 ****************************************************************************************/
static void cmp_cc_work0(uint8_t period)
{
    if (cc_conf[0].temperature == RT_NULL)
        return;
    cc_conf[0].procc.temp_val = cc_conf[0].temperature->drv_analog_get_real_val();

    cmp_cc_temp_status_conv(&cc_conf[0], period);
}

/***************************************************************************************
 * @Function    : cmp_sock_temp_conf_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/22
 ****************************************************************************************/
static void cmp_sock_temp_conf_init(void)
{
    cc_conf[0].temperature = drv_dev_find(DRV_DEV_ANALOG_TMP_P);

    cc_conf[0].procc.abn_count = 0;
    cc_conf[0].procc.idx = 0;
}

/***************************************************************************************
 * @Function    : cmp_sock_temp_local_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/22
 ****************************************************************************************/
static void cmp_sock_temp_local_dev_create(void)
{
    static cmp_sock_temp_dev_stu cc_dev[BSP_POLE_PLUG_NUM];

    cc_dev[0].cmp_dev_work = cmp_cc_work0;

    cmp_dev_append(CMP_DEV_NAME_SOCK_TEMP, cc_dev);
}

/***************************************************************************************
 * @Function    : cmp_sock_temp_oper_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/23
 ****************************************************************************************/
static void cmp_sock_temp_oper_init(void)
{
    bsp_bind_oper_data_dev(0, 9, 1, (uint8_t *)&cc_conf[0].procc.abn_tmr, 2);

    bsp_bind_oper_data_dev(0, 9, 2, (uint8_t *)&cc_conf[0].procc.temp_val, 2);
}

/***************************************************************************************
 * @Function    : cmp_sock_temp_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/22
 ****************************************************************************************/
static int cmp_sock_temp_dev_create()
{
    cmp_sock_temp_conf_init();

    cmp_sock_temp_local_dev_create();

    cmp_sock_temp_oper_init();

    return 0;
}
INIT_ENV_EXPORT(cmp_sock_temp_dev_create);
