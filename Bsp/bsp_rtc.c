#include "bsp_rtc.h"


static uint32_t rtc_seconds = 0;

/***************************************************************************************
 * @Function    : bsp_rtc_config()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/16
 ****************************************************************************************/
static uint8_t bsp_rtc_config(void)
{
    /* enable PMU and BKPI clocks */
    rcu_periph_clock_enable(RCU_BKPI);
    rcu_periph_clock_enable(RCU_PMU);

    /* allow access toBKP domain */
    pmu_backup_write_enable();

    /* reset backup domain */
    bkp_deinit();

    /* enable LXTAL */
    rcu_osci_on(RCU_LXTAL);

    /* wait until LXTAL is ready */
    if (ERROR == rcu_osci_stab_wait(RCU_LXTAL))
        return 1;

    /* select RCU_LXTAL as RTC clock source */
    rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);

    /* enable RTC clock */
    rcu_periph_clock_enable(RCU_RTC);

    /* wait for RTC registers synchronization */
    if (rtc_register_sync_wait() == 1)
        return 1;
    // rtc_register_sync_wait();

    /* wait until last write operation on RTC registers has finished */
    if (rtc_lwoff_wait() > 0)
    {
        return 1;
    }

    /* enable the RTC second interrupt */
    // rtc_interrupt_enable(RTC_INT_SECOND);

    /* wait until last write operation on RTC registers has finished */
    // rtc_lwoff_wait();

    /* set RTC prescaler: set RTC period to 1s */
    // rtc_prescaler_set(62500);
    rtc_prescaler_set(32767);

    /* wait until last write operation on RTC registers has finished */
    if (rtc_lwoff_wait() > 0)
        return 1;

    return 0;
}

/***************************************************************************************
 * @Function    : bsp_rtc_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/16
 ****************************************************************************************/
static uint8_t bsp_rtc_init(void)
{
    if (bkp_read_data(BKP_DATA_0) != 0xA5A5)
    {
        if (bsp_rtc_config() == 1)
            return 1;

        bkp_write_data(BKP_DATA_0, 0xA5A5);
    }
    else
    {
        /* allow access to BKP domain */
        rcu_periph_clock_enable(RCU_PMU);
        pmu_backup_write_enable();

        /* wait for RTC registers synchronization */
        if (rtc_register_sync_wait() == 1)
            return 1;

        /* enable the RTC second */
        rtc_interrupt_enable(RTC_INT_SECOND);

        /* wait until last write operation on RTC registers has finished */
        if (rtc_lwoff_wait() == 1)
            return 1;
    }
    return 0;
}

/***************************************************************************************
 * @Function    : bsp_rtc_set_calendar()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/16
 ****************************************************************************************/
static void bsp_rtc_set_time(uint32_t seconds)
{
    rtc_lwoff_wait();
    rtc_counter_set(seconds);
    rtc_lwoff_wait();
}

/***************************************************************************************
 * @Function    : bsp_rtc_get_time()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/31
 ****************************************************************************************/
static uint32_t bsp_rtc_get_time(void)
{
    rtc_seconds = rtc_counter_get();

    return rtc_seconds;
}

/***************************************************************************************
 * @Function    : bsp_rtc_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/16
 ****************************************************************************************/
int bsp_rtc_create(void)
{
    static bsp_rtc_dev_stu rtc_dev;

    if (bsp_rtc_init() > 0)
    {
        bsp_alarm_mb_send(0xff, 201, 0, 2);
    }

    rtc_dev.get_time_seconds = bsp_rtc_get_time;

    rtc_dev.set_time_seconds = bsp_rtc_set_time;

    bsp_dev_append(BSP_DEV_NAME_RTC, &rtc_dev);

    bsp_bind_oper_data_dev(0xff, 201, 1, (uint8_t *)&rtc_seconds, 4);
    bsp_bind_oper_data_dev(0xff, 201, 0, 0, 1);

    return 0;
}

