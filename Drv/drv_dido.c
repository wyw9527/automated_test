#include "drv_dido.h"


static bsp_io_dev_stu *io_dev;

/***************************************************************************************
 * @Function    : drv_di_eme_get_val()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/30
 ****************************************************************************************/
static uint8_t drv_di_eme_get_val(void)
{
    return io_dev->bsp_io_get_value(BSP_SOURCE_DI_EME_STA1) > 0 ? 0 : 1;
}

/***************************************************************************************
 * @Function    : drv_eme_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/30
 ****************************************************************************************/
static void drv_eme_dev_create(void)
{
    static drv_dido_di_dev_stu eme_dev;

    eme_dev.drv_di_get_val = drv_di_eme_get_val;

    io_dev->bsp_io_init(BSP_SOURCE_DI_EME_STA1);
    drv_dev_append(DRV_DEV_DIDO_EME, &eme_dev);
}

/***************************************************************************************
 * @Function    : drv_di_pnc_get_val()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/30
 ****************************************************************************************/
static uint8_t drv_di_pnc_get_val(void)
{
    return io_dev->bsp_io_get_value(BSP_SOURCE_DI_PNC_STA1);
}

/***************************************************************************************
 * @Function    : drv_di_ne_get_val()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : WYW            Version   : V2.0.0                Date:2023/10/24
 ****************************************************************************************/
static uint8_t drv_di_ne_get_val(void)
{
    return io_dev->bsp_io_get_value(BSP_SOURCE_DI_NE);
}

/***************************************************************************
*@Function    :drv_di_open_btn_get_val
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-11-06
***************************************************************************/
static uint8_t drv_di_open_btn_get_val(void)
{
    return io_dev->bsp_io_get_value(BSP_SOURCE_DI_OPENLID_STA);
}

/***************************************************************************************
 * @Function    : drv_stop_btn_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/30
 ****************************************************************************************/
static void drv_stop_btn_dev_create(void)
{
    static drv_dido_di_dev_stu stop_btn_dev;

    stop_btn_dev.drv_di_get_val = drv_di_pnc_get_val;

    io_dev->bsp_io_init(BSP_SOURCE_DI_PNC_STA1);
    drv_dev_append(DRV_DEV_DIDO_PNC, &stop_btn_dev);
}

/***************************************************************************************
 * @Function    : drv_ne_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : WYW            Version   : V2.0.0                Date:2023/10/24
 ****************************************************************************************/
static void drv_ne_dev_create(void)
{
    static drv_dido_di_dev_stu ne_dev;

    ne_dev.drv_di_get_val = drv_di_ne_get_val;

    io_dev->bsp_io_init(BSP_SOURCE_DI_NE);
    drv_dev_append(DRV_DEV_DIDO_NE, &ne_dev);
}

/***************************************************************************
*@Function    :drv_open_btn_dev_create
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-11-06
***************************************************************************/
static void drv_open_btn_dev_create()
{
    static drv_dido_di_dev_stu open_btn_dev;
    io_dev->bsp_io_init(BSP_SOURCE_DI_OPENLID_STA);
    open_btn_dev.drv_di_get_val = drv_di_open_btn_get_val;
    drv_dev_append(DRV_DEV_DIDO_OPENLID, &open_btn_dev);
}

/***************************************************************************************
 * @Function    : drv_di_plug_sel_get_val()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/30
 ****************************************************************************************/
static uint8_t drv_di_plug_sel_get_val(void)
{
    return io_dev->bsp_io_get_value(BSP_SOURCE_DI_PLUG_SEL);
}

/***************************************************************************************
 * @Function    : drv_dido_run_led_off()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/30
 ****************************************************************************************/
static void drv_dido_run_led_off(void)
{
    io_dev->bsp_io_reset(BSP_SOURCE_DO_RUN_LED);
}

/***************************************************************************************
 * @Function    : drv_dido_run_led_on()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/30
 ****************************************************************************************/
static void drv_dido_run_led_on(void)
{
    io_dev->bsp_io_set(BSP_SOURCE_DO_RUN_LED);
}

/***************************************************************************************
 * @Function    : drv_run_led_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/30
 ****************************************************************************************/
static void drv_run_led_dev_create(void)
{
    static drv_dido_do_dev_stu run_led;

    run_led.drv_do_off = drv_dido_run_led_off;
    run_led.drv_do_on = drv_dido_run_led_on;

    io_dev->bsp_io_init(BSP_SOURCE_DO_RUN_LED);
    drv_dev_append(DRV_DEV_DIDO_RUN_LED, &run_led);
}

/***************************************************************************************
 * @Function    : drv_dido_cpcc_sw_off()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/30
 ****************************************************************************************/
