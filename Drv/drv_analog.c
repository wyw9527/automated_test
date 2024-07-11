#include "drv_analog.h"


static drv_analog_stu analog_conf;
static drv_analog_avg_stu avg_volt[3];
static drv_analog_avg_stu avg_curr[3];

/***************************************************************************************
 * @Function    : bubbleSort()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :冒泡排序
 *
 * @Author      : WYW            Version   : V2.0.0                Date:2023/11/14
 ****************************************************************************************/
static uint8_t bubbleSort(int16_t *arr, uint16_t n)
{
    drv_sort_s16_data(arr, n);
    return 0;
}

/***************************************************************************************
 * @Function    : drv_analog_cp_get_real_val()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/31
 ****************************************************************************************/
static uint16_t drv_analog_cp_get_real_val(void)
{
    return bsp_adc_val_get(ADC_CP);
}

/***************************************************************************************
 * @Function    : drv_analog_cp_set_start_duty()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/15
 ****************************************************************************************/
static void drv_analog_cp_set_start_duty(uint16_t duty)
{
    bsp_adc_set_cp_pwm_val(duty);
}

/***************************************************************************************
 * @Function    : drv_analog_cp_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/31
 ****************************************************************************************/
static void drv_analog_cp_create(void)
{
    static drv_analog_dev_stu cp_dev;

    cp_dev.drv_analog_get_real_val = drv_analog_cp_get_real_val;

    cp_dev.drv_analog_set_start_duty = drv_analog_cp_set_start_duty;

    drv_dev_append(DRV_DEV_ANALOG_CP, &cp_dev);
}

/***************************************************************************************
 * @Function    : drv_analog_cc_get_real_val()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/31
 ****************************************************************************************/
static uint16_t drv_analog_cc_get_real_val(void)
{
    return bsp_adc_val_get(ADC_CC);
}

/***************************************************************************************
 * @Function    : drv_analog_cc_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/31
 ****************************************************************************************/
static void drv_analog_cc_create(void)
{
    static drv_analog_dev_stu cc_dev;

    cc_dev.drv_analog_get_real_val = drv_analog_cc_get_real_val;

    drv_dev_append(DRV_DEV_ANALOG_CC, &cc_dev);
}

/***************************************************************************************
 * @Function    : drv_analog_temp_board_get_real_val()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/31
 ****************************************************************************************/
static uint16_t drv_analog_temp_board_get_real_val()
{
    return bsp_adc_val_get(ADC_TEMP_BOARD);
}
static uint16_t drv_analog_temp_powr1_get_real_val()
{
    return bsp_adc_val_get(ADC_TEMP_PLUG);
}
static uint16_t drv_analog_temp_powr2_get_real_val()
{
    return bsp_adc_val_get(ADC_TEMP_PLUG2);
}

/***************************************************************************************
 * @Function    : drv_analog_temp_board_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/31
 ****************************************************************************************/
static void drv_analog_temp_board_create(void)
{
    static drv_analog_dev_stu temp_board;

    temp_board.drv_analog_get_real_val = drv_analog_temp_board_get_real_val;

    drv_dev_append(DRV_DEV_ANALOG_TMP_B, &temp_board);
}

static void drv_analog_temp_powr1_create(void)
{
    static drv_analog_dev_stu temp_powr1;

    temp_powr1.drv_analog_get_real_val = drv_analog_temp_powr1_get_real_val;

    drv_dev_append(DRV_DEV_ANALOG_TMP_PWR1, &temp_powr1);
}

static void drv_analog_temp_powr2_create(void)
{
    static drv_analog_dev_stu temp_powr2;

    temp_powr2.drv_analog_get_real_val = drv_analog_temp_powr2_get_real_val;

    drv_dev_append(DRV_DEV_ANALOG_TMP_PWR2, &temp_powr2);
}

/***************************************************************************************
 * @Function    : drv_analog_temp_plug_get_real_val()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/31
 ****************************************************************************************/
static uint16_t drv_analog_temp_plug_get_real_val()
{
    return analog_conf.real_val.temp_powr1 > analog_conf.real_val.temp_powr2 ? analog_conf.real_val.temp_powr1 : analog_conf.real_val.temp_powr2;
}

/***************************************************************************************
 * @Function    : drv_analog_temp_plug_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/31
 ****************************************************************************************/
static void drv_analog_temp_plug_create(void)
{
    static drv_analog_dev_stu temp_plug;

    temp_plug.drv_analog_get_real_val = drv_analog_temp_plug_get_real_val;

    drv_dev_append(DRV_DEV_ANALOG_TMP_P, &temp_plug);
}

/***************************************************************************************
 * @Function    : drv_analog_volt_get_real_val()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/31
 ****************************************************************************************/
static uint16_t drv_analog_volt1_get_real_val()
{
    return analog_conf.real_val.volt1 > 300 ? analog_conf.real_val.volt1 : 0;
}

static uint16_t drv_analog_volt2_get_real_val()
{
    return analog_conf.real_val.volt2 > 300 ? analog_conf.real_val.volt2 : 0;
}

