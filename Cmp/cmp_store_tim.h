#ifndef _CMP_STORE_TIM_H_
#define _CMP_STORE_TIM_H_

#include "cmp.h"

#define CMP_STORE_TIM_PERIOD_CNT 0x08
#define CMP_STORE_TIM_PEAK_CNT 0x08
#define CMP_STORE_FLAG 0xA5A5A5A5

typedef struct
{
    uint8_t state; /* 0 无效  1 有效 */
    uint8_t type;  /*0 私桩定时充电  1 运营桩插枪预约，2运营桩预约 */
    uint8_t no[8];
    uint8_t plug_idx;
    uint8_t enabel;
    uint8_t plan;
    uint8_t start_hour;
    uint8_t start_min;
    uint8_t stop_hour;
    uint8_t stop_min;
} cmp_store_tim_info_ct_stu;

typedef struct
{
    cmp_store_tim_info_ct_stu single; /* 单次定时 */
    cmp_store_tim_info_ct_stu period[CMP_STORE_TIM_PERIOD_CNT];
    cmp_store_tim_info_ct_stu peak[CMP_STORE_TIM_PEAK_CNT];
    uint8_t period_cnt;
    uint8_t peak_cnt;
} cmp_store_tim_info_stu;

typedef struct
{
    uint32_t store_flag;
    cmp_store_tim_info_stu info;
} cmp_store_tim_stu;

typedef struct
{
    uint32_t tim_addr;
    uint8_t peak_en;
} cmp_store_tim_param_stu;

typedef struct
{
    cmp_store_tim_param_stu param;
    drv_flash_dev_stu *ext_flash;
    const drv_clock_dev_stu *clock;
} cmp_store_tims_stu;

typedef enum
{
    CMP_STORE_TIM_OPER_DISABLE = 0,
    CMP_STORE_TIM_OPER_ENABLE,
    CMP_STORE_TIM_OPER_DEL,
    CMP_STORE_TIM_OPER_NUM,
} CMP_STORE_TIM_OPER_TYPE;

typedef struct
{
    void (*cmp_set_ext_flash)(drv_flash_dev_stu *ext_flash, drv_clock_dev_stu *clock, uint8_t type);

    uint8_t (*cmp_store_tim_add)(cmp_store_tim_info_ct_stu *tim);

    uint8_t (*cmp_store_tim_oper)(CMP_STORE_TIM_OPER_TYPE oper, uint8_t *tim_no);

    const cmp_store_tim_info_stu *(*cmp_store_tim_query)(void);

    cmp_store_tim_info_ct_stu *(*cmp_store_tim_get_vival)(uint8_t *tim_type);

    void (*cmp_store_tim_set_peak)(uint8_t peak);
} cmp_store_tim_dev_stu;

int cmp_store_tim_subdev_create(void);

#endif