#include "cmp_measure.h"

static cmp_measure_pub_stu measure_pub;
static cmp_measure_stu measure_conf[BSP_POLE_PLUG_NUM];
static cmp_measure_oper_stu measure_oper[BSP_POLE_PLUG_NUM];

/***************************************************************************************
 * @Function    : cmp_measure_get_mode0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/25
 ****************************************************************************************/
static uint8_t cmp_measure_get_mode0(void)
{
    return measure_pub.mode;
}

/***************************************************************************************
 * @Function    : cmp_measure_get_volt0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/25
 ****************************************************************************************/
static uint16_t cmp_measure_get_volt0(uint8_t phase)
{
    if (phase > 2)
        return 0;

    return measure_conf[0].volume.volt[phase];
}

/***************************************************************************************
 * @Function    : cmp_measure_get_curr0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/25
 ****************************************************************************************/
static uint16_t cmp_measure_get_curr0(uint8_t phase)
{
    if (phase > 2)
        return 0;

    return measure_conf[0].volume.curr[phase];
}

/***************************************************************************************
 * @Function    : cmp_measure_get_curr0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/25
 ****************************************************************************************/
static uint16_t cmp_measure_get_max_curr0(void)
{
    uint16_t max_curr;

    if (measure_conf[0].volume.curr[0] > measure_conf[0].volume.curr[1])
        max_curr = measure_conf[0].volume.curr[0];
    else
        max_curr = measure_conf[0].volume.curr[1];

    if (measure_conf[0].volume.curr[2] > max_curr)
        max_curr = measure_conf[0].volume.curr[2];

    return max_curr;
}

/***************************************************************************************
 * @Function    : cmp_measure_get_power0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/25
 ****************************************************************************************/
static uint32_t cmp_measure_get_power0(void)
{
    return measure_conf[0].volume.power;
}

/***************************************************************************************
 * @Function    : cmp_measure_get_energy0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/25
 ****************************************************************************************/
static uint32_t cmp_measure_get_energy0(void)
{
    return (uint32_t)(measure_conf[0].volume.energy_val);
}

/***************************************************************************************
 * @Function    : cmp_measure_meter_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/27
 ****************************************************************************************/
static void cmp_measure_meter_work(uint8_t plug_idx, uint8_t period)
{
}

/***************************************************************************************
 * @Function    : cmp_measure_m8209_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/27
 ****************************************************************************************/
static void cmp_measure_m8209_work(uint8_t plug_idx, uint8_t period)
{
}

/***************************************************************************************
 * @Function    : cmp_measure_board_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/27
 ****************************************************************************************/
static void cmp_measure_board_work(uint8_t plug_idx, uint8_t period)
{
    uint32_t tmp = 0;

    measure_conf[plug_idx].volume.volt[0] = measure_conf[plug_idx].volt_dev[0]->drv_analog_get_real_val();
    measure_conf[plug_idx].volume.volt[1] = measure_conf[plug_idx].volt_dev[1]->drv_analog_get_real_val();
    measure_conf[plug_idx].volume.volt[2] = measure_conf[plug_idx].volt_dev[2]->drv_analog_get_real_val();

    if (measure_conf[0].procc.chrg_sta != 3)
    {
        measure_conf[plug_idx].volume.curr[0] = 0;
        measure_conf[plug_idx].volume.curr[1] = 0;
        measure_conf[plug_idx].volume.curr[2] = 0;
    }
    else
    {
        measure_conf[plug_idx].volume.curr[0] = measure_conf[plug_idx].curr_dev[0]->drv_analog_get_real_val();
        measure_conf[plug_idx].volume.curr[1] = measure_conf[plug_idx].curr_dev[1]->drv_analog_get_real_val();
        measure_conf[plug_idx].volume.curr[2] = measure_conf[plug_idx].curr_dev[2]->drv_analog_get_real_val();
    }

    for (uint8_t i = 0; i < 3; i++)
    {
        tmp = tmp + (uint32_t)(measure_conf[plug_idx].volume.volt[i] * measure_conf[plug_idx].volume.curr[i] / 10000.0);
    }
    measure_conf[plug_idx].volume.power = tmp;
    measure_conf[plug_idx].volume.energy = measure_conf[plug_idx].volume.energy + tmp * period / 3600000.0;

    measure_conf[plug_idx].volume.energy_val = (uint32_t)(measure_conf[plug_idx].volume.energy * 10);
}

