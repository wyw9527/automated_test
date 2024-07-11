#include "cmp_store_param.h"

#define PARAM_VAL_NUM 12
#define PARAM_STR_NUM 5
#define CMP_STORE_REDIS_ON 0xAA
#define CMP_STORE_REDIS_OFF 0x55

static cmp_store_param_stu param_conf;

static cmp_store_param_val_stu param_val[PARAM_VAL_NUM] =
    {
        // set_idx	len	chrg_en	parm_type		min	max	def
        {15, 1, 0, STORE_PARAM_SOCK_STYLE, 0, 1, 0},                      // STORE_PARAM_VAL_SOCK_STYLE
        {2, 1, 0, STORE_PARAM_WORK_MODE, 0, 2, 0},                        // STORE_PARAM_VAL_WORK_MODE
        {3, 2, 1, STORE_PARAM_MAX_CURR, 6, CHRG_CURR_MAX, CHRG_CURR_MAX}, // STORE_PARAM_VAL_MAX_CURR
        {7, 2, 1, STORE_PARAM_SERV_PORT, 0, 65535, 1},                    // STORE_PARAM_VAL_SERV_PORT
        {8, 2, 1, STORE_PARAM_LEAK_TH, 30, 800, 60},                      // STORE_PARAM_VAL_LEAK_TH
        {9, 1, 1, STORE_PARAM_PLB_EN, 0, 1, 0},                           // STORE_PARAM_VAL_PLB_EN
        {10, 2, 1, STORE_PARAM_PLB_TH, 10, 300, 30},                      // STORE_PARAM_VAL_PLB_TH
        {11, 1, 1, STORE_PARAM_SLOAR_MODE, 0, 3, 0},                      // STORE_PARAM_VAL_SLOAR_MODE
        {12, 1, 0, STORE_PARAM_OPER_TYPE, 0, 1, 0},                       // STORE_PARAM_VAL_OPER_TYPE
        {14, 1, 0, STORE_PARAM_SLB_EN, 0, 1, 0},                          // STORE_PARAM_VAL_SLB_EN
        {16, 1, 1, STORE_PARAM_PEAK_EN, 0, 1, 0},                         // STORE_PARAM_VAL_PEAK_EN
        {17, 1, 1, STORE_PARAM_LIGHT, 0, 100, 100},                       // STORE_PARAM_VAL_LIGHT
};

static cmp_store_param_str_stu param_str[PARAM_STR_NUM] =
    {
        // set_idx	len	chrg_en	parm_type 		def
        {1, 29, 0, STORE_PARAM_SN, "FOXESS-CHARGER"}, // STORE_PARAM_STR_SN
        {4, 33, 1, STORE_PARAM_WIFI_SSID, ""},        // STORE_PARAM_STR_WIFI_SSID
        {5, 33, 1, STORE_PARAM_WIFI_PWD, ""},         // STORE_PARAM_STR_WIFI_PWD
        {6, 49, 1, STORE_PARAM_SERV_ADDR, ""},        // STORE_PARAM_STR_SERV_ADDR
        {13, 25, 1, STORE_PARAM_SIM_NO, ""},          // STORE_PARAM_STR_SIM_NO
};

/***************************************************************************************
 * @Function    : cmp_store_param_get_save_type()
 *
 * @Param       :
 *
 * @Return      : 0:none  1 sys  2 set
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/10/8
 ****************************************************************************************/
static uint8_t cmp_store_param_get_save_type_by_pty(uint8_t param_type)
{
    if (param_type <= STORE_PARAM_SOCK_STYLE)
    {
        return 0;
    }

    if (param_type <= STORE_PARAM_LIGHT)
    {
        return 1;
    }
    return 0xff;
}

/***************************************************************************************
 * @Function    : cmp_store_param_get_type_by_setidx()
 *
 * @Param       : data_type 0 数值类型  1 字符串类型   save_type 0 sys   1 set
 *
 * @Return      : 0xff: param not exit  other:idx
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/10/8
 ****************************************************************************************/
