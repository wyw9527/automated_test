#include "cmp_ext_leak.h"

static cmp_ext_leak_stu ext_leak_conf;
static cmp_ext_leak_plug_dev_stu plug_dev;

/***************************************************************************************
 * @Function    : cmp_ext_leak_conf_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/19
 ****************************************************************************************/
static void cmp_ext_leak_conf_init(void)
{
    ext_leak_conf.meter = drv_dev_find(DRV_DEV_METER_CHINT);
    if (ext_leak_conf.meter == RT_NULL)
    {
        ext_leak_conf.param.dev_sta = 1;
    }

    plug_dev.chrg_dev_cnt = 0;
}

/***************************************************************************************
 * @Function    : dev_ext_set_param()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/19
 ****************************************************************************************/
static void dev_ext_set_param(const uint8_t *load_bal_flag, const uint8_t *load_bal_th, const uint8_t *solar_bal_mode, const uint8_t *station_bal, const uint8_t *max_curr)
{
    ext_leak_conf.param.load_bal_flag = load_bal_flag;
    ext_leak_conf.param.load_bal_th = load_bal_th;
    ext_leak_conf.param.solar_bal_mode = solar_bal_mode;
    ext_leak_conf.param.station_bal = station_bal;
    ext_leak_conf.param.max_curr = max_curr;
}

/***************************************************************************************
 * @Function    : dev_ext_set_param()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/19
 ****************************************************************************************/
static void dev_ext_set_resume_adj_flag(uint8_t flag)
{
    ext_leak_conf.procc.resume_adj_flag = flag;
}

/***************************************************************************************
 * @Function    : dev_ext_set_param()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/19
 ****************************************************************************************/
static void dev_ext_set_station_set_curr(uint16_t curr)
{
    ext_leak_conf.procc.station_set_curr = curr;
}

/***************************************************************************************
 * @Function    : ext_leakage_curr_set()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2023/11/20
 ****************************************************************************************/
static void ext_leakage_curr_set(int16_t set_curr, uint8_t type, uint16_t min_curr)
{
    int16_t curr, diff_curr, set_curr_value, cnt = 0;

    set_curr_value = set_curr > 0 ? set_curr : 0;
    if (set_curr_value >= plug_dev.chrg_dev_cnt * min_curr)
    {
        curr = set_curr_value / plug_dev.chrg_dev_cnt;
        for (uint8_t i = 0; i < plug_dev.chrg_dev_cnt; i++)
        {
            diff_curr = curr - plug_dev.plug_dev[i].set_curr;
            if ((diff_curr > 2) || (diff_curr < -2))
            {
                plug_dev.plug_dev[i].set_curr = curr;
                ext_leak_conf.param.hook_func(plug_dev.plug_dev[i].plug_idx, type, plug_dev.plug_dev[i].set_curr);
            }
        }
        return;
    }

    curr = 0;
    for (uint8_t i = 0; i < plug_dev.chrg_dev_cnt; i++)
    {
        curr += min_curr;
        if (set_curr_value < curr)
        {
            break;
        }
        cnt++;
    }

    if (cnt > 0)
    {
        curr = set_curr_value / cnt;
    }
    for (uint8_t i = 0; i < plug_dev.chrg_dev_cnt; i++)
    {
        if (i < cnt)
        {
            diff_curr = curr - plug_dev.plug_dev[i].set_curr;
            if ((diff_curr > 2) || (diff_curr < -2))
            {
                plug_dev.plug_dev[i].set_curr = curr;
                ext_leak_conf.param.hook_func(plug_dev.plug_dev[i].plug_idx, type, plug_dev.plug_dev[i].set_curr);
            }
            continue;
        }
        plug_dev.plug_dev[i].set_curr = 0;
        ext_leak_conf.param.hook_func(plug_dev.plug_dev[i].plug_idx, type, plug_dev.plug_dev[i].set_curr);
    }
}

