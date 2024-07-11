#include "bsp_can.h"

static can_msg_buff_stu can_msg;
/***************************************************************************************
 * @Function    : bsp_can_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : SSY            Version   : V2.0.0                Date:2024/6/17
 ****************************************************************************************/
void bsp_can_init(void)
{
	can_parameter_struct	can_parameter;
    can_filter_parameter_struct can_filter;
	/* initialize CAN register */
    can_deinit(CAN0);
    /* enable CAN clock */
    rcu_periph_clock_enable(RCU_CAN0);
    rcu_periph_clock_enable(RCU_GPIOA);//使能时钟

    gpio_init(GPIOA, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ,GPIO_PIN_11);
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_12);//IO复用为CAN功能
  
    /* configure CAN0 NVIC */
    nvic_irq_enable(CAN0_RX1_IRQn,3,0); //中断配置

    can_struct_para_init(CAN_INIT_STRUCT, &can_parameter);
    can_struct_para_init(CAN_FILTER_STRUCT, &can_filter);//初始化参数
  /* baudrate 125Kbps */
  	can_parameter.resync_jump_width = CAN_BT_SJW_1TQ;
	can_parameter.time_segment_1 = CAN_BT_BS1_5TQ;
	can_parameter.time_segment_2 = CAN_BT_BS2_4TQ;
	can_parameter.prescaler = 48;
    /* initialize CAN */
    can_init(CAN0, &can_parameter);
	
	can_filter.filter_fifo_number = CAN_FIFO1;
    can_filter.filter_enable = ENABLE;
    can_filter_init(&can_filter);
	 /* enable can receive FIFO0 not empty interrupt */
    can_interrupt_enable(CAN0, CAN_INT_RFNE1|CAN_INT_TME);

}

/***************************************************************************************
 * @Function    : bsp_can_tx_msg()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : SSY            Version   : V2.0.0                Date:2024/6/17
 ****************************************************************************************/
uint8_t bsp_can_tx_msg(can_trasnmit_message_struct can_frame)
{  
	uint8_t ret=0;
	ret = can_message_transmit(CAN0,&can_frame);
	if(ret == CAN_NOMAILBOX)
	{
		return 1;
	}
  	return 0;
}




static void bsp_push_can_buff_msg(can_receive_message_struct* msg)
{
	memcpy(&can_msg.rev_buff[can_msg.head], msg, sizeof(can_receive_message_struct));
	can_msg.head = (can_msg.head + 1) %  CAN_BUFFER_SIZE;

}

static can_receive_message_struct* bsp_get_can_buff_msg()
{
	if(can_msg.head == can_msg.tail)
	{
		return NULL;
	}
		can_receive_message_struct* msg = &can_msg.rev_buff[can_msg.tail];
		can_msg.tail = (can_msg.tail + 1) %  CAN_BUFFER_SIZE;
		return msg;
}



/***************************************************************************************
 * @Function    : bsp_can_rx_msg()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : SSY            Version   : V2.0.0                Date:2024/6/17
 ****************************************************************************************/
void CAN0_RX1_IRQHandler(void)
{ 
	can_receive_message_struct can_msg;
	can_message_receive(CAN0, CAN_FIFO1, &can_msg); 
	bsp_push_can_buff_msg(&can_msg);
}
/***************************************************************************************
 * @Function    : bsp_can_error()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : SSY            Version   : V2.0.0                Date:2024/6/17
 ****************************************************************************************/
void bsp_can_error(void)
{
	if(	can_flag_get(CAN0, CAN_FLAG_MTE2) != RESET ||
		can_flag_get(CAN0, CAN_FLAG_MTE0) != RESET ||
		can_flag_get(CAN0, CAN_FLAG_MTE1) != RESET || 
		can_flag_get(CAN0, CAN_FLAG_PERR) != RESET ||
		can_flag_get(CAN0, CAN_FLAG_WERR) != RESET)
	{
		can_flag_clear(CAN0, CAN_FLAG_MTE0);
		can_flag_clear(CAN0, CAN_FLAG_MTE1);
		can_flag_clear(CAN0, CAN_FLAG_MTE2);
		can_flag_clear(CAN0, CAN_FLAG_PERR);
		can_flag_clear(CAN0, CAN_FLAG_WERR);
		can_wakeup(CAN0);
		bsp_can_init();
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
int bsp_can_dev_create(void)
{
    static bsp_can_dev_stu can_dev;

    can_dev.bsp_can_init = bsp_can_init;

    can_dev.bsp_can_get_msg = bsp_get_can_buff_msg;

    can_dev.bsp_can_tx_msg = bsp_can_tx_msg;


    bsp_dev_append(BSP_DEV_NAME_CAN, &can_dev);


    return 0;
}

