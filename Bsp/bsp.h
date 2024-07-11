#ifndef _BSP_H_
#define _BSP_H_

#include <rtthread.h>
#include "rthw.h"
#include "gd32f30x.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"

#define BSP_MB_APP_ID 15
#define BSP_CHRG_MIN_CURR 60      /* 充电桩最小电流 ×10 */
#define BSP_CHRG_MIN_STAT_CURR 65 /* 充电桩暂停后再启动最小电流门限 ×10 */
#define BSP_DEV_NAME_MAX_LEN 12   /* 程序设备对象名称最大长度 */

/* ---------------------------内部存储flash信息------------------------*/
#define CMP_STORE_INN_PAGE_SIZE 0x800
#define CMP_STORE_INN_ADDR_BOOTLOAD 0x08000000 /* bootload 地址 */
#define CMP_STORE_INN_ADDR_APP 0x08008000      /* app区域地址 32K*/
#define CMP_STORE_INN_ADDR_APP_BAK 0x08028000  /* 备份区地址 */
#define CMP_STORE_INN_ADDR_APP_BUFF 0x08048000 /* 暂存区地址 */
#define CMP_STORE_INN_ADDR_APP_CONF 0x08068000 /* 升级配置区地址 */

#define CMP_STORE_INN_ADDR_PARAM_SYS 0x08070000    /* 系统定值 4K */
#define CMP_STORE_INN_ADDR_PARAM_SET 0x08071000    /* 系统参数 4K */
#define CMP_STORE_INN_ADDR_TIM 0x08072000          /* 定时记录 4K */
#define CMP_STORE_INN_ADDR_WHITELIST 0x08073000    /* 白名单 4K */
#define CMP_STORE_INN_ADDR_MODEL 0x08074000        /* 计费模型 4K */
#define CMP_STORE_INN_ADDR_ORDER_INFO 0x08075000   /* 订单统计信息 4K */
#define CMP_STORE_INN_ADDR_ORDER_UNSEND 0x08076000 /* 未发送订单 8K 64条 */
#define CMP_STORE_INN_ADDR_ORDER_ABN 0x08078000    /* 异常订单 8K 64条 */
#define CMP_STORE_INN_ADDR_ORDER_HIS 0x0807A000    /* 历史订单 8K 64条 */

#define CMP_STORE_INN_ADDR_PROCC_BAK_ADDR 0x0807C000 /* 掉电保存过程信息区 1K字节 */
#define CMP_STORE_INN_ADDR_ORDER_BAK_ADDR 0x0807C400 /* 掉电保存订单备份区 1K字节 */

#define CMP_STORE_INN_ORDER_UNSEND_SIZE 64 /* 外部存储未发送订单数量 */
#define CMP_STORE_INN_ORDER_ABN_SIZE 64    /* 外部存储异常订单数量 */
#define CMP_STORE_INN_ORDER_HIS_SIZE 64    /* 外部存储历史订单数量 */

/* ---------------------------外部存储flash信息------------------------*/
#define CMP_STORE_EXT_ADDR_PARAM_SYS 0x00000    /* 系统定值 4K */
#define CMP_STORE_EXT_ADDR_PARAM_SET 0x01000    /* 系统参数 4K */
#define CMP_STORE_EXT_ADDR_TIM 0x02000          /* 定时记录 4K */
#define CMP_STORE_EXT_ADDR_WHITELIST 0x03000    /* 白名单 4K */
#define CMP_STORE_EXT_ADDR_MODEL 0x04000        /* 计费模型 4K */
#define CMP_STORE_EXT_ADDR_ORDER_INFO 0x05000   /* 订单统计信息 4K */
#define CMP_STORE_EXT_ADDR_ORDER_UNSEND 0x06000 /* 未发送订单 40K 320条 */
#define CMP_STORE_EXT_ADDR_ORDER_ABN 0x10000    /* 异常订单 40K 320条 */
#define CMP_STORE_EXT_ADDR_ORDER_HIS 0x1A000    /* 历史订单 80K 640条 */

