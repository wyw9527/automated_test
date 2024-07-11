#include "cmp_bill.h"

static cmp_bill_pub_dev_stu bill_pub;
static cmp_bill_stu bill_conf[BSP_POLE_PLUG_NUM];

/***************************************************************************************
 * @Function    : cmp_bill_get_order_no()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/7
 ****************************************************************************************/
static void cmp_bill_get_order_no(uint8_t *order_no)
{
    static uint16_t order_idx = 1;

    bill_pub.clock->drv_get_bcd_datetime((drv_bcd_datetime_stu *)order_no);
    uint8_t val_1 = math_decimal_2_bcd((uint8_t)order_idx);
    uint8_t val_2 = math_decimal_2_bcd(order_idx >> 8);
    uint16_t val = ((uint16_t)val_2 << 8) | (uint16_t)val_1;
    memcpy(&order_no[6], (uint8_t *)&val, 2);
    order_idx++;
}

/***************************************************************************************
 * @Function    : cmp_bill_get_chrg_volt0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/27
 ****************************************************************************************/
static uint16_t cmp_bill_get_chrg_volt0(uint8_t phase)
{
    return bill_conf[0].measure->cmp_get_chrg_volt(phase);
}

/***************************************************************************************
 * @Function    : cmp_bill_get_chrg_curr0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/27
 ****************************************************************************************/
static uint16_t cmp_bill_get_chrg_curr0(uint8_t phase)
{
    return bill_conf[0].measure->cmp_get_chrg_curr(phase);
}

/***************************************************************************************
 * @Function    : cmp_bill_get_start_type()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/4
 ****************************************************************************************/
static uint8_t cmp_bill_get_start_type0(void)
{
    return bill_conf[0].procc.order.start_chrg_type;
}

/***************************************************************************************
 * @Function    : cmp_bill_get_chrg_money0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/27
 ****************************************************************************************/
static uint32_t cmp_bill_get_chrg_money0(void)
{
    return (bill_conf[0].procc.order.total_elec_money + bill_conf[0].procc.order.total_serv_money);
}

/***************************************************************************************
 * @Function    : cmp_bill_get_chrg_power0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/4
 ****************************************************************************************/
static uint32_t cmp_bill_get_chrg_power0(void)
{
    return bill_conf[0].measure->cmp_get_chrg_power();
}

/***************************************************************************************
 * @Function    : cmp_bill_get_chrg_energy0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/27
 ****************************************************************************************/
static uint32_t cmp_bill_get_chrg_energy0(void)
{
    return bill_conf[0].procc.order.chrg_energy;
}

/***************************************************************************************
 * @Function    : cmp_bill_get_chrg_time0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/7
 ****************************************************************************************/
static uint16_t cmp_bill_get_chrg_time0(void)
{
    return bill_conf[0].procc.order.chrg_time;
}

/***************************************************************************
*@Function    :
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-12-21
***************************************************************************/
static void cmp_set_order_bak_info()
{
    memset((uint8_t *)&bill_conf->procc.order_bak, 0, sizeof(cmp_store_order_bak_stu));
    bill_conf->procc.order_bak.bak_procc.bak_flag = CMP_STORE_FLAG;
    bill_conf->procc.order_bak.order = &bill_conf->procc.order;
    bill_conf->procc.order.stop_reason = 29;
}

/***************************************************************************************
 * @Function    : cmp_bill_chrg_by_card0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/7
 ****************************************************************************************/
static void cmp_bill_chrg_by_card0(uint8_t card_type, uint32_t uid)
{
    memset((uint8_t *)&bill_conf[0].procc.order, 0, sizeof(bill_conf[0].procc.order));
    bill_conf[0].procc.order.plug_idx = 0;
    bill_conf[0].procc.order.card_type = card_type;
    bill_conf[0].procc.order.card_uid = uid;
    bill_conf[0].procc.order.start_chrg_type = 0;
    cmp_bill_get_order_no(bill_conf[0].procc.order.order_no);
    bill_pub.clock->drv_get_bcd_datetime((drv_bcd_datetime_stu *)bill_conf[0].procc.order.start_time);
    memcpy(bill_conf[0].procc.order.stop_time, bill_conf[0].procc.order.start_time, sizeof(bill_conf[0].procc.order.start_time));
    // bill_conf[0].procc.order.start_energy = bill_pub.store->order->cmp_store_get_meter_val(0);
    bill_conf[0].procc.order.start_energy = bill_conf[0].measure->cmp_get_chrg_energy();
    bill_conf[0].procc.curr_energy = bill_conf[0].procc.order.start_energy;

    cmp_set_order_bak_info();
}

