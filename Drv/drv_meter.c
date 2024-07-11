#include "drv_meter.h"


static drv_meter_stu meter_conf;

/***************************************************************************************
 * @Function    : drv_meter_set_type()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/18
 ****************************************************************************************/
static void drv_meter_set_type(DRV_METER_TYPE type, serial_tx_func_type tx_func)
{
    meter_conf.param.type = type;
    meter_conf.param.tx_func = tx_func;
}

/***************************************************************************************
 * @Function    : drv_meter_parse_single_volume()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2023/11/15
 ****************************************************************************************/
static void drv_meter_parse_band(uint8_t func_code)
{
    if (func_code == 0x83)
    {
        meter_conf.param.band = METER_CHNT;
    }
    else
    {
        meter_conf.param.band = METER_EASTRON;
    }
    meter_conf.procc.cmd_sta = METER_INPUT;
}

/***************************************************************************************
 * @Function    : drv_meter_chnt_parse_single_volume()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2023/11/15
 ****************************************************************************************/
static void drv_meter_chnt_parse_single_volume(uint8_t *data)
{
    meter_conf.procc.single_vol.volt_a = hex_to_float(&data[0]);
    meter_conf.procc.single_vol.power_a = hex_to_float(&data[8]);
    if (meter_conf.procc.single_vol.power_a > 0)
        meter_conf.procc.single_vol.curr_a = hex_to_float(&data[4]);
    else
        meter_conf.procc.single_vol.curr_a = 0 - hex_to_float(&data[4]);

    meter_conf.procc.single_vol.curr_have_a = 1000 * meter_conf.procc.single_vol.power_a / meter_conf.procc.single_vol.volt_a;
    meter_conf.procc.single_vol.curr_max = meter_conf.procc.single_vol.curr_a;
    meter_conf.procc.single_vol.curr_have_max = meter_conf.procc.single_vol.curr_a;
}

/***************************************************************************************
 * @Function    : drv_meter_chnt_parse_three_volume()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2023/11/15
 ****************************************************************************************/
static void drv_meter_chnt_parse_three_volume(uint8_t *data)
{
    meter_conf.procc.three_vol.volt_a = hex_to_float(&data[0]);
    meter_conf.procc.three_vol.volt_b = hex_to_float(&data[4]);
    meter_conf.procc.three_vol.volt_c = hex_to_float(&data[8]);
    meter_conf.procc.three_vol.power_total = hex_to_float(&data[24]);
    meter_conf.procc.three_vol.power_a = hex_to_float(&data[28]);
    meter_conf.procc.three_vol.power_b = hex_to_float(&data[32]);
    meter_conf.procc.three_vol.power_c = hex_to_float(&data[36]);
    if (meter_conf.procc.three_vol.power_a > 0)
        meter_conf.procc.three_vol.curr_a = hex_to_float(&data[12]);
    else
        meter_conf.procc.three_vol.curr_a = 0 - hex_to_float(&data[12]);

    if (meter_conf.procc.three_vol.power_b > 0)
        meter_conf.procc.three_vol.curr_b = hex_to_float(&data[16]);
    else
        meter_conf.procc.three_vol.curr_b = 0 - hex_to_float(&data[16]);

    if (meter_conf.procc.three_vol.power_c > 0)
        meter_conf.procc.three_vol.curr_c = hex_to_float(&data[20]);
    else
        meter_conf.procc.three_vol.curr_c = 0 - hex_to_float(&data[20]);

    meter_conf.procc.three_vol.curr_have_a = 1000 * meter_conf.procc.three_vol.power_a / meter_conf.procc.three_vol.volt_a;
    meter_conf.procc.three_vol.curr_have_b = 1000 * meter_conf.procc.three_vol.power_b / meter_conf.procc.three_vol.volt_b;
    meter_conf.procc.three_vol.curr_have_c = 1000 * meter_conf.procc.three_vol.power_c / meter_conf.procc.three_vol.volt_c;

    if (meter_conf.procc.three_vol.curr_a > meter_conf.procc.three_vol.curr_b)
        meter_conf.procc.three_vol.curr_max = meter_conf.procc.three_vol.curr_a;
    else
        meter_conf.procc.three_vol.curr_max = meter_conf.procc.three_vol.curr_b;

    if (meter_conf.procc.three_vol.curr_c > meter_conf.procc.three_vol.curr_max)
        meter_conf.procc.three_vol.curr_max = meter_conf.procc.three_vol.curr_c;

    if (meter_conf.procc.three_vol.curr_have_a > meter_conf.procc.three_vol.curr_have_b)
        meter_conf.procc.three_vol.curr_have_max = meter_conf.procc.three_vol.curr_have_a;
    else
        meter_conf.procc.three_vol.curr_have_max = meter_conf.procc.three_vol.curr_have_b;

    if (meter_conf.procc.three_vol.curr_have_c > meter_conf.procc.three_vol.curr_have_max)
        meter_conf.procc.three_vol.curr_have_max = meter_conf.procc.three_vol.curr_have_c;
}

