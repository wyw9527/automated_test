#ifndef _BOARD_H_
#define _BOARD_H_

/***************************************** TDG项目 **********************************************/
#include "bsp.h"
#include "bsp_adc.h"
#include "bsp_io.h"
#include "bsp_pmu.h"
#include "bsp_pwm.h"
#include "bsp_rtc.h"
#include "bsp_uart.h"
#include "bsp_spi.h"
#include "bsp_can.h"

/* 板载资源 */
#define BSP_POLE_PLUG_NUM 1         /* 充电桩枪数量 */
#define CHRG_TYPE CHRG_PLUG         // 充电枪类型 枪座/枪线
#define LEAK_TYPE LEAK_DC           // 漏电流类型 交流/直流
#define CONN_TYPE CONN_TPHASE       // 接触器类型 三相 CONN_TPHASE/单相 CONN_SINGLE
#define CONN_FB_TYPE CONN_FB_L      // 接触器闭合反馈 高/低
#define CONN_ACT_TYPE CONN_ACT_NORM // 接触器闭合方式 一般/零点		CONN_ACT_NORM/CONN_ACT_ZERO
#define ELOCK_FB_TYPE ELOCK_FB_H    // 电子锁闭合反馈类型 高/低

/* 采样相关 */
#define ANALOG_CURR_TYPE ANALOG_TYPE_1 // 电流计算方式1 ANALOG_TYPE_0/ANALOG_TYPE_1
#define ANALOG_CURR_FACTOR_K 1         // 电流换算系数K
#define ANALOG_CURR_FACTOR_B 0         // 电流换算系数B

#define ANALOG_CURR_FACTOR_L1_K 0.2004  // 电流换算系数K
#define ANALOG_CURR_FACTOR_L1_B -4.5692 // 电流换算系数B

#define ANALOG_CURR_FACTOR_L2_K 0.2 // 电流换算系数K
#define ANALOG_CURR_FACTOR_L2_B -2  // 电流换算系数B

#define ANALOG_CURR_FACTOR_L3_K 0.1919 // 电流换算系数K
#define ANALOG_CURR_FACTOR_L3_B -2.32  // 电流换算系数B

#define ANALOG_VOLT_TYPE ANALOG_TYPE_0 // 电压计算方式1 ANALOG_TYPE_0/ANALOG_TYPE_1
#define ANALOG_VOLT_FACTOR_K 1.5       //
#define ANALOG_VOLT_FACTOR_B 0         //

#define ANALOG_VOLT_FACTOR_L1_K 0.3625 //   l1电压系数  0.3545
#define ANALOG_VOLT_FACTOR_L1_B 0.6253 //   1.1772

#define ANALOG_VOLT_FACTOR_L2_K 0.3632  //  l2电压系数
#define ANALOG_VOLT_FACTOR_L2_B -0.6555 //

#define ANALOG_VOLT_FACTOR_L3_K 0.3615 //     l3电压系数
#define ANALOG_VOLT_FACTOR_L3_B 0.0918 //

/* 相关门限 */
#define CMP_MEASURE_VOLT_OV_TH 2760
#define CMP_MEASURE_VOLT_UV_TH 1840
#define CMP_MEASURE_VOLT_LOSS_TH 700
#define CMP_SENSOR_LEAK_TH 50   // 480/40mA 72/6mA
#define CMP_SENSOR_LEAK_BL 2000 // 0mA基准值

#define CMP_PE_VALOT_TH 300      // PE接地检测电压
#define CMP_STORE_TYPE FLASH_EXT /* 存储方式 外部/内部 */

#endif