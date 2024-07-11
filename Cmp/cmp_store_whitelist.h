#ifndef _CMP_STORE_WHITELIST_H_
#define _CMP_STORE_WHITELIST_H_

#include "cmp.h"

#define CMP_STORE_WHITELIST_MAX_CNT	0x08
#define CMP_STORE_FLAG 	0xA5A5A5A5

typedef struct
{
    uint32_t card_uid[CMP_STORE_WHITELIST_MAX_CNT];
    uint8_t card_cnt;
}cmp_store_wlist_cont_stu;

typedef struct
{
    uint32_t store_flag;
    cmp_store_wlist_cont_stu cont;
}cmp_store_wlist_info_stu;

typedef struct
{
    uint32_t store_addr;
}cmp_store_wlist_param_stu;
    
typedef struct
{
    cmp_store_wlist_param_stu param;
    
    drv_flash_dev_stu* ext_flash;
    
    cmp_store_wlist_info_stu info;
}cmp_store_whitelist_stu;

typedef struct
{
    void (*cmp_set_ext_flash)(drv_flash_dev_stu* ext_flash,uint8_t type);

    uint8_t (*store_whitelist_query)(cmp_store_wlist_cont_stu* wlist);

    uint8_t (*store_whitelist_add)(uint32_t uid);

    uint8_t (*store_whitelist_del)(uint32_t uid);
    
}cmp_store_whitelist_dev_stu;

int cmp_store_whitelist_subdev_create();
























#endif