static uint16_t drv_analog_volt3_get_real_val()
{
    return analog_conf.real_val.volt3 > 300 ? analog_conf.real_val.volt3 : 0;
}

/***************************************************************************************
 * @Function    : drv_analog_volt_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/31
 ****************************************************************************************/
static void drv_analog_volt1_create(void)
{
    static drv_analog_dev_stu volt1_dev;

    volt1_dev.drv_analog_get_real_val = drv_analog_volt1_get_real_val;

    drv_dev_append(DRV_DEV_ANALOG_VOLT1, &volt1_dev);
}

static void drv_analog_volt2_create(void)
{
    static drv_analog_dev_stu volt2_dev;

    volt2_dev.drv_analog_get_real_val = drv_analog_volt2_get_real_val;

    drv_dev_append(DRV_DEV_ANALOG_VOLT2, &volt2_dev);
}

static void drv_analog_volt3_create(void)
{
    static drv_analog_dev_stu volt3_dev;

    volt3_dev.drv_analog_get_real_val = drv_analog_volt3_get_real_val;

    drv_dev_append(DRV_DEV_ANALOG_VOLT3, &volt3_dev);
}

/***************************************************************************************
 * @Function    : drv_analog_curr_get_real_val()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/31
 ****************************************************************************************/
static uint16_t drv_analog_curr1_get_real_val()
{
    return analog_conf.real_val.curr1;
}
static uint16_t drv_analog_curr2_get_real_val()
{
    return analog_conf.real_val.curr2;
}
static uint16_t drv_analog_curr3_get_real_val()
{
    return analog_conf.real_val.curr3;
}
/***************************************************************************************
 * @Function    : drv_analog_curr_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/31
 ****************************************************************************************/
static void drv_analog_curr1_create(void)
{
    static drv_analog_dev_stu curr1_dev;

    curr1_dev.drv_analog_get_real_val = drv_analog_curr1_get_real_val;

    drv_dev_append(DRV_DEV_ANALOG_CURR1, &curr1_dev);
}

static void drv_analog_curr2_create(void)
{
    static drv_analog_dev_stu curr2_dev;

    curr2_dev.drv_analog_get_real_val = drv_analog_curr2_get_real_val;

    drv_dev_append(DRV_DEV_ANALOG_CURR2, &curr2_dev);
}

static void drv_analog_curr3_create(void)
{
    static drv_analog_dev_stu curr3_dev;

    curr3_dev.drv_analog_get_real_val = drv_analog_curr3_get_real_val;

    drv_dev_append(DRV_DEV_ANALOG_CURR3, &curr3_dev);
}

/***************************************************************************************
 * @Function    : drv_analog_ne_get_real_val()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/31
 ****************************************************************************************/
static uint16_t drv_analog_ne_get_real_val(void)
{
    return 0;
}
static uint16_t drv_analog_pe_get_real_val(void)
{
    return bsp_adc_val_get(ADC_PE);
}

/***************************************************************************************
 * @Function    : drv_analog_ne_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/31
 ****************************************************************************************/
static void drv_analog_ne_create(void)
{
    static drv_analog_dev_stu ne_dev;

    ne_dev.drv_analog_get_real_val = drv_analog_ne_get_real_val;

    drv_dev_append(DRV_DEV_ANALOG_NE, &ne_dev);
}
static void drv_analog_pe_create(void)
{
    static drv_analog_dev_stu pe_dev;

    pe_dev.drv_analog_get_real_val = drv_analog_pe_get_real_val;

    drv_dev_append(DRV_DEV_ANALOG_PE, &pe_dev);
}

/***************************************************************************************
 * @Function    : drv_analog_leak_get_real_val()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/31
 ****************************************************************************************/
static uint16_t drv_analog_leak_get_real_val(void)
{
    return bsp_adc_val_get(ADC_LEAK);
}

/***************************************************************************************
 * @Function    : drv_analog_leak_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/31
 ****************************************************************************************/
static void drv_analog_leak_create(void)
{
    static drv_analog_dev_stu leak_dev;

    leak_dev.drv_analog_get_real_val = drv_analog_leak_get_real_val;

    drv_dev_append(DRV_DEV_ANALOG_LEAK, &leak_dev);
}

