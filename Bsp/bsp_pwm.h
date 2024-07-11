#ifndef _BSP_PWM_H_
#define _BSP_PWM_H_



#include "bsp.h"


typedef struct
{
    void (* bsp_pwm_cp_adjust)(uint16_t duty);
}bsp_pwm_cp_dev_stu;

typedef struct
{
    uint8_t cp_enable;
    uint8_t led_enable;
}bsp_pwm_oper_stu;

typedef struct
{
    void (* bsp_pwm_led_start)(void);
    
    void (* bsp_pwm_led_stop)(void);
    
    void (* bsp_pwm_led_set_buff)(uint16_t* buff,uint16_t size);
}bsp_pwm_led_dev_stu;

























#endif