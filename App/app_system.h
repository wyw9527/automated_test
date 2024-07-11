#ifndef _APP_SYSTEM_H_
#define _APP_SYSTEM_H_

#include "app_config.h"

/* 0  normal 1 create dev fail 2 init fail 3 run fail */

typedef struct
{
    drv_clock_dev_stu *clock;
    drv_dido_do_dev_stu *run_led;
    drv_analog_interface_stu *analog_intf;
    // cmp_load_dev_stu *cmp_load;
    // cmp_power_dev_stu *cmp_power;

} app_system_stu;



#endif