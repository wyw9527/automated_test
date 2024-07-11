#ifndef _DRV_DIDO_H_
#define _DRV_DIDO_H_

#include "drv.h"

typedef struct
{
    void (*drv_do_on)(void);

    void (*drv_do_off)(void);

} drv_dido_do_dev_stu;

typedef struct
{
    uint8_t (*drv_di_get_val)(void);

} drv_dido_di_dev_stu;

#endif