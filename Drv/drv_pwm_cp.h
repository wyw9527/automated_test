#ifndef _DRV_PWM_CP_H_
#define _DRV_PWM_CP_H_

#include "drv.h"

typedef struct
{
    bsp_pwm_cp_dev_stu *cp_pwm;

} drv_pwm_cp_stu;

typedef struct
{
    void (*drv_dev_set_duty)(uint16_t duty);
    void (*drv_dev_stop)(void);
} drv_pwm_cp_dev_stu;

#endif