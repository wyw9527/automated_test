#ifndef __RYJSONAPI_H__
#define __RYJSONAPI_H__

#include "RyanJson.h"
#include "cmp.h"





typedef struct 
{
  /* data */
  uint8_t (*ryjson_api_parse)(char *msg,RyanJson_t *ryjson);
  RyanJson_t (*ryjson_api_creat_ryjson)(void);

  void (*ryjson_api_add_stringItem)(char* key,char * value,RyanJson_t *ryjson);
  void (*ryjson_api_add_intItem)(char* key,int32_t value,RyanJson_t *ryjson);
  void (*ryjson_api_add_doubleItem)(char* key,double value,RyanJson_t *ryjson);
  void (*ryjson_api_add_boolItem)(char* key,RyanJsonBool value,RyanJson_t *ryjson);
  void (*ryjson_api_add_nullItem)(char* key,RyanJson_t *ryjson);
  void (*ryjson_api_add_objectItem)(char* key,RyanJson_t value,RyanJson_t *ryjson);

  uint8_t (*ryjson_api_get_int_byKey)(char* key,int32_t *value,RyanJson_t *ryjson);
  uint8_t (*ryjson_api_get_double_byKey)(char* key,double *value,RyanJson_t *ryjson);
  uint8_t (*ryjson_api_get_string_byKey)(char* key,char** value,RyanJson_t *ryjson);
  uint8_t (*ryjson_api_get_bool_byKey)(char* key,RyanJsonBool* value,RyanJson_t *ryjson);
  uint8_t (*ryjson_api_get_object_byKey)(char* key,RyanJson_t* value,RyanJson_t *ryjson);


  char* (*ryjson_api_ryjson_to_string)(RyanJson_t* ryjson);
  uint8_t (*ryjson_api_ryjson_delet)(RyanJson_t *ryjson);
}ryjson_api_dev_stu;






#endif

