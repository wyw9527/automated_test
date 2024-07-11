#include "cmp_store_tim.h"

static cmp_store_tim_stu store_tim;
static cmp_store_tims_stu tim_conf;

/***************************************************************************************
 * @Function    : cmp_store_tim_query()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/2
 ****************************************************************************************/
static const cmp_store_tim_info_stu *cmp_store_tim_query(void)
{
    return (&store_tim.info);
}

/***************************************************************************************
 * @Function    : cmp_store_tim_check_peak_id()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/10
 ****************************************************************************************/
static int8_t cmp_store_tim_check_peak_id(cmp_store_tim_stu *tm, cmp_store_tim_info_ct_stu *tim)
{
    uint8_t idx;
    uint32_t base_id = 0;

    /* 判断是否为非高峰时段的ID */
    if (*((uint32_t *)tim->no) != 0x11)
    {
        return -1;
    }

    for (idx = 0; idx < CMP_STORE_TIM_PEAK_CNT; idx++)
    {
        base_id += 0x11000000;
        if (memcmp(&tim->no[4], (uint8_t *)&base_id, 4) == 0)
            break;
    }
    /* 不是高峰时段ID */
    if (idx >= CMP_STORE_TIM_PEAK_CNT)
        return -1;

    for (idx = 0; idx < tm->info.peak_cnt; idx++)
    {
        if (memcmp(tm->info.peak[idx].no, tim->no, sizeof(tm->info.peak[idx].no)) == 0)
        {
            return idx;
        }
    }
    tm->info.peak_cnt++;
    return idx;
}

/***************************************************************************************
 * @Function    : cmp_store_tim_add()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/2
 ****************************************************************************************/
static uint8_t cmp_store_tim_add(cmp_store_tim_info_ct_stu *tim)
{
    int8_t idx;
    cmp_store_tim_stu tim_s;

    memcpy((uint8_t *)&tim_s, (uint8_t *)&store_tim, sizeof(tim_s));

    /* peak tim */
    if ((idx = cmp_store_tim_check_peak_id(&tim_s, tim)) >= 0)
    {
        if (idx >= CMP_STORE_TIM_PEAK_CNT)
            return 2;
        memcpy((uint8_t *)&tim_s.info.peak[idx], (uint8_t *)tim, sizeof(cmp_store_tim_info_ct_stu));
    }
    else if (tim->plan == 0x0)
    {
        /* tim once check and handle*/
        memcpy((uint8_t *)&tim_s.info.single, (uint8_t *)tim, sizeof(cmp_store_tim_info_ct_stu));
    }
    else if ((tim->plan & 0x01) > 0)
    {
        /* tim exit check */
        for (idx = 0; idx < tim_s.info.period_cnt; idx++)
        {
            if (memcmp(tim_s.info.period[idx].no, tim->no, sizeof(tim_s.info.period[idx].no)) == 0)
            {
                break;
            }
        }
        if (idx < tim_s.info.period_cnt)
        {
            // tim exit
            memcpy((uint8_t *)&tim_s.info.period[idx], (uint8_t *)tim, sizeof(cmp_store_tim_info_ct_stu));
        }
        else
        {
            // tim not exit
            if (idx >= CMP_STORE_TIM_PERIOD_CNT)
            {
                return 2;
            }
            memcpy((uint8_t *)&tim_s.info.period[idx], (uint8_t *)tim, sizeof(cmp_store_tim_info_ct_stu));
            tim_s.info.period_cnt++;
        }
    }
    else
    {
        return 1;
    }

    /* tim info save */
    if (tim_conf.ext_flash->drv_flash_erase_sector(tim_conf.param.tim_addr) > 0)
    {
        return 1;
    }
    if (tim_conf.ext_flash->drv_flash_write_data(tim_conf.param.tim_addr, (uint8_t *)&tim_s, sizeof(tim_s)) > 0)
    {
        return 1;
    }

    /* upate info */
    memcpy((uint8_t *)&store_tim, (uint8_t *)&tim_s, sizeof(tim_s));

    return 0;
}

/***************************************************************************************
 * @Function    : cmp_store_tim_single_operation()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/11
 ****************************************************************************************/
static uint8_t cmp_store_tim_single_operation(cmp_store_tim_info_ct_stu *tim_c, CMP_STORE_TIM_OPER_TYPE oper, uint8_t *tim_no)
{
    if (memcmp(tim_c->no, tim_no, sizeof(tim_c->no)) != 0)
        return 0;

    if (oper == CMP_STORE_TIM_OPER_DISABLE)
    {
        tim_c->enabel = 0;
    }
    else if (oper == CMP_STORE_TIM_OPER_ENABLE)
    {
        tim_c->enabel = 1;
    }
    else
    {
        tim_c->plan = 0xff;
    }
    return 1;
}