/***************************************************************************************
 * @Function    : drv_meter_parse_single_volume()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2023/11/15
 ****************************************************************************************/
static void drv_meter_chnt_parse_input_volume(uint8_t *data)
{
    if (meter_conf.param.type == 0)
    {
        drv_meter_chnt_parse_single_volume(data);
    }
    else
    {
        drv_meter_chnt_parse_three_volume(data);
    }
}

/***************************************************************************************
 * @Function    : drv_meter_eastron_parse_single_volume()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2023/11/15
 ****************************************************************************************/
static void drv_meter_eastron_parse_single_volume(uint8_t *data)
{
    meter_conf.procc.single_vol.volt_a = hex_to_float(&data[0]);
    meter_conf.procc.single_vol.power_a = hex_to_float(&data[24]) / 1000;
    if (meter_conf.procc.single_vol.power_a > 0)
        meter_conf.procc.single_vol.curr_a = hex_to_float(&data[12]);
    else
        meter_conf.procc.single_vol.curr_a = 0 - hex_to_float(&data[12]);

    meter_conf.procc.single_vol.curr_have_a = 1000 * meter_conf.procc.single_vol.power_a / meter_conf.procc.single_vol.volt_a;
    meter_conf.procc.single_vol.curr_max = meter_conf.procc.single_vol.curr_a;
    meter_conf.procc.single_vol.curr_have_max = meter_conf.procc.single_vol.curr_have_a;
}

/***************************************************************************************
 * @Function    : drv_meter_eastron_parse_three_volume()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2023/11/15
 ****************************************************************************************/
