#ifndef _CMP_EC600_H_
#define _CMP_EC600_H_


#include "stdio.h"
#include "cmp.h"

#define CMP_EC600_RX_BUFF_SIZE 1152

typedef struct
{
    const char* serv_addr;
    uint16_t port;
}cmp_ec600_param_stu;

typedef enum
{
    EC600_STA_RESET,
    EC600_STA_PARAM_QUERY,
    EC600_STA_SERV_CONN,
    EC600_STA_SPP,
}CMP_EC600_STA_TYPE;

typedef enum
{
    EC600_REASON_INIT=0,
    EC600_REASON_RESET_TMOUT,
    EC600_REASON_ATE0_TMOUT,
    EC600_REASON_CPIN_TMOUT,
    EC600_REASON_CREG_ERROR,
    EC600_REASON_CREG_TMOUT,
    EC600_REASON_CGREG_ERROR,
    EC600_REASON_CGREG_TMOUT,
    EC600_REASON_QCCID_TMOUT,
    EC600_REASON_QIACT_TMOUT,
    EC600_REASON_QIOPEN_TMOUT,
    EC600_REASON_OK
}CMP_EC600_REASON_TYPE;
    
typedef struct
{
    CMP_EC600_STA_TYPE state;
    CMP_EC600_REASON_TYPE reason;
    uint8_t status;
    uint8_t useable;
}cmp_ec600_procc_stu;

typedef struct
{
    drv_serial_dev_stu* serial;
    
    drv_dido_do_dev_stu* reset;
    
    cmp_ec600_param_stu param;
    
    cmp_ec600_procc_stu procc;
    
}cmp_ec600_stu;

typedef struct
{
    uint8_t (*ec600_get_status)(void);
    
    void (*ec600_work)(uint8_t period);
    
    void (*ec600_set_serial)(drv_serial_dev_stu* serial);
    
    void (*ec600_set_param)(cmp_ec600_param_stu* param);
    
    uint8_t (*ec600_get_useable_state)(void);
}cmp_ec600_dev_stu;

























#endif