static uint8_t cmp_store_param_get_type_by_setidx(uint8_t set_idx, uint8_t *save_type, uint8_t *data_type)
{
    uint8_t save_type_tmp;

    // 数值类型中查找
    for (uint8_t i = 0; i < PARAM_VAL_NUM; i++)
    {
        if (set_idx == param_val[i].idx)
        {
            if ((save_type_tmp = cmp_store_param_get_save_type_by_pty(param_val[i].parm_type)) > 1)
                return 0xff;
            *data_type = 0;
            *save_type = save_type_tmp;
            return i;
        }
    }

    // 字符串类型中查找
    for (uint8_t i = 0; i < PARAM_STR_NUM; i++)
    {
        if (set_idx == param_str[i].idx)
        {
            *data_type = 1;
            *save_type = param_str[i].parm_type <= STORE_PARAM_SOCK_STYLE ? 0 : 1;
            return i;
        }
    }

    return 0xff;
}

/***************************************************************************************
 * @Function    : cmp_store_param_get_len()
 *
 * @Param       :
 *
 * @Return      : 0 fail   1 succ
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/27
 ****************************************************************************************/
static uint8_t cmp_store_param_get_len(uint8_t set_idx, uint8_t *len)
{
    uint8_t idx, save_type, data_type;

    if ((idx = cmp_store_param_get_type_by_setidx(set_idx, &save_type, &data_type)) == 0xff)
        return 0;

    if (data_type == 0 && idx < PARAM_VAL_NUM)
    {
        *len = param_val[idx].len;
        return 1;
    }

    if (data_type == 1 && idx < PARAM_STR_NUM)
    {
        *len = param_str[idx].len - 1;
        return 1;
    }
    return 0;
}

/***************************************************************************************
 * @Function    : cmp_store_param_get_redis_data_buff_pos()
 *
 * @Param       : param_type:当前要找的枚举类型  save_type:存储类型 0 sys 1 set  pos_type 0 当前变量位置  1 下一个存储位置
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/10/8
 ****************************************************************************************/
static uint8_t cmp_store_param_get_redis_data_buff_pos(uint8_t param_type, uint8_t save_type, uint8_t *data_pos)
{
    uint8_t len = 0, last_len = 0, flag = 0, start;

    start = save_type == 0 ? STORE_PARAM_SN : STORE_PARAM_WORK_MODE;

    // 数值类型
    for (uint8_t i = start; i <= param_type; i++)
    {
        uint8_t res = 0;
        // 通过type找到param
        for (uint8_t j = 0; j < PARAM_VAL_NUM; j++)
        {
            if (param_val[j].parm_type == i)
            {
                len += last_len;
                last_len = param_val[j].len;

                flag++;
                res = 1;
                break;
            }
        }
        if (res > 0)
            continue;
        for (uint8_t j = 0; j < PARAM_STR_NUM; j++)
        {
            if (param_str[j].parm_type == i)
            {
                len += last_len;
                last_len = param_str[j].len;
                flag++;
                break;
            }
        }
    }

    *data_pos = len;
    return flag > 0 ? 1 : 0;
}

/***************************************************************************************
 * @Function    : cmp_store_param_redis_val_sys()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/10/8
 ****************************************************************************************/
static uint8_t cmp_store_param_redis_sys(uint8_t param_type, uint8_t save_type, uint8_t *data, uint8_t len)
{
    uint8_t save_pos;

    if (cmp_store_param_get_redis_data_buff_pos(param_type, save_type, &save_pos) == 0)
        return 2;
    if (param_conf.procc.redis.sys_redis_flag != CMP_STORE_REDIS_ON)
    {
        memcpy(param_conf.procc.redis.sys_buff, param_conf.procc.param.sys_buff, CMP_STORE_PARAM_SYS_BUFF_SIZE);
        param_conf.procc.redis.sys_redis_flag = CMP_STORE_REDIS_ON;
    }

    memset(&param_conf.procc.redis.sys_buff[save_pos], 0, len);
    memcpy(&param_conf.procc.redis.sys_buff[save_pos], data, len);
    return 0;
}

