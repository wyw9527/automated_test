#ifndef _DRV_ANALOG_H_
#define _DRV_ANALOG_H_

#include "math.h"
#include "drv.h"

#define NTC_TBL_LEN 166
#define NTC_TEMP_LO -40
#define NTC_TEMP_HI 125
#define NTC_TBL_SCALE 10.0

#define AC_AVG_BUFF_SIZE 20
#define AC_AVG_CURR_BUFF_SIZE 20

typedef struct
{
    int16_t *msg;
    uint16_t size;
    uint16_t pos;
} drv_analog_buff_stu;

typedef struct
{
    /* data */
    uint32_t avg_sum;
    uint16_t avg_idx;
    uint16_t avg_val;
    uint16_t avg_buff[AC_AVG_BUFF_SIZE];
    uint8_t avg_tick;
} drv_analog_avg_stu;

typedef enum
{
    ANALOG_AC_DER_NONE,
    ANALOG_AC_DER_DOWM,
    ANALOG_AC_DER_UP,
} DRV_ANALOG_AC_DER_TYPE;

typedef enum
{
    NALOG_AC_POLARITY_NONE,
    NALOG_AC_POLARITY_NEG,
    NALOG_AC_POLARITY_POS,
} DRV_ANALOG_AC_POLARITY_TYPE;

typedef struct
{
    uint8_t dec_up_count;
    uint8_t dec_down_count;
    uint8_t pol_neg_count;
    uint8_t pol_pos_count;
    DRV_ANALOG_AC_DER_TYPE decetion;
    DRV_ANALOG_AC_POLARITY_TYPE polarity;
} drv_analog_ac_wave_count_stu;

typedef struct
{
    uint16_t cp;
    uint16_t cc;
    uint16_t temp_board;
    uint16_t temp_powr1;
    uint16_t temp_powr2;

    uint16_t temp_plug;
    uint16_t volt1;
    uint16_t volt2;
    uint16_t volt3;
    uint16_t curr1;
    uint16_t curr2;
    uint16_t curr3;
    uint16_t ne;
    uint16_t pe;
    uint16_t leak;
} drv_analog_real_val_stu;

typedef struct
{
    uint8_t cp_start_idx;

    drv_analog_real_val_stu real_val;

} drv_analog_stu;

typedef struct
{
    uint16_t (*drv_analog_get_real_val)(void);

    void (*drv_analog_set_start_duty)(uint16_t duty);

} drv_analog_dev_stu;

typedef struct
{
    void (*drv_analog_conv_interface)(void);
} drv_analog_interface_stu;

#endif