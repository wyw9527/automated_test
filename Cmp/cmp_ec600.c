#include "cmp_ec600.h"

static cmp_ec600_stu ec600_conf;

/***************************************************************************************
* @Function    : cmp_ec600_hnd_rx_dst()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 0 get dst result  1 get err result  2 dot have msg
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/8
****************************************************************************************/
static uint8_t cmp_ec600_hnd_rx_dst(const char* dst,char* err,char* ret_stp, char* ret,uint8_t ret_max)
{
    char* start_s,*stop_s;
    uint8_t len;
    drv_serial_rx_stu rx_msg;
    
    if(ec600_conf.serial->dev_get_msg(&rx_msg,0) != RT_EOK) return 2;
    
    if(rx_msg.len ==0) return 2;
    
    if(strstr((char*)rx_msg.buff,err) != RT_NULL) return 1;
    
    if((start_s = strstr((char*)rx_msg.buff,dst)) == RT_NULL) return 2;
    
    if(ret_stp == RT_NULL || ret == RT_NULL) return 0;
    
    if((stop_s = strstr(start_s,ret_stp)) == RT_NULL) return 2;
    
    len = strlen(start_s) - strlen(stop_s) - strlen(dst);
    if(len > ret_max)
    {
	len = ret_max;
    }
    strncpy(ret,&start_s[strlen(dst)],len);
    return 0;
}

/***************************************************************************************
* @Function    : cmp_ec600_send_cmd()
*
* @Param       : 
*
* @Return      : 0 succ 1 err 2 tmout
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/8
****************************************************************************************/
static uint8_t cmp_ec600_send_cmd(char* cmd,char*dst,char* err,char* ret_stp,char* ret,uint8_t max,uint16_t tmout_ms)
{
    uint8_t result;
    uint16_t wait_count = 0;
    
    ec600_conf.serial->dev_rx_buff_clear();
    if(cmd != RT_NULL)
    {
	ec600_conf.serial->dev_send_msg((uint8_t*)cmd,strlen(cmd));
    }
    
    while(wait_count < tmout_ms)
    {
	rt_thread_mdelay(10);
	if(dst == RT_NULL) 
	{
	     return 0;
	}
	
	if((result = cmp_ec600_hnd_rx_dst(dst,err,ret_stp,ret,max)) < 2)
	{
	    return result;
	}
	wait_count += 10;
    }
  
    return 2;
}

/***************************************************************************************
* @Function    : cmp_ec600_hardware_reset_cmd_cmd()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/8
****************************************************************************************/
static uint8_t cmp_ec600_hardware_reset_cmd(void)
{
    ec600_conf.reset->drv_do_off();
    rt_thread_mdelay(2000);
    ec600_conf.reset->drv_do_on();
    
    if(cmp_ec600_send_cmd(RT_NULL,"RDY",RT_NULL,RT_NULL,RT_NULL,0,5000) > 0)
    {
	ec600_conf.procc.reason = EC600_REASON_RESET_TMOUT;
	return 1;
    }
    
    if(cmp_ec600_send_cmd("ATE0\r\n","OK\r\n",RT_NULL,RT_NULL,RT_NULL,0,200)>0)
    {
	ec600_conf.procc.reason = EC600_REASON_ATE0_TMOUT;
	return 2;
    }
    
    return 0;
}