static void drv_meter_eastron_parse_three_volume(uint8_t *data)
{
    meter_conf.procc.cmd_sta = METER_ETRON_POWER;
    meter_conf.procc.three_vol.volt_a = hex_to_float(&data[0]);
    meter_conf.procc.three_vol.volt_b = hex_to_float(&data[4]);
    meter_conf.procc.three_vol.volt_c = hex_to_float(&data[8]);
    meter_conf.procc.three_vol.power_a = hex_to_float(&data[24]) / 1000;
    meter_conf.procc.three_vol.power_b = hex_to_float(&data[28]) / 1000;
    meter_conf.procc.three_vol.power_c = hex_to_float(&data[32]) / 1000;
    if (meter_conf.procc.three_vol.power_a > 0)
        meter_conf.procc.three_vol.curr_a = hex_to_float(&data[12]);
    else
        meter_conf.procc.three_vol.curr_a = 0 - hex_to_float(&data[12]);

    if (meter_conf.procc.three_vol.power_b > 0)
        meter_conf.procc.three_vol.curr_b = hex_to_float(&data[16]);
    else
        meter_conf.procc.three_vol.curr_b = 0 - hex_to_float(&data[16]);

    if (meter_conf.procc.three_vol.power_c > 0)
        meter_conf.procc.three_vol.curr_c = hex_to_float(&data[20]);
    else
        meter_conf.procc.three_vol.curr_c = 0 - hex_to_float(&data[20]);

    meter_conf.procc.three_vol.curr_have_a = 1000 * meter_conf.procc.three_vol.power_a / meter_conf.procc.three_vol.volt_a;
    meter_conf.procc.three_vol.curr_have_b = 1000 * meter_conf.procc.three_vol.power_b / meter_conf.procc.three_vol.volt_b;
    meter_conf.procc.three_vol.curr_have_c = 1000 * meter_conf.procc.three_vol.power_c / meter_conf.procc.three_vol.volt_c;

    if (meter_conf.procc.three_vol.curr_a > meter_conf.procc.three_vol.curr_b)
        meter_conf.procc.three_vol.curr_max = meter_conf.procc.three_vol.curr_a;
    else
        meter_conf.procc.three_vol.curr_max = meter_conf.procc.three_vol.curr_b;

    if (meter_conf.procc.three_vol.curr_c > meter_conf.procc.three_vol.curr_max)
        meter_conf.procc.three_vol.curr_max = meter_conf.procc.three_vol.curr_c;

    if (meter_conf.procc.three_vol.curr_have_a > meter_conf.procc.three_vol.curr_have_b)
        meter_conf.procc.three_vol.curr_have_max = meter_conf.procc.three_vol.curr_have_a;
    else
        meter_conf.procc.three_vol.curr_have_max = meter_conf.procc.three_vol.curr_have_b;

    if (meter_conf.procc.three_vol.curr_have_c > meter_conf.procc.three_vol.curr_have_max)
        meter_conf.procc.three_vol.curr_have_max = meter_conf.procc.three_vol.curr_have_c;
}

/***************************************************************************************
 * @Function    : drv_meter_eastron_parse_three_volume_power_total()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2023/11/15
 ****************************************************************************************/
static void drv_meter_eastron_parse_three_volume_power_total(uint8_t *data)
{
    meter_conf.procc.cmd_sta = METER_INPUT;
    meter_conf.procc.three_vol.power_total = hex_to_float(&data[0]);
}

/***************************************************************************************
 * @Function    : drv_meter_eastron_parse_input_volume()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2023/11/15
 ****************************************************************************************/
static void drv_meter_eastron_parse_input_volume(uint8_t *data)
{
    if (meter_conf.param.type == 0)
    {
        drv_meter_eastron_parse_single_volume(data);
    }
    else
    {
        drv_meter_eastron_parse_three_volume(data);
    }
}

/***************************************************************************************
 * @Function    : drv_meter_parse_volume()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2023/11/16
 ****************************************************************************************/
static void drv_meter_parse_volume(uint8_t func_code, uint8_t *data, uint8_t len)
{
    meter_conf.procc.tx_cnt = 0;

    switch (meter_conf.procc.cmd_sta)
    {
    case METER_SN:
        drv_meter_parse_band(func_code);
        break;

    case METER_INPUT:
        if (meter_conf.param.band == METER_CHNT)
        {
            drv_meter_chnt_parse_input_volume(data);
        }
        else
        {
            drv_meter_eastron_parse_input_volume(data);
        }
        break;

    case METER_ETRON_POWER:
        drv_meter_eastron_parse_three_volume_power_total(data);
        break;
    }
}

/***************************************************************************************
 * @Function    : drv_meter_set_msg()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/18
 ****************************************************************************************/
static uint8_t drv_meter_set_msg(uint8_t *msg, uint8_t len)
{
    uint8_t msg_len;
    drv_meter_msg_rx_stu rx_msg;

    if (len > 4 + DRV_METER_RX_DATA_MAX_LEN)
    {
        len = 4 + DRV_METER_RX_DATA_MAX_LEN;
    }
    while (len >= 5)
    {
        memcpy((uint8_t *)&rx_msg, msg, len);
        if (rx_msg.addr != 0x01)
        {
            msg++;
            len--;
            continue;
        }
        if ((rx_msg.func_code != 0x03) && (rx_msg.func_code != 0x83) && (rx_msg.func_code != 0x04))
        {
            msg += 2;
            ;
            len -= 2;
            continue;
        }
        msg_len = rx_msg.len;
        if (rx_msg.func_code != 0x83)
        {
            if ((msg_len + 5) > len)
                return 0;
        }
        drv_meter_parse_volume(rx_msg.func_code, rx_msg.data, msg_len);
        meter_conf.procc.succ_flag = 1;
        return 1;
    }
    return 0;
}