/***************************************************************************************
 * @Function    : cmp_store_param_redis_val_sets()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/10/8
 ****************************************************************************************/
static uint8_t cmp_store_param_redis_sets(uint8_t param_type, uint8_t save_type, uint8_t *data, uint8_t len)
{
    uint8_t data_pos = 0;

    if (cmp_store_param_get_redis_data_buff_pos(param_type, save_type, &data_pos) == 0)
        return 2;

    if (param_conf.procc.redis.set_redis_flag != CMP_STORE_REDIS_ON)
    {
        memcpy(param_conf.procc.redis.set_buff, param_conf.procc.param.set_buff, CMP_STORE_PARAM_SET_BUFF_SIZE);
        param_conf.procc.redis.set_redis_flag = CMP_STORE_REDIS_ON;
    }

    memset(&param_conf.procc.redis.set_buff[data_pos], 0, len);
    memcpy(&param_conf.procc.redis.set_buff[data_pos], data, len);
    return 0;
}

/***************************************************************************************
 * @Function    : cmp_store_param_redis_data()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/10/8
 ****************************************************************************************/
static uint8_t cmp_store_param_redis_data(uint8_t param_type, uint8_t save_type, uint8_t *data, uint8_t len)
{
    if (save_type == 0)
    {
        return cmp_store_param_redis_sys(param_type, save_type, data, len);
    }

    // 存储定值参数
    if (save_type == 1)
    {
        return cmp_store_param_redis_sets(param_type, save_type, data, len);
    }

    return 3;
}

/***************************************************************************************
 * @Function    : cmp_store_param_val_redis()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/10/8
 ****************************************************************************************/
static uint8_t cmp_store_param_redis_val(cmp_store_param_val_stu *val_obj, uint8_t save_type, uint8_t *data, uint8_t len, uint8_t cur_chrg_sta)
{
    uint32_t val = 0;

    // 当前在充电
    if (cur_chrg_sta > 0)
    {
        if (val_obj->chrg_enable == 0)
            return 4;
    }

    // 判断参数是否合法
    if (len != val_obj->len)
        return 3;
    memcpy((uint8_t *)&val, data, len);
    if (val > val_obj->max || val < val_obj->min)
        return 3;

    // 存储系统参数
    return cmp_store_param_redis_data(val_obj->parm_type, save_type, data, len);
}

/***************************************************************************************
 * @Function    : cmp_store_param_str_redis()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/10/8
 ****************************************************************************************/
static uint8_t cmp_store_param_redis_str(cmp_store_param_str_stu *str_obj, uint8_t save_type, uint8_t *data, uint8_t len, uint8_t cur_chrg_sta)
{
    // 当前在充电
    if (cur_chrg_sta > 0)
    {
        if (str_obj->chrg_enable == 0)
            return 4;
    }

    // 判断参数是否合法
    if (len > str_obj->len - 1)
        return 3;

    return cmp_store_param_redis_data(str_obj->parm_type, save_type, data, len);
}

/***************************************************************************************
 * @Function    : cmp_store_param_redis()
 *
 * @Param       :
 *
 * @Return      :  0 succ   1 flash abn 2 param not exit 3 param not vavil 4 非允许更改状态
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/27
 ****************************************************************************************/
static uint8_t cmp_store_param_set_redis(uint8_t set_idx, uint8_t *data, uint8_t len, uint8_t cur_chrg_sta)
{
    uint8_t data_type, save_type, idx;

    if ((idx = cmp_store_param_get_type_by_setidx(set_idx, &save_type, &data_type)) == 0xff)
        return 2;

    // 数值类型存储
    if (data_type == 0 && idx < PARAM_VAL_NUM)
    {
        return cmp_store_param_redis_val(&param_val[idx], save_type, data, len, cur_chrg_sta);
    }

    // 字符类型存储
    if (data_type == 1 && idx < PARAM_STR_NUM)
    {
        return cmp_store_param_redis_str(&param_str[idx], save_type, data, len, cur_chrg_sta);
    }

    return 2;
}