/***************************************************************************************
 * @Function    : cmp_measure_calc_work0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/17
 ****************************************************************************************/
static void cmp_measure_calc_work0(uint8_t period)
{
    switch (measure_pub.mode)
    {
    case 2:
        cmp_measure_meter_work(0, period);
        break;
    case 1:
        cmp_measure_m8209_work(0, period);
        break;
    case 0:
        cmp_measure_board_work(0, period);
        break;
    }
}

/***************************************************************************************
 * @Function    : cmp_measure_alarm_volt_work()
 *
 * @Param       :
 *
 * @Return      : 0 无动作  1 欠压恢复  2 过压恢复 3 欠压产生  4 过压产生
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/17
 ****************************************************************************************/
static void cmp_measure_alarm_volt_work(cmp_measure_stu *meas, uint8_t pidx, uint8_t period)
{
    uint8_t type;

    /* 过欠压 */
    if (meas->volume.volt[pidx] >= CMP_MEASURE_VOLT_OV_TH)
    {
        if (meas->volume.volt_alarm[pidx] == 2)
            return;
        type = 1;
    }
    else if (meas->volume.volt[pidx] <= CMP_MEASURE_VOLT_UV_TH && meas->volume.volt[pidx] > CMP_MEASURE_VOLT_LOSS_TH)
    {
        if (meas->volume.volt_alarm[pidx] == 1)
            return;
        type = 2;
    }
    else if (meas->volume.volt[pidx] <= CMP_MEASURE_VOLT_LOSS_TH)
    {
        if (meas->volume.volt_alarm[pidx] == 3)
            return;
        type = 3;
    }

    else
    {
        if (meas->volume.volt_alarm[pidx] == 1)
        {
            meas->volume.volt_alarm[pidx] = 0;
            bsp_alarm_mb_send(0, 10, 13 + pidx, 0);
            return;
        }
        if (meas->volume.volt_alarm[pidx] == 2)
        {
            meas->volume.volt_alarm[pidx] = 0;
            bsp_alarm_mb_send(0, 10, 10 + pidx, 0);
            return;
        }
        if (meas->volume.volt_alarm[pidx] == 3)
        {
            meas->volume.volt_alarm[pidx] = 0;
            bsp_alarm_mb_send(0, 10, 22 + pidx, 0);
            return;
        }
        meas->volume.volt_alarm[pidx] = 0;
        meas->volume.volt_count[pidx] = 0;
        return;
    }

    meas->volume.volt_count[pidx] += period;
    if (meas->volume.volt_count[pidx] < CMP_MEASURE_ALARM_TMOUT)
    {
        return;
    }
    meas->volume.volt_count[pidx] = 0;

    // 如果已存在另一种故障，则先清除
    if (meas->volume.volt_alarm[pidx] != 0)
    {
        switch (meas->volume.volt_alarm[pidx])
        {
        case 1:
            bsp_alarm_mb_send(0, 10, 13 + pidx, 0);
            break;
        case 2:
            bsp_alarm_mb_send(0, 10, 10 + pidx, 0);
            break;
        case 3:
            bsp_alarm_mb_send(0, 10, 22 + pidx, 0);
            break;

        default:
            break;
        }
    }
    if (type == 1)
    {
        meas->volume.volt_alarm[pidx] = 2;
        measure_oper[meas->idx].ov_count++;
        bsp_alarm_mb_send(0, 10, 10 + pidx, 2);
        return;
    }
    else if (type == 2)
    {
        meas->volume.volt_alarm[pidx] = 1;
        measure_oper[meas->idx].uv_count++;
        bsp_alarm_mb_send(0, 10, 13 + pidx, 2);
        return;
    }

    meas->volume.volt_alarm[pidx] = 3;
    bsp_alarm_mb_send(0, 10, 22 + pidx, 1);
}

