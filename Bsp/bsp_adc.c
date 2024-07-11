#include "gd32f30x.h"
#include "bsp.h"
#include "bsp_adc.h"

static uint16_t ad_val[ADC_CONV_NUM];
static bsp_adc_stu adc_info;
static bsp_cp_sta_change_callback_func cp_sta_chang_callback = NULL;
static bsp_leak_change_callback_func leak_change_callback = NULL;

static void adc_cp_conv();
static void adc_leak_conv();
/***************************************************************************
 * FuncName     : adc_gpio_init()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/3/22
 ***************************************************************************/
static void adc_gpio_init()
{
    /* enable GPIOC clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);

    /* config the GPIO as analog mode */
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    gpio_init(GPIOB, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_0 | GPIO_PIN_1);
    gpio_init(GPIOC, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
}

/***************************************************************************
 * FuncName     : adc_dma_init()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/3/22
 ***************************************************************************/
static void adc_dma_init()
{
    dma_parameter_struct dma_stu;

    /* enable DMA clock*/
    rcu_periph_clock_enable(RCU_DMA0);

    /* ADC_DMA_channel deinit */
    dma_deinit(DMA0, DMA_CH0);

    /* initialize DMA data mode */
    dma_stu.periph_addr = (uint32_t)(&ADC_RDATA(ADC0));
    dma_stu.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_stu.memory_addr = (uint32_t)(&ad_val);
    dma_stu.memory_inc = DMA_PERIPH_INCREASE_ENABLE;
    dma_stu.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_stu.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_stu.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_stu.number = ADC_CONV_NUM;
    dma_stu.priority = DMA_PRIORITY_HIGH;
    dma_init(DMA0, DMA_CH0, &dma_stu);

    dma_circulation_enable(DMA0, DMA_CH0);

    // nvic_irq_enable(DMA_CHANNEL0_IRQ,1,1);
    // dma_interrupt_enable(DMA_CH0,DMA_INT_FTF);

    nvic_irq_enable(DMA0_Channel0_IRQn, 0, 0);
    dma_interrupt_enable(DMA0, DMA_CH0, DMA_INT_FTF | DMA_INT_ERR);

    dma_memory_to_memory_disable(ADC0, DMA_CH0);

    /*enable DMA channel */
    dma_channel_enable(DMA0, DMA_CH0);
}

/***************************************************************************
 * FuncName     : adc_param_config()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/3/22
 ***************************************************************************/
static void adc_param_config()
{
    /* enable adc clock */
    rcu_periph_clock_enable(RCU_ADC0);

    /* CONFIG ADC clock source :APB2 DIV6 */
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV6);

    /* reset ADC */
    adc_deinit(ADC0);

    /* ADC mode config*/
    adc_mode_config(ADC_MODE_FREE);

    /*ADC data aligment config */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);

    /* ADC SCAN function enable*/
    adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);

    /* ADC continuous mode enable*/
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);

    /* ADC channel length config */
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, ADC_CONV_NUM);

    /*ADC AC VOLT_L1 channel config */
    adc_regular_channel_config(ADC0, ADC_CONV_VOLT1, ADC_CHANNEL_10, ADC_SAMPLETIME_239POINT5);

    /*ADC AC VOLT_L2 channel config */
    adc_regular_channel_config(ADC0, ADC_CONV_VOLT2, ADC_CHANNEL_11, ADC_SAMPLETIME_239POINT5);

    /*ADC AC VOLT_L3 channel config */
    adc_regular_channel_config(ADC0, ADC_CONV_VOLT3, ADC_CHANNEL_12, ADC_SAMPLETIME_239POINT5);

    /*ADC AC VOLT_L3 channel config */
    adc_regular_channel_config(ADC0, ADC_CONV_CURR3, ADC_CHANNEL_4, ADC_SAMPLETIME_239POINT5);

    /*ADC AC VOLT_L3 channel config */
    adc_regular_channel_config(ADC0, ADC_CONV_CURR2, ADC_CHANNEL_5, ADC_SAMPLETIME_239POINT5);

    /*ADC AC VOLT_L3 channel config */
    adc_regular_channel_config(ADC0, ADC_CONV_CURR1, ADC_CHANNEL_6, ADC_SAMPLETIME_239POINT5);

    /*ADC AC VOLT_L3 channel config */
    adc_regular_channel_config(ADC0, ADC_CONV_TEMP_BOARD, ADC_CHANNEL_1, ADC_SAMPLETIME_239POINT5);

    /*ADC AC VOLT_L3 channel config */
    adc_regular_channel_config(ADC0, ADC_CONV_TEMP_PLUG, ADC_CHANNEL_13, ADC_SAMPLETIME_239POINT5);

    /*ADC AC VOLT_L3 channel config */
    adc_regular_channel_config(ADC0, ADC_CONV_TEMP_PLUG2, ADC_CHANNEL_7, ADC_SAMPLETIME_239POINT5);

    /*ADC AC VOLT_L3 channel config */
    adc_regular_channel_config(ADC0, ADC_CONV_CP, ADC_CHANNEL_14, ADC_SAMPLETIME_239POINT5);

    /*ADC AC VOLT_L3 channel config */
    adc_regular_channel_config(ADC0, ADC_CONV_CC, ADC_CHANNEL_15, ADC_SAMPLETIME_239POINT5);

    /*ADC AC VOLT_L3 channel config */
    adc_regular_channel_config(ADC0, ADC_CONV_PE, ADC_CHANNEL_8, ADC_SAMPLETIME_239POINT5);
    /*ADC AC VOLT_L3 channel config */
    adc_regular_channel_config(ADC0, ADC_CONV_LEAK, ADC_CHANNEL_9, ADC_SAMPLETIME_239POINT5);

    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);
    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);

    /* ADC DMA function enable */
    adc_dma_mode_enable(ADC0);

    adc_enable(ADC0);

    // bsp_delay_ms(10);
    rt_thread_mdelay(10);

    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);

    /* software start adc conv */
    adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);
}

