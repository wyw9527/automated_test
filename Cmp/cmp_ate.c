#include "cmp_ate.h"

static cmp_ate_stu ate_conf[CMP_ATE_NUM];

/***************************************************************************************
 * @Function    : cmp_ate_pack_and_send_msg()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/5
 ****************************************************************************************/
static void cmp_ate_pack_and_send_msg(cmp_ate_stu *ate_dev, uint8_t cmd, uint8_t *data, uint16_t data_len)
{
    uint8_t pos = 0;
    uint8_t buff[256];
    uint32_t crc32;

    const uint8_t *pole_sn;
    pole_sn = ate_conf->store->param->store_parameter_query(1);

    buff[pos++] = 'M';
    buff[pos++] = 'T';
    memcpy(&buff[pos], pole_sn, 28);
    pos += 28;
    buff[pos++] = ate_dev->param.encry_type;
    buff[pos++] = cmd;
    buff[pos++] = 0x0;
    memcpy(&buff[pos], (uint8_t *)&data_len, 2);
    pos += 2;
    memcpy(&buff[35], data, data_len);
    pos += data_len;
    crc32 = crc32_calc(buff, pos);
    memcpy(&buff[pos], (uint8_t *)&crc32, 4);
    pos += 4;

    ate_dev->tx_msg_func(buff, pos);
}

/***************************************************************************************
 * @Function    : cmp_ate_param_set_rpy()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/5
 ****************************************************************************************/
static void cmp_ate_param_set_rpy(cmp_ate_stu *ate_dev, uint8_t idx, uint8_t result)
{
    uint8_t buff[2];

    buff[0] = idx;
    buff[1] = result;

    cmp_ate_pack_and_send_msg(ate_dev, 0x01, buff, 2);
}

/***************************************************************************************
 * @Function    : cmp_ate_param_query_rpy()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/5
 ****************************************************************************************/
static void cmp_ate_param_query_rpy(cmp_ate_stu *ate_dev, uint8_t idx, uint8_t result, uint8_t len)
{
    uint8_t buff[128];
    const uint8_t *msg;

    msg = ate_dev->store->param->store_parameter_query(idx);
    buff[0] = idx;
    buff[1] = result;
    memcpy(&buff[2], msg, len);

    cmp_ate_pack_and_send_msg(ate_dev, 0x02, buff, len + 2);
}

/***************************************************************************************
 * @Function    : cmp_ate_param_set()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/5
 ****************************************************************************************/
static void cmp_ate_param_set(cmp_ate_stu *ate_dev, uint8_t *data, uint16_t data_len)
{
    uint8_t idx, msg_len, param_len, result = 0;

    idx = data[0];
    msg_len = data[1];

    if (msg_len + 2 > data_len)
        return;

    if (ate_dev->store->param->store_get_parameter_len(idx, &param_len) == 0)
    {
        result = 1;
    }
    else if (param_len > msg_len)
    {
        result = 2;
    }
    else if (ate_dev->store->param->store_parameter_set_redis(idx, &data[2], param_len, 0) > 0)
    {
        result = 3;
    }

    if (result == 0)
    {
        ate_dev->store->param->store_parameter_save_redis();
    }

    cmp_ate_param_set_rpy(ate_dev, idx, result);
}

/***************************************************************************************
 * @Function    : cmp_ate_param_query()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/5
 ****************************************************************************************/
static void cmp_ate_param_query(cmp_ate_stu *ate_dev, uint8_t *data, uint16_t data_len)
{
    uint8_t idx, param_len, result = 0;

    idx = data[0];
    if (ate_dev->store->param->store_get_parameter_len(idx, &param_len) == 0)
    {
        result = 1;
    }

    cmp_ate_param_query_rpy(ate_dev, idx, result, param_len);
}

/***************************************************************************************
 * @Function    : cmp_ate_param_soft_version_query()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/5
 ****************************************************************************************/
static void cmp_ate_param_soft_version_query(cmp_ate_stu *ate_dev, uint8_t *data, uint16_t data_len)
{
    uint8_t soft_type;
    uint8_t buff[4];

    soft_type = data[0];
    buff[0] = soft_type;
    if (soft_type == 0) // 充电桩软件版本
    {
        buff[1] = math_decimal_2_bcd(PROT_SOFT_VER_MAIN);
        buff[2] = math_decimal_2_bcd(PROT_SOFT_VER_SEC);
    }
    else if (soft_type == 3)
    {
        buff[1] = 0;
        buff[2] = SOFTWARE_TYPE;
    }
    else
    {
        return;
    }

    cmp_ate_pack_and_send_msg(ate_dev, 0x11, buff, 3);
}

