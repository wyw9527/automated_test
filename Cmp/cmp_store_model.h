#ifndef _CMP_STROE_MODEL_H_
#define _CMP_STROE_MODEL_H_

#include "cmp.h"

#define CMP_STORE_MODEL_CONTENT_CNT 0x0A
#define CMP_STORE_FLAG 0xA5A5A5A5

typedef struct
{
    uint8_t start_time[2];
    uint8_t elec_fee[4]; /*单位 元 分辨率 0.0001*/
    uint8_t serv_fee[4];
} cmp_store_model_info_content_stu;

typedef struct
{
    uint8_t model_no[2];
    uint8_t model_cnt;
    cmp_store_model_info_content_stu content[CMP_STORE_MODEL_CONTENT_CNT];
} cmp_store_model_info_stu;

typedef struct
{
    uint32_t store_flag;
    cmp_store_model_info_stu info;
} cmp_store_model_stu;

typedef struct
{
    uint32_t model_addr;
} cmp_store_model_param_stu;

typedef struct
{
    drv_flash_dev_stu *ext_flash;

    cmp_store_model_param_stu param;
} cmp_store_model_s_stu;

typedef struct
{
    uint8_t (*cmp_store_model_set)(cmp_store_model_info_stu *model);

    const cmp_store_model_info_stu *(*cmp_store_model_query)(void);

    void (*cmp_set_ext_flash)(drv_flash_dev_stu *ext_flash, uint8_t type);

} cmp_store_model_dev_stu;

int cmp_store_model_subdev_create(void);

#endif