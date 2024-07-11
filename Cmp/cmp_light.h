#ifndef _CMP_LIGHT_H_
#define _CMP_LIGHT_H_

#include "cmp.h"

typedef struct
{
    uint8_t chrg_led_sta;
    uint8_t run_led_sta;
    uint16_t run_led_count;
    uint8_t run_led_val;
    uint8_t chrg_led_type;
    uint8_t work_mode;

    uint8_t dot_led_sta;
    uint8_t dot_led_mode; // 0 Idle  1 Conned 2 Start 3 Work 4 Car_Pend 5 Stop 6 Fault 7 Pole_Pend 8 Resv
} cmp_light_procc_stu;

typedef struct
{
    /* data */
    drv_dido_do_dev_stu *led_r;
    drv_dido_do_dev_stu *led_g;
    drv_dido_do_dev_stu *led_b;
} cmp_dotlight_stu;

typedef struct
{
    drv_led_dev_stu *chrg_led;

    drv_dido_do_dev_stu *run_led;

    cmp_dotlight_stu dot_led;

    cmp_light_procc_stu procc;

} cmp_light_stu;

typedef struct
{
    void (*chrg_led_work)(uint8_t period, uint8_t sta, uint8_t work_mode);
    void (*chrg_led_set_brightness)(uint8_t brightness);
} cmp_light_dev_stu;

typedef struct
{
    void (*pwm_led_work)(uint8_t period, uint8_t sta, uint8_t work_mode);
} cmp_pwmled_dev_stu;

typedef struct
{
    void (*run_led_work)(uint8_t period);
} cmp_led_dev_stu;

#endif