/***************************************************************************
*@Function    :drv_get_volt_val
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2024-01-22
***************************************************************************/
static uint16_t drv_get_volt_val(uint8_t idx)
{
    static uint16_t valout_cnt = 0;
    if (idx > 2)
        return 0;
    avg_volt[idx].avg_buff[avg_volt[idx].avg_idx++] = bsp_adc_val_get((ADC_TYPE)idx);
    if (avg_volt[idx].avg_idx >= AC_AVG_BUFF_SIZE)
    {
        bubbleSort((int16_t *)avg_volt[idx].avg_buff, AC_AVG_BUFF_SIZE);

        uint16_t res = avg_volt[idx].avg_buff[AC_AVG_BUFF_SIZE - 1] - avg_volt[idx].avg_buff[0];

        for (uint8_t i = 0; i < AC_AVG_BUFF_SIZE; i++)
        {
            avg_volt[idx].avg_sum += avg_volt[idx].avg_buff[i];
        }
        uint16_t tmp = avg_volt[idx].avg_sum / AC_AVG_BUFF_SIZE;

        avg_volt[idx].avg_sum = 0;
        avg_volt[idx].avg_idx = 0;

        if (res > 300)
        {
            valout_cnt++;
        }
        else
        {
            avg_volt[idx].avg_val = tmp;
        }
    }
    return avg_volt[idx].avg_val > 30 ? avg_volt[idx].avg_val : 0;
}

/***************************************************************************
*@Function    :drv_get_volt_val_func
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2024-01-22
***************************************************************************/
static void drv_get_volt_val_func()
{
    analog_conf.real_val.volt1 = drv_get_volt_val(0);
    analog_conf.real_val.volt2 = drv_get_volt_val(1);
    analog_conf.real_val.volt3 = drv_get_volt_val(2);
}

/***************************************************************************
*@Function    :
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2024-01-22
***************************************************************************/
static uint16_t drv_get_curr_val(uint8_t idx)
{
    uint8_t val_tick = 0;
    if (idx > 2)
        return 0;
    avg_curr[idx].avg_buff[avg_curr[idx].avg_idx++] = bsp_adc_val_get((ADC_TYPE)(idx + 3));
    if (avg_curr[idx].avg_idx >= AC_AVG_CURR_BUFF_SIZE)
    {
        bubbleSort((int16_t *)avg_curr[idx].avg_buff, AC_AVG_CURR_BUFF_SIZE);
        for (uint8_t i = 0; i < AC_AVG_CURR_BUFF_SIZE; i++)
        {
            if (avg_curr[idx].avg_buff[i] > 0)
            {
                avg_curr[idx].avg_sum += avg_curr[idx].avg_buff[i];
                val_tick++;
            }
        }
        avg_curr[idx].avg_val = 0;
        if (val_tick > 0)
        {
            avg_curr[idx].avg_val = avg_curr[idx].avg_sum / val_tick;
        }

        avg_curr[idx].avg_sum = 0;
        avg_curr[idx].avg_idx = 0;
    }
    return avg_curr[idx].avg_val > 5 ? avg_curr[idx].avg_val : 0;
}

static void drv_get_curr_val_func()
{
    analog_conf.real_val.curr1 = drv_get_curr_val(0);
    analog_conf.real_val.curr2 = drv_get_curr_val(1);
    analog_conf.real_val.curr3 = drv_get_curr_val(2);
}

/***************************************************************************
*@Function    :
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2024-01-17
***************************************************************************/
static void drv_analog_get_adc_conv_val()
{
    drv_get_volt_val_func();
    drv_get_curr_val_func();

    analog_conf.real_val.cp = bsp_adc_val_get(ADC_CP);
    analog_conf.real_val.cc = bsp_adc_val_get(ADC_CC);
    analog_conf.real_val.leak = bsp_adc_val_get(ADC_LEAK);
    analog_conf.real_val.pe = bsp_adc_val_get(ADC_PE);
    analog_conf.real_val.temp_board = bsp_adc_val_get(ADC_TEMP_BOARD);
    analog_conf.real_val.temp_powr1 = bsp_adc_val_get(ADC_TEMP_PLUG);
    analog_conf.real_val.temp_powr2 = bsp_adc_val_get(ADC_TEMP_PLUG2);
}

/***************************************************************************
*@Function    :drv_analog_volt_compute
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-12-01
***************************************************************************/
void drv_analog_volt_011k_compute()
{
    extern void bsp_adc_conv(void);
    bsp_adc_conv();
    drv_analog_get_adc_conv_val();
}

/***************************************************************************************
 * @Function    : drv_analog_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/18
 ****************************************************************************************/
static int drv_analog_dev_create(void)
{
    static drv_analog_interface_stu drv_analog_interf;
    memset(avg_volt, 0, sizeof(avg_volt));
    memset(avg_curr, 0, sizeof(avg_volt));
    drv_analog_cp_create();
    drv_analog_cc_create();

    drv_analog_temp_board_create();
    drv_analog_temp_powr1_create();
    drv_analog_temp_powr2_create();
    drv_analog_temp_plug_create();

    drv_analog_volt1_create();
    drv_analog_volt2_create();
    drv_analog_volt3_create();

    drv_analog_curr1_create();
    drv_analog_curr2_create();
    drv_analog_curr3_create();

    drv_analog_ne_create();
    drv_analog_leak_create();
    drv_analog_pe_create();

    drv_analog_interf.drv_analog_conv_interface = drv_analog_volt_011k_compute;

    drv_dev_append(DRV_DEV_ANALOG_INTF, &drv_analog_interf);

    return 0;
}
INIT_COMPONENT_EXPORT(drv_analog_dev_create);

