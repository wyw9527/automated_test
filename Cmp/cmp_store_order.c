#include "cmp_store_order.h"

static cmp_store_order_stu order_conf;

/***************************************************************************************
 * @Function    : cmp_order_write()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/25
 ****************************************************************************************/
static uint8_t cmp_order_write(uint32_t addr, cmp_store_order_info_stu *order)
{
    if (addr % 4096 == 0)
    {
        if (order_conf.ext_flash->drv_flash_erase_sector(addr) > 0)
        {
            return 1;
        }
    }

    if (order_conf.ext_flash->drv_flash_write_data(addr, (uint8_t *)order, sizeof(cmp_store_order_info_stu)) > 0)
    {
        return 1;
    }
    return 0;
}

/***************************************************************************************
 * @Function    : cmp_order_read()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/26
 ****************************************************************************************/
static uint8_t cmp_order_read(uint32_t order_addr, cmp_store_order_info_stu *order)
{
    if (order_conf.ext_flash->drv_flash_read_data(order_addr, (uint8_t *)order, sizeof(cmp_store_order_info_stu)) > 0)
    {
        return 1;
    }

    return 0;
}

/***************************************************************************************
 * @Function    : cmp_order_statics_read()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/26
 ****************************************************************************************/
static uint8_t cmp_order_statics_read(cmp_store_order_statics_stu *statics)
{
    if (order_conf.ext_flash->drv_flash_read_data(order_conf.param.statis_addr, (uint8_t *)statics, sizeof(cmp_store_order_statics_stu)) > 0)
    {
        return 1;
    }

    return 0;
}

/***************************************************************************************
 * @Function    : cmp_order_statics_idx_cal()
 *
 * @Param       : type  0 write  1 read
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/25
 ****************************************************************************************/
static void cmp_order_statics_idx_cal(CMP_STORE_ORDER_SAVE_TYPE save_type, cmp_store_order_statics_unit_stu *statics, uint16_t order_max_num)
{
    uint8_t one_page_order_num, mirror_flag;

    one_page_order_num = (4096 / CMP_STORE_ORDER_ONE_SIZE);
    // mirror_flag = statics->wr_mirror ^ statics->rd_mirror;

    // 写新索引
    if (save_type == STORE_ORDER_SAVE_WRITE)
    {
        // 增加写索引
        if (++statics->wr_idx >= order_max_num)
        {
            statics->wr_mirror = ~statics->wr_mirror;
            statics->wr_idx = 0;
        }
        mirror_flag = statics->wr_mirror ^ statics->rd_mirror;
        // 非同一页时，擦除时读取索引需要调整
        if (mirror_flag != 0)
        {
            if ((statics->wr_idx % one_page_order_num) == 0)
            {
                if (statics->rd_idx - statics->wr_idx <= one_page_order_num)
                {
                    statics->rd_idx = ((statics->rd_idx / one_page_order_num) + 1) * one_page_order_num;
                    if (statics->rd_idx >= order_max_num)
                    {
                        statics->rd_mirror = ~statics->rd_mirror;
                        statics->rd_idx = 0;
                    }
                }
            }
        }
    }
    // 读取索引
    else
    {
        mirror_flag = statics->wr_mirror ^ statics->rd_mirror;
        if (mirror_flag == 0)
        {
            if (statics->rd_idx < statics->wr_idx)
            {
                statics->rd_idx++;
            }
        }
        else
        {
            if (++statics->rd_idx >= order_max_num)
            {
                statics->rd_mirror = ~statics->rd_mirror;
                statics->rd_idx = 0;
            }
        }
    }
}

/***************************************************************************************
 * @Function    : cmp_order_statics_opera()
 *
 * @Param       : save_flag :0 save  other :do not save
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/25
 ****************************************************************************************/
static uint8_t cmp_order_statics_opera(CMP_STORE_ORDER_TYPE order_type, CMP_STORE_ORDER_SAVE_TYPE save_type, uint8_t save_flag)
{
    cmp_store_order_statics_stu statics_info;

    memcpy((uint8_t *)&statics_info, (uint8_t *)&order_conf.statics, sizeof(statics_info));
    if (order_type == STORE_ORDER_UNS)
    {
        cmp_order_statics_idx_cal(save_type, &statics_info.uns, order_conf.param.uns_max_cnt);
    }
    else if (order_type == STORE_ORDER_ABN)
    {
        cmp_order_statics_idx_cal(save_type, &statics_info.abn, order_conf.param.abn_max_cnt);
    }
    else if (order_type == STORE_ORDER_HIS)
    {
        cmp_order_statics_idx_cal(save_type, &statics_info.his, order_conf.param.his_max_cnt);
    }
    else
        return 1;

    if (order_conf.ext_flash->drv_flash_erase_sector(order_conf.param.statis_addr) > 0)
    {
        return 1;
    }
    if (save_flag == 0)
    {
        if (order_conf.ext_flash->drv_flash_write_data(order_conf.param.statis_addr, (uint8_t *)&statics_info, sizeof(statics_info)) > 0)
        {
            return 1;
        }
    }
    memcpy((uint8_t *)&order_conf.statics, (uint8_t *)&statics_info, sizeof(statics_info));

    return 0;
}

