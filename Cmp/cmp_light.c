#include "cmp_light.h"

static cmp_light_stu light_conf;

/***************************************************************************************
 * @Function    : cmp_chrg_pwm_led_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/13
 ****************************************************************************************/
static void cmp_chrg_pwm_led_work(uint8_t period, uint8_t sta, uint8_t work_mode)
{
    if (light_conf.chrg_led == RT_NULL)
        return;
    light_conf.chrg_led->drv_led_work(period);

    if (sta == light_conf.procc.chrg_led_type && work_mode == light_conf.procc.work_mode)
        return;
    light_conf.procc.chrg_led_type = sta;
    light_conf.procc.work_mode = work_mode;

    if (work_mode == 2 && sta != 6)
    {
        light_conf.chrg_led->drv_led_light(DRV_LED_COLOR_YELLOW);
        return;
    }

    switch (sta)
    {
    case 0:
        light_conf.chrg_led->drv_led_breath(DRV_LED_COLOR_GREEN);
        break;
    case 1:
        light_conf.chrg_led->drv_led_light(DRV_LED_COLOR_BLUE);
        break;
    case 2:
        light_conf.chrg_led->drv_led_breath(DRV_LED_COLOR_BLUE);
        break;
    case 3:
        light_conf.chrg_led->drv_led_shift(DRV_LED_COLOR_BLUE);
        break;
    case 4:
        light_conf.chrg_led->drv_led_breath(DRV_LED_COLOR_BLUE);
        break;
    case 5:
        light_conf.chrg_led->drv_led_light(DRV_LED_COLOR_GREEN);
        break;
    case 6:
        light_conf.chrg_led->drv_led_light(DRV_LED_COLOR_RED);
        break;
    case 7:
        light_conf.chrg_led->drv_led_breath(DRV_LED_COLOR_BLUE);
        break;
    case 8:
        light_conf.chrg_led->drv_led_light(DRV_LED_COLOR_YELLOW);
        break;
    }
}

/***************************************************************************************
 * @Function    : cmp_chrg_run_led_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/13
 ****************************************************************************************/
static void cmp_chrg_run_led_work(uint8_t period)
{
    if (light_conf.procc.run_led_sta > 0)
        return;

    if (light_conf.procc.run_led_count > 300)
    {
        light_conf.procc.run_led_count = 0;
        light_conf.procc.run_led_val = !light_conf.procc.run_led_val;
        if (light_conf.procc.run_led_val == 0)
        {
            light_conf.run_led->drv_do_on();
        }
        else
        {
            light_conf.run_led->drv_do_off();
        }
    }

    light_conf.procc.run_led_count += period;
}

/***************************************************************************************
 * @Function    : cmp_chrg_dot_led_idle()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : WYW            Version   : V2.0.0                Date:2023/10/8
 ****************************************************************************************/
static void cmp_chrg_dot_led_idle()
{
    light_conf.dot_led.led_g->drv_do_on();
    light_conf.dot_led.led_b->drv_do_off();
    light_conf.dot_led.led_r->drv_do_off();
}

/***************************************************************************************
 * @Function    : cmp_chrg_dot_led_conned()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : WYW            Version   : V2.0.0                Date:2023/10/8
 ****************************************************************************************/
static void cmp_chrg_dot_led_conned(uint8_t period)
{
    static uint16_t count = 0;
    static uint8_t step = 0;

    if (step == 0 && count > 200)
    {
        light_conf.dot_led.led_g->drv_do_on();
        step = 1;
        count = 0;
    }
    else if (step == 1 && count > 200)
    {
        light_conf.dot_led.led_g->drv_do_off();
        step = 0;
        count = 0;
    }
    light_conf.dot_led.led_b->drv_do_off();
    light_conf.dot_led.led_r->drv_do_off();
    count += period;
}

/***************************************************************************************
 * @Function    : cmp_chrg_dot_led_start()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : WYW            Version   : V2.0.0                Date:2023/10/8
 ****************************************************************************************/
static void cmp_chrg_dot_led_start(uint8_t period)
{
    static uint16_t count = 0;
    static uint8_t step = 0;
    light_conf.dot_led.led_g->drv_do_off();

    if (step == 0 && count > 200)
    {
        light_conf.dot_led.led_b->drv_do_on();
        step = 1;
        count = 0;
    }
    else if (step == 1 && count > 200)
    {
        light_conf.dot_led.led_b->drv_do_off();
        step = 0;
        count = 0;
    }

    light_conf.dot_led.led_r->drv_do_off();
    count += period;
}

/***************************************************************************************
 * @Function    : cmp_chrg_dot_led_chrging()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : WYW            Version   : V2.0.0                Date:2023/10/8
 ****************************************************************************************/
static void cmp_chrg_dot_led_chrging()
{
    light_conf.dot_led.led_g->drv_do_off();

    light_conf.dot_led.led_b->drv_do_on();

    light_conf.dot_led.led_r->drv_do_off();
}

/***************************************************************************************
 * @Function    : cmp_chrg_dot_led_car_pend()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : WYW            Version   : V2.0.0                Date:2023/10/8
 ****************************************************************************************/
