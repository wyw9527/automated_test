#include "cmp_sensor.h"
#include "SEGGER_RTT.h"
static cmp_sensor_stu sensor_conf;

/***************************************************************************************
 * @Function    : cmp_sensor_leak_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/14
 ****************************************************************************************/
static void cmp_sensor_leak_work(uint8_t period)
{
    static uint16_t tmp_val;
    extern void drv_relay_fault_handler_func();

    if (!CMP_SENSOR_DEV_LEAK)
    {
        return;
    }
    if (sensor_conf.leak == RT_NULL)
    {
        return;
    }
    tmp_val = sensor_conf.leak->drv_analog_get_real_val();
    sensor_conf.procc.data.leak_val = tmp_val;

    if (tmp_val >= CMP_SENSOR_LEAK_BL)
    {
        tmp_val = tmp_val - CMP_SENSOR_LEAK_BL;
    }
    else
    {
        tmp_val = CMP_SENSOR_LEAK_BL - tmp_val;
    }

    if (tmp_val > CMP_SENSOR_LEAK_TH)
    {

        if (sensor_conf.procc.alarm.bits.leak > 0)
            return;

        // if (sensor_conf.procc.data.leak_count < 10)
        // {
        //     sensor_conf.procc.data.leak_count += period;
        //     return;
        // }
        sensor_conf.procc.alarm.bits.leak = 1;
        sensor_conf.procc.data.leak_count = 0;
        sensor_conf.procc.data.leak_alarm_cnt++;

        bsp_alarm_mb_send(0x0, 5, 0, 2);
        drv_relay_fault_handler_func();
        return;
    }
    sensor_conf.procc.data.leak_count = 0;
    if (sensor_conf.procc.alarm.bits.leak == 0)
    {
        return;
    }
    bsp_alarm_mb_send(0x0, 5, 0, 0);
    sensor_conf.procc.alarm.bits.leak = 0;
}

/***************************************************************************************
 * @Function    : cmp_sensor_ne_ana_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : WYW            Version   : V2.0.0                Date:2023/10/24
 ****************************************************************************************/
static void cmp_sensor_ne_ana_work(uint8_t period)
{
    uint16_t tmp_val;

    if (sensor_conf.ne == RT_NULL)
    {
        return;
    }

    tmp_val = sensor_conf.ne->drv_analog_get_real_val();
    sensor_conf.procc.data.ne_val = tmp_val;

    if (tmp_val > 700)
    {
        if (sensor_conf.procc.alarm.bits.ne > 0)
            return;

        if (sensor_conf.procc.data.ne_count <= 3000)
        {
            sensor_conf.procc.data.ne_count += period;
            return;
        }
        sensor_conf.procc.data.ne_alarm_cnt++;
        bsp_alarm_mb_send(0xff, 2, 6, 2);
        sensor_conf.procc.alarm.bits.ne = 1;
        return;
    }
    sensor_conf.procc.data.ne_count = 0;
    if (sensor_conf.procc.alarm.bits.ne == 0)
        return;

    sensor_conf.procc.alarm.bits.ne = 0;

    bsp_alarm_mb_send(0xff, 2, 6, 0);
}

/***************************************************************************************
 * @Function    : cmp_sensor_ne_di_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : WYW            Version   : V2.0.0                Date:2023/10/24
 ****************************************************************************************/
static void cmp_sensor_ne_di_work(uint8_t period)
{
    if (sensor_conf.di_ne == RT_NULL)
    {
        return;
    }
    sensor_conf.procc.data.ne_val = sensor_conf.di_ne->drv_di_get_val();
    if (sensor_conf.procc.data.ne_val == 0)
    {
        if (sensor_conf.procc.alarm.bits.ne > 0)
        {
            return;
        }
        if (sensor_conf.procc.data.ne_count <= 3000)
        {
            sensor_conf.procc.data.ne_count += period;
            return;
        }
        sensor_conf.procc.data.ne_alarm_cnt++;
        bsp_alarm_mb_send(0xff, 2, 6, 2);
        sensor_conf.procc.alarm.bits.ne = 1;
        return;
    }
    sensor_conf.procc.data.ne_count = 0;
    if (sensor_conf.procc.alarm.bits.ne == 0)
        return;

    sensor_conf.procc.alarm.bits.ne = 0;
    bsp_alarm_mb_send(0xff, 2, 6, 0);
}

