#ifndef _DRV_H_
#define _DRV_H_

#include "board.h"

#define DRV_DEV_SERIAL_LOAD "seri_load"     /* 串口设备 */
#define DRV_DEV_SERIAL_RFID "seri_rfid"   /* 读卡器 */
#define DRV_DEV_SERIAL_SERV "seri_serv"   /* 服务器 */
#define DRV_DEV_SERIAL_POWER "seri_power" /* 内部电表 */

#define DRV_DEV_CLOCK "clock_rtc"
#define DRV_DEV_METER_CHINT "meter_chint"
#define DRV_DEV_M8209 "m8209"

#define DRV_DEV_ANALOG_INTF "ana_intf"
#define DRV_DEV_ANALOG_CP "ana_cp" /* 模拟量 */
#define DRV_DEV_ANALOG_CC "ana_cc"
#define DRV_DEV_ANALOG_TMP_B "ana_tmp_b"
#define DRV_DEV_ANALOG_TMP_P "ana_tmp_p"
#define DRV_DEV_ANALOG_TMP_PWR1 "ana_tmp_p1"
#define DRV_DEV_ANALOG_TMP_PWR2 "ana_tmp_p2"
#define DRV_DEV_ANALOG_TMP_PWR3 "ana_tmp_p3"

#define DRV_DEV_ANALOG_VOLT1 "ana_volt1"
#define DRV_DEV_ANALOG_VOLT2 "ana_volt2"
#define DRV_DEV_ANALOG_VOLT3 "ana_volt3"

#define DRV_DEV_ANALOG_CURR1 "ana_curr1"
#define DRV_DEV_ANALOG_CURR2 "ana_curr2"
#define DRV_DEV_ANALOG_CURR3 "ana_curr3"

#define DRV_DEV_ANALOG_NE "ana_ne"
#define DRV_DEV_ANALOG_PE "ana_pe"
#define DRV_DEV_ANALOG_LEAK "ana_leak"
#define DRV_DEV_PWM_CP "pwm_cp"

#define DRV_DEV_EXT_FLASH "flash_w25"
#define DRV_DEV_INN_FLASH "flash_inn"

#define DRV_DEV_LED "led_diode"

#define DRV_DEV_RELAY_CONTACTOR "conn"
#define DRV_DEV_RELAY_ELOCK "elock"

#define DRV_DEV_DIDO_EME "di_eme"
#define DRV_DEV_DIDO_PNC "di_pnc"
#define DRV_DEV_DIDO_PLUG_SEL "plug_sel"
#define DRV_DEV_DIDO_RUN_LED "run_led"
#define DRV_DEV_DIDO_CPCC_SW "cpcc_sw"
#define DRV_DEV_DIDO_ESP_RESET "esp_reset"
#define DRV_DEV_DIDO_NE "di_ne"
#define DRV_DEV_DIDO_OPENLID "di_openlid"

#define DRV_DEV_LED_R "led_r"
#define DRV_DEV_LED_G "led_g"
#define DRV_DEV_LED_B "led_b"
#define DRV_DEV_DIDO_LEAK_PROD "leak_prod"
#define DRV_DEV_DIDO_LEAK_SELFTEST "leak_sefttt"

#define DRV_DEV_LOAD_CAN "drv_load_can"
#define DRV_DEV_LOAD_485 "drv_load_485"
#define DRV_DEV_POWER_BOX "drv_power_box"

typedef void (*serial_tx_func_type)(uint8_t *data, uint16_t len);

uint8_t drv_dev_append(const char *name, void *dev);

void *drv_dev_find(const char *name);

void drv_sort_s16_data(int16_t *arr, uint16_t len);

int16_t drv_get_s16_avage(int16_t *data, uint16_t len);

uint8_t xor_check(uint8_t *buff, uint16_t len);

uint8_t math_decimal_2_bcd(uint8_t decimal);

uint8_t math_bcd_2_decimal(uint8_t bcd);

uint32_t crc32_calc(uint8_t *buf, uint32_t len);

uint16_t crc16_calc(uint8_t *ptr, uint32_t len);

float hex_to_float(uint8_t *data);

uint16_t get_rond_num(uint16_t max, uint16_t min, uint16_t seed);

void sort_high_byte_first(uint8_t *msg, uint16_t val);

void sort_low_byte_first(uint8_t *msg, uint32_t value);

#endif