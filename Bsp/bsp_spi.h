#ifndef _BSP_SPI_H_
#define _BSP_SPI_H_

#include "bsp.h"



typedef enum
{
    BSP_SPI0=0,
    BSP_SPI1,
    BSP_SPI2,
    BSP_SPI_NUM
}BSP_SPI_TYPE;


typedef struct
{
    uint32_t spi_nss_port;
    uint32_t spi_nss_pin;
    uint32_t spi_clk_port;
    uint32_t spi_clk_pin;
    uint32_t spi_miso_port;
    uint32_t spi_miso_pin;
    uint32_t spi_mosi_port;
    uint32_t spi_mosi_pin;
}bsp_spi_pin_stu;

typedef struct
{
    uint32_t spi_nss_port;
    uint32_t spi_nss_pin;
    uint32_t spi_periph;
}bsp_spi_stu;

typedef struct
{
    void (*bsp_spi_cs_high)(BSP_SPI_TYPE SPI_x);
    void (*bsp_spi_cs_low)(BSP_SPI_TYPE SPI_x);
    uint8_t (*bsp_spi_transmit_byte)(BSP_SPI_TYPE SPI_x,uint8_t byte);
}bsp_spi_dev_stu;



















#endif