#include "bsp_uart.h"


#define BSP_UART_REMAP_NONE 0

/* uart global info */
static bsp_uart_stu uart_info[BSP_UART_NUM];
static bsp_uart_oper_stu uart_oper[BSP_UART_NUM];

/* uart config parameter*/
static bsp_uart_param_stu uart_param[BSP_UART_NUM] =
    {
        {USART0, BSP_UART_REMAP_NONE},
        //        {USART1, GPIO_USART1_REMAP},
        {USART1, BSP_UART_REMAP_NONE},
        {USART2, GPIO_USART2_FULL_REMAP},
        {UART3, BSP_UART_REMAP_NONE}, // 此串口没有重隐射，配置时注意
        {UART4, BSP_UART_REMAP_NONE}, // 此串口没有重隐射，配置时注意
};

/***************************************************************************************
 * @Function    : bsp_uart_get_pin()
 *
 * @Param       :
 *
 * @Return      : 0 succ  1 input param abn  2 did not find pin
 *
 * @Description : GD32F305VE芯片固定引脚
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/4/12
 ****************************************************************************************/
static uint8_t bsp_uart_get_pin(BSP_UART_TYPE uart_x, uint32_t remap, bsp_uart_pin_stu *back_pin)
{
    if (back_pin == RT_NULL)
    {
        return 0;
    }

    switch (uart_x)
    {
    case BSP_UART0:
        if (remap == GPIO_USART0_REMAP)
        {
            back_pin->tx_gpio_periph = GPIOB;
            back_pin->tx_gpio_pin = GPIO_PIN_6;
            back_pin->rx_gpio_periph = GPIOB;
            back_pin->rx_gpio_pin = GPIO_PIN_7;
        }
        else
        {
            back_pin->tx_gpio_periph = GPIOA;
            back_pin->tx_gpio_pin = GPIO_PIN_9;
            back_pin->rx_gpio_periph = GPIOA;
            back_pin->rx_gpio_pin = GPIO_PIN_10;
        }
        break;

    case BSP_UART1:
        if (remap == GPIO_USART1_REMAP)
        {
            back_pin->tx_gpio_periph = GPIOD;
            back_pin->tx_gpio_pin = GPIO_PIN_5;
            back_pin->rx_gpio_periph = GPIOD;
            back_pin->rx_gpio_pin = GPIO_PIN_6;
        }
        else
        {
            back_pin->tx_gpio_periph = GPIOA;
            back_pin->tx_gpio_pin = GPIO_PIN_2;
            back_pin->rx_gpio_periph = GPIOA;
            back_pin->rx_gpio_pin = GPIO_PIN_3;
        }
        break;

    case BSP_UART2:
        if (remap == GPIO_USART2_PARTIAL_REMAP)
        {
            back_pin->tx_gpio_periph = GPIOC;
            back_pin->tx_gpio_pin = GPIO_PIN_10;
            back_pin->rx_gpio_periph = GPIOC;
            back_pin->rx_gpio_pin = GPIO_PIN_11;
        }
        else if (remap == GPIO_USART2_FULL_REMAP)
        {
            back_pin->tx_gpio_periph = GPIOD;
            back_pin->tx_gpio_pin = GPIO_PIN_8;
            back_pin->rx_gpio_periph = GPIOD;
            back_pin->rx_gpio_pin = GPIO_PIN_9;
        }
        else
        {
            back_pin->tx_gpio_periph = GPIOB;
            back_pin->tx_gpio_pin = GPIO_PIN_10;
            back_pin->rx_gpio_periph = GPIOB;
            back_pin->rx_gpio_pin = GPIO_PIN_11;
        }
        break;

    case BSP_UART3:
        back_pin->tx_gpio_periph = GPIOC;
        back_pin->tx_gpio_pin = GPIO_PIN_10;
        back_pin->rx_gpio_periph = GPIOC;
        back_pin->rx_gpio_pin = GPIO_PIN_11;
        break;

    case BSP_UART4:
        back_pin->tx_gpio_periph = GPIOC;
        back_pin->tx_gpio_pin = GPIO_PIN_12;
        back_pin->rx_gpio_periph = GPIOD;
        back_pin->rx_gpio_pin = GPIO_PIN_2;
        break;
    default:
        return 0;
    }

    return 1;
}

