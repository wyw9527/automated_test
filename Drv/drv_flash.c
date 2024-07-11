#include "drv_flash.h"


#define DRV_FLASH_SPI BSP_SPI1

static drv_flash_stu flash_conf;

/***************************************************************************************
 * @Function    : drv_flash_write_enable()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/23
 ****************************************************************************************/
static void drv_flash_write_enable(void)
{
    flash_conf.spi->bsp_spi_cs_low(DRV_FLASH_SPI);

    flash_conf.spi->bsp_spi_transmit_byte(DRV_FLASH_SPI, DRV_FLASH_INS_WR_EN);

    flash_conf.spi->bsp_spi_cs_high(DRV_FLASH_SPI);
}

/***************************************************************************************
 * @Function    : spi_flash_wait_wr_end()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/23
 ****************************************************************************************/
static void spi_flash_wait_wr_end(void)
{
    uint8_t status = 0;

    /* select the flash: chip select low */
    flash_conf.spi->bsp_spi_cs_low(DRV_FLASH_SPI);

    /* send "read status register" instruction */
    flash_conf.spi->bsp_spi_transmit_byte(DRV_FLASH_SPI, DRV_FLASH_INS_RD_SR);

    /* loop as long as the memory is busy with a write cycle */
    do
    {
        /* send a dummy byte to generate the clock needed by the flash
        and put the value of the status register in flash_status variable */
        status = flash_conf.spi->bsp_spi_transmit_byte(DRV_FLASH_SPI, DRV_FLASH_INS_DUMMY_BYTE);
    } while ((status & DRV_FLASH_FLAG_WIP) == SET);

    /* deselect the flash: chip select high */
    flash_conf.spi->bsp_spi_cs_high(DRV_FLASH_SPI);
}

/***************************************************************************************
 * @Function    : drv_flash_read_status_register()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/23
 ****************************************************************************************/
static uint8_t drv_flash_read_status_register(void)
{
    uint8_t byte;

    /* select the flash: chip select low */
    flash_conf.spi->bsp_spi_cs_low(DRV_FLASH_SPI);

    flash_conf.spi->bsp_spi_transmit_byte(DRV_FLASH_SPI, DRV_FLASH_INS_RD_SR);

    byte = flash_conf.spi->bsp_spi_transmit_byte(DRV_FLASH_SPI, DRV_FLASH_W25_DUMMY);

    flash_conf.spi->bsp_spi_cs_high(DRV_FLASH_SPI);

    return byte;
}

/***************************************************************************************
 * @Function    : drv_flash_wait_busy()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/23
 ****************************************************************************************/
static uint8_t drv_flash_wait_busy(void)
{
    uint32_t src_tick, tick_count = 0;

    src_tick = rt_tick_get();

    while ((drv_flash_read_status_register() & 0x01) == 0x01)
    {
        if (src_tick != rt_tick_get())
        {
            src_tick = rt_tick_get();
            if (++tick_count > 500)
            {
                return 0;
            }
        }
    }
    return 1;
}

/***************************************************************************************
 * @Function    : drv_flash_erase_sector()
 *
 * @Param       :
 *
 * @Return      : 0 SUCC   1  busy
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/23
 ****************************************************************************************/
static uint8_t drv_flash_erase_sector(uint32_t sector_addr)
{
    rt_enter_critical();
    drv_flash_write_enable();

    if (drv_flash_wait_busy() == 0)
    {
        rt_exit_critical();
        return 1;
    }

    /* select the flash: chip select low */
    flash_conf.spi->bsp_spi_cs_low(DRV_FLASH_SPI);
    /* send sector erase instruction */
    flash_conf.spi->bsp_spi_transmit_byte(DRV_FLASH_SPI, DRV_FLASH_INS_SE);
    /* send sector_addr high nibble address byte */
    flash_conf.spi->bsp_spi_transmit_byte(DRV_FLASH_SPI, (sector_addr & 0xFF0000) >> 16);
    /* send sector_addr medium nibble address byte */
    flash_conf.spi->bsp_spi_transmit_byte(DRV_FLASH_SPI, (sector_addr & 0xFF00) >> 8);
    /* send sector_addr low nibble address byte */
    flash_conf.spi->bsp_spi_transmit_byte(DRV_FLASH_SPI, sector_addr & 0xFF);

    flash_conf.spi->bsp_spi_cs_high(DRV_FLASH_SPI);

    if (drv_flash_wait_busy() == 0)
    {
        rt_exit_critical();
        return 1;
    }

    spi_flash_wait_wr_end();
    rt_exit_critical();
    return 0;
}