/***************************************************************************
*@Function    :cmp_sensor_pe_work
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-11-10
***************************************************************************/
static void cmp_sensor_pe_work(uint8_t period)
{

    uint16_t tmp_val;

    if (!CMP_SENSOR_DEV_PE)
    {
        return;
    }
    if (sensor_conf.pe == RT_NULL)
    {
        return;
    }
    tmp_val = sensor_conf.pe->drv_analog_get_real_val();
    sensor_conf.procc.data.pe.val = tmp_val;

    if (tmp_val > CMP_PE_VALOT_TH)
    {
        if (sensor_conf.procc.alarm.bits.pe > 0)
            return;

        if (sensor_conf.procc.data.pe.count <= 3000)
        {
            sensor_conf.procc.data.pe.count += period;
            return;
        }
        sensor_conf.procc.data.pe.alarm_cnt++;
        bsp_alarm_mb_send(0xff, 2, 9, 2);
        sensor_conf.procc.alarm.bits.pe = 1;
        return;
    }
    sensor_conf.procc.data.pe.count = 0;
    if (sensor_conf.procc.alarm.bits.pe == 0)
        return;

    sensor_conf.procc.alarm.bits.pe = 0;

    bsp_alarm_mb_send(0xff, 2, 9, 0);
}

/***************************************************************************************
 * @Function    : cmp_sensor_ne_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/14
 ****************************************************************************************/
static void cmp_sensor_ne_work(uint8_t period)
{
    if (!CMP_SENSOR_DEV_NE)
    {
        return;
    }
    cmp_sensor_ne_ana_work(period);
    cmp_sensor_ne_di_work(period);
}

/***************************************************************************************
 * @Function    : cmp_sensor_temp_ev_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/14
 ****************************************************************************************/
static void cmp_sensor_temp_ev_work(uint8_t period)
{
    uint16_t tmp_val;

    if (!CMP_SENSOR_DEV_TEMP_EV)
    {
        return;
    }
    if (sensor_conf.temp_ev == RT_NULL)
    {
        return;
    }

    tmp_val = sensor_conf.temp_ev->drv_analog_get_real_val();
    sensor_conf.procc.data.temp_ev_val = tmp_val;

    if (tmp_val > 1500)
    {
        if (sensor_conf.procc.alarm.bits.temp_ev == 2)
        {
            sensor_conf.procc.data.temp_ev_count = 0;
            return;
        }
        if (sensor_conf.procc.data.temp_ev_count <= 120000)
        {
            sensor_conf.procc.data.temp_ev_count += period;
            return;
        }
        bsp_alarm_mb_send(0xff, 2, 16, 2);
        sensor_conf.procc.data.temp_ev_alarm_cnt++;
        sensor_conf.procc.alarm.bits.temp_ev = 2;
        return;
    }
    sensor_conf.procc.data.temp_ev_count = 0;
    if (sensor_conf.procc.alarm.bits.temp_ev == 0)
        return;
    sensor_conf.procc.alarm.bits.temp_ev = 0;

    bsp_alarm_mb_send(0xff, 2, 16, 0);
}

/***************************************************************************************
 * @Function    : cmp_sensor_eme_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/14
 ****************************************************************************************/
