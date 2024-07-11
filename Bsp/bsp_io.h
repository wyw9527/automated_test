#ifndef _BSP_IO_H_
#define _BSP_IO_H_

#include "bsp.h"

#define RUN_LED_ON gpio_bit_set(GPIOD, GPIO_PIN_0)
#define RUN_LED_OFF gpio_bit_reset(GPIOD, GPIO_PIN_0)

typedef enum
{
    BSP_DO_RUN_LED = 0,      // PD0
    BSP_DO_INV_RS485_CTRL,   // 逆变器&外部电表
    BSP_DO_CC_CP_SW,         /* CCCP开关 */
    BSP_DO_ESP32_RESET,      /* ESP32模块复位 */
    BSP_DO_AC_RELAY_CTRL_L1, /*交流接触器控制_L1 */
    BSP_DO_AC_RELAY_CTRL_L2, /*交流接触器控制_L2 */
    BSP_DO_AC_RELAY_PWR,     /*交流接触器电源 */

    BSP_DO_AC_LEAK_PRODUCE,   /*交流漏电流产生*/
    BSP_DO_AC_LEAK_SELF_TEST, /*漏电流自检*/

    BSP_DI_RELAY_STA_L1, /* 继电器反馈_L1 */
    BSP_DI_RELAY_STA_L2, /* 继电器反馈_L2 */
    BSP_DI_PNC_STA,      /* 即插即充停止 */
    BSP_DI_OPENLID_STA,  /*开盖检测*/

    BSP_IO_NUM
} BSP_IO_TYPE;

typedef void (*func_exit_handler_hook)(void);

typedef enum
{
    BSP_IO_MODE_DI,
    BSP_IO_MODE_DO,
    BSP_IO_MODE_INT,
    BSP_IO_MODE_DUMMY
} BSP_IO_MODE_TYPE;

typedef struct
{
    BSP_IO_MODE_TYPE mode;
    uint32_t gpio_periph;
    uint32_t pin;
    BSP_SOURCE_TYPE type;
} bsp_io_stu;

typedef struct
{
    func_exit_handler_hook exit_hook;
} bsp_io_exit_stu;

typedef struct
{
    uint8_t (*bsp_io_init)(BSP_SOURCE_TYPE type);
    uint8_t (*bsp_io_get_value)(BSP_SOURCE_TYPE type);
    uint8_t (*bsp_io_set)(BSP_SOURCE_TYPE type);
    uint8_t (*bsp_io_reset)(BSP_SOURCE_TYPE type);
    uint8_t (*bsp_io_exit_func_set)(BSP_SOURCE_TYPE type, func_exit_handler_hook exit_hook);
} bsp_io_dev_stu;

#endif