/***************************************************************************************
 * @Function    : bsp_uart_get_dma_info()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/1/18
 ****************************************************************************************/
static uint8_t bsp_uart_get_dma_info(BSP_UART_TYPE uart_x, bsp_uart_dma_stu *dma)
{
    if (dma == RT_NULL)
    {
        return 0;
    }

    if (uart_x == BSP_UART0)
    {
        dma->periph = DMA0;
        dma->tx_chann = DMA_CH3;
        dma->rx_chann = DMA_CH4;
        return 1;
    }

    if (uart_x == BSP_UART1)
    {
        dma->periph = DMA0;
        dma->tx_chann = DMA_CH6;
        dma->rx_chann = DMA_CH5;
        return 1;
    }

    if (uart_x == BSP_UART2)
    {
        dma->periph = DMA0;
        dma->tx_chann = DMA_CH1;
        dma->rx_chann = DMA_CH2;
        return 1;
    }

    if (uart_x == BSP_UART3)
    {
        dma->periph = DMA1;
        dma->tx_chann = DMA_CH4;
        dma->rx_chann = DMA_CH2;
        return 1;
    }

    dma->periph = 0;
    return 0;
}

/***************************************************************************************
 * @Function    : bsp_uart_get_irq()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/1/28
 ****************************************************************************************/
static uint8_t bsp_uart_get_irq(BSP_UART_TYPE uart_x, uint8_t *irq)
{
    if (irq == RT_NULL)
        return 0;

    if (uart_x == BSP_UART0)
    {
        *irq = USART0_IRQn;
        return 1;
    }

    if (uart_x == BSP_UART1)
    {
        *irq = USART1_IRQn;
        return 1;
    }

    if (uart_x == BSP_UART2)
    {
        *irq = USART2_IRQn;
        return 1;
    }

    if (uart_x == BSP_UART3)
    {
        *irq = UART3_IRQn;
        return 1;
    }

    if (uart_x == BSP_UART4)
    {
        *irq = UART4_IRQn;
        return 1;
    }
    return 0;
}

/***************************************************************************************
 * @Function    : bsp_uart_get_clock()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/1/28
 ****************************************************************************************/
static uint8_t bsp_uart_get_clock(BSP_UART_TYPE uart_x, rcu_periph_enum *clock)
{
    if (clock == RT_NULL)
        return 0;

    switch (uart_x)
    {
    case BSP_UART0:
        *clock = RCU_USART0;
        return 1;
    case BSP_UART1:
        *clock = RCU_USART1;
        return 1;
    case BSP_UART2:
        *clock = RCU_USART2;
        return 1;
    case BSP_UART3:
        *clock = RCU_UART3;
        return 1;
    case BSP_UART4:
        *clock = RCU_UART4;
        return 1;
    }

    return 0;
}

/***************************************************************************************
 * @Function    : bsp_uart_pin_clock_config()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description : 串口对应引脚时钟使能,当使用重映射功能时，必须使能复用时钟
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/4/13
 ****************************************************************************************/
static void bsp_uart_pin_clock_config(bsp_uart_pin_stu *pin, uint32_t remap)
{
    rcu_periph_enum rcu;

    /* 开启重映射功能时，需要使能复用时钟 */
    if (remap != BSP_UART_REMAP_NONE)
    {
        rcu_periph_clock_enable(RCU_AF);
    }

    rcu = bsp_get_pin_rcu(pin->tx_gpio_periph);
    if (rcu != 0)
    {
        rcu_periph_clock_enable(rcu);
    }

    if (pin->tx_gpio_periph == pin->rx_gpio_periph)
    {
        return;
    }

    rcu = bsp_get_pin_rcu(pin->rx_gpio_periph);
    if (rcu != 0)
    {
        rcu_periph_clock_enable(rcu);
    }
}

/***************************************************************************************
 * @Function    : bsp_uart_periph_clock_enable()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/4/13
 ****************************************************************************************/
