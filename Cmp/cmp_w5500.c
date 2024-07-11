#include "w5500.h"
#include "W5500_conf.h"
#include "tcp_demo.h"
#include "dhcp.h"
#include "socket.h"
#include "cmp_w5500.h"

static uint8_t socket_txsize[8] = {2,2,2,2,2,2,2,2}; // tx buffer set K bytes
static uint8_t socket_rxsize[8] = {2,2,2,2,2,2,2,2}; // rx buffet set K bytes

cmp_w5500_stu w5500_conf;

uint8 remote_ip[4]={192,168,1,100};

/***************************************************************************
* FuncName     : w5500_get_MCU_ID()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : RGxiang         Version:    V1.0      Date:    2022/8/23
***************************************************************************/      
 static uint32_t w5500_get_MCU_ID(uint8_t* pMcuID)
{
    uint32_t CpuID[3] = {0};
		
    //获取CPU唯一ID
    CpuID[0]=*(uint32_t*)(0x1ffff7e8);
    CpuID[1]=*(uint32_t*)(0x1ffff7ec);
    CpuID[2]=*(uint32_t*)(0x1ffff7f0);

    //按字节（8位）读取
    pMcuID[0] = (uint8_t)(CpuID[0] & 0x000000FF);
    pMcuID[1] = (uint8_t)((CpuID[0] & 0xFF00) >>8);
    pMcuID[2] = (uint8_t)((CpuID[0] & 0xFF0000) >>16);
    pMcuID[3] = (uint8_t)((CpuID[0] & 0xFF000000) >>24);

    pMcuID[4] = (uint8_t)(CpuID[1] & 0xFF);
    pMcuID[5] = (uint8_t)((CpuID[1] & 0xFF00) >>8);
    pMcuID[6] = (uint8_t)((CpuID[1] & 0xFF0000) >>16);
    pMcuID[7] = (uint8_t)((CpuID[1] & 0xFF000000) >>24);

    pMcuID[8] = (uint8_t)(CpuID[2] & 0xFF);
    pMcuID[9] = (uint8_t)((CpuID[2] & 0xFF00) >>8);
    pMcuID[10] = (uint8_t)((CpuID[2] & 0xFF0000) >>16);
    pMcuID[11] = (uint8_t)((CpuID[2] & 0xFF000000) >>24);

    return (CpuID[0]>>1)+(CpuID[1]>>2)+(CpuID[2]>>3);
}

/***************************************************************************
* FuncName     : w5500_create_sysMAC()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : RGxiang         Version:    V1.0      Date:    2022/8/23
***************************************************************************/
static void w5500_create_sysMAC(uint8_t* pMacBuf)
{
    uint32_t uiMcuId = 0;
    uint8_t McuID[15] = {0};
    int i = 0;

    uiMcuId = w5500_get_MCU_ID(McuID);

    for(i=0; i<12; i++) //获取McuID[12]
    {
        McuID[12] += McuID[i];	
    }
    for(i=0; i<12; i++)	//获取McuID[13]
    {
        McuID[13] ^= McuID[i];	
    }
    /*前3个字节是OUI分配给W5500是固定的*/
    /*后3位取的是MCU ID转换的MAC的后3位*/
    pMacBuf[0] = 0x00;
    pMacBuf[1] = 0x08;
    pMacBuf[2] = 0xdc;
    pMacBuf[3] = (uint8_t)((uiMcuId & 0xFF000000) >>24);
    pMacBuf[4] = McuID[12];
    pMacBuf[5] = McuID[13];
}
//uint8_t flag = 0;
/***************************************************************************
* FuncName     : w5500_module_reset()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : RGxiang         Version:    V1.0      Date:    2022/7/14
***************************************************************************/
static void w5500_module_reset()
{
    uint8_t w5500_mac[6] = {0x00};
    
    w5500_conf.process_stu = READY;
    /*硬复位W5500*/
    reset_w5500();
    /*配置MAC地址*/
    w5500_create_sysMAC(w5500_mac);
    memcpy(w5500_conf.w5500_data.mac, w5500_mac, 6);
    set_w5500_mac(w5500_conf.w5500_data.mac);
    //flag = getPHYStatus();
    setPHYCFGR(0xCD);
    //flag = getPHYStatus();
    /*初始化8个Socket的发送接收缓存大小*/
    socket_buf_init(socket_txsize, socket_rxsize);
    /*DHCP client 初始化*/
    DHCP_Client_Init();
}

/***************************************************************************
* FuncName     : w5500_tx_msg()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : RGxiang         Version:    V1.0      Date:    2022/7/14
***************************************************************************/
static uint8_t w5500_tx_msg(uint8_t* buff,uint8_t len)
{
    return send(SOCK_TCPC,buff,len);
}

