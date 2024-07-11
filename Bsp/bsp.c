#include "bsp.h"

#define MB_ALARM_BUFF_SIZE 16
#define ALARM_CURR_DATA_BUFF_NUM 32
#define MB_OPER_DATA_NUM 70

static bsp_dev_lists *bsp_dev_list_p = RT_NULL;

struct rt_mailbox mb_sys;
uint8_t mb_sys_pool[32];

static uint8_t mb_alarm_buff_idx = 0;
static bsp_alarm_mb_stu mb_alarm_buff[MB_ALARM_BUFF_SIZE];
static bsp_int_union_stu bsp_alarm_cur_val_buff[ALARM_CURR_DATA_BUFF_NUM];

static uint8_t oper_data_idx = 0;
static bsp_dev_oper_stu mb_oper_data[MB_OPER_DATA_NUM];

/***************************************************************************************
 * @Function    : bsp_alarm_mb_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/18
 ****************************************************************************************/
void bsp_alarm_mb_init(void)
{
    rt_mb_init(&mb_sys, "sys", &mb_sys_pool[0], sizeof(mb_sys_pool) / 4, RT_IPC_FLAG_PRIO);
    memset(bsp_alarm_cur_val_buff, 0xff, ALARM_CURR_DATA_BUFF_NUM * 4);
}

/***************************************************************************************
 * @Function    : bsp_alarm_put_val()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/5
 ****************************************************************************************/
static void bsp_alarm_put_val(bsp_int_union_stu union_data)
{
    uint8_t idx;
    // uint32_t msk_idx1, msk_idx2;

    for (idx = 0; idx < ALARM_CURR_DATA_BUFF_NUM; idx++)
    {
        if (bsp_alarm_cur_val_buff[idx].val == 0xffffffff)
        {
            break;
        }

        if (union_data.eles.byte[0] == bsp_alarm_cur_val_buff[idx].eles.byte[0] && union_data.eles.byte[1] == bsp_alarm_cur_val_buff[idx].eles.byte[1] && union_data.eles.byte[2] == bsp_alarm_cur_val_buff[idx].eles.byte[2])
        {
            break;
        }
    }

    if (idx >= ALARM_CURR_DATA_BUFF_NUM)
        return;

    bsp_alarm_cur_val_buff[idx].val = union_data.val;
}

/***************************************************************************************
 * @Function    : bsp_get_alram_val()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/5
 ****************************************************************************************/
uint8_t bsp_get_alram_val(uint8_t *buff)
{
    uint8_t idx = 0, alarm_num = 0;

    for (uint8_t i = 0; i < ALARM_CURR_DATA_BUFF_NUM; i++)
    {
        if (bsp_alarm_cur_val_buff[i].val == 0xFFFFFFFF)
            return alarm_num;

        if (bsp_alarm_cur_val_buff[i].eles.byte[3] > 0 && bsp_alarm_cur_val_buff[i].eles.byte[3] < 3)
        {
            memcpy(&buff[idx], &bsp_alarm_cur_val_buff[i].eles.byte[0], 4);
            idx += 4;
            alarm_num++;
        }
    }
    return alarm_num;
}

/***************************************************************************************
 * @Function    : bsp_alarm_mb_send()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/18
 ****************************************************************************************/
void bsp_alarm_mb_send(uint8_t plug_idx, uint8_t unit, uint8_t sub_unit, uint8_t val)
{
    bsp_int_union_stu union_val;

    union_val.eles.byte[0] = plug_idx;
    union_val.eles.byte[1] = unit;
    union_val.eles.byte[2] = sub_unit;
    union_val.eles.byte[3] = val;

    bsp_alarm_put_val(union_val);
    mb_alarm_buff[mb_alarm_buff_idx].from_app_id = BSP_MB_APP_ID;
    mb_alarm_buff[mb_alarm_buff_idx].data = union_val.val;
    rt_mb_send(&mb_sys, (rt_uint32_t)&mb_alarm_buff[mb_alarm_buff_idx]);
    if (++mb_alarm_buff_idx >= MB_ALARM_BUFF_SIZE)
    {
        mb_alarm_buff_idx = 0;
    }
}

