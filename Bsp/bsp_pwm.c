#include "bsp_pwm.h"


static bsp_pwm_oper_stu pwm_oper;

/***************************************************************************************
 * @Function    : bsp_pwm_cp_pin_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/24
 ****************************************************************************************/
static void bsp_pwm_cp_pin_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_AF);

    gpio_pin_remap_config(GPIO_TIMER0_FULL_REMAP, ENABLE);
    gpio_init(GPIOE, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
}

/***************************************************************************************
 * @Function    : bsp_pwm_led_pin_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/24
 ****************************************************************************************/
static void bsp_pwm_led_pin_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOC);
    gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
}

/***************************************************************************************
 * @Function    : bsp_pwm_pin_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/24
 ****************************************************************************************/
static void bsp_pwm_pin_init(void)
{
    bsp_pwm_cp_pin_init();

    bsp_pwm_led_pin_init();
}

/***************************************************************************************
 * @Function    : bsp_pwm_cp_param_config.()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/24
 ****************************************************************************************/
static void bsp_pwm_cp_param_config(void)
{
    timer_oc_parameter_struct tm_oc_stu;
    timer_parameter_struct tm_init_stu;

    rcu_periph_clock_enable(RCU_TIMER0);
    timer_deinit(TIMER0);

    /* TIMER0 configuration */
    tm_init_stu.prescaler = 119;                     // 预分频系数
    tm_init_stu.period = 999;                        // 自动重载值
    tm_init_stu.alignedmode = TIMER_COUNTER_EDGE;    // 计数器对齐模式：边沿对齐
    tm_init_stu.counterdirection = TIMER_COUNTER_UP; // 计数器计数方向：向上计数

    tm_init_stu.clockdivision = TIMER_CKDIV_DIV1; // DTS 时间分频值
    tm_init_stu.repetitioncounter = 0;            // 重复计数值
    timer_init(TIMER0, &tm_init_stu);

    /*CH3 configueration in PWM mode */
    tm_oc_stu.outputstate = TIMER_CCX_ENABLE;          // 通道使能
    tm_oc_stu.outputnstate = TIMER_CCXN_DISABLE;       // 通道互补输出使能：无效
    tm_oc_stu.ocpolarity = TIMER_OC_POLARITY_HIGH;     // 通道极性
    tm_oc_stu.ocnpolarity = TIMER_OCN_POLARITY_HIGH;   // 互补通道极性
    tm_oc_stu.ocidlestate = TIMER_OC_IDLE_STATE_LOW;   // 通道空闲状态输出
    tm_oc_stu.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW; // 互补通道空闲状态输出
    timer_channel_output_config(TIMER0, TIMER_CH_3, &tm_oc_stu);

    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_3, 250);                // 通道占空比设置
    timer_channel_output_mode_config(TIMER0, TIMER_CH_3, TIMER_OC_MODE_PWM0);        // PWM模式0
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_3, TIMER_OC_SHADOW_DISABLE); // 不使用输出影子比较器

    timer_primary_output_config(TIMER0, ENABLE);
    timer_auto_reload_shadow_enable(TIMER0); // 自动装载影子比较器使能
    timer_enable(TIMER0);
}

/***************************************************************************************
 * @Function    : bsp_pwm_cp_config()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/24
 ****************************************************************************************/
static void bsp_pwm_cp_config(void)
{
    bsp_pwm_cp_param_config();
}

/***************************************************************************************
 * @Function    : bsp_pwm_led_dma_config()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/24
 ****************************************************************************************/
static void bsp_pwm_led_dma_config()
{
    dma_parameter_struct dma_init_struct;
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA1);

    /* initialize DMA channel0 */
    dma_deinit(DMA1, DMA_CH0);
    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr = (uint32_t)RT_NULL;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_init_struct.number = 0;
    dma_init_struct.periph_addr = (uint32_t)(&TIMER_CH2CV(TIMER7));
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA1, DMA_CH0, &dma_init_struct);

    dma_circulation_enable(DMA1, DMA_CH0);

    /* enable DMA channel0 */
    dma_channel_enable(DMA1, DMA_CH0);
}

/***************************************************************************************
 * @Function    : bsp_pwm_led_param_config()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/24
 ****************************************************************************************/