/***************************************************************************************
 * @Function    : cmp_store_tim_comm_operation()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/11
 ****************************************************************************************/
static uint8_t cmp_store_tim_comm_operation(cmp_store_tim_info_ct_stu *tim_c, uint8_t tim_cnt, CMP_STORE_TIM_OPER_TYPE oper, uint8_t *tim_no)
{
    uint8_t idx;

    for (idx = 0; idx < tim_cnt; idx++)
    {
        if (memcmp(tim_no, tim_c[idx].no, sizeof(tim_c[idx].no)) == 0)
        {
            break;
        }
    }
    if (idx >= tim_cnt)
    {
        return 0;
    }

    if (oper == CMP_STORE_TIM_OPER_DISABLE)
    {
        tim_c[idx].enabel = 0;
    }
    else if (oper == CMP_STORE_TIM_OPER_ENABLE)
    {
        tim_c[idx].enabel = 1;
    }
    else if (oper == CMP_STORE_TIM_OPER_DEL)
    {
        for (uint8_t i = 0; i < tim_cnt - 1; i++)
        {
            if (i < idx)
            {
                continue;
            }
            memcpy((uint8_t *)&tim_c[i], (uint8_t *)&tim_c[i + 1], sizeof(cmp_store_tim_info_ct_stu));
        }
        memset((uint8_t *)&tim_c[tim_cnt - 1], 0, sizeof(cmp_store_tim_info_ct_stu));
    }
    return 1;
}

/***************************************************************************************
 * @Function    : cmp_store_tim_operation()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/2
 ****************************************************************************************/
static uint8_t cmp_store_tim_operation(CMP_STORE_TIM_OPER_TYPE oper, uint8_t *tim_no)
{
    cmp_store_tim_stu tim_s;

    if (oper >= CMP_STORE_TIM_OPER_NUM)
        return 3;

    memcpy((uint8_t *)&tim_s, (uint8_t *)&store_tim, sizeof(tim_s));

    if (cmp_store_tim_comm_operation(tim_s.info.period, tim_s.info.period_cnt, oper, tim_no) > 0)
    {
        if (oper == CMP_STORE_TIM_OPER_DEL && tim_s.info.period_cnt > 0)
        {
            tim_s.info.period_cnt--;
        }
    }
    else if (cmp_store_tim_comm_operation(tim_s.info.peak, tim_s.info.peak_cnt, oper, tim_no) > 0)
    {
        if (oper == CMP_STORE_TIM_OPER_DEL && tim_s.info.peak_cnt > 0)
        {
            tim_s.info.peak_cnt--;
        }
    }
    else
    {
        if (cmp_store_tim_single_operation(&tim_s.info.single, oper, tim_no) == 0)
        {
            return 1;
        }
    }

    /* tim info save */
    if (tim_conf.ext_flash->drv_flash_erase_sector(tim_conf.param.tim_addr) > 0)
    {
        return 2;
    }
    if (tim_conf.ext_flash->drv_flash_write_data(tim_conf.param.tim_addr, (uint8_t *)&tim_s, sizeof(tim_s)) > 0)
    {
        return 2;
    }

    /* upate info */
    memcpy((uint8_t *)&store_tim, (uint8_t *)&tim_s, sizeof(tim_s));

    return 0;
}

/***************************************************************************************
 * @Function    : cmp_store_tim_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/2
 ****************************************************************************************/
static uint8_t cmp_store_tim_init(void)
{
    if (tim_conf.ext_flash->drv_flash_read_data(tim_conf.param.tim_addr, (uint8_t *)&store_tim, sizeof(store_tim)) > 0)
    {
        return 2;
    }

    if ((store_tim.store_flag == CMP_STORE_FLAG) && (store_tim.info.period_cnt <= CMP_STORE_TIM_PERIOD_CNT) && (store_tim.info.peak_cnt <= CMP_STORE_TIM_PEAK_CNT))
    {
        return 0;
    }

    memset((uint8_t *)&store_tim, 0, sizeof(store_tim));
    store_tim.store_flag = CMP_STORE_FLAG;
    store_tim.info.single.plan = 0xff;
    if (tim_conf.ext_flash->drv_flash_erase_sector(tim_conf.param.tim_addr) > 0)
    {
        return 3;
    }
    if (tim_conf.ext_flash->drv_flash_write_data(tim_conf.param.tim_addr, (uint8_t *)&store_tim, sizeof(store_tim)) > 0)
    {
        return 3;
    }

    return 0;
}