static void cmp_sensor_eme_work(uint8_t period)
{
    if (!CMP_SENSOR_DEV_EME)
    {
        return;
    }
    if (sensor_conf.eme == RT_NULL)
    {
        return;
    }
    sensor_conf.procc.data.ne_val = sensor_conf.eme->drv_di_get_val();

    if (sensor_conf.procc.data.ne_val > 0)
    {
        if (sensor_conf.procc.alarm.bits.eme > 0)
        {
            sensor_conf.procc.data.eme_count = 0;
            return;
        }
        if (sensor_conf.procc.data.eme_count <= 100)
        {
            sensor_conf.procc.data.eme_count += period;
            return;
        }

        bsp_alarm_mb_send(0xff, 2, 12, 2);
        sensor_conf.procc.data.eme_alarm_count++;
        sensor_conf.procc.alarm.bits.eme = 1;
        return;
    }
    sensor_conf.procc.data.eme_count = 0;
    if (sensor_conf.procc.alarm.bits.eme == 0)
        return;

    sensor_conf.procc.alarm.bits.eme = 0;

    bsp_alarm_mb_send(0xff, 2, 12, 0);
}

/***************************************************************************************
 * @Function    : cmp_sensor_pnc_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/14
 ****************************************************************************************/
static void cmp_sensor_pnc_work(uint8_t period)
{

    if (!CMP_SENSOR_DEV_PNC)
    {
        return;
    }
    if (sensor_conf.pnc == RT_NULL)
    {
        return;
    }
    sensor_conf.procc.data.pnc_val = sensor_conf.pnc->drv_di_get_val();
    if (sensor_conf.procc.data.pnc_val > 0)
    {
        if (sensor_conf.procc.alarm.bits.pnc > 0)
        {
            sensor_conf.procc.data.pnc_count = 0;
            return;
        }
        if (sensor_conf.procc.data.pnc_count < 500)
        {
            sensor_conf.procc.data.pnc_count += period;
            return;
        }

        sensor_conf.procc.data.pnc_act_cnt++;
        sensor_conf.procc.alarm.bits.pnc = 1;
        return;
    }
    sensor_conf.procc.data.pnc_count = 0;
    if (sensor_conf.procc.alarm.bits.pnc == 0)
        return;

    sensor_conf.procc.alarm.bits.pnc = 0;
}

/***************************************************************************
*@Function    :cmp_sensor_openlid_work
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-11-13
***************************************************************************/
static void cmp_sensor_openlid_work(uint8_t period)
{
    if (!CMP_SENSOR_DEV_OPL)
    {
        return;
    }
    if (sensor_conf.openlid == RT_NULL)
    {
        return;
    }
    sensor_conf.procc.data.openlid.val = sensor_conf.openlid->drv_di_get_val();
    if (sensor_conf.procc.data.openlid.val == 0)
    {
        if (sensor_conf.procc.alarm.bits.openlid > 0)
        {
            sensor_conf.procc.data.openlid.count = 0;
            return;
        }
        if (sensor_conf.procc.data.openlid.count <= 1000)
        {
            sensor_conf.procc.data.openlid.count += period;
            return;
        }

        sensor_conf.procc.data.openlid.alarm_cnt++;
        bsp_alarm_mb_send(0xff, 2, 2, 2);
        sensor_conf.procc.alarm.bits.openlid = 1;
        return;
    }
    sensor_conf.procc.data.openlid.count = 0;
    if (sensor_conf.procc.alarm.bits.openlid == 0)
        return;
    bsp_alarm_mb_send(0xff, 2, 2, 0);
    sensor_conf.procc.alarm.bits.openlid = 0;
}

/***************************************************************************************
 * @Function    : cmp_sensor_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/14
 ****************************************************************************************/
static void cmp_sensor_work(uint8_t period)
{
    cmp_sensor_leak_work(period);

    cmp_sensor_ne_work(period);

    cmp_sensor_pe_work(period);

    cmp_sensor_temp_ev_work(period);

    cmp_sensor_eme_work(period);

    cmp_sensor_pnc_work(period);

    cmp_sensor_openlid_work(period);
}

/***************************************************************************************
 * @Function    : cmp_sensor_get_pnc_sta()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : WYW            Version   : V2.0.0                Date:2023/10/9
 ****************************************************************************************/
static uint8_t cmp_sensor_get_pnc_sta()
{
    return sensor_conf.procc.alarm.bits.pnc;
}

/***************************************************************************************
 * @Function    : cmp_sensor_conf_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/14
 ****************************************************************************************/