/***************************************************************************************
 * @Function    : cmp_order_opera()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/25
 ****************************************************************************************/
static uint8_t cmp_order_opera(CMP_STORE_ORDER_TYPE order_type, CMP_STORE_ORDER_SAVE_TYPE save_type, cmp_store_order_info_stu *order)
{
    uint8_t ret = 1;
    uint32_t addr;

    switch (order_type)
    {
    case STORE_ORDER_UNS:
        addr = order_conf.param.uns_addr + order_conf.statics.uns.wr_idx * CMP_STORE_ORDER_ONE_SIZE;
        break;

    case STORE_ORDER_ABN:
        addr = order_conf.param.abn_addr + order_conf.statics.abn.wr_idx * CMP_STORE_ORDER_ONE_SIZE;
        break;

    case STORE_ORDER_HIS:
        addr = order_conf.param.his_addr + order_conf.statics.his.wr_idx * CMP_STORE_ORDER_ONE_SIZE;
        break;
    default:
        return 2;
    }

    switch (save_type)
    {
    case STORE_ORDER_SAVE_READ:
        ret = cmp_order_read(addr, order);
        break;

    case STORE_ORDER_SAVE_WRITE:
        ret = cmp_order_write(addr, order);
        break;
    default:
        return 1;
    }

    return ret;
}

/***************************************************************************************
 * @Function    : cmp_store_order_add_uns()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description : 新增未发送订单
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/26
 ****************************************************************************************/
static uint8_t cmp_store_order_add_uns(cmp_store_order_info_stu *order)
{
    /* 存储订单 */
    if (cmp_order_opera(STORE_ORDER_UNS, STORE_ORDER_SAVE_WRITE, order) > 0)
        return 1;

    /* 统计数据增加 */
    if (cmp_order_statics_opera(STORE_ORDER_UNS, STORE_ORDER_SAVE_WRITE, 0) > 0)
        return 1;

    return 0;
}

/***************************************************************************************
 * @Function    : cmp_store_order_add_abn()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/26
 ****************************************************************************************/
static uint8_t cmp_store_order_add_abn(cmp_store_order_info_stu *order)
{
    /* 存储订单 */
    if (cmp_order_opera(STORE_ORDER_ABN, STORE_ORDER_SAVE_WRITE, order) > 0)
        return 1;

    /* 统计数据增加 */
    if (cmp_order_statics_opera(STORE_ORDER_UNS, STORE_ORDER_SAVE_READ, 1) > 0)
        return 1;

    if (cmp_order_statics_opera(STORE_ORDER_ABN, STORE_ORDER_SAVE_WRITE, 0) > 0)
        return 1;

    return 0;
}

/***************************************************************************************
 * @Function    : cmp_store_order_add_his()
 *
 * @Param       : src_type : 0 uns_order  1 abn_order
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/26
 ****************************************************************************************/
static uint8_t cmp_store_order_add_his(cmp_store_order_info_stu *order, uint8_t src_type)
{
    uint8_t ret = 1;

    /* 存储订单 */
    if (cmp_order_opera(STORE_ORDER_HIS, STORE_ORDER_SAVE_WRITE, order) > 0)
        return 1;

    /* 历史统计数据增加 */
    if (cmp_order_statics_opera(STORE_ORDER_HIS, STORE_ORDER_SAVE_WRITE, 1) > 0)
        return 1;

    /* 源统计数据减少 */
    if (src_type == 0)
    {
        ret = cmp_order_statics_opera(STORE_ORDER_UNS, STORE_ORDER_SAVE_READ, 0);
    }
    else
    {
        ret = cmp_order_statics_opera(STORE_ORDER_ABN, STORE_ORDER_SAVE_READ, 0);
    }
    return ret;
}

/***************************************************************************************
 * @Function    : cmp_store_get_statics_cnt()
 *
 * @Param       :
 *
 * @Return      : <0 异常  >=0 个数
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/26
 ****************************************************************************************/
static int8_t cmp_store_get_statics_cnt(cmp_store_order_statics_unit_stu *statics, uint16_t max_num)
{
    uint8_t mirror_flag;

    mirror_flag = statics->wr_mirror ^ statics->rd_mirror;

    // 不同页
    if (mirror_flag > 0)
    {
        if (statics->wr_idx > statics->rd_idx)
            return -1;
        return statics->wr_idx + max_num - statics->rd_idx;
    }

    if (statics->wr_idx < statics->rd_idx)
        return -1;

    return statics->wr_idx - statics->rd_idx;
}

