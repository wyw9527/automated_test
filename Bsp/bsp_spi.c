#include "bsp_spi.h"


static bsp_spi_stu spi_conf[BSP_SPI_NUM];

/***************************************************************************************
 * @Function    : bsp_spi_get_pin_src()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/23
 ****************************************************************************************/
static uint8_t bsp_spi_get_pin_src(BSP_SPI_TYPE SPI_x, uint8_t remap, bsp_spi_pin_stu *src)
{
    switch (SPI_x)
    {
    case BSP_SPI0:
        if (remap == 0)
        {
            src->spi_nss_port = GPIOA;
            src->spi_nss_pin = GPIO_PIN_4;
            src->spi_clk_port = GPIOA;
            src->spi_clk_pin = GPIO_PIN_5;
            src->spi_miso_port = GPIOA;
            src->spi_miso_pin = GPIO_PIN_6;
            src->spi_mosi_port = GPIOA;
            src->spi_mosi_pin = GPIO_PIN_7;
        }
        else
        {
            src->spi_nss_port = GPIOA;
            src->spi_nss_pin = GPIO_PIN_15;
            src->spi_clk_port = GPIOB;
            src->spi_clk_pin = GPIO_PIN_3;
            src->spi_miso_port = GPIOB;
            src->spi_miso_pin = GPIO_PIN_4;
            src->spi_mosi_port = GPIOB;
            src->spi_mosi_pin = GPIO_PIN_5;
        }
        break;

    case BSP_SPI1:
        if (remap == 0)
        {
            src->spi_nss_port = GPIOB;
            src->spi_nss_pin = GPIO_PIN_12;
            src->spi_clk_port = GPIOB;
            src->spi_clk_pin = GPIO_PIN_13;
            src->spi_miso_port = GPIOB;
            src->spi_miso_pin = GPIO_PIN_14;
            src->spi_mosi_port = GPIOB;
            src->spi_mosi_pin = GPIO_PIN_15;
        }
        else
        {
            return 1;
        }
        break;

    case BSP_SPI2:
        if (remap == 0)
        {
            src->spi_nss_port = GPIOA;
            src->spi_nss_pin = GPIO_PIN_15;
            src->spi_clk_port = GPIOB;
            src->spi_clk_pin = GPIO_PIN_3;
            src->spi_miso_port = GPIOB;
            src->spi_miso_pin = GPIO_PIN_4;
            src->spi_mosi_port = GPIOB;
            src->spi_mosi_pin = GPIO_PIN_5;
        }
        else
        {
            src->spi_nss_port = GPIOA;
            src->spi_nss_pin = GPIO_PIN_4;
            src->spi_clk_port = GPIOC;
            src->spi_clk_pin = GPIO_PIN_10;
            src->spi_miso_port = GPIOC;
            src->spi_miso_pin = GPIO_PIN_11;
            src->spi_mosi_port = GPIOC;
            src->spi_mosi_pin = GPIO_PIN_12;
        }
        break;
    default:
        return 1;
    }
    return 0;
}

/***************************************************************************************
 * @Function    : bsp_spi_conf_data_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/23
 ****************************************************************************************/
static uint8_t bsp_spi_conf_data_init(BSP_SPI_TYPE SPI_x, bsp_spi_pin_stu *src)
{
    if (src == RT_NULL)
        return 1;

    spi_conf[SPI_x].spi_nss_port = src->spi_nss_port;
    spi_conf[SPI_x].spi_nss_pin = src->spi_nss_pin;

    switch (SPI_x)
    {
    case BSP_SPI0:
        spi_conf[SPI_x].spi_periph = SPI0;
        break;
    case BSP_SPI1:
        spi_conf[SPI_x].spi_periph = SPI1;
        break;
    case BSP_SPI2:
        spi_conf[SPI_x].spi_periph = SPI2;
        break;
    default:
        return 1;
    }

    return 0;
}

/***************************************************************************************
 * @Function    : bsp_spi_get_pin_src()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/23
 ****************************************************************************************/
static uint8_t bsp_spi_pin_init(BSP_SPI_TYPE SPI_x, uint8_t remap)
{
    bsp_spi_pin_stu pin_src;

    if (bsp_spi_get_pin_src(SPI_x, remap, &pin_src) > 0)
        return 1;

    if (remap > 0)
    {
        rcu_periph_clock_enable(RCU_AF);
    }

    rcu_periph_clock_enable(bsp_get_pin_rcu(pin_src.spi_nss_port));
    rcu_periph_clock_enable(bsp_get_pin_rcu(pin_src.spi_clk_port));
    rcu_periph_clock_enable(bsp_get_pin_rcu(pin_src.spi_miso_port));
    rcu_periph_clock_enable(bsp_get_pin_rcu(pin_src.spi_mosi_port));

    gpio_init(pin_src.spi_nss_port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, pin_src.spi_nss_pin);
    gpio_init(pin_src.spi_clk_port, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, pin_src.spi_clk_pin);
    gpio_init(pin_src.spi_miso_port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, pin_src.spi_miso_pin);
    gpio_init(pin_src.spi_mosi_port, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, pin_src.spi_mosi_pin);

    bsp_spi_conf_data_init(SPI_x, &pin_src);

    return 0;
}

