#ifndef _CMP_STORE_H_
#define _CMP_STORE_H_

#include "cmp.h"
#include "cmp_store_order.h"
#include "cmp_store_param.h"
#include "cmp_store_whitelist.h"
#include "cmp_store_tim.h"
#include "cmp_store_ota.h"
#include "cmp_store_model.h"
#include "drv_clock.h"
#include "drv_serial.h"

#define CMP_STORE_REDIS_ON 0xAA
#define CMP_STORE_REDIS_OFF 0x55

typedef enum
{
    EXIT_FLASH = 0,
    INN_FLASH,
} CMP_FLASH_TYPE;

typedef struct
{
    drv_flash_dev_stu *ext_flash;
    drv_flash_dev_stu *inn_flash;
    drv_flash_dev_stu *curr_flash;
    drv_clock_dev_stu *clock;

} cmp_store_stu;

typedef struct
{
    cmp_store_param_dev_stu *param;

    cmp_store_whitelist_dev_stu *whitelist;

    cmp_store_tim_dev_stu *tim;

    cmp_store_model_dev_stu *model;

    cmp_store_order_dev_stu *order;

    cmp_store_ota_dev_stu *ota;

    uint8_t (*cmp_restore_store)(uint32_t addr, uint8_t page_cnt);

    drv_flash_dev_stu *(*cmp_get_flash)(CMP_FLASH_TYPE type);

} cmp_store_dev_stu;

int cmp_store_dev_create(void);

#endif