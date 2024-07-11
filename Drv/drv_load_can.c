#include "drv_load_can.h"
#include "SEGGER_RTT.h"


static drv_load_can_pub_stu load_can_pub;
// static drv_can_stu serial_conf[DEV_SERIAL_NUM];


static void load_printf(uint8_t *data, uint16_t len)
{
    uint16_t i;
    for(i=0;i<len;i++)
    {
        Fox_ESS_Printf("%02x", data[i]);
    }
    Fox_ESS_Printf("\r\n");
}


/***************************************************************************************
 * @Function    : can_param_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : SSY            Version   : V2.0.0                Date:2024/6/17
 ****************************************************************************************/
static void can_param_init()
{
    load_can_pub.can->bsp_can_init();
}

/***************************************************************************************
 * @Function    : can_get_msg()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : SSY            Version   : V2.0.0                Date:2024/6/17
 ****************************************************************************************/
// static uint8_t can_get_msg(can_receive_message_struct* rxbuff)
// {
//     return load_can_pub.can->bsp_can_get_msg(rxbuff);
// }

/***************************************************************************************
 * @Function    : drv_load_set_R2()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : SSY            Version   : V2.0.0                Date:2024/6/17
 ****************************************************************************************/
// static void drv_load_set_R2(uint8_t *value)
// {
//     can_trasnmit_message_struct tx_buff;
    
//     tx_buff.tx_sfid = 0;
//     tx_buff.tx_efid = 0x03050E62;
//     tx_buff.tx_ff = CAN_FF_EXTENDED;
//     tx_buff.tx_ft = CAN_FT_DATA;
//     tx_buff.tx_dlen = 0x08;
//     memcpy(tx_buff.tx_data, value, sizeof(tx_buff.tx_data));
    
//     Fox_ESS_Printf("can_tx:");
//     load_printf(tx_buff.tx_data, 8);

//     load_can_pub.can->bsp_can_tx_msg(tx_buff);
// }


/***************************************************************************
* FuncName     : can_switch_status_handle()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : SSY         Version:    V1.0      Date:    2024/6/19
***************************************************************************/
static void can_switch_status_handle(uint8_t * data)
{
    uint16_t combined_data = (data[1] << 8) | data[0]; 

    load_can_pub.procc.cmd_61.S1_switch = (combined_data >> 0) & 1;
    load_can_pub.procc.cmd_61.CP_grounding = (combined_data >> 1) & 1;
    load_can_pub.procc.cmd_61.PE_switch = (combined_data >> 2) & 1;
    load_can_pub.procc.cmd_61.S2_switch = (combined_data >> 3) & 1;
    load_can_pub.procc.cmd_61.S3_switch = (combined_data >> 4) & 1;
    load_can_pub.procc.cmd_61.reserve1 = (combined_data >> 5) & 1;
    load_can_pub.procc.cmd_61.reserve2 = (combined_data >> 6) & 1;
    load_can_pub.procc.cmd_61.reserve3 = (combined_data >> 7) & 1;
    load_can_pub.procc.cmd_61.CP_switch = (combined_data >> 8) & 1;
    load_can_pub.procc.cmd_61.ABC_switch = (combined_data >> 9) & 1;
    load_can_pub.procc.cmd_61.N_switch = (combined_data >> 10) & 1;
    load_can_pub.procc.cmd_61.emergency_stop = (data[2] >> 0) & 1;
    
}

/***************************************************************************
* FuncName     : can_resistance_value_handle()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : SSY         Version:    V1.0      Date:    2024/6/19
***************************************************************************/
static void can_resistance_value_handle(uint8_t * data)
{
    load_can_pub.procc.cmd_62.R2_value = (data[0] << 8) | data[1];
    load_can_pub.procc.cmd_62.R3_value = (data[2] << 8) | data[3];
    load_can_pub.procc.cmd_62.R4_value = (data[4] << 8) | data[5];
    load_can_pub.procc.cmd_62.RC_value = (data[6] << 8) | data[7];
    
}

/***************************************************************************
* FuncName     : can_PWM_value_handle()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : SSY         Version:    V1.0      Date:    2024/6/19
***************************************************************************/
static void can_PWM_value_handle(uint8_t * data)
{
    load_can_pub.procc.cmd_63.PWM_duty_cycle = (data[0] << 8) | data[1];
    load_can_pub.procc.cmd_63.PWM_frequency = (data[2] << 8) | data[3];
    load_can_pub.procc.cmd_63.PWM_amplitude = (data[4] << 8) | data[5];
    
}

/***************************************************************************
* FuncName     : can_resistance_meter_handle()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : SSY         Version:    V1.0      Date:    2024/6/19
***************************************************************************/
static void can_resistance_meter_handle(uint8_t * data)
{
    load_can_pub.procc.cmd_64.R_meter_value = (data[0] << 8) | data[1];
    
}

