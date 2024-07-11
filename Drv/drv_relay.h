#ifndef _DRV_RELAY_H_
#define _DRV_RELAY_H_

#include "drv.h"

typedef void (*func_relay_bond_handler)(void);
typedef uint16_t (*func_relay_judg_other_hook)(uint8_t phase);

typedef struct
{
    bsp_io_dev_stu *relay_io;

} drv_relay_pub_dev_stu;

typedef struct
{
    BSP_SOURCE_TYPE io_ctrl;
    BSP_SOURCE_TYPE io_pwr;
    BSP_SOURCE_TYPE io_sta;
} drv_relay_io_stu;

typedef struct
{
    BSP_SOURCE_TYPE ctrl_l1;
    BSP_SOURCE_TYPE ctrl_l2;

    BSP_SOURCE_TYPE sta_l1;
    BSP_SOURCE_TYPE sta_l2;

    BSP_SOURCE_TYPE pwr;
} drv_relay_tphase_stu;

typedef struct
{
    uint8_t relay_type;    // 三相/单相
    uint8_t relay_fb_type; // 继电器导通时的反馈电平
    uint8_t relay_act_type;
    uint8_t chrg_type; /* 枪座/枪线 */
} drv_relay_param_stu;

typedef struct
{
    uint8_t cmd_flag;    /* 命令待执行标志 */
    uint8_t cmp_type;    /* 命令类型 0 断开  1 闭合 */
    uint8_t fb_state;    /* 继电器反馈状态 */
    uint8_t relay_alarm; /* 告警状态 */
    uint8_t step;        /* 执行步骤 */
    uint16_t count;
    uint16_t fault_count;
    uint8_t act_flag;   /* 动作完成标志 */
    uint16_t act_count; /* 动作次数 */

    uint8_t zero_flag;     /* 零点标志 */
    uint8_t zero_step;     /* 执行步骤 */
    uint8_t zero_tmr_flag; /* 定时器启动标志 */
} drv_relay_procc_stu;

typedef struct
{
    drv_relay_param_stu param;

    drv_relay_procc_stu procc;

    func_relay_bond_handler callback_func;

    func_relay_judg_other_hook judg_hook_func;

} drv_relay_stu;

typedef struct
{
    void (*drv_relay_work)(uint8_t period); // 工作

    void (*drv_relay_on)(void); // 闭合开关

    void (*drv_relay_off)(void); // 断开开关

    void (*drv_relay_fault_handler)(void);

    void (*drv_relay_bonding_register)(func_relay_bond_handler callback_func);

    void (*drv_relay_judgment)(func_relay_judg_other_hook callback_func);

} drv_relay_dev_stu;

#endif