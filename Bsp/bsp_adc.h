#ifndef _BSP_ADC_H_
#define _BSP_ADC_H_

#include "bsp_io.h"
#include "rtthread.h"
#include "math.h"

#define ADC_VOLT_SAMP_CNT 480
#define ADC_CP_SAMP_CNT 64
#define ADC_CURR_SAMP_CNT 480
#define ADC_TEMP_SAMP_CNT 256
#define ADC_PE_SAMP_CNT 512
#define ADC_LEAK_SAMP_CNT 160
#define ADC_CC_SAMP_CNT 10

#define AC_WAVE_ADC_LOW -15
#define AC_WAVE_ADC_HIGH 15

#define NTC_TBL_LEN 166
#define NTC_TEMP_LO -40
#define NTC_TEMP_HI 125
#define NTC_TBL_SCALE 10.0

#define ADC_LEAK_OFFSET 2000 // leak alarm threshold offset 10 pos

typedef enum
{
    ADC_CONV_VOLT1 = 0,
    ADC_CONV_VOLT2,
    ADC_CONV_VOLT3,
    ADC_CONV_CURR1,
    ADC_CONV_CURR2,
    ADC_CONV_CURR3,
    ADC_CONV_CP,
    ADC_CONV_TEMP_BOARD,
    ADC_CONV_TEMP_PLUG,
    ADC_CONV_TEMP_PLUG2,
    ADC_CONV_PE,
    ADC_CONV_LEAK,
    ADC_CONV_CC,
    ADC_CONV_NUM
} ADC_CONV_TYPE;

typedef enum
{
    ADC_VOLT1 = 0,
    ADC_VOLT2,
    ADC_VOLT3,
    ADC_CURR1,
    ADC_CURR2,
    ADC_CURR3,
    ADC_FREQ1,
    ADC_FREQ2,
    ADC_FREQ3,
    ADC_CP,
    ADC_TEMP_BOARD,
    ADC_TEMP_PLUG,
    ADC_TEMP_PLUG2,
    ADC_PE,
    ADC_LEAK,
    ADC_CC,
    ADC_CP_STA,
    ADC_NUM
} ADC_TYPE;

typedef struct
{
    uint16_t first_idx;
    uint16_t last_idx;
} waveform_stu;

typedef struct
{
    uint16_t point_h;
    uint16_t value_h;
    uint16_t point_low;
    uint16_t value_low;
} adc_ac_wave_stu;

/* bsp head file include */
#include "bsp_adc.h"
#include "bsp_uart.h"
// #include "../Comm/comm_math.h"

/* app head file include */
// #include "app_init.h"

typedef struct
{
    uint8_t conv_flag;
    uint16_t samp_idx;
    int16_t samp_val[ADC_VOLT_SAMP_CNT];
} adc_volt_stu;

typedef struct
{
    uint8_t conv_flag;
    uint16_t pwm_cnt;
    uint16_t samp_idx;
    uint16_t samp_val[ADC_CP_SAMP_CNT];
} adc_cp_stu;

typedef struct
{
    uint8_t conv_flag;
    uint16_t samp_idx;
    uint16_t samp_val[ADC_TEMP_SAMP_CNT];
} adc_temp_stu;

typedef struct
{
    uint8_t conv_flag;
    uint16_t samp_idx;
    int16_t samp_val[ADC_CURR_SAMP_CNT];
} adc_curr_stu;

typedef struct
{
    uint8_t conv_flag;
    uint16_t samp_idx;
    int16_t samp_val[ADC_PE_SAMP_CNT];
} adc_pe_stu;

typedef struct
{
    uint8_t conv_flag;
    uint16_t samp_idx;
    uint16_t samp_val[ADC_LEAK_SAMP_CNT];
} adc_leak_stu;

typedef struct
{
    uint8_t conv_flag;
    uint8_t samp_idx;
    uint16_t samp_val[ADC_CC_SAMP_CNT];
} adc_cc_stu;

typedef enum
{
    VOLT1 = 0,
    VOLT2,
    VOLT3,
    VOLT_NUM
} VOLT_INDEX_TYPE;

typedef enum
{
    CURR1 = 0,
    CURR2,
    CURR3,
    CURR_NUM
} CURR_INDEX_TYPE;

typedef enum
{
    FREQ1 = 0,
    FREQ2,
    FREQ3,
    FREQ_NUM
} FREQ_INDEX_TYPE;

typedef struct
{
    adc_volt_stu volt[VOLT_NUM];

    adc_cp_stu cp;

    adc_curr_stu curr[CURR_NUM];

    adc_temp_stu temp_board;

    adc_temp_stu temp_plug;

    adc_temp_stu temp_plug2;

    adc_pe_stu pe;

    adc_leak_stu leak;

    adc_cc_stu cc;

    uint16_t real_val[ADC_NUM];

} bsp_adc_stu;

void bsp_adc_init();

void bsp_adc_conv();

uint16_t bsp_adc_val_get(ADC_TYPE adc_n);

void bsp_adc_set_cp_pwm_val(uint16_t pwm);

typedef void (*bsp_cp_sta_change_callback_func)(uint8_t cp_sta);
typedef void (*bsp_leak_change_callback_func)(uint16_t leak_val);

void bsp_set_cp_sta_change_func(bsp_cp_sta_change_callback_func cp_callback);
void bsp_set_leak_change_func(bsp_leak_change_callback_func leak_callback);

#endif