/***************************************************************************
*@Function    :drv_flash_erase_area
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-10-19
***************************************************************************/
static uint8_t drv_flash_erase_area(uint32_t addr, uint8_t page_cnt)
{
    for (uint8_t i = 0; i < page_cnt; i++)
    {
        if (drv_flash_erase_sector(addr + i * 0x1000) > 0)
        {
            return 1;
        }
    }

    return 0;
}

/***************************************************************************************
 * @Function    : drv_flash_erase_chip()
 *
 * @Param       :
 *
 * @Return      : 0 SUCC   1  busy
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/23
 ****************************************************************************************/
static uint8_t drv_flash_erase_chip(void)
{
    drv_flash_write_enable();

    rt_enter_critical();
    if (drv_flash_wait_busy() == 0)
    {
        rt_exit_critical();
        return 1;
    }

    /* select the flash: chip select low */
    flash_conf.spi->bsp_spi_cs_low(DRV_FLASH_SPI);
    /* send sector erase instruction */
    flash_conf.spi->bsp_spi_transmit_byte(DRV_FLASH_SPI, DRV_FLASH_INS_BE);

    flash_conf.spi->bsp_spi_cs_high(DRV_FLASH_SPI);

    if (drv_flash_wait_busy() == 0)
    {
        rt_exit_critical();
        return 1;
    }

    spi_flash_wait_wr_end();
    rt_exit_critical();
    return 0;
}

/***************************************************************************************
 * @Function    : drv_flash_page_write()
 *
 * @Param       :
 *
 * @Return      : 0 SUCC   1  busy
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/23
 ****************************************************************************************/
static uint8_t drv_flash_page_write(uint8_t *wr_data, uint32_t wr_addr, uint16_t wr_num)
{
    drv_flash_write_enable();

    if (drv_flash_wait_busy() == 0)
    {
        return 1;
    }

    flash_conf.spi->bsp_spi_cs_low(DRV_FLASH_SPI);
    /* send "write to memory" instruction */
    flash_conf.spi->bsp_spi_transmit_byte(DRV_FLASH_SPI, DRV_FLASH_INS_WR);
    /* send sector_addr high nibble address byte */
    flash_conf.spi->bsp_spi_transmit_byte(DRV_FLASH_SPI, (wr_addr & 0xFF0000) >> 16);
    /* send sector_addr medium nibble address byte */
    flash_conf.spi->bsp_spi_transmit_byte(DRV_FLASH_SPI, (wr_addr & 0xFF00) >> 8);
    /* send sector_addr low nibble address byte */
    flash_conf.spi->bsp_spi_transmit_byte(DRV_FLASH_SPI, wr_addr & 0xFF);

    /* while there is data to be written on the flash */
    while (wr_num--)
    {
        /* send the current byte */
        flash_conf.spi->bsp_spi_transmit_byte(DRV_FLASH_SPI, *wr_data);
        /* point on the next byte to be written */
        wr_data++;
    }

    flash_conf.spi->bsp_spi_cs_high(DRV_FLASH_SPI);

    if (drv_flash_wait_busy() == 0)
    {
        return 1;
    }

    spi_flash_wait_wr_end();
    return 0;
}

/***************************************************************************************
 * @Function    : drv_flash_write_data()
 *
 * @Param       :
 *
 * @Return      : 0 SUCC   1  busy
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/23
 ****************************************************************************************/
