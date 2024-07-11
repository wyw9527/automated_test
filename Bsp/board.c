#include "board.h"


/***************************************************************************************
 * @Function    : bsp_base_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/18
 ****************************************************************************************/

static int bsp_base_init(void)
{
	extern void bsp_alarm_mb_init(void);
	extern void bsp_adc_init(void);
	extern int bsp_io_dev_create(void);
	extern int bsp_pmu_dev_create(void);
	extern int bsp_pwm_dev_ctreate(void);
	extern int bsp_rtc_create(void);
	extern int bsp_spi_dev_create(void);
	extern int bsp_uart_dev_create(void);
	extern int bsp_can_dev_create(void);

	bsp_alarm_mb_init();
	bsp_adc_init();
	bsp_io_dev_create();
	bsp_pmu_dev_create();
	bsp_pwm_dev_ctreate();
	bsp_rtc_create();
	bsp_spi_dev_create();
	bsp_uart_dev_create();
	bsp_can_dev_create();
	return 0;
}
INIT_PREV_EXPORT(bsp_base_init);
