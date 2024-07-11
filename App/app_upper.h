#ifndef _APP_UPPER_H_
#define _APP_UPPER_H_

#include "app_system.h"


// typedef struct
// {

// } app_upper_param_stu;


// typedef struct
// {

// } app_upper_procc_stu;


typedef struct
{
    char rx_buff[1024];
    uint16_t rx_len;

}app_net_uart_comm_stu;

typedef struct
{
    // cmp_load_dev_stu *cmp_load;

    cmp_prot_upper_dev_stu *upper;

    cmp_w5500_dev_stu* w5500;

    app_net_uart_comm_stu w5500_comm;

    // cmp_ble_dev_stu *ble;


    // app_upper_procc_stu procc;

    // app_upper_param_stu param;

} app_upper_stu;


#endif