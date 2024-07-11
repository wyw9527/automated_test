#ifndef _CMP_W5500_H_
#define _CMP_W5500_H_

#include "cmp.h"
#include "bsp_io.h"


#define W5500_RX_BUFF_LEN 1024
#define W5500_RX_BUFF_SIZE 4

#define W5500_TX_BUFF_LEN 256
#define W5500_TX_BUFF_SIZE 8


typedef struct
{
    uint8_t data_len;
    uint8_t msg[W5500_TX_BUFF_LEN];
}w5500_tx_buff_msg_stu;

typedef struct
{
    uint8_t msg[W5500_RX_BUFF_LEN];
    uint16_t data_len;
}w5500_rx_buff_msg_stu;

typedef struct
{
    uint8_t head;
    uint8_t trail;
    w5500_tx_buff_msg_stu buff[W5500_TX_BUFF_SIZE];
}w5500_tx_buff_stu;

typedef struct
{
    uint8_t head;
    uint8_t trail;
    w5500_rx_buff_msg_stu buff[W5500_RX_BUFF_SIZE];
}w5500_rx_buff_stu;

typedef enum
{
    W5500_SERV_MSG_CMD_STA,/* 状态命令 */
    W5500_SERV_MSG_CMD_RX, /* 接收到报文命令 */
}w5500_serv_msg_cmd_type;

typedef struct
{
    uint8_t app_id;
    w5500_serv_msg_cmd_type cmd;
    uint16_t len;
    void* data;
}w5500_serv_msg_stu;

typedef enum
{
    W5500_UPPER_BLE_CONN_STA = 0x0,

}W5500_UPPER_TYPE;

typedef enum
{
    W5500_CALLBACK_CMD_SPP,
    CMP_W5500_RX_MSG,
    W5500_CALLBACK_CMD_AT,
    
} CMP_W5500_CONN_STA_TYPE;


typedef void (*w5500_conn_sta_hook_func)(CMP_W5500_CONN_STA_TYPE cmd, void *data);

typedef struct
{
    /*远程ip*/
    uint8_t remote_ip[4];

    /*远程port*/
    uint16_t remote_port;
   
    /* 工作周期 */
    uint8_t period;
        
    /*告警计数 */
    uint32_t alarm_count;

    /*定时器计数*/
    uint32_t timer_count;
        
}w5500_init_stu;

typedef struct
{
    /* 设备MAC */
    uint8_t mac[6];

    /*本地ip*/
    uint8_t local_ip[4];

    /*本地port*/
    uint16_t local_port;

}w5500_data_stu;

typedef union
{
    uint8_t val;
    struct
    {
        uint8_t PHY_unconn:1;//网线未连接
        uint8_t tcp_unconn:1;//tcp 未连接
        uint8_t dhcp_abn:1;//dhcp 异常
    }bits;
}w5500_alarm_stu;

typedef enum
{
    READY,
    DHCP_ING,
    DHCP_OK,
    TCP_OPEN,
    TCP_CONN_ING,
    TCP_CONN_ED,
    TCP_CLOSE
}W5500_STU_TYPE;

typedef struct
{
    void (*w5500_push_tx_msg_to_buff_func)(uint8_t* buff,uint8_t len);
}w5500_ext_func_stu;

typedef struct
{
    uint8_t* rx_buff;
    uint16_t rx_len;
    uint8_t  rx_flag;
}cmp_w5500_rx_stu;

typedef struct
{
    w5500_init_stu init_param;

    w5500_data_stu w5500_data;
    
    cmp_w5500_rx_stu rx;
    
    /*w5500 模块运行状态*/
    W5500_STU_TYPE process_stu;
    
    /*告警 状态:0 正常  1 故障 */
    uint8_t alarm_sta;
    
    /* 告警 数据*/
    w5500_alarm_stu alarm;
        
    /* 运行计时 */
    uint16_t pro_count;
  
    /*告警计数 */
    uint32_t alarm_count;

    w5500_conn_sta_hook_func w5500_callback;


}cmp_w5500_stu;


typedef struct 
{
    
    /* 复位w5500模块 */
    void(*func_reset_w5500_module)(void);
    
    /* 接收数据 */
    uint16_t(*func_rx_msg)(uint8_t* buff, uint16_t max_len);

    /*接收数据处理*/
    void (*func_rx_msg_hnd)(uint8_t* buff,uint16_t len);
    
    /* 发送数据 */
    uint8_t(*func_tx_msg)(uint8_t* msg,uint8_t len);

    /* 工作 */  
    void (*cmp_w5500_work)(uint8_t period);

    void (*w5500_conn_sta_hook)(w5500_conn_sta_hook_func w5500_hook);

}cmp_w5500_dev_stu;






#endif