/***************************************************************************
* FuncName     : w5500_rx_msg()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : RGxiang         Version:    V1.0      Date:    2022/7/14
***************************************************************************/
static uint16 w5500_rx_msg(uint8_t* buff, uint16_t max_len)
{    
    uint16 len=0;

    if(w5500_conf.process_stu != TCP_CONN_ED)
        return len;
    
    len=getSn_RX_RSR(SOCK_TCPC); /*定义len为已接收数据的长度*/
    
    if(len)
    {
        if(len > max_len)
        {
            len = max_len;
        }
        /*接收来自Server的数据*/
        recv(SOCK_TCPC,buff,len); 
    }
    return len;
}

/***************************************************************************
* FuncName     : w5500_tcp_client_deal()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : RGxiang         Version:    V1.0      Date:    2022/7/14
***************************************************************************/
void w5500_tcp_client_deal()
{	
    uint8 status = 0;

    if(w5500_conf.alarm.bits.PHY_unconn||(w5500_conf.process_stu == DHCP_ING))//网线未连接或dhcp失败
        return;

    status = getSn_SR(SOCK_TCPC);
    switch(status)/*获取socket的状态*/
    {
        case SOCK_CLOSED:/*socket处于关闭状态*/
        w5500_conf.process_stu = TCP_OPEN;
        socket(SOCK_TCPC,Sn_MR_TCP,local_port,Sn_MR_ND);
        //setKPALVTR(SOCK_TCPC,1);	/*心跳机制*/
        break;

        case SOCK_INIT:/*socket处于初始化状态*/
        w5500_conf.process_stu = TCP_CONN_ING;
        //rt_enter_critical();
        connect(SOCK_TCPC,remote_ip,8080);/*socket连接服务器*/ 
        //rt_exit_critical();
        break;

        case SOCK_SYNSENT:/*socket处于正在建立连接中三步握手*/
        w5500_conf.process_stu = TCP_CONN_ING;
        break;
        
        case SOCK_ESTABLISHED: /*socket处于连接建立状态*/
        w5500_conf.process_stu = TCP_CONN_ED;
        if(getSn_IR(SOCK_TCPC) & Sn_IR_CON)
        {
            setSn_IR(SOCK_TCPC, Sn_IR_CON);/*清除接收中断标志位*/
        }
	  
        break;

        case SOCK_CLOSE_WAIT: /*socket处于等待关闭状态*/
        w5500_conf.process_stu = TCP_CLOSE;
        close(SOCK_TCPC);
        break;
    }
}

/***************************************************************************
* FuncName     : w5500_dhcp_deal()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : RGxiang         Version:    V1.0      Date:    2022/7/14
***************************************************************************/
void w5500_dhcp_deal()
{
    uint8 dhcpret=0;
    
    if(w5500_conf.alarm.bits.PHY_unconn)
    {
        w5500_conf.process_stu = DHCP_ING;
        return;
    }
    dhcpret = check_DHCP_state();/*获取DHCP服务状态*/
	
    switch(dhcpret)
    {
        case DHCP_RUNNING: /*DHCP正在运行*/ 
        w5500_conf.process_stu = DHCP_ING;
        break;
        
        case DHCP_IP_ASSIGN: /*IP地址DHCP已分配*/ 
        case DHCP_IP_CHANGED: /*IP地址DHCP已重新变更*/ 
        w5500_conf.process_stu = DHCP_ING;
        break;
		
        case DHCP_IP_LEASED: /*成功获取到IP租约*/ 
        w5500_conf.process_stu = DHCP_OK;
        break;
		
        case DHCP_IP_CONFLICT: /*IP地址获取冲突*/ 
        case DHCP_FAILED:     
        close(SOCK_DHCP);
        break;     

        default:
        break;
    }
}

/***************************************************************************
* FuncName     : w5500_act_work()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : RGxiang         Version:    V1.0      Date:    2022/7/14
***************************************************************************/
static void w5500_act_work()
{
    w5500_dhcp_deal();
    w5500_tcp_client_deal();
}

/***************************************************************************
* FuncName     : w5500_alarm_sta_work()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : RGxiang         Version:    V1.0      Date:    2022/7/14
***************************************************************************/
static void w5500_alarm_sta_work(uint8_t period)
{
    w5500_conf.alarm.bits.PHY_unconn = PHY_check() == 1?0:1;
    w5500_conf.alarm.bits.tcp_unconn = w5500_conf.process_stu == TCP_CONN_ED?0:1;
    w5500_conf.alarm.bits.dhcp_abn = w5500_conf.process_stu == DHCP_ING?1:0;

    /*判断是否有告警数据*/
    if(w5500_conf.alarm.val == 0)
    {
        w5500_conf.alarm_count = 0;
        w5500_conf.alarm_sta = 0;
        return;
    }
    
    /* 已是告警状态则不需判断 */
    if(w5500_conf.alarm_sta > 0)
    {
        return;
    }
    
    /* 告警超时判断 */
    w5500_conf.alarm_count += period;
    if(w5500_conf.alarm_count > 10000)
    {
        w5500_conf.alarm_sta = 1;
    }
}

