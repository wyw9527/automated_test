#ifndef _CMP_PLUG_H_
#define _CMP_PLUG_H_

#include "cmp.h"
#include "cmp_cp.h"
#include "cmp_cc.h"
#include "cmp_sock_temp.h"
#include "cmp_measure.h"

typedef enum
{
    PLUG_CMD_OFF,
    PLUG_CMD_ON
} CMP_PLUG_CMD_TYPE;

typedef struct
{
    uint8_t sock_type; /* 0 抢座 1 枪线 */
    uint8_t plug_idx;  /* 0 枪索引 */
} cmp_plug_param_stu;

typedef struct
{
    uint8_t cpcc_sta; /* cpcc开关状态 */
} cmp_plug_procc_stu;

typedef struct
{
    drv_relay_dev_stu *contactor; /* 接触器 */

    drv_relay_dev_stu *elock; /* 电子锁 */

    cmp_cp_dev_stu *cp; /* cp */

    cmp_cc_dev_stu *cc; /* cc */

    cmp_sock_temp_dev_stu *sock_temp; /* sock_temp */

    drv_dido_do_dev_stu *cp_io_sw; /* cpcc switch */

    const cmp_measure_dev_stu *messure;

    cmp_plug_param_stu param;

    cmp_plug_procc_stu procc;
} cmp_plug_stu;

typedef struct
{
    void (*plug_set_sock_type)(uint8_t type);

    void (*plug_work)(uint8_t period);

    uint8_t (*plug_get_conn_sta)(void); /* 充电枪连接状态 */

    uint8_t (*plug_get_cp_sta)(void); /* 获取CP状态 */

    void (*plug_cpcc_sw)(CMP_PLUG_CMD_TYPE cmd);

    void (*plug_set_chrg_curr)(uint16_t curr);

    void (*plug_stop_pwm_output)(void);

    void (*plug_relay_fault_handler)(void);

    void (*plug_elock_ctrl)(CMP_PLUG_CMD_TYPE cmd);

    void (*plug_contactor_ctrl)(CMP_PLUG_CMD_TYPE cmd);

} cmp_plug_dev_stu;

#endif