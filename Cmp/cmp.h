#ifndef _CMP_H_
#define _CMP_H_

#include <rtthread.h>
#include "bsp.h"
#include "drv.h"



#define CMP_DEV_NAME_LOAD "load_dev"
#define CMP_DEV_NAME_POWER "power_dev"
#define CMP_DEV_NAME_PROT_UPPER "prot_upper_dev"
#define CMP_DEV_NAME_PROT21_MT "prot21_mt_dev"
// #define CMP_DEV_NAME_STORE "store_dev"
#define CMP_DEV_NAME_BLE "ble_dev"
#define CMP_DEV_NAME_W5500 "w5500_dev"
#define CMP_DEV_NAME_RYJSON "ryjson_dev"


#define ESP_CMD_SERI_NUM 8
#define ESP_CMD_SERI_LEN 32

typedef struct
{
    uint8_t head;
    uint8_t trail;
    char cmd[ESP_CMD_SERI_NUM][ESP_CMD_SERI_LEN];
}esp_cmd_seri_stu;


void *cmp_dev_find(const char *name);

uint8_t cmp_dev_append(const char *name, void *dev);

void cmp_set_plug_sta(uint8_t idx, uint8_t sta);
uint8_t cmp_get_plug_sta(uint8_t idx, uint8_t *ret);
uint8_t cmp_get_plug_uasable_sta(uint8_t idx, uint8_t *ret);

#endif