/***************************************************************************
* FuncName     : w5500_process_sta_handle()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : RGxiang         Version:    V1.0      Date:    2022/7/14
***************************************************************************/
static void w5500_process_sta_handle()
{
    static W5500_STU_TYPE pre_sta = READY;

    if(w5500_conf.process_stu == TCP_CONN_ED)
    {
        if(pre_sta != TCP_CONN_ED)//进入透传
            w5500_conf.w5500_callback(W5500_CALLBACK_CMD_SPP,RT_NULL);
        else//透传
        {
            if(w5500_conf.rx.rx_flag == 1)
            {
                w5500_conf.rx.rx_flag = 0;
                w5500_conf.w5500_callback(CMP_W5500_RX_MSG, &w5500_conf.rx);
            }
        }
    }
    else
    {
        if(pre_sta == TCP_CONN_ED)//退出透传
        {
            w5500_conf.w5500_callback(W5500_CALLBACK_CMD_AT,RT_NULL);
        }
    }

    pre_sta = w5500_conf.process_stu;
}

/***************************************************************************
* FuncName     : w5500_timer_work()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : RGxiang         Version:    V1.0      Date:    2022/7/14
***************************************************************************/
static void w5500_timer_work(uint8_t period)
{
    /* 定时器超时判断 */
    w5500_conf.pro_count += period;
    if(w5500_conf.pro_count > 1000)
    {
        w5500_conf.pro_count = 0;
        /*软件定时器处理*/
        timer_counter_process();
    }
}

/***************************************************************************
* FuncName     : w5500_dev_cyc_work()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : RGxiang         Version:    V1.0      Date:    2022/7/14
***************************************************************************/
static void w5500_dev_cyc_work(uint8_t period)
{
    /*软件定时器处理*/
    w5500_timer_work(period);
        
    /* 动作执行 */
    w5500_act_work();

    /*告警状态检测*/
    w5500_alarm_sta_work(period);

    /*W5500运行状态处理*/
    w5500_process_sta_handle();

}

/***************************************************************************************
* @Function    : w5500_rx_msg_hnd()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V2.0.0                Date:2023/4/11
****************************************************************************************/
static void w5500_rx_msg_hnd(uint8_t* buff,uint16_t len)
{
    if(w5500_conf.process_stu == TCP_CONN_ED)
    {
	w5500_conf.rx.rx_buff = buff;
	w5500_conf.rx.rx_len = len;
	w5500_conf.rx.rx_flag = 1;
	return;
    }
}

static void cmp_w5500_conn_sta_hook(w5500_conn_sta_hook_func w5500_hook)
{
    w5500_conf.w5500_callback = w5500_hook;
}

/***************************************************************************
* FuncName     : app_w5500_init()
*
* Desc         : 
*
* Change Logs  :
*
* Author       : RGxiang         Version:    V1.0      Date:    2022/7/14
***************************************************************************/
static void cmp_w5500_create()
{
    static cmp_w5500_dev_stu w5500_dev;

    /* set initialization parameters */
    // memcpy((uint8_t*)&w5500_dev.init_param,(uint8_t*)init,sizeof(w5500_init_stu));
    
    /* w5500模块复位*/
    w5500_dev.func_reset_w5500_module = w5500_module_reset;
    
    /* tcp接收报文注册 */
    w5500_dev.func_rx_msg = w5500_rx_msg;

    /*接收报文数据处理*/
    w5500_dev.func_rx_msg_hnd = w5500_rx_msg_hnd;

    /*tcp发送报文注册*/
    w5500_dev.func_tx_msg = w5500_tx_msg;
        
    w5500_dev.cmp_w5500_work = w5500_dev_cyc_work;

    w5500_dev.w5500_conn_sta_hook = cmp_w5500_conn_sta_hook;
    
    w5500_conf.process_stu = READY;
    
    /*w5500模块复位*/
    w5500_dev.func_reset_w5500_module();
        
    cmp_dev_append(CMP_DEV_NAME_W5500, &w5500_dev);

}

/***************************************************************************************
 * @Function    : cmp_prot_mt_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/15
 ****************************************************************************************/
static int cmp_w5500_dev_create(void)
{
    /*初始化MCU相关引脚*/
    bsp_W5500_init();

    cmp_w5500_create();

    return 0;
}
INIT_APP_EXPORT(cmp_w5500_dev_create);