/***************************************************************************************
 * @Function    : drv_chrg_station_bal_chrg_start_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2023/7/19
 ****************************************************************************************/
static void drv_chrg_station_bal_chrg_start_work(void)
{
    ext_leak_conf.procc.set_curr = BSP_CHRG_MIN_CURR * plug_dev.chrg_dev_cnt;
    ext_leak_conf.procc.station_set_curr = ext_leak_conf.procc.set_curr;
    for (uint8_t i = 0; i < plug_dev.chrg_dev_cnt; i++)
    {
        plug_dev.plug_dev[i].set_curr = BSP_CHRG_MIN_STAT_CURR;
        ext_leak_conf.param.hook_func(plug_dev.plug_dev[i].plug_idx, EXT_LEAK_STATION, plug_dev.plug_dev[i].set_curr);
    }
}

static void drv_chrg_station_bal_chrg_and_pend_work(uint8_t chrg_sta)
{
    int16_t min_curr;
    if (chrg_sta != 3 && chrg_sta != 7)
        return;

    min_curr = chrg_sta == 3 ? BSP_CHRG_MIN_CURR : BSP_CHRG_MIN_STAT_CURR;
    ext_leak_conf.procc.set_curr = ext_leak_conf.procc.station_set_curr;

    ext_leakage_curr_set(ext_leak_conf.procc.set_curr, EXT_LEAK_STATION, min_curr);
}

/***************************************************************************************
 * @Function    : drv_chrg_station_bal_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/19
 ****************************************************************************************/
static void drv_chrg_station_bal_work(uint8_t period, uint8_t chrg_sta)
{
    if (plug_dev.chrg_dev_cnt == 0)
        return;

    if (chrg_sta == 2)
    {
        drv_chrg_station_bal_chrg_start_work();
    }
    else
    {
        drv_chrg_station_bal_chrg_and_pend_work(chrg_sta);
    }
}

/***************************************************************************************
 * @Function    : drv_ext_leak_load_bal_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/19
 ****************************************************************************************/
static uint8_t drv_ext_leak_load_bal_work(uint8_t period)
{
    /* pole load bal */
    int16_t offset;
    int16_t set_curr;

    if (*ext_leak_conf.param.load_bal_flag == 0)
    {
        ext_leak_conf.procc.load_bal_adj_flag = 0;
        return 0;
    }

    /* 负载均衡正在调整值，则直接返回 */
    if (ext_leak_conf.procc.load_bal_adj_flag > 0)
    {
        return 0;
    }

    /* 当前满足负载均衡的充电设置电流 */
    offset = ext_leak_conf.procc.load_bal_margin - ext_leak_conf.procc.load_bal_margin_pre;
    if ((offset >= 3) || (offset < -3))
    {
        ext_leak_conf.procc.load_bal_count += period;
        if (ext_leak_conf.procc.load_bal_count < 3000)
        {
            return 0;
        }
    }
    else
    {
        ext_leak_conf.procc.load_bal_count = 0;
        return 0;
    }

    set_curr = ext_leak_conf.procc.chrg_curr + ext_leak_conf.procc.load_bal_margin;
    ext_leak_conf.procc.load_bal_margin_pre = ext_leak_conf.procc.load_bal_margin;

    if ((*ext_leak_conf.param.solar_bal_mode != 0) && (ext_leak_conf.procc.load_bal_margin > 0)) // 电流提升
    {
        return 0;
    }

    /* 余量足够最小充电门限，则直接返回充电值 */
    if (set_curr >= BSP_CHRG_MIN_CURR)
    {
        ext_leak_conf.procc.load_bal_count = 0;
        if (set_curr != ext_leak_conf.procc.set_curr)
        {
            ext_leak_conf.procc.load_bal_adj_flag = 1;
            ext_leak_conf.procc.set_curr = set_curr;
        }
        return 0;
    }

    /* 余量不够，且持续3s，则直接停机处理 */
    for (uint8_t i = 0; i < plug_dev.chrg_dev_cnt; i++)
    {
        plug_dev.plug_dev[i].set_curr = 0;
        ext_leak_conf.param.hook_func(plug_dev.plug_dev[i].plug_idx, EXT_LEAK_SOLAR, plug_dev.plug_dev[i].set_curr);
    }
    return 1;
}