/***************************************************************************************
 * @Function    : cmp_bill_start_chrg_remote0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/12
 ****************************************************************************************/
static void cmp_bill_start_chrg_remote0(uint8_t *order_no)
{
    memset((uint8_t *)&bill_conf[0].procc.order, 0, sizeof(bill_conf[0].procc.order));
    bill_conf[0].procc.order.plug_idx = 0;
    bill_conf[0].procc.order.card_type = 0xff;
    bill_conf[0].procc.order.card_uid = 0xffffffff;
    bill_conf[0].procc.order.start_chrg_type = 3;
    memcpy(bill_conf[0].procc.order.order_no, order_no, sizeof(bill_conf[0].procc.order.order_no));
    bill_pub.clock->drv_get_bcd_datetime((drv_bcd_datetime_stu *)bill_conf[0].procc.order.start_time);
    memcpy(bill_conf[0].procc.order.stop_time, bill_conf[0].procc.order.start_time, sizeof(bill_conf[0].procc.order.start_time));
    // bill_conf[0].procc.order.start_energy = bill_pub.store->order->cmp_store_get_meter_val(0);
    bill_conf[0].procc.order.start_energy = bill_conf[0].measure->cmp_get_chrg_energy();
    bill_conf[0].procc.curr_energy = bill_conf[0].measure->cmp_get_chrg_energy();

    cmp_set_order_bak_info();
}

/***************************************************************************************
 * @Function    : cmp_bill_chrg_by_tim0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/12
 ****************************************************************************************/
static void cmp_bill_chrg_by_tim0(uint8_t type, uint8_t *order_no)
{
    memset((uint8_t *)&bill_conf[0].procc.order, 0, sizeof(bill_conf[0].procc.order));
    bill_conf[0].procc.order.plug_idx = 0;
    bill_conf[0].procc.order.card_type = 0xff;
    bill_conf[0].procc.order.card_uid = 0xffffffff;
    bill_conf[0].procc.order.start_chrg_type = 2;
    if (type == 0)
    {
        memcpy(bill_conf[0].procc.order.order_no, order_no, sizeof(bill_conf[0].procc.order.order_no));
    }
    else
    {
        cmp_bill_get_order_no(bill_conf[0].procc.order.order_no);
    }
    bill_pub.clock->drv_get_bcd_datetime((drv_bcd_datetime_stu *)bill_conf[0].procc.order.start_time);
    memcpy(bill_conf[0].procc.order.stop_time, bill_conf[0].procc.order.start_time, sizeof(bill_conf[0].procc.order.start_time));
    // bill_conf[0].procc.order.start_energy = bill_pub.store->order->cmp_store_get_meter_val(0);
    bill_conf[0].procc.order.start_energy = bill_conf[0].measure->cmp_get_chrg_energy();
    bill_conf[0].procc.curr_energy = bill_conf[0].measure->cmp_get_chrg_energy();

    cmp_set_order_bak_info();
}

/***************************************************************************************
 * @Function    : cmp_bill_start_chrg_pole0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/2
 ****************************************************************************************/
static void cmp_bill_start_chrg_pole0(uint8_t chrg_type)
{
    memset((uint8_t *)&bill_conf[0].procc.order, 0, sizeof(bill_conf[0].procc.order));
    bill_conf[0].procc.order.plug_idx = 0;
    bill_conf[0].procc.order.card_type = 0xff;
    bill_conf[0].procc.order.card_uid = 0xffffffff;
    bill_conf[0].procc.order.start_chrg_type = chrg_type;
    cmp_bill_get_order_no(bill_conf[0].procc.order.order_no);
    bill_pub.clock->drv_get_bcd_datetime((drv_bcd_datetime_stu *)bill_conf[0].procc.order.start_time);
    memcpy(bill_conf[0].procc.order.stop_time, bill_conf[0].procc.order.start_time, sizeof(bill_conf[0].procc.order.start_time));
    // bill_conf[0].procc.order.start_energy = bill_pub.store->order->cmp_store_get_meter_val(0);
    bill_conf[0].procc.order.start_energy = bill_conf[0].measure->cmp_get_chrg_energy();
    bill_conf[0].procc.curr_energy = bill_conf[0].procc.order.start_energy;

    cmp_set_order_bak_info();
}

/***************************************************************************************
 * @Function    : cmp_bill_get_cur_period_idx()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/2
 ****************************************************************************************/
