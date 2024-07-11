#include "cmp_store_ota.h"

static cmp_store_ota_stu store_ota;
static cmp_store_ota_c_stu ota_conf;

/***************************************************************************************
 * @Function    : cmp_store_ota_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/15
 ****************************************************************************************/
static uint8_t cmp_store_ota_init(void)
{
    ota_conf.inn_flash->drv_flash_read_data(CMP_STORE_INN_ADDR_APP_CONF, (uint8_t *)&store_ota, sizeof(store_ota));
    return 0;
}

/***************************************************************************************
 * @Function    : cmp_store_ota_start()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/15
 ****************************************************************************************/
static uint8_t cmp_store_ota_start(cmp_store_ota_info_stu *ota_info)
{
    if (memcmp((uint8_t *)&store_ota.info, (uint8_t *)ota_info, sizeof(cmp_store_ota_info_stu)) != 0)
    {
        /* 保存信息 */
        memcpy((uint8_t *)&store_ota.info, (uint8_t *)ota_info, sizeof(cmp_store_ota_info_stu));

        /* 重置索引 */
        memset((uint8_t *)&store_ota.procc, 0, sizeof(store_ota.procc));
        memset((uint8_t *)&store_ota.redis, 0, sizeof(store_ota.redis));
        return 0;
    }

    return store_ota.procc.ota_pack_idx;
}

/***************************************************************************************
 * @Function    : cmp_store_ota_data_redis_sector()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/9
 ****************************************************************************************/
static void cmp_store_ota_data_redis_sector()
{
    uint32_t store_addr;

    store_addr = CMP_STORE_INN_ADDR_APP_BUFF + store_ota.procc.ota_addr_head;
    if (store_addr % CMP_STORE_INN_PAGE_SIZE == 0)
    {
        ota_conf.inn_flash->drv_flash_erase_sector(store_addr);
    }

    // 存储处理
    ota_conf.inn_flash->drv_flash_write_data(store_addr, store_ota.redis.ota_buff, CMP_STORE_OTA_REDIS_BUFF_SIZE);
    store_ota.procc.ota_addr_head += CMP_STORE_OTA_REDIS_BUFF_SIZE;

    // 缓存处理
    store_ota.redis.ota_buff_idx = 0;
    memset(store_ota.redis.ota_buff, 0, CMP_STORE_OTA_REDIS_BUFF_SIZE);
}

/***************************************************************************************
 * @Function    : cmp_store_ota_procc_save_data()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/15
 ****************************************************************************************/
static void cmp_store_ota_procc_save_data(uint8_t *data, uint16_t len)
{
    uint16_t buff_left_size, redis_left;

    /* 将数据存储到缓冲区 */
    buff_left_size = CMP_STORE_OTA_REDIS_BUFF_SIZE - store_ota.redis.ota_buff_idx;

    /* 足够缓存数据 */
    if (len < buff_left_size)
    {
        memcpy(&store_ota.redis.ota_buff[store_ota.redis.ota_buff_idx], data, len);
        store_ota.redis.ota_buff_idx += len;
        return;
    }

    /* 正好缓存数据 */
    if (len == buff_left_size)
    {
        memcpy(&store_ota.redis.ota_buff[store_ota.redis.ota_buff_idx], data, len);
        cmp_store_ota_data_redis_sector();
        return;
    }

    /* 不足存储 */
    memcpy(&store_ota.redis.ota_buff[store_ota.redis.ota_buff_idx], data, buff_left_size);
    cmp_store_ota_data_redis_sector();

    /* 缓存剩余部分 */
    redis_left = len - buff_left_size;
    memcpy(&store_ota.redis.ota_buff, &data[buff_left_size], redis_left);
    store_ota.redis.ota_buff_idx = redis_left;
}

/***************************************************************************************
 * @Function    : cmp_store_ota_check_soft()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/15
 ****************************************************************************************/