/***************************************************************************************
 * @Function    : cmp_ate_alarm_data_query()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/5
 ****************************************************************************************/
static void cmp_ate_alarm_data_query(cmp_ate_stu *ate_dev, uint8_t *data, uint16_t data_len)
{
    uint8_t buff[128];

    buff[0] = bsp_get_alram_val(&buff[1]);
    cmp_ate_pack_and_send_msg(ate_dev, 0x12, buff, buff[0] * 4 + 1);
}

/***************************************************************************************
 * @Function    : cmp_ate_state_data_query()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/5
 ****************************************************************************************/
static void cmp_ate_state_data_query(cmp_ate_stu *ate_dev, uint8_t *data, uint16_t data_len)
{
    uint8_t buff[32], len;

    const bsp_dev_oper_stu *oper = bsp_get_oper_data(data[0], data[1], data[2]);
    if (oper == RT_NULL)
    {
        buff[0] = 1;
        len = 1;
    }
    else
    {
        buff[0] = 0;
        memcpy(&buff[1], data, 3);
        memcpy(&buff[4], oper->data, oper->len);
        len = 4 + oper->len;
    }

    cmp_ate_pack_and_send_msg(ate_dev, 0x13, buff, len);
}

/***************************************************************************************
 * @Function    : cmp_ate_ota_start()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/5
 ****************************************************************************************/
static void cmp_ate_ota_start(cmp_ate_stu *ate_dev, uint8_t *data, uint16_t data_len)
{
    uint8_t buff[8];

    memcpy((uint8_t *)&ate_dev->procc.ota.ota_soft_ver, data, 2);
    memcpy((uint8_t *)&ate_dev->procc.ota.ota_pack_cnt, &data[2], 2);
    memcpy((uint8_t *)&ate_dev->procc.ota.ota_pack_size, &data[4], 2);
    memcpy((uint8_t *)&ate_dev->procc.ota.ota_soft_crc, &data[6], 4);
    memcpy((uint8_t *)&ate_dev->procc.ota.ota_soft_size, &data[10], 4);

    ate_dev->procc.cur_pack_idx = ate_dev->store->ota->store_ota_start(&ate_dev->procc.ota);
    buff[0] = 0;
    cmp_ate_pack_and_send_msg(ate_dev, 0x31, buff, 1);
}

/***************************************************************************************
 * @Function    : cmp_ate_ota_data_dowmload()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/5
 ****************************************************************************************/
static void cmp_ate_ota_data_dowmload(cmp_ate_stu *ate_dev, uint8_t *data, uint16_t data_len)
{
    uint8_t buff[8];
    uint16_t pack_idx, msg_len;
    uint32_t crc32, cal_crc;

    pack_idx = *(uint16_t *)data;

    if (pack_idx != ate_dev->procc.cur_pack_idx)
    {
        memcpy(buff, (uint8_t *)&ate_dev->procc.cur_pack_idx, 2);
        buff[2] = 1;
        cmp_ate_pack_and_send_msg(ate_dev, 0x32, buff, 3);
        return;
    }

    memcpy(buff, data, 2);
    crc32 = *(uint32_t *)&data[2];
    msg_len = *(uint32_t *)&data[6];
    if (8 + msg_len > data_len)
        return;
    cal_crc = crc32_calc(&data[6], msg_len + 2);
    if (cal_crc != crc32)
        return;

    ate_dev->store->ota->store_ota_procc_save_data(&data[8], msg_len);
    ate_dev->procc.cur_pack_idx++;

    buff[2] = 0;
    cmp_ate_pack_and_send_msg(ate_dev, 0x32, buff, 3);
}

/***************************************************************************************
 * @Function    : cmp_ate_ota_result()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/5
 ****************************************************************************************/
static void cmp_ate_ota_result(cmp_ate_stu *ate_dev, uint8_t *data, uint16_t data_len)
{
    uint8_t buff[4];

    buff[0] = ate_dev->store->ota->store_ota_check_soft(ate_dev->procc.ota.ota_soft_size, ate_dev->procc.ota.ota_soft_crc);
    cmp_ate_pack_and_send_msg(ate_dev, 0x33, buff, 1);
}