/***************************************************************************
* FuncName     : can_three_phase_voltage_handle()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : SSY         Version:    V1.0      Date:    2024/6/19
***************************************************************************/
static void can_three_phase_voltage_handle(uint8_t * data)
{
    load_can_pub.procc.cmd_65.Va_value = ((data[0] << 8) | data[1])*260*1/10000;
    load_can_pub.procc.cmd_65.Vb_value = ((data[2] << 8) | data[3])*260*1/10000;
    load_can_pub.procc.cmd_65.Vc_value = ((data[4] << 8) | data[5])*260*1/10000;
    
}

/***************************************************************************
* FuncName     : can_three_line_voltage_handle()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : SSY         Version:    V1.0      Date:    2024/6/19
***************************************************************************/
static void can_three_line_voltage_handle(uint8_t * data)
{
    load_can_pub.procc.cmd_66.Vab_value = ((data[0] << 8) | data[1])*260*1/10000;
    load_can_pub.procc.cmd_66.Vbc_value = ((data[2] << 8) | data[3])*260*1/10000;
    load_can_pub.procc.cmd_66.Vac_value = ((data[4] << 8) | data[5])*260*1/10000;
    
}

/***************************************************************************
* FuncName     : can_three_phase_current_handle()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : SSY         Version:    V1.0      Date:    2024/6/19
***************************************************************************/
static void can_three_phase_current_handle(uint8_t * data)
{
    load_can_pub.procc.cmd_67.Ia_value = ((data[0] << 8) | data[1])*80*1/10000;
    load_can_pub.procc.cmd_67.Ib_value = ((data[2] << 8) | data[3])*80*1/10000;
    load_can_pub.procc.cmd_67.Ic_value = ((data[4] << 8) | data[5])*80*1/10000;
    
}

static void drv_load_rx_buff_msg_frame(uint8_t period)
{
    can_receive_message_struct *rx_msg;

    while((rx_msg = load_can_pub.can->bsp_can_get_msg()) != NULL)
    {
        if ((rx_msg->rx_efid & 0xFFFFFF00) == 0x03050E00 && (rx_msg->rx_efid & 0x000000FF) != 0) 
        { 
            Fox_ESS_Printf("can_tx:");
            load_printf(rx_msg->rx_data, 8);

            switch (rx_msg->rx_efid & 0x000000FF) {
                case 0x61:
                    can_switch_status_handle(rx_msg->rx_data); break;
                case 0x62:
                    can_resistance_value_handle(rx_msg->rx_data); break;
                case 0x63:
                    can_PWM_value_handle(rx_msg->rx_data); break;    
                case 0x64:
                    can_resistance_meter_handle(rx_msg->rx_data); break;
                case 0x65:
                    can_three_phase_voltage_handle(rx_msg->rx_data); break;
                case 0x66:
                    can_three_line_voltage_handle(rx_msg->rx_data); break;
                case 0x67:
                    can_three_phase_current_handle(rx_msg->rx_data); break;
            }
        }
        load_can_pub.comm.count = 0;
    }

}

static void drv_load_state_check(uint8_t period)
{
    
    if(load_can_pub.comm.count <= 5000)
    {
        load_can_pub.comm.count += period;
    }
    load_can_pub.comm.comm_flag = load_can_pub.comm.count>5000?1:0;

}
static void drv_load_can_work(uint8_t period)
{
    drv_load_rx_buff_msg_frame(period);
    drv_load_state_check(period);
}

static uint8_t* drv_load_can_get_reg_val(DRV_LOAD_CAN_VAL_TYPE type)
{
    switch(type)
    {
        case CAN_SWITCH_SIGNAL:return (uint8_t*)&load_can_pub.procc.cmd_61;
        case CAN_RESISTANCE_VALUE:return (uint8_t*)&load_can_pub.procc.cmd_62;
        case CAN_PWM_VALUE:return (uint8_t*)&load_can_pub.procc.cmd_63;
        case CAN_R_METER_VALUE:return (uint8_t*)&load_can_pub.procc.cmd_64;
        case CAN_P_VOLTAGE_VALUE:return (uint8_t*)&load_can_pub.procc.cmd_65;
        case CAN_L_VOLTAGE_VALUE:return (uint8_t*)&load_can_pub.procc.cmd_66;
        case CAN_CURRENT_VALUE:return (uint8_t*)&load_can_pub.procc.cmd_67;
    }
}

static void drv_load_can_pack_func(uint32_t frame_id, uint8_t* frame_data)
{
    can_trasnmit_message_struct tx_buff;
    
    tx_buff.tx_sfid = 0;
    tx_buff.tx_efid = frame_id;
    tx_buff.tx_ff = CAN_FF_EXTENDED;
    tx_buff.tx_ft = CAN_FT_DATA;
    tx_buff.tx_dlen = 0x08;
    memcpy(tx_buff.tx_data, frame_data, 8);
    
    Fox_ESS_Printf("can_tx:");
    load_printf(tx_buff.tx_data, 8);

    load_can_pub.can->bsp_can_tx_msg(tx_buff);
}

