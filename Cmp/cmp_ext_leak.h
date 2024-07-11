#ifndef _CMP_EXT_LEAK_H_
#define _CMP_EXT_LEAK_H_

#include "cmp.h"
#include "cmp_measure.h"

#define CMP_EXT_LEAK_ADJ_DIFF 3
#define EXT_LEAK_SOLAR_START_CURR 10
#define EXT_LEAK_SOLAR_UP_CURR 5
#define EXT_LEAK_SOLAR_DWON_CURR 10
#define EXT_LEAK_SOLAR_OFFSET 10

typedef enum
{
    EXT_LEAK_NONE,
    EXT_LEAK_SOLAR,
    EXT_LEAK_STATION,
} CMP_EXT_LEAK_TYPE;

typedef void (*ext_leak_hook_func)(uint8_t plug_idx, uint8_t type, uint16_t set_curr);

typedef struct
{
    const uint8_t *load_bal_flag;
    const uint8_t *load_bal_th;
    const uint8_t *solar_bal_mode;
    const uint8_t *station_bal;
    const uint8_t *max_curr;
    uint8_t dev_sta;
    ext_leak_hook_func hook_func;
} cmp_ext_leak_param_stu;

typedef enum
{
    EXT_LEAK_ADJ_STD = 0,
    EXT_LEAK_ADJ_UP,
    EXT_LEAK_ADJ_DOWM,
    EXT_LEAK_ADJ_NUM
} CMP_EXT_LEAK_ADJ_TYPE;

typedef struct
{
    int16_t chrg_curr;             /* 当前充电电流*/
    int16_t set_curr;              /* 下发的设置值 */
    uint8_t plug_idx;              /* 充电枪编号 */
    CMP_EXT_LEAK_ADJ_TYPE adj_sta; /* 调节状态 */
} cmp_ext_leak_plug_stu;

typedef struct
{
    uint8_t chrg_dev_cnt;
    cmp_ext_leak_plug_stu plug_dev[BSP_POLE_PLUG_NUM];
} cmp_ext_leak_plug_dev_stu;

typedef struct
{
    int16_t station_set_curr; /* 站级负载下发电流*/
    int16_t load_bal_margin;
    int16_t load_bal_margin_pre;
    int16_t meter_curr;      /* 电表电流0.1A*/
    int16_t meter_have_curr; // 0.1A
    int16_t chrg_curr;
    int16_t chrg_curr_pre;
    int16_t set_curr;
    uint16_t load_bal_count;
    uint16_t set_adj_stdy_count;
    uint32_t solar_run_count;
    uint32_t solar_set_count;
    uint32_t solar_green_puse_count;
    uint32_t load_bal_adj_flag : 1;
    uint32_t solar_start_complet_flag : 1;
    uint32_t solar_adj_flag : 1;
    uint32_t resume_adj_flag : 1;
    uint32_t resume_flag : 1;
} cmp_ext_procc_stu;

typedef struct
{
    drv_serial_dev_stu *serial;

    drv_meter_dev_stu *meter;

    cmp_ext_leak_param_stu param;

    cmp_ext_procc_stu procc;
} cmp_ext_leak_stu;

typedef struct
{
    void (*drv_set_plug_sta)(uint8_t plug_idx, uint8_t plug_sta);

    void (*drv_set_curr_hook_func)(ext_leak_hook_func hook);

    void (*drv_ext_leak_work)(uint8_t period, uint8_t chrg_sta);

    void (*drv_ext_set_param)(const uint8_t *load_bal_flag, const uint8_t *load_bal_th, const uint8_t *solar_bal_mode, const uint8_t *station_bal, const uint8_t *max_curr);

    void (*drv_ext_set_resume_adj_flag)(uint8_t flag);

    void (*drv_ext_set_station_set_curr)(uint16_t curr);

    uint8_t (*drv_ext_get_leak_resume_flag)(void);
} cmp_ext_leak_dev_stu;

#endif