static void cmp_chrg_dot_led_car_pend(uint8_t period)
{
    static uint16_t count = 0;
    static uint8_t step = 0;
    light_conf.dot_led.led_g->drv_do_off();

    if (step == 0 && count > 200)
    {
        light_conf.dot_led.led_b->drv_do_on();
        step = 1;
        count = 0;
    }
    else if (step == 1 && count > 200)
    {
        light_conf.dot_led.led_b->drv_do_off();
        step = 0;
        count = 0;
    }

    light_conf.dot_led.led_r->drv_do_off();
    count += period;
}

/***************************************************************************************
 * @Function    : cmp_chrg_dot_led_stop()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : WYW            Version   : V2.0.0                Date:2023/10/8
 ****************************************************************************************/
static void cmp_chrg_dot_led_stop(uint8_t period)
{
    static uint16_t count = 0;
    static uint8_t step = 0;
    light_conf.dot_led.led_g->drv_do_off();

    if (step == 0 && count > 1000)
    {
        light_conf.dot_led.led_b->drv_do_on();
        step = 1;
        count = 0;
    }
    else if (step == 1 && count > 1000)
    {
        light_conf.dot_led.led_b->drv_do_off();
        step = 0;
        count = 0;
    }

    light_conf.dot_led.led_r->drv_do_off();
    count += period;
}

/***************************************************************************************
 * @Function    : cmp_chrg_dot_led_car_pend()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : WYW            Version   : V2.0.0                Date:2023/10/8
 ****************************************************************************************/
static void cmp_chrg_dot_led_fault()
{
    light_conf.dot_led.led_g->drv_do_off();

    light_conf.dot_led.led_b->drv_do_off();

    light_conf.dot_led.led_r->drv_do_on();
}

/***************************************************************************************
 * @Function    : cmp_chrg_dot_led_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : WYW            Version   : V2.0.0                Date:2023/10/8
 ****************************************************************************************/
static void cmp_chrg_dot_led_work(uint8_t period, uint8_t dot_mode)
{
    if (light_conf.dot_led.led_r == RT_NULL || light_conf.dot_led.led_g == RT_NULL || light_conf.dot_led.led_b == RT_NULL)
        return;
    if (light_conf.procc.dot_led_sta > 0)
        return;
    switch (dot_mode)
    {
    case 0:
        cmp_chrg_dot_led_idle();
        break;
    case 1:
        cmp_chrg_dot_led_conned(period);
        break;
    case 2:
        cmp_chrg_dot_led_start(period);
        break;
    case 3:
        cmp_chrg_dot_led_chrging();
        break;
    case 4:
        cmp_chrg_dot_led_car_pend(period);
        break;
    case 5:
        cmp_chrg_dot_led_stop(period);
        break;
    case 6:
        cmp_chrg_dot_led_fault();
        break;
    default:
        break;
    }
}

/***************************************************************************************
 * @Function    : cmp_light_chrg_led_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2023/11/17
 ****************************************************************************************/
static void cmp_light_chrg_led_work(uint8_t period, uint8_t sta, uint8_t work_mode)
{
    cmp_chrg_pwm_led_work(period, sta, work_mode);
    cmp_chrg_dot_led_work(period, sta);
}

/***************************************************************************************
 * @Function    : cmp_light_chrg_led_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2023/11/17
 ****************************************************************************************/
static void cmp_light_chrg_led_set_brightness(uint8_t brightness)
{
    if (brightness > 100)
        brightness = 100;

    uint8_t tmp = (uint8_t)(brightness * 2.55);

    light_conf.chrg_led->drv_led_set_brightness(tmp);
}

/***************************************************************************************
 * @Function    : cmp_local_dev_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/13
 ****************************************************************************************/
static void cmp_local_dev_init(void)
{
    light_conf.chrg_led = drv_dev_find(DRV_DEV_LED);
    if (light_conf.chrg_led == RT_NULL)
    {
        light_conf.procc.chrg_led_sta = 1;
    }
    light_conf.procc.chrg_led_type = 0xFF;

    light_conf.run_led = drv_dev_find(DRV_DEV_DIDO_RUN_LED);
    if (light_conf.run_led == RT_NULL)
    {
        light_conf.procc.run_led_sta = 1;
    }

    light_conf.dot_led.led_r = drv_dev_find(DRV_DEV_LED_R);
    light_conf.dot_led.led_g = drv_dev_find(DRV_DEV_LED_G);
    light_conf.dot_led.led_b = drv_dev_find(DRV_DEV_LED_B);

    if (light_conf.dot_led.led_r == RT_NULL || light_conf.dot_led.led_g == RT_NULL || light_conf.dot_led.led_b == RT_NULL)
    {
        light_conf.procc.dot_led_sta = 1;
    }
}

/***************************************************************************************
 * @Function    : cmp_light_dev_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/13
 ****************************************************************************************/
static void cmp_light_dev_init(void)
{
    static cmp_light_dev_stu light_dev;
    light_dev.chrg_led_work = cmp_light_chrg_led_work;
    light_dev.chrg_led_set_brightness = cmp_light_chrg_led_set_brightness;
    cmp_dev_append(CMP_DEV_NAME_LIGHT, &light_dev);

    static cmp_led_dev_stu led_dev;
    led_dev.run_led_work = cmp_chrg_run_led_work;
    cmp_dev_append(CMP_DEV_NAME_LED, &led_dev);
}

/***************************************************************************************
 * @Function    : cmp_light_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/13
 ****************************************************************************************/
static int cmp_light_dev_create(void)
{
    cmp_local_dev_init();

    cmp_light_dev_init();

    return 0;
}
INIT_ENV_EXPORT(cmp_light_dev_create);