/***************************************************************************************
 * @Function    : cmp_store_get_order_info()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/26
 ****************************************************************************************/
static uint8_t cmp_store_get_order_info(CMP_STORE_ORDER_TYPE order_type, uint16_t order_idx, cmp_store_order_info_stu *order)
{
    int8_t idx;
    uint32_t addr;

    switch (order_type)
    {
    case STORE_ORDER_UNS:
        idx = cmp_store_get_statics_cnt(&order_conf.statics.uns, order_conf.param.uns_max_cnt);
        addr = order_conf.param.uns_addr + ((order_conf.statics.uns.rd_idx + order_idx) % order_conf.param.uns_max_cnt) * CMP_STORE_ORDER_ONE_SIZE;
        break;

    case STORE_ORDER_ABN:
        idx = cmp_store_get_statics_cnt(&order_conf.statics.abn, order_conf.param.abn_max_cnt);
        addr = order_conf.param.abn_addr + ((order_conf.statics.abn.rd_idx + order_idx) % order_conf.param.abn_max_cnt) * CMP_STORE_ORDER_ONE_SIZE;
        break;

    case STORE_ORDER_HIS:
        idx = cmp_store_get_statics_cnt(&order_conf.statics.his, order_conf.param.his_max_cnt);
        addr = order_conf.param.his_addr + ((order_conf.statics.his.rd_idx + order_idx) % order_conf.param.his_max_cnt) * CMP_STORE_ORDER_ONE_SIZE;
        break;

    default:
        return 1;
    }

    if (order_idx >= idx)
        return 1;

    return cmp_order_read(addr, order);
}

/***************************************************************************************
 * @Function    : cmp_order_set_ext_flash()
 *
 * @Param       : type: 0 ext flash  1 inn flash
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/25
 ****************************************************************************************/
static void cmp_order_set_ext_flash(drv_flash_dev_stu *ext_flash, uint8_t type)
{
    int8_t cnt = 4;
    cmp_store_order_statics_stu statics_info;

    order_conf.ext_flash = ext_flash;
    if (type == 0)
    {
        order_conf.param.statis_addr = CMP_STORE_EXT_ADDR_ORDER_INFO;
        order_conf.param.uns_addr = CMP_STORE_EXT_ADDR_ORDER_UNSEND;
        order_conf.param.uns_max_cnt = CMP_STORE_EXT_ORDER_UNSEND_SIZE;
        order_conf.param.abn_addr = CMP_STORE_EXT_ADDR_ORDER_ABN;
        order_conf.param.abn_max_cnt = CMP_STORE_EXT_ORDER_ABN_SIZE;
        order_conf.param.his_addr = CMP_STORE_EXT_ADDR_ORDER_HIS;
        order_conf.param.his_max_cnt = CMP_STORE_EXT_ORDER_HIS_SIZE;
    }
    else
    {
        order_conf.param.statis_addr = CMP_STORE_INN_ADDR_ORDER_INFO;
        order_conf.param.uns_addr = CMP_STORE_INN_ADDR_ORDER_UNSEND;
        order_conf.param.uns_max_cnt = CMP_STORE_INN_ORDER_UNSEND_SIZE;
        order_conf.param.abn_addr = CMP_STORE_INN_ADDR_ORDER_ABN;
        order_conf.param.abn_max_cnt = CMP_STORE_INN_ORDER_ABN_SIZE;
        order_conf.param.his_addr = CMP_STORE_INN_ADDR_ORDER_HIS;
        order_conf.param.his_max_cnt = CMP_STORE_INN_ORDER_HIS_SIZE;
    }

    // 读取statics初始化参数
    while (cnt--)
    {
        if (cmp_order_statics_read(&statics_info) == 0)
            break;
        rt_thread_mdelay(20);
    }

    // 标准性检查
    if (statics_info.flag != CMP_STORE_FLAG)
    {
        memset((uint8_t *)&statics_info, 0, sizeof(cmp_store_order_statics_stu));
        statics_info.flag = CMP_STORE_FLAG;
    }
    // 合法性校验
    else
    {
        if (cmp_store_get_statics_cnt(&statics_info.uns, order_conf.param.uns_max_cnt) < 0 || cmp_store_get_statics_cnt(&statics_info.abn, order_conf.param.abn_max_cnt) < 0 || cmp_store_get_statics_cnt(&statics_info.his, order_conf.param.his_max_cnt) < 0)
        {
            memset((uint8_t *)&statics_info, 0, sizeof(cmp_store_order_statics_stu));
            statics_info.flag = CMP_STORE_FLAG;
        }
    }

    memcpy((uint8_t *)&order_conf.statics, (uint8_t *)&statics_info, sizeof(statics_info));
}

