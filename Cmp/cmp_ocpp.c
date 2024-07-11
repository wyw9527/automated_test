#include "cmp_ocpp.h"

static cmp_prot_ocpp_stu ocpp_conf;


/***************************************************************************************
* @Function    : cmp_prot_ocpp_pack_and_tx_msg()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/12
****************************************************************************************/
static void cmp_prot_ocpp_pack_and_tx_msg(cmp_prot_ocpp_stu* prot,uint8_t id,uint8_t* data,uint16_t len)
{
    uint8_t buff[512],pos=0,tx_len;
    uint16_t crc16;
    
    tx_len = len + 6;
    buff[pos++] = 0x5E;
    buff[pos++] = tx_len >> 8;
    buff[pos++] = tx_len;
    buff[pos++] = id;
    memcpy(&buff[pos],data,len);
    pos += len;
    
    crc16 = crc16_calc(buff,pos);
    buff[pos++] = crc16 >> 8;
    buff[pos++] = crc16;
    
    if(prot->pub_dev.serial != RT_NULL)
    {
	prot->pub_dev.serial->dev_send_msg(buff,pos);
    }
}

/***************************************************************************************
* @Function    : cmp_prot_ocpp_pase_init_msg()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/12
****************************************************************************************/
static void cmp_prot_ocpp_pase_init_msg(cmp_prot_ocpp_stu* prot,uint8_t* msg)
{
    cmp_prot_ocpp_rx_init_stu init_msg;
    
    memcpy((uint8_t*)&init_msg,msg,sizeof(init_msg));
    if(init_msg.sim_len >24 || init_msg.ver_len > 16)
    {
	return;
    }
    
    memset((uint8_t*)&ocpp_conf.ocpp_info.init,0,sizeof(ocpp_conf.ocpp_info.init));
    memcpy(ocpp_conf.ocpp_info.init.ppm_ver,init_msg.verion,init_msg.ver_len);
    memcpy(ocpp_conf.ocpp_info.init.ppm_sim_no,init_msg.sim,init_msg.sim_len);
    
    prot->tx_ctrl[OCPP_MSG_INIT].tx_flag = 1;
    prot->procc.ocpp_sta = 1;
}

/***************************************************************************************
* @Function    : cmp_prot_ocpp_pase_network_msg()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/13
****************************************************************************************/
static void cmp_prot_ocpp_pase_network_msg(cmp_prot_ocpp_stu* prot,uint8_t* msg)
{
    if(prot->procc.ocpp_sta != 1) return;
    
    prot->procc.ocpp_sta = 2;
}

/***************************************************************************************
* @Function    : cmp_prot_ocpp_pase_serv_conn_msg()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/13
****************************************************************************************/
static void cmp_prot_ocpp_pase_serv_conn_msg(cmp_prot_ocpp_stu* prot,uint8_t* msg)
{
    if(prot->procc.ocpp_sta != 2) return;
    
    prot->procc.ocpp_sta = 3;
}

/***************************************************************************************
* @Function    : cmp_prot_ocpp_pase_hb_msg()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/13
****************************************************************************************/
static void cmp_prot_ocpp_pase_hb_msg(cmp_prot_ocpp_stu* prot,uint8_t* msg)
{
    prot->procc.csms_sta = msg[0];
    
    prot->procc.tcp_sta = msg[1];
}

/***************************************************************************************
* @Function    : cmp_prot_ocpp_pase_unclock_msg()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/13
****************************************************************************************/
static void cmp_prot_ocpp_pase_unclock_msg(cmp_prot_ocpp_stu* prot,uint8_t* msg)
{
    if(msg[0] == 0 || msg[0] > BSP_POLE_PLUG_NUM) return;
    
    prot->procc.cmd_hook_func(OCPP_CMD_UNLOCK,msg[0]-1);
}

/***************************************************************************************
* @Function    : cmp_prot_ocpp_pase_remote_start_msg()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/13
****************************************************************************************/
static void cmp_prot_ocpp_pase_remote_start_msg(cmp_prot_ocpp_stu* prot,uint8_t* msg)
{
    if(msg[0] == 0 || msg[0] > BSP_POLE_PLUG_NUM) return;
    
    prot->procc.cmd_hook_func(OCPP_CMD_REMOTE_START,msg[0]-1);
}

/***************************************************************************************
* @Function    : cmp_prot_ocpp_pase_remote_stop_msg()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/13
****************************************************************************************/
static void cmp_prot_ocpp_pase_remote_stop_msg(cmp_prot_ocpp_stu* prot,uint8_t* msg)
{
    if(msg[0] == 0 || msg[0] > BSP_POLE_PLUG_NUM) return;
    
    prot->procc.cmd_hook_func(OCPP_CMD_REMOTE_STOP,msg[0]-1);
}

