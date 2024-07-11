#include "cmp_cc.h"

static cmp_cc_stu cc_conf[BSP_POLE_PLUG_NUM];

/***************************************************************************************
* @Function    : cmp_plug_cc_status_conv()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/12
****************************************************************************************/
static void cmp_cc_status_conv(cmp_cc_stu* cc)
{
    uint16_t tmp_val;
    
    tmp_val = cc->procc.cc_val;
    
    if(tmp_val < CMP_CC_TH)
    {
	if(cc->procc.sta_pre != 1)
	{
	    cc->procc.cc_count = 0;
	}
	cc->procc.sta_pre = 1;
    }
    else
    {
	if(cc->procc.sta_pre != 0)
	{
	    cc->procc.cc_count = 0;
	}
	cc->procc.sta_pre = 0;
    }
    
    if(cc->procc.cc_count < 5)
    {
	cc->procc.cc_count++;
	return;
    }
    cc->procc.sta = cc->procc.sta_pre;
}

/***************************************************************************************
* @Function    : cmp_plug_cc_work()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/12
****************************************************************************************/
static void cmp_plug_cc_work(cmp_cc_stu* cc)
{
    cc->procc.cc_val = cc->cc_dev->drv_analog_get_real_val();
    cmp_cc_status_conv(cc);
}

/***************************************************************************************
* @Function    : cmp_cc_work0()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/8/22
****************************************************************************************/
static void cmp_cc_work0(void)
{
    cmp_plug_cc_work(&cc_conf[0]);
}

/***************************************************************************************
* @Function    : cmp_cc_get_sta0()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/8/22
****************************************************************************************/
static uint8_t cmp_cc_get_sta0(void)
{
    return cc_conf[0].procc.sta;
}

/***************************************************************************************
* @Function    : cmp_cc_conf_init()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/8/22
****************************************************************************************/
static void cmp_cc_conf_init(void)
{
    cc_conf[0].cc_dev = drv_dev_find(DRV_DEV_ANALOG_CC);
}

/***************************************************************************************
* @Function    : cmp_cc_local_dev_create()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/8/22
****************************************************************************************/
static void cmp_cc_local_dev_create(void)
{
    static cmp_cc_dev_stu cc_dev[BSP_POLE_PLUG_NUM];
    
    cc_dev[0].cmp_dev_work = cmp_cc_work0;
    cc_dev[0].cmp_get_sta =  cmp_cc_get_sta0;
    
    cmp_dev_append(CMP_DEV_NAME_CC,cc_dev);
}

/***************************************************************************************
* @Function    : cmp_cc_dev_create()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/8/22
****************************************************************************************/
static int cmp_cc_dev_create(void)
{
    cmp_cc_conf_init();
    
    cmp_cc_local_dev_create();
    
    return 0;
}
INIT_ENV_EXPORT(cmp_cc_dev_create);