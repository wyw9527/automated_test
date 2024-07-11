#ifndef _BSP_RTC_H_
#define _BSP_RTC_H_

#include "bsp.h"


typedef struct
{
   uint32_t (*get_time_seconds)(void);
   
   void (*set_time_seconds)(uint32_t seconds);
   
}bsp_rtc_dev_stu;









#endif