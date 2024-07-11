#ifndef __NETWORK_H
#define __NETWORK_H

#include <stdint.h>

uint16_t htons(uint16_t hostshort);

unsigned long htonl(unsigned long hostlong);

unsigned long ntohs(unsigned short netshort);

unsigned long ntohl(unsigned long netlong);

void a2A(char* sbuf, char* dbuf);

void Netwwork_Init(void);

uint8_t Link_Detect(void);

uint8_t Local_IP_Check(void);

uint8_t Pack_Discover(void);

#endif	
