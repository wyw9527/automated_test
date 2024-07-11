#include "app_system.h"

#define SYSTEM_QUE_PEND_TMOUT 10

static void app_sys_work(uint8_t period);
static void app_sys_init(void);

static app_system_stu sys_info;


/***************************************************************************
 * FuncName     : main()
 *
 * Desc         : 主入口函数
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/3/19
 ***************************************************************************/
int main(void)
{
	uint8_t *p_msg;
	uint32_t tick_rec, tick_curr = 0, qmout = SYSTEM_QUE_PEND_TMOUT, tm_accu = 0;

	app_sys_init();

	while (1)
	{
		tick_rec = rt_tick_get();
		if (RT_EOK == rt_mb_recv(&mb_sys, (rt_ubase_t *)&p_msg, qmout))
		{
			switch (*p_msg)
			{
			case BSP_MB_APP_ID:
				break;
			}

			tick_curr = rt_tick_get();
			if (tick_curr < tick_rec)
			{
				qmout = (0xFFFFFFFFu - tick_rec) + tick_curr;
			}
			else
			{
				qmout = tick_curr - tick_rec;
			}

			tm_accu += qmout;
			if (tm_accu >= SYSTEM_QUE_PEND_TMOUT)
			{
				tm_accu = 0;

				app_sys_work(SYSTEM_QUE_PEND_TMOUT);
			}
			qmout = SYSTEM_QUE_PEND_TMOUT - tm_accu;
		}
		else
		{
			app_sys_work(SYSTEM_QUE_PEND_TMOUT);

			qmout = SYSTEM_QUE_PEND_TMOUT;

			tm_accu = 0;
		}
	}
}


/***************************************************************************************
 * @Function    : app_sys_dev_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/17
 ****************************************************************************************/
static void app_sys_dev_init(void)
{
	/* system clock init */
	sys_info.clock = drv_dev_find(DRV_DEV_CLOCK);
	sys_info.run_led = drv_dev_find(DRV_DEV_DIDO_RUN_LED);
	sys_info.analog_intf = drv_dev_find(DRV_DEV_ANALOG_INTF);
    // sys_info.cmp_load = cmp_dev_find(CMP_DEV_NAME_LOAD);
    // sys_info.cmp_power = cmp_dev_find(CMP_DEV_NAME_POWER);

}

/***************************************************************************************
 * @Function    : app_sys_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/16
 ****************************************************************************************/
static void app_sys_init(void)
{
	app_config_mb_init();

	app_config_task_init();

	app_sys_dev_init();
}

/***************************************************************************************
 * @Function    : app_sys_clock_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/17
 ****************************************************************************************/
static void app_sys_clock_work(void)
{
	 sys_info.clock->drv_clock_upt();
}

/***************************************************************************************
 * @Function    : app_sys_run_led_work()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : WYW            Version   : V2.0.0                Date:2023/9/28
 ****************************************************************************************/
static void app_sys_run_led_work(uint8_t period)
{
	static uint16_t count = 0;
	static uint8_t flag = 0;
	if (count < 500)
	{
		if(flag == 0)
		{
			sys_info.run_led->drv_do_off();
			flag = 1;
		}
		
	}
	else
	{
		if(count > 1000)
		{
			count = 0;
			return;
		}
		if(flag == 1)
		{
			sys_info.run_led->drv_do_on();
			flag = 0;
		}
	}
	count += period;
}

/***************************************************************************************
 * @Function    : app_sys_analog_work()
 *
 * @Param       : void
 *
 * @Return      : void
 *
 * @Description :
 *
 * @Author      : WYW            Version   : V2.0.0                Date:2023/11/23
 ****************************************************************************************/
static void app_sys_analog_work(uint8_t period)
{
	// sys_info.analog_intf->drv_analog_conv_interface();
}

// static void upper_485_test(uint8_t period)
// {
    // static uint8_t test_485 = 0;
    // if(test_485 == 1 )
    // {
    //     sys_info.cmp_load->cmp_load_set_current(10,10,10);
    //     test_485 = 0;
    // }
    // else if(test_485 == 2)
    // {
    //     sys_info.cmp_load->cmp_load_set_current(0,0,0);
    //     test_485 = 0;
    // }

// 	static uint8_t test_485 = 0;
//     if(test_485 == 1 )
//     {
//         sys_info.cmp_power->cmp_set_power_box_voltage(230,230,230);
//         test_485 = 0;
//     }
//     else if(test_485 == 2)
//     {
//         sys_info.cmp_power->cmp_set_power_box_voltage(280,280,280);
//         test_485 = 0;
//     }
//     else if(test_485 == 3)
//     {
//         sys_info.cmp_power->cmp_set_power_box_voltage(180,180,180);
//         test_485 = 0;
//     }
// 	else if(test_485 == 4)
//     {
//         sys_info.cmp_power->cmp_set_power_box_voltage(230,230,0);
//         test_485 = 0;
//     }
// 	else if(test_485 == 5)
//     {
//         sys_info.cmp_power->cmp_set_power_box_voltage(0,0,0);
//         test_485 = 0;
//     }
// 	else if(test_485 == 6)
// 	{
// 		sys_info.cmp_power->cmp_set_power_box_start();
// 		        test_485 = 0;

// 	}
// 		else if(test_485 == 7)
// 	{
// 		sys_info.cmp_power->cmp_set_power_box_stop();
// 		        test_485 = 0;

// 	}
// }

// static void upper_can_test(uint8_t period)
// {
//     static uint8_t test_can = 0;
//     if(test_can == 1 )
//     {
//         sys_info.cmp_load->cmp_load_set_all_switch_on();
//         test_can = 0;
//     }
//     else if(test_can == 2)
//     {
//         sys_info.cmp_load->cmp_load_set_all_switch_off();
//         test_can = 0;
//     }

// }


/***************************************************************************
 * FuncName     : app_sys_work()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/6/16
 ***************************************************************************/
static void app_sys_work(uint8_t period)
{
	app_sys_clock_work();
	app_sys_run_led_work(period);
	app_sys_analog_work(period);
	// app_upper_work(period);
}
