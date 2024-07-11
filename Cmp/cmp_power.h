#ifndef _CMP_POWER_H_
#define _CMP_POWER_H_

#include "cmp.h"
#include "drv_power_box.h"





typedef struct
{
    drv_power_box_dev_stu* power_box;

    drv_serial_dev_stu* power_serial;

    // cmp_load_para_stu* procc;
    
} cmp_power_stu;

typedef struct 
{
    void (*cmp_power_work)(uint8_t period);
    void (*cmp_set_power_box_voltage)(uint32_t volt_a, uint32_t volt_b, uint32_t volt_c);
    void (*cmp_set_power_box_start)();
    void (*cmp_set_power_box_stop)();

} cmp_power_dev_stu;



#endif