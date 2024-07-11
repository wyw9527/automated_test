#include "cmp_mite.h"

#define CMP_MITE_MSG_HEAD 	0x02
#define CMP_MITE_MSG_TRAIL	0x03

#define CMP_MITE_BUZZ_SUCC_TIME 500
#define CMP_MITE_BUZZ_FAIL_TIME 70
#define CMP_MITE_BUZZ_TIP_TIME  40

#define CMP_MITE_VERSION_CMD 		0x31
#define CMP_MITE_VERSION_CMD_PARAM 	0x11
#define CMP_MITE_ACTIVE_CMD 		0x34
#define CMP_MITE_ACTIVE_CMD_PARAM 	0x30
#define CMP_MITE_UID_CMD 		0x34
#define CMP_MITE_UID_CMD_PARAM 		0x31
#define CMP_MITE_BUZZ_CMD 		0x31
#define CMP_MITE_BUZZ_CMD_PARAM 	0x3e

static cmp_mite_stu mite_conf;

/***************************************************************************************
* @Function    : cmp_mite_send_msg()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/9
****************************************************************************************/
static void cmp_mite_send_msg(uint8_t cmd,uint8_t cmd_param,uint8_t* data,uint8_t data_len)
{
    uint8_t pos = 0;
    uint8_t mite_buff[64];
    
    mite_buff[pos++] = CMP_MITE_MSG_HEAD;
    mite_buff[pos++] = 0;
    mite_buff[pos++] = data_len + 2;
    mite_buff[pos++] = cmd;
    mite_buff[pos++] = cmd_param;
    if(data != RT_NULL && data_len > 0)
    {
	memcpy(&mite_buff[pos],data,data_len);
	pos += data_len;
    }
    mite_buff[pos++] = CMP_MITE_MSG_TRAIL;
    mite_buff[pos] = xor_check(mite_buff,pos);
    pos++;
    
    if(mite_conf.serial == RT_NULL)
    {
	return;
    }
    
    mite_conf.serial->dev_send_msg(mite_buff,pos);
    if(++mite_conf.comm_count > 10)
    {
	mite_conf.comm_fail = 1;
    }
    else
    {
	mite_conf.comm_fail = 0;
    }
}

/***************************************************************************************
* @Function    : cmp_mite_verison_query_msg()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/9
****************************************************************************************/
static void cmp_mite_verison_query_msg(void)
{
    cmp_mite_send_msg(CMP_MITE_VERSION_CMD,CMP_MITE_VERSION_CMD_PARAM,RT_NULL,0);
}

/***************************************************************************************
* @Function    : cmp_mite_card_active_msg()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/9
****************************************************************************************/
static void cmp_mite_card_active_msg(void)
{
    cmp_mite_send_msg(CMP_MITE_ACTIVE_CMD,CMP_MITE_ACTIVE_CMD_PARAM,RT_NULL,0);
}

/***************************************************************************************
* @Function    : cmp_mite_card_uid_msg()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/9
****************************************************************************************/
static void cmp_mite_card_uid_msg(void)
{
    cmp_mite_send_msg(CMP_MITE_UID_CMD,CMP_MITE_UID_CMD_PARAM,RT_NULL,0);
}

/***************************************************************************************
* @Function    : cmp_mite_buzz_ctrl_msg()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/9
****************************************************************************************/
static void cmp_mite_buzz_ctrl_msg(CMP_MITE_BUZZ_TYPE buzz_t)
{
    uint8_t buff[3];
    uint8_t times;
    uint16_t continues;
    
    if(buzz_t == MITE_BUZZ_TIP)
    {
	times = 3;
	continues = CMP_MITE_BUZZ_TIP_TIME;
    }
    else if(buzz_t == MITE_BUZZ_FAIL)
    {
	times = 2;
	continues = CMP_MITE_BUZZ_FAIL_TIME;
    }
    else if(buzz_t == MITE_BUZZ_SUCC)
    {
	times = 1;
	continues = CMP_MITE_BUZZ_SUCC_TIME;
    }
    buff[0] = times;
    buff[1] = continues >> 8;
    buff[2] = continues;
    
    cmp_mite_send_msg(CMP_MITE_BUZZ_CMD,CMP_MITE_BUZZ_CMD_PARAM,buff,sizeof(buff));
}

/***************************************************************************************
* @Function    : cmp_mite_hnd_rx_msg()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/9
****************************************************************************************/
static uint8_t cmp_mite_hnd_rx_msg(cmp_mite_rx_data_stu* rx_data,uint16_t tmout)
{
    uint8_t* buff_p;
    uint8_t msg_len,cnt = 10;
    drv_serial_rx_stu rx_msg;
    
    if(mite_conf.serial == RT_NULL) return 0;
    
    if(mite_conf.serial->dev_get_msg(&rx_msg,tmout) != RT_EOK) return RT_EEMPTY;
   
    buff_p = rx_msg.buff;
    while(rx_msg.len > 7 && cnt > 0)
    {
	cnt--;
	if(buff_p[0] != CMP_MITE_MSG_HEAD)
	{
	    buff_p++;
	    rx_msg.len--;
	    continue;
	}
	
	msg_len = buff_p[1] *256 + buff_p[2];
	if(msg_len > rx_msg.len -4)
	{
	    buff_p += 3;
	    rx_msg.len -= 3;
	    continue;
	}
	
	if(buff_p[msg_len+3] != CMP_MITE_MSG_TRAIL)
	{
	    if(rx_msg.len > 7)
	    {
		buff_p += 7;
		rx_msg.len -= 7;
		continue;
	    }
	    return 0;
	}
	
	if(buff_p[msg_len+4] != xor_check(buff_p,msg_len+4))
	{
	    if(rx_msg.len > 8)
	    {
		buff_p += 8;
		rx_msg.len -= 8;
		continue;
	    }
	    return 0;
	}
	
	memcpy(&rx_data->cmd,&buff_p[3],msg_len);
	rx_data->data_len = msg_len - 3;
	mite_conf.comm_count = 0;
	return RT_EOK;
    }
    
    return RT_EEMPTY;
}