static uint8_t cmp_bill_get_cur_period_idx(cmp_bill_stu *bill_conf_p)
{
    uint8_t period_idx;
    uint32_t cur_min, period_min;

    if (bill_conf_p->model->model_cnt > CMP_STORE_MODEL_CONTENT_CNT)
        return 0;
    cur_min = math_bcd_2_decimal(bill_conf_p->procc.order.stop_time[4]) * 60 + math_bcd_2_decimal(bill_conf_p->procc.order.stop_time[5]);
    period_idx = bill_conf_p->model->model_cnt;

    do
    {
        period_idx--;
        period_min = bill_conf_p->model->content[period_idx].start_time[0] * 60 + bill_conf_p->model->content[period_idx].start_time[1];
        if (cur_min >= period_min)
        {
            return period_idx;
        }
    } while (period_idx > 0);

    return bill_conf_p->model->model_cnt - 1;
}

/***************************************************************************************
 * @Function    : cmp_bill_chrg_data_basic_cyc0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/2
 ****************************************************************************************/
static void cmp_bill_chrg_data_basic_cyc0(cmp_bill_stu *bill_conf_p)
{
    bill_pub.clock->drv_get_bcd_datetime((drv_bcd_datetime_stu *)bill_conf_p->procc.order.stop_time);
    bill_conf_p->procc.order.chrg_time = (bill_pub.clock->drv_clock_get_bcd_datetime_diff((drv_bcd_datetime_stu *)bill_conf_p->procc.order.start_time, (drv_bcd_datetime_stu *)bill_conf_p->procc.order.stop_time) / 60);

    bill_conf_p->procc.order.stop_energy = bill_conf_p->measure->cmp_get_chrg_energy();
    bill_conf_p->procc.order.chrg_energy = bill_conf_p->procc.order.stop_energy - bill_conf_p->procc.order.start_energy;
}

/***************************************************************************************
 * @Function    : cmp_bill_chrg_data_fee_cyc0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/8/2
 ****************************************************************************************/
static void cmp_bill_chrg_data_fee_cyc0(cmp_bill_stu *bill_conf_p)
{
    uint8_t period_idx, cur_idx;
    uint32_t serv_rate, elec_rate;
    uint32_t serv_money = 0, elec_money = 0;
    /* 计费模型个数 */
    if (bill_conf_p->model->model_cnt == 0)
        return;

    period_idx = cmp_bill_get_cur_period_idx(bill_conf_p);

    /* 费率段变化 */
    if (period_idx != bill_conf_p->procc.curr_period_idx)
    {
        bill_conf_p->procc.curr_energy = bill_conf_p->procc.order.stop_energy;
        bill_conf_p->procc.order.chrg_period_cnt++;
        bill_conf_p->procc.curr_period_idx = period_idx;
    }

    /* 电度计算 */
    if (bill_conf_p->procc.order.chrg_period_cnt < 1)
        return;
    cur_idx = bill_conf_p->procc.order.chrg_period_cnt - 1;
    bill_conf_p->procc.order.period[cur_idx].model_period_idx = period_idx;
    *((uint16_t *)bill_conf_p->procc.order.period[cur_idx].model_period_energy) = bill_conf_p->procc.order.stop_energy - bill_conf_p->procc.curr_energy;

    /* 费用计算 */
    for (uint8_t i = 0; i < bill_conf_p->procc.order.chrg_period_cnt; i++)
    {
        serv_rate = *((uint32_t *)bill_conf_p->model->content[bill_conf_p->procc.order.period[i].model_period_idx].serv_fee);
        elec_rate = *((uint32_t *)bill_conf_p->model->content[bill_conf_p->procc.order.period[i].model_period_idx].elec_fee);
        serv_money += (uint32_t)(serv_rate * (*(uint16_t *)bill_conf_p->procc.order.period[i].model_period_energy) / 1000);
        elec_money += (uint32_t)(elec_rate * (*(uint16_t *)bill_conf_p->procc.order.period[i].model_period_energy) / 1000);
    }
    bill_conf_p->procc.order.total_elec_money = elec_money;
    bill_conf_p->procc.order.total_serv_money = serv_money;
}

/***************************************************************************************
 * @Function    : cmp_bill_chrg_data_cyc0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/7
 ****************************************************************************************/
static void cmp_bill_chrg_data_cyc0(uint8_t period)
{
    /* 计算充电时长和充电电量 */
    cmp_bill_chrg_data_basic_cyc0(&bill_conf[0]);

    cmp_bill_chrg_data_fee_cyc0(&bill_conf[0]);
}