/***************************************************************************************
 * @Function    : cmp_ate_remote_chrg_ctrl_cmd()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/19
 ****************************************************************************************/
static void cmp_ate_remote_chrg_ctrl_cmd(cmp_ate_stu *ate_dev, uint8_t *data, uint16_t data_len)
{
    if (ate_dev->cmd_hook_func == RT_NULL)
        return;

    CMP_ATE_CMD_TYPE cmd = data[1] == 0x01 ? ATE_CMD_CHRG_START : ATE_CMD_CHRG_STOP;

    ate_dev->cmd_hook_func(cmd, data[0]);
}

/***************************************************************************************
 * @Function    : cmp_ate_remote_whitelist_card_cmd()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/19
 ****************************************************************************************/
static void cmp_ate_remote_whitelist_card_cmd(cmp_ate_stu *ate_dev, uint8_t *data, uint16_t data_len)
{
    if (ate_dev->cmd_hook_func == RT_NULL)
        return;

    CMP_ATE_CMD_TYPE cmd = data[0] == 0x00 ? ATE_CMD_CARD_READ : ATE_CMD_CARD_CAL;

    ate_dev->cmd_hook_func(cmd, data[1]);
}

/***************************************************************************************
 * @Function    : cmp_ate_ota_reboot_cmd()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/19
 ****************************************************************************************/
static void cmp_ate_ota_reboot_cmd(cmp_ate_stu *ate_dev, uint8_t *data, uint16_t data_len)
{
    if (ate_dev->cmd_hook_func == RT_NULL)
        return;

    ate_dev->cmd_hook_func(ATE_CMD_REBOOT, data[0]);
}

/***************************************************************************************
 * @Function    : cmp_ate_work_state_query()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/19
 ****************************************************************************************/
static void cmp_ate_work_state_query(cmp_ate_stu *ate_dev, uint8_t *data, uint16_t data_len)
{
    if (ate_dev->cmd_hook_func == RT_NULL)
        return;

    ate_dev->cmd_hook_func(ATE_CMD_PLUG_STA, data[0]);
}

/***************************************************************************
*@Function    :cmp_ate_pile_info_query
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2024-01-10
***************************************************************************/
static void cmp_ate_pile_info_query(cmp_ate_stu *ate_dev, uint8_t *data, uint16_t data_len)
{
    uint8_t info_type;
    uint8_t buff[4];

    info_type = data[0];

    if (info_type == 0)
    {
        buff[0] = 0;
        buff[1] = SOFTWARE_TYPE;

        cmp_ate_pack_and_send_msg(ate_dev, 0x15, buff, 2);
    }
}

/***************************************************************************
*@Function    :cmp_ate_stop_reason_query
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2024-01-10
***************************************************************************/
static void cmp_ate_stop_reason_query(cmp_ate_stu *ate_dev, uint8_t *data, uint16_t data_len)
{
    if (ate_dev->cmd_hook_func == RT_NULL)
        return;

    ate_dev->cmd_hook_func(ATE_CMD_STOP_REASON, data[0]);
}

/***************************************************************************************
 * @Function    : cmp_ate_rx_msg_analy()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/5
 ****************************************************************************************/
static void cmp_ate_rx_msg_analy(cmp_ate_stu *ate_dev, uint16_t data_cmd, uint8_t *data, uint16_t data_len)
{
    switch (data_cmd)
    {
    case 0x8001:
        cmp_ate_param_set(ate_dev, data, data_len);
        break;
    case 0x8002:
        cmp_ate_param_query(ate_dev, data, data_len);
        break;

    case 0x8011:
        cmp_ate_param_soft_version_query(ate_dev, data, data_len);
        break;
    case 0x8012:
        cmp_ate_alarm_data_query(ate_dev, data, data_len);
        break;
    case 0x8013:
        cmp_ate_state_data_query(ate_dev, data, data_len);
        break;
    case 0x8014:
        cmp_ate_work_state_query(ate_dev, data, data_len);
        break;
    case 0x8015:
        cmp_ate_pile_info_query(ate_dev, data, data_len);
        break;
    case 0x8016:
        cmp_ate_stop_reason_query(ate_dev, data, data_len);
        break;

    case 0x8021:
        cmp_ate_remote_chrg_ctrl_cmd(ate_dev, data, data_len);
        break;
    case 0x8022:
        cmp_ate_remote_whitelist_card_cmd(ate_dev, data, data_len);
        break;

    case 0x8031:
        cmp_ate_ota_start(ate_dev, data, data_len);
        break;
    case 0x8032:
        cmp_ate_ota_data_dowmload(ate_dev, data, data_len);
        break;
    case 0x8033:
        cmp_ate_ota_result(ate_dev, data, data_len);
        break;

    case 0x8034:
        cmp_ate_ota_reboot_cmd(ate_dev, data, data_len);
        break;
    }
}

