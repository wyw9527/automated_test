#ifndef _DRV_CLOCK_H_
#define _DRV_CLOCK_H_

#include "time.h"
#include "drv.h"

typedef struct
{
    uint16_t year;
    uint32_t month : 4;
    uint32_t day : 5;
    uint32_t hour : 5;
    uint32_t min : 6;
    uint32_t second : 6;
    uint32_t week : 3;
} drv_clock_calendar_stu;

typedef struct
{
    /* data */
    uint8_t timing_flag; // 已对时 0xAA
} drv_clock_procc_stu;

typedef struct
{
    bsp_rtc_dev_stu *rtc;

    drv_clock_calendar_stu calendar;

    drv_clock_procc_stu procc;

} drv_clock_stu;

typedef struct
{
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
} drv_bcd_datetime_stu;

typedef struct
{
    void (*drv_clock_upt)();
    void (*drv_clock_syn)(drv_clock_calendar_stu *calendar);
    void (*drv_clock_get)(drv_clock_calendar_stu *calendar);
    int8_t (*drv_clock_compare)(drv_clock_calendar_stu *calendar);
    void (*drv_clock_get_next_day)(drv_clock_calendar_stu *calendar);
    void (*drv_get_bcd_datetime)(drv_bcd_datetime_stu *bcd_date);
    void (*drv_clock_syn_bcd)(drv_bcd_datetime_stu *bcd_date);
    int32_t (*drv_clock_get_bcd_datetime_diff)(drv_bcd_datetime_stu *bcd_pre, drv_bcd_datetime_stu *bcd_next);
    int32_t (*drv_clock_get_calendar_diff)(drv_clock_calendar_stu *cale_pre, drv_clock_calendar_stu *cale_next);
    void (*drv_clock_check_timing_flag)(void);
} drv_clock_dev_stu;

#endif