#include "cmp.h"

static bsp_dev_lists *cmp_dev_list_p = RT_NULL;

static uint8_t plug_sta[BSP_POLE_PLUG_NUM];

/***************************************************************************************
 * @Function    : cmp_dev_append()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/1
 ****************************************************************************************/
uint8_t cmp_dev_append(const char *name, void *dev)
{
    if (dev == RT_NULL)
        return 0;

    /* 创建一个设备 */
    bsp_dev_lists *new_dev = rt_malloc(sizeof(bsp_dev_lists));
    if (new_dev == RT_NULL)
        return 0;

    memset(new_dev->name, 0, sizeof(new_dev->name));
    strncpy(new_dev->name, name, BSP_DEV_NAME_MAX_LEN);
    new_dev->bsp_dev = dev;
    new_dev->next = RT_NULL;

    if (cmp_dev_list_p == RT_NULL)
    {
        cmp_dev_list_p = new_dev;
        return 1;
    }

    bsp_dev_lists *dev_p = cmp_dev_list_p;
    while (dev_p->next != RT_NULL)
    {
        dev_p = dev_p->next;
    }

    dev_p->next = new_dev;
    return 1;
}

/***************************************************************************************
 * @Function    : cmp_dev_find()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/1
 ****************************************************************************************/
void *cmp_dev_find(const char *name)
{
    bsp_dev_lists *dev_p = cmp_dev_list_p;

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
 * @Function    : cmp_set_plug_sta()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/21
 ****************************************************************************************/
void cmp_set_plug_sta(uint8_t idx, uint8_t sta)
{
    if (idx >= BSP_POLE_PLUG_NUM)
        return;

    plug_sta[idx] = sta;
}

/***************************************************************************************
 * @Function    : cmp_get_plug_sta()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/21
 ****************************************************************************************/
uint8_t cmp_get_plug_sta(uint8_t idx, uint8_t *ret)
{
    if (idx >= BSP_POLE_PLUG_NUM)
        return RT_ERROR;

    *ret = plug_sta[idx];
    return RT_EOK;
}

/***************************************************************************************
 * @Function    : cmp_get_plug_uasable_sta()
 *
 * @Param       :
 *
 * @Return      : 0 可用 1 准备好  2 充电  3 不可用
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/22
 ****************************************************************************************/
uint8_t cmp_get_plug_uasable_sta(uint8_t idx, uint8_t *ret)
{
    if (idx >= BSP_POLE_PLUG_NUM)
        return RT_ERROR;

    if (plug_sta[idx] == 2 || plug_sta[idx] == 3 || plug_sta[idx] == 4 || plug_sta[idx] == 7)
    {
        *ret = 2;
        return RT_EOK;
    }

    if (plug_sta[idx] == 1)
    {
        *ret = 1;
        return RT_EOK;
    }

    if (plug_sta[idx] == 6 || plug_sta[idx] == 8)
    {
        *ret = 3;
        return RT_EOK;
    }

    *ret = 0;
    return RT_EOK;
}

/***************************************************************************************
 * @Function    : cmp_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : WYW            Version   : V2.0.0                Date:2023/10/19
 ****************************************************************************************/


static int cmp_init(void)
{
    return 0;
}
INIT_APP_EXPORT(cmp_init);