/***************************************************************************************
 * @Function    : cmp_measure_alarm_volt_work()
 *
 * @Param       :
 *
 * @Return      : 0 无动作 1 动作恢复  2 故障产生
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/17
 ****************************************************************************************/
static void cmp_measure_alarm_curr_work(cmp_measure_stu *meas, uint8_t pidx, uint8_t period)
{
    uint16_t curr_th_val;

    /* 门限判断 */
    if (measure_pub.curr_th != RT_NULL)
    {
        if (*measure_pub.curr_th > 16)
        {
            curr_th_val = (uint16_t)(*measure_pub.curr_th * 11);
        }
        else
        {
            curr_th_val = 180;
        }
    }
    else
    {
        curr_th_val = 180;
    }

    /* 过流 */
    if (meas->volume.curr[pidx] >= curr_th_val)
    {
        if (meas->volume.curr_alarm[pidx] > 0)
            return;
    }
    else
    {
        if (meas->volume.curr_alarm[pidx] > 0)
        {
            meas->volume.curr_alarm[pidx] = 0;
            meas->volume.curr_count[pidx] = 0;
            bsp_alarm_mb_send(0, 10, 16 + pidx, 0);
            return;
        }
        return;
    }

    meas->volume.curr_count[pidx] += period;
    if (meas->volume.curr_count[pidx] < CMP_MEASURE_ALARM_TMOUT)
    {
        return;
    }
    meas->volume.curr_count[pidx] = 0;

    meas->volume.curr_alarm[pidx] = 1;
    measure_oper[meas->idx].oc_count++;
    bsp_alarm_mb_send(0, 10, 16 + pidx, 2);
}

/***************************************************************************************
 * @Function    : cmp_measure_alarm_work0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/17
 ****************************************************************************************/
static void cmp_measure_alarm_work(cmp_measure_stu *meas, uint8_t period)
{
    uint8_t phase_num;

    phase_num = measure_pub.phase == 1 ? 3 : 1;

    // over/under/loss volt
    for (uint8_t i = 0; i < phase_num; i++)
    {
        cmp_measure_alarm_volt_work(meas, i, period);
        cmp_measure_alarm_curr_work(meas, i, period);
    }
}

/***************************************************************************************
 * @Function    : cmp_measure_cyc_work0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/27
 ****************************************************************************************/
static void cmp_measure_cyc_work0(uint8_t period)
{
    cmp_measure_calc_work0(period);

    cmp_measure_alarm_work(&measure_conf[0], period);
}

/***************************************************************************************
 * @Function    : cmp_measure_dev_init0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/25
 ****************************************************************************************/
static void cmp_measure_dev_init0(cmp_measure_stu *conf)
{
    conf->volt_dev[0] = drv_dev_find(DRV_DEV_ANALOG_VOLT1);
    conf->volt_dev[1] = drv_dev_find(DRV_DEV_ANALOG_VOLT2);
    conf->volt_dev[2] = drv_dev_find(DRV_DEV_ANALOG_VOLT3);
    if (conf->volt_dev[0] == RT_NULL)
    {
        measure_pub.volt_dev_sta = 1;
    }

    conf->curr_dev[0] = drv_dev_find(DRV_DEV_ANALOG_CURR1);
    conf->curr_dev[1] = drv_dev_find(DRV_DEV_ANALOG_CURR2);
    conf->curr_dev[2] = drv_dev_find(DRV_DEV_ANALOG_CURR3);
    if (conf->curr_dev[0] == RT_NULL)
    {
        measure_pub.curr_dev_sta = 1;
    }

    conf->m8209 = drv_dev_find(DRV_DEV_M8209);
    if (conf->m8209 == RT_NULL)
    {
        measure_pub.m8209_dev_sta = 1;
    }

    conf->meter = drv_dev_find(DRV_DEV_METER_CHINT);
    if (conf->meter == RT_NULL)
    {
        measure_pub.meter_dev_sta = 1;
    }

    measure_pub.mode = 0;
    measure_pub.phase = CONN_TYPE;
}