/***************************************************************************************
 * @Function    : cmp_store_get_statis_num()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/26
 ****************************************************************************************/
static uint16_t cmp_store_get_statis_num(CMP_STORE_ORDER_TYPE order_ty)
{
    int16_t num;

    if (order_ty == STORE_ORDER_UNS)
    {
        num = cmp_store_get_statics_cnt(&order_conf.statics.uns, order_conf.param.uns_max_cnt);
        return num < 0 ? 0 : num;
    }
    else if (order_ty == STORE_ORDER_ABN)
    {
        num = cmp_store_get_statics_cnt(&order_conf.statics.abn, order_conf.param.abn_max_cnt);
        return num < 0 ? 0 : num;
    }
    else if (order_ty == STORE_ORDER_HIS)
    {
        num = cmp_store_get_statics_cnt(&order_conf.statics.his, order_conf.param.his_max_cnt);
        return num < 0 ? 0 : num;
    }

    return 0;
}

/***************************************************************************
*@Function    :cmp_get_order_bak_info_func
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-12-20
***************************************************************************/
static uint8_t cmp_get_order_bak_info_func(drv_flash_dev_stu *inn_flash, drv_bcd_datetime_stu *bak_clock)
{
    cmp_store_order_info_stu order;

    memset((uint8_t *)&order, 0, sizeof(cmp_store_order_info_stu));
    memset((uint8_t *)&order_conf.procc.order_bak, 0, sizeof(cmp_store_order_bak_stu));

    // 从flash读备份内容
    inn_flash->drv_flash_read_data(CMP_STORE_INN_ADDR_PROCC_BAK_ADDR, (uint8_t *)&order_conf.procc.order_bak.bak_procc, sizeof(cmp_store_procc_info_stu));
    // 拷贝备份的时钟
    memcpy((uint8_t *)bak_clock, (uint8_t *)&order_conf.procc.order_bak.bak_procc.bak_clock, sizeof(drv_bcd_datetime_stu));
    // 判断是否存在掉电订单
    if (order_conf.procc.order_bak.bak_procc.bak_flag == CMP_STORE_FLAG)
    {
        // 读出订单内容
        inn_flash->drv_flash_read_data(CMP_STORE_INN_ADDR_ORDER_BAK_ADDR, (uint8_t *)&order, sizeof(cmp_store_order_info_stu));
        if (order.start_chrg_type == 0xFF)
        {
            inn_flash->drv_flash_erase_area(CMP_STORE_INN_ADDR_PROCC_BAK_ADDR, 1);
            return 0;
        }
        if (order.model_period_cnt == 0xFF)
        {
            inn_flash->drv_flash_erase_area(CMP_STORE_INN_ADDR_PROCC_BAK_ADDR, 1);
            return 0;
        }
        // 添加到未发送订单区
        cmp_store_order_add_uns(&order);
    }
    // 重置备份区
    inn_flash->drv_flash_erase_area(CMP_STORE_INN_ADDR_PROCC_BAK_ADDR, 1);

    return 0;
}

/***************************************************************************
*@Function    :cmp_save_order_bak_info_func
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-12-21
***************************************************************************/
static uint8_t cmp_save_order_bak_info_func(cmp_store_order_bak_stu *order_bak, drv_flash_dev_stu *inn_flash)
{
    inn_flash->drv_flash_write_data(CMP_STORE_INN_ADDR_PROCC_BAK_ADDR, (uint8_t *)&order_bak->bak_procc, sizeof(cmp_store_procc_info_stu));
    if (order_bak->bak_procc.bak_flag == CMP_STORE_FLAG)
    {
        inn_flash->drv_flash_write_data(CMP_STORE_INN_ADDR_ORDER_BAK_ADDR, (uint8_t *)order_bak->order, sizeof(cmp_store_order_info_stu));
    }

    return 0;
}

/***************************************************************************************
 * @Function    : cmp_stroe_order_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/25
 ****************************************************************************************/
int cmp_stroe_order_subdev_create(void)
{
    static cmp_store_order_dev_stu order_dev;

    order_dev.cmp_set_ext_flash = cmp_order_set_ext_flash;

    order_dev.cmp_unsend_add = cmp_store_order_add_uns;

    order_dev.cmp_abn_add = cmp_store_order_add_abn;

    order_dev.cmp_his_add = cmp_store_order_add_his;

    order_dev.cmp_get_order_info = cmp_store_get_order_info;

    order_dev.cmp_get_staitcs = cmp_store_get_statis_num;

    order_dev.cmp_get_order_bak_info = cmp_get_order_bak_info_func;

    order_dev.cmp_save_order_bak_info = cmp_save_order_bak_info_func;

    cmp_dev_append(CMP_DEV_NAME_ORDER, &order_dev);

    return 0;
}
// INIT_COMPONENT_EXPORT(cmp_stroe_order_dev_create);