/***************************************************************************************
 * @Function    : drv_ext_leak_solar_link_load_bal_chrg_start_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2023/11/20
 ****************************************************************************************/
static void drv_ext_leak_solar_link_load_bal_chrg_start_work(void)
{
    ext_leak_conf.procc.set_curr = 0;

    if (*ext_leak_conf.param.solar_bal_mode == 0)
    {
        if (*ext_leak_conf.param.load_bal_flag > 0)
            ext_leak_conf.procc.set_curr = ext_leak_conf.procc.load_bal_margin;
        else
            ext_leak_conf.procc.set_curr = 320 * plug_dev.chrg_dev_cnt;
    }
    else
    {
        if (*ext_leak_conf.param.load_bal_flag > 0)
        {
            ext_leak_conf.procc.set_curr = ext_leak_conf.procc.load_bal_margin;
            if (ext_leak_conf.procc.set_curr < BSP_CHRG_MIN_CURR)
            {
                ext_leak_conf.procc.set_curr = 0;
                ext_leakage_curr_set(ext_leak_conf.procc.set_curr, EXT_LEAK_SOLAR, BSP_CHRG_MIN_CURR);
                return;
            }
        }
        if (ext_leak_conf.procc.meter_have_curr < -BSP_CHRG_MIN_STAT_CURR * plug_dev.chrg_dev_cnt)
            ext_leak_conf.procc.set_curr = -ext_leak_conf.procc.meter_have_curr;
        else
            ext_leak_conf.procc.set_curr = BSP_CHRG_MIN_CURR * plug_dev.chrg_dev_cnt;
    }
    ext_leakage_curr_set(ext_leak_conf.procc.set_curr, EXT_LEAK_SOLAR, BSP_CHRG_MIN_CURR);
}

static void load_bal_solar_process(uint8_t period)
{
    int16_t offset;

    ext_leak_conf.procc.load_bal_count += period;
    if (ext_leak_conf.procc.load_bal_count < period * 3)
    {
        ext_leakage_curr_set(ext_leak_conf.procc.set_curr, EXT_LEAK_SOLAR, BSP_CHRG_MIN_CURR);
    }
    else if (ext_leak_conf.procc.load_bal_count < 50000)
    {
        /* 充电电流稳定判断 */
        offset = ext_leak_conf.procc.chrg_curr - ext_leak_conf.procc.chrg_curr_pre;
        if (offset > 5 || offset < -5)
        {
            ext_leak_conf.procc.set_adj_stdy_count = 0;
            ext_leak_conf.procc.chrg_curr_pre = ext_leak_conf.procc.chrg_curr;
            return;
        }
        else
        {
            ext_leak_conf.procc.set_adj_stdy_count += period;
            if (ext_leak_conf.procc.set_adj_stdy_count < 5000)
            {
                return;
            }
        }

        /* 负载均衡调节完成 */
        ext_leak_conf.procc.load_bal_adj_flag = 0;
        ext_leak_conf.procc.load_bal_count = 0;
        ext_leak_conf.procc.set_adj_stdy_count = 0;
        return;
    }
}

/***************************************************************************************
 * @Function    : exec_work_ext_leakage_solar()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2023/11/21
 ****************************************************************************************/
