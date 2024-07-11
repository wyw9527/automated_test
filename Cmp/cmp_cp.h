#ifndef _CMP_CP_H_
#define _CMP_CP_H_

#include "cmp.h"

#define CMP_CP_OFFSET 8
#define CMP_CP_12V_UP (120 + CMP_CP_OFFSET)
#define CMP_CP_12V_DOWN (120 - CMP_CP_OFFSET)
#define CMP_CP_9V_UP (90 + CMP_CP_OFFSET)
#define CMP_CP_9V_DOWN (90 - CMP_CP_OFFSET)
#define CMP_CP_6V_UP (60 + CMP_CP_OFFSET)
#define CMP_CP_6V_DOWN (60 - CMP_CP_OFFSET)

typedef struct
{
    uint8_t sta;
    uint8_t sta_pre;
    uint8_t count;
    uint16_t cp_val;
    uint16_t abn_tmr;  // 异常次数
    uint16_t pwm_duty; // 输出PWM
    uint8_t cp_fault;

} cmp_cp_procc_stu;

typedef struct
{
    drv_analog_dev_stu *samp; /* cp */

    drv_pwm_cp_dev_stu *pwm;

    cmp_cp_procc_stu procc;

} cmp_cp_stu;

typedef struct
{
    uint8_t (*cmp_get_sta)(void);

    void (*cmp_dev_work)(uint8_t period);

    void (*cmp_set_pwm_duty)(uint16_t duty);

    void (*cmp_pwm_stop)(void);
} cmp_cp_dev_stu;

#endif