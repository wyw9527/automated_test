#ifndef _CMP_MEASURE_H_
#define _CMP_MEASURE_H_

#include "cmp.h"
#include "cmp_store.h"

#define CMP_MEASURE_ALARM_TMOUT 5000

typedef struct
{
    uint8_t mode;  /* 2 meter  1 m8209  0 analog */
    uint8_t phase; /* 0 single  1 three */
    const uint8_t *curr_th;
    uint8_t volt_dev_sta;
    uint8_t curr_dev_sta;
    uint8_t m8209_dev_sta;
    uint8_t meter_dev_sta;
} cmp_measure_pub_stu;

typedef struct
{
    uint16_t volt[3];
    uint16_t curr[3];
    uint16_t power;
    double energy;
    uint32_t energy_val;
    uint8_t volt_alarm[3]; /* 0 正常 1 欠压  2 过压 3 缺相*/
    uint8_t curr_alarm[3];
    uint16_t volt_count[3];
    uint16_t curr_count[3];
} cmp_measure_volume_stu;

typedef struct
{
    /* data */
    uint8_t chrg_sta;
} cmp_measure_procc_stu;

typedef struct
{
    drv_meter_dev_stu *meter;
    drv_m8209_dev_stu *m8209;
    drv_analog_dev_stu *volt_dev[3];
    drv_analog_dev_stu *curr_dev[3];
    cmp_measure_volume_stu volume;
    uint8_t idx;
    cmp_measure_procc_stu procc;
} cmp_measure_stu;

typedef struct
{
    uint8_t dev_fault;
    uint16_t ov_count;
    uint16_t uv_count;
    uint16_t oc_count;
} cmp_measure_oper_stu;

typedef struct
{
    uint8_t (*cmp_get_mode)(void);
    uint16_t (*cmp_get_chrg_volt)(uint8_t phase);
    uint16_t (*cmp_get_chrg_curr)(uint8_t phase);
    uint16_t (*cmp_get_chrg_max_curr)(void);
    uint32_t (*cmp_get_chrg_power)(void);
    uint32_t (*cmp_get_chrg_energy)(void);
    void (*cmp_cyc_work)(uint8_t period);
    void (*cmp_set_chrg_sta)(uint8_t val);
} cmp_measure_dev_stu;

int cmp_measure_dev_create(void);

#endif