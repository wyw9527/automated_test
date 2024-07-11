#ifndef _BSP_CAN_H_
#define _BSP_CAN_H_
#define CAN_BUFFER_SIZE 32

#include "bsp.h"

typedef struct
{
    uint8_t head;
    uint8_t tail;
    can_receive_message_struct rev_buff[CAN_BUFFER_SIZE];
} can_msg_buff_stu;

typedef struct
{
    /* Can initialization function */
    void (*bsp_can_init)(void);
    
    /* Can receive handler function*/
    can_receive_message_struct* (*bsp_can_get_msg)();
    
    /* Can send function */
    uint8_t (*bsp_can_tx_msg)(can_trasnmit_message_struct tx_buff);

    
}bsp_can_dev_stu;



#endif