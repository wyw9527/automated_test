#include "drv_led.h"



static drv_led_stu led_conf;
static uint16_t led_buff[DRV_LED_BUFF_SIZE];

/***************************************************************************************
 * @Function    : drv_led_get_color_dot()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/25
 ****************************************************************************************/
static uint32_t drv_led_get_color_dot(DRV_LED_COLOR_TYPE color, uint8_t brightness)
{
    uint8_t color_r = 0, color_g = 0, color_b = 0;
    switch (color)
    {
    case DRV_LED_COLOR_RED:
        color_r = brightness;
        break;

    case DRV_LED_COLOR_GREEN:
        color_g = brightness;
        break;

    case DRV_LED_COLOR_BLUE:
        color_b = brightness;
        break;

    case DRV_LED_COLOR_YELLOW:
        color_r = brightness;
        color_g = brightness;
        break;

    default:
        return 0;
    }

    return (color_g << 16 | color_r << 8 | color_b);
}

/***************************************************************************************
 * @Function    : drv_led_write_dot_color()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/25
 ****************************************************************************************/
static void drv_led_write_dot_color(uint8_t dot_idx, DRV_LED_COLOR_TYPE color, uint8_t brightness)
{
    uint32_t color_rgb;

    if (dot_idx >= DRV_LED_DOT_NUM)
        return;

    color_rgb = drv_led_get_color_dot(color, brightness);
    for (uint8_t i = 0; i < 24; i++)
    {
        led_buff[dot_idx * 24 + 23 - i] = (color_rgb & 0x01) ? DRV_LED_T1_COUNT : DRV_LED_T0_COUNT;
        color_rgb >>= 1;
    }
}

/***************************************************************************************
 * @Function    : drv_led_light_all_node()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/25
 ****************************************************************************************/
static void drv_led_light_all_node(DRV_LED_COLOR_TYPE color, uint16_t brightness)
{
    for (uint8_t i = 0; i < DRV_LED_DOT_NUM; i++)
    {
        drv_led_write_dot_color(i, color, brightness);
    }
}

/***************************************************************************************
 * @Function    : drv_led_breath()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/25
 ****************************************************************************************/
static void drv_led_breath(DRV_LED_COLOR_TYPE color)
{
    led_conf.procc.color = color;
    led_conf.procc.cmd = 2;
    led_conf.procc.count = 0;
    led_conf.procc.flag = 0;
    led_conf.procc.brightness = led_conf.param.brightness;
}

/***************************************************************************************
 * @Function    : drv_led_off()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/25
 ****************************************************************************************/
static void drv_led_off()
{
    drv_led_light_all_node(DRV_LED_COLOR_BLACK, 0);
}

/***************************************************************************************
 * @Function    : drv_led_flash()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/25
 ****************************************************************************************/
static void drv_led_flash(DRV_LED_COLOR_TYPE color)
{
    led_conf.procc.color = color;
    led_conf.procc.cmd = 1;
    led_conf.procc.count = 0;
}

/***************************************************************************************
 * @Function    : drv_led_shift()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/25
 ****************************************************************************************/
static void drv_led_shift(DRV_LED_COLOR_TYPE color)
{
    led_conf.procc.color = color;
    led_conf.procc.cmd = 3;
    led_conf.procc.count = 0;
    led_conf.procc.flag = 0;
}

/***************************************************************************************
 * @Function    : drv_led_light()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/25
 ****************************************************************************************/
static void drv_led_light(DRV_LED_COLOR_TYPE color)
{
    led_conf.procc.cmd = 0;
    led_conf.procc.color = color;
}

/***************************************************************************************
 * @Function    : drv_led_light_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2023/11/17
 ****************************************************************************************/
static void drv_led_light_work(void)
{
    if (led_conf.procc.cmd != 0)
        return;
    drv_led_light_all_node(led_conf.procc.color, led_conf.param.brightness);
}

/***************************************************************************************
 * @Function    : drv_led_flash_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/25
 ****************************************************************************************/
static void drv_led_flash_work(void)
{
    if (led_conf.procc.cmd != 1)
        return;

    if (led_conf.procc.count > led_conf.param.flash_speed)
    {
        if (led_conf.procc.flag > 0)
        {
            drv_led_light(led_conf.procc.color);
            led_conf.procc.flag = 0;
        }
        else
        {
            drv_led_off();
            led_conf.procc.flag = 1;
        }
        led_conf.procc.count = 0;
    }

    led_conf.procc.count += led_conf.param.period;
}

/***************************************************************************************
 * @Function    : drv_led_breath_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/25
 ****************************************************************************************/