/***************************************************************************************
* @Function    : cmp_prot_ocpp_pase_change_avai_msg()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/13
****************************************************************************************/
static void cmp_prot_ocpp_pase_change_avai_msg(cmp_prot_ocpp_stu* prot,uint8_t* msg)
{
    if(msg[0] > 1) return;
    
    prot->procc.cmd_hook_func(OCPP_CMD_CHANGE_AVAI,msg[0]);
}

/***************************************************************************************
* @Function    : cmp_prot_ocpp_pase_reset_msg()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/13
****************************************************************************************/
static void cmp_prot_ocpp_pase_reset_msg(cmp_prot_ocpp_stu* prot,uint8_t* msg)
{
    prot->procc.cmd_hook_func(OCPP_CMD_RESET,0);
}

/***************************************************************************************
* @Function    : cmp_prot_ocpp_pase_chrg_limit_msg()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/13
****************************************************************************************/
static void cmp_prot_ocpp_pase_chrg_limit_msg(cmp_prot_ocpp_stu* prot,uint8_t* msg)
{
    uint32_t curr;
    
    if(msg[0] == 0 || msg[0] > BSP_POLE_PLUG_NUM) return;
    if(msg[1] != 1) return;
    curr = msg[2] << 24 | msg[3] << 16 | msg[4] << 8 | msg[5];
    
    prot->procc.cmd_hook_func(OCPP_CMD_CHRG_LIMIT,curr);
}

/***************************************************************************************
* @Function    : cmp_prot_ocpp_pase_resverve_msg()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/13
****************************************************************************************/
static void cmp_prot_ocpp_pase_resverve_msg(cmp_prot_ocpp_stu* prot,uint8_t* msg)
{
    if(msg[0] == 0 || msg[0] > BSP_POLE_PLUG_NUM) return;
    
    prot->ocpp_info.resv[msg[0]].resv_time = msg[1] << 8 | msg[2];
    prot->ocpp_info.resv[msg[0]].resv_flag = 1;
    
    prot->procc.resv_result = 0;
    prot->tx_ctrl[OCPP_MSG_RESV_CAL].tx_flag = 1;
}

/***************************************************************************************
* @Function    : cmp_prot_ocpp_pase_resv_cal_msg()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/13
****************************************************************************************/
static void cmp_prot_ocpp_pase_resv_cal_msg(cmp_prot_ocpp_stu* prot,uint8_t* msg)
{
    if(msg[0] == 0 || msg[0] > BSP_POLE_PLUG_NUM) return;
    prot->ocpp_info.resv[msg[0]].resv_flag = 0;
    
    prot->procc.resv_result = 0;
    prot->tx_ctrl[OCPP_MSG_RESV_CAL].tx_flag = 1;
}

/***************************************************************************************
* @Function    : cmp_prot_ocpp_load_meter_data()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/13
****************************************************************************************/
static void cmp_prot_ocpp_load_meter_data(cmp_prot_ocpp_stu* prot,uint8_t plug_idx)
{
    uint16_t tmp;
    uint8_t buff[64],pos=0;
    const cmp_measure_dev_stu* measure;
    
    if(plug_idx >= BSP_POLE_PLUG_NUM) return;
    measure = cmp_dev_find(CMP_DEV_NAME_MEASURE);
    if(measure == RT_NULL) return;
    
    for(uint8_t i=0;i<3;i++)
    {
	tmp = measure[plug_idx].cmp_get_chrg_volt(i) * 10;
	buff[pos++] = tmp >>8;
	buff[pos++] = tmp;
    }
    for(uint8_t i=0;i<3;i++)
    {
	tmp = measure[plug_idx].cmp_get_chrg_curr(i) * 10;
	buff[pos++] = tmp >>8;
	buff[pos++] = tmp;
    }
    tmp = measure[plug_idx].cmp_get_chrg_power() * 10;
    buff[pos++] = tmp >>8;
    buff[pos++] = tmp;
    
    tmp = measure[plug_idx].cmp_get_chrg_energy();
    buff[pos++] = tmp >>24;
    buff[pos++] = tmp >>16;
    buff[pos++] = tmp >>8;
    buff[pos++] = tmp;
    
     cmp_prot_ocpp_pack_and_tx_msg(prot,0x41,buff,pos);
}

/***************************************************************************************
* @Function    : cmp_prot_ocpp_pase_meter_msg()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/13
****************************************************************************************/
static void cmp_prot_ocpp_pase_meter_msg(cmp_prot_ocpp_stu* prot,uint8_t* msg)
{
    if(msg[0] == 0 || msg[0] > BSP_POLE_PLUG_NUM) return;
    
    cmp_prot_ocpp_load_meter_data(prot,msg[0]);
}