static uint8_t drv_flash_write_data(uint32_t write_addr, uint8_t *pbuffer, uint16_t num_byte_to_write)
{
    uint8_t num_of_page = 0, num_of_single = 0, addr = 0, count = 0, temp = 0;

    addr = write_addr % SPI_FLASH_PAGE_SIZE;
    count = SPI_FLASH_PAGE_SIZE - addr;
    num_of_page = num_byte_to_write / SPI_FLASH_PAGE_SIZE;
    num_of_single = num_byte_to_write % SPI_FLASH_PAGE_SIZE;

    if (flash_conf.status > 0)
    {
        return 1;
    }

    rt_enter_critical();
    /* write_addr is SPI_FLASH_PAGE_SIZE aligned  */
    if (0 == addr)
    {
        /* num_byte_to_write < SPI_FLASH_PAGE_SIZE */
        if (0 == num_of_page)
        {
            if (drv_flash_page_write(pbuffer, write_addr, num_byte_to_write) > 0)
            {
                rt_exit_critical();
                return 1;
            }
        }
        /* num_byte_to_write > SPI_FLASH_PAGE_SIZE */
        else
        {
            while (num_of_page--)
            {
                if (drv_flash_page_write(pbuffer, write_addr, SPI_FLASH_PAGE_SIZE) > 0)
                {
                    rt_exit_critical();
                    return 1;
                }
                write_addr += SPI_FLASH_PAGE_SIZE;
                pbuffer += SPI_FLASH_PAGE_SIZE;
            }
            if (drv_flash_page_write(pbuffer, write_addr, num_of_single) > 0)
            {
                rt_exit_critical();
                return 1;
            }
        }
    }
    else
    {
        /* write_addr is not SPI_FLASH_PAGE_SIZE aligned  */
        if (0 == num_of_page)
        {
            /* (num_byte_to_write + write_addr) > SPI_FLASH_PAGE_SIZE */
            if (num_of_single > count)
            {
                temp = num_of_single - count;
                if (drv_flash_page_write(pbuffer, write_addr, count) > 0)
                {
                    rt_exit_critical();
                    return 1;
                }
                write_addr += count;
                pbuffer += count;
                if (drv_flash_page_write(pbuffer, write_addr, temp) > 0)
                {
                    rt_exit_critical();
                    return 1;
                }
            }
            else if (drv_flash_page_write(pbuffer, write_addr, num_byte_to_write) > 0)
            {
                rt_exit_critical();
                return 1;
            }
        }
        else
        {
            /* num_byte_to_write > SPI_FLASH_PAGE_SIZE */
            num_byte_to_write -= count;
            num_of_page = num_byte_to_write / SPI_FLASH_PAGE_SIZE;
            num_of_single = num_byte_to_write % SPI_FLASH_PAGE_SIZE;

            if (drv_flash_page_write(pbuffer, write_addr, count) > 0)
            {
                rt_exit_critical();
                return 1;
            }
            write_addr += count;
            pbuffer += count;

            while (num_of_page--)
            {
                if (drv_flash_page_write(pbuffer, write_addr, SPI_FLASH_PAGE_SIZE) > 0)
                {
                    rt_exit_critical();
                    return 1;
                }
                write_addr += SPI_FLASH_PAGE_SIZE;
                pbuffer += SPI_FLASH_PAGE_SIZE;
            }

            if (0 != num_of_single)
            {
                if (drv_flash_page_write(pbuffer, write_addr, num_of_single) > 0)
                {
                    rt_exit_critical();
                    return 1;
                }
            }
        }
    }
    rt_exit_critical();
    return 0;
}

/***************************************************************************************
 * @Function    : drv_flash_read_data()
 *
 * @Param       :
 *
 * @Return      : 0 SUCC   1  busy
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/23
 ****************************************************************************************/
static uint8_t drv_flash_read_data(uint32_t read_addr, uint8_t *pbuffer, uint16_t num_byte_to_read)
{
    if (flash_conf.status > 0)
    {
        return 1;
    }

    rt_enter_critical();
    if (drv_flash_wait_busy() == 0)
    {
        rt_exit_critical();
        return 1;
    }

    flash_conf.spi->bsp_spi_cs_low(DRV_FLASH_SPI);
    /* send "read from memory " instruction */
    flash_conf.spi->bsp_spi_transmit_byte(DRV_FLASH_SPI, DRV_FLASH_INS_RD);
    /* send sector_addr high nibble address byte */
    flash_conf.spi->bsp_spi_transmit_byte(DRV_FLASH_SPI, (read_addr & 0xFF0000) >> 16);
    /* send sector_addr medium nibble address byte */
    flash_conf.spi->bsp_spi_transmit_byte(DRV_FLASH_SPI, (read_addr & 0xFF00) >> 8);
    /* send sector_addr low nibble address byte */
    flash_conf.spi->bsp_spi_transmit_byte(DRV_FLASH_SPI, read_addr & 0xFF);

    /* while there is data to be read */
    while (num_byte_to_read--)
    {
        /* read a byte from the flash */
        *pbuffer = flash_conf.spi->bsp_spi_transmit_byte(DRV_FLASH_SPI, DRV_FLASH_INS_DUMMY_BYTE);
        /* point to the next location where the byte read will be saved */
        pbuffer++;
    }

    flash_conf.spi->bsp_spi_cs_high(DRV_FLASH_SPI);
    rt_exit_critical();
    return 0;
}

/***************************************************************************************
 * @Function    : drv_ext_flash_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/15
 ****************************************************************************************/
