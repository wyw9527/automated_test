#include "RyJsonAPI.h"



/***************************************************************************
*@Function    :
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2024-07-09
***************************************************************************/
static void remove_whitespace_and_newlines(char *str) {
    char *src = str, *dst = str;

    while (*src != '\0') {
        if (*src != '\r' && *src != '\n' && *src != ' ' && *src != '\t') {
            *dst = *src;
            dst++;
        }
        src++;
    }
    *dst = '\0'; // 确保结果字符串以NULL结尾
}
/***************************************************************************
*@Function    :
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2024-07-09
***************************************************************************/
static uint8_t ryjson_api_parse_func(char *msg,RyanJson_t *ryjson)
{
  RyanJson_t jsonRoot;
	char *str = NULL;
	remove_whitespace_and_newlines(msg);

  jsonRoot = RyanJsonParse(msg);
	if (jsonRoot == NULL)
    {
				return 1;//未找到json格式
		}

    // 将序列化的数据以无格式样式打印出来，并和原始数据进行对比
    str = RyanJsonPrint(jsonRoot, 250, RyanJsonFalse, NULL);
    if (strcmp(str, msg) != 0)
    { 
        RyanJsonFree(str);
        RyanJsonDelete(jsonRoot);
				return 2;//该json去除格式后不一致
		}
    RyanJsonFree(str);
    memcpy(ryjson,&jsonRoot,sizeof(RyanJson_t));
    // 删除json对象
    RyanJsonDelete(jsonRoot);
    return 0;
}

/***************************************************************************
*@Function    :
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2024-07-09
***************************************************************************/
static RyanJson_t ryjson_api_creat_ryjson_func()
{  
  RyanJson_t jsonRoot;
  jsonRoot = RyanJsonCreateObject();
  return jsonRoot;
}

/***************************************************************************
*@Function    :
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2024-07-09
***************************************************************************/
static void ryjson_api_add_boolItem_func(char* key,RyanJsonBool value,RyanJson_t *ryjson)
{
  RyanJsonAddBoolToObject(*ryjson,key,value);
}


/***************************************************************************
*@Function    :
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2024-07-09
***************************************************************************/
static void ryjson_api_add_doubleItem_func(char* key,double value,RyanJson_t *ryjson)
{
  RyanJsonAddDoubleToObject(*ryjson, key, value);
}

/***************************************************************************
*@Function    :
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2024-07-09
***************************************************************************/
static void ryjson_api_add_intItem_func(char* key,int32_t value,RyanJson_t *ryjson)
{
  RyanJsonAddIntToObject(*ryjson,key,value);
}

/***************************************************************************
*@Function    :
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2024-07-09
***************************************************************************/
static void ryjson_api_add_stringItem_func(char* key,char * value,RyanJson_t *ryjson)
{
  RyanJsonAddStringToObject(*ryjson, key, value);
}

/***************************************************************************
*@Function    :
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2024-07-09
***************************************************************************/
static void ryjson_api_add_objectItem_func(char* key,RyanJson_t value,RyanJson_t *ryjson)
{
  RyanJsonAddItemToObject(*ryjson,key,value);
}

/***************************************************************************
*@Function    :
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2024-07-09
***************************************************************************/
static void ryjson_api_add_nullItem_func(char* key,RyanJson_t *ryjson)
{
  RyanJsonAddNullToObject(*ryjson,key);
}

/***************************************************************************
*@Function    :
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2024-07-09
***************************************************************************/
static uint8_t ryjson_api_get_bool_byKey_func(char* key,RyanJsonBool* value,RyanJson_t *ryjson)
{
  RyanJson_t rx_jsonRoot = RyanJsonGetObjectToKey(*ryjson,key);
  if (rx_jsonRoot == NULL)
  {
    return 1; // 未找到此项
  }
  if (RyanJsonTrue != RyanJsonIsBool(rx_jsonRoot))
  {
    return 2;//此项不为bool
  }
  *value = RyanJsonGetBoolValue(rx_jsonRoot);
  return 0;
}

/***************************************************************************
*@Function    :
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2024-07-09
***************************************************************************/
static uint8_t ryjson_api_get_int_byKey_func(char* key,int32_t *value,RyanJson_t *ryjson)
{
  RyanJson_t rx_jsonRoot = RyanJsonGetObjectToKey(*ryjson,key);
  if (rx_jsonRoot == NULL)
  {
    return 1; // 未找到此项
  }
  if (RyanJsonTrue != RyanJsonIsInt(rx_jsonRoot))
  {
    return 2;//此项不为bool
  }
  *value = RyanJsonGetIntValue(rx_jsonRoot);
  return 0;
}

