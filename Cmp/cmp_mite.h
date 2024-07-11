#ifndef _CMP_MITE_H_
#define _CMP_MITE_H_

#include "cmp.h"

typedef struct
{
    uint8_t cmd;
    uint8_t cmd_param;
    uint8_t status;
    uint8_t data[16];
    uint8_t data_len;    
}cmp_mite_rx_data_stu;
    
typedef enum
{
    MITE_RX_MSG_VERSION_QUERY,
    MITE_RX_MSG_CARD_ATCTIVE,
    MITE_RX_MSG_CARD_UID,
    MITE_RX_MSG_BUZZ_CTRL,
}CMP_MITE_RX_MSG_TYPE;

typedef enum
{
    MITE_BUZZ_TIP,
    MITE_BUZZ_FAIL,
    MITE_BUZZ_SUCC,
}CMP_MITE_BUZZ_TYPE;

typedef struct
{
    drv_serial_dev_stu* serial;
    uint8_t comm_fail;
    uint8_t comm_count;
}cmp_mite_stu;


typedef struct
{
    void (*mite_set_serial)(drv_serial_dev_stu* serial);
    
    uint8_t (*mite_buzz_action)(CMP_MITE_BUZZ_TYPE buzz_t);
    
    uint8_t (*mite_uid_action)(uint32_t* uid);
    
    uint8_t (*mite_version_action)(void);
    
    uint8_t (*mite_get_comm_fail_sta)(void);
    
}cmp_mite_dev_stu;
























#endif