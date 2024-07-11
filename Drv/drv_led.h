#ifndef _DRV_LED_H_
#define _DRV_LED_H_

#include "drv.h"

#define DRV_LED_DOT_NUM 9
#define DRV_LED_T1_COUNT 51
#define DRV_LED_T0_COUNT 24
#define DRV_LED_RESET_COUNT 95
#define BRIGHTNESS_DFT 255

#define DRV_LED_BUFF_SIZE DRV_LED_DOT_NUM * 24 + DRV_LED_RESET_COUNT

typedef enum
{
    DRV_LED_COLOR_BLACK,
    DRV_LED_COLOR_RED,
    DRV_LED_COLOR_GREEN,
    DRV_LED_COLOR_BLUE,
    DRV_LED_COLOR_YELLOW
} DRV_LED_COLOR_TYPE;

typedef enum
{
    DRV_LED_SPEED_FLASH,
    DRV_LED_SPEED_SHIFT,
    DRV_LED_SPEED_BREATH,
} DRV_LED_SPEED_TYPE;

typedef struct
{
    uint8_t cmd; /* 0 light 1 flash 2 breath 3 shift*/
    uint8_t flag;
    uint16_t count;
    DRV_LED_COLOR_TYPE color;
    uint8_t brightness;
} drv_led_procc_stu;

typedef struct
{
    uint8_t period;
    uint8_t brightness; /* 0 ~ 255*/
    uint16_t flash_speed;
    uint16_t shift_speed;
    uint16_t breath_speed;
} drv_led_param_stu;

typedef struct
{
    bsp_pwm_led_dev_stu *pwm;
    uint8_t status; /* 0 normal 1 find dev abn */
    drv_led_procc_stu procc;
    drv_led_param_stu param;
} drv_led_stu;

typedef struct
{
    void (*drv_led_breath)(DRV_LED_COLOR_TYPE color);

    void (*drv_led_light)(DRV_LED_COLOR_TYPE color);

    void (*drv_led_flash)(DRV_LED_COLOR_TYPE color);

    void (*drv_led_off)();

    void (*drv_led_shift)(DRV_LED_COLOR_TYPE color);

    void (*drv_led_set_brightness)(uint8_t brightness);

    void (*drv_led_set_speed)(DRV_LED_SPEED_TYPE type, uint16_t speed);

    void (*drv_led_work)(uint8_t period);

} drv_led_dev_stu;

#endif