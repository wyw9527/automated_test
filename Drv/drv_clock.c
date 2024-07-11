#include "drv_clock.h"


static drv_clock_stu clock_dev;

/***************************************************************************************
 * @Function    : drv_clock_conv_seconds_to_calendar()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/31
 ****************************************************************************************/
static void drv_clock_conv_seconds_to_calendar(uint32_t seconds, drv_clock_calendar_stu *calendar_t)
{
    // 定义一个 time_t 类型的变量，用于存储从1970年1月1日至今的秒数
    time_t seconds_since_epoch = seconds;

    // 将 seconds_since_epoch 转换为 struct tm 类型
    struct tm *time_info = localtime(&seconds_since_epoch);

    calendar_t->year = time_info->tm_year + 1900;
    calendar_t->month = time_info->tm_mon + 1;
    calendar_t->week = time_info->tm_wday;
    if (calendar_t->week == 0)
    {
        calendar_t->week = 7;
    }
    calendar_t->day = time_info->tm_mday;
    calendar_t->hour = time_info->tm_hour;
    calendar_t->min = time_info->tm_min;
    calendar_t->second = time_info->tm_sec;
}

/***************************************************************************************
 * @Function    : drv_clock_conv_calendar_to_seconds()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/31
 ****************************************************************************************/
static uint32_t drv_clock_conv_calendar_to_seconds(drv_clock_calendar_stu *calendar_t)
{
    // 将日历形式的日期和时间转换为秒数
    struct tm time_info = {0};
    time_info.tm_year = calendar_t->year - 1900; // 年份要减去2000
    time_info.tm_mon = calendar_t->month - 1;    // 月份要从0开始计数，因此要减去1
    time_info.tm_mday = calendar_t->day;
    time_info.tm_hour = calendar_t->hour;
    time_info.tm_min = calendar_t->min;
    time_info.tm_sec = calendar_t->second;
    time_t time_stamp = mktime(&time_info); // 将结构体转换为time_t类型

    // 计算总秒数
    return ((uint32_t)time_stamp); // time_t类型转换为int类型
}

/***************************************************************************************
 * @Function    : drv_clock_syn()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/17
 ****************************************************************************************/
static void drv_clock_syn(drv_clock_calendar_stu *calendar)
{
    uint32_t seconds;

    seconds = drv_clock_conv_calendar_to_seconds(calendar);

    clock_dev.rtc->set_time_seconds(seconds);
}

/***************************************************************************************
 * @Function    : drv_clock_get()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/17
 ****************************************************************************************/
static void drv_clock_get(drv_clock_calendar_stu *calendar_t)
{
    uint32_t seconds;

    seconds = clock_dev.rtc->get_time_seconds();
    drv_clock_conv_seconds_to_calendar(seconds, calendar_t);
}

/***************************************************************************************
 * @Function    : drv_clock_upt()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/31
 ****************************************************************************************/
static void drv_clock_upt(void)
{
    /* 周期定时测试代码 */
    static drv_clock_calendar_stu clock_tmp;
    static uint8_t flag_tmp = 0;
    if (flag_tmp != 0)
    {
        drv_clock_syn(&clock_tmp);
        flag_tmp = 0;
    }
    drv_clock_get(&clock_dev.calendar);
}

/***************************************************************************************
 * @Function    : drv_clock_get_datetime_diff()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/14
 ****************************************************************************************/
static int32_t drv_clock_get_calendar_diff(drv_clock_calendar_stu *cal_pre, drv_clock_calendar_stu *cal_next)
{
    uint32_t pre, next;

    pre = drv_clock_conv_calendar_to_seconds(cal_pre);
    next = drv_clock_conv_calendar_to_seconds(cal_next);

    return (next - pre);
}

/***************************************************************************************
 * @Function    : drv_clock_compare()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description : 0 相等  -1 小于当前时间  1 大于当前时间
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/17
 ****************************************************************************************/