static void drv_dido_cpcc_sw_off(void)
{
    io_dev->bsp_io_set(BSP_SOURCE_DO_CC_CP_SW);
}

/***************************************************************************************
 * @Function    : drv_dido_cpcc_sw_on()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/30
 ****************************************************************************************/
static void drv_dido_cpcc_sw_on(void)
{
    io_dev->bsp_io_reset(BSP_SOURCE_DO_CC_CP_SW);
}

/***************************************************************************************
 * @Function    : drv_cpcc_sw_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/30
 ****************************************************************************************/
static void drv_cpcc_sw_dev_create(void)
{
    static drv_dido_do_dev_stu cpcc_sw;

    cpcc_sw.drv_do_off = drv_dido_cpcc_sw_off;
    cpcc_sw.drv_do_on = drv_dido_cpcc_sw_on;

    io_dev->bsp_io_init(BSP_SOURCE_DO_CC_CP_SW);

    drv_dev_append(DRV_DEV_DIDO_CPCC_SW, &cpcc_sw);
}

/***************************************************************************************
 * @Function    : drv_dido_esp_reset_off()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/30
 ****************************************************************************************/
static void drv_dido_esp_reset_off(void)
{
    io_dev->bsp_io_reset(BSP_SOURCE_DO_ESP32_RESET);
}

/***************************************************************************************
 * @Function    : drv_dido_esp_reset_on()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/30
 ****************************************************************************************/
static void drv_dido_esp_reset_on(void)
{
    io_dev->bsp_io_set(BSP_SOURCE_DO_ESP32_RESET);
}

/***************************************************************************************
 * @Function    : drv_esp32_reset_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/30
 ****************************************************************************************/
static void drv_esp32_reset_create(void)
{
    static drv_dido_do_dev_stu esp_reset;

    esp_reset.drv_do_off = drv_dido_esp_reset_off;
    esp_reset.drv_do_on = drv_dido_esp_reset_on;

    io_dev->bsp_io_init(BSP_SOURCE_DO_ESP32_RESET);
    drv_dido_esp_reset_off();
    drv_dev_append(DRV_DEV_DIDO_ESP_RESET, &esp_reset);
}

/***************************************************************************************
 * @Function    : drv_dido_led_r_off()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : WYW            Version   : V2.0.0                Date:2023/9/27
 ****************************************************************************************/
static void drv_dido_led_r_off(void)
{
    io_dev->bsp_io_reset(BSP_SOURCE_DO_LED_R);
}

/***************************************************************************************
 * @Function    : drv_dido_led_r_on()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : WYW            Version   : V2.0.0                Date:2023/9/27
 ****************************************************************************************/
static void drv_dido_led_r_on(void)
{
    io_dev->bsp_io_set(BSP_SOURCE_DO_LED_R);
}

/***************************************************************************************
 * @Function    : drv_led_r_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : WYW            Version   : V2.0.0                Date:2023/9/27
 ****************************************************************************************/
static void drv_led_r_dev_create(void)
{
    static drv_dido_do_dev_stu led_r;

    led_r.drv_do_off = drv_dido_led_r_off;
    led_r.drv_do_on = drv_dido_led_r_on;
    io_dev->bsp_io_init(BSP_SOURCE_DO_LED_R);

    drv_dev_append(DRV_DEV_LED_R, &led_r);
}

/***************************************************************************************
 * @Function    : drv_dido_led_g_off()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : WYW            Version   : V2.0.0                Date:2023/9/27
 ****************************************************************************************/
static void drv_dido_led_g_off(void)
{
    io_dev->bsp_io_reset(BSP_SOURCE_DO_LED_G);
}

/***************************************************************************************
 * @Function    : drv_dido_led_g_on()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : WYW            Version   : V2.0.0                Date:2023/9/27
 ****************************************************************************************/
static void drv_dido_led_g_on(void)
{
    io_dev->bsp_io_set(BSP_SOURCE_DO_LED_G);
}

/***************************************************************************************
 * @Function    : drv_led_g_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : WYW            Version   : V2.0.0                Date:2023/9/27
 ****************************************************************************************/
static void drv_led_g_dev_create(void)
{
    static drv_dido_do_dev_stu led_g;

    led_g.drv_do_off = drv_dido_led_g_off;
    led_g.drv_do_on = drv_dido_led_g_on;

    io_dev->bsp_io_init(BSP_SOURCE_DO_LED_G);
    drv_dev_append(DRV_DEV_LED_G, &led_g);
}

/***************************************************************************************
 * @Function    : drv_dido_led_b_off()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : WYW            Version   : V2.0.0                Date:2023/9/27
 ****************************************************************************************/