/***************************************************************************************
* @Function    : cmp_prot_ocpp_pase_auth_msg()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/14
****************************************************************************************/
static void cmp_prot_ocpp_pase_auth_msg(cmp_prot_ocpp_stu* prot,uint8_t* msg)
{
    bsp_int_union_stu tmp;
    
    if(msg[0] == 0 || msg[0] > BSP_POLE_PLUG_NUM) return;
    
    if(msg[1] > 0) return;
    
    memcpy(tmp.eles.byte,msg,4);
    prot->procc.cmd_hook_func(OCPP_CMD_CARD_AUTH,tmp.val);
}

/***************************************************************************************
* @Function    : cmp_prot_ocpp_pase_transp_msg()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/14
****************************************************************************************/
static void cmp_prot_ocpp_pase_transp_msg(cmp_prot_ocpp_stu* prot,uint8_t* msg,uint16_t len)
{
    if(prot->procc.ocpp_rx_transparent_func == RT_NULL) return;
    
    prot->procc.ocpp_rx_transparent_func(msg,len);
}

/***************************************************************************************
* @Function    : cmp_prot_ocpp_pase_plug_stat_msg()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/21
****************************************************************************************/
static void cmp_prot_ocpp_pase_plug_stat_msg(cmp_prot_ocpp_stu* prot,uint8_t* msg)
{
    if(msg[0] >= BSP_POLE_PLUG_NUM) return;
    
    prot->tx_ctrl[OCPP_MSG_PLUG_STA].tx_flag = 0;
}

/***************************************************************************************
* @Function    : cmp_prot_ocpp_cmd_pase()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/12
****************************************************************************************/
static void cmp_prot_ocpp_cmd_pase(cmp_prot_ocpp_stu* prot,uint8_t cmd,uint8_t* data,uint16_t len)
{
    switch(cmd)
    {
	/* 接收命令 */
    case 0x30:	cmp_prot_ocpp_pase_init_msg(prot,data);		break;//初始化
    case 0x36:	cmp_prot_ocpp_pase_hb_msg(prot,data);		break;//心跳
    case 0x38:	cmp_prot_ocpp_pase_transp_msg(prot,data,len);	break;//透传
    case 0x3A:	cmp_prot_ocpp_pase_unclock_msg(prot,data);	break;//解锁
    case 0x3C:	cmp_prot_ocpp_pase_remote_start_msg(prot,data);	break;//启动充电
    case 0x3E:	cmp_prot_ocpp_pase_remote_stop_msg(prot,data);	break;//停止充电
    case 0x40:	cmp_prot_ocpp_pase_meter_msg(prot,data);	break;//电表数据
    case 0x42:	cmp_prot_ocpp_pase_change_avai_msg(prot,data);	break;//改变可用性
    case 0x44:	cmp_prot_ocpp_pase_reset_msg(prot,data);	break;//复位
    case 0x46:	cmp_prot_ocpp_pase_resverve_msg(prot,data);	break;//预约
    case 0x48:	cmp_prot_ocpp_pase_chrg_limit_msg(prot,data);	break;//充电限制
    case 0x4A:	cmp_prot_ocpp_pase_resv_cal_msg(prot,data);	break;//取消预定
    case 0x4C:							break;//固件状态通知
    case 0x4E:							break;//改变配置
    
    /* 接收回复 */
    case 0x60:	cmp_prot_ocpp_pase_network_msg(prot,data);	break;//网络配置回复
    case 0x62:	cmp_prot_ocpp_pase_serv_conn_msg(prot,data);	break;//服务器连接回复
    case 0x64:	cmp_prot_ocpp_pase_auth_msg(prot,data);		break;//认证回复
    case 0x66:	cmp_prot_ocpp_pase_remote_start_msg(prot,data);	break;//开始充电
    case 0x68:							break;//停止充电
    case 0x6A:	cmp_prot_ocpp_pase_plug_stat_msg(prot,data);	break;						break;//状态通知
    case 0x6C:							break;//ATE
    case 0x6E:							break;//ppm 固件更新
    default:return;
    }
    
    prot->procc.hb_count = 0;
    prot->procc.comm_sta = 1;
}

/***************************************************************************************
* @Function    : cmp_ocpp_analy_msg()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/12
****************************************************************************************/
static void cmp_ocpp_analy_msg(uint8_t* msg,uint16_t len)
{
    uint16_t msg_len,crc16_cal,crc16_rx;
    
    while(len >= 6)
    {
	if(msg[0] != 0x5E)
	{
	    len--;
	    msg++;
	    continue;
	}
	
	msg_len = msg[1] * 256 + msg[2];
	if(msg_len > len) return;
	
	crc16_cal = crc16_calc(msg,msg_len-2);
	crc16_rx = msg[msg_len - 2] * 256 + msg[msg_len - 1];
	if(crc16_rx == crc16_cal)
	{
	    cmp_prot_ocpp_cmd_pase(&ocpp_conf,msg[3],&msg[4],msg_len-6);
	}
	len -= msg_len;
	msg += msg_len;
    }
}