/***************************************************************************************
 * @Function    : drv_meter_gen_modubus_msg()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2023/11/16
 ****************************************************************************************/
static void drv_meter_gen_modubus_msg(drv_meter_modbus_tx_stu *modbus_msg, uint8_t func_code, uint16_t reg_addr, uint8_t reg_cnt)
{
    uint16_t crc;

    modbus_msg->addr = 0x01;
    modbus_msg->code = func_code;
    modbus_msg->start_reg[0] = reg_addr >> 8;
    modbus_msg->start_reg[1] = reg_addr;

    modbus_msg->reg_cnt[0] = 0x00;
    modbus_msg->reg_cnt[1] = reg_cnt;
    crc = crc16_calc((uint8_t *)modbus_msg, 6);
    modbus_msg->crc[0] = crc;
    modbus_msg->crc[1] = crc >> 8;
}

/***************************************************************************************
 * @Function    : drv_meter_chnt_gen_modubus_msg()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2023/11/16
 ****************************************************************************************/
static void drv_meter_chnt_gen_modubus_msg(drv_meter_modbus_tx_stu *modbus)
{
    if (meter_conf.procc.cmd_sta == METER_SN)
    {
        return;
    }

    if (meter_conf.param.type == 0)
    {
        drv_meter_gen_modubus_msg(modbus, 0x03, 0x2000, 0x06);
    }
    else
    {
        drv_meter_gen_modubus_msg(modbus, 0x03, 0x1510, 0x14);
    }
}

/***************************************************************************************
 * @Function    : drv_meter_eastron_gen_modubus_msg()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2023/11/16
 ****************************************************************************************/
static void drv_meter_eastron_gen_modubus_msg(drv_meter_modbus_tx_stu *modbus)
{
    if (meter_conf.procc.cmd_sta == METER_SN)
    {
        return;
    }

    switch (meter_conf.procc.cmd_sta)
    {
    case METER_INPUT:
        if (meter_conf.param.type == 0)
        {
            drv_meter_gen_modubus_msg(modbus, 0x04, 0x0000, 0x0E);
        }
        else
        {
            drv_meter_gen_modubus_msg(modbus, 0x04, 0x0000, 0x12);
        }
        break;

    case METER_ETRON_POWER:
        drv_meter_gen_modubus_msg(modbus, 0x04, 0x0034, 0x02);
        break;
    }
}

/***************************************************************************************
 * @Function    : drv_meter_get_volume()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/18
 ****************************************************************************************/
static int32_t drv_meter_get_volume(DRV_METER_VOLUME_TYPE volume)
{
    if (meter_conf.param.type == 0)
    {
        switch (volume)
        {
        case METER_VOLUME_VOLT_A:
            return (int32_t)(meter_conf.procc.single_vol.volt_a * 100);
        case METER_VOLUME_CURR_A:
            return (int32_t)(meter_conf.procc.single_vol.curr_a * 100);
        case METER_VOLUME_CURR_MAX:
            return (int32_t)(meter_conf.procc.single_vol.curr_max * 100);
        case METER_VOLUME_CURR_HAVE_MAX:
            return (int32_t)(meter_conf.procc.single_vol.curr_have_max * 100);
        case METER_VOLUME_POWER_TOTAL:
            return (int32_t)(meter_conf.procc.single_vol.power_a * 100);
        }
    }
    else
    {
        switch (volume)
        {
        case METER_VOLUME_VOLT_A:
            return (int32_t)(meter_conf.procc.three_vol.volt_a * 100);
        case METER_VOLUME_VOLT_B:
            return (int32_t)(meter_conf.procc.three_vol.volt_b * 100);
        case METER_VOLUME_VOLT_C:
            return (int32_t)(meter_conf.procc.three_vol.volt_c * 100);
        case METER_VOLUME_CURR_A:
            return (int32_t)(meter_conf.procc.three_vol.curr_a * 100);
        case METER_VOLUME_CURR_B:
            return (int32_t)(meter_conf.procc.three_vol.curr_b * 100);
        case METER_VOLUME_CURR_C:
            return (int32_t)(meter_conf.procc.three_vol.curr_c * 100);
        case METER_VOLUME_CURR_MAX:
            return (int32_t)(meter_conf.procc.three_vol.curr_max * 100);
        case METER_VOLUME_CURR_HAVE_MAX:
            return (int32_t)(meter_conf.procc.three_vol.curr_have_max * 100);
        case METER_VOLUME_POWER_TOTAL:
            return (int32_t)(meter_conf.procc.three_vol.power_total * 100);
        }
    }
    return 0;
}