static void drv_ext_flash_dev_create(void)
{
    static drv_flash_dev_stu flash_dev;

    /* drv flash function init */
    flash_dev.drv_flash_erase_sector = drv_flash_erase_sector;
    flash_dev.drv_flash_erase_chip = drv_flash_erase_chip;
    flash_dev.drv_flash_write_data = drv_flash_write_data;
    flash_dev.drv_flash_read_data = drv_flash_read_data;
    flash_dev.drv_flash_erase_area = drv_flash_erase_area;

    /* find spi dev */
    flash_conf.spi = bsp_dev_find(BSP_DEV_NAME_SPI);
    if (flash_conf.spi == RT_NULL)
    {
        flash_conf.status = 1;
    }

    drv_dev_append(DRV_DEV_EXT_FLASH, &flash_dev);
}

/***************************************************************************************
 * @Function    : drv_inn_flash_erase_sector()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/15
 ****************************************************************************************/
static uint8_t drv_inn_flash_erase_sector(uint32_t addr)
{
    fmc_unlock();

    fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);

    fmc_page_erase(addr);

    fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);

    fmc_lock();

    return 0;
}

/***************************************************************************
*@Function    :drv_inn_flash_erase_area
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2023-10-19
***************************************************************************/
static uint8_t drv_inn_flash_erase_area(uint32_t addr, uint8_t page_cnt)
{
    for (uint8_t i = 0; i < page_cnt * 2; i++)
    {
        if (drv_inn_flash_erase_sector(addr + i * 0x0800) > 0)
        {
            return 1;
        }
    }
    return 0;
}

/***************************************************************************************
 * @Function    : drv_inn_flash_erase_chip()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/15
 ****************************************************************************************/
static uint8_t drv_inn_flash_erase_chip(void)
{
    fmc_unlock();

    fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);

    fmc_mass_erase();

    fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);

    fmc_lock();

    return 0;
}

/***************************************************************************************
 * @Function    : drv_inn_flash_write_data()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/15
 ****************************************************************************************/
static uint8_t drv_inn_flash_write_data(uint32_t addr, uint8_t *data, uint16_t len)
{
    uint32_t wr_addr;
    uint16_t i;

    /* Unlock the Flash Bank1 Program Erase controller */
    fmc_unlock();

    /* Clear All pending flags */
    fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);

    /* Program Flash Bank0 */
    for (i = 0; i < len / 4; i++)
    {
        wr_addr = i * 4 + addr;
        fmc_word_program(wr_addr, *((uint32_t *)&data[i * 4]));
        fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);
    }

    /* Handle remaining bytes if length is not a multiple of 4 */
    if (len % 4 != 0)
    {
        wr_addr = len - (len % 4) + addr;
        uint32_t remaining_data = 0;
        for (uint8_t j = 0; j < len % 4; j++)
        {
            remaining_data |= data[i * 4 + j] << (j * 8);
        }
        fmc_word_program(wr_addr, remaining_data);
        fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);
    }

    fmc_lock();

    return 0;
}

/***************************************************************************************
 * @Function    : drv_inn_flash_read_data()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/15
 ****************************************************************************************/
static uint8_t drv_inn_flash_read_data(uint32_t addr, uint8_t *data, uint16_t len)
{
    memcpy(data, (uint8_t *)addr, len);
    return 0;
}

/***************************************************************************************
 * @Function    : drv_inn_flash_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/7/15
 ****************************************************************************************/
static void drv_inn_flash_dev_create(void)
{
    static drv_flash_dev_stu inn_flash_dev;

    /* drv flash function init */
    inn_flash_dev.drv_flash_erase_sector = drv_inn_flash_erase_sector;
    inn_flash_dev.drv_flash_erase_chip = drv_inn_flash_erase_chip;
    inn_flash_dev.drv_flash_write_data = drv_inn_flash_write_data;
    inn_flash_dev.drv_flash_read_data = drv_inn_flash_read_data;
    inn_flash_dev.drv_flash_erase_area = drv_inn_flash_erase_area;

    drv_dev_append(DRV_DEV_INN_FLASH, &inn_flash_dev);
}

/***************************************************************************************
 * @Function    : drv_flash_dev_create()
 *
 * @Param       :
 *
 * @Return      :
 *
 * @Description :
 *
 * @Author      : chenbao            Version   : V3.0.0                Date:2023/5/23
 ****************************************************************************************/
static int drv_flash_dev_create(void)
{
    drv_ext_flash_dev_create();

    drv_inn_flash_dev_create();

    return 0;
}
INIT_COMPONENT_EXPORT(drv_flash_dev_create);