/***************************************************************************
 * FuncName     : adc_volt_set_val()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/4/6
 ***************************************************************************/
static void adc_volt_set_val(uint16_t val, VOLT_INDEX_TYPE index)
{
    int16_t uval = val - 2048;

    if (adc_info.volt[index].conv_flag > 0)
    {
        return;
    }

    adc_info.volt[index].samp_val[adc_info.volt[index].samp_idx++] = uval;

    if (adc_info.volt[index].samp_idx >= ADC_VOLT_SAMP_CNT)
    {
        adc_info.volt[index].conv_flag = 0xaa;
        adc_info.volt[index].samp_idx = 0;
    }
}

/***************************************************************************
 * FuncName     : adc_cp_stat_check()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/5/6
 ***************************************************************************/
static void adc_cp_stat_check()
{

    static uint16_t pre_cp_sta = 0;
    static uint8_t cp_count = 0;

    uint16_t cp_val;

    cp_val = adc_info.real_val[ADC_CP];
    if (cp_val > 112 && cp_val < 128)
    {
        if (pre_cp_sta != 0)
        {
            cp_count = 0;
        }
        pre_cp_sta = 0;
    }
    else if (cp_val > 82 && cp_val < 98)
    {
        if (pre_cp_sta != 1)
        {
            cp_count = 0;
        }
        pre_cp_sta = 1;
    }
    else if (cp_val > 52 && cp_val < 68)
    {
        if (pre_cp_sta != 2)
        {
            cp_count = 0;
        }
        pre_cp_sta = 2;
    }
    else
    {
        if (pre_cp_sta != 3)
        {
            cp_count = 0;
        }
        pre_cp_sta = 3;
    }

    if (adc_info.real_val[ADC_CP_STA] == pre_cp_sta)
    {
        return;
    }

    if (++cp_count > 5)
    {
        adc_info.real_val[ADC_CP_STA] = pre_cp_sta;
        if (pre_cp_sta != 2 && cp_sta_chang_callback != NULL)
        {
            cp_sta_chang_callback(pre_cp_sta);
        }
    }
}

/***************************************************************************
 * FuncName     : adc_cp_set_val()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/4/6
 ***************************************************************************/
static void adc_cp_set_val(uint16_t val)
{
    adc_info.cp.samp_val[adc_info.cp.samp_idx++] = val;

    if (adc_info.cp.samp_idx >= ADC_CP_SAMP_CNT)
    {
        adc_cp_conv();
        adc_cp_stat_check();
        adc_info.cp.samp_idx = 0;
    }
}

/***************************************************************************
 * FuncName     : adc_temp_set_val()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/4/6
 ***************************************************************************/
static void adc_temp_board_set_val(uint16_t val)
{
    if (adc_info.temp_board.conv_flag > 0)
    {
        return;
    }

    adc_info.temp_board.samp_val[adc_info.temp_board.samp_idx++] = val;

    if (adc_info.temp_board.samp_idx >= ADC_TEMP_SAMP_CNT)
    {
        adc_info.temp_board.samp_idx = 0;
        adc_info.temp_board.conv_flag = 0xaa;
    }
}

