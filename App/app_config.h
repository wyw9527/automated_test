#ifndef _SYS_CONFIG_H_
#define _SYS_CONFIG_H_

#define APP_PLUG_NUM 1

#include <rtthread.h>

/* include driver */
#include "drv.h"
#include "drv_clock.h"
#include "drv_analog.h"
#include "drv_dido.h"
#include "drv_meter.h"
#include "drv_serial.h"
#include "drv_load_can.h"
#include "drv_load_485.h"
#include "drv_power_box.h"



/* include cmp */
#include "cmp.h"
#include "cmp_load.h"
#include "cmp_power.h"
#include "cmp_prot_mt.h"
#include "cmp_prot_upper.h"
#include "cmp_ble.h"
#include "cmp_w5500.h"

/* app task prio define 1~14 */
#define APP_SERV_PRIO 1
#define APP_SYS_PRIO 2
#define APP_UPPER_PRIO 3

#define APP_ALARM_UNIT_MAX_CNT 32

extern struct rt_mailbox mb_sys;
extern struct rt_mailbox mb_card;
extern struct rt_mailbox mb_chrg;
extern struct rt_mailbox mb_debug;
extern struct rt_mailbox mb_mgmt;
extern struct rt_mailbox mb_net;
extern struct rt_mailbox mb_serv;

typedef enum
{
    MB_CARD_CMD_START_RES,
    MB_CARD_CMD_STOP_RES,
    MB_CARD_CMD_READ_RPY_SUCC,
    MB_CARD_CMD_READ_RPY_FAIL,
    MB_CARD_CMD_READ_CAL_RES,
    MB_CARD_CMD_OCPP_READ,

    MB_CHRG_CMD_CARD_SWIP, /* 刷卡充电 */
    MB_CHRG_CMD_SERV_PLUG_STA,
    MB_CHRG_CMD_SERV_START_NOTIFY,
    MB_CHRG_CMD_SERV_STOP_NOTIFY,
    MB_CHRG_CMD_MGMT_CHRG_STA,

    MB_NET_CMD_SERV_SPP,
    MB_NET_CMD_SERV_TX,
    MB_NET_CMD_CHRG_START,
    MB_NET_CMD_CHRG_STOP,

    MB_SERV_CMD_NET_TX,
    MB_SERV_CMD_NET_RESET,

    MB_SERV_CMD_CARD_READ_CMD,
    MB_SERV_CMD_CARD_READ_CAL,
    MB_SERV_CMD_CHRG_START_CMD,
    MB_SERV_CMD_CHRG_STOP_CMD,
    MB_SERV_CMD_CHRG_REBOOT,
    MB_SERV_CMD_CHRG_STATION_CURR_SET_CMD,
    MB_SERV_CMD_CHRG_COMM_ABN,
    MB_SERV_CMD_MGMT_RESTORE,
    MB_SERV_CMD_PARAM_BRTNESS,
    MB_SERV_CMD_PARAM_CURRMAX,

    MB_MGMT_CMD_CHRG_TIM_START,
    MB_MGMT_CMD_CHRG_PNC_STA,

    MB_DEBUG_CMD_CHRG_START,
    MB_DEBUG_CMD_CHRG_STOP,
    MB_DEBUG_CMD_CARD_READ,
    MB_DEBUG_CMD_CARD_CAL,
    MB_DEBUG_CMD_CHRG_REBOOT,
    MB_DEBUG_CMD_CHRG_PLUG_STA,
    MB_DEBUG_CMD_MGMT_RESTORE,
    MB_DEBUG_CMD_CHRG_STOP_REASON,
    MB_DEBUG_CMD_CHRG_AGEING_DATA,

    MB_SERV_CMD_CONTROL_CAN_CMD,

} APP_MB_CMD_TYPE;

typedef struct
{
    uint8_t from_app_id;
    APP_MB_CMD_TYPE cmd;
    uint32_t data; // 数值类型
    uint8_t *msg;  // 多值指类型
} app_conf_mb_stu;

typedef union
{
    uint32_t val;
    struct
    {
        uint8_t byte[4];
    } ele;
} app_mb_data_stu;

typedef struct
{
    uint16_t stop_time;
    uint16_t stop_energy;
    uint16_t stop_money;
    uint8_t stop_soc;
    uint8_t stop_code[6];
} app_pole_remote_start_stu;

typedef struct
{
    uint8_t fee_type;
    uint16_t fee_no; /* 无效时填充0xFFFF*/
} app_pole_fee_stu;

typedef struct
{
    uint8_t card_type;
    uint8_t card_uid;
} app_pole_card_start_stu;

typedef struct
{
    uint8_t start_type; /* 0 card 1 pnc  2 tim  3 app/平台  4 ble 5 预约 */
    uint8_t order_no[8];
    app_pole_remote_start_stu remote;
    app_pole_card_start_stu card;
    app_pole_fee_stu fee;
} app_pole_chrg_stu;

typedef struct
{
    uint8_t upt_flag;
    uint8_t alarm_cnt;
    uint32_t alarm[APP_ALARM_UNIT_MAX_CNT];
} app_device_alarm_stu;

void app_config_mb_init(void);
void app_config_task_init(void);
void app_config_mb_send(uint8_t to_mb_id, app_conf_mb_stu *mb_msg);
app_pole_chrg_stu *get_pole_chrg_info(uint8_t plug_idx);

void app_push_alarm_unit(uint8_t plug_idx, uint8_t unit, uint8_t sub_unit, uint8_t val);
const app_device_alarm_stu *app_get_alarm_unit(void);
void app_reset_alarm_flag(void);

#endif