static uint8_t cmp_store_ota_check_soft(uint32_t soft_size, uint32_t crc)
{
    uint32_t store_addr;
    uint32_t soft_crc;

    /* 有还未存储的先存储 */
    if (store_ota.redis.ota_buff_idx > 0)
    {
        store_addr = CMP_STORE_INN_ADDR_APP_BUFF + store_ota.procc.ota_addr_head;
        if (store_addr % CMP_STORE_INN_PAGE_SIZE == 0)
        {
            ota_conf.inn_flash->drv_flash_erase_sector(store_addr);
        }
        ota_conf.inn_flash->drv_flash_write_data(store_addr, store_ota.redis.ota_buff, store_ota.redis.ota_buff_idx);
    }
    memset((uint8_t *)&store_ota, 0, sizeof(store_ota));
    soft_crc = crc32_calc((uint8_t *)CMP_STORE_INN_ADDR_APP_BUFF, soft_size);
    if (soft_crc != crc)
        return 1;

    store_ota.conf.byte_cnt = soft_size;
    store_ota.conf.err_flag = 0;
    store_ota.conf.new_soft_flag = CMP_STORE_NEW_SOFT_FLAG;
    store_ota.conf.crc32_val = soft_crc;

    ota_conf.inn_flash->drv_flash_erase_sector(CMP_STORE_INN_ADDR_APP_CONF);
    ota_conf.inn_flash->drv_flash_write_data(CMP_STORE_INN_ADDR_APP_CONF, (uint8_t *)&store_ota, sizeof(store_ota));
    return 0;
}

/***************************************************************************
*@Function    :cmp_store_ota_save_info
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-11-01
***************************************************************************/
static void cmp_store_ota_erae_info()
{
    ota_conf.inn_flash->drv_flash_erase_sector(CMP_STORE_INN_ADDR_APP_CONF);
    memset((uint8_t *)&store_ota.procc, 0, sizeof(store_ota.procc));
    memset((uint8_t *)&store_ota.redis, 0, sizeof(store_ota.redis));
}

/***************************************************************************
*@Function    :cmp_store_ota_read_info
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-11-01
***************************************************************************/
static uint32_t cmp_store_ota_read_info(void)
{

    uint32_t ota_info_flag;

    ota_conf.inn_flash->drv_flash_read_data(CMP_STORE_INN_ADDR_APP_CONF, (uint8_t *)&ota_info_flag, sizeof(ota_info_flag));
    return ota_info_flag;
}

/***************************************************************************************
 * @Function    : cmp_store_ota_save_break_info()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/15
 ****************************************************************************************/
static void cmp_store_ota_save_break_info(uint8_t pack_idx)
{
    store_ota.procc.ota_pack_idx = pack_idx;

    ota_conf.inn_flash->drv_flash_erase_sector(CMP_STORE_INN_ADDR_APP_CONF);
    ota_conf.inn_flash->drv_flash_write_data(CMP_STORE_INN_ADDR_APP_CONF, (uint8_t *)&store_ota, sizeof(store_ota));
}

/***************************************************************************************
 * @Function    : cmp_store_ota_set_flash()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/10/9
 ****************************************************************************************/
static void cmp_store_ota_set_flash(drv_flash_dev_stu *ext_flash)
{
    ota_conf.inn_flash = ext_flash;

    cmp_store_ota_init();
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
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/10/9
 ****************************************************************************************/
int cmp_store_ota_subdev_create(void)
{
    static cmp_store_ota_dev_stu ota_dev;

    ota_dev.store_ota_check_soft = cmp_store_ota_check_soft;
    ota_dev.store_ota_procc_save_data = cmp_store_ota_procc_save_data;
    ota_dev.store_ota_save_break_info = cmp_store_ota_save_break_info; // 保存断点信息
    ota_dev.store_ota_start = cmp_store_ota_start;
    ota_dev.store_ota_set_ext_flash = cmp_store_ota_set_flash;
    ota_dev.store_ota_read_info = cmp_store_ota_read_info;
    ota_dev.store_ota_erae_info = cmp_store_ota_erae_info;

    cmp_dev_append(CMP_DEV_NAME_OTA, &ota_dev);
    return 0;
}
// INIT_COMPONENT_EXPORT(cmp_store_ota_dev_create);
