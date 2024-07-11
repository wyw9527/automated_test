#ifndef _CMP_CC_H_
#define _CMP_CC_H_

#include "cmp.h"

#define CMP_CC_TH		2000

typedef struct
{
    uint8_t cc_val;
    uint8_t sta;
    uint8_t sta_pre;
    uint8_t cc_count;
    uint16_t act_count;
}cmp_cc_procc_stu;

typedef struct
{
    drv_analog_dev_stu* cc_dev; /* cc */
    
    cmp_cc_procc_stu procc;
}cmp_cc_stu;


typedef struct
{
    uint8_t (*cmp_get_sta)(void);
    
    void (*cmp_dev_work)(void);
}cmp_cc_dev_stu;

























#endif