#include "cmp_store.h"

#include "rthw.h"
static cmp_store_stu store_conf;

/***************************************************************************************
 * @Function    : cmp_store_whilelist_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/2
 ****************************************************************************************/
static uint8_t cmp_store_tim_dev_create(cmp_store_dev_stu *store)
{
    store->tim = cmp_dev_find(CMP_DEV_NAME_TIM);
    if (store->tim == RT_NULL)
        return 1;

    store->tim->cmp_set_ext_flash(store_conf.curr_flash, store_conf.clock, CMP_STORE_TYPE);
    return 0;
}

/***************************************************************************************
 * @Function    : cmp_store_model_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/2
 ****************************************************************************************/
static uint8_t cmp_store_model_dev_create(cmp_store_dev_stu *store)
{
    store->model = cmp_dev_find(CMP_DEV_NAME_MODEL);
    if (store->model == RT_NULL)
        return 1;

    store->model->cmp_set_ext_flash(store_conf.curr_flash, CMP_STORE_TYPE);
    return 0;
}

/***************************************************************************************
 * @Function    : cmp_store_order_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/2
 ****************************************************************************************/
static uint8_t cmp_store_order_dev_create(cmp_store_dev_stu *store)
{
    store->order = cmp_dev_find(CMP_DEV_NAME_ORDER);
    if (store->order == RT_NULL)
        return 1;

    store->order->cmp_set_ext_flash(store_conf.curr_flash, CMP_STORE_TYPE);
    return 0;
}

/***************************************************************************************
 * @Function    : cmp_store_ota_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/15
 ****************************************************************************************/
static uint8_t cmp_store_ota_dev_create(cmp_store_dev_stu *store)
{
    store->ota = cmp_dev_find(CMP_DEV_NAME_OTA);
    if (store->ota == RT_NULL)
        return 1;

    store->ota->store_ota_set_ext_flash(store_conf.inn_flash);
    return 0;
}

/***************************************************************************************
 * @Function    : cmp_store_param_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/10/8
 ****************************************************************************************/
static uint8_t cmp_store_param_dev_create(cmp_store_dev_stu *store)
{
    store->param = cmp_dev_find(CMP_DEV_NAME_PARAM);
    if (store->param == RT_NULL)
        return 1;

    store->param->cmp_set_ext_flash(store_conf.curr_flash, CMP_STORE_TYPE);
    return 0;
}

/***************************************************************************************
 * @Function    : cmp_store_whitelist_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/10/9
 ****************************************************************************************/
static uint8_t cmp_store_whitelist_dev_create(cmp_store_dev_stu *store)
{
    store->whitelist = cmp_dev_find(CMP_DEV_NAME_WHL);
    if (store->whitelist == RT_NULL)
        return 1;

    store->whitelist->cmp_set_ext_flash(store_conf.curr_flash, CMP_STORE_TYPE);
    return 0;
}

/***************************************************************************
*@Function    :
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-10-19
***************************************************************************/
static uint8_t cmp_restore_store(uint32_t addr, uint8_t page_cnt)
{
    return store_conf.curr_flash->drv_flash_erase_area(addr, page_cnt);
}

/***************************************************************************
*@Function    :cmp_get_flash_func
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-12-21
***************************************************************************/
static drv_flash_dev_stu *cmp_get_flash_func(CMP_FLASH_TYPE type)
{
    if (type == INN_FLASH)
    {
        return store_conf.inn_flash;
    }
    return store_conf.ext_flash;
}

/***************************************************************************************
 * @Function    : cmp_store_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/1
 ****************************************************************************************/
int cmp_store_dev_create(void)
{
    static cmp_store_dev_stu store_dev;

    /* flash dev find */
    store_conf.ext_flash = drv_dev_find(DRV_DEV_EXT_FLASH);
    store_conf.inn_flash = drv_dev_find(DRV_DEV_INN_FLASH);
    store_conf.clock = drv_dev_find(DRV_DEV_CLOCK);
    store_conf.curr_flash = CMP_STORE_TYPE == 0 ? store_conf.ext_flash : store_conf.inn_flash;

    store_dev.cmp_restore_store = cmp_restore_store;
    store_dev.cmp_get_flash = cmp_get_flash_func;

    /* system param part init */
    cmp_store_param_dev_create(&store_dev);

    /* whitelist part init */
    cmp_store_whitelist_dev_create(&store_dev);

    /* tim part init */
    cmp_store_tim_dev_create(&store_dev);

    /* fee mode init */
    cmp_store_model_dev_create(&store_dev);

    /* order part init */
    cmp_store_order_dev_create(&store_dev);

    /* ota init */
    cmp_store_ota_dev_create(&store_dev);

    cmp_dev_append(CMP_DEV_NAME_STORE, &store_dev);
    return 0;
}
// INIT_ENV_EXPORT(cmp_store_dev_create);