/***************************************************************************************
* @Function    : cmp_prot_ocpp_init_load_data()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/12
****************************************************************************************/
static void cmp_prot_ocpp_init_load_data(cmp_prot_ocpp_stu* prot)
{
    uint8_t buff[128],pos=0;
    
    /* soft verison */
    buff[pos++] = 3;//cpFirmwareVersionLen
    buff[pos++] = PROT_SOFT_VER_MAIN + 0x30;
    buff[pos++] = '.';
    buff[pos++] = PROT_SOFT_VER_SEC + 0x30;
    /* pole sn */
    const char* sn =(char*)prot->pub_dev.store->param->store_parameter_query(1);
    buff[pos++] = strlen(sn);
    memcpy(&buff[pos],sn,buff[pos-1]);
    pos += buff[pos-1];
    /* vendor*/
    char* vendor = "foxess";
    buff[pos++] = strlen(vendor);
    memcpy(&buff[pos],vendor,buff[pos-1]);
    pos += buff[pos-1];
    /* model */
    char* model ="A011KS1-E5-R2";
    buff[pos++] = strlen(model);
    memcpy(&buff[pos],model,buff[pos-1]);
    pos += buff[pos-1];
    
    /* meter */
    buff[pos++] = 0;
    /* meterType */
    buff[pos++] = 0;
    /* numberOfConnectors */
    buff[pos++] = 1;
    /* numberPhases */
    buff[pos++] = 1;
    /* ratedAcPhaseVoltage */
    buff[pos++] = 230;
    /* maxChargingLimitUnit */
    buff[pos++] = 1;//A
    /* maxChargingLimit */
    buff[pos++] = 0x00;
    buff[pos++] = 0x00;
    buff[pos++] = 320>>8;
    buff[pos++] = 320 & 0xff;
    
    cmp_prot_ocpp_pack_and_tx_msg(prot,0x31,buff,pos);
}

/***************************************************************************************
* @Function    : cmp_prot_ocpp_net_load_data()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/12
****************************************************************************************/
static void cmp_prot_ocpp_net_load_data(cmp_prot_ocpp_stu* prot)
{
    uint8_t buff[128],pos=0;
    
    /* network type */
    buff[pos++] = prot->param.net_type;
    /* wifiSsid */
    const char* ssid = (char*)prot->pub_dev.store->param->store_parameter_query(4);
    buff[pos++] = strlen(ssid);
    memcpy(&buff[pos],ssid,buff[pos-1]);
    pos += buff[pos-1];
    /* wifiSecurityType */
    buff[pos++] = prot->param.wifi_secuity_type;
    /* wifi password */
    const char* wifi_pwd = (char*)prot->pub_dev.store->param->store_parameter_query(5);
    buff[pos++] = strlen(wifi_pwd);
    memcpy(&buff[pos],wifi_pwd,buff[pos-1]);
    pos += buff[pos-1];
    /* lteApn */
    buff[pos++] = 0;
    /* lteUser */
    buff[pos++] = 0;
    /* lte password */
    buff[pos++] = 0;
    /* lteAuthentication */
    buff[pos++] = 0;
    /* dhcpEnabled */
    buff[pos++] = 1;
    /* ip */
    buff[pos++] = 192;
    buff[pos++] = 168;
    buff[pos++] = 1;
    buff[pos++] = 1;
    /* subnetMask */
    buff[pos++] = 255;
    buff[pos++] = 255;
    buff[pos++] = 255;
    buff[pos++] = 1;
    /* getway */
    buff[pos++] = 255;
    buff[pos++] = 255;
    buff[pos++] = 255;
    buff[pos++] = 1;
    /* dns */
    buff[pos++] = 0;
    buff[pos++] = 0;
    buff[pos++] = 0;
    buff[pos++] = 1;
    
    cmp_prot_ocpp_pack_and_tx_msg(prot,0x61,buff,pos);
}

