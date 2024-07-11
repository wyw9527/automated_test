#ifndef _CMP_LOAD_H_
#define _CMP_LOAD_H_

#include "cmp.h"
#include "drv_load_can.h"
#include "drv_load_485.h"
#include "drv_power_box.h"


typedef struct
{
    uint8_t count;
    uint8_t fan_flag;

}cmp_load_para_stu;


typedef struct
{
    drv_load_can_dev_stu* load_can;

    drv_load_485_dev_stu* load_485;

    drv_power_box_dev_stu* power_box;

    drv_serial_dev_stu* load_serial;

    cmp_load_para_stu* procc;
    
} cmp_load_stu;

typedef struct 
{
    void (*cmp_load_work)(uint8_t period);
    void (*cmp_load_set_current)(uint16_t curr_a, uint16_t curr_b, uint16_t curr_c);
    void (*cmp_load_set_all_switch_on)();
    void (*cmp_load_set_all_switch_off)();

} cmp_load_dev_stu;



#endif