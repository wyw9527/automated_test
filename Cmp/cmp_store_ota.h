#ifndef _CMP_STORE_OTA_H_
#define _CMP_STORE_OTA_H_

#include "cmp.h"

#define CMP_STORE_OTA_REDIS_BUFF_SIZE 1024
#define CMP_STORE_NEW_SOFT_FLAG 0xA55AA55A
#define CMP_STORE_ERR_SOFT_FLAG 0x5A5AA5A5

typedef struct
{
    uint8_t ota_buff[CMP_STORE_OTA_REDIS_BUFF_SIZE];
    uint16_t ota_buff_idx;
} cmp_store_ota_redis_stu;

typedef struct
{
    uint32_t ota_soft_size;
    uint32_t ota_soft_crc;
    uint16_t ota_pack_size;
    uint16_t ota_pack_cnt;
	uint16_t ota_soft_ver;
} cmp_store_ota_info_stu;

typedef struct
{
    uint16_t ota_pack_idx;
    uint32_t ota_addr_head;
} cmp_store_ota_procc_stu;

typedef struct
{
    uint32_t new_soft_flag; /* 升级标志 */
    uint32_t byte_cnt;      /* 程序总字节数 */
    uint32_t err_flag;
    uint32_t crc32_val;
} cmp_store_ota_conf_stu;

typedef struct
{
    cmp_store_ota_conf_stu conf;
    cmp_store_ota_procc_stu procc;
    cmp_store_ota_info_stu info;
    cmp_store_ota_redis_stu redis;
} cmp_store_ota_stu;

typedef struct
{
    drv_flash_dev_stu *inn_flash;
} cmp_store_ota_c_stu;

typedef struct
{
    uint8_t (*store_ota_start)(cmp_store_ota_info_stu *ota_info);

    void (*store_ota_procc_save_data)(uint8_t *data, uint16_t len);

    uint8_t (*store_ota_check_soft)(uint32_t soft_size, uint32_t crc);

    void (*store_ota_save_break_info)(uint8_t pack_idx);

    void (*store_ota_set_ext_flash)(drv_flash_dev_stu *ext_flash);

    uint32_t (*store_ota_read_info)(void);

    void (*store_ota_erae_info)(void);

} cmp_store_ota_dev_stu;

int cmp_store_ota_subdev_create(void);

#endif