/***************************************************************************************
 * @Function    : drv_meter_get_state()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/18
 ****************************************************************************************/
static uint8_t drv_meter_get_state(void)
{
    return meter_conf.param.state;
}

/***************************************************************************
*@Function    :drv_set_meter_enable_func
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-12-08
***************************************************************************/
static void drv_set_meter_enable_func(uint8_t enable_flag)
{
    meter_conf.procc.enable_flag = enable_flag;
}

/***************************************************************************
*@Function    :drv_get_meter_succ_flag_func
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :RenGX            Version:  V3.0.0             Date:2024-1-23
***************************************************************************/
static uint8_t drv_get_meter_succ_flag_func(void)
{
    return meter_conf.procc.succ_flag;
}

/***************************************************************************************
 * @Function    : drv_meter_tx_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/18
 ****************************************************************************************/
static void drv_meter_tx_work(uint8_t period)
{
    drv_meter_modbus_tx_stu modbus;
    static uint8_t pre_cmd_sta = 0xFF;

    if (meter_conf.procc.tx_count < DRV_METER_CYC_WORK_PEIOD)
    {
        meter_conf.procc.tx_count += period;
        return;
    }
    meter_conf.procc.tx_count = 0;
    if (meter_conf.procc.tx_cnt < 5)
    {
        meter_conf.procc.tx_cnt++;
        meter_conf.param.state = 0;
    }
    else
    {
        meter_conf.param.state = 1;
    }

    switch (meter_conf.param.band)
    {
    case METER_NONE:
        drv_meter_gen_modubus_msg(&modbus, 0x03, 0xFC00, 0x02);
        break;

    case METER_CHNT:
        drv_meter_chnt_gen_modubus_msg(&modbus);
        break;

    case METER_EASTRON:
        drv_meter_eastron_gen_modubus_msg(&modbus);
        break;
    }

    if (pre_cmd_sta != meter_conf.procc.cmd_sta)
    {
        pre_cmd_sta = meter_conf.procc.cmd_sta;
        meter_conf.param.tx_data.tx_len = sizeof(modbus);
        memcpy(meter_conf.param.tx_data.tx_buff, (uint8_t *)&modbus, meter_conf.param.tx_data.tx_len);
    }
    meter_conf.param.tx_func(meter_conf.param.tx_data.tx_buff, meter_conf.param.tx_data.tx_len);
}

/***************************************************************************
*@Function    :
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2024-04-02
***************************************************************************/
static void drv_meter_volume_conv()
{
    if (meter_conf.param.type == 0)
    {
        meter_conf.procc.volume.volt_a = (uint16_t)(meter_conf.procc.single_vol.volt_a * 10);
        meter_conf.procc.volume.curr_a = (uint32_t)(meter_conf.procc.single_vol.curr_a * 1000);
        meter_conf.procc.volume.power = (uint32_t)(meter_conf.procc.single_vol.power_a * 10);
    }
    else
    {
        meter_conf.procc.volume.volt_a = (uint16_t)(meter_conf.procc.three_vol.volt_a * 10);
        meter_conf.procc.volume.volt_b = (uint16_t)(meter_conf.procc.three_vol.volt_b * 10);
        meter_conf.procc.volume.volt_c = (uint16_t)(meter_conf.procc.three_vol.volt_c * 10);
        meter_conf.procc.volume.curr_a = (uint32_t)(meter_conf.procc.three_vol.curr_a * 1000);
        meter_conf.procc.volume.curr_b = (uint32_t)(meter_conf.procc.three_vol.curr_b * 1000);
        meter_conf.procc.volume.curr_c = (uint32_t)(meter_conf.procc.three_vol.curr_c * 1000);
        meter_conf.procc.volume.power = (uint32_t)(meter_conf.procc.three_vol.power_total * 100);
        meter_conf.procc.volume.energy = 0;
    }
}