/***************************************************************************************
* @Function    : cmp_mite_get_dst_msg()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/10
****************************************************************************************/
static uint8_t cmp_mite_get_dst_msg(CMP_MITE_RX_MSG_TYPE dst,uint8_t* ret,uint8_t ret_len,uint16_t tmout)
{
    cmp_mite_rx_data_stu rx_data;
    
    if(cmp_mite_hnd_rx_msg(&rx_data,tmout) != RT_EOK) return RT_EEMPTY;
    
    switch(dst)
    {
    case MITE_RX_MSG_VERSION_QUERY:
	if(rx_data.cmd == CMP_MITE_VERSION_CMD && rx_data.cmd_param == CMP_MITE_VERSION_CMD_PARAM)
	{
	    return RT_EOK;
	}
	break;
	
    case MITE_RX_MSG_CARD_ATCTIVE:
	if(rx_data.cmd == CMP_MITE_ACTIVE_CMD && rx_data.cmd_param == CMP_MITE_ACTIVE_CMD_PARAM)
	{
	    if(ret!= RT_NULL)
	    {
		*ret = rx_data.status;
	    }
	    return RT_EOK;
	}
	break;
	
    case MITE_RX_MSG_CARD_UID:
	if(rx_data.cmd == CMP_MITE_UID_CMD && rx_data.cmd_param == CMP_MITE_UID_CMD_PARAM)
	{
	    if(ret != RT_NULL && ret_len >0)
		memcpy(ret,rx_data.data,ret_len);
	    return RT_EOK;
	}
	break;
	
    case MITE_RX_MSG_BUZZ_CTRL:
	if(rx_data.cmd == CMP_MITE_BUZZ_CMD && rx_data.cmd_param == CMP_MITE_BUZZ_CMD_PARAM)
	{
	    return RT_EOK;
	}
	break;
    }
    return RT_EEMPTY;
}

/***************************************************************************************
* @Function    : cmp_mite_version_action()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/10
****************************************************************************************/
static uint8_t cmp_mite_version_action(void)
{
    cmp_mite_verison_query_msg();
    
    return cmp_mite_get_dst_msg(MITE_RX_MSG_VERSION_QUERY,RT_NULL,0,500);
}

/***************************************************************************************
* @Function    : cmp_mite_uid_action()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/10
****************************************************************************************/
static uint8_t cmp_mite_uid_action(uint32_t* uid)
{
    char active;
    
    cmp_mite_card_active_msg();
    
    if(cmp_mite_get_dst_msg(MITE_RX_MSG_CARD_ATCTIVE,(uint8_t*)&active,0,500) != RT_EOK)
    {
	return RT_EEMPTY;
    }
    
    if(active != 'Y')
    {
	return RT_EEMPTY;
    }
    
    cmp_mite_card_uid_msg();
    
    return cmp_mite_get_dst_msg(MITE_RX_MSG_CARD_UID,(uint8_t*)uid,4,500);
}

/***************************************************************************************
* @Function    : cmp_mite_buzz_action()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/10
****************************************************************************************/
static uint8_t cmp_mite_buzz_action(CMP_MITE_BUZZ_TYPE buzz_t)
{
    cmp_mite_buzz_ctrl_msg(buzz_t);
    
    return cmp_mite_get_dst_msg(MITE_RX_MSG_VERSION_QUERY,RT_NULL,0,500);
}

/***************************************************************************************
* @Function    : cmp_mite_set_serial()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/10
****************************************************************************************/
static void cmp_mite_set_serial(drv_serial_dev_stu* serial)
{
    mite_conf.serial = serial;
}

/***************************************************************************************
* @Function    : cmp_get_comm_fail_sta()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/10
****************************************************************************************/
static uint8_t cmp_get_comm_fail_sta(void)
{
    return mite_conf.comm_fail;
}

/***************************************************************************************
* @Function    : cmp_mite_dev_create()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/10
****************************************************************************************/
static int cmp_mite_dev_create(void)
{
    static cmp_mite_dev_stu mite_dev;
    
    mite_dev.mite_buzz_action = cmp_mite_buzz_action;
    mite_dev.mite_set_serial = cmp_mite_set_serial;
    mite_dev.mite_uid_action = cmp_mite_uid_action;
    mite_dev.mite_version_action = cmp_mite_version_action;
    mite_dev.mite_get_comm_fail_sta = cmp_get_comm_fail_sta;
     
    cmp_dev_append(CMP_DEV_NAME_MITE,&mite_dev);
    return 0;
}
INIT_ENV_EXPORT(cmp_mite_dev_create);
