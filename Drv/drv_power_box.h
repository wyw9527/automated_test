#ifndef _DRV_POWER_BOX_H_
#define _DRV_POWER_BOX_H_

#include "stdio.h"
#include "drv.h"
#include "drv_serial.h"




typedef struct
{
    bsp_uart_dev_stu *uart;   /* can设备 */

    drv_serial_dev_stu* power_box_serial;

} drv_power_box_pub_stu;

// typedef void (*func_uart_handler_hook)(drv_serial_dev_stu *serial);

typedef struct
{
    void (*drv_power_box_work)(uint8_t period);

    void (*drv_power_box_set_voltage)(uint32_t volt_a, uint32_t volt_b, uint32_t volt_c);
    
    void (*drv_power_box_start)();
    
    void (*drv_power_box_stop)();

    void (*drv_power_box_set_uart)(drv_serial_dev_stu *serial);


} drv_power_box_dev_stu;

#endif