/***************************************************************************
 * FuncName     : adc_temp_plug_set_val()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/6/20
 ***************************************************************************/
static void adc_temp_plug_set_val(uint16_t val)
{
    if (adc_info.temp_plug.conv_flag > 0)
    {
        return;
    }

    adc_info.temp_plug.samp_val[adc_info.temp_plug.samp_idx++] = val;

    if (adc_info.temp_plug.samp_idx >= ADC_TEMP_SAMP_CNT)
    {
        adc_info.temp_plug.samp_idx = 0;
        adc_info.temp_plug.conv_flag = 0xaa;
    }
}
static void adc_temp_plug2_set_val(uint16_t val)
{
    if (adc_info.temp_plug2.conv_flag > 0)
    {
        return;
    }

    adc_info.temp_plug2.samp_val[adc_info.temp_plug2.samp_idx++] = val;

    if (adc_info.temp_plug2.samp_idx >= ADC_TEMP_SAMP_CNT)
    {
        adc_info.temp_plug2.samp_idx = 0;
        adc_info.temp_plug2.conv_flag = 0xaa;
    }
}

/***************************************************************************
 * FuncName     : adc_curr_set_val()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/4/7
 ***************************************************************************/
static void adc_curr_set_val(uint16_t val, CURR_INDEX_TYPE index)
{
    if (adc_info.curr[index].conv_flag > 0)
    {
        return;
    }

    adc_info.curr[index].samp_val[adc_info.curr[index].samp_idx++] = val - 2048;

    if (adc_info.curr[index].samp_idx >= ADC_CURR_SAMP_CNT)
    {
        adc_info.curr[index].samp_idx = 0;
        adc_info.curr[index].conv_flag = 0xaa;
    }
}

/***************************************************************************
 * FuncName     : adc_pe_set_val()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : RGxiang         Version:    V1.0      Date:    2022/8/31
 ***************************************************************************/
static void adc_pe_set_val(uint16_t val)
{
    if (adc_info.pe.conv_flag > 0)
    {
        return;
    }

    adc_info.pe.samp_val[adc_info.pe.samp_idx] = val - 2048;
    if (++adc_info.pe.samp_idx >= ADC_PE_SAMP_CNT)
    {
        adc_info.pe.conv_flag = 0xaa;
        adc_info.pe.samp_idx = 0;
    }
}

/***************************************************************************
 * FuncName     : adc_leak_set_val()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/4/7
 ***************************************************************************/
static void adc_leak_set_val(uint16_t val)
{
    if (adc_info.leak.conv_flag > 0)
    {
        return;
    }

    adc_info.leak.samp_val[adc_info.leak.samp_idx] = val;
    if (++adc_info.leak.samp_idx >= ADC_LEAK_SAMP_CNT)
    {

        adc_info.leak.samp_idx = 0;
        adc_info.leak.conv_flag = 0xaa;
        adc_leak_conv();
    }
}

/***************************************************************************
 * FuncName     : adc_cc_set_val()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/4/16
 ***************************************************************************/
static void adc_cc_set_val(uint16_t val)
{
    if (adc_info.cc.conv_flag > 0)
    {
        return;
    }

    adc_info.cc.samp_val[adc_info.cc.samp_idx++] = val;

    if (adc_info.cc.samp_idx >= ADC_CC_SAMP_CNT)
    {
        adc_info.cc.samp_idx = 0;
        adc_info.cc.conv_flag = 0xaa;
    }
}

/***************************************************************************
 * FuncName     : DMA0_Channel0_IRQHandler()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/3/31
 ***************************************************************************/
