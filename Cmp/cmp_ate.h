#ifndef _CMP_ATE_H_
#define _CMP_ATE_H_

#include "cmp_store.h"

typedef void (*ate_tx_msg_func_type)(uint8_t *msg, uint16_t len);
typedef void (*ate_cmd_hook_func_type)(uint8_t cmd, uint32_t data);

typedef enum
{
    CMP_ATE_DEBUG,
    CMP_ATE_SERV,
    CMP_ATE_NUM
} CMP_ATE_TYPE;

typedef enum
{
    ATE_CMD_CHRG_START,
    ATE_CMD_CHRG_STOP,
    ATE_CMD_CARD_READ,
    ATE_CMD_CARD_CAL,
    ATE_CMD_REBOOT,
    ATE_CMD_PLUG_STA,
    ATE_CMD_STOP_REASON,
    ATE_CMD_AGEING_DATA,
} CMP_ATE_CMD_TYPE;

typedef struct
{
    /* data */
    uint8_t plug_idx;
    uint8_t chrg_start_type; // 0 卡启动 1 APP启动 2 即插即充
    uint16_t volt_a;
    uint16_t volt_b;
    uint16_t volt_c;
    uint16_t curr_a;
    uint16_t curr_b;
    uint16_t curr_c;
    uint16_t power;
    uint16_t chrg_time;
    uint8_t plug_temp;
    uint16_t chrg_energy;
    uint32_t chrg_amount;
    uint8_t chrg_order[8];
} cmp_ate_mt_tx_ageing_data_stu;

typedef struct
{
    uint8_t encry_type;

} cmp_ate_param_stu;

typedef struct
{
    cmp_store_ota_info_stu ota;
    uint16_t cur_pack_idx;
} cmp_ate_procc_stu;

typedef struct
{
    cmp_store_dev_stu *store;

    cmp_ate_param_stu param;

    cmp_ate_procc_stu procc;

    ate_tx_msg_func_type tx_msg_func;

    ate_cmd_hook_func_type cmd_hook_func;
} cmp_ate_stu;

typedef struct
{
    uint8_t (*set_rx_msg)(CMP_ATE_TYPE ate_type, uint8_t *msg, uint16_t len);

    void (*set_tx_hnd_func)(CMP_ATE_TYPE ate_type, ate_tx_msg_func_type tx_func);

    void (*set_cmd_hook_func)(CMP_ATE_TYPE ate_type, ate_cmd_hook_func_type hook);

    void (*set_cmd_result)(CMP_ATE_TYPE ate_type, CMP_ATE_CMD_TYPE cmd, uint8_t result, uint32_t data);

    void (*set_ageing_data)(CMP_ATE_TYPE ate_type, uint8_t *ageing_data, uint16_t len);

} cmp_ate_dev_stu;

#endif