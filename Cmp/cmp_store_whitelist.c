#include "cmp_store_whitelist.h"

static cmp_store_whitelist_stu wlist_conf;

/***************************************************************************************
* @Function    : cmp_store_whitelist_init()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/6/2
****************************************************************************************/
static uint8_t cmp_store_whitelist_init(void)
{
	if(wlist_conf.ext_flash->drv_flash_read_data(wlist_conf.param.store_addr,(uint8_t*)&wlist_conf.info,sizeof(wlist_conf.info)) > 0)
	{
		return 2;
	}
	
	if(wlist_conf.info.store_flag == CMP_STORE_FLAG)
	{
		return 0;
	}
	
	memset((uint8_t*)&wlist_conf.info,0,sizeof(wlist_conf.info));
	wlist_conf.info.store_flag = CMP_STORE_FLAG;
	if(wlist_conf.ext_flash->drv_flash_erase_sector(wlist_conf.param.store_addr)>0)
	{
		return 3;
	}
	if(wlist_conf.ext_flash->drv_flash_write_data(wlist_conf.param.store_addr,(uint8_t*)&wlist_conf.info,sizeof(wlist_conf.info))>0)
	{
		return 3;
	}
	
	return 0;
}

/***************************************************************************************
* @Function    : cmp_whitelist_set_ext_flash()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/10/9
****************************************************************************************/
static void cmp_whitelist_set_ext_flash(drv_flash_dev_stu* ext_flash,uint8_t type)
{
	wlist_conf.ext_flash = ext_flash;
	
	if(type ==0)
	{
		wlist_conf.param.store_addr = CMP_STORE_EXT_ADDR_WHITELIST;
	}
	else
	{
		wlist_conf.param.store_addr = CMP_STORE_INN_ADDR_WHITELIST;
	}
	
	cmp_store_whitelist_init();
}

/***************************************************************************************
* @Function    : cmp_whitelist_add()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/10/9
****************************************************************************************/
static uint8_t cmp_whitelist_add(uint32_t uid)
{
	cmp_store_wlist_info_stu card_info;
	
	/* card repeat check */
	for(uint8_t idx=0;idx<wlist_conf.info.cont.card_cnt;idx++)
	{
		if(uid == wlist_conf.info.cont.card_uid[idx])
		{
			return 1;
		}
	}
	
	/* card full check */
	if(wlist_conf.info.cont.card_cnt >= CMP_STORE_WHITELIST_MAX_CNT)
	{
		return 2;
	}
	
	/* add card info */
	memcpy((uint8_t*)&card_info,(uint8_t*)&wlist_conf.info,sizeof(wlist_conf.info));
	card_info.cont.card_uid[card_info.cont.card_cnt] = uid;
	card_info.cont.card_cnt++;
	
	/* save card info to flash */
	if(wlist_conf.ext_flash->drv_flash_erase_sector(wlist_conf.param.store_addr)>0)
	{
		return 3;
	}
	if(wlist_conf.ext_flash->drv_flash_write_data(wlist_conf.param.store_addr,(uint8_t*)&card_info,sizeof(card_info))>0)
	{
		return 3;
	}
	
	/* update data to lacal */
	memcpy((uint8_t*)&wlist_conf.info,(uint8_t*)&card_info,sizeof(wlist_conf.info));
	return 0;
}

/***************************************************************************************
* @Function    : cmp_whitelist_del()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/10/9
****************************************************************************************/
static uint8_t cmp_whitelist_del(uint32_t uid)
{
	uint8_t idx,pos =0;
	cmp_store_wlist_info_stu card_info;
	
	/* card exit check */
	for(idx=0;idx < wlist_conf.info.cont.card_cnt;idx++)
	{
		if(uid == wlist_conf.info.cont.card_uid[idx])
		{
			break;
		}
	}
	if(idx >= wlist_conf.info.cont.card_cnt)
	{
		return 4;
	}
	
	/* card del handle */
	memcpy((uint8_t*)&card_info,(uint8_t*)&wlist_conf.info,sizeof(wlist_conf.info));
	for(uint8_t i=0;i<card_info.cont.card_cnt;i++)
	{
		if(i == idx) 
		{
			continue;
		}
		card_info.cont.card_uid[pos++] = card_info.cont.card_uid[i];
	}
	card_info.cont.card_cnt--;
	card_info.cont.card_uid[card_info.cont.card_cnt] = 0;
	
	/* card save handle */
	if(wlist_conf.ext_flash->drv_flash_erase_sector(wlist_conf.param.store_addr)>0)
	{
		return 3;
	}
	if(wlist_conf.ext_flash->drv_flash_write_data(wlist_conf.param.store_addr,(uint8_t*)&card_info,sizeof(card_info))>0)
	{
		return 3;
	}
	
	/* update data to lacal */
	memcpy((uint8_t*)&wlist_conf.info,(uint8_t*)&card_info,sizeof(wlist_conf.info));
	return 0;
}

/***************************************************************************************
* @Function    : cmp_whitelist_query()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/10/9
****************************************************************************************/
static uint8_t cmp_whitelist_query(cmp_store_wlist_cont_stu* whitelist)
{
	if(whitelist == RT_NULL)
	{
		return 2;
	}
	
	memcpy((uint8_t*)whitelist,(uint8_t*)&wlist_conf.info.cont,sizeof(cmp_store_wlist_cont_stu));
	return 0;
}

/***************************************************************************************
* @Function    : cmp_store_whitelist_dev_create()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/10/9
****************************************************************************************/
int cmp_store_whitelist_subdev_create(void)
{
	static cmp_store_whitelist_dev_stu whitelist_dev;
	
	whitelist_dev.cmp_set_ext_flash = cmp_whitelist_set_ext_flash;
	
	whitelist_dev.store_whitelist_add = cmp_whitelist_add;
	
	whitelist_dev.store_whitelist_del = cmp_whitelist_del;
	
	whitelist_dev.store_whitelist_query = cmp_whitelist_query;
	
	cmp_dev_append(CMP_DEV_NAME_WHL,&whitelist_dev);
	return 0;
}
//INIT_COMPONENT_EXPORT(cmp_store_whitelist_dev_create);