/***************************************************************************
*@Function    :
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2024-01-23
***************************************************************************/
static void cmp_set_chrg_sta_func(uint8_t val)
{
    measure_conf[0].procc.chrg_sta = val;
}

/***************************************************************************************
 * @Function    : cmp_measure_dev_conf0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/25
 ****************************************************************************************/
static void cmp_measure_dev_conf0()
{
    static cmp_measure_dev_stu measure_dev[BSP_POLE_PLUG_NUM];

    measure_dev[0].cmp_get_mode = cmp_measure_get_mode0;
    measure_dev[0].cmp_get_chrg_volt = cmp_measure_get_volt0;
    measure_dev[0].cmp_get_chrg_curr = cmp_measure_get_curr0;
    measure_dev[0].cmp_get_chrg_max_curr = cmp_measure_get_max_curr0;
    measure_dev[0].cmp_get_chrg_power = cmp_measure_get_power0;
    measure_dev[0].cmp_get_chrg_energy = cmp_measure_get_energy0;
    measure_dev[0].cmp_cyc_work = cmp_measure_cyc_work0;
    measure_dev[0].cmp_set_chrg_sta = cmp_set_chrg_sta_func;

    cmp_dev_append(CMP_DEV_NAME_MEASURE, measure_dev);
}

/***************************************************************************************
 * @Function    : cmp_measure_data_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/17
 ****************************************************************************************/
static void cmp_measure_data_init(void)
{
    const cmp_store_dev_stu *store = cmp_dev_find(CMP_DEV_NAME_STORE);

    if (store != RT_NULL)
    {
        measure_pub.curr_th = store->param->store_parameter_query(3);
    }
}

/***************************************************************************************
 * @Function    : cmp_measure_oper_dev_init0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/21
 ****************************************************************************************/
static void cmp_measure_oper_dev_init0(void)
{
    bsp_bind_oper_data_dev(0x0, 10, 1, &measure_pub.mode, 1);
    for (uint8_t i = 0; i < 3; i++)
    {
        bsp_bind_oper_data_dev(0x0, 10, 2 + i, (uint8_t *)&measure_conf[0].volume.volt[i], 2);
        bsp_bind_oper_data_dev(0x0, 10, 5 + i, (uint8_t *)&measure_conf[0].volume.curr[i], 2);
    }

    bsp_bind_oper_data_dev(0x0, 10, 8, (uint8_t *)&measure_conf[0].volume.power, 4);
    bsp_bind_oper_data_dev(0x0, 10, 9, (uint8_t *)&measure_conf[0].volume.energy_val, 4);

    bsp_bind_oper_data_dev(0x0, 10, 19, (uint8_t *)&measure_oper[0].ov_count, 2);
    bsp_bind_oper_data_dev(0x0, 10, 20, (uint8_t *)&measure_oper[0].uv_count, 2);
    bsp_bind_oper_data_dev(0x0, 10, 21, (uint8_t *)&measure_oper[0].oc_count, 2);
}

/***************************************************************************************
 * @Function    : cmp_measure_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/24
 ****************************************************************************************/
int cmp_measure_dev_create(void)
{
    cmp_measure_data_init();

    cmp_measure_dev_init0(&measure_conf[0]);

    cmp_measure_dev_conf0();

    cmp_measure_oper_dev_init0();

    return 0;
}
// INIT_APP_EXPORT(cmp_measure_dev_create);