/***************************************************************************************
* @Function    : cmp_prot_ocpp_serv_load_data()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/12
****************************************************************************************/
static void cmp_prot_ocpp_serv_load_data(cmp_prot_ocpp_stu* prot)
{
    uint8_t buff[256],pos=0;
    
    /* tcpServerHost */
    const char* ip = (char*)prot->pub_dev.store->param->store_parameter_query(6);
    buff[pos++] = strlen(ip);
    memcpy(&buff[pos],ip,buff[pos-1]);
    pos += buff[pos-1];
    
    /* tcpServerPort */
    uint16_t port = *(uint16_t*)prot->pub_dev.store->param->store_parameter_query(7);
    buff[pos++] = port>>8;
    buff[pos++] = port;
    
    /* url */
    char* url = "ws://ocpp-toolkit-api.monta.app";
    buff[pos++] = strlen(url);
    memcpy(&buff[pos],url,buff[pos-1]);
    pos += buff[pos-1];
    
    /* securityProfile */
    buff[pos++] = 0;
    /* authorizationKey */
    buff[pos++] = 0;
    /* authorization */
    buff[pos++] = 0;
    /* cpoName */
    buff[pos++] = 0;
    
    cmp_prot_ocpp_pack_and_tx_msg(prot,0x63,buff,pos);
}

/***************************************************************************************
* @Function    : cmp_prot_ocpp_hb_load_data()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/13
****************************************************************************************/
static void cmp_prot_ocpp_hb_load_data(cmp_prot_ocpp_stu* prot)
{
    uint8_t buff[6],pos=0;
    
    /* TcpTimeout */
    buff[pos++] = prot->procc.comm_sta;
    /* Initialized */
    buff[pos++] = prot->procc.ocpp_sta >0?1:0;
    
    cmp_prot_ocpp_pack_and_tx_msg(prot,0x37,buff,pos);
}

/***************************************************************************************
* @Function    : cmp_prot_ocpp_resv_repy()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/13
****************************************************************************************/
static void cmp_prot_ocpp_resv_repy(cmp_prot_ocpp_stu* prot,uint8_t cmd)
{
    uint8_t buff[6],pos=0;
    
    /* TcpTimeout */
    buff[pos++] = prot->procc.resv_result;
    
    cmp_prot_ocpp_pack_and_tx_msg(prot,cmd,buff,pos);
}

/***************************************************************************************
* @Function    : cmp_prot_ocpp_cmd_repy()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/13
****************************************************************************************/
static void cmp_prot_ocpp_cmd_repy(cmp_prot_ocpp_stu* prot,uint8_t cmd,uint8_t result)
{
    uint8_t buff[6],pos=0;
    
    buff[pos++] = result;
    
    cmp_prot_ocpp_pack_and_tx_msg(prot,cmd,buff,pos);
}

/***************************************************************************************
* @Function    : cmp_prot_ocpp_plug_stat_tx()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/21
****************************************************************************************/
static void cmp_prot_ocpp_plug_stat_tx(cmp_prot_ocpp_stu* prot,uint8_t cmd)
{
    uint8_t buff[16],pos=0;
    
    for(uint8_t idx=0;idx<BSP_POLE_PLUG_NUM;idx++)
    {
	buff[pos++] = idx + 1;
	buff[pos++] = prot->procc.tx_status[idx].plug_stat;
	buff[pos++] = prot->procc.tx_status[idx].error_code;
	
	cmp_prot_ocpp_pack_and_tx_msg(&ocpp_conf,cmd,buff,pos);
	pos = 0;
    }
}

/***************************************************************************************
* @Function    : cmp_prot_ocpp_plug_stat_tx()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/22
****************************************************************************************/
static void cmp_prot_ocpp_tranction_tx(uint8_t plug_idx,uint8_t tranc_type)
{
    uint8_t buff[16],pos=0;
    uint32_t tmp;
    const cmp_measure_dev_stu* measure;
    if(plug_idx >= BSP_POLE_PLUG_NUM) return;
    
    buff[0] = plug_idx;
    measure = cmp_dev_find(CMP_DEV_NAME_MEASURE);
    if(measure == RT_NULL) return;
    tmp = measure[plug_idx].cmp_get_chrg_energy();
    buff[pos++] = tmp >>24;
    buff[pos++] = tmp >>16;
    buff[pos++] = tmp >>8;
    buff[pos++] = tmp;
    
    if(tranc_type == 1)
    {
	buff[pos++] = 0x07;
	cmp_prot_ocpp_pack_and_tx_msg(&ocpp_conf,0x69,buff,pos);
    }
    else
    {
	cmp_prot_ocpp_pack_and_tx_msg(&ocpp_conf,0x67,buff,pos);
    }
}

/***************************************************************************************
* @Function    : cmp_prot_ocpp_load_msg_data()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/12
****************************************************************************************/
static void cmp_prot_ocpp_load_msg_data(cmp_prot_ocpp_stu* prot,CMP_PROT_OCPP_MSG_TYPE msg_type)
{
    switch(msg_type)
    {
    case OCPP_MSG_INIT:		cmp_prot_ocpp_init_load_data(prot);	break;
    case OCPP_MSG_NET:		cmp_prot_ocpp_net_load_data(prot);	break;
    case OCPP_MSG_SERV:		cmp_prot_ocpp_serv_load_data(prot);	break;
    case OCPP_MSG_HB:		cmp_prot_ocpp_hb_load_data(prot);	break;
    case OCPP_MSG_RESV:		cmp_prot_ocpp_resv_repy(prot,0x47);	break;
    case OCPP_MSG_RESV_CAL:	cmp_prot_ocpp_resv_repy(prot,0x4B);	break;
    case OCPP_MSG_PLUG_STA:	cmp_prot_ocpp_plug_stat_tx(prot,0x6B);	break;
    }
    
}