/***************************************************************************************
 * @Function    : bsp_get_uart_pin_rcu()
 *
 * @Param       :
 *
 * @Return      : 0 abn periph  other: normal periph
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/4/13
 ****************************************************************************************/
rcu_periph_enum bsp_get_pin_rcu(uint32_t gpio_periph)
{
    switch (gpio_periph)
    {
    case GPIOA:
        return RCU_GPIOA;
    case GPIOB:
        return RCU_GPIOB;
    case GPIOC:
        return RCU_GPIOC;
    case GPIOD:
        return RCU_GPIOD;
    case GPIOE:
        return RCU_GPIOE;
    case GPIOF:
        return RCU_GPIOF;
    }
    return ((rcu_periph_enum)0);
}

/***************************************************************************************
 * @Function    : bsp_dev_append()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description : 新增一个设备结点
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/4/27
 ****************************************************************************************/
uint8_t bsp_dev_append(const char *name, void *dev)
{
    if (dev == RT_NULL)
        return RT_ERROR;

    /* 创建一个设备 */
    bsp_dev_lists *new_dev = rt_malloc(sizeof(bsp_dev_lists));
    if (new_dev == RT_NULL)
        return RT_ERROR;

    memset(new_dev->name, 0, sizeof(new_dev->name));
    strncpy(new_dev->name, name, BSP_DEV_NAME_MAX_LEN);
    new_dev->bsp_dev = dev;
    new_dev->next = RT_NULL;

    if (bsp_dev_list_p == RT_NULL)
    {
        bsp_dev_list_p = new_dev;
        return RT_EOK;
    }

    bsp_dev_lists *dev_p = bsp_dev_list_p;
    while (dev_p->next != RT_NULL)
    {
        dev_p = dev_p->next;
    }

    dev_p->next = new_dev;
    return RT_EOK;
}

/***************************************************************************************
 * @Function    : bsp_dev_find()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/4/27
 ****************************************************************************************/
void *bsp_dev_find(const char *name)
{
    bsp_dev_lists *dev_p = bsp_dev_list_p;

    if (name == RT_NULL)
        return RT_NULL;

    while (dev_p != RT_NULL)
    {
        if (strncmp(dev_p->name, name, BSP_DEV_NAME_MAX_LEN) == 0)
        {
            return dev_p->bsp_dev;
        }
        dev_p = dev_p->next;
    }

    return RT_NULL;
}

/***************************************************************************************
 * @Function    : bsp_bind_oper_data_dev()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/18
 ****************************************************************************************/
void bsp_bind_oper_data_dev(uint8_t plug_idx, uint8_t unit, uint8_t sub_unit, uint8_t *data, uint8_t len)
{
    if (oper_data_idx >= MB_OPER_DATA_NUM)
        return;

    mb_oper_data[oper_data_idx].plug_idx = plug_idx;
    mb_oper_data[oper_data_idx].unit = unit;
    mb_oper_data[oper_data_idx].sub_unit = sub_unit;
    mb_oper_data[oper_data_idx].len = len;
    mb_oper_data[oper_data_idx].data = data;

    oper_data_idx++;
}

/***************************************************************************************
 * @Function    : bsp_get_oper_data()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/18
 ****************************************************************************************/
const bsp_dev_oper_stu *bsp_get_oper_data(uint8_t plug_idx, uint8_t unit, uint8_t sub_unit)
{
    for (uint8_t i = 0; i < oper_data_idx; i++)
    {
        if (mb_oper_data[i].plug_idx != plug_idx)
            continue;
        if (mb_oper_data[i].unit != unit)
            continue;
        if (mb_oper_data[i].sub_unit != sub_unit)
            continue;
        return &mb_oper_data[i];
    }
    return RT_NULL;
}

