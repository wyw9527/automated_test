#include "drv_power_box.h"
#include "SEGGER_RTT.h"


static drv_power_box_pub_stu power_box_pub;
// static drv_can_stu serial_conf[DEV_SERIAL_NUM];

static void power_box_printf(uint8_t *data, uint16_t len)
{
    uint16_t i;
    for(i=0;i<len;i++)
    {
        Fox_ESS_Printf("%02x", data[i]);
    }
    Fox_ESS_Printf("\r\n");
}

static void drv_power_box_pub_dev_init(void)
{
    // bsp_uart_config_param_stu bsp_param;

    // load_485_pub.uart = bsp_dev_find(BSP_DEV_NAME_UART);
    // if (NULL == load_485_pub.uart)
    // {
    //     return;
    // }
    // load_485_pub.uart->bsp_uart_init(BSP_UART3, &bsp_param);
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
static void drv_power_box_rx_buff_msg_frame(uint8_t period)
{


}

static void drv_power_box_state_check(uint8_t period)
{
    

}
static void drv_power_box_work(uint8_t period)
{
    drv_power_box_rx_buff_msg_frame(period);
    drv_power_box_state_check(period);
}


// static void calculate_load_current(uint8_t set_curr)
// {

// }


static void drv_power_box_pack_func(uint8_t len, uint8_t type, uint8_t cmd, uint8_t *data)
{
    uint16_t pos = 0;
    uint8_t buff[50];
    uint8_t check_value;

    memset(buff, 0, sizeof(buff));

    buff[pos++] = 0x7B;  //帧头
    sort_high_byte_first(&buff[pos],len);  //总字节
    pos += 2;
    buff[pos++] = 0x01;  //从机地址
    buff[pos++] = type;  //设置类
    buff[pos++] = cmd;  //命令码
    memcpy(&buff[pos],data,len-8);  //数据
    pos += len-8;
    check_value = power_box_msg_sum_check(&buff[1], len-3);
    buff[pos++] = check_value;  //校验和
    buff[pos++] = 0x7D;  //帧尾

    Fox_ESS_Printf("power_tx:");
    power_box_printf(buff, len);

    power_box_pub.power_box_serial->dev_send_msg(buff, len);
}

static void drv_power_box_set_voltage(uint32_t volt_a, uint32_t volt_b, uint32_t volt_c)
{
    uint8_t data[20];
    
    uint8_t set_Va[3];
    uint8_t set_Vb[3];
    uint8_t set_Vc[3];

    set_Va[0] = ((volt_a*100) >> 16) & 0xFF; 
    set_Va[1] = ((volt_a*100) >> 8) & 0xFF; 
    set_Va[2] = (volt_a*100) & 0xFF; 
    set_Vb[0] = ((volt_b*100) >> 16) & 0xFF; 
    set_Vb[1] = ((volt_b*100) >> 8) & 0xFF; 
    set_Vb[2] = (volt_b*100) & 0xFF; 
    set_Vc[0] = ((volt_c*100) >> 16) & 0xFF; 
    set_Vc[1] = ((volt_c*100) >> 8) & 0xFF; 
    set_Vc[2] = (volt_c*100) & 0xFF; 


    data[0] = 0x00;
    data[1] = 0x14;  //缓启动2s
    data[2] = 0x00; 
    data[3] = 0xC3;
    data[4] = 0x50;  //缓启动50Hz
    memcpy(&data[5],set_Va,3);  //U相电压
    data[8] = 0x00;
    data[9] = 0x00;  //U相角：0°
    memcpy(&data[10],set_Vb,3);  //V相电压
    data[13] = 0x04;
    data[14] = 0xB0;  //V相角：120°
    memcpy(&data[15],set_Vc,3);  //W相电压
    data[18] = 0x09;
    data[19] = 0x60;  //W相角：240°

    drv_power_box_pack_func(0x1C, 0x5A, 0x41, data);
    
}

static void drv_power_box_start()
{
    drv_power_box_pack_func(0x08, 0x0F, 0xFF, NULL);
}
static void drv_power_box_stop()
{
    drv_power_box_pack_func(0x08, 0x0F, 0x00, NULL);
}

static void drv_power_box_set_uart(drv_serial_dev_stu *serial)
{
    power_box_pub.power_box_serial = serial;
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
static void drv_power_box_create(void)
{
    static drv_power_box_dev_stu power_box;

    power_box.drv_power_box_work = drv_power_box_work;
    power_box.drv_power_box_set_voltage = drv_power_box_set_voltage;
    power_box.drv_power_box_start = drv_power_box_start;
    power_box.drv_power_box_stop = drv_power_box_stop;
    power_box.drv_power_box_set_uart = drv_power_box_set_uart;

    drv_dev_append(DRV_DEV_POWER_BOX, &power_box);
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
int drv_power_box_dev_create(void)
{
    drv_power_box_pub_dev_init();

    drv_power_box_create();

    return 0;
}

INIT_COMPONENT_EXPORT(drv_power_box_dev_create);

