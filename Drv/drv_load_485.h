#ifndef _DRV_LOAD_485_H_
#define _DRV_LOAD_485_H_

#include "stdio.h"
#include "drv.h"
#include "drv_serial.h"



 typedef struct
 {
     char dataType;
     float dataValue;
 }LoadValueType;

typedef struct
{
	uint8_t FanSta;
	uint8_t LoadSwt;
    uint8_t data[3];
}LoadInputDataType;

typedef struct
{
    bsp_uart_dev_stu *uart;   /* can设备 */

    drv_serial_dev_stu* load_485_serial;

} drv_load_485_pub_stu;

// typedef void (*func_uart_handler_hook)(drv_serial_dev_stu *serial);

typedef struct
{
    void (*drv_load_485_work)(uint8_t period);

    void (*drv_load_485_set_current)(uint8_t curr_a, uint8_t curr_b, uint8_t curr_c);

    void (*drv_load_485_turn_off_fan)();

    void (*drv_load_485_set_uart)(drv_serial_dev_stu *serial);


} drv_load_485_dev_stu;

#endif