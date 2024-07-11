#ifndef _DRV_METER_H_
#define _DRV_METER_H_

#include "bsp.h"
#include "drv.h"

#define DRV_METER_CYC_WORK_PEIOD 1000
#define DRV_METER_RX_DATA_MAX_LEN 64

typedef enum
{
    METER_TYPE_SINGLE,
    METER_TYPE_THREE,
} DRV_METER_TYPE;

typedef enum
{
    METER_NONE,
    METER_CHNT,
    METER_EASTRON
} DRV_METER_BAND;

typedef enum
{
    METER_SN,
    METER_INPUT,
    METER_ETRON_POWER,
} DRV_METER_CMD_STA;

typedef enum
{
    METER_VOLUME_VOLT_A,
    METER_VOLUME_VOLT_B,
    METER_VOLUME_VOLT_C,
    METER_VOLUME_CURR_A,
    METER_VOLUME_CURR_B,
    METER_VOLUME_CURR_C,
    METER_VOLUME_CURR_MAX,
    METER_VOLUME_CURR_HAVE_MAX,
    METER_VOLUME_POWER_TOTAL,
    METER_VOLUME_ENERGY,
} DRV_METER_VOLUME_TYPE;

typedef struct
{
    uint8_t addr;
    uint8_t code;
    uint8_t start_reg[2];
    uint8_t reg_cnt[2];
    uint8_t crc[2];
} drv_meter_modbus_tx_stu;

typedef struct
{
    uint8_t addr;
    uint8_t func_code;
    uint8_t len;
    uint8_t data[DRV_METER_RX_DATA_MAX_LEN];
} drv_meter_msg_rx_stu;

typedef struct
{
    uint8_t tx_len;
    uint8_t tx_buff[32];
} drv_meter_single_tx_stu;

typedef struct
{
    float volt_a; // V
    float curr_a; // A
    float curr_have_a;
    float curr_max;
    float curr_have_max;
    float power_a; // KW
} drv_meter_single_volume_stu;

typedef struct
{
    float volt_a; // V
    float volt_b;
    float volt_c;
    float curr_a; // A
    float curr_b;
    float curr_c;
    float curr_have_a; // A
    float curr_have_b;
    float curr_have_c;
    float curr_max;
    float curr_have_max;
    float power_a; // KW
    float power_b;
    float power_c;
    float power_total;
} drv_meter_three_volume_stu;

typedef struct
{
    /* data */
    uint16_t volt_a;
    uint16_t volt_b;
    uint16_t volt_c;

    uint32_t curr_a;
    uint32_t curr_b;
    uint32_t curr_c;

    uint32_t energy;
    uint32_t power;
} drv_meter_volume_stu;

typedef struct
{
    DRV_METER_BAND band;
    uint8_t type;  // 0 single  1 three
    uint8_t state; // 0 normal 1 abn
    serial_tx_func_type tx_func;
    drv_meter_single_tx_stu tx_data;
} drv_meter_param_stu;

typedef struct
{
    drv_meter_single_volume_stu single_vol;
    drv_meter_three_volume_stu three_vol;
    drv_meter_volume_stu volume;
    uint16_t tx_count;
    uint16_t tx_circle;
    uint8_t tx_cnt;
    DRV_METER_CMD_STA cmd_sta;

    uint8_t enable_flag; // 0xAA enable  other disable
    uint8_t succ_flag;
} drv_meter_procc_stu;

typedef struct
{
    drv_meter_param_stu param;

    drv_meter_procc_stu procc;
} drv_meter_stu;

typedef struct
{
    void (*drv_set_meter_type)(DRV_METER_TYPE type, serial_tx_func_type tx_func);

    uint8_t (*drv_set_meter_msg)(uint8_t *msg, uint8_t len);

    int32_t (*drv_get_meter_volume)(DRV_METER_VOLUME_TYPE type);

    uint8_t (*drv_get_meter_state)(void);

    void (*drv_meter_work)(uint8_t period);

    void (*drv_set_meter_enable)(uint8_t enable_flag);

    uint8_t (*drv_get_meter_succ_flag)(void);

} drv_meter_dev_stu;

#endif