#ifndef _BSP_PMU_H_
#define _BSP_PMU_H_

#include "bsp.h"



typedef void (*bsp_powerdown_callback_func_type)(void);






typedef enum
{
    BSP_PD_OPER=0,
    BSP_PD_ORDER,
    BSP_PD_NUM
}BSP_PD_TYPE;





void bsp_powerdown_callback_hook(BSP_PD_TYPE pd_type,bsp_powerdown_callback_func_type func);




#endif