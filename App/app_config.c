#include "app_config.h"

#define MB_MSG_BUFF_SIZE 32

struct rt_mailbox mb_serv;
static uint8_t mb_serv_pool[64];


static uint8_t mb_msg_buff_idx = 0;
static app_conf_mb_stu mb_msg_buff[MB_MSG_BUFF_SIZE];

/* app task stack define */
static uint8_t serv_stack[1024];
static struct rt_thread serv_thread;



/***************************************************************************************
 * @Function    : app_config_mb_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/15
 ****************************************************************************************/
void app_config_mb_init(void)
{

}

/***************************************************************************************
 * @Function    : app_sys_task_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/16
 ****************************************************************************************/
void app_config_task_init(void)
{
    extern void app_server(void *parameter);
    rt_thread_init(&serv_thread, "serv", app_server, RT_NULL, serv_stack, sizeof(serv_stack), APP_SERV_PRIO, 20);
    rt_thread_startup(&serv_thread);

    // extern void app_upper(void *parameter);
    // rt_thread_init(&upper_thread, "upper", app_upper, RT_NULL, upper_stack, sizeof(upper_stack), APP_UPPER_PRIO, 20);
    // rt_thread_startup(&upper_thread);

}

/***************************************************************************************
 * @Function    : app_config_mb_send()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/15
 ****************************************************************************************/
void app_config_mb_send(uint8_t to_mb_id, app_conf_mb_stu *mb_msg)
{
    rt_mailbox_t mb_c;

    memcpy((uint8_t *)&mb_msg_buff[mb_msg_buff_idx], (uint8_t *)mb_msg, sizeof(app_conf_mb_stu));

    switch (to_mb_id)
    {

    default:
        return;
    }

    rt_mb_send(mb_c, (rt_uint32_t)&mb_msg_buff[mb_msg_buff_idx]);
    if (++mb_msg_buff_idx >= MB_MSG_BUFF_SIZE)
    {
        mb_msg_buff_idx = 0;
    }
}