void DMA0_Channel0_IRQHandler()
{
    dma_interrupt_flag_clear(DMA0, DMA_CH0, DMA_INT_FLAG_G);

    /* volt1 & freq samp value */
    adc_volt_set_val(ad_val[ADC_CONV_VOLT1], VOLT1);

    /* volt2 & freq samp value */
    adc_volt_set_val(ad_val[ADC_CONV_VOLT2], VOLT2);

    /* volt3 & freq samp value */
    adc_volt_set_val(ad_val[ADC_CONV_VOLT3], VOLT3);

    /* cp samp value */
    adc_cp_set_val(ad_val[ADC_CONV_CP]);

    /* temp_board samp value */
    adc_temp_board_set_val(ad_val[ADC_CONV_TEMP_BOARD]);

    /* temp_plug samp value */
    adc_temp_plug_set_val(ad_val[ADC_CONV_TEMP_PLUG]);

    adc_temp_plug2_set_val(ad_val[ADC_CONV_TEMP_PLUG2]);

    /* curr1 samp value */
    adc_curr_set_val(ad_val[ADC_CONV_CURR1], CURR1); // TEST

    /* curr2 samp value */
    adc_curr_set_val(ad_val[ADC_CONV_CURR2], CURR2); // TEST

    /* curr3 samp value */
    adc_curr_set_val(ad_val[ADC_CONV_CURR3], CURR3); // TEST

    /* NE samp value */
    adc_pe_set_val(ad_val[ADC_CONV_PE]);

    /* Leak samp value */
    adc_leak_set_val(ad_val[ADC_CONV_LEAK]); // TEST

    /* CC samp value */
    adc_cc_set_val(ad_val[ADC_CONV_CC]);
}

/***************************************************************************
 * FuncName     : bsp_adc_init()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/3/22
 ***************************************************************************/
void bsp_adc_init()
{
    adc_gpio_init();

    adc_dma_init();

    adc_param_config();
}

/***************************************************************************
 * FuncName     : get_average_data()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/4/6
 ***************************************************************************/
static uint16_t get_average_data(uint16_t *data, uint16_t len)
{
    uint32_t sum = 0;

    for (uint16_t i = 0; i < len; i++)
    {
        sum += data[i];
    }

    uint16_t ave = sum / len;

    return ave;
}

/***************************************************************************
 * FuncName     : get_ac_wave_point()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/8/3
 ***************************************************************************/
// static uint8_t get_ac_wave_point(uint16_t* buff,uint16_t len,adc_ac_wave_stu* point)
//{
//     int8_t up_down_flag = -1 ;
//     uint16_t up_count=0,down_count = 0;
//
//     for(uint8_t i=0;i< len-1;i++)
//     {
//         if(buff[i] >= buff[i+1])
//         {
//             if(++up_count > 3)
//             {
//
//             }
//         }
//         else
//         {
//             down_count++
//         }
//     }
// }

/***************************************************************************
 * FuncName     : get_wave_piont()
 *
 * Desc         : return 0 fail ,1 succ
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/4/7
 ***************************************************************************/
static uint8_t get_wave_piont(int16_t *buff, uint16_t len, waveform_stu *wave)
{
    uint8_t start_sta = 0, step = 0; // 1 波谷开始  2波峰开始
    uint16_t i;
    int16_t start_idx = -1;

    /* 计算波峰波谷*/
    for (i = 0; i < len; i++)
    {
        if (buff[i] < AC_WAVE_ADC_LOW)
        {
            start_sta = 1;
            start_idx = i;
            break;
        }

        if (buff[i] > AC_WAVE_ADC_HIGH)
        {
            start_sta = 2;
            start_idx = i;
            break;
        }
    }

    /* 没有找到波峰或波谷 返回*/
    if (start_idx == -1)
    {
        return 0;
    }

    /* look for one circle */
    for (i = start_idx; i < len; i++)
    {
        /* start from wave low */
        if (start_sta == 1)
        {
            switch (step)
            {
            case 0:
                if (buff[i] >= AC_WAVE_ADC_LOW)
                {
                    wave->first_idx = i;
                    step++;
                }
                break;

            case 1:
                if (buff[i] >= AC_WAVE_ADC_HIGH)
                {
                    step++;
                }
                break;

            case 2:
                if (buff[i] <= AC_WAVE_ADC_LOW)
                {
                    step++;
                }
                break;

            case 3:
                if (buff[i] >= AC_WAVE_ADC_LOW)
                {
                    step++;
                    wave->last_idx = i;
                }
                break;

            default:
                break;
            }
        }
        /* start from wave low */
        else if (start_sta == 2)
        {
            switch (step)
            {
            case 0:
                if (buff[i] <= AC_WAVE_ADC_HIGH)
                {
                    wave->first_idx = i;
                    step++;
                }
                break;

            case 1:
                if (buff[i] <= AC_WAVE_ADC_LOW)
                {
                    step++;
                }
                break;

            case 2:
                if (buff[i] >= AC_WAVE_ADC_HIGH)
                {
                    step++;
                }
                break;

            case 3:
                if (buff[i] <= AC_WAVE_ADC_HIGH)
                {
                    step++;
                    wave->last_idx = i;
                }
                break;

            default:
                break;
            }
        }

        if (step == 4)
        {
            return 1;
        }
    }

    return 0;
}