/***************************************************************************************
* @Function    : cmp_prot_ocpp_tx_cyc_work()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/12
****************************************************************************************/
static void cmp_prot_ocpp_tx_cyc_work(cmp_prot_ocpp_stu* prot,uint8_t period)
{
    for(uint8_t i=0;i<OCPP_MSG_NUM;i++)
    {
	if(prot->tx_ctrl[i].tx_flag == 0) continue;
	cmp_prot_ocpp_load_msg_data(prot,(CMP_PROT_OCPP_MSG_TYPE)i);
	prot->tx_ctrl[i].tx_flag = 0;
	return;
    }
}

/***************************************************************************************
* @Function    : cmp_ocpp_tx_ctrl_config_work()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/12
****************************************************************************************/
static void cmp_ocpp_tx_ctrl_config_work(cmp_prot_ocpp_stu* prot,uint8_t period)
{
    uint16_t period_count;
    CMP_PROT_OCPP_MSG_TYPE msg_ty;
    
    if(prot->procc.ocpp_sta ==0)
    {
	prot->tx_ctrl[OCPP_MSG_INIT].tx_flag = 0;
	prot->tx_ctrl[OCPP_MSG_NET].tx_flag = 0;
	prot->tx_ctrl[OCPP_MSG_SERV].tx_flag = 0;
	return;
    }
    
    /* send net msg */
    if(prot->procc.ocpp_sta == 1)
    {
	period_count = 1000;
	msg_ty = OCPP_MSG_NET;
    }
    /* send server msg */
    else if(prot->procc.ocpp_sta == 2)
    {
	period_count = 1000;
	msg_ty = OCPP_MSG_SERV;
    }
    else
    {
	period_count = 5000;
	msg_ty = OCPP_MSG_HB;
    }
    
    if(prot->tx_ctrl[msg_ty].tx_flag == 1) return;
    
    if(prot->procc.ocpp_count < period_count)
    {
	prot->procc.ocpp_count += period;
	return;
    }
    prot->procc.ocpp_count = 0;
    prot->tx_ctrl[msg_ty].tx_flag = 1;
}

/***************************************************************************************
* @Function    : cmp_ocpp_tx_plug_stat_work()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/21
****************************************************************************************/
static void cmp_ocpp_tx_plug_stat_work(cmp_prot_ocpp_stu* prot,uint8_t period)
{
    uint8_t sta,err=6,cnt,buff[128];
    bsp_int_union_stu* err_p;
    
    if(prot->procc.ocpp_sta < 3)
    {
	return;
    }
    
    
    if(prot->procc.plug_sta_count < 1000)
    {
	prot->procc.plug_sta_count += period;
	return;
    }
    prot->procc.plug_sta_count = 0;
    
    for(uint8_t i=0;i<BSP_POLE_PLUG_NUM;i++)
    {
	if(cmp_get_plug_sta(i,&sta) != RT_EOK) continue;
	
	err_p = (bsp_int_union_stu*)buff;
	cnt = bsp_get_alram_val(buff);
	
	for(uint8_t i=0;i<cnt;i++)
	{
	    if(err_p->eles.byte[1] == 7 && err_p->eles.byte[2] ==0)
	    {
		err = 0;
		break;
	    }
	    if(err_p->eles.byte[1] == 6 && err_p->eles.byte[2] ==0)
	    {
		err = 1;
		break;
	    }
	    err = 7;
	}
	
	if(prot->procc.tx_status[i].plug_stat == sta && err == prot->procc.tx_status[i].error_code) continue;
	
	if(sta == 7)
	{
	    sta =8;
	}
	else if(sta ==8)
	{
	    sta =7;
	}
	prot->procc.tx_status[i].plug_stat = sta;
	prot->procc.tx_status[i].error_code = err;
	prot->tx_ctrl[OCPP_MSG_PLUG_STA].tx_flag = 1;
    }
}

/***************************************************************************************
* @Function    : cmp_ocpp_tx_transction_work()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/22
****************************************************************************************/
static void cmp_ocpp_tx_transction_work(cmp_prot_ocpp_stu* prot,uint8_t period)
{
    uint8_t stat;
    static uint8_t pre_sta = 0;/* 0 空闲  1 充电 */
    
    if(cmp_get_plug_uasable_sta(0,&stat) != RT_EOK) return;
    
    if(pre_sta == 0)
    {
	if(stat == 2)
	{
	    cmp_prot_ocpp_tranction_tx(0,0);
	    pre_sta = 1;
	}
    }
    else
    {
	if(stat != 2)
	{
	    cmp_prot_ocpp_tranction_tx(0,1);
	     pre_sta = 0;
	}
    }
}