static void drv_dido_led_b_off(void)
{
    io_dev->bsp_io_reset(BSP_SOURCE_DO_LED_B);
}

/***************************************************************************************
 * @Function    : drv_dido_led_b_on()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : WYW            Version   : V2.0.0                Date:2023/9/27
 ****************************************************************************************/
static void drv_dido_led_b_on(void)
{
    io_dev->bsp_io_set(BSP_SOURCE_DO_LED_B);
}

/***************************************************************************
*@Function    :drv_do_leak_prod_on
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-11-06
***************************************************************************/
static void drv_do_leak_prod_on(void)
{
    io_dev->bsp_io_set(BSP_SOURCE_DO_AC_LEAK_PROD);
}

/***************************************************************************
*@Function    :drv_do_leak_selftest_on
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-11-06
***************************************************************************/
static void drv_do_leak_selftest_on(void)
{
    io_dev->bsp_io_set(BSP_SOURCE_DO_AC_LEAK_SELF_TEST);
}

/***************************************************************************
*@Function    :drv_do_leak_selftest_off
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-11-06
***************************************************************************/
static void drv_do_leak_selftest_off(void)
{
    io_dev->bsp_io_reset(BSP_SOURCE_DO_AC_LEAK_SELF_TEST);
}

/***************************************************************************
*@Function    :
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-11-06
***************************************************************************/
static void drv_do_leak_prod_off(void)
{
    io_dev->bsp_io_reset(BSP_SOURCE_DO_AC_LEAK_PROD);
}
/***************************************************************************************
 * @Function    : drv_led_b_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : WYW            Version   : V2.0.0                Date:2023/9/27
 ****************************************************************************************/
static void drv_led_b_dev_create(void)
{
    static drv_dido_do_dev_stu led_b;

    led_b.drv_do_off = drv_dido_led_b_off;
    led_b.drv_do_on = drv_dido_led_b_on;

    io_dev->bsp_io_init(BSP_SOURCE_DO_LED_B);
    drv_dev_append(DRV_DEV_LED_B, &led_b);
}

/***************************************************************************************
 * @Function    : drv_plug_sel_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/30
 ****************************************************************************************/
static void drv_plug_sel_dev_create(void)
{
    static drv_dido_di_dev_stu plug_sel;

    plug_sel.drv_di_get_val = drv_di_plug_sel_get_val;

    io_dev->bsp_io_init(BSP_SOURCE_DI_PLUG_SEL);
    drv_dev_append(DRV_DEV_DIDO_PLUG_SEL, &plug_sel);
}

/***************************************************************************
*@Function    :drv_ac_leak_prod_create
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-11-06
***************************************************************************/
static void drv_ac_leak_prod_create(void)
{
    static drv_dido_do_dev_stu leak_prod_dev;

    leak_prod_dev.drv_do_on = drv_do_leak_prod_on;
    leak_prod_dev.drv_do_off = drv_do_leak_prod_off;

    io_dev->bsp_io_init(BSP_SOURCE_DO_AC_LEAK_PROD);
    drv_dev_append(DRV_DEV_DIDO_LEAK_PROD, &leak_prod_dev);
}

/***************************************************************************
*@Function    :drv_dc_leak_sefttest_create
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-11-06
***************************************************************************/
static void drv_dc_leak_sefttest_create(void)
{
    static drv_dido_do_dev_stu leak_selftest_dev;
    leak_selftest_dev.drv_do_on = drv_do_leak_selftest_on;
    leak_selftest_dev.drv_do_off = drv_do_leak_selftest_off;

    io_dev->bsp_io_init(BSP_SOURCE_DO_AC_LEAK_SELF_TEST);
    leak_selftest_dev.drv_do_on();
    drv_dev_append(DRV_DEV_DIDO_LEAK_SELFTEST, &leak_selftest_dev);
}

/***************************************************************************************
 * @Function    : drv_dido_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/30
 ****************************************************************************************/
static int drv_dido_dev_create(void)
{
    io_dev = bsp_dev_find(BSP_DEV_NAME_IO);
    if (io_dev == RT_NULL)
    {
        return 0;
    }

    drv_eme_dev_create();

    drv_stop_btn_dev_create();

    drv_plug_sel_dev_create();

    drv_run_led_dev_create();

    drv_cpcc_sw_dev_create();

    drv_esp32_reset_create();

    drv_led_r_dev_create();

    drv_led_g_dev_create();

    drv_led_b_dev_create();

    drv_ne_dev_create();

    drv_open_btn_dev_create();

    // 交流漏电流产生信号
    drv_ac_leak_prod_create();

    // 直流漏电流自检
    drv_dc_leak_sefttest_create();

    return 0;
}
INIT_COMPONENT_EXPORT(drv_dido_dev_create);