/***************************************************************************************
 * @Function    : cmp_bill_stop_chrg_procc0()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/7
 ****************************************************************************************/
static void cmp_bill_stop_chrg_procc0(uint16_t stop_reason)
{
    bill_conf[0].procc.order.stop_energy = bill_conf[0].procc.order.start_energy + bill_conf[0].procc.order.chrg_energy;
    bill_conf[0].procc.order.chrg_time = (bill_pub.clock->drv_clock_get_bcd_datetime_diff((drv_bcd_datetime_stu *)&bill_conf[0].procc.order.start_time, (drv_bcd_datetime_stu *)&bill_conf[0].procc.order.stop_time) / 60);
    bill_conf[0].procc.order.stop_reason = stop_reason;

    bill_pub.store->order->cmp_unsend_add(&bill_conf[0].procc.order);

    memset((uint8_t *)&bill_conf->procc.order_bak, 0, sizeof(cmp_store_order_bak_stu));
}

/***************************************************************************************
 * @Function    : cmp_bill_get_chrg_order_info()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/14
 ****************************************************************************************/
static const cmp_store_order_info_stu *cmp_bill_get_chrg_order_info(void)
{
    return &bill_conf[0].procc.order;
}

/***************************************************************************
*@Function    :cmp_bill_save_order_bak_info_func
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-12-21
***************************************************************************/
static void cmp_bill_save_order_bak_info_func()
{
    bill_pub.clock->drv_get_bcd_datetime(&bill_conf->procc.order_bak.bak_procc.bak_clock);
    bill_pub.store->order->cmp_save_order_bak_info(&bill_conf->procc.order_bak, bill_pub.store->cmp_get_flash(INN_FLASH));
}

/***************************************************************************************
 * @Function    : cmp_bill_dev_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/7
 ****************************************************************************************/
static void cmp_bill_dev_init(cmp_bill_dev_stu *bill)
{
    bill->start_chrg_by_card = cmp_bill_chrg_by_card0;
    bill->start_chrg_by_remote = cmp_bill_start_chrg_remote0;
    bill->start_chrg_by_pole = cmp_bill_start_chrg_pole0;
    bill->start_chrg_by_tim = cmp_bill_chrg_by_tim0;
    bill->chrg_data_cyc = cmp_bill_chrg_data_cyc0;
    bill->stop_chrg_procc = cmp_bill_stop_chrg_procc0;
    bill->get_chrg_order_info = cmp_bill_get_chrg_order_info;
    bill->get_chrg_volt = cmp_bill_get_chrg_volt0;
    bill->get_chrg_curr = cmp_bill_get_chrg_curr0;
    bill->get_chrg_time = cmp_bill_get_chrg_time0;
    bill->get_chrg_money = cmp_bill_get_chrg_money0;
    bill->get_chrg_energy = cmp_bill_get_chrg_energy0;
    bill->get_chrg_power = cmp_bill_get_chrg_power0;
    bill->get_start_type = cmp_bill_get_start_type0;
    bill->save_order_bak_info = cmp_bill_save_order_bak_info_func;
}

/***************************************************************************************
 * @Function    : cmp_bill_pub_dev_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/7
 ****************************************************************************************/
static void cmp_bill_pub_dev_init(cmp_bill_pub_dev_stu *pub)
{
    pub->clock = drv_dev_find(DRV_DEV_CLOCK);

    pub->store = cmp_dev_find(CMP_DEV_NAME_STORE);
}

/***************************************************************************************
 * @Function    : cmp_bill_dev_conf_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/26
 ****************************************************************************************/
static void cmp_bill_dev_conf_init(cmp_bill_stu *bill_conf)
{
    bill_conf->measure = cmp_dev_find(CMP_DEV_NAME_MEASURE);

    bill_conf->model = bill_pub.store->model->cmp_store_model_query();
}

/***************************************************************************************
 * @Function    : cmp_bill_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/7
 ****************************************************************************************/
int cmp_bill_dev_create(void)
{
    static cmp_bill_dev_stu bill_dev[BSP_POLE_PLUG_NUM];

    cmp_bill_pub_dev_init(&bill_pub);

    cmp_bill_dev_conf_init(&bill_conf[0]);

    cmp_bill_dev_init(&bill_dev[0]);

    cmp_dev_append(CMP_DEV_NAME_BILL, bill_dev);

    return 0;
}
// INIT_APP_EXPORT(cmp_bill_dev_create);