/***************************************************************************************
* @Function    : cmp_ec600_param_query_cmd()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/8
****************************************************************************************/
static uint8_t cmp_ec600_param_query_cmd(void)
{
    static char cmd_ret[32];
    
    memset((uint8_t*)cmd_ret,0,sizeof(cmd_ret));
    
    if(cmp_ec600_send_cmd("AT+CPIN?\r\n","CPIN: READY","ERROR",RT_NULL,RT_NULL,0,12000)>0)
    {
	ec600_conf.procc.reason = EC600_REASON_CPIN_TMOUT;
	return 2;
    }
    
    if(cmp_ec600_send_cmd("AT+CREG?\r\n","+CREG: 0,","ERROR","\r\n\r\nOK",cmd_ret,32,30000)>0)
    {
	ec600_conf.procc.reason = EC600_REASON_CREG_ERROR;
	return 3;
    }
    if(cmd_ret[0] != '1' && cmd_ret[0] != '5')
    {
	ec600_conf.procc.reason = EC600_REASON_CREG_TMOUT;
	return 4;
    }
    
    memset((uint8_t*)cmd_ret,0,sizeof(cmd_ret));
    if(cmp_ec600_send_cmd("AT+CGREG?\r\n","+CGREG: 0,","ERROR","\r\n\r\nOK",cmd_ret,32,30000)>0)
    {
	ec600_conf.procc.reason = EC600_REASON_CGREG_ERROR;
	return 5;
    }
    if(cmd_ret[0] != '1' && cmd_ret[0] != '5')
    {
	ec600_conf.procc.reason = EC600_REASON_CGREG_TMOUT;
	return 6;
    }
    
    memset((uint8_t*)cmd_ret,0,sizeof(cmd_ret));
    if(cmp_ec600_send_cmd("AT+QCCID\r\n","+QCCID:","ERROR","\r\n\r\nOK",cmd_ret,32,3000)>0)
    {
	ec600_conf.procc.reason = EC600_REASON_QCCID_TMOUT;
	return 7;
    }
    
    memset((uint8_t*)cmd_ret,0,sizeof(cmd_ret));
    if(cmp_ec600_send_cmd("AT+CSQ\r\n","+CSQ:","ERROR","\r\n\r\nOK",cmd_ret,32,3000)>0)
    {
	ec600_conf.procc.reason = EC600_REASON_QCCID_TMOUT;
	return 8;
    }
    
    return 0;
}

/***************************************************************************************
* @Function    : cmp_ec600_serv_conn_cmd()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/8
****************************************************************************************/
static uint8_t cmp_ec600_serv_conn_cmd(void)
{
    char send_buff[96];
    
    rt_thread_mdelay(3000);
    if(cmp_ec600_send_cmd("AT+QIACT=1\r\n","OK\r\n","ERROR",RT_NULL,RT_NULL,0,30000)>0)
    {
	ec600_conf.procc.reason = EC600_REASON_QIACT_TMOUT;
	return 1;
    }
    
    rt_thread_mdelay(500);
    memset((uint8_t*)send_buff,0,sizeof(send_buff));
    sprintf(send_buff,"AT+QIOPEN=1,0,\"TCP\",\"%s\",%d,0,2\r\n",ec600_conf.param.serv_addr,ec600_conf.param.port);
    if(cmp_ec600_send_cmd(send_buff,"CONNECT","ERROR",RT_NULL,RT_NULL,0,15000)>0)
    {
	ec600_conf.procc.reason = EC600_REASON_QIOPEN_TMOUT;
	return 2;
    }
    
    return 0;
}

/***************************************************************************************
* @Function    : cmp_ec600_exit_spp_cmd()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/8
****************************************************************************************/
static uint8_t cmp_ec600_exit_spp_cmd(void)
{
    if(cmp_ec600_send_cmd("AT+QIDEACT=1\r\n","OK\r\n",RT_NULL,RT_NULL,RT_NULL,0,15000)>0)
    {
	return 1;
    }
    return 0;
}

/***************************************************************************************
* @Function    : cmp_ec600_sta_work_reset()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/8
****************************************************************************************/
static void cmp_ec600_sta_work_reset(void)
{
    if(cmp_ec600_hardware_reset_cmd() == 0)
    {
	ec600_conf.procc.useable = 1;
	ec600_conf.procc.state = EC600_STA_PARAM_QUERY;
	return;
    }
    rt_thread_mdelay(10000);
}

/***************************************************************************************
* @Function    : cmp_ec600_sta_work_param_query()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/8
****************************************************************************************/
static void cmp_ec600_sta_work_param_query(void)
{
    uint8_t try_cnt = 10;
    
    while(try_cnt--)
    {
	if(cmp_ec600_param_query_cmd()==0)
	{
	    ec600_conf.procc.state = EC600_STA_SERV_CONN;
	    return;
	}
	rt_thread_mdelay(5000);
    }
    ec600_conf.procc.state = EC600_STA_RESET;
}

/***************************************************************************************
* @Function    : cmp_ec600_sta_work_serv_conn()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/8
****************************************************************************************/
static void cmp_ec600_sta_work_serv_conn(void)
{
    if(cmp_ec600_serv_conn_cmd() == 0)
    {
	ec600_conf.procc.reason = EC600_REASON_OK;
	ec600_conf.procc.state = EC600_STA_SPP;
	return;
    }
    ec600_conf.procc.state = EC600_STA_RESET;
}

