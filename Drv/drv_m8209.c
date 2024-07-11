#include "drv_m8209.h"



static drv_m8209_stu m8209_conf[BSP_POLE_PLUG_NUM];

/***************************************************************************************
* @Function    : m8209_crc_calc()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V2.0.0                Date:2023/4/10
****************************************************************************************/
static uint8_t m8209_crc_calc(uint8_t init_val,uint8_t* buff,uint8_t len)
{
    for(uint8_t i=0;i<len;i++)
    {
	init_val += buff[i];
    }
    
    return(~init_val);
}

/***************************************************************************************
* @Function    : m8209_read_reg()
*
* @Param       : 
*
* @Return      : 0 read 1 fail 2 succ
*
* @Description : 
*
* @Author      : chenbao            Version   : V2.0.0                Date:2023/4/10
****************************************************************************************/
static uint8_t m8209_read_reg(drv_m8209_stu* m8209,uint8_t reg_addr)
{
    uint8_t len,read_buff[8],check;
    
    if(m8209->tx.tx_func == RT_NULL) return 1;
    
    if(m8209->tx.tx_flag ==0)
    {
	m8209->tx.tx_func(&reg_addr,1);//发送cmd
	m8209->tx.tx_flag = 1;
	m8209->tx.tx_tmr = 0;
	return 0;
    }
    
    if(++m8209->tx.tx_tmr < 250)
    {
	if(m8209->tx.tx_tmr < 3)
	{
	    return 0;
	}
	if(m8209->rx.rx_flag ==0) return 0;
	
	check = m8209_crc_calc(reg_addr,m8209->rx.rx_buff,m8209->rx.rx_len -1);
	if(check != read_buff[len-1])
	{
	    return 0;
	}
	
	m8209->rx.rx_val = 0;
	for(uint8_t i=0;i<len-1;i++)
	{
	    m8209->rx.rx_val = m8209->rx.rx_val << 8;
	    m8209->rx.rx_val += read_buff[i];
	}
	m8209->tx.tx_tmr = 0;
	m8209->tx.tx_flag = 0;
	return 2;
    }
    
    m8209->tx.tx_tmr = 0;
    m8209->tx.tx_flag = 0;
    return 1;
}

/***************************************************************************************
* @Function    : m8209_read_chip_id()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V2.0.0                Date:2023/4/21
****************************************************************************************/
static uint8_t m8209_read_chip_id(drv_m8209_stu* m8209)
{
    uint8_t reg;
    
    reg = m8209_read_reg(m8209,DEVICE_ID_ADDR);
    
    if(reg == 0) return 0;
    
    if(reg == 1)
    {
	m8209->procc.state = 1;
	return 0;
    }

    if(m8209->rx.rx_val != 0x820900)
    {
	m8209->procc.state = 2;
	return 0;
    }
    return 1;
}

/***************************************************************************************
* @Function    : rn8209_write_buff()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/7/21
****************************************************************************************/
static void rn8209_write_buff(drv_m8209_stu* m8209,uint8_t Address, uint8_t *dwData,uint8_t Date_len)
{
    uint8_t i,CheckSumR = 0;
    uint8_t DwTemp[6] = {0};
  
    DwTemp[0] = Address|0x80;//cmd
    memcpy(&DwTemp[1], dwData, Date_len);//data
    for(i=0;i<Date_len+1;i++)
    {
        CheckSumR += DwTemp[i];
    }
    DwTemp[Date_len+1] = ~CheckSumR;
    m8209->tx.tx_func(DwTemp,Date_len+2);//发送cmd
    rt_thread_mdelay(20);
}

