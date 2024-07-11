#ifndef _CMP_STORE_ORDER_H_
#define _CMP_STORE_ORDER_H_

#include "cmp.h"

#define CMP_ORDER_PERIOD_NUM 0x0A
// #define CMP_ORDER_ONE_SIZE	128
// #define CMP_ORDER_UNS_MAX_NUM	320
// #define CMP_ORDER_ABN_MAX_NUM	320
// #define CMP_ORDER_HIS_MAX_NUM	640

#define CMP_STORE_FLAG 0xA5A5A5A5
#define CMP_STORE_ORDER_ONE_SIZE 128 // Byte

typedef enum
{
    STORE_ORDER_UNS = 0,
    STORE_ORDER_ABN,
    STORE_ORDER_HIS,
} CMP_STORE_ORDER_TYPE;

typedef enum
{
    STORE_ORDER_SAVE_READ,
    STORE_ORDER_SAVE_WRITE
} CMP_STORE_ORDER_SAVE_TYPE;

typedef struct
{
    uint8_t model_period_idx;       /* 时段计费模型索引 */
    uint8_t model_period_energy[2]; /* 时段电度 */
} cmp_store_order_info_period_stu;

typedef struct
{
    uint8_t plug_idx;          /* 枪编号，从0开始 */
    uint8_t order_no[8];       /* 订单编号 */
    uint16_t stop_reason;      /* 停止原因 */
    uint8_t start_chrg_type;   /* 启动方式：0 充电卡 1 即插即充 2定时 3 平台 4 蓝牙 5 预约 6 ocpp*/
    uint8_t card_type;         /* 充电卡类型：0 离线卡 1 在线卡 */
    uint32_t card_uid;         /* 充电卡物理卡号 */
    uint8_t start_time[6];     /* 订单开始时间 */
    uint8_t stop_time[6];      /* 订单停止时间 */
    uint16_t chrg_time;        /* 充电时长，单位 分 */
    uint32_t start_energy;     /* 起始电度， 分辨率 0.01 */
    uint32_t stop_energy;      /* 停止电度，分辨率0.01 */
    uint32_t chrg_energy;      /* 充电电度，0.01 */
    uint8_t fee_type;          /* 计费类型：0 不计费 1 本地计费模型 2 协议计费*/
    uint32_t total_serv_money; /* 总服务费 0.001 */
    uint32_t total_elec_money; /* 总电费 0.001 */
    uint16_t model_no;         /* 计费模型费率号 */
    uint8_t model_period_cnt;  /* 计费模型时段数 */
    uint8_t chrg_period_cnt;   /* 充电时段数 */
    cmp_store_order_info_period_stu period[CMP_ORDER_PERIOD_NUM];
} cmp_store_order_info_stu;

typedef struct
{
    uint16_t wr_idx : 15;
    uint16_t wr_mirror : 1;
    uint16_t rd_idx : 15;
    uint16_t rd_mirror : 1;
} cmp_store_order_statics_unit_stu;

typedef struct
{
    uint32_t flag;
    cmp_store_order_statics_unit_stu uns;
    cmp_store_order_statics_unit_stu abn;
    cmp_store_order_statics_unit_stu his;
} cmp_store_order_statics_stu;

typedef struct
{
    uint32_t statis_addr;
    uint32_t uns_addr;
    uint32_t abn_addr;
    uint32_t his_addr;
    uint16_t uns_max_cnt;
    uint16_t abn_max_cnt;
    uint16_t his_max_cnt;
} cmp_store_order_param_stu;

typedef struct
{
    uint32_t bak_flag;
    drv_bcd_datetime_stu bak_clock;
} cmp_store_procc_info_stu;

typedef struct
{
    cmp_store_procc_info_stu bak_procc;
    cmp_store_order_info_stu *order;
} cmp_store_order_bak_stu;

typedef struct
{
    cmp_store_order_bak_stu order_bak;
} cmp_store_order_procc_stu;

typedef struct
{
    cmp_store_order_param_stu param;

    cmp_store_order_statics_stu statics;

    drv_flash_dev_stu *ext_flash;

    cmp_store_order_procc_stu procc;
} cmp_store_order_stu;

typedef struct
{
    void (*cmp_set_ext_flash)(drv_flash_dev_stu *ext_flash, uint8_t type);

    uint8_t (*cmp_unsend_add)(cmp_store_order_info_stu *order); // 未发送订单

    uint8_t (*cmp_abn_add)(cmp_store_order_info_stu *order); // 异常订单

    uint8_t (*cmp_his_add)(cmp_store_order_info_stu *order, uint8_t type); // 历史订单

    uint16_t (*cmp_get_staitcs)(CMP_STORE_ORDER_TYPE order_type);

    uint8_t (*cmp_get_order_info)(CMP_STORE_ORDER_TYPE order_type, uint16_t order_idx, cmp_store_order_info_stu *order);

    uint8_t (*cmp_get_order_bak_info)(drv_flash_dev_stu *inn_flash, drv_bcd_datetime_stu *bak_clock);

    uint8_t (*cmp_save_order_bak_info)(cmp_store_order_bak_stu *order_bak, drv_flash_dev_stu *inn_flash);

} cmp_store_order_dev_stu;

int cmp_stroe_order_subdev_create(void);

#endif