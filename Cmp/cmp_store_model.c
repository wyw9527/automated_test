#include "cmp_store_model.h"

static cmp_store_model_s_stu model_conf;
static cmp_store_model_stu store_model;

/***************************************************************************************
 * @Function    : cmp_store_model_query()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/2
 ****************************************************************************************/
static const cmp_store_model_info_stu *cmp_store_model_query(void)
{
    return &store_model.info;
}

/***************************************************************************************
 * @Function    : cmp_store_model_set()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/2
 ****************************************************************************************/
static uint8_t cmp_store_model_set(cmp_store_model_info_stu *model)
{
    cmp_store_model_stu model_s;

    if (model->model_cnt > CMP_STORE_MODEL_CONTENT_CNT || model->model_cnt == 0)
    {
        return 3;
    }

    for (uint8_t i = 0; i < model->model_cnt - 1; i++)
    {
        if ((model->content[i].start_time[0] * 60 + model->content[i].start_time[1]) > (model->content[i + 1].start_time[0] * 60 + model->content[i + 1].start_time[1]))
        {
            return 1;
        }
    }

    memcpy((uint8_t *)&model_s, (uint8_t *)&store_model, sizeof(model_s));
    memcpy(model_s.info.model_no, model->model_no, sizeof(model_s.info.model_no));
    memcpy((uint8_t *)model_s.info.content, (uint8_t *)model->content, sizeof(cmp_store_model_info_content_stu) * model->model_cnt);
    model_s.info.model_cnt = model->model_cnt;

    if (model_conf.ext_flash->drv_flash_erase_sector(model_conf.param.model_addr) > 0)
    {
        return 1;
    }
    if (model_conf.ext_flash->drv_flash_write_data(model_conf.param.model_addr, (uint8_t *)&model_s, sizeof(model_s)) > 0)
    {
        return 1;
    }

    /* upate info */
    memcpy((uint8_t *)&store_model, (uint8_t *)&model_s, sizeof(model_s));

    return 0;
}

/***************************************************************************************
 * @Function    : cmp_store_model_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/6/2
 ****************************************************************************************/
static uint8_t cmp_store_model_init(void)
{
    if (model_conf.ext_flash->drv_flash_read_data(model_conf.param.model_addr, (uint8_t *)&store_model, sizeof(store_model)) > 0)
    {
        return 2;
    }

    if (store_model.store_flag == CMP_STORE_FLAG)
    {
        return 0;
    }

    memset((uint8_t *)&store_model, 0, sizeof(store_model));
    store_model.store_flag = CMP_STORE_FLAG;
    if (model_conf.ext_flash->drv_flash_erase_sector(model_conf.param.model_addr) > 0)
    {
        return 3;
    }
    if (model_conf.ext_flash->drv_flash_write_data(model_conf.param.model_addr, (uint8_t *)&store_model, sizeof(store_model)) > 0)
    {
        return 3;
    }

    return 0;
}

/***************************************************************************************
 * @Function    : cmp_store_model_set_flash()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/10/9
 ****************************************************************************************/
static void cmp_store_model_set_flash(drv_flash_dev_stu *ext_flash, uint8_t type)
{
    model_conf.ext_flash = ext_flash;

    model_conf.param.model_addr = type == 0 ? CMP_STORE_EXT_ADDR_MODEL : CMP_STORE_INN_ADDR_MODEL;

    cmp_store_model_init();
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
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/10/9
 ****************************************************************************************/
int cmp_store_model_subdev_create(void)
{
    static cmp_store_model_dev_stu model_dev;

    model_dev.cmp_store_model_query = cmp_store_model_query;
    model_dev.cmp_store_model_set = cmp_store_model_set;
    model_dev.cmp_set_ext_flash = cmp_store_model_set_flash;

    cmp_dev_append(CMP_DEV_NAME_MODEL, &model_dev);
    return 0;
}
//INIT_COMPONENT_EXPORT(cmp_store_model_dev_create);