static uint8_t bsp_uart_periph_clock_enable(BSP_UART_TYPE uart_x)
{
    bsp_uart_pin_stu pin_info;
    rcu_periph_enum uart_clock;

    /* Enable the pin clock corresponding to the serial port */
    if (0 == bsp_uart_get_pin(uart_x, uart_param[uart_x].remap, &pin_info))
    {
        return 0;
    }
    bsp_uart_pin_clock_config(&pin_info, uart_param[uart_x].remap);

    /* Enable the DMA clock corresponding to the serial port */
    if (uart_info[uart_x].use_dma_flag > 0)
    {
        if (uart_info[uart_x].dma_periph == DMA0)
        {
            rcu_periph_clock_enable(RCU_DMA0);
        }
        else if (uart_info[uart_x].dma_periph == DMA1)
        {
            rcu_periph_clock_enable(RCU_DMA1);
        }
    }

    /* Serial port enable */
    if (0 == bsp_uart_get_clock(uart_x, &uart_clock))
    {
        return 0;
    }
    rcu_periph_clock_enable(uart_clock);

    return 1;
}

/***************************************************************************************
 * @Function    : bsp_uart_pin_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description : 串口引脚初始化，包括引脚的重映射
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/4/13
 ****************************************************************************************/
static uint8_t bsp_uart_pin_init(BSP_UART_TYPE uart_x)
{
    uint32_t remap;
    bsp_uart_pin_stu pin;

    remap = uart_param[uart_x].remap;
    if (bsp_uart_get_pin(uart_x, remap, &pin) == 0)
        return 0;

    /* tx pin config */
    gpio_init(pin.tx_gpio_periph, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, pin.tx_gpio_pin);

    /* rx pin config */
    gpio_init(pin.rx_gpio_periph, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, pin.rx_gpio_pin);

    if (remap != BSP_UART_REMAP_NONE)
    {
        gpio_pin_remap_config(remap, ENABLE);
    }

    return 1;
}

/***************************************************************************************
 * @Function    : bsp_uart_dma_config()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/1/18
 ****************************************************************************************/
static uint8_t bsp_uart_dma_config(BSP_UART_TYPE uart_x)
{
    bsp_uart_dma_stu dma_info;
    dma_parameter_struct init;

    if (bsp_uart_get_dma_info(uart_x, &dma_info) == 0)
        return 0;

    /* tx dma config */
    dma_deinit(dma_info.periph, dma_info.tx_chann);
    init.direction = DMA_MEMORY_TO_PERIPHERAL;
    init.memory_addr = (uint32_t)RT_NULL;
    init.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    init.memory_width = DMA_MEMORY_WIDTH_8BIT;
    init.number = (uint32_t)0;
    init.periph_addr = (uint32_t)(&USART_DATA(uart_param[uart_x].periph));
    init.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    init.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    init.priority = DMA_PRIORITY_LOW;
    dma_init(dma_info.periph, dma_info.tx_chann, &init);
    dma_circulation_disable(dma_info.periph, dma_info.tx_chann);
    dma_memory_to_memory_disable(dma_info.periph, dma_info.tx_chann);

    /* rx dma config */
    dma_deinit(dma_info.periph, dma_info.rx_chann);
    init.direction = DMA_PERIPHERAL_TO_MEMORY;
    init.memory_addr = (uint32_t)uart_info[uart_x].rx_buff;
    init.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    init.memory_width = DMA_MEMORY_WIDTH_8BIT;
    init.number = uart_info[uart_x].rx_buff_size;
    init.periph_addr = (uint32_t)(&USART_DATA(uart_param[uart_x].periph));
    init.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    init.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    init.priority = DMA_PRIORITY_LOW;
    dma_init(dma_info.periph, dma_info.rx_chann, &init);
    dma_circulation_disable(dma_info.periph, dma_info.rx_chann);
    dma_memory_to_memory_disable(dma_info.periph, dma_info.rx_chann);

    dma_channel_disable(dma_info.periph, dma_info.tx_chann);
    dma_channel_enable(dma_info.periph, dma_info.rx_chann);

    return 1;
}