/***************************************************************************
*@Function    :
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2024-04-07
***************************************************************************/
static uint8_t cmp_ate_prot2_rx_msg(CMP_ATE_TYPE ate_type, uint8_t *msg, uint16_t len)
{
    uint16_t data_len, data_cmd;
    int32_t rx_len;
    uint32_t crc32;
    uint8_t res = 0;

    const uint8_t *pole_sn;
    pole_sn = ate_conf->store->param->store_parameter_query(1);

    rx_len = len;

    while (rx_len > 39)
    {
        if (msg[0] != 'M')
        {
            msg++;
            rx_len--;
            continue;
        }

        if (msg[1] != 'T')
        {
            msg += 2;
            rx_len -= 2;
            continue;
        }

        if (memcmp(&msg[2], pole_sn, 28) != 0 && strncmp((char *)&msg[2], "FOXESS-CHARGER", 28) != 0)
        {
            msg += 30;
            rx_len -= 30;
            continue;
        }

        if (msg[30] < 1 || msg[30] > 2)
        {
            msg += 31;
            rx_len -= 31;
            continue;
        }

        data_len = *((uint16_t *)&msg[33]);
        if ((data_len + 39) > rx_len)
        {
            msg += 35;
            rx_len -= 35;
            continue;
        }

        crc32 = crc32_calc(msg, data_len + 35);
        if (crc32 != *(uint32_t *)&msg[data_len + 35])
        {
            rx_len = rx_len - data_len - 39;
            msg = msg + data_len + 39;
            continue;
        }
        data_cmd = *(uint16_t *)&msg[31];

        cmp_ate_rx_msg_analy(&ate_conf[ate_type], data_cmd, &msg[35], data_len);

        rx_len = rx_len - data_len - 39;
        msg = msg + data_len + 39;

        if (++res > 10)
            return res;
    }
    return res;
}

/***************************************************************************
*@Function    :
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2024-04-07
***************************************************************************/
static uint8_t cmp_ate_prot1_rx_msg(CMP_ATE_TYPE ate_type, uint8_t *msg, uint16_t len)
{
    uint16_t data_len;
    uint8_t cmd;

    if (msg[0] != 'M' || msg[1] != 'T')
    {
        return 0;
    }

    data_len = *((uint16_t *)&msg[36]);
    if (39 + data_len > len)
    {
        return 0;
    }

    // if (msg[data_len + 38] != mt_data_check(&msg[35], 3 + data_len))
    // {
    //     return 0;
    // }

    cmd = msg[35];
    if (cmd != 0xDB)
    {
        return 0;
    }

    ate_conf[ate_type].cmd_hook_func(ATE_CMD_AGEING_DATA, 0);

    return 1;
}
/***************************************************************************************
 * @Function    : cmp_ate_set_rx_msg()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/5
 ****************************************************************************************/
static uint8_t cmp_ate_set_rx_msg(CMP_ATE_TYPE ate_type, uint8_t *msg, uint16_t len)
{
    uint8_t res = 0;
    res = cmp_ate_prot2_rx_msg(ate_type, msg, len);
    if (res > 0)
    {
        return res;
    }

    return cmp_ate_prot1_rx_msg(ate_type, msg, len);
}

/***************************************************************************************
 * @Function    : cmp_ate_set_tx_hnd_msg()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/5
 ****************************************************************************************/
static void cmp_ate_set_tx_hnd_msg(CMP_ATE_TYPE ate_type, ate_tx_msg_func_type tx_func)
{
    ate_conf[ate_type].tx_msg_func = tx_func;
}

/***************************************************************************************
 * @Function    : cmp_ate_dev_conf_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/5
 ****************************************************************************************/
static void cmp_ate_dev_conf_init(void)
{
    for (int i = 0; i < CMP_ATE_NUM; i++)
    {
        ate_conf[i].store = cmp_dev_find(CMP_DEV_NAME_STORE);

        ate_conf[i].param.encry_type = 0x01;
    }
}

/***************************************************************************************
 * @Function    : cmp_ate_cmd_hook_func()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/19
 ****************************************************************************************/