/***************************************************************************************
 * @Function    : cmp_store_tim_is_time_reach()
 *
 * @Param       :
 *
 * @Return      : -1 time not reach  0 reach  1 tmout
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/12
 ****************************************************************************************/
static int8_t cmp_store_tim_is_time_reach(cmp_store_tim_info_ct_stu *tim_c)
{
    uint16_t mins, cur_mins;
    drv_clock_calendar_stu calendar;

    if (tim_c->enabel != 0x01)
        return 0;

    mins = math_bcd_2_decimal(tim_c->start_hour) * 60 + math_bcd_2_decimal(tim_c->start_min);
    tim_conf.clock->drv_clock_get(&calendar);
    cur_mins = calendar.hour * 60 + calendar.min;

    if (tim_c->plan & 0x01)
    {
        if ((tim_c->plan & (1 << calendar.week)) == 0)
        {
            return 0;
        }
    }

    if (mins == cur_mins)
    {
        if (tim_c->state == 0)
        {
            tim_c->state = 1;
            return 1;
        }
        return 0;
    }

    if (cur_mins > mins)
    {
        tim_c->state = 1;
        return 0;
    }

    tim_c->state = 0;
    return -1;
}

/***************************************************************************************
 * @Function    : cmp_store_tim_get_vival()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/12
 ****************************************************************************************/
static cmp_store_tim_info_ct_stu *cmp_store_tim_get_vival(uint8_t *tim_type)
{
    /* 单次 */
    if (store_tim.info.single.enabel == 0x01)
    {
        if (cmp_store_tim_is_time_reach(&store_tim.info.single) > 0)
        {
            cmp_store_tim_operation(CMP_STORE_TIM_OPER_DEL, store_tim.info.single.no);
            *tim_type = 1;
            return &store_tim.info.single;
        }
    }

    /* 周期 */
    for (uint8_t i = 0; i < store_tim.info.period_cnt; i++)
    {
        if (cmp_store_tim_is_time_reach(&store_tim.info.period[i]) > 0)
        {
            *tim_type = 2;
            return &store_tim.info.period[i];
        }
    }

    /* 非高峰 */
    if (tim_conf.param.peak_en != 0x01)
        return RT_NULL;
    for (uint8_t i = 0; i < store_tim.info.peak_cnt; i++)
    {
        if (cmp_store_tim_is_time_reach(&store_tim.info.peak[i]) > 0)
        {
            *tim_type = 3;
            return &store_tim.info.peak[i];
        }
    }

    return RT_NULL;
}

/***************************************************************************************
 * @Function    : cmp_tim_set_ext_flash()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/10/9
 ****************************************************************************************/
static void cmp_tim_set_ext_flash(drv_flash_dev_stu *ext_flash, drv_clock_dev_stu *clock, uint8_t type)
{
    tim_conf.ext_flash = ext_flash;
    tim_conf.clock = clock;

    tim_conf.param.tim_addr = type == 0 ? CMP_STORE_EXT_ADDR_TIM : CMP_STORE_INN_ADDR_TIM;

    cmp_store_tim_init();
}

/***************************************************************************************
 * @Function    : cmp_stroe_tim_set_peak()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/10/9
 ****************************************************************************************/
static void cmp_stroe_tim_set_peak(uint8_t peak)
{
    tim_conf.param.peak_en = peak;
}

/***************************************************************************************
 * @Function    : cmp_store_tim_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/10/9
 ****************************************************************************************/
int cmp_store_tim_subdev_create(void)
{
    static cmp_store_tim_dev_stu tim_dev;

    tim_dev.cmp_set_ext_flash = cmp_tim_set_ext_flash;
    tim_dev.cmp_store_tim_add = cmp_store_tim_add;
    tim_dev.cmp_store_tim_get_vival = cmp_store_tim_get_vival;
    tim_dev.cmp_store_tim_oper = cmp_store_tim_operation;
    tim_dev.cmp_store_tim_query = cmp_store_tim_query;
    tim_dev.cmp_store_tim_set_peak = cmp_stroe_tim_set_peak;
    cmp_dev_append(CMP_DEV_NAME_TIM, &tim_dev);
    return 0;
}
// INIT_COMPONENT_EXPORT(cmp_store_tim_dev_create);