static void exec_work_ext_leakage_solar(uint8_t period)
{
    int16_t offset;
    int16_t adj_curr;

    if (ext_leak_conf.procc.load_bal_adj_flag == 1)
    {
        load_bal_solar_process(period);
        return;
    }

    if (*ext_leak_conf.param.solar_bal_mode == 0)
    {
        return;
    }

    /*绿色模式和经济模式稳定充电45S后再做调整处理*/
    if (ext_leak_conf.procc.solar_start_complet_flag == 0)
    {
        ext_leak_conf.procc.solar_run_count += period;
        if (ext_leak_conf.procc.solar_run_count < 45000)
        {
            return;
        }

        // 绿色模式，不允许使用外部电源，则禁止启动，进入暂停
        if (*ext_leak_conf.param.solar_bal_mode == 2)
        {
            ext_leak_conf.procc.solar_set_count += period;
            if (ext_leak_conf.procc.solar_set_count < 10000)
            {
                if (ext_leak_conf.procc.meter_have_curr > EXT_LEAK_SOLAR_START_CURR)
                {
                    ext_leak_conf.procc.set_adj_stdy_count += period;
                    if (ext_leak_conf.procc.set_adj_stdy_count < 3000)
                    {
                        return;
                    }

                    ext_leak_conf.procc.set_curr -= ext_leak_conf.procc.meter_have_curr;
                    ext_leakage_curr_set(ext_leak_conf.procc.set_curr, EXT_LEAK_SOLAR, BSP_CHRG_MIN_CURR);
                    ext_leak_conf.procc.solar_set_count = 0;
                    ext_leak_conf.procc.set_adj_stdy_count = 0;
                    return;
                }
                else
                {
                    ext_leak_conf.procc.set_adj_stdy_count = 0;
                    return;
                }
            }
        }
        ext_leak_conf.procc.solar_start_complet_flag = 1;
        ext_leak_conf.procc.solar_set_count = 0;
        ext_leak_conf.procc.set_adj_stdy_count = 0;
        ext_leak_conf.procc.solar_run_count = 0;
    }

    /* 已经再调节 */
    if (ext_leak_conf.procc.solar_adj_flag > 0)
    {
        ext_leak_conf.procc.solar_set_count += period;
        if (ext_leak_conf.procc.solar_set_count < 30000) // 光伏联动调节电流后，先等待30s
        {
            return;
        }

        if (ext_leak_conf.procc.solar_set_count < 80000)
        {
            /* 充电电流稳定判断 */
            offset = ext_leak_conf.procc.chrg_curr - ext_leak_conf.procc.chrg_curr_pre;
            if (offset > 5 || offset < -5)
            {
                ext_leak_conf.procc.set_adj_stdy_count = 0;
                ext_leak_conf.procc.chrg_curr_pre = ext_leak_conf.procc.chrg_curr;
                return;
            }
            else
            {
                ext_leak_conf.procc.set_adj_stdy_count += period;
                if (ext_leak_conf.procc.set_adj_stdy_count < 5000)
                {
                    return;
                }
            }
        }
        ext_leak_conf.procc.set_adj_stdy_count = 0;
        ext_leak_conf.procc.solar_set_count = 0;
        ext_leak_conf.procc.solar_adj_flag = 0;
        return;
    }

    if ((ext_leak_conf.procc.meter_have_curr > EXT_LEAK_SOLAR_DWON_CURR) || (ext_leak_conf.procc.meter_have_curr < EXT_LEAK_SOLAR_UP_CURR)) // 需要主动改变充电电流
    {
        ext_leak_conf.procc.set_adj_stdy_count += period;
        if (ext_leak_conf.procc.set_adj_stdy_count < 3000)
        {
            return;
        }
    }
    else
    {
        ext_leak_conf.procc.set_adj_stdy_count = 0;
        return;
    }
    ext_leak_conf.procc.set_adj_stdy_count = 0;

    if (ext_leak_conf.procc.meter_have_curr > EXT_LEAK_SOLAR_DWON_CURR)
    {
        if (ext_leak_conf.procc.set_curr > ext_leak_conf.procc.chrg_curr + 50)
        {
            ext_leak_conf.procc.set_curr = ext_leak_conf.procc.chrg_curr;
        }
        else
        {
            ext_leak_conf.procc.set_curr -= EXT_LEAK_SOLAR_OFFSET;
        }
    }
    else if (ext_leak_conf.procc.meter_have_curr < EXT_LEAK_SOLAR_UP_CURR)
    {
        uint16_t max_curr = 0;
        memcpy((uint8_t *)&max_curr, ext_leak_conf.param.max_curr, 2);
        if (ext_leak_conf.procc.set_curr < max_curr * 10)
            ext_leak_conf.procc.set_curr += EXT_LEAK_SOLAR_OFFSET;
    }
    else
    {
        return;
    }

    /* 设置电流<6A时*/
    if (ext_leak_conf.procc.set_curr < BSP_CHRG_MIN_CURR)
    {
        /* 绿色模式：持续3s暂停充电 */
        if (*ext_leak_conf.param.solar_bal_mode == 2)
        {
            ext_leakage_curr_set(ext_leak_conf.procc.set_curr, EXT_LEAK_SOLAR, BSP_CHRG_MIN_CURR);
            return;
        }

        /* 经济模式:满足负载均衡时以6A充，否则进入暂停状态 */
        adj_curr = BSP_CHRG_MIN_CURR - ext_leak_conf.procc.chrg_curr;

        if ((*ext_leak_conf.param.load_bal_flag > 0) && (adj_curr > ext_leak_conf.procc.load_bal_margin))
        {
            // 不足负载均衡，则进入暂停
            ext_leak_conf.procc.set_curr = 0;
            ext_leakage_curr_set(ext_leak_conf.procc.set_curr, EXT_LEAK_SOLAR, BSP_CHRG_MIN_CURR);
            return;
        }

        /*经济模式最小电流充电*/
        ext_leak_conf.procc.set_curr = BSP_CHRG_MIN_CURR * plug_dev.chrg_dev_cnt;
        for (uint8_t i = 0; i < plug_dev.chrg_dev_cnt; i++)
        {
            plug_dev.plug_dev[i].set_curr = BSP_CHRG_MIN_CURR;
            ext_leak_conf.param.hook_func(plug_dev.plug_dev[i].plug_idx, EXT_LEAK_SOLAR, plug_dev.plug_dev[i].set_curr);
        }
        ext_leak_conf.procc.solar_adj_flag = 1;
        return;
    }

    /* 设置电流>=6A时，设置此值时，此时电表电流就为0，肯定不会触发负载均衡 */
    ext_leakage_curr_set(ext_leak_conf.procc.set_curr, EXT_LEAK_SOLAR, BSP_CHRG_MIN_CURR);
    ext_leak_conf.procc.solar_adj_flag = 1;
}