/***************************************************************************************
 * @Function    : bsp_spi_clock_enable()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/23
 ****************************************************************************************/
static void bsp_spi_clock_enable(BSP_SPI_TYPE SPI_x)
{
    switch (SPI_x)
    {
    case BSP_SPI0:
        rcu_periph_clock_enable(RCU_SPI0);
        break;
    case BSP_SPI1:
        rcu_periph_clock_enable(RCU_SPI1);
        break;
    case BSP_SPI2:
        rcu_periph_clock_enable(RCU_SPI2);
        break;
    }
}

/***************************************************************************************
 * @Function    : bsp_spi_cs_high()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/23
 ****************************************************************************************/
static void bsp_spi_cs_high(BSP_SPI_TYPE SPI_x)
{
    gpio_bit_set(spi_conf[SPI_x].spi_nss_port, spi_conf[SPI_x].spi_nss_pin);
}

/***************************************************************************************
 * @Function    : bsp_spi_cs_low()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/23
 ****************************************************************************************/
static void bsp_spi_cs_low(BSP_SPI_TYPE SPI_x)
{
    gpio_bit_reset(spi_conf[SPI_x].spi_nss_port, spi_conf[SPI_x].spi_nss_pin);
}

/***************************************************************************************
 * @Function    : bsp_spi_param_config()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/23
 ****************************************************************************************/
static void bsp_spi_param_config(BSP_SPI_TYPE SPI_x)
{
    spi_parameter_struct spi_init_struct;

    /* chip select invalid*/
    bsp_spi_cs_high(SPI_x);

    bsp_spi_clock_enable(SPI_x);

    /* SPI1 parameter config */
    spi_init_struct.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode = SPI_MASTER;

    spi_init_struct.frame_size = SPI_FRAMESIZE_8BIT;

    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss = SPI_NSS_SOFT;
    spi_init_struct.prescale = SPI_PSC_256;
    spi_init_struct.endian = SPI_ENDIAN_MSB;

    spi_init(spi_conf[SPI_x].spi_periph, &spi_init_struct);

    /* set crc polynomial */
    spi_crc_polynomial_set(spi_conf[SPI_x].spi_periph, 7);
    /* enable SPI1 */
    spi_enable(spi_conf[SPI_x].spi_periph);
}

/***************************************************************************************
 * @Function    : bsp_spi_init()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/23
 ****************************************************************************************/
static uint8_t bsp_spi_init(BSP_SPI_TYPE SPI_x)
{
    if (bsp_spi_pin_init(SPI_x, 0) > 0)
    {
        return 1;
    }

    bsp_spi_param_config(SPI_x);
    return 0;
}

/***************************************************************************************
 * @Function    : bsp_spi_send_byte()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/23
 ****************************************************************************************/
static uint8_t bsp_spi_send_byte(BSP_SPI_TYPE SPI_x, uint8_t byte)
{
    /* loop while data register in not emplty */
    while (RESET == spi_i2s_flag_get(spi_conf[SPI_x].spi_periph, SPI_FLAG_TBE))
        ;

    /* send byte through the SPI1 peripheral */
    spi_i2s_data_transmit(spi_conf[SPI_x].spi_periph, byte);

    /* wait to receive a byte */
    while (RESET == spi_i2s_flag_get(spi_conf[SPI_x].spi_periph, SPI_FLAG_RBNE))
        ;

    /* return the byte read from the SPI bus */
    return (spi_i2s_data_receive(spi_conf[SPI_x].spi_periph));
}

/***************************************************************************************
 * @Function    : bsp_spi_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/23
 ****************************************************************************************/
int bsp_spi_dev_create(void)
{
    static bsp_spi_dev_stu spi_dev;
    if (bsp_spi_init(BSP_SPI1) > 0)
    {
        bsp_alarm_mb_send(0xff, 202, 0, 2);
    }

    spi_dev.bsp_spi_cs_high = bsp_spi_cs_high;
    spi_dev.bsp_spi_cs_low = bsp_spi_cs_low;
    spi_dev.bsp_spi_transmit_byte = bsp_spi_send_byte;

    bsp_dev_append(BSP_DEV_NAME_SPI, &spi_dev);
    return 0;
}


