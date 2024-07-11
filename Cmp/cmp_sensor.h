#ifndef _CMP_SENSOR_H_
#define _CMP_SENSOR_H_

#include "cmp.h"

typedef struct
{
    uint16_t count;
    uint16_t alarm_cnt;
    uint16_t val;
} cmp_sensor_procc_dat_stu;

typedef struct
{
    uint16_t leak_count;
    uint16_t leak_alarm_cnt;
    uint16_t leak_val;
    uint16_t ne_count;
    uint16_t ne_alarm_cnt;
    uint16_t ne_val;
    uint32_t temp_ev_count;
    uint16_t temp_ev_alarm_cnt;
    uint16_t temp_ev_val;
    uint16_t eme_count;
    uint16_t eme_alarm_count;
    uint16_t eme_val;
    uint16_t pnc_count;
    uint16_t pnc_act_cnt;
    uint8_t pnc_val;
    cmp_sensor_procc_dat_stu pe;
    cmp_sensor_procc_dat_stu openlid;

} cmp_sensor_procc_data_stu;

typedef union
{
    uint16_t total;
    struct
    {
        uint16_t leak : 1;
        uint16_t ne : 1;
        uint16_t temp_ev : 2;
        uint16_t eme : 1;
        uint16_t pnc : 1;
        uint16_t pe : 1;
        uint16_t openlid : 1;
    } bits;
} cmp_sensor_procc_alarm_stu;

typedef struct
{
    /* 是否存在此设备 */
    uint16_t leak : 1;
    uint16_t leak_seft : 1;
    uint16_t ne : 1;
    uint16_t pe : 1;
    uint16_t di_ne : 1;
    uint16_t temp_ev : 1;
    uint16_t temp_pv : 1;
    uint16_t eme : 1;
    uint16_t pnc : 1;
    uint16_t open_lid : 1;

} cmp_sensor_sub_dev_stu;

typedef struct
{
    cmp_sensor_procc_data_stu data;

    cmp_sensor_procc_alarm_stu alarm;

    cmp_sensor_sub_dev_stu dev_exit; // 设备存在状态
} cmp_sensor_procc_stu;

typedef struct
{
    uint16_t leak_th;
} cmp_sensor_param_stu;

typedef struct
{
    drv_analog_dev_stu *leak; // DRV_DEV_ANALOG_LEAK

    drv_analog_dev_stu *ne; // DRV_DEV_ANALOG_NE

    drv_analog_dev_stu *pe;

    drv_analog_dev_stu *temp_ev; // DRV_DEV_ANALOG_TMP_B

    drv_analog_dev_stu *temp_pv; // DRV_DEV_ANALOG_TMP_P

    drv_dido_di_dev_stu *eme; // DRV_DEV_DIDO_EME

    drv_dido_di_dev_stu *di_ne;

    drv_dido_di_dev_stu *pnc;

    drv_dido_di_dev_stu *openlid;

    drv_dido_do_dev_stu *leak_seft;

    cmp_sensor_procc_stu procc;

    cmp_sensor_param_stu param;

} cmp_sensor_stu;

typedef struct
{
    void (*sensor_work)(uint8_t period);
    uint8_t (*get_pnc_sta)();
} cmp_sensor_dev_stu;

#endif