/***************************************************************************************
 * @Function    : drv_ext_leak_get_chrg_curr()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2023/11/20
 ****************************************************************************************/
static void drv_ext_leak_get_chrg_curr(uint8_t chrg_sta)
{
    const cmp_measure_dev_stu *measure;

    if (plug_dev.chrg_dev_cnt == 0)
        return;

    ext_leak_conf.procc.chrg_curr = 0;
    measure = cmp_dev_find(CMP_DEV_NAME_MEASURE);
    if (measure == RT_NULL)
        return;

    for (uint8_t i = 0; i < plug_dev.chrg_dev_cnt; i++)
    {
        ext_leak_conf.procc.chrg_curr += measure[i].cmp_get_chrg_max_curr();
    }
    return;
}

/***************************************************************************************
 * @Function    : exec_puse_resume_work_by_leakage_check()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/3/25
 ****************************************************************************************/
static void exec_puse_resume_work_by_leakage_check(uint8_t period)
{
    /* 在调整中，则不用检测恢复条件 */
    if (ext_leak_conf.procc.resume_adj_flag > 0)
    {
        return;
    }
    ext_leak_conf.procc.resume_flag = 0;
    ext_leak_conf.procc.set_curr = 0;

    if (*ext_leak_conf.param.load_bal_flag > 0)
    {
        /* 负载均衡余量小于6.5A,则进入暂停状态 */
        if (ext_leak_conf.procc.load_bal_margin < BSP_CHRG_MIN_STAT_CURR)
            return;
        else
            ext_leak_conf.procc.set_curr = ext_leak_conf.procc.load_bal_margin;
    }

    if (*ext_leak_conf.param.solar_bal_mode == 0)
    {
        ext_leak_conf.procc.resume_flag = 1;
        ext_leakage_curr_set(ext_leak_conf.procc.set_curr, EXT_LEAK_SOLAR, BSP_CHRG_MIN_STAT_CURR);
        return;
    }

    /*当电表值>-6.5时 */
    if (ext_leak_conf.procc.meter_have_curr > -BSP_CHRG_MIN_STAT_CURR)
    {
        // 绿色模式不允许启动
        if (*ext_leak_conf.param.solar_bal_mode == 2)
        {
            ext_leak_conf.procc.solar_green_puse_count += period;
            if (ext_leak_conf.procc.solar_green_puse_count < 900000)
            {
                return;
            }
        }

        /* 经济模式，负载均衡余量小于6.5A，则不允许启动 */
        if ((*ext_leak_conf.param.solar_bal_mode == 1) && (ext_leak_conf.procc.load_bal_margin < BSP_CHRG_MIN_STAT_CURR))
        {
            return;
        }
        /*以最小电流启动*/
        ext_leak_conf.procc.set_curr = BSP_CHRG_MIN_CURR;
        ext_leakage_curr_set(ext_leak_conf.procc.set_curr, EXT_LEAK_SOLAR, BSP_CHRG_MIN_CURR);
        ext_leak_conf.procc.resume_flag = 1;
        return;
    }

    ext_leak_conf.procc.set_curr = -ext_leak_conf.procc.meter_have_curr;
    ext_leakage_curr_set(ext_leak_conf.procc.set_curr, EXT_LEAK_SOLAR, BSP_CHRG_MIN_STAT_CURR);
    ext_leak_conf.procc.resume_flag = 1;
}