/***************************************************************************************
 * @Function    : cmp_store_param_save_redis_action()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/10/8
 ****************************************************************************************/
static uint8_t cmp_store_param_save_redis_action(uint8_t *flag, uint32_t addr, uint8_t *buff, uint16_t buff_size, uint8_t *dst_buff)
{
    if (*flag != CMP_STORE_REDIS_ON)
        return 0;

    /* erease sector */
    if (param_conf.ext_flash->drv_flash_erase_sector(addr) > 0)
        return 1;
    /* save data */
    if (param_conf.ext_flash->drv_flash_write_data(addr, buff, buff_size) > 0)
        return 1;
    /* update date to locate */
    memcpy(dst_buff, buff, buff_size);
    /* clear redis*/
    *flag = CMP_STORE_REDIS_OFF;

    return 0;
}

/***************************************************************************************
 * @Function    : cmp_store_param_save_redis()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/27
 ****************************************************************************************/
static uint8_t cmp_store_param_save_redis(void)
{
    uint8_t ret1, ret2;

    ret1 = cmp_store_param_save_redis_action(&param_conf.procc.redis.sys_redis_flag, param_conf.data.sys_addr, param_conf.procc.redis.sys_buff, CMP_STORE_PARAM_SYS_BUFF_SIZE, param_conf.procc.param.sys_buff);
    ret2 = cmp_store_param_save_redis_action(&param_conf.procc.redis.set_redis_flag, param_conf.data.set_addr, param_conf.procc.redis.set_buff, CMP_STORE_PARAM_SET_BUFF_SIZE, param_conf.procc.param.set_buff);

    return ret1 | ret2;
}

/***************************************************************************************
 * @Function    : cmp_store_param_query()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/27
 ****************************************************************************************/
static const uint8_t *cmp_store_param_query(uint8_t set_idx)
{
    uint8_t data_type, idx, data_pos, save_type = 0, param_type;

    idx = cmp_store_param_get_type_by_setidx(set_idx, &save_type, &data_type);
    if (idx == 0xff)
        return RT_NULL;

    param_type = data_type == 0 ? param_val[idx].parm_type : param_str[idx].parm_type;
    if (cmp_store_param_get_redis_data_buff_pos(param_type, save_type, &data_pos) == 0)
        return RT_NULL;

    if (save_type == 0)
    {
        if (data_pos >= CMP_STORE_PARAM_SYS_BUFF_SIZE)
            return RT_NULL;
        return &param_conf.procc.param.sys_buff[data_pos];
    }
    else if (save_type == 1)
    {
        if (data_pos >= CMP_STORE_PARAM_SET_BUFF_SIZE)
            return RT_NULL;
        return &param_conf.procc.param.set_buff[data_pos];
    }

    return RT_NULL;
}

/***************************************************************************************
 * @Function    : cmp_store_param_vival_check()
 *
 * @Param       :
 *
 * @Return      : 0 合法  1 不合法
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/10/8
 ****************************************************************************************/
