#include "bsp_io.h"

static bsp_io_stu io_conf[BSP_IO_NUM] =
    {
        BSP_IO_MODE_DO, GPIOD, GPIO_PIN_0, BSP_SOURCE_DO_RUN_LED,            // BSP_DO_RUN_LED
        BSP_IO_MODE_DO, GPIOA, GPIO_PIN_4, BSP_SOURCE_DO_LOAD_RS485_CTRL,     // BSP_DO_LOAD_RS485_CTRL
        BSP_IO_MODE_DO, GPIOA, GPIO_PIN_12, BSP_SOURCE_DO_CC_CP_SW,          // BSP_DO_CC_CP_SW
        BSP_IO_MODE_DO, GPIOD, GPIO_PIN_1, BSP_SOURCE_DO_ESP32_RESET,        // BSP_DO_ESP32_RESET
        BSP_IO_MODE_DO, GPIOE, GPIO_PIN_12, BSP_SOURCE_DO_RELAY_CTRL1,       // BSP_DO_AC_RELAY_CTRL_L1
        BSP_IO_MODE_DO, GPIOE, GPIO_PIN_13, BSP_SOURCE_DO_RELAY_CTRL2,       // BSP_DO_AC_RELAY_CTRL_L2
        BSP_IO_MODE_DO, GPIOC, GPIO_PIN_6, BSP_SOURCE_DO_RELAY_PWR,          // BSP_DO_AC_RELAY_PWR
        BSP_IO_MODE_DO, GPIOB, GPIO_PIN_11, BSP_SOURCE_DO_AC_LEAK_PROD,      // BSP_DO_AC_LEAK_PRODUCE
        BSP_IO_MODE_DO, GPIOD, GPIO_PIN_15, BSP_SOURCE_DO_AC_LEAK_SELF_TEST, // BSP_DO_AC_LEAK_SELF_TEST

        BSP_IO_MODE_DI, GPIOB, GPIO_PIN_1, BSP_SOURCE_DI_RELAY_STA1, // BSP_DI_RELAY_STA_L1
        BSP_IO_MODE_DI, GPIOB, GPIO_PIN_2, BSP_SOURCE_DI_RELAY_STA2, // BSP_DI_RELAY_STA_L2

        BSP_IO_MODE_DI, GPIOD, GPIO_PIN_14, BSP_SOURCE_DI_PNC_STA1,   // BSP_DI_PNC_STA
        BSP_IO_MODE_DI, GPIOE, GPIO_PIN_2, BSP_SOURCE_DI_OPENLID_STA, // BSP_DI_OPENLID_STA
};


/***************************************************************************************
 * @Function    : bsp_io_get_gpio_source()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/10
 ****************************************************************************************/
static uint8_t bsp_io_get_gpio_source(uint32_t gpio)
{
    switch (gpio)
    {
    case GPIOA:
        return GPIO_PORT_SOURCE_GPIOA;
    case GPIOB:
        return GPIO_PORT_SOURCE_GPIOB;
    case GPIOC:
        return GPIO_PORT_SOURCE_GPIOC;
    case GPIOD:
        return GPIO_PORT_SOURCE_GPIOD;
    case GPIOE:
        return GPIO_PORT_SOURCE_GPIOE;
    case GPIOF:
        return GPIO_PORT_SOURCE_GPIOF;
    case GPIOG:
        return GPIO_PORT_SOURCE_GPIOG;
    }

    return 0;
}

/***************************************************************************************
 * @Function    : bsp_io_get_pin_source()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/10
 ****************************************************************************************/
static uint8_t bsp_io_get_pin_source(uint32_t pin)
{
    switch (pin)
    {
    case GPIO_PIN_0:
        return GPIO_PIN_SOURCE_0;
    case GPIO_PIN_1:
        return GPIO_PIN_SOURCE_1;
    case GPIO_PIN_2:
        return GPIO_PIN_SOURCE_2;
    case GPIO_PIN_3:
        return GPIO_PIN_SOURCE_3;
    case GPIO_PIN_4:
        return GPIO_PIN_SOURCE_4;
    case GPIO_PIN_5:
        return GPIO_PIN_SOURCE_5;
    case GPIO_PIN_6:
        return GPIO_PIN_SOURCE_6;
    case GPIO_PIN_7:
        return GPIO_PIN_SOURCE_7;
    case GPIO_PIN_8:
        return GPIO_PIN_SOURCE_8;
    case GPIO_PIN_9:
        return GPIO_PIN_SOURCE_9;
    case GPIO_PIN_10:
        return GPIO_PIN_SOURCE_10;
    case GPIO_PIN_11:
        return GPIO_PIN_SOURCE_11;
    case GPIO_PIN_12:
        return GPIO_PIN_SOURCE_12;
    case GPIO_PIN_13:
        return GPIO_PIN_SOURCE_13;
    case GPIO_PIN_14:
        return GPIO_PIN_SOURCE_14;
    case GPIO_PIN_15:
        return GPIO_PIN_SOURCE_15;
    }
    return 0;
}

