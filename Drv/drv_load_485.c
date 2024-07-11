#include "drv_load_485.h"
#include "SEGGER_RTT.h"


static drv_load_485_pub_stu load_485_pub;
// static drv_can_stu serial_conf[DEV_SERIAL_NUM];

static void load_printf(uint8_t *data, uint16_t len)
{
    uint16_t i;
    for(i=0;i<len;i++)
    {
        Fox_ESS_Printf("%02x", data[i]);
    }
    Fox_ESS_Printf("\r\n");
}

static void drv_load_485_pub_dev_init(void)
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
static void drv_load_rx_buff_msg_frame(uint8_t period)
{


}

static void drv_load_state_check(uint8_t period)
{
    

}
static void drv_load_485_work(uint8_t period)
{
    drv_load_rx_buff_msg_frame(period);
    drv_load_state_check(period);
}



int getIndex(char type, float value)
{
	const LoadValueType loadVaule[32] = 
	{
		{'F',0},  {'L',0},  {'A',0.1},{'A',0.2},{'A',0.3},{'A',0.4},{'B',0.1},{'B',0.2},
		{'B',0.3},{'B',0.4},{'C',0.1},{'C',0.2},{'C',0.3},{'C',0.4},{'B',4},  {'B',3},
		{'B',2},  {'B',1},  {'A',4},  {'A',3},  {'A',2},  {'A',1},  {'C',1},  {'B',2},
		{'C',15}, {'C',10}, {'C',4},  {'C',3},  {'B',15}, {'B',10}, {'A',15}, {'A',10},
	};
	
	for(uint8_t i=0;i<32;i++)
	{
		if(loadVaule[i].dataType == type && loadVaule[i].dataValue == value)
		{
			return i;	
		}
	}
	return -1;
}

uint32_t getConvValue(char type, float value, uint32_t output)
{
	uint8_t dataIdx;
	
	dataIdx = getIndex(type, value);
			
	if(dataIdx >=0)
	{
		output = output |(1 << dataIdx);
	}
	return output;
}

static uint32_t calculate_load_current(uint8_t curr_a, uint8_t curr_b, uint8_t curr_c)
{	
	const float param[10]={15, 10, 4, 3, 2, 1, 0.4, 0.3, 0.2, 0.1};
	const char  param_c[3] ={'A','B','C'};
	LoadInputDataType input;
	
	input.data[0] = curr_a;  
	input.data[1] = curr_b; 
	input.data[2] = curr_c; 
	
	uint32_t output = 0;
	float temp = 0;
	
	//设置值
	for(int i=0;i<3;i++)
	{
		//风扇判断
		if(input.FanSta)
		{
			output = getConvValue('F', 0, output);
		}
		//加载判断
		if(input.FanSta)
		{
			output = getConvValue('L', 0, output);
		}
		//数据判断
		temp = input.data[i];
		for(int j=0;j<10;j++)
		{
			if(temp>=param[j])
			{
				temp -= param[j];
				output = getConvValue(param_c[i], param[j], output);
			}
		}
	}
	   
   return output;
}

static void drv_load_485_pack_func(uint8_t *msg)
{
    uint8_t check_code;
    uint8_t buff[17];
    buff[0] = 0x01;
    buff[1] = 0xA6;
    buff[2] = 0x00;
    buff[3] = 0x00;
    buff[4] = 0x00;
    buff[5] = 0x40;
    buff[6] = 0x08;
    memcpy(&buff[7],msg,8);
    check_code = crc16_calc((uint8_t *)buff, 15);
    memcpy(&buff[15],&check_code,2);

    Fox_ESS_Printf("485_tx:");
    load_printf(buff, 17);

    load_485_pub.load_485_serial->dev_send_msg(buff, 17);
}

static void drv_load_485_set_current(uint8_t curr_a, uint8_t curr_b, uint8_t curr_c)
{
    uint32_t output;
    uint8_t msg[8];
    if(curr_a == 0 && curr_b == 0 && curr_c == 0)
    {
        uint8_t msg[8] = {0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
        drv_load_485_pack_func(msg);
    }
    else
    {
        output = calculate_load_current(curr_a, curr_b, curr_c);
        sort_low_byte_first(msg, output);
        drv_load_485_pack_func(msg);
    }
}

static void drv_load_485_turn_off_fan()
{
    uint8_t msg[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    drv_load_485_pack_func(msg);
}

static void drv_load_485_set_uart(drv_serial_dev_stu *serial)
{
    load_485_pub.load_485_serial = serial;
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
static void drv_load_485_create(void)
{
    static drv_load_485_dev_stu load_485;

    load_485.drv_load_485_work = drv_load_485_work;
    load_485.drv_load_485_set_current = drv_load_485_set_current;
    load_485.drv_load_485_turn_off_fan = drv_load_485_turn_off_fan;
    load_485.drv_load_485_set_uart = drv_load_485_set_uart;

    drv_dev_append(DRV_DEV_LOAD_485, &load_485);
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
int drv_load_485_dev_create(void)
{
    drv_load_485_pub_dev_init();

    drv_load_485_create();

    return 0;
}

INIT_COMPONENT_EXPORT(drv_load_485_dev_create);

