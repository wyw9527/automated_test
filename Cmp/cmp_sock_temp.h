#ifndef _CMP_SOCK_TEMP_H_
#define _CMP_SOCK_TEMP_H_

#include "cmp.h"


#define CMP_TEMP_ALARM_TH  	600
#define CMP_TEMP_FAULT_TH	650

typedef struct
{
    uint16_t temp_val;
    uint16_t abn_tmr;
    uint32_t abn_count;
    uint8_t sta;
    uint8_t sta_pre;
    uint8_t idx;
}cmp_sock_temp_procc_stu;

typedef struct
{
    drv_analog_dev_stu* temperature; /* temperature */
    
    cmp_sock_temp_procc_stu procc;
}cmp_sock_temp_stu;


typedef struct
{
    void (*cmp_dev_work)(uint8_t period);
    
}cmp_sock_temp_dev_stu;







#endif