#define CMP_STORE_EXT_ORDER_UNSEND_SIZE 320 /* 外部存储未发送订单数量 */
#define CMP_STORE_EXT_ORDER_ABN_SIZE 320    /* 外部存储异常订单数量 */
#define CMP_STORE_EXT_ORDER_HIS_SIZE 640    /* 外部存储历史订单数量 */

/*---------------------对象设备名称---------------------------------------*/
#define BSP_DEV_NAME_UART "BSP_UART"       // UART DEV NAME
#define BSP_DEV_NAME_IO "BSP_IO"           // IO DEV NAME
#define BSP_DEV_NAME_RTC "BSP_RTC"         // RTC DEV NAME
#define BSP_DEV_NAME_ADC "BSP_ADC"         // ADC DEV NAME
#define BSP_DEV_NAME_SPI "BSP_SPI"         // SPI DEV NAME
#define BSP_DEV_NAME_PWM_CP "BSP_PWM_CP"   // PWM CP DEV NAME
#define BSP_DEV_NAME_PWM_LED "BSP_PWM_LED" // PWM LED NAME
#define BSP_DEV_NAME_CAN "BSP_CAN" // PWM LED NAME

/**************************************************资源相关宏******************************************************/
/* 接口类型 */
#define CHRG_PLUG 1  // 枪线
#define CHRG_STACK 0 // 枪座

/* 漏电流类型 */
#define LEAK_AC 0 // 交流
#define LEAK_DC 1 // 直流

/* 接触器类型 */
#define CONN_SINGLE 0   // 单相
#define CONN_TPHASE 1   // 三相
#define CONN_FB_H 1     // 接触器导通反馈为高电平
#define CONN_FB_L 0     // 接触器导通反馈为低电平
#define CONN_ACT_NORM 0 // 标准
#define CONN_ACT_ZERO 1 // 零点

/* 电子锁类型 */
#define ELOCK_FB_H 1 // 电子锁反馈为高电平
#define ELOCK_FB_L 0 // 电子锁反馈为高电平

/* 存储类型 */
#define FLASH_EXT 0 // 外部存储
#define FLASH_INN 1 // 内部存储

#define ANALOG_TYPE_0 0 // 电压计算方式0 检测波形算均方根
#define ANALOG_TYPE_1 1 // 电压计算方式1 不检测波形，直接计算峰峰值

/*****************************************************板载资源*******************************************************/
/* 指示灯定义 */
// #define __DOTLED // 点状指示灯
#define __PWMLED // PWM指示灯