/***************************************************************************
 * FuncName     : adc_volt_conv()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/4/6
 ***************************************************************************/
static void adc_volt_conv(VOLT_INDEX_TYPE index)
{
    uint8_t res;
    uint32_t i, volt_sum = 0;
    waveform_stu wave_point;

    if (adc_info.volt[index].conv_flag != 0xaa)
    {
        return;
    }

    res = get_wave_piont(adc_info.volt[index].samp_val, ADC_VOLT_SAMP_CNT, &wave_point);
    if (res == 1)
    {
        adc_info.real_val[ADC_FREQ1 + (uint8_t)index] = (uint32_t)(0.17924 * 10000000.0 / ((wave_point.last_idx - wave_point.first_idx) * 27));

        for (i = wave_point.first_idx; i < wave_point.last_idx; i++)
        {
            volt_sum = volt_sum + adc_info.volt[index].samp_val[i] * adc_info.volt[index].samp_val[i];
        }
        adc_info.real_val[ADC_VOLT1 + (uint8_t)index] = (uint16_t)(sqrt(volt_sum / (wave_point.last_idx - wave_point.first_idx)) * 3.5596);
    }
    else
    {
        adc_info.real_val[ADC_VOLT1 + (uint8_t)index] = 0;
    }

    adc_info.volt[index].conv_flag = 0;
}

/***************************************************************************
 * FuncName     : swap()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/5/6
 ***************************************************************************/
static void swap(uint16_t *val_a, uint16_t *val_b)
{
    uint16_t tmp = *val_a;
    *val_a = *val_b;
    *val_b = tmp;
}

/***************************************************************************
 * FuncName     : sort_data()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/5/6
 ***************************************************************************/
void sort_data(uint16_t *arr, uint16_t len)
{
    for (uint32_t i = 0; i < len - 1; ++i)
    {
        uint32_t max_idx = 0;
        for (uint32_t j = 0; j < len - i; ++j)
        {
            if (arr[j] > arr[max_idx])
            {
                max_idx = j;
            }
        }
        swap(&arr[max_idx], &arr[len - 1 - i]);
    }
}

/***************************************************************************
 * FuncName     : adc_cp_conv()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/4/6
 ***************************************************************************/
static void adc_cp_conv()
{
    uint16_t cp_ave = 0;

    sort_data(adc_info.cp.samp_val, ADC_CP_SAMP_CNT);

    if (adc_info.cp.pwm_cnt >= ADC_CP_SAMP_CNT)
    {
        adc_info.real_val[ADC_CP] = (uint16_t)((adc_info.cp.samp_val[ADC_CP_SAMP_CNT - 1]) * 0.0488 + 5.344);
        return;
    }

    cp_ave = get_average_data(&adc_info.cp.samp_val[adc_info.cp.pwm_cnt], ADC_CP_SAMP_CNT - adc_info.cp.pwm_cnt);
    adc_info.real_val[ADC_CP] = (uint16_t)(cp_ave * 0.0488 + 5.344);

    adc_info.cp.conv_flag = 0;
}