static void bsp_pwm_led_param_config(void)
{
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER7);
    timer_deinit(TIMER7);

    /* TIMER7 configuration */
    timer_initpara.prescaler = 1; // 1 / 120Mhz = 0.00833us
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period = 74; // 1.25us / (1 / 120Mhz) = 150
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER7, &timer_initpara);

    /* CH2 configuration in PWM mode */
    timer_channel_output_struct_para_init(&timer_ocintpara);
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
    timer_channel_output_config(TIMER7, TIMER_CH_2, &timer_ocintpara);

    /* CH2 configuration in PWM mode0, duty cycle 0% */
    timer_channel_output_pulse_value_config(TIMER7, TIMER_CH_2, 0);
    timer_channel_output_mode_config(TIMER7, TIMER_CH_2, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER7, TIMER_CH_2, TIMER_OC_SHADOW_DISABLE);

    /* TIMER7 update DMA enable */
    timer_dma_enable(TIMER7, TIMER_DMA_UPD);

    /* TIMER7 configure primary output function */
    timer_primary_output_config(TIMER7, ENABLE);
}

/***************************************************************************************
 * @Function    : bsp_pwm_led_config()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/24
 ****************************************************************************************/
static void bsp_pwm_led_config(void)
{
    bsp_pwm_led_dma_config();

    bsp_pwm_led_param_config();
}

/***************************************************************************************
 * @Function    : bsp_pwm_config()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/25
 ****************************************************************************************/
static void bsp_pwm_config(void)
{
    bsp_pwm_cp_config();

    bsp_pwm_led_config();
}

/***************************************************************************************
 * @Function    : bsp_pwm_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/25
 ****************************************************************************************/
static void bsp_pwm_init(void)
{
    bsp_pwm_pin_init();

    bsp_pwm_config();
}

/***************************************************************************************
 * @Function    : bsp_pwm_cp_adjust()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/25
 ****************************************************************************************/
static void bsp_pwm_cp_adjust(uint16_t duty)
{
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_3, duty);

    if (duty == 1000)
    {
        pwm_oper.cp_enable = 0;
    }
    else
    {
        pwm_oper.cp_enable = 1;
    }
}

/***************************************************************************************
 * @Function    : bsp_pwm_led_start()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/25
 ****************************************************************************************/
static void bsp_pwm_led_start(void)
{
    timer_enable(TIMER7);
    pwm_oper.led_enable = 1;
}

/***************************************************************************************
 * @Function    : bsp_pwm_led_stop()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/25
 ****************************************************************************************/
static void bsp_pwm_led_stop(void)
{
    timer_disable(TIMER7);
    pwm_oper.led_enable = 0;
}

/***************************************************************************************
 * @Function    : bsp_pwm_led_set_buff()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/25
 ****************************************************************************************/
static void bsp_pwm_led_set_buff(uint16_t *buff, uint16_t size)
{
    dma_channel_disable(DMA1, DMA_CH0);
    dma_memory_address_config(DMA1, DMA_CH0, (uint32_t)buff);
    dma_transfer_number_config(DMA1, DMA_CH0, size);
    dma_channel_enable(DMA1, DMA_CH0);
}

/***************************************************************************************
 * @Function    : bsp_pwm_dev_ctreate()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/25
 ****************************************************************************************/
int bsp_pwm_dev_ctreate(void)
{
    static bsp_pwm_cp_dev_stu pwm_cp;
    static bsp_pwm_led_dev_stu pwm_led;

    bsp_pwm_init();

    pwm_cp.bsp_pwm_cp_adjust = bsp_pwm_cp_adjust;
    bsp_dev_append(BSP_DEV_NAME_PWM_CP, &pwm_cp);

    pwm_led.bsp_pwm_led_start = bsp_pwm_led_start;
    pwm_led.bsp_pwm_led_stop = bsp_pwm_led_stop;
    pwm_led.bsp_pwm_led_set_buff = bsp_pwm_led_set_buff;
    bsp_dev_append(BSP_DEV_NAME_PWM_LED, &pwm_led);

    bsp_bind_oper_data_dev(0xff, 203, 0, &pwm_oper.cp_enable, 1);
    bsp_bind_oper_data_dev(0xff, 203, 1, &pwm_oper.led_enable, 1);

    return 0;
}