/***************************************************************************************
 * @Function    : drv_ext_leak_solar_link_load_bal_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2023/11/20
 ****************************************************************************************/
static void drv_ext_leak_solar_link_load_bal_work(uint8_t period, uint8_t chrg_sta)
{
    static uint8_t flag = 0;
    uint16_t load_bal_th = 0;

    if (plug_dev.chrg_dev_cnt == 0)
        return;

    drv_ext_leak_get_chrg_curr(chrg_sta);
    ext_leak_conf.procc.meter_curr = ext_leak_conf.meter->drv_get_meter_volume(METER_VOLUME_CURR_MAX) / 10;
    ext_leak_conf.procc.meter_have_curr = ext_leak_conf.meter->drv_get_meter_volume(METER_VOLUME_CURR_HAVE_MAX) / 10;
    if (*ext_leak_conf.param.load_bal_flag > 0)
    {
        memcpy((uint8_t *)&load_bal_th, ext_leak_conf.param.load_bal_th, 2);
        load_bal_th = load_bal_th * 10;
        ext_leak_conf.procc.load_bal_margin = load_bal_th - ext_leak_conf.procc.meter_curr;
    }

    if (chrg_sta == 2)
    {
        if (flag == 0)
        {
            flag = 1;
            drv_ext_leak_solar_link_load_bal_chrg_start_work();
        }
    }
    else
    {
        flag = 0;
    }

    if (chrg_sta == 3)
    {
        if (drv_ext_leak_load_bal_work(period) > 0)
            return;
        exec_work_ext_leakage_solar(period);
    }
    else
    {
        ext_leak_conf.procc.solar_start_complet_flag = 0;
        ext_leak_conf.procc.solar_set_count = 0;
        ext_leak_conf.procc.set_adj_stdy_count = 0;
        ext_leak_conf.procc.solar_run_count = 0;
    }

    if (chrg_sta == 7)
    {
        exec_puse_resume_work_by_leakage_check(period);
    }
    else
    {
        ext_leak_conf.procc.solar_green_puse_count = 0;
    }
}
/***************************************************************************************
 * @Function    : drv_ext_leak_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/19
 ****************************************************************************************/