/***************************************************************************************
 * @Function    : bsp_uart_param_config()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/1/18
 ****************************************************************************************/
static void bsp_uart_param_config(BSP_UART_TYPE uart_x, bsp_uart_config_param_stu *config)
{
    uint32_t uart_periph;

    uart_periph = uart_param[uart_x].periph;
    usart_deinit(uart_periph);
    usart_baudrate_set(uart_periph, config->baut);
    usart_word_length_set(uart_periph, config->wd_len);
    usart_stop_bit_set(uart_periph, config->stop_bits);
    usart_parity_config(uart_periph, config->parity);
    usart_hardware_flow_rts_config(uart_periph, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(uart_periph, USART_CTS_DISABLE);
    usart_receive_config(uart_periph, USART_RECEIVE_ENABLE);
    usart_transmit_config(uart_periph, USART_TRANSMIT_ENABLE);
    usart_enable(uart_periph);

    usart_dma_transmit_config(uart_periph, USART_DENT_ENABLE);
    usart_dma_receive_config(uart_periph, USART_DENR_ENABLE);
}

/***************************************************************************************
 * @Function    : bsp_uart_nvic_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/1/28
 ****************************************************************************************/
static void bsp_uart_nvic_init(BSP_UART_TYPE uart_x)
{
    uint8_t irq;
    uint32_t uart_periph;

    if (bsp_uart_get_irq(uart_x, &irq) == 0)
        return;

    /* enable uart interrupt */
    nvic_irq_enable(irq, 1, 0);

    uart_periph = uart_param[uart_x].periph;

    /* enable uart idle interrupt */
    usart_interrupt_enable(uart_periph, USART_INT_IDLE);

    /* if uart do not use dma transmit:enalbe uart rx interrupt */
    if (uart_info[uart_x].use_dma_flag == 0)
    {
        usart_interrupt_enable(uart_periph, USART_INT_RBNE);
        //usart_interrupt_enable(uart_periph, USART_INT_TBE);
//		usart_interrupt_enable(uart_periph, USART_INT_TC);
    }

    /* enable uart send complete interrupt */
    usart_interrupt_enable(uart_periph, USART_INT_TC);
}

/***************************************************************************************
 * @Function    : bsp_uart_irq_idle_dma_handle()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/4/13
 ****************************************************************************************/
static void bsp_uart_irq_idle_dma_handle(BSP_UART_TYPE uart_x)
{
    uint16_t dma_rx_len;

    dma_channel_disable(uart_info[uart_x].dma_periph, uart_info[uart_x].dma_rx_chann);

    dma_rx_len = uart_info[uart_x].rx_buff_size - dma_transfer_number_get(uart_info[uart_x].dma_periph, uart_info[uart_x].dma_rx_chann);

    if (uart_info[uart_x].rx_hook != RT_NULL && dma_rx_len > 1)
    {
        uart_info[uart_x].rx_hook(dma_rx_len);
    }

    dma_memory_address_config(uart_info[uart_x].dma_periph, uart_info[uart_x].dma_rx_chann, (uint32_t)uart_info[uart_x].rx_buff);
    dma_transfer_number_config(uart_info[uart_x].dma_periph, uart_info[uart_x].dma_rx_chann, uart_info[uart_x].rx_buff_size);
    dma_channel_enable(uart_info[uart_x].dma_periph, uart_info[uart_x].dma_rx_chann);
}

/***************************************************************************************
 * @Function    : bsp_uart_irq_idle_handle()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/4/13
 ****************************************************************************************/
static void bsp_uart_irq_idle_handle(BSP_UART_TYPE uart_x)
{
    if (uart_info[uart_x].rx_hook != RT_NULL)
    {
        uart_info[uart_x].rx_hook(uart_info[uart_x].irq_rx_pos);
    }
    uart_info[uart_x].irq_rx_pos = 0;
}

/***************************************************************************************
 * @Function    : bsp_uart_irq_rx_handle()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description : Serial port receives data interrupt processing function
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/4/14
 ****************************************************************************************/
static void bsp_uart_irq_rx_handle(BSP_UART_TYPE uart_x)
{
    if (uart_info[uart_x].rx_buff == RT_NULL || uart_info[uart_x].use_dma_flag > 0)
        return;

    if (uart_info[uart_x].irq_rx_pos < uart_info[uart_x].rx_buff_size)
    {
        uart_info[uart_x].rx_buff[uart_info[uart_x].irq_rx_pos++] = usart_data_receive(uart_param[uart_x].periph);
    }
}

/***************************************************************************************
 * @Function    : bsp_uart_irq_tc_handle()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description : Serial port transmission completion interrupt processing
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/4/14
 ****************************************************************************************/
static void bsp_uart_irq_tc_handle(BSP_UART_TYPE uart_x)
{
    if (uart_info[uart_x].tx_fnsh_hook != RT_NULL)
    {
        uart_info[uart_x].tx_fnsh_hook();
    }
}

/***************************************************************************************
 * @Function    : bsp_uart_irq_tx_idle_handle()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description : Serial port sending register empty interrupt handling function
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/4/14
 ****************************************************************************************/
static void bsp_uart_irq_tx_idle_handle(BSP_UART_TYPE uart_x)
{
    if (uart_info[uart_x].use_dma_flag > 0)
        return;

    if (uart_info[uart_x].tx_len == 0 || uart_info[uart_x].tx_len <= uart_info[uart_x].tx_pos)
    {
        return;
    }

    usart_data_transmit(uart_param[uart_x].periph, uart_info[uart_x].tx_buff[uart_info[uart_x].tx_pos++]);
}

/***************************************************************************************
 * @Function    : bsp_uart_irq_handle()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description : All interrupt handling functions on the serial port
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/1/28
 ****************************************************************************************/
static void bsp_uart_irq_handle(BSP_UART_TYPE uart_x)
{
    /* receive data */
    usart_data_receive(uart_param[uart_x].periph);

    if (RESET != usart_interrupt_flag_get(uart_param[uart_x].periph, USART_INT_FLAG_IDLE))
    {
        uart_oper[uart_x].rx_tmr++;

        if (uart_info[uart_x].use_dma_flag > 0)
        {
            bsp_uart_irq_idle_dma_handle(uart_x);
        }
        else
        {
            bsp_uart_irq_idle_handle(uart_x);
        }
        usart_interrupt_flag_clear(uart_param[uart_x].periph, USART_INT_FLAG_IDLE);
    }
    else if (RESET != usart_interrupt_flag_get(uart_param[uart_x].periph, USART_INT_FLAG_RBNE))
    {
        bsp_uart_irq_rx_handle(uart_x);
        usart_interrupt_flag_clear(uart_param[uart_x].periph, USART_INT_FLAG_RBNE);
    }
    else if (RESET != usart_interrupt_flag_get(uart_param[uart_x].periph, USART_INT_FLAG_TC))
    {
        bsp_uart_irq_tc_handle(uart_x);
        usart_interrupt_flag_clear(uart_param[uart_x].periph, USART_INT_FLAG_TC);
    }
    else if (RESET != usart_interrupt_flag_get(uart_param[uart_x].periph, USART_INT_FLAG_TBE))
    {
        bsp_uart_irq_tx_idle_handle(uart_x);
        usart_interrupt_flag_clear(uart_param[uart_x].periph, USART_INT_FLAG_TBE);
    }
    else if (RESET != usart_interrupt_flag_get(uart_param[uart_x].periph, USART_INT_FLAG_RT))
    {
        usart_interrupt_flag_clear(uart_param[uart_x].periph, USART_INT_FLAG_RT);
    }
}

/***************************************************************************************
 * @Function    : bsp_uart_set_global_param()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/4/14
 ****************************************************************************************/
static void bsp_uart_set_global_param(BSP_UART_TYPE uart_x, uint8_t dma_flag)
{
    bsp_uart_dma_stu dma;

    uart_info[uart_x].irq_rx_pos = 0;
    uart_info[uart_x].tx_buff = RT_NULL;
    uart_info[uart_x].tx_len = 0;
    uart_info[uart_x].tx_pos = 0;

    if (dma_flag == 0 || bsp_uart_get_dma_info(uart_x, &dma) == 0)
    {
        uart_info[uart_x].use_dma_flag = 0;
        return;
    }

    uart_info[uart_x].dma_periph = dma.periph;
    uart_info[uart_x].dma_tx_chann = dma.tx_chann;
    uart_info[uart_x].dma_rx_chann = dma.rx_chann;
    uart_info[uart_x].use_dma_flag = 1;
}

/***************************************************************************************
 * @Function    : bsp_uart_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/1/28
 ****************************************************************************************/
static uint8_t bsp_uart_init(BSP_UART_TYPE uart_x, bsp_uart_config_param_stu *param)
{
    if (uart_x >= BSP_UART_NUM || param == RT_NULL)
        return 0;

    /* uart set global parameter */
    bsp_uart_set_global_param(uart_x, param->dma_flag);

    /* clock enable */
    bsp_uart_periph_clock_enable(uart_x);

    /* Serial port pin initialization */
    bsp_uart_pin_init(uart_x);

    /* Serial DMA configuration */
    bsp_uart_dma_config(uart_x);

    /* Serial parameter configuration */
    bsp_uart_param_config(uart_x, param);

    bsp_uart_nvic_init(uart_x);

    uart_oper[uart_x].enable = 1;

    return 1;
}

/***************************************************************************
 * FuncName     : is_uart_tx_fnsh()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/4/27
 ***************************************************************************/
static uint8_t is_uart_tx_fnsh(BSP_UART_TYPE uart_x)
{

    if (uart_x >= BSP_UART_NUM)
        return 0;

    /* uart have no use dma*/
    if (uart_info[uart_x].use_dma_flag == 0)
    {
        if (uart_info[uart_x].tx_pos == 0)
            return 1;

        return 0;
    }

    return (dma_transfer_number_get(uart_info[uart_x].dma_periph, uart_info[uart_x].dma_tx_chann) > 0 ? 0 : 1);
}

/***************************************************************************************
 * @Function    : bsp_uart_dma_tx_msg()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/1/29
 ****************************************************************************************/
static uint8_t bsp_uart_dma_tx_msg(BSP_UART_TYPE uart_x)
{
    /* disable tx dma */
    dma_channel_disable(uart_info[uart_x].dma_periph, uart_info[uart_x].dma_tx_chann);

    /* set tx dma memory address */
    dma_memory_address_config(uart_info[uart_x].dma_periph, uart_info[uart_x].dma_tx_chann, (uint32_t)uart_info[uart_x].tx_buff);
    dma_transfer_number_config(uart_info[uart_x].dma_periph, uart_info[uart_x].dma_tx_chann, uart_info[uart_x].tx_len);

    dma_channel_enable(uart_info[uart_x].dma_periph, uart_info[uart_x].dma_tx_chann);
    return 1;
}

/***************************************************************************************
 * @Function    : bsp_uart_it_tx_msg()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/4/14
 ****************************************************************************************/
static uint8_t bsp_uart_it_tx_msg(BSP_UART_TYPE uart_x)
{
    uart_info[uart_x].tx_pos = 0;
    usart_data_transmit(uart_param[uart_x].periph, uart_info[uart_x].tx_buff[uart_info[uart_x].tx_pos++]);

    return 1;
}

/***************************************************************************************
 * @Function    : uart_tx_msg()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/1/28
 ****************************************************************************************/
static uint8_t bsp_uart_tx_msg(BSP_UART_TYPE uart_x, uint8_t *tx_buff, uint16_t tx_len)
{
    if (uart_x >= BSP_UART_NUM || tx_buff == RT_NULL)
        return 0;

    if (is_uart_tx_fnsh(uart_x) == 0)
    {
        return 0;
    }

    uart_oper[uart_x].tx_tmr++;
    uart_info[uart_x].tx_buff = tx_buff;
    uart_info[uart_x].tx_len = tx_len;

    if (uart_info[uart_x].use_dma_flag > 0)
    {
        return bsp_uart_dma_tx_msg(uart_x);
    }

    return bsp_uart_it_tx_msg(uart_x);
}

/***************************************************************************
 * FuncName     : USART0_IRQHandler()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/4/2
 ***************************************************************************/
void USART0_IRQHandler(void)
{
    rt_interrupt_enter();

    bsp_uart_irq_handle(BSP_UART0);

    rt_interrupt_leave();
}

/***************************************************************************
 * FuncName     : USART1_IRQHandler()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/4/2
 ***************************************************************************/
void USART1_IRQHandler(void)
{
    rt_interrupt_enter();

    bsp_uart_irq_handle(BSP_UART1);

    rt_interrupt_leave();
}

/***************************************************************************
 * FuncName     : USART2_IRQHandler()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/4/2
 ***************************************************************************/
void USART2_IRQHandler(void)
{
    rt_interrupt_enter();

    bsp_uart_irq_handle(BSP_UART2);

    rt_interrupt_leave();
}

/***************************************************************************
 * FuncName     : USART3_IRQHandler()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/4/2
 ***************************************************************************/
void UART3_IRQHandler(void)
{
    rt_interrupt_enter();

    bsp_uart_irq_handle(BSP_UART3);

    rt_interrupt_leave();
}

/***************************************************************************************
 * @Function    : UART4_IRQHandler()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/4/14
 ****************************************************************************************/
void UART4_IRQHandler(void)
{
    rt_interrupt_enter();

    bsp_uart_irq_handle(BSP_UART4);

    rt_interrupt_leave();
}

/***************************************************************************************
 * @Function    : bsp_uart_rx_func_regist()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/4/14
 ****************************************************************************************/
static uint8_t bsp_uart_rx_func_regist(BSP_UART_TYPE uart_x, func_rx_msg_hook rx_func, uint8_t *rx_buff, uint16_t buff_size)
{
    if (uart_x >= BSP_UART_NUM)
        return 0;

    uart_info[uart_x].rx_hook = rx_func;

    uart_info[uart_x].rx_buff = rx_buff;
    uart_info[uart_x].rx_buff_size = buff_size;

    /* set tx dma memory address */
    dma_channel_disable(uart_info[uart_x].dma_periph, uart_info[uart_x].dma_rx_chann);
    dma_memory_address_config(uart_info[uart_x].dma_periph, uart_info[uart_x].dma_rx_chann, (uint32_t)uart_info[uart_x].rx_buff);
    dma_transfer_number_config(uart_info[uart_x].dma_periph, uart_info[uart_x].dma_rx_chann, uart_info[uart_x].tx_len);
    return 1;
}

/***************************************************************************************
 * @Function    : bsp_uart_tx_fnsh_func_regist()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/4/14
 ****************************************************************************************/
static void bsp_uart_tx_fnsh_func_regist(BSP_UART_TYPE uart_x, func_tx_fnsh_hook tx_fnsh_func)
{
    uart_info[uart_x].tx_fnsh_hook = tx_fnsh_func;
}

/***************************************************************************************
 * @Function    : bsp_uart_oper_dev_bind()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/21
 ****************************************************************************************/
static void bsp_uart_oper_dev_bind(void)
{
    for (uint8_t i = 0; i < BSP_UART_NUM; i++)
    {
        bsp_bind_oper_data_dev(0xff, 204, i, &uart_oper[i].enable, 1);
        bsp_bind_oper_data_dev(0xff, 204, 5 + i, &uart_oper[i].rx_tmr, 1);
        bsp_bind_oper_data_dev(0xff, 204, 10 + i, &uart_oper[i].tx_tmr, 1);
    }
}

/***************************************************************************************
 * @Function    : bsp_uart_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/4/27
 ****************************************************************************************/
int bsp_uart_dev_create(void)
{
    static bsp_uart_dev_stu uart_dev;

    uart_dev.bsp_uart_init = bsp_uart_init;

    uart_dev.bsp_uart_rx_func_regist = bsp_uart_rx_func_regist;

    uart_dev.bsp_uart_tx_msg = bsp_uart_tx_msg;

    uart_dev.bsp_uart_tx_fnsh_func_regist = bsp_uart_tx_fnsh_func_regist;

    bsp_dev_append(BSP_DEV_NAME_UART, &uart_dev);

    bsp_uart_oper_dev_bind();

    return 0;
}

