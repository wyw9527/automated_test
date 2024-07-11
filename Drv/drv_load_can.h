#ifndef _DRV_LOAD_CAN_H_
#define _DRV_LOAD_CAN_H_

#include "stdio.h"
#include "drv.h"
typedef struct
{
    uint8_t S1_switch;
    uint8_t CP_grounding;
    uint8_t PE_switch;
    uint8_t S2_switch;
    uint8_t S3_switch;
    uint8_t reserve1;
    uint8_t reserve2;
    uint8_t reserve3;
    uint8_t CP_switch;
    uint8_t ABC_switch;
    uint8_t N_switch;
    uint8_t emergency_stop;

} cmd_61_save_data_stu;

typedef struct
{
    uint16_t R2_value;
    uint16_t R3_value;
    uint16_t R4_value;
    uint16_t RC_value;

} cmd_62_save_data_stu;

typedef struct
{
    uint8_t PWM_duty_cycle;
    uint16_t PWM_frequency;
    uint16_t PWM_amplitude;

} cmd_63_save_data_stu;

typedef struct
{
    uint8_t R_meter_value;

} cmd_64_save_data_stu;

typedef struct
{
    uint16_t Va_value;
    uint16_t Vb_value;
    uint16_t Vc_value;

} cmd_65_save_data_stu;

typedef struct
{
    uint16_t Vab_value;
    uint16_t Vbc_value;
    uint16_t Vac_value;

} cmd_66_save_data_stu;

typedef struct
{
    uint16_t Ia_value;
    uint16_t Ib_value;
    uint16_t Ic_value;

} cmd_67_save_data_stu;


typedef struct 
{
    cmd_61_save_data_stu cmd_61;
    cmd_62_save_data_stu cmd_62;
    cmd_63_save_data_stu cmd_63;
    cmd_64_save_data_stu cmd_64;
    cmd_65_save_data_stu cmd_65;
    cmd_66_save_data_stu cmd_66;
    cmd_67_save_data_stu cmd_67;

} drv_load_can_procc_stu;


typedef struct
{
    uint16_t comm_flag;
    uint16_t count;

}drv_load_comm_status_stu;

typedef struct
{
    bsp_can_dev_stu *can;   /* can设备 */
    drv_load_can_procc_stu procc;
    drv_load_comm_status_stu comm;
} drv_load_can_pub_stu;


typedef enum
{
    CAN_SWITCH_SIGNAL,
    CAN_RESISTANCE_VALUE,
    CAN_PWM_VALUE,
    CAN_R_METER_VALUE,
    CAN_P_VOLTAGE_VALUE,
    CAN_L_VOLTAGE_VALUE,
    CAN_CURRENT_VALUE,


}DRV_LOAD_CAN_VAL_TYPE;

typedef struct
{
    void (*drv_load_can_work)(uint8_t period);

    uint8_t* (*drv_load_can_get_reg_val)(DRV_LOAD_CAN_VAL_TYPE type);

    void (*drv_load_can_all_switch_on)();

    void (*drv_load_can_all_switch_off)();

    void (*drv_load_can_CP_off)();

    void (*drv_load_can_CP_grounding)();

    void (*drv_load_can_PE_off)();

    void (*drv_load_can_S2_off)();

    void (*drv_load_can_ABC_off)();

    void (*drv_load_can_N_off)();    

    void (*drv_load_can_set_R2)(uint16_t value);

    void (*drv_load_can_set_R3)(uint16_t value);
    
    void (*drv_load_can_set_R4)(uint16_t value);
    
    void (*drv_load_can_set_RC)(uint16_t value);

    void (*drv_load_can_set_PWM)(uint16_t value);

} drv_load_can_dev_stu;

#endif