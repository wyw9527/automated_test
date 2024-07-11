#ifndef _DRV_FLASH_H_
#define _DRV_FLASH_H_

#include "drv.h"
#include "board.h"

#define DRV_FLASH_INS_WR 0x02    /* write to memory instruction */
#define DRV_FLASH_INS_RD 0x03    /* read from memory instruction */
#define DRV_FLASH_INS_WR_EN 0x06 /* write enable instruction */
#define DRV_FLASH_INS_RD_SR 0x05 /* read status register instruction  */
#define DRV_FLASH_INS_SE 0x20    /* sector erase instruction */
#define DRV_FLASH_INS_BE 0xC7    /* bulk erase instruction */
#define DRV_FLASH_INS_DUMMY_BYTE 0xA5

#define DRV_FLASH_W25_DUMMY 0x00
#define DRV_FLASH_FLAG_WIP 0x01 /* write in progress(wip)flag */

#define SPI_FLASH_PAGE_SIZE 0x100

typedef enum
{
    W25_ERASE_4K = 0x20,
    W25_ERASE_32K = 0x52,
    W25_ERASE_64K = 0xD8,
} DRV_FLASH_ERASE_TYPE;

typedef struct
{
    bsp_spi_dev_stu *spi;
    uint8_t status; // 0 normal 1 find dev fail  2 init dev fail
} drv_flash_stu;

typedef struct
{
    uint8_t (*drv_flash_erase_sector)(uint32_t sector_addr);

    uint8_t (*drv_flash_erase_chip)(void);

    uint8_t (*drv_flash_write_data)(uint32_t wr_addr, uint8_t *pbuffer, uint16_t wr_num);

    uint8_t (*drv_flash_read_data)(uint32_t rd_addr, uint8_t *pbuffer, uint16_t rd_num);

    uint8_t (*drv_flash_erase_area)(uint32_t area_addr, uint8_t page_cnt);

} drv_flash_dev_stu;

#endif