static void drv_ext_leak_work(uint8_t period, uint8_t chrg_sta)
{
    if (*ext_leak_conf.param.station_bal > 0)
    {
        drv_chrg_station_bal_work(period, chrg_sta);
        return;
    }

    drv_ext_leak_solar_link_load_bal_work(period, chrg_sta);
}

/***************************************************************************************
 * @Function    : drv_ext_hook_func()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/19
 ****************************************************************************************/
static void drv_ext_hook_func(ext_leak_hook_func hook)
{
    ext_leak_conf.param.hook_func = hook;
}

/***************************************************************************************
 * @Function    : drv_set_plug_sta()
 *
 * @Param       : plug_sta: 0 idle  1 chrg
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/19
 ****************************************************************************************/
static void drv_set_plug_sta(uint8_t plug_idx, uint8_t plug_sta)
{
    uint8_t flag = 0;

    if (plug_idx >= BSP_POLE_PLUG_NUM)
        return;

    /* 开始充电 */
    if (plug_sta == 1)
    {
        plug_dev.plug_dev[plug_dev.chrg_dev_cnt].plug_idx = plug_idx;
        plug_dev.plug_dev[plug_dev.chrg_dev_cnt].chrg_curr = 0;
        plug_dev.plug_dev[plug_dev.chrg_dev_cnt].set_curr = 0;
        plug_dev.chrg_dev_cnt++;
        return;
    }

    /* 结束充电 */
    for (uint8_t i = 0; i < plug_dev.chrg_dev_cnt; i++)
    {
        if (plug_dev.plug_dev[i].plug_idx == plug_idx)
        {
            flag = 1;
        }
        if (flag == 0)
            continue;

        if ((i + 1) < i < plug_dev.chrg_dev_cnt)
        {
            plug_dev.plug_dev[i] = plug_dev.plug_dev[i + 1];
        }
    }

    if (flag == 1 && plug_dev.chrg_dev_cnt > 0)
    {
        plug_dev.chrg_dev_cnt--;
    }
}

/***************************************************************************************
 * @Function    : dev_ext_get_leak_resume_flag()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2024/02/24
 ****************************************************************************************/
static uint8_t dev_ext_get_leak_resume_flag(void)
{
    return ext_leak_conf.procc.resume_flag;
}

/***************************************************************************************
 * @Function    : cmp_ext_leak_local_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/19
 ****************************************************************************************/
static void cmp_ext_leak_local_dev_create(void)
{
    static cmp_ext_leak_dev_stu ext_leak;

    ext_leak.drv_set_plug_sta = drv_set_plug_sta;

    ext_leak.drv_set_curr_hook_func = drv_ext_hook_func;

    ext_leak.drv_ext_leak_work = drv_ext_leak_work;

    ext_leak.drv_ext_set_param = dev_ext_set_param;

    ext_leak.drv_ext_set_resume_adj_flag = dev_ext_set_resume_adj_flag;

    ext_leak.drv_ext_set_station_set_curr = dev_ext_set_station_set_curr;

    ext_leak.drv_ext_get_leak_resume_flag = dev_ext_get_leak_resume_flag;

    cmp_dev_append(CMP_DEV_NAME_EXT_LEAK, &ext_leak);
}

/***************************************************************************************
 * @Function    : cmp_ext_leak_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/19
 ****************************************************************************************/
static int cmp_ext_leak_dev_create(void)
{
    cmp_ext_leak_conf_init();

    cmp_ext_leak_local_dev_create();

    return 0;
}
INIT_ENV_EXPORT(cmp_ext_leak_dev_create);