static void cmp_sensor_conf_init(void)
{
    sensor_conf.leak = drv_dev_find(DRV_DEV_ANALOG_LEAK);

    sensor_conf.leak_seft = drv_dev_find(DRV_DEV_DIDO_LEAK_SELFTEST);

    sensor_conf.ne = drv_dev_find(DRV_DEV_ANALOG_NE);

    sensor_conf.pe = drv_dev_find(DRV_DEV_ANALOG_PE);

    sensor_conf.di_ne = drv_dev_find(DRV_DEV_DIDO_NE);

    sensor_conf.temp_ev = drv_dev_find(DRV_DEV_ANALOG_TMP_B);

    sensor_conf.temp_pv = drv_dev_find(DRV_DEV_ANALOG_TMP_P);

    sensor_conf.eme = drv_dev_find(DRV_DEV_DIDO_EME);

    sensor_conf.pnc = drv_dev_find(DRV_DEV_DIDO_PNC);

    sensor_conf.openlid = drv_dev_find(DRV_DEV_DIDO_OPENLID);
}

/***************************************************************************************
 * @Function    : cmp_sensor_ext_func_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/14
 ****************************************************************************************/
static void cmp_sensor_local_dev_init(void)
{
    static cmp_sensor_dev_stu sensor_dev;

    sensor_dev.sensor_work = cmp_sensor_work;
    sensor_dev.get_pnc_sta = cmp_sensor_get_pnc_sta;

    cmp_dev_append(CMP_DEV_NAME_SENSOR, &sensor_dev);
}

/***************************************************************************************
 * @Function    : cmp_sensor_oper_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/24
 ****************************************************************************************/
static void cmp_sensor_oper_init(void)
{
    // leak
    bsp_bind_oper_data_dev(0x0, 5, 1, (uint8_t *)&sensor_conf.procc.data.leak_alarm_cnt, 2);

    bsp_bind_oper_data_dev(0x0, 5, 2, (uint8_t *)&sensor_conf.procc.data.leak_val, 2);

    // N线反接
    bsp_bind_oper_data_dev(0xff, 2, 8, (uint8_t *)&sensor_conf.procc.data.ne_alarm_cnt, 2);
    bsp_bind_oper_data_dev(0xff, 2, 7, (uint8_t *)&sensor_conf.procc.data.ne_val, 2);

    // 环境温度
    bsp_bind_oper_data_dev(0xff, 2, 15, (uint8_t *)&sensor_conf.procc.data.temp_ev_val, 2);
    bsp_bind_oper_data_dev(0xff, 2, 17, (uint8_t *)&sensor_conf.procc.data.temp_ev_alarm_cnt, 2);

    // 急停
    bsp_bind_oper_data_dev(0xff, 2, 13, (uint8_t *)&sensor_conf.procc.data.eme_alarm_count, 2);
    bsp_bind_oper_data_dev(0xff, 2, 14, (uint8_t *)&sensor_conf.procc.data.eme_val, 1);

    // PNC
    bsp_bind_oper_data_dev(0xff, 2, 19, (uint8_t *)&sensor_conf.procc.data.pnc_val, 1);
    bsp_bind_oper_data_dev(0xff, 2, 20, (uint8_t *)&sensor_conf.procc.data.pnc_act_cnt, 1);

    // pe
    bsp_bind_oper_data_dev(0xff, 2, 10, (uint8_t *)&sensor_conf.procc.data.pe.val, 2);
    bsp_bind_oper_data_dev(0xff, 2, 11, (uint8_t *)&sensor_conf.procc.data.pe.alarm_cnt, 2);

    // openlid
    bsp_bind_oper_data_dev(0xff, 2, 3, (uint8_t *)&sensor_conf.procc.data.openlid.alarm_cnt, 2);
}

/***************************************************************************************
 * @Function    : cmp_sensor_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/14
 ****************************************************************************************/
static int cmp_sensor_dev_create(void)
{
    cmp_sensor_conf_init();

    cmp_sensor_local_dev_init();

    cmp_sensor_oper_init();

    return 0;
}
INIT_ENV_EXPORT(cmp_sensor_dev_create);