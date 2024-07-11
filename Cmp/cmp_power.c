#include "cmp_power.h"



static cmp_power_stu power_conf;



static void cmp_power_pub_dev_init()
{
    power_conf.power_box = drv_dev_find(DRV_DEV_POWER_BOX);
    power_conf.power_serial = drv_dev_find(DRV_DEV_SERIAL_POWER);
    if (RT_NULL == power_conf.power_box || RT_NULL == power_conf.power_serial)
    {
        return;
    }
    drv_serial_init_param_stu param;
    param.baut =  SERIAL_BAUT_9600;
    param.dma_sta = SERIAL_DMA_ENABLE;
    param.parity = SERIAL_PARITY_NONE;
    param.stb = SERIAL_STB_1BIT;
    param.wl = SERIAL_WL_8BIT;

    power_conf.power_serial->dev_param_init(&param);
    power_conf.power_box->drv_power_box_set_uart(power_conf.power_serial);
}

static void cmp_set_power_box_voltage(uint32_t volt_a, uint32_t volt_b, uint32_t volt_c)
{
    power_conf.power_box->drv_power_box_set_voltage(volt_a, volt_b, volt_c);
}

static void cmp_set_power_box_start()
{
    power_conf.power_box->drv_power_box_start();
}

static void cmp_set_power_box_stop()
{
    power_conf.power_box->drv_power_box_stop();
}

static void cmp_power_work(uint8_t period)
{
    power_conf.power_box->drv_power_box_work(period);
    // cmp_load_status_check(period);
    // cmp_load_fan_work(period);
}
static void cmp_power_create()
{
    static cmp_power_dev_stu power_dev;
    power_dev.cmp_power_work = cmp_power_work;
    power_dev.cmp_set_power_box_voltage = cmp_set_power_box_voltage;
    power_dev.cmp_set_power_box_start = cmp_set_power_box_start;
    power_dev.cmp_set_power_box_stop = cmp_set_power_box_stop;
    

    cmp_dev_append(CMP_DEV_NAME_POWER, &power_dev);
}

/***************************************************************************
* FuncName     : cmp_prot_can_dev_create()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : SSY         Version:    V1.0      Date:    2024/6/18
***************************************************************************/
static int cmp_power_dev_create(void)
{
    
    cmp_power_pub_dev_init();

    cmp_power_create();

    return 0;

}
INIT_ENV_EXPORT(cmp_power_dev_create);
