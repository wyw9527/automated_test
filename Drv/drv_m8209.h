#ifndef _DRV_M8209_H_
#define _DRV_M8209_H_


#include "drv.h"

#define SYSCON_ADDR    0x00
#define EMUCON_ADDR    0x01
#define DEVICE_ID_ADDR 0x7f
#define SPEC_CMD_ADDR  0xEA

#define SysStatus 0x43
#define RData 0x44
#define WData 0x45
#define DeviceID 0x7f

#define SpecialCommand 0xEA
#define WriteEN 0xE5
#define WriteProtect 0xDC
#define SelectA 0x5A
#define SelectB 0xA5
#define ResetEmu 0xFA

#define SYSCON 0x00
#define EMUCON 0x01


#define HFConst 0x02
#define PStart 0x03
#define QStart 0x04
#define GPQA 0x05
#define GPQB 0x06
#define PhsA 0x07
#define PhsB 0x08
#define QPhsCal 0x09
#define APOSA 0x0a
#define APOSB 0x0b
#define RPOSA 0x0c
#define RPOSB 0x0d
#define IARMSOS 0x0e
#define IBRMSOS 0x0f
#define IBGain 0x10

#define PFCnt 0x20
#define QFCnt 0x21
#define IARMS 0x22
#define IBRMS 0x23
#define URMS 0x24
#define UFreq 0x25
#define PowerPA 0x26
#define EnergyP 0x29


#define SPEC_CMD_DATA_RESET 0xFA
#define SPEC_CMD_DATA_WR_EN 0xE5
#define SPEC_CMD_DATA_SEL_A 0x5A
#define SPEC_CMD_DATA_WR_PROTECT 0xDC


#define DRV_M8209_RX_BUFF_SIZE 32

typedef enum
{
    M8209_VOLU_VOLT,
    M8209_VOLU_CURR,
    M8209_VOLU_POWER,
    M8209_VOLU_FREQ,
    M8209_VOLU_ENERGY,
}DRV_M8209_VOLU_TYPE;

typedef struct
{
    float Kp;
    float Ku;
    float Ki;
    float Kep;
}drv_m8208_param_stu;

typedef struct
{
    uint8_t rx_flag;
    uint8_t rx_len;
    uint8_t rx_buff[DRV_M8209_RX_BUFF_SIZE];
    int32_t rx_val;
}drv_m8209_rx_stu;

typedef struct
{
    uint8_t tx_flag;
    uint8_t tx_tmr;
    serial_tx_func_type tx_func;
}drv_m8209_tx_stu;

typedef struct
{
    uint8_t state;
    uint8_t measure_step;
}drv_m8208_procc_stu;

typedef struct
{
    uint16_t volt; // 0.1
    uint16_t curr;
    uint16_t power;
    uint16_t freq;
    uint32_t energy;//0.01
}drv_m8209_measure_stu;

typedef struct
{
    drv_m8208_param_stu param;
    
    drv_m8209_rx_stu rx;
    
    drv_m8209_tx_stu tx;
    
    drv_m8208_procc_stu procc;
    
    drv_m8209_measure_stu measure;
}drv_m8209_stu;

typedef struct
{
    void (*drv_set_msg)(uint8_t* msg,uint8_t len);
    
    void (*drv_set_send_func)(serial_tx_func_type tx_func);
    
    uint32_t (*drv_get_volume)(DRV_M8209_VOLU_TYPE type);
    
    uint8_t (*drv_get_state)();
    
}drv_m8209_dev_stu;



























#endif