/***************************************************************************************
* @Function    : m8209_param_config0()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V2.0.0                Date:2023/4/21
****************************************************************************************/
static uint8_t m8209_param_config0(drv_m8209_stu* m8209)
{
    uint8_t regdata[4] = {0};
//    uint32_t id_read = 0;
    
//    id_read = rn8209_read_buff(DeviceID,3);//芯片ID//supposed to be 0x00830200(hex)
//    if(id_read != 0x820900) //读ID
//    {
//	return 0;
//    }
    
    regdata[0] = ResetEmu;
    rn8209_write_buff(m8209,SpecialCommand,regdata,1);//复位
    
    regdata[0] = WriteEN;
    rn8209_write_buff(m8209,SpecialCommand,regdata,1);//写使能
    
    regdata[0] = SelectA;
    rn8209_write_buff(m8209,SpecialCommand,regdata,1);//选择A通道作为计量
    
    /*通道B电流关闭，电流电压增益为1*/
    regdata[0] = 0x00;
    regdata[1] = 0x00;
    rn8209_write_buff(m8209,SYSCON,regdata,2);
    
    /* 使能PF/QF脉冲输出和电能读后不清零 */
    regdata[0] = 0x00;
    regdata[1] = 0x03;
    rn8209_write_buff(m8209,EMUCON,regdata,2);
    
    /* 03H  - 10H 先清零 */
    regdata[0] = 0x00;
    regdata[1] = 0x00;
    rn8209_write_buff(m8209,PStart,regdata,2);   
    regdata[0] = 0x00;
    regdata[1] = 0x00;
    rn8209_write_buff(m8209,QStart,regdata,2);
    regdata[0] = 0xFC;
    regdata[1] = 0xE7;
    rn8209_write_buff(m8209,GPQA,regdata,2);
    regdata[0] = 0x00;
    regdata[1] = 0x00;
    rn8209_write_buff(m8209,GPQB,regdata,2);
    regdata[0] = 0x20;
    regdata[1] = 0x00;
    rn8209_write_buff(m8209,PhsA,regdata,1);
    regdata[0] = 0x00;
    regdata[1] = 0x00;
    rn8209_write_buff(m8209,PhsB,regdata,1);
    regdata[0] = 0x00;
    regdata[1] = 0x00;
    rn8209_write_buff(m8209,QPhsCal,regdata,2);
    regdata[0] = 0x00;
    regdata[1] = 0x00;
    rn8209_write_buff(m8209,APOSA,regdata,2);
    regdata[0] = 0x00;
    regdata[1] = 0x00;
    rn8209_write_buff(m8209,APOSB,regdata,2);
    regdata[0] = 0x00;
    regdata[1] = 0x00;
    rn8209_write_buff(m8209,RPOSA,regdata,2);
    regdata[0] = 0x00;
    regdata[1] = 0x00;
    rn8209_write_buff(m8209,RPOSB,regdata,2);
    regdata[0] = 0x00;
    regdata[1] = 0x00;
    rn8209_write_buff(m8209,IARMSOS,regdata,2);
    regdata[0] = 0x00;
    regdata[1] = 0x00;
    rn8209_write_buff(m8209,IBRMSOS,regdata,2);
    regdata[0] = 0x00;
    regdata[1] = 0x00;
    rn8209_write_buff(m8209,IBGain,regdata,2);
    
    
    regdata[0] = 0x2E;
    regdata[1] = 0x38;
    rn8209_write_buff(m8209,HFConst,regdata,2);
    
    regdata[0] = WriteProtect;
    rn8209_write_buff(m8209,SpecialCommand,regdata,1);//写保护
    
    return 1; 
}