/***************************************************************************
*@Function    :
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2024-07-09
***************************************************************************/
static uint8_t ryjson_api_get_double_byKey_func(char* key,double *value,RyanJson_t *ryjson)
{
  RyanJson_t rx_jsonRoot = RyanJsonGetObjectToKey(*ryjson,key);
  if (rx_jsonRoot == NULL)
  {
    return 1; // 未找到此项
  }
  if (RyanJsonTrue != RyanJsonIsDouble(rx_jsonRoot))
  {
    return 2;//此项不为bool
  }
  *value = RyanJsonGetDoubleValue(rx_jsonRoot);
  return 0;
}

/***************************************************************************
*@Function    :
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2024-07-09
***************************************************************************/
static uint8_t ryjson_api_get_string_byKey_func(char* key,char** value,RyanJson_t *ryjson)
{
  RyanJson_t rx_jsonRoot = RyanJsonGetObjectToKey(*ryjson,key);
  if (rx_jsonRoot == NULL)
  {
    return 1; // 未找到此项
  }
  if (RyanJsonTrue != RyanJsonIsString(rx_jsonRoot))
  {
    return 2;//此项不为bool
  }
  *value = RyanJsonGetStringValue(rx_jsonRoot);
  return 0;
}

/***************************************************************************
*@Function    :
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2024-07-09
***************************************************************************/
static uint8_t ryjson_api_get_object_byKey_func(char* key,RyanJson_t* value,RyanJson_t *ryjson)
{
  RyanJson_t rx_jsonRoot = RyanJsonGetObjectToKey(*ryjson,key);
  if (rx_jsonRoot == NULL)
  {
    return 1; // 未找到此项
  }
  if (RyanJsonTrue != RyanJsonIsObject(rx_jsonRoot))
  {
    return 2;//此项不为bool
  }
  *value = rx_jsonRoot;
  return 0;
}

/***************************************************************************
*@Function    :
*
*@Param       :
*
*@Return      :
*
*@Description :

*@Author      :WYW            Version:  V3.0.0             Date:2024-07-09
***************************************************************************/
static char* ryjson_api_ryjson_to_string_func(RyanJson_t* ryjson)
{
  char * str = NULL;
  uint32_t real_len = 0;
  str = RyanJsonPrint(*ryjson, 256, RyanJsonTrue, &real_len); // 以带格式方式将数据打印出来
  return str;
}

static uint8_t ryjson_api_ryjson_delet_func(RyanJson_t *ryjson)
{
  RyanJsonDelete(*ryjson);
  return 0;
}


int RyJsonAPI_Init(void) 
{
  static ryjson_api_dev_stu ryjson_dev;

  ryjson_dev.ryjson_api_parse = ryjson_api_parse_func;
  ryjson_dev.ryjson_api_creat_ryjson = ryjson_api_creat_ryjson_func;

  ryjson_dev.ryjson_api_add_boolItem = ryjson_api_add_boolItem_func;
  ryjson_dev.ryjson_api_add_doubleItem = ryjson_api_add_doubleItem_func;
  ryjson_dev.ryjson_api_add_intItem = ryjson_api_add_intItem_func;
  ryjson_dev.ryjson_api_add_stringItem = ryjson_api_add_stringItem_func;
  ryjson_dev.ryjson_api_add_objectItem = ryjson_api_add_objectItem_func;
  ryjson_dev.ryjson_api_add_nullItem = ryjson_api_add_nullItem_func;

  ryjson_dev.ryjson_api_get_bool_byKey = ryjson_api_get_bool_byKey_func;
  ryjson_dev.ryjson_api_get_int_byKey = ryjson_api_get_int_byKey_func;
  ryjson_dev.ryjson_api_get_double_byKey = ryjson_api_get_double_byKey_func;
  ryjson_dev.ryjson_api_get_string_byKey = ryjson_api_get_string_byKey_func;
  ryjson_dev.ryjson_api_get_object_byKey = ryjson_api_get_object_byKey_func;

  ryjson_dev.ryjson_api_ryjson_to_string = ryjson_api_ryjson_to_string_func;
  ryjson_dev.ryjson_api_ryjson_delet = ryjson_api_ryjson_delet_func;

  cmp_dev_append(CMP_DEV_NAME_RYJSON,&ryjson_dev);
	return 0;
}
INIT_ENV_EXPORT(RyJsonAPI_Init);