static void drv_led_breath_work(void)
{
    if (led_conf.procc.cmd != 2)
        return;

    if (led_conf.param.brightness <= 2)
    {
        led_conf.procc.brightness = led_conf.param.brightness;
        drv_led_light_all_node(DRV_LED_COLOR_BLACK, 0);
        return;
    }

    if (led_conf.procc.flag == 0)
    {
        if (led_conf.procc.brightness < led_conf.param.brightness)
        {
            led_conf.procc.count += led_conf.param.period;
            if (led_conf.procc.count > led_conf.param.breath_speed)
            {
                drv_led_light_all_node(led_conf.procc.color, led_conf.procc.brightness);
                led_conf.procc.brightness += 3;
                led_conf.procc.count = 0;
            }
        }
        else
        {
            led_conf.procc.flag++;
            led_conf.procc.count = 0;
        }
    }
    else if (led_conf.procc.flag == 1)
    {
        led_conf.procc.count += led_conf.param.period;
        if (led_conf.procc.count > led_conf.param.breath_speed)
        {
            led_conf.procc.flag++;
            led_conf.procc.count = 0;
        }
    }
    else if (led_conf.procc.flag == 2)
    {
        if (led_conf.procc.brightness > 3)
        {
            led_conf.procc.count += led_conf.param.period;
            if (led_conf.procc.count > led_conf.param.breath_speed)
            {
                drv_led_light_all_node(led_conf.procc.color, led_conf.procc.brightness);
                led_conf.procc.brightness -= 3;
                led_conf.procc.count = 0;
            }
        }
        else
        {
            led_conf.procc.flag++;
            led_conf.procc.count = 0;
        }
    }
    else if (led_conf.procc.flag == 3)
    {
        led_conf.procc.count += led_conf.param.period;
        if (led_conf.procc.count > led_conf.param.breath_speed)
        {
            led_conf.procc.flag = 0;
            led_conf.procc.count = 0;
        }
    }
}

/***************************************************************************************
 * @Function    : drv_led_shift_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/25
 ****************************************************************************************/
static void drv_led_shift_work(void)
{
    uint8_t diff;

    if (led_conf.procc.cmd != 3)
        return;

    led_conf.procc.count += led_conf.param.period;
    if (led_conf.procc.count <= led_conf.param.shift_speed)
    {
        return;
    }
    led_conf.procc.count = 0;

    for (uint8_t i = 0; i < DRV_LED_DOT_NUM; i++)
    {
        diff = (led_conf.procc.flag - i + DRV_LED_DOT_NUM) % DRV_LED_DOT_NUM;

        if (diff < 5)
        {
            diff *= 20;
            if (led_conf.param.brightness >= diff)
            {
                led_conf.procc.brightness = led_conf.param.brightness - diff;
            }
        }
        else
        {
            led_conf.procc.brightness = led_conf.param.brightness == BRIGHTNESS_DFT ? 5 : 0;
        }
        drv_led_write_dot_color(i, led_conf.procc.color, led_conf.procc.brightness);
    }

    led_conf.procc.flag = (led_conf.procc.flag + 1) % DRV_LED_DOT_NUM;
}

/***************************************************************************************
 * @Function    : drv_led_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/25
 ****************************************************************************************/
static void drv_led_work(uint8_t period)
{
    led_conf.param.period = period;

    drv_led_light_work();

    drv_led_flash_work();

    drv_led_breath_work();

    drv_led_shift_work();
}

/***************************************************************************************
 * @Function    : drv_led_set_brightness()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/25
 ****************************************************************************************/
static void drv_led_set_brightness(uint8_t brightness)
{
    led_conf.param.brightness = brightness;
    led_conf.param.breath_speed = 5100 / brightness;
}

/***************************************************************************************
 * @Function    : drv_led_set_speed()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/25
 ****************************************************************************************/
static void drv_led_set_speed(DRV_LED_SPEED_TYPE type, uint16_t speed)
{
    switch (type)
    {
    case DRV_LED_SPEED_FLASH:
        led_conf.param.flash_speed = speed;
        break;
    case DRV_LED_SPEED_SHIFT:
        led_conf.param.shift_speed = speed;
        break;
    case DRV_LED_SPEED_BREATH:
        led_conf.param.breath_speed = speed;
        break;
    }
}

/***************************************************************************************
 * @Function    : drv_led_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/25
 ****************************************************************************************/

static int drv_led_dev_create(void)
{
    static drv_led_dev_stu led_dev;

    led_dev.drv_led_breath = drv_led_breath;
    led_dev.drv_led_off = drv_led_off;
    led_dev.drv_led_flash = drv_led_flash;
    led_dev.drv_led_shift = drv_led_shift;
    led_dev.drv_led_light = drv_led_light;
    led_dev.drv_led_work = drv_led_work;
    led_dev.drv_led_set_brightness = drv_led_set_brightness;
    led_dev.drv_led_set_speed = drv_led_set_speed;

    led_conf.pwm = bsp_dev_find(BSP_DEV_NAME_PWM_LED);
    if (led_conf.pwm == RT_NULL)
    {
        led_conf.status = 1;
    }
    else
    {
        led_conf.param.brightness = BRIGHTNESS_DFT;
        led_conf.param.flash_speed = 500;
        led_conf.param.breath_speed = 20;
        led_conf.param.shift_speed = 50;
        led_conf.pwm->bsp_pwm_led_set_buff(led_buff, DRV_LED_BUFF_SIZE);
        led_conf.pwm->bsp_pwm_led_start();
        led_conf.status = 0;
    }

    drv_dev_append(DRV_DEV_LED, &led_dev);
    return 0;
}
INIT_COMPONENT_EXPORT(drv_led_dev_create);