/***************************************************************************************
* @Function    : m8209_work_measure_get()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V2.0.0                Date:2023/4/11
****************************************************************************************/
static void m8209_work_measure_get(drv_m8209_stu* m8209)
{
    uint8_t ret;
    
    switch(m8209->procc.measure_step)
    {
    case 0:
	if(m8209_read_chip_id(m8209)>0)
	{
	    m8209->procc.measure_step++;
	}
	break;
	
    case 10://read power
	ret = m8209_read_reg(m8209,PowerPA);
	if(ret ==0) return;
	if(ret == 2)
	{
	    if(m8209->rx.rx_val < 0)
	    {
		m8209->rx.rx_val = -m8209->rx.rx_val;
	    }
	    m8209->measure.power = (uint32_t)(m8209->rx.rx_val * m8209->param.Kp);
	}
	m8209->procc.measure_step++;
	break;
	
    case 20://read volt 
	ret = m8209_read_reg(m8209,URMS);
	if(ret ==0) return;
	if(ret == 2)
	{
	    m8209->measure.volt = (uint32_t)(m8209->rx.rx_val * m8209->param.Ku);
	}
	m8209->procc.measure_step++;
	break;
	
    case 30://read curr
	ret = m8209_read_reg(m8209,IARMS);
	if(ret ==0) return;
	if(ret == 2)
	{
	    m8209->measure.curr = (uint32_t)(m8209->rx.rx_val * m8209->param.Ki);
	}
	m8209->procc.measure_step++;
	break;
	
    case 40://read freq
	ret = m8209_read_reg(m8209,UFreq);
	if(ret ==0) return;
	if(ret == 2)
	{
	    m8209->measure.freq = (uint32_t)(4474430/m8209->rx.rx_val);
	}
	m8209->procc.measure_step++;
	break;
	
    case 50:// read energy
	ret = m8209_read_reg(m8209,EnergyP);
	if(ret ==0) return;
	if(ret == 2)
	{
	    uint32_t temp = (uint32_t)(m8209->rx.rx_val * m8209->param.Kep);
	    if(temp > m8209->measure.energy)
	    {
		m8209->measure.energy = temp;
	    }
	}
	m8209->procc.measure_step++;
	break;
	
    default:
	if(++m8209->procc.measure_step > 200)
	{
	    m8209->procc.measure_step = 0;
	}
	break;
	
    }
}

/***************************************************************************************
* @Function    : drv_m8209_set_msg0()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/7/21
****************************************************************************************/
static void drv_m8209_set_msg0(uint8_t* msg,uint8_t len)
{
    m8209_conf[0].rx.rx_len = len;
    memcpy(m8209_conf[0].rx.rx_buff,msg,len);
    m8209_conf[0].rx.rx_flag = 1;
}

/***************************************************************************************
* @Function    : drv_m8209_set_send_func0()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/7/21
****************************************************************************************/
static void drv_m8209_set_send_func0(serial_tx_func_type tx_func)
{
    m8209_conf[0].tx.tx_func = tx_func;
}

/***************************************************************************************
* @Function    : drv_m8209_get_volume()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/7/24
****************************************************************************************/
static uint32_t drv_m8209_get_volume(DRV_M8209_VOLU_TYPE type)
{
    switch(type)
    {
    case M8209_VOLU_VOLT:return m8209_conf[0].measure.volt;
    case M8209_VOLU_CURR:return m8209_conf[0].measure.curr;
    case M8209_VOLU_POWER:return m8209_conf[0].measure.power;
    case M8209_VOLU_FREQ:return m8209_conf[0].measure.freq;
    case M8209_VOLU_ENERGY:return m8209_conf[0].measure.energy;
    }
    return 0;
}

/***************************************************************************************
* @Function    : drv_m8209_get_state()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/7/24
****************************************************************************************/
static uint8_t drv_m8209_get_state(void)
{
    return 0;
}

/***************************************************************************************
* @Function    : drv_m8209_dev_create()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/7/21
****************************************************************************************/
static int  drv_m8209_dev_create(void)
{
    static drv_m8209_dev_stu m8209_dev;
    
    m8209_dev.drv_set_msg = drv_m8209_set_msg0;
    m8209_dev.drv_set_send_func = drv_m8209_set_send_func0;
    m8209_dev.drv_get_volume = drv_m8209_get_volume;
    m8209_dev.drv_get_state = drv_m8209_get_state;
    drv_dev_append(DRV_DEV_M8209,&m8209_dev);
    return 0;
}
INIT_COMPONENT_EXPORT(drv_m8209_dev_create);