/***************************************************************************************
* @Function    : cmp_ocpp_tx_ctrl_work()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/12
****************************************************************************************/
static void cmp_ocpp_tx_ctrl_work(cmp_prot_ocpp_stu* prot,uint8_t period)
{
    cmp_ocpp_tx_ctrl_config_work(prot,period);
    
    cmp_ocpp_tx_plug_stat_work(prot,period);
    
    cmp_ocpp_tx_transction_work(prot,period);
}

/***************************************************************************************
* @Function    : cmp_prot_ocpp_resv_work()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/13
****************************************************************************************/
static void cmp_prot_ocpp_resv_work(cmp_prot_ocpp_stu* prot,uint8_t period)
{
    for(uint8_t i=0;i< BSP_POLE_PLUG_NUM;i++)
    {
	if(prot->ocpp_info.resv[i].resv_flag ==0) continue;
	if(prot->ocpp_info.resv[i].resv_time < period)
	{
	    prot->procc.cmd_hook_func(OCPP_CMD_REMOTE_START,i);
	    prot->ocpp_info.resv[i].resv_flag = 0;
	    return;
	}
	prot->ocpp_info.resv[i].resv_time -= period;
    }
}

/***************************************************************************************
* @Function    : cmp_ocpp_rx_msg_work()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/20
****************************************************************************************/
static void cmp_ocpp_rx_msg_work(cmp_prot_ocpp_stu* prot)
{
    drv_serial_rx_stu rx_msg;
    
    if(prot->pub_dev.serial == RT_NULL) return;
    
    if(prot->pub_dev.serial->dev_get_msg(&rx_msg,0) != RT_EOK) return;
    
    cmp_ocpp_analy_msg(rx_msg.buff,rx_msg.len);
}

/***************************************************************************************
* @Function    : cmp_ocpp_cyc_work()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/12
****************************************************************************************/
static void cmp_ocpp_cyc_work(uint8_t period)
{
    cmp_ocpp_rx_msg_work(&ocpp_conf);
    
    cmp_ocpp_tx_ctrl_work(&ocpp_conf,period);
    
    cmp_prot_ocpp_tx_cyc_work(&ocpp_conf,period);
    
    cmp_prot_ocpp_resv_work(&ocpp_conf,period);
}

/***************************************************************************************
* @Function    : cmp_ocpp_set_cmd_hook()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/13
****************************************************************************************/
static void cmp_ocpp_set_cmd_hook(ocpp_cmd_msg_func_type cmd_func)
{
    ocpp_conf.procc.cmd_hook_func = cmd_func;
}

/***************************************************************************************
* @Function    : cmp_prot_ocpp_src_dev_init()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/12
****************************************************************************************/
static void cmp_prot_ocpp_src_dev_init(void)
{
    ocpp_conf.pub_dev.store = cmp_dev_find(CMP_DEV_NAME_STORE);
    
    ocpp_conf.pub_dev.reset_io = drv_dev_find(DRV_DEV_DIDO_ESP_RESET);
    
    ocpp_conf.param.net_type = 1;
}

/***************************************************************************************
* @Function    : cmp_ocpp_set_cmd_result()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/13
****************************************************************************************/
static void cmp_ocpp_set_cmd_result(CMP_PROT_OCPP_CMD_TYPE type,uint8_t result)
{
    switch(type)
    {
    case OCPP_CMD_UNLOCK:cmp_prot_ocpp_cmd_repy(&ocpp_conf,0x3B,result);	break;
    case OCPP_CMD_REMOTE_START:cmp_prot_ocpp_cmd_repy(&ocpp_conf,0x3D,result);	break;
    case OCPP_CMD_REMOTE_STOP:cmp_prot_ocpp_cmd_repy(&ocpp_conf,0x3F,result);	break;
    case OCPP_CMD_CHANGE_AVAI:cmp_prot_ocpp_cmd_repy(&ocpp_conf,0x43,result);	break;
    case OCPP_CMD_RESET:cmp_prot_ocpp_cmd_repy(&ocpp_conf,0x45,result);		break;
    case OCPP_CMD_CHRG_LIMIT:cmp_prot_ocpp_cmd_repy(&ocpp_conf,0x49,result);	break;
    }
}