static uint8_t cmp_store_param_sys_vival_check(uint8_t start, uint8_t end, uint8_t *buff)
{
    uint8_t flag, vival_flag = 0;
    uint32_t item_val = 0, pre_len = 0, def;

    for (uint8_t i = start; i <= end; i++)
    {
        flag = 0;

        // 定值中寻找
        for (uint8_t j = 0; j < PARAM_VAL_NUM; j++)
        {
            if (param_val[j].parm_type != i)
                continue;
            item_val = 0;
            memcpy((uint8_t *)&item_val, &buff[pre_len], param_val[j].len);
            if (item_val > param_val[j].max || item_val < param_val[j].min)
            {
                def = param_val[j].def;
                memcpy(&buff[pre_len], (uint8_t *)&def, param_val[j].len);
                vival_flag = 1;
            }
            pre_len += param_val[j].len;
            flag++;
            break;
        }

        if (flag > 0)
            continue;

        // 字符串中寻找
        for (uint8_t j = 0; j < PARAM_STR_NUM; j++)
        {
            if (param_str[j].parm_type != i)
                continue;

            if (strlen((char *)&buff[pre_len]) > param_str[j].len - 1)
            {
                memset(&buff[pre_len], 0, param_str[j].len);
                strncpy((char *)&buff[pre_len], param_str[j].def, param_str[j].len - 1);
                vival_flag = 1;
            }
            pre_len += param_str[j].len;
            break;
        }
    }
    return vival_flag;
}

/***************************************************************************************
 * @Function    : cmp_store_param_set_flash()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/10/8
 ****************************************************************************************/
static void cmp_store_param_set_flash(drv_flash_dev_stu *flash, uint8_t type)
{
    uint8_t cnt = 4;

    param_conf.ext_flash = flash;
    if (type == 0)
    {
        param_conf.data.sys_addr = CMP_STORE_EXT_ADDR_PARAM_SYS;
        param_conf.data.set_addr = CMP_STORE_EXT_ADDR_PARAM_SET;
    }
    else
    {
        param_conf.data.sys_addr = CMP_STORE_INN_ADDR_PARAM_SYS;
        param_conf.data.set_addr = CMP_STORE_INN_ADDR_PARAM_SET;
    }

    if (param_conf.ext_flash == RT_NULL)
        return;

    while (cnt--)
    {
        if (param_conf.ext_flash->drv_flash_read_data(param_conf.data.sys_addr, param_conf.procc.param.sys_buff, CMP_STORE_PARAM_SYS_BUFF_SIZE) == 0)
            break;
        rt_thread_mdelay(40);
    }
    cnt = 4;
    while (cnt--)
    {
        if (param_conf.ext_flash->drv_flash_read_data(param_conf.data.set_addr, param_conf.procc.param.set_buff, CMP_STORE_PARAM_SET_BUFF_SIZE) == 0)
            break;
        rt_thread_mdelay(40);
    }

    if (cmp_store_param_sys_vival_check(STORE_PARAM_SN, STORE_PARAM_SOCK_STYLE, param_conf.procc.param.sys_buff) > 0)
    {
        cmp_store_param_save_redis_action(&param_conf.procc.redis.sys_redis_flag, param_conf.data.sys_addr, param_conf.procc.param.sys_buff, CMP_STORE_PARAM_SYS_BUFF_SIZE, param_conf.procc.param.sys_buff);
    }

    if (cmp_store_param_sys_vival_check(STORE_PARAM_WORK_MODE, STORE_PARAM_LIGHT, param_conf.procc.param.set_buff) > 0)
    {
        cmp_store_param_save_redis_action(&param_conf.procc.redis.set_redis_flag, param_conf.data.set_addr, param_conf.procc.param.set_buff, CMP_STORE_PARAM_SET_BUFF_SIZE, param_conf.procc.param.set_buff);
    }
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
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/9/27
 ****************************************************************************************/
int cmp_store_param_subdev_create(void)
{
    static cmp_store_param_dev_stu param_dev;

    param_dev.store_get_parameter_len = cmp_store_param_get_len;

    param_dev.store_parameter_set_redis = cmp_store_param_set_redis;

    param_dev.store_parameter_save_redis = cmp_store_param_save_redis;

    param_dev.store_parameter_query = cmp_store_param_query;

    param_dev.cmp_set_ext_flash = cmp_store_param_set_flash;

    cmp_dev_append(CMP_DEV_NAME_PARAM, &param_dev);

    return 0;
}
// INIT_COMPONENT_EXPORT(cmp_store_param_dev_create);