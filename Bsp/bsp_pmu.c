#include "bsp_pmu.h"


static bsp_powerdown_callback_func_type powerdown_callback[BSP_PD_NUM] = {RT_NULL};

/***************************************************************************************
 * @Function    : bsp_pmu_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/8
 ****************************************************************************************/
static void bsp_pmu_init(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);

    nvic_irq_enable(LVD_IRQn, 0, 0);

    /* enable clock */
    rcu_periph_clock_enable(RCU_PMU);

    /* EXTI_16 configuration */
    exti_init(EXTI_16, EXTI_INTERRUPT, EXTI_TRIG_RISING);

    while (RESET != pmu_flag_get(PMU_FLAG_LVD))
        ;
    /* configure the LVD threshold to 3.0v */
    pmu_lvd_select(PMU_LVDT_6);
}

/***************************************************************************************
 * @Function    : bsp_powerdown_callback_func()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/8
 ****************************************************************************************/
static void bsp_powerdown_callback_func(void)
{
    for (uint8_t i = 0; i < BSP_PD_NUM; i++)
    {
        if (powerdown_callback[i] == RT_NULL)
            continue;

        powerdown_callback[i]();
    }
}

/***************************************************************************************
 * @Function    : LVD_IRQHandler()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/8
 ****************************************************************************************/
void LVD_IRQHandler(void)
{
    if (RESET != exti_interrupt_flag_get(EXTI_16))
    {
        bsp_powerdown_callback_func();

        exti_interrupt_flag_clear(EXTI_16);
    }
}

/***************************************************************************************
 * @Function    : bsp_powerdown_callback_hook()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/8
 ****************************************************************************************/
void bsp_powerdown_callback_hook(BSP_PD_TYPE pd_type, bsp_powerdown_callback_func_type func)
{
    powerdown_callback[pd_type] = func;
}

/***************************************************************************************
 * @Function    : bsp_param_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/8
 ****************************************************************************************/
static void bsp_param_init(void)
{
    for (uint8_t i = 0; i < BSP_PD_NUM; i++)
    {
        powerdown_callback[i] = RT_NULL;
    }
}

/***************************************************************************************
 * @Function    : bsp_pmu_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/8
 ****************************************************************************************/
int bsp_pmu_dev_create(void)
{
    bsp_pmu_init();

    bsp_param_init();

    return 0;
}


