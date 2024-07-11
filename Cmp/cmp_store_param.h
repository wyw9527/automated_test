#ifndef _CMP_STORE_PARAM_H_
#define _CMP_STORE_PARAM_H_

#include "cmp.h"

#define CMP_STORE_PARAM_SYS_BUFF_SIZE 48
#define CMP_STORE_PARAM_SET_BUFF_SIZE 160

typedef enum
{
    // SYS 存储区
    STORE_PARAM_SN = 0,
    STORE_PARAM_SOCK_STYLE,

    // SET 存储区
    STORE_PARAM_WORK_MODE,
    STORE_PARAM_MAX_CURR,
    STORE_PARAM_WIFI_SSID,
    STORE_PARAM_WIFI_PWD,
    STORE_PARAM_SERV_ADDR,
    STORE_PARAM_SERV_PORT,
    STORE_PARAM_LEAK_TH,
    STORE_PARAM_PLB_EN,
    STORE_PARAM_PLB_TH,
    STORE_PARAM_SLOAR_MODE,
    STORE_PARAM_OPER_TYPE,
    STORE_PARAM_SIM_NO,
    STORE_PARAM_SLB_EN,
    STORE_PARAM_PEAK_EN,
    STORE_PARAM_LIGHT,
} CMP_STORE_PARAM_TYPE;

typedef struct
{
    uint8_t idx;
    uint8_t len;
    uint8_t chrg_enable;
    uint8_t parm_type;
    uint32_t min;
    uint32_t max;
    uint32_t def;
} cmp_store_param_val_stu;

typedef struct
{
    uint8_t idx;
    uint8_t len;
    uint8_t chrg_enable;
    uint8_t parm_type;
    char *def;
} cmp_store_param_str_stu;

typedef struct
{
    uint8_t param_obj;
    uint8_t type; /* 0 str  1 val */
} cmp_store_param_col_stu;

typedef struct
{
    uint8_t sys_redis_flag;
    uint8_t sys_buff[CMP_STORE_PARAM_SYS_BUFF_SIZE];
    uint8_t set_redis_flag;
    uint8_t set_buff[CMP_STORE_PARAM_SET_BUFF_SIZE];
} cmp_store_procc_redis_data_stu;

typedef struct
{
    uint8_t sys_buff[CMP_STORE_PARAM_SYS_BUFF_SIZE];
    uint8_t set_buff[CMP_STORE_PARAM_SET_BUFF_SIZE];
} cmp_store_procc_param_data_stu;

typedef struct
{
    cmp_store_procc_redis_data_stu redis;

    cmp_store_procc_param_data_stu param;
} cmp_store_param_procc_stu;

typedef struct
{
    uint32_t sys_addr;
    uint32_t set_addr;
} cmp_store_param_data_stu;

typedef struct
{
    drv_flash_dev_stu *ext_flash;

    cmp_store_param_data_stu data;

    cmp_store_param_procc_stu procc;
} cmp_store_param_stu;

typedef struct
{
    const uint8_t *(*store_parameter_query)(uint8_t set_idx);

    uint8_t (*store_parameter_set_redis)(uint8_t set_idx, uint8_t *data, uint8_t len, uint8_t cur_chrg_sta);

    uint8_t (*store_parameter_save_redis)(void);

    uint8_t (*store_get_parameter_len)(uint8_t set_idx, uint8_t *len);

    void (*cmp_set_ext_flash)(drv_flash_dev_stu *ext_flash, uint8_t type);

} cmp_store_param_dev_stu;

int cmp_store_param_subdev_create(void);

#endif