static void drv_load_can_all_switch_on()
{
    uint32_t frame_id = 0x000E0580;
    uint8_t frame_data[8] = {0x0F, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    drv_load_can_pack_func(frame_id,frame_data);
}
static void drv_load_can_all_switch_off()
{
    uint32_t frame_id = 0x000E0580;
    uint8_t frame_data[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    drv_load_can_pack_func(frame_id,frame_data);
}

static void drv_load_can_CP_off()
{
    uint32_t frame_id = 0x000E0580;
    uint8_t frame_data[8] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09};
    drv_load_can_pack_func(frame_id,frame_data);
}

static void drv_load_can_CP_grounding()
{
    uint32_t frame_id = 0x000E0580;
    uint8_t frame_data[8] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
    drv_load_can_pack_func(frame_id,frame_data);
}
static void drv_load_can_PE_off()
{
    uint32_t frame_id = 0x000E0580;
    uint8_t frame_data[8] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03};
    drv_load_can_pack_func(frame_id,frame_data);
}
static void drv_load_can_S2_off()
{
    uint32_t frame_id = 0x000E0580;
    uint8_t frame_data[8] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04};
    drv_load_can_pack_func(frame_id,frame_data);
}
static void drv_load_can_ABC_off()
{
    uint32_t frame_id = 0x000E0580;
    uint8_t frame_data[8] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A};
    drv_load_can_pack_func(frame_id,frame_data);
}
static void drv_load_can_N_off()
{
    uint32_t frame_id = 0x000E0580;
    uint8_t frame_data[8] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0B};
    drv_load_can_pack_func(frame_id,frame_data);
}
static void drv_load_can_set_R2(uint16_t R2_value)
{
    uint32_t frame_id = 0x000E0500;
    uint8_t frame_data[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    sort_high_byte_first(frame_data, R2_value);
    drv_load_can_pack_func(frame_id,frame_data);
}
static void drv_load_can_set_R3(uint16_t R3_value)
{
    uint32_t frame_id = 0x000E0510;
    uint8_t frame_data[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    sort_high_byte_first(frame_data, R3_value);
    drv_load_can_pack_func(frame_id,frame_data);
}
static void drv_load_can_set_R4(uint16_t R4_value)
{
    uint32_t frame_id = 0x000E0520;
    uint8_t frame_data[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    sort_high_byte_first(frame_data, R4_value);
    drv_load_can_pack_func(frame_id,frame_data);
}
static void drv_load_can_set_RC(uint16_t RC_value)
{
    uint32_t frame_id = 0x000E0530;
    uint8_t frame_data[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    sort_high_byte_first(frame_data, RC_value);
    drv_load_can_pack_func(frame_id,frame_data);
}
static void drv_load_can_set_PWM(uint16_t PWM_value)
{
    uint32_t frame_id = 0x000E0540;
    uint8_t frame_data[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    sort_high_byte_first(frame_data, PWM_value);
    drv_load_can_pack_func(frame_id,frame_data);
}

/***************************************************************************************
 * @Function    : drv_serial_serv_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/9
 ****************************************************************************************/
static void drv_can_serv_create(void)
{
    static drv_load_can_dev_stu load_can;

    load_can.drv_load_can_work = drv_load_can_work;
    load_can.drv_load_can_get_reg_val = drv_load_can_get_reg_val;
    load_can.drv_load_can_all_switch_on = drv_load_can_all_switch_on;
    load_can.drv_load_can_all_switch_off = drv_load_can_all_switch_off;
    load_can.drv_load_can_CP_off = drv_load_can_CP_off;
    load_can.drv_load_can_CP_grounding = drv_load_can_CP_grounding;
    load_can.drv_load_can_PE_off = drv_load_can_PE_off;
    load_can.drv_load_can_S2_off = drv_load_can_S2_off;
    load_can.drv_load_can_ABC_off = drv_load_can_ABC_off;
    load_can.drv_load_can_N_off = drv_load_can_N_off;
    load_can.drv_load_can_set_R2 = drv_load_can_set_R2;
    load_can.drv_load_can_set_R3 = drv_load_can_set_R3;
    load_can.drv_load_can_set_R4 = drv_load_can_set_R4;
    load_can.drv_load_can_set_RC = drv_load_can_set_RC;
    load_can.drv_load_can_set_PWM = drv_load_can_set_PWM;

    drv_dev_append(DRV_DEV_LOAD_CAN, &load_can);
}

/***************************************************************************************
 * @Function    : drv_can_pub_dev_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : SSY            Version   : V3.0.0                Date:2024/6/17
 ****************************************************************************************/
static void drv_can_pub_dev_init(void)
{
    load_can_pub.can = bsp_dev_find(BSP_DEV_NAME_CAN);
    if (NULL == load_can_pub.can)
    {
        return;
    }
    load_can_pub.can->bsp_can_init();
}

/***************************************************************************************
 * @Function    : drv_serial_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/8
 ****************************************************************************************/
int drv_can_dev_create(void)
{
    drv_can_pub_dev_init();

    drv_can_serv_create();

    return 0;
}

INIT_COMPONENT_EXPORT(drv_can_dev_create);