static void cmp_ate_cmd_hook_func(CMP_ATE_TYPE ate_type, ate_cmd_hook_func_type hook)
{
    ate_conf[ate_type].cmd_hook_func = hook;
}

/***************************************************************************************
 * @Function    : cmp_ate_cmd_result()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/19
 ****************************************************************************************/
static void cmp_ate_cmd_result(CMP_ATE_TYPE ate_type, CMP_ATE_CMD_TYPE cmd, uint8_t result, uint32_t data)
{
    uint8_t buff[8], len, msg_cmd;

    if (cmd == ATE_CMD_CHRG_START || cmd == ATE_CMD_CHRG_STOP)
    {
        buff[0] = result;
        len = 1;
        msg_cmd = 0x21;
    }
    else if (cmd == ATE_CMD_CARD_READ || cmd == ATE_CMD_CARD_CAL)
    {
        buff[0] = cmd == ATE_CMD_CARD_READ ? 0 : 1;
        buff[1] = result;
        memcpy(&buff[2], (uint8_t *)&data, 4);
        len = 6;
        msg_cmd = 0x22;
    }
    else if (cmd == ATE_CMD_REBOOT)
    {
        buff[0] = result;
        len = 1;
        msg_cmd = 0x34;
    }
    else if (cmd == ATE_CMD_PLUG_STA)
    {
        //        buff[0] = 0;
        //        cmp_get_plug_sta(0, &buff[1]);
        //        //	data >>= 8;
        memcpy(buff, (uint8_t *)&data, 2);
        len = 2;
        msg_cmd = 0x14;
    }
    else if (cmd == ATE_CMD_STOP_REASON)
    {
        memcpy(buff, (uint8_t *)&data, 3);
        len = 3;
        msg_cmd = 0x16;
    }
    else
    {
        return;
    }

    cmp_ate_pack_and_send_msg(&ate_conf[ate_type], msg_cmd, buff, len);
}

/***************************************************************************
*@Function    :
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2024-03-29
***************************************************************************/
static void cmp_ate_prot1_pack_and_send_msg(cmp_ate_stu *ate_dev, uint8_t cmd, uint8_t *data, uint16_t data_len)
{
    uint8_t pos = 0;
    uint8_t buff[128];
    uint8_t xor = 0;

    memset(buff, 0, sizeof(buff));

    const uint8_t *pole_sn;
    pole_sn = ate_conf->store->param->store_parameter_query(1);

    buff[pos++] = 'M';
    buff[pos++] = 'T';               // 帧头
    memcpy(&buff[pos], pole_sn, 28); // SN
    pos += 32;
    buff[pos++] = ate_dev->param.encry_type;     // 加密
    buff[pos++] = cmd;                           // 命令
    memcpy(&buff[pos], (uint8_t *)&data_len, 2); // 数据域长度
    pos += 2;
    memcpy(&buff[38], data, data_len); // 数据域
    pos += data_len;
    xor = xor_check(&buff[35], data_len + 3);
    buff[pos++] = xor;
    ate_dev->tx_msg_func(buff, pos);
}

/***************************************************************************
*@Function    :
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2024-03-29
***************************************************************************/
static void set_ageing_data_func(CMP_ATE_TYPE ate_type, uint8_t *ageing_data, uint16_t len)
{
    cmp_ate_prot1_pack_and_send_msg(&ate_conf[ate_type], 0x11, ageing_data, len);
}

/***************************************************************************************
 * @Function    : cmp_ate_dev_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/5
 ****************************************************************************************/
static void cmp_ate_dev_init(void)
{
    static cmp_ate_dev_stu ate_dev;

    ate_dev.set_rx_msg = cmp_ate_set_rx_msg;

    ate_dev.set_tx_hnd_func = cmp_ate_set_tx_hnd_msg;

    ate_dev.set_cmd_hook_func = cmp_ate_cmd_hook_func;

    ate_dev.set_cmd_result = cmp_ate_cmd_result;

    ate_dev.set_ageing_data = set_ageing_data_func;

    cmp_dev_append(CMP_DEV_NAME_ATE, &ate_dev);
}

/***************************************************************************************
 * @Function    : cmp_ate_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/5
 ****************************************************************************************/
static int cmp_ate_dev_create(void)
{
    cmp_ate_dev_conf_init();

    cmp_ate_dev_init();

    return 0;
}
INIT_APP_EXPORT(cmp_ate_dev_create);