static const int32_t ntc_refer_data[NTC_TBL_LEN] =
    {
        //-40, -31
        248300L,
        233100L,
        219000L,
        205900L,
        193600L,
        182200L,
        171600L,
        161600L,
        152300L,
        143600L,
        //-30, -21
        135500L,
        127800L,
        120700L,
        114000L,
        107800L,
        101900L,
        96390L,
        91220L,
        86370L,
        81810L,
        //-20, -11
        77520L,
        73490L,
        69700L,
        66130L,
        62770L,
        59610L,
        56620L,
        53810L,
        51160L,
        48650L,
        //-10, -1
        46290L,
        44060L,
        41950L,
        39960L,
        38070L,
        36290L,
        34600L,
        33010L,
        31490L,
        30060L,
        // 0, 9
        28700L,
        27420L,
        26200L,
        25040L,
        23940L,
        22900L,
        21910L,
        20960L,
        20070L,
        19220L,
        // 10, 19
        18410L,
        17640L,
        16910L,
        16210L,
        15550L,
        14920L,
        14310L,
        13740L,
        13190L,
        12670L,
        // 20, 29
        12170L,
        11700L,
        11240L,
        10810L,
        10400L,
        10000L,
        9622L,
        9261L,
        8916L,
        8585L,
        // 30, 39
        8269L,
        7967L,
        7678L,
        7400L,
        7135L,
        6881L,
        6637L,
        6403L,
        6179L,
        5965L,
        // 40, 49
        5759L,
        5561L,
        5372L,
        5189L,
        5015L,
        4847L,
        4686L,
        4531L,
        4382L,
        4239L,
        // 50, 59
        4101L,
        3969L,
        3842L,
        3719L,
        3601L,
        3488L,
        3379L,
        3274L,
        3172L,
        3075L,
        // 60, 69
        2981L,
        2890L,
        2803L,
        2719L,
        2638L,
        2559L,
        2484L,
        2411L,
        2341L,
        2273L,
        // 70, 79
        2207L,
        2144L,
        2083L,
        2024L,
        1967L,
        1912L,
        1858L,
        1807L,
        1757L,
        1709L,
        // 80, 89
        1662L,
        1617L,
        1574L,
        1532L,
        1491L,
        1451L,
        1413L,
        1376L,
        1340L,
        1305L,
        // 90, 99
        1272L,
        1239L,
        1208L,
        1177L,
        1147L,
        1118L,
        1091L,
        1063L,
        1037L,
        1012L,
        // 100, 109
        987L,
        963L,
        940L,
        918L,
        895L,
        874L,
        853L,
        833L,
        814L,
        795L,
        // 110, 119
        776L,
        758L,
        741L,
        724L,
        707L,
        692L,
        676L,
        661L,
        646L,
        632L,
        // 120, 125
        618L,
        604L,
        591L,
        578L,
        566L,
        554L,
};

/***************************************************************************
 * FuncName     : sFindNtcTemp()
 *
 * Desc         : offset value 50??
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/6/20
 ***************************************************************************/
static uint16_t sFindNtcTemp(int32_t i32_Threshold, const int32_t *const Tbl)
{
    uint16_t start, end, mid;
    int32_t i32_Ret;

    start = 0U;
    end = NTC_TBL_LEN - 1U;
    if (i32_Threshold > Tbl[start])
    {
        i32_Ret = (NTC_TEMP_LO * NTC_TBL_SCALE);
    }
    else if (i32_Threshold < Tbl[end])
    {
        i32_Ret = (NTC_TEMP_HI * NTC_TBL_SCALE);
    }
    else
    {
        while ((start + 1U) < end)
        {
            mid = (start + end) >> 1;
            if (i32_Threshold > Tbl[mid])
            {
                end = mid;
            }
            else
            {
                start = mid;
            }
        }

        mid = (start + end) / 2U;
        if ((mid == (uint32_t)(NTC_TEMP_HI - NTC_TEMP_LO)) || (mid == 0U))
        {
            i32_Ret = (int32_t)(((int16_t)mid + NTC_TEMP_LO) * NTC_TBL_SCALE);
        }
        else if (i32_Threshold >= Tbl[mid])
        {
            i32_Ret = (int32_t)(((int16_t)mid + NTC_TEMP_LO) * NTC_TBL_SCALE);
        }
        else if (i32_Threshold <= Tbl[mid + 1U])
        {
            i32_Ret = (int32_t)(((int16_t)(mid + 1U) + NTC_TEMP_LO) * NTC_TBL_SCALE);
        }
        else
        {
            i32_Ret = (int32_t)(((int16_t)mid + NTC_TEMP_LO) * NTC_TBL_SCALE);
            i32_Ret += (int32_t)(((Tbl[mid] - i32_Threshold) * NTC_TBL_SCALE) / (Tbl[mid] - Tbl[mid + 1U]));
        }
    }

    if (i32_Ret < -500)
    {
        i32_Ret = -500;
    }

    return i32_Ret + 500;
}

/***************************************************************************
 * FuncName     : adc_temp_conv()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/4/6
 ***************************************************************************/
static void adc_temp_board_conv()
{
    uint32_t temp_avg = 0, ad_r = 0;

    if (adc_info.temp_board.conv_flag != 0xaa)
    {
        return;
    }

    temp_avg = get_average_data(adc_info.temp_board.samp_val, ADC_TEMP_SAMP_CNT);

    ad_r = (uint32_t)((float)(temp_avg * 10) / (4096 - temp_avg) * 1000);

    adc_info.real_val[ADC_TEMP_BOARD] = sFindNtcTemp(ad_r, ntc_refer_data);

    adc_info.temp_board.conv_flag = 0;
}

/***************************************************************************
 * FuncName     : adc_temp_plug_conv()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/6/20
 ***************************************************************************/
