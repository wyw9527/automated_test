#include "cmp_load.h"



static cmp_load_stu load_conf;



static void cmp_load_pub_dev_init()
{
    load_conf.load_can = drv_dev_find(DRV_DEV_LOAD_CAN);
    load_conf.load_485 = drv_dev_find(DRV_DEV_LOAD_485);
    load_conf.load_serial = drv_dev_find(DRV_DEV_SERIAL_LOAD);
    if (NULL == load_conf.load_can || NULL == load_conf.load_485 || NULL == load_conf.load_serial)
    {
        return;
    }
    drv_serial_init_param_stu param;
    param.baut =  SERIAL_BAUT_9600;
    param.dma_sta = SERIAL_DMA_ENABLE;
    param.parity = SERIAL_PARITY_NONE;
    param.stb = SERIAL_STB_1BIT;
    param.wl = SERIAL_WL_8BIT;

    load_conf.load_serial->dev_param_init(&param);
    load_conf.load_485->drv_load_485_set_uart(load_conf.load_serial);
}

// static void cmp_load_status_check()
// {
//     //判断通信、温度、。。。
//     //出现异常，设置电流为0，卸载
// }

// static void cmp_load_fan_work(uint8_t period)
// {
//     //负载开风扇开;负载关后，等温度降到域值一下关闭风扇
//     //负载关后30s关风扇
//     if(load_conf.procc->count <= 30000)
//     {
//         load_conf.procc->count += period;
//     }
//     if(load_conf.procc->fan_flag == 0)
//     {
//         load_conf.load_485->drv_load_turn_off_fan();
//         load_conf.procc->fan_flag = 1;
//     }
// }

static void cmp_load_set_current(uint16_t curr_a, uint16_t curr_b, uint16_t curr_c)
{
    load_conf.load_485->drv_load_485_set_current(curr_a, curr_b, curr_c);
    if(curr_a == 0 && curr_b == 0 && curr_c == 0)
    {
        load_conf.procc->count = 0;
    }

}

static void cmp_load_set_all_switch_on()
{
    load_conf.load_can->drv_load_can_all_switch_on();
}

static void cmp_load_set_all_switch_off()
{
    load_conf.load_can->drv_load_can_all_switch_off();
}


static void cmp_load_work(uint8_t period)
{
    load_conf.load_can->drv_load_can_work(period);
    load_conf.load_485->drv_load_485_work(period);
    // cmp_load_status_check(period);
    // cmp_load_fan_work(period);
}
static void cmp_load_create()
{
    static cmp_load_dev_stu load_dev;
    load_dev.cmp_load_work = cmp_load_work;
    load_dev.cmp_load_set_current = cmp_load_set_current;
    load_dev.cmp_load_set_all_switch_on = cmp_load_set_all_switch_on;
    load_dev.cmp_load_set_all_switch_off = cmp_load_set_all_switch_off;
    
    cmp_dev_append(CMP_DEV_NAME_LOAD, &load_dev);
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
static int cmp_load_dev_create(void)
{
    
    cmp_load_pub_dev_init();

    cmp_load_create();

    return 0;

}
INIT_ENV_EXPORT(cmp_load_dev_create);
