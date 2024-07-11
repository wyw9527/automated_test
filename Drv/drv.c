#include "drv.h"

static bsp_dev_lists *drv_dev_list_p = RT_NULL;

/***************************************************************************************
 * @Function    : drv_dev_append()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/9
 ****************************************************************************************/
uint8_t drv_dev_append(const char *name, void *dev)
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

    if (drv_dev_list_p == RT_NULL)
    {
        drv_dev_list_p = new_dev;
        return 1;
    }

    bsp_dev_lists *dev_p = drv_dev_list_p;
    while (dev_p->next != RT_NULL)
    {
        dev_p = dev_p->next;
    }

    dev_p->next = new_dev;
    return 1;
}

/***************************************************************************************
 * @Function    : drv_dev_find()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/5/9
 ****************************************************************************************/
void *drv_dev_find(const char *name)
{
    bsp_dev_lists *dev_p = drv_dev_list_p;

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
 * @Function    : drv_u16_swap()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/31
 ****************************************************************************************/
static void drv_s16_swap(int16_t *val_a, int16_t *val_b)
{
    int16_t tmp = *val_a;
    *val_a = *val_b;
    *val_b = tmp;
}

/***************************************************************************************
 * @Function    : drv_sort_u16_data()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/31
 ****************************************************************************************/
void drv_sort_s16_data(int16_t *arr, uint16_t len)
{
    for (uint32_t i = 0; i < len - 1; ++i)
    {
        uint32_t max_idx = 0;
        for (uint32_t j = 0; j < len - i; ++j)
        {
            if (arr[j] > arr[max_idx])
            {
                max_idx = j;
            }
        }
        drv_s16_swap(&arr[max_idx], &arr[len - 1 - i]);
    }
}

/***************************************************************************************
 * @Function    : drv_get_u16_avage()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/31
 ****************************************************************************************/
int16_t drv_get_s16_avage(int16_t *data, uint16_t len)
{
    uint32_t sum = 0;

    for (uint16_t i = 0; i < len; i++)
    {
        sum += data[i];
    }

    int16_t ave = sum / len;

    return ave;
}

/***************************************************************************
 * FuncName     : xor_check()
 *
 * Desc         :
 *
 * Change Logs  :
 *
 * Author       : chenbao         Version:    V1.0      Date:    2022/4/18
 ***************************************************************************/
uint8_t xor_check(uint8_t *buff, uint16_t len)
{
    uint8_t xor_sum = buff[0];

    for (uint16_t i = 1; i < len; i++)
    {
        xor_sum ^= buff[i];
    }

    return xor_sum;
}

/*!
 *  功  能: 校验和
 *  param1: 指向要校验的数据的指针
 *  param2: 要校验的数据的长度
 */
uint8_t power_box_msg_sum_check(uint8_t *data, uint8_t length) 
{
    uint8_t total_sum = 0;

    for (uint8_t i = 0; i < length; i++) {
        total_sum += data[i];
    }
    uint8_t checksum = total_sum & 0xFF;

    return checksum;
}

/***************************************************************************************
 * @Function    : math_int_2_bcd()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/2/22
 ****************************************************************************************/
uint8_t math_decimal_2_bcd(uint8_t decimal)
{
    return (decimal + (decimal / 10) * 6);
}

/***************************************************************************************
 * @Function    : math_bcd_2_decimal()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V2.0.0                Date:2023/2/23
 ****************************************************************************************/
uint8_t math_bcd_2_decimal(uint8_t bcd)
{
    return ((bcd >> 4) * 10 + (bcd & 0x0f));
}

/***************************************************************************************
 * @Function    : crc32_calc()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/15
 ****************************************************************************************/
uint32_t crc32_calc(uint8_t *buf, uint32_t len)
{
    int i, j;
    uint32_t crc, mask;

    crc = 0xFFFFFFFF;
    for (i = 0; i < len; i++)
    {
        crc = crc ^ (uint32_t)buf[i];
        for (j = 7; j >= 0; j--)
        { // Do eight times.
            mask = -(crc & 1);
            crc = (crc >> 1) ^ (0xEDB88320 & mask);
        }
    }
    crc = ~crc;
    return crc;
}

#define HIG_UINT16(a) (((a) >> 8) & 0xFF)
#define LOW_UINT16(a) ((a) & 0xFF)

#define HIG_UINT8(a) (((a) >> 4) & 0x0F)
#define LOW_UINT8(a) ((a) & 0x0F)

/* CRC16 余式表 */
static uint16_t crctalbeabs[] = {
    0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401,
    0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400};

/*!
 *  功  能: CRC16校验
 *  param1: 指向要校验的数据的指针
 *  param2: 要校验的数据的长度
 *  retval: 校验所得到的值，uint16_t 类型
 *
 *  说  明: 本次CRC校验为查表法，多项式为 x16+x15+x2+1(0x8005)，CRC的初始值为0xFFFF
 */
uint16_t crc16_calc(uint8_t *ptr, uint32_t len)
{
    uint16_t crc = 0xffff;
    uint32_t i;
    uint8_t ch;

    for (i = 0; i < len; i++)
    {
        ch = *ptr++;
        crc = crctalbeabs[(ch ^ crc) & 15] ^ (crc >> 4);
        crc = crctalbeabs[((ch >> 4) ^ crc) & 15] ^ (crc >> 4);
    }

    return crc;
}

/*!
 *  功  能: 将十进制转换为两个字节的十六进制，并按照高位在前的顺序放进一个数组的指定位置
 *  param1: 数组的指定起始位置
 *  param2: 十进制数
 */
void sort_high_byte_first(uint8_t *msg, uint16_t val) {
    msg[0] = (val >> 8) & 0xFF;  // 高字节
    msg[1] = val & 0xFF;         // 低字节
}

void sort_low_byte_first(uint8_t *msg, uint32_t value) {
    msg[0] = value & 0xFF;          // 最低字节
    msg[1] = (value >> 8) & 0xFF;   // 次低字节
    msg[2] = (value >> 16) & 0xFF;  // 次高字节
    msg[3] = (value >> 24) & 0xFF;  // 最高字节
    msg[4] = (value >> 24) & 0xFF;  // 高字节补0
    msg[5] = 0x00;                  // 低字节补0
    msg[6] = 0x00;                  // 低字节补0
    msg[7] = 0x00;                  // 低字节补0
}

/***************************************************************************************
 * @Function    : hex_to_float()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : RenGX            Version   : V3.0.0                Date:2023/11/15
 ****************************************************************************************/
float hex_to_float(uint8_t *data)
{
    float temp = 0;

    *((char *)(&temp)) = data[3];
    *((char *)(&temp) + 1) = data[2];
    *((char *)(&temp) + 2) = data[1];
    *((char *)(&temp) + 3) = data[0];

    return temp;
}

/***************************************************************************
*@Function    :
*
*@Param       :max-最大值 min-最小值 seed-随机数种子
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-12-13
***************************************************************************/
uint16_t get_rond_num(uint16_t max, uint16_t min, uint16_t seed)
{
    srand(seed);
    return rand() % (max - min + 1) + min;
}