static void adc_temp_plug_conv()
{
    uint32_t temp_avg = 0, ad_r = 0;

    if (adc_info.temp_plug.conv_flag != 0xaa)
    {
        return;
    }

    temp_avg = get_average_data(adc_info.temp_plug.samp_val, ADC_TEMP_SAMP_CNT);

    ad_r = (uint32_t)((float)(temp_avg * 10) / (4096 - temp_avg) * 1000);

    adc_info.real_val[ADC_TEMP_PLUG] = sFindNtcTemp(ad_r, ntc_refer_data);

    adc_info.temp_plug.conv_flag = 0;
}

static void adc_temp_plug2_conv()
{
    uint32_t temp_avg = 0, ad_r = 0;

    if (adc_info.temp_plug2.conv_flag != 0xaa)
    {
        return;
    }

    temp_avg = get_average_data(adc_info.temp_plug2.samp_val, ADC_TEMP_SAMP_CNT);

    ad_r = (uint32_t)((float)(temp_avg * 10) / (4096 - temp_avg) * 1000);

    adc_info.real_val[ADC_TEMP_PLUG2] = sFindNtcTemp(ad_r, ntc_refer_data);

    adc_info.temp_plug2.conv_flag = 0;
}

/***************************************************************************
 * FuncName     : adc_curr_conv()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/4/7
 ***************************************************************************/
static void adc_curr_conv(CURR_INDEX_TYPE index)
{
    uint8_t res;
    uint32_t curr_sum = 0;
    waveform_stu wave_point;

    if (adc_info.curr[index].conv_flag != 0xaa)
    {
        return;
    }

    res = get_wave_piont(adc_info.curr[index].samp_val, ADC_CURR_SAMP_CNT, &wave_point);
    if (res == 1)
    {
        for (uint16_t i = wave_point.first_idx; i < wave_point.last_idx; i++)
        {
            curr_sum = curr_sum + adc_info.curr[index].samp_val[i] * adc_info.curr[index].samp_val[i];
        }

        adc_info.real_val[ADC_CURR1 + (uint8_t)index] = (uint16_t)(sqrt(curr_sum / (wave_point.last_idx - wave_point.first_idx)) * 0.271);
    }
    else
    {
        adc_info.real_val[ADC_CURR1 + (uint8_t)index] = 0;
    }

    adc_info.curr[index].conv_flag = 0;
}

/***************************************************************************
 * FuncName     : adc_pe_conv()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : RGxiang         Version:    V1.0      Date:    2022/8/31
 ***************************************************************************/
static void adc_pe_conv()
{
    uint8_t res;
    uint32_t i, sum = 0;
    waveform_stu wave_point;

    if (adc_info.pe.conv_flag != 0xaa)
    {
        return;
    }

    res = get_wave_piont(adc_info.pe.samp_val, ADC_PE_SAMP_CNT, &wave_point);
    if (res == 1)
    {
        for (i = wave_point.first_idx; i < wave_point.last_idx; i++)
        {
            sum = sum + adc_info.pe.samp_val[i] * adc_info.pe.samp_val[i];
        }
        adc_info.real_val[ADC_PE] = (uint16_t)(sqrt(sum / (wave_point.last_idx - wave_point.first_idx)) / 0.21942 * 0.17188);
    }
    else
    {
        adc_info.real_val[ADC_PE] = 0;
    }

    adc_info.pe.conv_flag = 0;
}

static uint8_t is_wave_piont(int16_t *buff, uint16_t len)
{
    uint8_t start_sta = 0, step = 0; // 1 波谷开始  2波峰开始
    uint16_t i;
    int16_t start_idx = -1;

    /* 计算波峰波谷*/
    for (i = 0; i < len; i++)
    {
        if (buff[i] < AC_WAVE_ADC_LOW)
        {
            start_sta = 1;
            start_idx = i;
            break;
        }

        if (buff[i] > AC_WAVE_ADC_HIGH)
        {
            start_sta = 2;
            start_idx = i;
            break;
        }
    }

    /* 没有找到波峰或波谷 返回*/
    if (start_idx == -1)
    {
        return 0;
    }

    /* look for one circle */
    for (i = start_idx; i < len; i++)
    {
        /* start from wave low */
        if (start_sta == 1)
        {
            switch (step)
            {
            case 0:
                if (buff[i] >= AC_WAVE_ADC_LOW)
                {
                    step++;
                }
                break;

            case 1:
                if (buff[i] >= AC_WAVE_ADC_HIGH)
                {
                    step++;
                }
                break;

            case 2:
                if (buff[i] <= AC_WAVE_ADC_LOW)
                {
                    step++;
                }
                break;

            case 3:
                if (buff[i] >= AC_WAVE_ADC_LOW)
                {
                    step++;
                }
                break;

            default:
                break;
            }
        }
        /* start from wave low */
        else if (start_sta == 2)
        {
            switch (step)
            {
            case 0:
                if (buff[i] <= AC_WAVE_ADC_HIGH)
                {
                    step++;
                }
                break;

            case 1:
                if (buff[i] <= AC_WAVE_ADC_LOW)
                {
                    step++;
                }
                break;

            case 2:
                if (buff[i] >= AC_WAVE_ADC_HIGH)
                {
                    step++;
                }
                break;

            case 3:
                if (buff[i] <= AC_WAVE_ADC_HIGH)
                {
                    step++;
                }
                break;

            default:
                break;
            }
        }

        if (step == 4)
        {
            return 1;
        }
    }

    return 0;
}