static int8_t drv_clock_compare(drv_clock_calendar_stu *calendar)
{
    int8_t res;

    if (calendar == RT_NULL || clock_dev.rtc == RT_NULL)
        return 1;

    res = drv_clock_get_calendar_diff(&clock_dev.calendar, calendar);

    if (res > 0)
    {
        return 1;
    }
    else if (res == 0)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

/***************************************************************************************
 * @Function    : drv_clock_get_next_day()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/17
 ****************************************************************************************/
static void drv_clock_get_next_day(drv_clock_calendar_stu *calendar)
{
    uint32_t seconds;

    seconds = drv_clock_conv_calendar_to_seconds(calendar);
    seconds += 86400;
    drv_clock_conv_seconds_to_calendar(seconds, calendar);
}

/***************************************************************************************
 * @Function    : drv_conv_bcd_to_normal()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/7
 ****************************************************************************************/
static void drv_conv_bcd_to_normal(drv_bcd_datetime_stu *bcd_date, drv_clock_calendar_stu *calendar)
{
    calendar->year = math_bcd_2_decimal(bcd_date->year) + 2000;
    calendar->month = math_bcd_2_decimal(bcd_date->month);
    calendar->day = math_bcd_2_decimal(bcd_date->day);
    calendar->hour = math_bcd_2_decimal(bcd_date->hour);
    calendar->min = math_bcd_2_decimal(bcd_date->min);
    calendar->second = math_bcd_2_decimal(bcd_date->sec);
}

/***************************************************************************************
 * @Function    : drv_clock_syn_bcd()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/16
 ****************************************************************************************/
static void drv_clock_syn_bcd(drv_bcd_datetime_stu *bcd_date)
{
    int32_t diff;
    drv_clock_calendar_stu calendar;

    drv_conv_bcd_to_normal(bcd_date, &calendar);
    diff = drv_clock_get_calendar_diff(&clock_dev.calendar, &calendar);
    if (diff > -60 && diff < 60)
        return;

    drv_clock_syn(&calendar);
}

/***************************************************************************************
 * @Function    : drv_get_bcd_datetime()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/16
 ****************************************************************************************/
static void drv_get_bcd_datetime(drv_bcd_datetime_stu *bcd_date)
{
    bcd_date->year = math_decimal_2_bcd(clock_dev.calendar.year - 2000);
    bcd_date->month = math_decimal_2_bcd(clock_dev.calendar.month);
    bcd_date->day = math_decimal_2_bcd(clock_dev.calendar.day);
    bcd_date->hour = math_decimal_2_bcd(clock_dev.calendar.hour);
    bcd_date->min = math_decimal_2_bcd(clock_dev.calendar.min);
    bcd_date->sec = math_decimal_2_bcd(clock_dev.calendar.second);
}

/***************************************************************************************
 * @Function    : drv_clock_get_bcd_datetime_diff()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/7
 ****************************************************************************************/
static int32_t drv_clock_get_bcd_datetime_diff(drv_bcd_datetime_stu *bcd_pre, drv_bcd_datetime_stu *bcd_next)
{
    drv_clock_calendar_stu cale_pre, cale_next;

    drv_conv_bcd_to_normal(bcd_pre, &cale_pre);
    drv_conv_bcd_to_normal(bcd_next, &cale_next);

    return drv_clock_get_calendar_diff(&cale_pre, &cale_next);
}

/***************************************************************************************
 * @Function    : drv_clock_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/17
 ****************************************************************************************/
static int drv_clock_dev_create(void)
{
    static drv_clock_dev_stu clock;

    clock.drv_clock_upt = drv_clock_upt;
    clock.drv_clock_syn = drv_clock_syn;
    clock.drv_clock_get_next_day = drv_clock_get_next_day;
    clock.drv_clock_compare = drv_clock_compare;
    clock.drv_clock_get = drv_clock_get;
    clock.drv_get_bcd_datetime = drv_get_bcd_datetime;
    clock.drv_clock_syn_bcd = drv_clock_syn_bcd;
    clock.drv_clock_get_bcd_datetime_diff = drv_clock_get_bcd_datetime_diff;
    clock.drv_clock_get_calendar_diff = drv_clock_get_calendar_diff;

    clock_dev.rtc = bsp_dev_find(BSP_DEV_NAME_RTC);

    drv_dev_append(DRV_DEV_CLOCK, &clock);
    return 0;
}

INIT_COMPONENT_EXPORT(drv_clock_dev_create);