/***************************************************************************************
* @Function    : cmp_ec600_sta_work_spp()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/8
****************************************************************************************/
static void cmp_ec600_sta_work_spp(void)
{
    drv_serial_rx_stu rx_msg;
    
    ec600_conf.procc.useable = 3;
    if(ec600_conf.serial->dev_get_msg(&rx_msg,0) != RT_EOK) return;
    if(rx_msg.len == 0) return;
    if(strstr((char*)rx_msg.buff,"NO CARRIER") != RT_NULL)
    {
	cmp_ec600_exit_spp_cmd();
	ec600_conf.procc.state = EC600_STA_SERV_CONN;
    }
}


/***************************************************************************************
* @Function    : cmp_ec600_sta_work()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/8
****************************************************************************************/
static void cmp_ec600_sta_work(void)
{
    switch(ec600_conf.procc.state)
    {
    case EC600_STA_RESET:
	cmp_ec600_sta_work_reset();
	break;
    case EC600_STA_PARAM_QUERY:
	cmp_ec600_sta_work_param_query();
	break;
    case EC600_STA_SERV_CONN:
	cmp_ec600_sta_work_serv_conn();
	break;
    case EC600_STA_SPP:
	cmp_ec600_sta_work_spp();
	break;
    }
}

/***************************************************************************************
* @Function    : cmp_ec600_reset_init()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/8
****************************************************************************************/
static uint8_t cmp_ec600_reset_init(void)
{
    ec600_conf.reset = drv_dev_find(DRV_DEV_DIDO_ESP_RESET);
    
    if(ec600_conf.reset == RT_NULL) return 1;
    
    return 0;
}

/***************************************************************************************
* @Function    : cmp_ec600_param_init()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/8
****************************************************************************************/
static void cmp_ec600_param_init(void)
{
    ec600_conf.param.serv_addr = RT_NULL;
}

/***************************************************************************************
* @Function    : cmp_ec600_get_status()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/8
****************************************************************************************/
static uint8_t cmp_ec600_get_status(void)
{
    return ec600_conf.procc.status;
}

/***************************************************************************************
* @Function    : cmp_ec600_set_serial()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/8
****************************************************************************************/
static void cmp_ec600_set_serial(drv_serial_dev_stu* serial)
{
    ec600_conf.serial = serial;
}

/***************************************************************************************
* @Function    : cmp_ec600_work()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/8
****************************************************************************************/
static void cmp_ec600_work(uint8_t period)
{
    if(ec600_conf.procc.status > 0) return;
    
    if(ec600_conf.serial == RT_NULL) return;
    
    if(ec600_conf.param.serv_addr == RT_NULL) return;
    
    cmp_ec600_sta_work();
}

/***************************************************************************************
* @Function    : cmp_ec600_set_param()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/8
****************************************************************************************/
static void cmp_ec600_set_param(cmp_ec600_param_stu* param)
{
    if(param == RT_NULL) return;
    memcpy((uint8_t*)&ec600_conf.param,(uint8_t*)param,sizeof(cmp_ec600_param_stu));
}

/***************************************************************************************
* @Function    : cmp_ec600_get_useable_state()
*
* @Param       : 
*
* @Return      : RT_EOK: find device  RT_EEMPTY: have not find device
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/9
****************************************************************************************/
static uint8_t cmp_ec600_get_useable_state(void)
{
   if(cmp_ec600_hardware_reset_cmd() == 0) return RT_EOK;
   
   return RT_EEMPTY;
}

/***************************************************************************************
* @Function    : cmp_ec600_dev_init()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/8
****************************************************************************************/
static void cmp_ec600_dev_init(void)
{
    static cmp_ec600_dev_stu ec600_dev;
    
    ec600_dev.ec600_get_status = cmp_ec600_get_status;
    ec600_dev.ec600_set_serial = cmp_ec600_set_serial;
    ec600_dev.ec600_work = cmp_ec600_work;
    ec600_dev.ec600_set_param = cmp_ec600_set_param;
    ec600_dev.ec600_get_useable_state = cmp_ec600_get_useable_state;
    
    cmp_dev_append(CMP_DEV_NAME_EC600,&ec600_dev);
}

/***************************************************************************************
* @Function    : cmp_ec600_dev_create()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/8
****************************************************************************************/
static int cmp_ec600_dev_create(void)
{
    if(cmp_ec600_reset_init() > 0)
    {
	ec600_conf.procc.status = 1;
    }
    
    cmp_ec600_param_init();
    
    cmp_ec600_dev_init();
    
    return 0;
}
INIT_ENV_EXPORT(cmp_ec600_dev_create);