/***************************************************************************************
 * @Function    : drv_meter_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/18
 ****************************************************************************************/
static void drv_meter_work(uint8_t period)
{
    if (meter_conf.procc.enable_flag != 0xAA)
    {
        meter_conf.procc.succ_flag = 0;
        return;
    }
    drv_meter_volume_conv();
    drv_meter_tx_work(period);
}

/***************************************************************************************
 * @Function    : drv_meter_param_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/18
 ****************************************************************************************/
static void drv_meter_param_init(void)
{
    drv_meter_modbus_tx_stu modbus;

    drv_meter_gen_modubus_msg(&modbus, 0x03, 0xFC00, 0x02);
    meter_conf.procc.tx_circle = 0;
    meter_conf.procc.cmd_sta = METER_SN;
    meter_conf.param.tx_data.tx_len = sizeof(modbus);
    memcpy(meter_conf.param.tx_data.tx_buff, (uint8_t *)&modbus, meter_conf.param.tx_data.tx_len);
}

/***************************************************************************
*@Function    :drv_meter_oper_init
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-11-24
***************************************************************************/
static void drv_meter_oper_init(void)
{
    bsp_bind_oper_data_dev(0xff, 1, 0, (uint8_t *)&meter_conf.param.state, 1);
    bsp_bind_oper_data_dev(0xff, 1, 1, (uint8_t *)&meter_conf.procc.volume.energy, 4);
    bsp_bind_oper_data_dev(0xff, 1, 2, (uint8_t *)&meter_conf.procc.volume.power, 4);
    bsp_bind_oper_data_dev(0xff, 1, 3, (uint8_t *)&meter_conf.procc.volume.volt_a, 2);
    bsp_bind_oper_data_dev(0xff, 1, 4, (uint8_t *)&meter_conf.procc.volume.volt_b, 2);
    bsp_bind_oper_data_dev(0xff, 1, 5, (uint8_t *)&meter_conf.procc.volume.volt_c, 2);
    bsp_bind_oper_data_dev(0xff, 1, 6, (uint8_t *)&meter_conf.procc.volume.curr_a, 4);
    bsp_bind_oper_data_dev(0xff, 1, 7, (uint8_t *)&meter_conf.procc.volume.curr_b, 4);
    bsp_bind_oper_data_dev(0xff, 1, 8, (uint8_t *)&meter_conf.procc.volume.curr_c, 4);
}

/***************************************************************************************
 * @Function    : drv_meter_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/18
 ****************************************************************************************/
static int drv_meter_dev_create(void)
{
    static drv_meter_dev_stu chint_meter;

    drv_meter_param_init();

    chint_meter.drv_set_meter_type = drv_meter_set_type;
    chint_meter.drv_set_meter_msg = drv_meter_set_msg;
    chint_meter.drv_get_meter_volume = drv_meter_get_volume;
    chint_meter.drv_get_meter_state = drv_meter_get_state;
    chint_meter.drv_set_meter_enable = drv_set_meter_enable_func;
    chint_meter.drv_get_meter_succ_flag = drv_get_meter_succ_flag_func;
    chint_meter.drv_meter_work = drv_meter_work;

    drv_meter_oper_init();

    drv_dev_append(DRV_DEV_METER_CHINT, &chint_meter);
    return 0;
}
INIT_COMPONENT_EXPORT(drv_meter_dev_create);

