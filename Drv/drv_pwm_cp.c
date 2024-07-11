#include "drv_pwm_cp.h"


static drv_pwm_cp_stu pwm_cp_conf[BSP_POLE_PLUG_NUM];

/***************************************************************************************
 * @Function    : drv_pwm_cp_set_duty0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/22
 ****************************************************************************************/
static void drv_pwm_cp_set_duty0(uint16_t duty)
{
    if (duty == 1000)
        return;
    if (duty < 100)
        duty = 100;
    pwm_cp_conf[0].cp_pwm->bsp_pwm_cp_adjust(duty);
}

/***************************************************************************************
 * @Function    : drv_pwm_cp_stop0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/22
 ****************************************************************************************/
static void drv_pwm_cp_stop0(void)
{
    pwm_cp_conf[0].cp_pwm->bsp_pwm_cp_adjust(1000);
}

/***************************************************************************************
 * @Function    : drv_pwm_cp_conf_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/22
 ****************************************************************************************/
static void drv_pwm_cp_conf_init(void)
{
    pwm_cp_conf[0].cp_pwm = bsp_dev_find(BSP_DEV_NAME_PWM_CP);
}

/***************************************************************************************
 * @Function    : drv_pwm_cp_loacl_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/22
 ****************************************************************************************/
static void drv_pwm_cp_loacl_dev_create(void)
{
    static drv_pwm_cp_dev_stu pwm_cp_dev[BSP_POLE_PLUG_NUM];

    pwm_cp_dev[0].drv_dev_set_duty = drv_pwm_cp_set_duty0;

    pwm_cp_dev[0].drv_dev_stop = drv_pwm_cp_stop0;

    drv_dev_append(DRV_DEV_PWM_CP, &pwm_cp_dev);
}

/***************************************************************************************
 * @Function    : drv_pwm_cp_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/22
 ****************************************************************************************/
static int drv_pwm_cp_dev_create(void)
{
    drv_pwm_cp_conf_init();

    drv_pwm_cp_loacl_dev_create();

    return 0;
}
INIT_COMPONENT_EXPORT(drv_pwm_cp_dev_create);