/***************************************************************************
 * FuncName     : adc_leak_conv()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/4/7
 ***************************************************************************/

static int16_t leak_wave_data[ADC_LEAK_SAMP_CNT];

static void adc_leak_conv()
{
    uint16_t temp_avg = 0;

    if (adc_info.leak.conv_flag != 0xaa)
    {
        return;
    }

    for (uint16_t i = 0; i < ADC_LEAK_SAMP_CNT; i++)
    {
        leak_wave_data[i] = adc_info.leak.samp_val[i] - 1400;
    }

    if (is_wave_piont(leak_wave_data, ADC_LEAK_SAMP_CNT))
    {
        adc_info.leak.conv_flag = 0;
        return;
    }

    sort_data(adc_info.leak.samp_val, ADC_LEAK_SAMP_CNT);
    temp_avg = get_average_data(&adc_info.leak.samp_val[16], ADC_LEAK_SAMP_CNT - 32);
    adc_info.real_val[ADC_LEAK] = temp_avg;

    if (leak_change_callback != NULL)
    {
        leak_change_callback(temp_avg);
    }
    adc_info.leak.conv_flag = 0;
}

/***************************************************************************
 * FuncName     : adc_leak_conv()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/4/16
 ***************************************************************************/
static void adc_cc_conv()
{
    uint32_t sum = 0;

    if (adc_info.cc.conv_flag != 0xaa)
    {
        return;
    }

    for (uint8_t i = 0; i < ADC_CC_SAMP_CNT; i++)
    {
        sum += adc_info.cc.samp_val[i];
    }
    adc_info.real_val[ADC_CC] = sum / ADC_CC_SAMP_CNT;

    adc_info.cc.conv_flag = 0;
}

/***************************************************************************
 * FuncName     : bsp_adc_conv()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/4/6
 ***************************************************************************/
void bsp_adc_conv()
{
    adc_volt_conv(VOLT1);

    adc_volt_conv(VOLT2);

    adc_volt_conv(VOLT3);

    adc_temp_board_conv();

    adc_temp_plug_conv();

    adc_temp_plug2_conv();

    adc_curr_conv(CURR1);

    adc_curr_conv(CURR2);

    adc_curr_conv(CURR3);

    adc_pe_conv();

    // adc_leak_conv();

    adc_cc_conv();
}

/***************************************************************************
 * FuncName     : bsp_adc_val_get()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/4/16
 ***************************************************************************/
uint16_t bsp_adc_val_get(ADC_TYPE adc_n)
{
    if (adc_n >= ADC_NUM)
        return 0;

    return adc_info.real_val[adc_n];
}

/***************************************************************************
 * FuncName     : bsp_adc_set_cp_pwm_val()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/11/1
 ***************************************************************************/
void bsp_adc_set_cp_pwm_val(uint16_t pwm)
{
    adc_info.cp.pwm_cnt = pwm * ADC_CP_SAMP_CNT / 1000;
    adc_info.cp.pwm_cnt = ADC_CP_SAMP_CNT - adc_info.cp.pwm_cnt + 5;
}

/***************************************************************************
 * FuncName     : bsp_set_cp_sta_change_func()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/11/2
 ***************************************************************************/
void bsp_set_cp_sta_change_func(bsp_cp_sta_change_callback_func cp_callback)
{
    cp_sta_chang_callback = cp_callback;
}

/***************************************************************************
 * FuncName     : bsp_set_leak_change_func()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/11/2
 ***************************************************************************/
void bsp_set_leak_change_func(bsp_leak_change_callback_func leak_callback)
{
    leak_change_callback = leak_callback;
}