typedef enum
{
    /* BSP ADC */
    BSP_SOURCE_ADC_VOLT_L1,     // L1相电压
    BSP_SOURCE_ADC_VOLT_L2,     // L2相电压
    BSP_SOURCE_ADC_VOLT_L3,     // L3相电压
    BSP_SOURCE_ADC_CURR_L1,     // L1相电流
    BSP_SOURCE_ADC_CURR_L2,     // L2相电流
    BSP_SOURCE_ADC_CURR_L3,     // L3相电流
    BSP_SOURCE_ADC_TEMP_BOARD,  // 板载温度
    BSP_SOURCE_ADC_TEMP_POWER1, // 功率板1温度
    BSP_SOURCE_ADC_TEMP_POWER2, // 功率板2温度
    BSP_SOURCE_ADC_TEMP_PLUG1,  // 充电枪1温度
    BSP_SOURCE_ADC_CP1,         // CP
    BSP_SOURCE_ADC_CC1,         // CC
    BSP_SOURCE_ADC_PE,          // PE
    BSP_SOURCE_ADC_LEAK,        // 漏电流
    BSP_SOURCE_ADC_NE,          // N线反接

    /* BSP DO */
    BSP_SOURCE_DO_RUN_LED,           // 系统运行灯
    BSP_SOURCE_DO_LOAD_RS485_CTRL,   // 负载
    BSP_SOURCE_DO_POWER_RS485_CTRL,  // 电源
    BSP_SOURCE_DO_ELOCK_CTRL1,       // 电子锁控制信号
    BSP_SOURCE_DO_ELOCK_PWR,         // 电子锁功率信号
    BSP_SOURCE_DO_RELAY_CTRL1,       // 继电器控制信号1
    BSP_SOURCE_DO_RELAY_CTRL2,       // 继电器控制信号2
    BSP_SOURCE_DO_RELAY_CTRL3,       // 继电器控制信号3
    BSP_SOURCE_DO_RELAY_CTRLN,       // 继电器控制信号N（N线）
    BSP_SOURCE_DO_RELAY_PWR,         // 继电器功率信号
    BSP_SOURCE_DO_CC_CP_SW,          // CC-CP开关
    BSP_SOURCE_DO_ESP32_RESET,       // ESP32复位信号
    BSP_SOURCE_DO_AC_LEAK_PROD,      // 交流漏电流产生信号
    BSP_SOURCE_DO_AC_LEAK_SELF_TEST, // 交流漏电流自检信号
    BSP_SOURCE_DO_LED_R,             // DOTLED_R指示灯驱动
    BSP_SOURCE_DO_LED_G,             // DOTLED_G指示灯驱动
    BSP_SOURCE_DO_LED_B,             // DOTLED_B指示灯驱动

    /* BSP DI */
    BSP_SOURCE_DI_ELOCK_STA1,    // 电子锁反馈1
    BSP_SOURCE_DI_RELAY_STA1,    // 继电器反馈1
    BSP_SOURCE_DI_RELAY_STA2,    // 继电器反馈2
    BSP_SOURCE_DI_RELAY_STA3,    // 继电器反馈3
    BSP_SOURCE_DI_EME_STA1,      // 急停反馈1
    BSP_SOURCE_DI_PNC_STA1,      // 停止按钮反馈1
    BSP_SOURCE_DI_PLUG_SEL,      // 枪线枪座选择
    BSP_SOURCE_DI_OUT_SHOUT_STA, // 输出短路反馈
    BSP_SOURCE_DI_NE,            // N线反接
    BSP_SOURCE_DI_OPENLID_STA,   // 开盖检测反馈

    BSP_SOURCE_NUM,

} BSP_SOURCE_TYPE;

/**************************************************其他******************************************************/
typedef struct
{
    uint32_t dma_periph;
    dma_channel_enum dma_channelx;
    rcu_periph_enum dma_rcu;
    uint8_t nvic_irq;
} bsp_periph_dma_stu;

typedef struct bsp_dev_list
{
    char name[BSP_DEV_NAME_MAX_LEN];

    void *bsp_dev;

    struct bsp_dev_list *next;

} bsp_dev_lists;

typedef struct
{
    uint8_t from_app_id;
    uint32_t data; // 数值类型
} bsp_alarm_mb_stu;

typedef union
{
    uint32_t val;
    struct
    {
        uint8_t byte[4];
    } eles;
} bsp_int_union_stu;

typedef struct
{
    uint8_t plug_idx;
    uint8_t unit;
    uint8_t sub_unit;
    uint8_t len;
    uint8_t *data;
} bsp_dev_oper_stu;

rcu_periph_enum bsp_get_pin_rcu(uint32_t gpio_periph);

uint8_t bsp_dev_append(const char *name, void *dev);

void *bsp_dev_find(const char *name);

void bsp_alarm_mb_send(uint8_t plug_idx, uint8_t unit, uint8_t sub_unit, uint8_t val);

void bsp_bind_oper_data_dev(uint8_t plug_idx, uint8_t unit, uint8_t sub_unit, uint8_t *data, uint8_t len);

const bsp_dev_oper_stu *bsp_get_oper_data(uint8_t plug_idx, uint8_t unit, uint8_t sub_unit);

uint8_t bsp_get_alram_val(uint8_t *buff);

#endif