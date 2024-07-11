#ifndef _CMP_BILL_H_
#define _CMP_BILL_H_

#include "cmp.h"

#include "cmp_store.h"
#include "cmp_measure.h"

#define CMP_BILL_CHAN_CNT 1

typedef struct
{
    uint8_t measure;
} cmp_bill_procc_dev_stu;

typedef struct
{
    cmp_bill_procc_dev_stu dev_sta;
    cmp_store_order_info_stu order;
    uint32_t curr_energy;
    uint8_t curr_period_idx;

    cmp_store_order_bak_stu order_bak;
} cmp_bill_procc_stu;

typedef struct
{
    drv_clock_dev_stu *clock;
    cmp_store_dev_stu *store;
} cmp_bill_pub_dev_stu;

typedef struct
{
    const cmp_measure_dev_stu *measure;
    const cmp_store_model_info_stu *model;
    cmp_bill_procc_stu procc;
} cmp_bill_stu;

typedef struct
{
    void (*start_chrg_by_card)(uint8_t card_type, uint32_t uid);
    void (*start_chrg_by_pole)(uint8_t chrg_type);
    void (*start_chrg_by_remote)(uint8_t *order_no);
    void (*start_chrg_by_tim)(uint8_t tim_type, uint8_t *tim_no);
    void (*chrg_data_cyc)(uint8_t period);
    void (*stop_chrg_procc)(uint16_t stop_reason);
    const cmp_store_order_info_stu *(*get_chrg_order_info)(void);
    uint16_t (*get_chrg_volt)(uint8_t phase);
    uint16_t (*get_chrg_curr)(uint8_t phase);
    uint16_t (*get_chrg_time)();
    uint32_t (*get_chrg_money)();
    uint32_t (*get_chrg_energy)();
    uint32_t (*get_chrg_power)();
    uint8_t (*get_start_type)();
    void (*save_order_bak_info)();
} cmp_bill_dev_stu;

int cmp_bill_dev_create(void);

#endif