/***************************************************************************************
* @Function    : cmp_ocpp_set_auth_info()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/14
****************************************************************************************/
static void cmp_ocpp_set_auth_info(uint8_t plug,uint32_t uid,uint8_t type)
{
    uint8_t buff[16],pos=0;
    
    buff[pos++] = plug;
    buff[pos++] = 8;
    sprintf((char*)&buff[pos],"%d",uid);
    pos += 8;
    buff[pos++] = type;
    
    cmp_prot_ocpp_pack_and_tx_msg(&ocpp_conf,0x65,buff,pos);
}

/***************************************************************************************
* @Function    : cmp_ocpp_set_chrg_notyfy()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/14
****************************************************************************************/
static void cmp_ocpp_set_chrg_notify(uint8_t plug,uint8_t type)
{
    uint32_t tmp;
    uint8_t buff[16],pos=0,cmd;
    
    const cmp_measure_dev_stu* measure;measure = cmp_dev_find(CMP_DEV_NAME_MEASURE);
    if(measure == RT_NULL || plug >= BSP_POLE_PLUG_NUM) return;
    
    buff[pos++] = plug;
    tmp = measure[plug].cmp_get_chrg_energy();
    buff[pos++] = tmp >>24;
    buff[pos++] = tmp >>16;
    buff[pos++] = tmp >>8;
    buff[pos++] = tmp;
    
    cmd = type == 0?0x69:0x6B;
    cmp_prot_ocpp_pack_and_tx_msg(&ocpp_conf,cmd,buff,pos);
}

/***************************************************************************************
* @Function    : cmp_ocpp_set_transparent_data()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/14
****************************************************************************************/
static void cmp_ocpp_set_transparent_data(uint8_t* data,uint8_t len)
{
    uint8_t buff[256];
    
    memcpy(buff,data,len);
    
    cmp_prot_ocpp_pack_and_tx_msg(&ocpp_conf,0x38,buff,len);
}

/***************************************************************************************
* @Function    : cmp_ocpp_set_transparent_hook()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/14
****************************************************************************************/
static void cmp_ocpp_set_transparent_hook(occp_transmit_msg_func_type rx_func)
{
    ocpp_conf.procc.ocpp_rx_transparent_func = rx_func;
}

/***************************************************************************************
* @Function    : cmp_ocpp_set_serial()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/20
****************************************************************************************/
static void cmp_ocpp_set_serial(drv_serial_dev_stu* serial)
{
    ocpp_conf.pub_dev.serial = serial;
}

/***************************************************************************************
* @Function    : cmp_ocpp_get_uasable_state()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/20
****************************************************************************************/
static uint8_t cmp_ocpp_get_uasable_state(void)
{
    int8_t count = 40;

    ocpp_conf.pub_dev.reset_io->drv_do_on();
    rt_thread_mdelay(1000);
    ocpp_conf.pub_dev.reset_io->drv_do_off();
    rt_thread_mdelay(500);
    
    while(count>=0)
    {
	cmp_ocpp_rx_msg_work(&ocpp_conf);
	if(ocpp_conf.procc.ocpp_sta > 0)
	{
	    ocpp_conf.procc.ocpp_sta = 0;
	    return RT_EOK;
	}
	count--;
	rt_thread_mdelay(500);
    }
    
    return RT_EEMPTY;
}

/***************************************************************************************
* @Function    : cmp_prot_ocpp_dev_init()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/12
****************************************************************************************/
static void cmp_prot_ocpp_dev_init(void)
{
    static cmp_prot_ocpp_dev_stu ocpp_dev;
    
    ocpp_dev.ocpp_set_serial = cmp_ocpp_set_serial;
    
    ocpp_dev.ocpp_get_uasable_state = cmp_ocpp_get_uasable_state;
    
    ocpp_dev.ocpp_cyc_work = cmp_ocpp_cyc_work;
    
    ocpp_dev.ocpp_set_cmd_result = cmp_ocpp_set_cmd_result;
    
    ocpp_dev.ocpp_set_auth_info = cmp_ocpp_set_auth_info;
    
    ocpp_dev.ocpp_set_chrg_notify = cmp_ocpp_set_chrg_notify;
    
    ocpp_dev.ocpp_set_cmd_msg_hook = cmp_ocpp_set_cmd_hook;
    
    ocpp_dev.ocpp_set_transparent_data = cmp_ocpp_set_transparent_data;
    
    ocpp_dev.ocpp_set_transparent_hook = cmp_ocpp_set_transparent_hook;
    
    cmp_dev_append(CMP_DEV_NAME_OCPP,&ocpp_dev);
}

/***************************************************************************************
* @Function    : cmp_prot_ocpp_dev_create()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/9/12
****************************************************************************************/
static int cmp_prot_ocpp_dev_create(void)
{
    cmp_prot_ocpp_src_dev_init();
    
    cmp_prot_ocpp_dev_init();
    
    return 0;
}
INIT_APP_EXPORT(cmp_prot_ocpp_dev_create);