/***************************************************************************************
 * @Function    : bsp_io_get_exit_irq()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/10
 ****************************************************************************************/
static IRQn_Type bsp_io_get_exit_irq(uint32_t pin)
{
    switch (pin)
    {
    case GPIO_PIN_0:
        return EXTI0_IRQn;
    case GPIO_PIN_1:
        return EXTI1_IRQn;
    case GPIO_PIN_2:
        return EXTI2_IRQn;
    case GPIO_PIN_3:
        return EXTI3_IRQn;
    case GPIO_PIN_4:
        return EXTI4_IRQn;
    case GPIO_PIN_5:
    case GPIO_PIN_6:
    case GPIO_PIN_7:
    case GPIO_PIN_8:
    case GPIO_PIN_9:
        return EXTI5_9_IRQn;
    case GPIO_PIN_10:
    case GPIO_PIN_11:
    case GPIO_PIN_12:
    case GPIO_PIN_13:
    case GPIO_PIN_14:
    case GPIO_PIN_15:
        return EXTI10_15_IRQn;
    }
    return NonMaskableInt_IRQn;
}

/***************************************************************************************
 * @Function    : bsp_io_get_exit_line()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/10
 ****************************************************************************************/
static exti_line_enum bsp_io_get_exit_line(uint32_t pin)
{
    switch (pin)
    {
    case GPIO_PIN_0:
        return EXTI_0;
    case GPIO_PIN_1:
        return EXTI_1;
    case GPIO_PIN_2:
        return EXTI_2;
    case GPIO_PIN_3:
        return EXTI_3;
    case GPIO_PIN_4:
        return EXTI_4;
    case GPIO_PIN_5:
        return EXTI_5;
    case GPIO_PIN_6:
        return EXTI_6;
    case GPIO_PIN_7:
        return EXTI_7;
    case GPIO_PIN_8:
        return EXTI_8;
    case GPIO_PIN_9:
        return EXTI_9;
    case GPIO_PIN_10:
        return EXTI_10;
    case GPIO_PIN_11:
        return EXTI_11;
    case GPIO_PIN_12:
        return EXTI_12;
    case GPIO_PIN_13:
        return EXTI_13;
    case GPIO_PIN_14:
        return EXTI_14;
    case GPIO_PIN_15:
        return EXTI_15;
    }
    return EXTI_17;
}

/***************************************************************************************
 * @Function    : bsp_io_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/10
 ****************************************************************************************/
static uint8_t bsp_io_init(BSP_SOURCE_TYPE type)
{
    rcu_periph_enum pin_rcu;

    BSP_IO_TYPE io = BSP_IO_NUM;

    for (uint8_t i = 0; i < BSP_IO_NUM; i++)
    {
        if (io_conf[i].type == type)
        {
            io = (BSP_IO_TYPE)i;
        }
    }

    if (io >= BSP_IO_NUM)
        return 1;

    pin_rcu = bsp_get_pin_rcu(io_conf[io].gpio_periph);
    if (pin_rcu == 0)
    {
        return 0;
    }
    rcu_periph_clock_enable(pin_rcu);

    if (io_conf[io].mode == BSP_IO_MODE_DI)
    {
        gpio_init(io_conf[io].gpio_periph, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_10MHZ, io_conf[io].pin);
    }
    else if (io_conf[io].mode == BSP_IO_MODE_DO)
    {
        gpio_init(io_conf[io].gpio_periph, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, io_conf[io].pin);
    }
    else if (io_conf[io].mode == BSP_IO_MODE_INT)
    {
        rcu_periph_clock_enable(RCU_AF);
        gpio_init(io_conf[io].gpio_periph, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, io_conf[io].pin);
        nvic_irq_enable(bsp_io_get_exit_irq(io_conf[io].pin), 2U, 0U);
        gpio_exti_source_select(bsp_io_get_gpio_source(io_conf[io].gpio_periph), bsp_io_get_pin_source(io_conf[io].pin));
        exti_init(bsp_io_get_exit_line(io_conf[io].pin), EXTI_INTERRUPT, EXTI_TRIG_RISING);
    }
    else
    {
        return 1;
    }

    return 0;
}

