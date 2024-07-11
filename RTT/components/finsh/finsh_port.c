/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include <rthw.h>
#include <rtconfig.h>
     
#ifndef RT_USING_FINSH
#error Please uncomment the line <#include "finsh_config.h"> in the rtconfig.h 
#endif

#ifdef RT_USING_FINSH

#define HW_FNSH_RING_LEN 128
#define WH_FINSH_RING_SIZE 8
static uint8_t hw_fnsh_head = 0;
static uint8_t hw_fnsh_trail = 0;

static char rt_hw_fnsh_ring[WH_FINSH_RING_SIZE][HW_FNSH_RING_LEN];

RT_WEAK int rt_hw_console_getchar(void)
{
    /* Note: the initial value of ch must < 0 */
    static char flag = 'N';
    static uint8_t pos = 0;
    int ch = -1,len;
    
    if(flag != 'N' && flag != 'Y')
    {
	flag = 'N';
	return -1;
    }
    
    if(flag == 'Y')
    {
	ch = rt_hw_fnsh_ring[hw_fnsh_trail][pos++];
	len = strlen(rt_hw_fnsh_ring[hw_fnsh_trail]);
	if(pos >= len)
	{
	    if(++hw_fnsh_trail >= WH_FINSH_RING_SIZE)
	    {
		hw_fnsh_trail = 0;
	    }
	    pos = 0;
	    flag = 'N';
	}
	return ch;
    }
    
    if(hw_fnsh_trail == hw_fnsh_head) return -1;
    ch = rt_hw_fnsh_ring[hw_fnsh_trail][pos++];
    flag = 'Y';
    
    return ch;
}


/***************************************************************************************
* @Function    : rt_hw_console_set_msg()
*
* @Param       : 
*
* @Return      : 
*
* @Description : 
*
* @Author      : chenbao            Version   : V3.0.0                Date:2023/7/5
****************************************************************************************/
void rt_hw_console_set_msg(uint8_t* buff,uint8_t len)
{
    if(len > HW_FNSH_RING_LEN) return;
    memset(rt_hw_fnsh_ring[hw_fnsh_head],0,HW_FNSH_RING_LEN);
    memcpy((uint8_t*)rt_hw_fnsh_ring[hw_fnsh_head],buff,len);
    if(++hw_fnsh_head >= WH_FINSH_RING_SIZE)
    {
	hw_fnsh_head = 0;
    }
}

#endif /* RT_USING_FINSH */