/***************************************************************************
*@Function    :bsp_io_exti_irq_handler
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-10-10
***************************************************************************/

void EXTI3_IRQHandler(void)
{
    if (RESET != exti_interrupt_flag_get(EXTI_3))
    {
        exti_interrupt_flag_clear(EXTI_3);
    }
}

void EXTI5_9_IRQHandler(void)
{
    if (RESET != exti_interrupt_flag_get(EXTI_7))
    {
        exti_interrupt_flag_clear(EXTI_7);
    }
    if (RESET != exti_interrupt_flag_get(EXTI_9))
    {
        exti_interrupt_flag_clear(EXTI_9);
    }
}

void EXTI10_15_IRQHandler(void)
{
    if (RESET != exti_interrupt_flag_get(EXTI_10))
    {
        exti_interrupt_flag_clear(EXTI_10);
    }
}

/***************************************************************************
 * FuncName     : bsp_do_ctrl()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/3/25
 ***************************************************************************/
static uint8_t bsp_do_ctrl(BSP_IO_TYPE io, uint8_t cmd)
{
    if (io >= BSP_IO_NUM)
        return 1;

    if (io_conf[io].mode != BSP_IO_MODE_DO)
    {
        return 1;
    }

    if (cmd == 1)
    {
        gpio_bit_set(io_conf[io].gpio_periph, io_conf[io].pin);
    }
    else
    {
        gpio_bit_reset(io_conf[io].gpio_periph, io_conf[io].pin);
    }
    return 0;
}

/***************************************************************************************
 * @Function    : bsp_io_set()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/29
 ****************************************************************************************/
static uint8_t bsp_io_set(BSP_SOURCE_TYPE type)
{
    BSP_IO_TYPE io = BSP_IO_NUM;

    for (uint8_t i = 0; i < BSP_IO_NUM; i++)
    {
        if (io_conf[i].type == type)
        {
            io = (BSP_IO_TYPE)i;
            break;
        }
    }

    return bsp_do_ctrl(io, 1);
}

/***************************************************************************************
 * @Function    : bsp_io_reset()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/29
 ****************************************************************************************/
static uint8_t bsp_io_reset(BSP_SOURCE_TYPE type)
{

    BSP_IO_TYPE io = BSP_IO_NUM;

    for (uint8_t i = 0; i < BSP_IO_NUM; i++)
    {
        if (io_conf[i].type == type)
        {
            io = (BSP_IO_TYPE)i;
            break;
        }
    }

    return bsp_do_ctrl(io, 0);
}

/***************************************************************************
 * FuncName     : bsp_di_val_get()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/3/25
 ***************************************************************************/
static uint8_t bsp_di_val_get(BSP_SOURCE_TYPE type)
{
    BSP_IO_TYPE io = BSP_IO_NUM;

    for (uint8_t i = 0; i < BSP_IO_NUM; i++)
    {
        if (io_conf[i].type == type)
        {
            io = (BSP_IO_TYPE)i;
            break;
        }
    }

    if (io >= BSP_IO_NUM)
        return 0;

    if (io_conf[io].mode != BSP_IO_MODE_DI)
    {
        return 0;
    }

    return gpio_input_bit_get(io_conf[io].gpio_periph, io_conf[io].pin) == RESET ? 0 : 1;
}

/***************************************************************************
*@Function    :
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-10-10
***************************************************************************/
static uint8_t bsp_io_exit_set(BSP_SOURCE_TYPE type, func_exit_handler_hook exit_hook)
{
    BSP_IO_TYPE io = BSP_IO_NUM;

    for (uint8_t i = 0; i < BSP_IO_NUM; i++)
    {
        if (io_conf[i].type == type)
        {
            io = (BSP_IO_TYPE)i;
            break;
        }
    }
    if (io >= BSP_IO_NUM)
        return 1;

    // io_exit_conf[io].exit_hook = exit_hook;
    return 0;
}

/***************************************************************************************
 * @Function    : bsp_io_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/15
 ****************************************************************************************/
int bsp_io_dev_create(void)
{
    static bsp_io_dev_stu io_dev;

    io_dev.bsp_io_init = bsp_io_init;
    io_dev.bsp_io_get_value = bsp_di_val_get;
    io_dev.bsp_io_set = bsp_io_set;
    io_dev.bsp_io_reset = bsp_io_reset;
    io_dev.bsp_io_exit_func_set = bsp_io_exit_set;

    bsp_dev_append(BSP_DEV_NAME_IO, &io_dev);

    return 1;
}

