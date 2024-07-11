#include "w5500.h"
#include "socket.h"
#include "w5500_conf.h"
#include "network.h"
#include "dhcp.h"
//#include "SEGGER_RTT.h"

//#define _DHCP_DEBUG_
//#define _DHCP_RVE_DEBUG_

uint32_t dhcp_time= 0;

//CONFIG_MSG  ConfigMsg;
uint8_t W5500_mac[6]={0x00};
uint8_t W5500_lip[4] = {0x00};
uint8_t W5500_sub[4] = {0x00};
uint8_t W5500_gw[4] = {0x00};
uint8_t W5500_dns[4] = {0x00};

/// <summary>DHCP Client MAC address.  20180625.</summary>
uint8_t* DHCP_CHADDR = W5500_mac;//ConfigMsg.mac;

/// <summary>Subnet mask received from the DHCP server.</summary>
uint8_t* GET_SN_MASK = W5500_sub;

/// <summary>Gateway ip address received from the DHCP server.</summary>
uint8_t* GET_GW_IP = W5500_gw;

/// <summary>DNS server ip address received from the DHCP server.</summary>
uint8_t* GET_DNS_IP = W5500_dns;

/// <summary>Local ip address received from the DHCP server.</summary>
uint8_t* GET_SIP = W5500_lip;

/// <summary>DNS server ip address is discovered.</summary>
uint8_t	DHCP_SIP[4] = { 0 };

/// <summary>For extract my DHCP server in a few DHCP servers.</summary>
uint8_t	DHCP_REAL_SIP[4] = { 0 };

/// <summary>Previous local ip address received from DHCP server.</summary>
uint8_t	OLD_SIP[4];

uint32_t dhcp_tick_next = DHCP_WAIT_TIME;

/// <summary>Network information from DHCP Server.</summary>

/// <summary>Previous IP address.</summary>
uint8_t OLD_allocated_ip[4] = { 0, };

/// <summary>IP address from DHCP.</summary>
uint8_t DHCP_allocated_ip[4] = { 0, };

/// <summary>Gateway address from DHCP.</summary>
uint8_t DHCP_allocated_gw[4] = { 0, };

/// <summary>Subnet mask from DHCP.</summary>
uint8_t DHCP_allocated_sn[4] = { 0, };

/// <summary>DNS address from DHCP.</summary>
uint8_t DHCP_allocated_dns[4] = { 0, };

uint8_t HOST_NAME[] = EXTERN_DHCP_NAME;

/// <summary>DHCP client status.</summary>
uint8_t dhcp_state = STATE_DHCP_READY;

/// <summary>retry count.</summary>
uint8_t dhcp_retry_count = 0;

/// <summary>DHCP Timeout flag.</summary>
uint8_t DHCP_timeout = 0;

/// <summary>Leased time.</summary>
uint32_t dhcp_lease_time;

/// <summary>DHCP time count(1ms).</summary>
//uint32_t dhcp_time = 0;

/// <summary>DHCP Time 1s.</summary>
uint32_t next_dhcp_time = 0;

/// <summary>1ms.</summary>
uint32_t dhcp_tick_cnt = 0;

uint32_t DHCP_XID = DEFAULT_XID;

uint8_t EXTERN_DHCPBUF[1024];

/// <summary>Pointer for the DHCP message.</summary>
RIP_MSG* pDHCPMSG = (RIP_MSG*)EXTERN_DHCPBUF;

///-------------------------------------------------------------------------------------------------
/// <summary>The default handler of ip assign first.</summary>
///
/// <remarks>Tony Wang, 2021/6/28.</remarks>
///-------------------------------------------------------------------------------------------------

//#define DHCP_LOCAL_IP_READY					TempParam.LocalIPGetFlag

void default_ip_assign(void)
{
    setSIPR(DHCP_allocated_ip);
    setSUBR(DHCP_allocated_sn);
    setGAR(DHCP_allocated_gw);
    //Modify Config
    memcpy((void*)GET_SIP, DHCP_allocated_ip, sizeof(DHCP_allocated_ip));
    memcpy((void*)GET_SN_MASK, DHCP_allocated_sn, sizeof(DHCP_allocated_sn));
    memcpy((void*)GET_GW_IP, DHCP_allocated_gw, sizeof(DHCP_allocated_gw));
    memcpy((void*)GET_DNS_IP, DHCP_allocated_dns, sizeof(DHCP_allocated_dns));
}

///-------------------------------------------------------------------------------------------------
/// <summary>The default handler of ip chaged.</summary>
///
/// <remarks>Tony Wang, 2021/6/28.</remarks>
///-------------------------------------------------------------------------------------------------

void default_ip_update(void)
{
    /* WIZchip Software Reset */
    setMR(MR_RST);
    getMR(); // for delay
    default_ip_assign();
    setSHAR((uint8_t*)DHCP_CHADDR);
}

///-------------------------------------------------------------------------------------------------
/// <summary>The default handler of ip chaged.</summary>
///
/// <remarks>Tony Wang, 2021/6/28.</remarks>
///-------------------------------------------------------------------------------------------------

void default_ip_conflict(void)
{
    // WIZchip Software Reset
    setMR(MR_RST);
    getMR(); // for delay
    setSHAR((uint8_t*)DHCP_CHADDR);
}

void (*dhcp_ip_assign)(void) = default_ip_assign;     /* handler to be called when the IP address from DHCP server is first assigned */
void (*dhcp_ip_update)(void) = default_ip_update;     /* handler to be called when the IP address from DHCP server is updated */
void (*dhcp_ip_conflict)(void) = default_ip_conflict;   /* handler to be called when the IP address from DHCP server is conflict */

///-------------------------------------------------------------------------------------------------
/// <summary>Make the common DHCP message.</summary>
///
/// <remarks>Tony Wang, 2021/6/28.</remarks>
///-------------------------------------------------------------------------------------------------

void makeDHCPMSG(void)
{
    uint8_t  bk_mac[6];
    uint8_t* ptmp;
    uint8_t  i;

    getSHAR(bk_mac);

    pDHCPMSG->op = DHCP_BOOTREQUEST;
    pDHCPMSG->htype = DHCP_HTYPE10MB;
    pDHCPMSG->hlen = DHCP_HLENETHERNET;
    pDHCPMSG->hops = DHCP_HOPS;
    ptmp = (uint8_t*)(&pDHCPMSG->xid);
    *(ptmp + 0) = (uint8_t)((DHCP_XID & 0xFF000000) >> 24);
    *(ptmp + 1) = (uint8_t)((DHCP_XID & 0x00FF0000) >> 16);
    *(ptmp + 2) = (uint8_t)((DHCP_XID & 0x0000FF00) >> 8);
    *(ptmp + 3) = (uint8_t)((DHCP_XID & 0x000000FF) >> 0);
    pDHCPMSG->secs = DHCP_SECS;
    ptmp = (uint8_t*)(&pDHCPMSG->flags);
    *(ptmp + 0) = (uint8_t)((DHCP_FLAGSBROADCAST & 0xFF00) >> 8);
    *(ptmp + 1) = (uint8_t)((DHCP_FLAGSBROADCAST & 0x00FF) >> 0);

    pDHCPMSG->ciaddr[0] = 0;
    pDHCPMSG->ciaddr[1] = 0;
    pDHCPMSG->ciaddr[2] = 0;
    pDHCPMSG->ciaddr[3] = 0;

    pDHCPMSG->yiaddr[0] = 0;
    pDHCPMSG->yiaddr[1] = 0;
    pDHCPMSG->yiaddr[2] = 0;
    pDHCPMSG->yiaddr[3] = 0;

    pDHCPMSG->siaddr[0] = 0;
    pDHCPMSG->siaddr[1] = 0;
    pDHCPMSG->siaddr[2] = 0;
    pDHCPMSG->siaddr[3] = 0;

    pDHCPMSG->giaddr[0] = 0;
    pDHCPMSG->giaddr[1] = 0;
    pDHCPMSG->giaddr[2] = 0;
    pDHCPMSG->giaddr[3] = 0;

    pDHCPMSG->chaddr[0] = DHCP_CHADDR[0];
    pDHCPMSG->chaddr[1] = DHCP_CHADDR[1];
    pDHCPMSG->chaddr[2] = DHCP_CHADDR[2];
    pDHCPMSG->chaddr[3] = DHCP_CHADDR[3];
    pDHCPMSG->chaddr[4] = DHCP_CHADDR[4];
    pDHCPMSG->chaddr[5] = DHCP_CHADDR[5];

    for (i = 6; i < 16; i++)  pDHCPMSG->chaddr[i] = 0;
    for (i = 0; i < 64; i++)  pDHCPMSG->sname[i] = 0;
    for (i = 0; i < 128; i++) pDHCPMSG->file[i] = 0;

    // MAGIC_COOKIE
    pDHCPMSG->OPT[0] = (uint8_t)((MAGIC_COOKIE & 0xFF000000) >> 24);
    pDHCPMSG->OPT[1] = (uint8_t)((MAGIC_COOKIE & 0x00FF0000) >> 16);
    pDHCPMSG->OPT[2] = (uint8_t)((MAGIC_COOKIE & 0x0000FF00) >> 8);
    pDHCPMSG->OPT[3] = (uint8_t)(MAGIC_COOKIE & 0x000000FF) >> 0;
}

void send_DHCP_DISCOVER(void)
{
    uint8_t ip[4] = { 255,255,255,255 };
    uint16_t i = 0;

    //the host name modified
    uint8_t host_name[18] = { 0x00, };

    memset((void*)pDHCPMSG, 0, sizeof(RIP_MSG));

    pDHCPMSG->op = DHCP_BOOTREQUEST;
    pDHCPMSG->htype = DHCP_HTYPE10MB;
    pDHCPMSG->hlen = DHCP_HLENETHERNET;
    pDHCPMSG->hops = DHCP_HOPS;
    pDHCPMSG->xid = htonl(DHCP_XID);
    pDHCPMSG->secs = htons(DHCP_SECS);
    pDHCPMSG->flags = htons(DHCP_FLAGSBROADCAST);
    pDHCPMSG->chaddr[0] = DHCP_CHADDR[0];
    pDHCPMSG->chaddr[1] = DHCP_CHADDR[1];
    pDHCPMSG->chaddr[2] = DHCP_CHADDR[2];
    pDHCPMSG->chaddr[3] = DHCP_CHADDR[3];
    pDHCPMSG->chaddr[4] = DHCP_CHADDR[4];
    pDHCPMSG->chaddr[5] = DHCP_CHADDR[5];

    /* MAGIC_COOKIE */
    pDHCPMSG->OPT[i++] = (uint8_t)((MAGIC_COOKIE >> 24) & 0xFF);
    pDHCPMSG->OPT[i++] = (uint8_t)((MAGIC_COOKIE >> 16) & 0xFF);
    pDHCPMSG->OPT[i++] = (uint8_t)((MAGIC_COOKIE >> 8) & 0xFF);
    pDHCPMSG->OPT[i++] = (uint8_t)(MAGIC_COOKIE & 0xFF);

    /* Option Request Param. */
    pDHCPMSG->OPT[i++] = dhcpMessageType;
    pDHCPMSG->OPT[i++] = 0x01;
    pDHCPMSG->OPT[i++] = DHCP_DISCOVER;

    // Client identifier
    pDHCPMSG->OPT[i++] = dhcpClientIdentifier;
    pDHCPMSG->OPT[i++] = 0x07;
    pDHCPMSG->OPT[i++] = 0x01;
    pDHCPMSG->OPT[i++] = DHCP_CHADDR[0];
    pDHCPMSG->OPT[i++] = DHCP_CHADDR[1];
    pDHCPMSG->OPT[i++] = DHCP_CHADDR[2];
    pDHCPMSG->OPT[i++] = DHCP_CHADDR[3];
    pDHCPMSG->OPT[i++] = DHCP_CHADDR[4];
    pDHCPMSG->OPT[i++] = DHCP_CHADDR[5];

    // host name
    pDHCPMSG->OPT[i++] = hostName;

    // set the host name
    sprintf((char*)host_name, "%.11s-%02X%02X%02X", EXTERN_DHCP_NAME, DHCP_CHADDR[3], DHCP_CHADDR[4], DHCP_CHADDR[5]);

    pDHCPMSG->OPT[i++] = (uint8_t)strlen((char*)host_name);

    strcpy((char*)(&(pDHCPMSG->OPT[i])), (char*)host_name);

    i += (uint16_t)strlen((char*)host_name);

    pDHCPMSG->OPT[i++] = dhcpParamRequest;
    pDHCPMSG->OPT[i++] = 0x06;
    pDHCPMSG->OPT[i++] = subnetMask;
    pDHCPMSG->OPT[i++] = routersOnSubnet;
    pDHCPMSG->OPT[i++] = dns;
    pDHCPMSG->OPT[i++] = domainName;
    pDHCPMSG->OPT[i++] = dhcpT1value;
    pDHCPMSG->OPT[i++] = dhcpT2value;
    pDHCPMSG->OPT[i++] = endOption;

    /* send broadcasting packet */
    sendto(DHCP_SOCKET, (uint8_t*)pDHCPMSG, sizeof(RIP_MSG), ip, DHCP_SERVER_PORT);

    #ifdef _DHCP_DEBUG_
    Fox_ESS_Printf("sent DHCP_DISCOVER\r\n");
    #endif	
}

void send_DHCP_REQUEST(void)
{
    uint8_t  ip[4];
    uint16_t i = 0;
    uint8_t  host_name[18] = { 0x00, };

    memset((void*)pDHCPMSG, 0, sizeof(RIP_MSG));

    pDHCPMSG->op = DHCP_BOOTREQUEST;
    pDHCPMSG->htype = DHCP_HTYPE10MB;
    pDHCPMSG->hlen = DHCP_HLENETHERNET;
    pDHCPMSG->hops = DHCP_HOPS;
    pDHCPMSG->xid = htonl(DHCP_XID);
    pDHCPMSG->secs = htons(DHCP_SECS);

    if (dhcp_state < STATE_DHCP_LEASED)
        pDHCPMSG->flags = htons(DHCP_FLAGSBROADCAST);
    else
    {
        // For Unicast
        pDHCPMSG->flags = 0;

        memcpy(pDHCPMSG->ciaddr, (const void*)GET_SIP, 4);
    }
    memcpy(pDHCPMSG->chaddr, (const void*)DHCP_CHADDR, 6);

    /* MAGIC_COOKIE */
    pDHCPMSG->OPT[i++] = (uint8_t)((MAGIC_COOKIE >> 24) & 0xFF);
    pDHCPMSG->OPT[i++] = (uint8_t)((MAGIC_COOKIE >> 16) & 0xFF);
    pDHCPMSG->OPT[i++] = (uint8_t)((MAGIC_COOKIE >> 8) & 0xFF);
    pDHCPMSG->OPT[i++] = (uint8_t)(MAGIC_COOKIE & 0xFF);

    /* Option Request Param. */
    pDHCPMSG->OPT[i++] = dhcpMessageType;
    pDHCPMSG->OPT[i++] = 0x01;
    pDHCPMSG->OPT[i++] = DHCP_REQUEST;

    pDHCPMSG->OPT[i++] = dhcpClientIdentifier;
    pDHCPMSG->OPT[i++] = 0x07;
    pDHCPMSG->OPT[i++] = 0x01;
    pDHCPMSG->OPT[i++] = DHCP_CHADDR[0];
    pDHCPMSG->OPT[i++] = DHCP_CHADDR[1];
    pDHCPMSG->OPT[i++] = DHCP_CHADDR[2];
    pDHCPMSG->OPT[i++] = DHCP_CHADDR[3];
    pDHCPMSG->OPT[i++] = DHCP_CHADDR[4];
    pDHCPMSG->OPT[i++] = DHCP_CHADDR[5];

    if (dhcp_state < STATE_DHCP_LEASED)
    {
        pDHCPMSG->OPT[i++] = dhcpRequestedIPaddr;
        pDHCPMSG->OPT[i++] = 0x04;
        pDHCPMSG->OPT[i++] = GET_SIP[0];
        pDHCPMSG->OPT[i++] = GET_SIP[1];
        pDHCPMSG->OPT[i++] = GET_SIP[2];
        pDHCPMSG->OPT[i++] = GET_SIP[3];

        pDHCPMSG->OPT[i++] = dhcpServerIdentifier;
        pDHCPMSG->OPT[i++] = 0x04;
        pDHCPMSG->OPT[i++] = DHCP_SIP[0];
        pDHCPMSG->OPT[i++] = DHCP_SIP[1];
        pDHCPMSG->OPT[i++] = DHCP_SIP[2];
        pDHCPMSG->OPT[i++] = DHCP_SIP[3];
    }

    // host name
    pDHCPMSG->OPT[i++] = hostName;

    //set the host name
    sprintf((char*)host_name, (char*)"%.11s-%02X%02X%02X", EXTERN_DHCP_NAME, DHCP_CHADDR[3], DHCP_CHADDR[4], DHCP_CHADDR[5]);

    pDHCPMSG->OPT[i++] = (uint8_t)strlen((char*)host_name);

    strcpy((char*)&(pDHCPMSG->OPT[i]), (char*)host_name);

    i += strlen((char*)host_name);

    pDHCPMSG->OPT[i++] = dhcpParamRequest;
    pDHCPMSG->OPT[i++] = 0x08;
    pDHCPMSG->OPT[i++] = subnetMask;
    pDHCPMSG->OPT[i++] = routersOnSubnet;
    pDHCPMSG->OPT[i++] = dns;
    pDHCPMSG->OPT[i++] = domainName;
    pDHCPMSG->OPT[i++] = dhcpT1value;
    pDHCPMSG->OPT[i++] = dhcpT2value;
    pDHCPMSG->OPT[i++] = performRouterDiscovery;
    pDHCPMSG->OPT[i++] = staticRoute;
    pDHCPMSG->OPT[i++] = endOption;

    /* send broadcasting packet */
    if (dhcp_state < STATE_DHCP_LEASED)
        memset(ip, 0xFF, 4);
    else
        memcpy(ip, (const void*)DHCP_SIP, 4);

    sendto(DHCP_SOCKET, (const uint8_t*)pDHCPMSG, sizeof(RIP_MSG), ip, DHCP_SERVER_PORT);

    #ifdef _DHCP_DEBUG_
    Fox_ESS_Printf("sent DHCP_REQUEST\r\n");
    #endif
}

void send_DHCP_DECLINE(void)
{
    uint16_t i = 0;
    uint8_t  ip[4];

    memset((void*)pDHCPMSG, 0, sizeof(RIP_MSG));

    pDHCPMSG->op = DHCP_BOOTREQUEST;
    pDHCPMSG->htype = DHCP_HTYPE10MB;
    pDHCPMSG->hlen = DHCP_HLENETHERNET;
    pDHCPMSG->hops = DHCP_HOPS;
    pDHCPMSG->xid = htonl(DHCP_XID);
    pDHCPMSG->secs = htons(DHCP_SECS);
    pDHCPMSG->flags = 0;// DHCP_FLAGSBROADCAST;

    memcpy(pDHCPMSG->chaddr, (const void*)DHCP_CHADDR, 6);

    /* MAGIC_COOKIE */
    pDHCPMSG->OPT[i++] = (uint8_t)((MAGIC_COOKIE >> 24) & 0xFF);
    pDHCPMSG->OPT[i++] = (uint8_t)((MAGIC_COOKIE >> 16) & 0xFF);
    pDHCPMSG->OPT[i++] = (uint8_t)((MAGIC_COOKIE >> 8) & 0xFF);
    pDHCPMSG->OPT[i++] = (uint8_t)(MAGIC_COOKIE & 0xFF);

    /* Option Request Param. */
    pDHCPMSG->OPT[i++] = dhcpMessageType;
    pDHCPMSG->OPT[i++] = 0x01;
    pDHCPMSG->OPT[i++] = DHCP_DECLINE;

    pDHCPMSG->OPT[i++] = dhcpClientIdentifier;
    pDHCPMSG->OPT[i++] = 0x07;
    pDHCPMSG->OPT[i++] = 0x01;
    pDHCPMSG->OPT[i++] = DHCP_CHADDR[0];
    pDHCPMSG->OPT[i++] = DHCP_CHADDR[1];
    pDHCPMSG->OPT[i++] = DHCP_CHADDR[2];
    pDHCPMSG->OPT[i++] = DHCP_CHADDR[3];
    pDHCPMSG->OPT[i++] = DHCP_CHADDR[4];
    pDHCPMSG->OPT[i++] = DHCP_CHADDR[5];

    pDHCPMSG->OPT[i++] = dhcpServerIdentifier;
    pDHCPMSG->OPT[i++] = 0x04;
    pDHCPMSG->OPT[i++] = DHCP_SIP[0];
    pDHCPMSG->OPT[i++] = DHCP_SIP[1];
    pDHCPMSG->OPT[i++] = DHCP_SIP[2];
    pDHCPMSG->OPT[i++] = DHCP_SIP[3];

    pDHCPMSG->OPT[i++] = dhcpRequestedIPaddr;
    pDHCPMSG->OPT[i++] = 0x04;
    pDHCPMSG->OPT[i++] = GET_SIP[0];
    pDHCPMSG->OPT[i++] = GET_SIP[1];
    pDHCPMSG->OPT[i++] = GET_SIP[2];
    pDHCPMSG->OPT[i++] = GET_SIP[3];

    pDHCPMSG->OPT[i++] = endOption;

    memset(ip, 0xFF, 4);

    sendto(DHCP_SOCKET, (uint8_t*)pDHCPMSG, sizeof(RIP_MSG), ip, DHCP_SERVER_PORT);
}

uint8_t parseDHCPMSG(void)
{
    uint8_t  svr_addr[6];
    uint16_t svr_port;
    uint16_t len;
    uint8_t* p;
    uint8_t* e;
    uint8_t  type;
    uint8_t  opt_len = 0;

    len = getSn_RX_RSR(DHCP_SOCKET);

    if (len <= 0)
        return 0;

    len = recvfrom(DHCP_SOCKET, (uint8_t*)pDHCPMSG, len, svr_addr, &svr_port);

    #ifdef _DHCP_RVE_DEBUG_
    Fox_ESS_Printf("DHCP_SIP:%u.%u.%u.%u\r\n", DHCP_SIP[0], DHCP_SIP[1], DHCP_SIP[2], DHCP_SIP[3]);
    Fox_ESS_Printf("DHCP_RIP:%u.%u.%u.%u\r\n", DHCP_REAL_SIP[0], DHCP_REAL_SIP[1], DHCP_REAL_SIP[2], DHCP_REAL_SIP[3]);
    Fox_ESS_Printf("svr_addr:%u.%u.%u.%u\r\n", svr_addr[0], svr_addr[1], svr_addr[2], svr_addr[3]);
    #endif	

    if (pDHCPMSG->op != DHCP_BOOTREPLY || svr_port != DHCP_SERVER_PORT)
    {
        #ifdef _DHCP_RVE_DEBUG_   
        Fox_ESS_Printf("DHCP : NO DHCP MSG\r\n");
        #endif		
        return 0;
    }

    if (memcmp(pDHCPMSG->chaddr, (const void*)DHCP_CHADDR, 6) != 0 || pDHCPMSG->xid != htonl(DHCP_XID))
    {
        #ifdef _DHCP_RVE_DEBUG_
        Fox_ESS_Printf("No My DHCP Message. This message is ignored.\r\n");
        Fox_ESS_Printf("\tSRC_MAC_ADDR(%02X.%02X.%02X.", DHCP_CHADDR[0], DHCP_CHADDR[1], DHCP_CHADDR[2]);
        Fox_ESS_Printf("%02X.%02X.%02X)", DHCP_CHADDR[3], DHCP_CHADDR[4], DHCP_CHADDR[5]);
        Fox_ESS_Printf(", pDHCPMSG->chaddr(%02X.%02X.%02X.", (char)pDHCPMSG->chaddr[0], (char)pDHCPMSG->chaddr[1], (char)pDHCPMSG->chaddr[2]);
        Fox_ESS_Printf("%02X.%02X.%02X)", pDHCPMSG->chaddr[3], pDHCPMSG->chaddr[4], pDHCPMSG->chaddr[5]);
        Fox_ESS_Printf("\tpDHCPMSG->xid(%08X), DHCP_XID(%08X)", pDHCPMSG->xid, (DHCP_XID));
        Fox_ESS_Printf("\tpRIMPMSG->yiaddr:%d.%d.%d.%d\r\n", pDHCPMSG->yiaddr[0], pDHCPMSG->yiaddr[1], pDHCPMSG->yiaddr[2], pDHCPMSG->yiaddr[3]);
        #endif		

        return 0;
    }

    if (*((uint32_t*)DHCP_SIP) != 0x00000000)
    {
        if (*((uint32_t*)DHCP_REAL_SIP) != *((uint32_t*)svr_addr) &&
                *((uint32_t*)DHCP_SIP) != *((uint32_t*)svr_addr))
        {
            #ifdef _DHCP_RVE_DEBUG_	
            Fox_ESS_Printf("Another DHCP sever send a response message. This is ignored.\r\n");
            Fox_ESS_Printf("\tIP:%u.%u.%u.%u\r\n", svr_addr[0], svr_addr[1], svr_addr[2], svr_addr[3]);
            #endif				

            return 0;
        }
    }

    memcpy((void*)GET_SIP, pDHCPMSG->yiaddr, 4);

    #ifdef _DHCP_RVE_DEBUG_
    Fox_ESS_Printf("DHCP MSG received\r\n");
    Fox_ESS_Printf("yiaddr : %u.%u.%u.%u\r\n", GET_SIP[0], GET_SIP[1], GET_SIP[2], GET_SIP[3]);
    #endif	

    type = 0;

    p = (uint8_t*)(&pDHCPMSG->op);
    p = p + 240;
    e = p + (len - 240);

    #ifdef _DHCP_RVE_DEBUG_
    Fox_ESS_Printf("p : %08X  e : %08X  len : %d\r\n", (uint32_t)p, (uint32_t)e, len);
    #endif

    while (p < e)
    {
        switch (*p++)
        {
            case endOption:
            return type;
            
            case padOption:
            opt_len = 0;				//Added by David @20190517
            break;

            case dhcpMessageType:
            opt_len = *p++;
            type = *p;

            #ifdef _DHCP_RVE_DEBUG_		
            Fox_ESS_Printf("dhcpMessageType : %02X\r\n", type);
            #endif			
            break;

            case subnetMask:
            opt_len = *p++;
            memcpy((void*)DHCP_allocated_sn, p, 4);

            #ifdef _DHCP_RVE_DEBUG_
            Fox_ESS_Printf("subnetMask : ");
            Fox_ESS_Printf("%u.%u.%u.%u\r\n", GET_SN_MASK[0], GET_SN_MASK[1], GET_SN_MASK[2], GET_SN_MASK[3]);
            #endif			
            break;

            case routersOnSubnet:
            opt_len = *p++;
            memcpy((void*)DHCP_allocated_gw, p, 4);

            #ifdef _DHCP_RVE_DEBUG_
            Fox_ESS_Printf("routersOnSubnet : ");
            Fox_ESS_Printf("%u.%u.%u.%u\r\n", GET_GW_IP[0], GET_GW_IP[1], GET_GW_IP[2], GET_GW_IP[3]);
            #endif			
            break;

            case dns:
            opt_len = *p++;
            memcpy((void*)DHCP_allocated_dns, p, 4);
            break;

            case dhcpIPaddrLeaseTime:
            opt_len = *p++;
            dhcp_lease_time = ntohl(*((uint32_t*)p));

            #ifdef _DHCP_RVE_DEBUG_		
            Fox_ESS_Printf("dhcpIPaddrLeaseTime : %d\r\n", dhcp_lease_time);
            #endif			
            break;

            case dhcpServerIdentifier:
            opt_len = *p++;

            #ifdef _DHCP_RVE_DEBUG_					
            Fox_ESS_Printf("DHCP_SIP : %u.%u.%u.%u\r\n", DHCP_SIP[0], DHCP_SIP[1], DHCP_SIP[2], DHCP_SIP[3]);
            #endif			

            if (*((uint32_t*)DHCP_SIP) == 0 ||
                    *((uint32_t*)DHCP_REAL_SIP) == *((uint32_t*)svr_addr) ||
                        *((uint32_t*)DHCP_SIP) == *((uint32_t*)svr_addr))
            {
                memcpy(DHCP_SIP, p, 4);
                memcpy(DHCP_REAL_SIP, svr_addr, 4);	// Copy the real ip address of my DHCP server

                #ifdef _DHCP_RVE_DEBUG_					
                Fox_ESS_Printf("My dhcpServerIdentifier : ");
                Fox_ESS_Printf("%u.%u.%u.%u\r\n", DHCP_SIP[0], DHCP_SIP[1], DHCP_SIP[2], DHCP_SIP[3]);
                Fox_ESS_Printf("My DHCP server real IP address : ");
                Fox_ESS_Printf("%u.%u.%u.%u\r\n", DHCP_REAL_SIP[0], DHCP_REAL_SIP[1], DHCP_REAL_SIP[2], DHCP_REAL_SIP[3]);
                #endif				
            }
            break;

            default:
            opt_len = *p++;

            #ifdef _DHCP_RVE_DEBUG_		
            Fox_ESS_Printf("opt_len : %u\r\n", opt_len);
            #endif		
            break;
        }
        p += opt_len;
    }
    return 0;
}

///-------------------------------------------------------------------------------------------------
/// <summary>Check if a leased IP is valid.</summary>
///
/// <remarks>Tony Wang, 2021/6/28.</remarks>
///
/// <returns>Success - 1, Fail - 0..</returns>
///-------------------------------------------------------------------------------------------------

int8_t check_DHCP_leasedIP(void)
{
    uint8_t tmp;
    int32_t ret;

    //WIZchip RCR value changed for ARP Timeout count control
    tmp = getRCR();

    setRCR(0x03);

    #ifdef _DHCP_DEBUG_
    Fox_ESS_Printf("<Check the IP Conflict %d.%d.%d.%d: ", DHCP_allocated_ip[0], DHCP_allocated_ip[1], DHCP_allocated_ip[2], DHCP_allocated_ip[3]);
    #endif

    // IP conflict detection : ARP request - ARP reply
    // Broadcasting ARP Request for check the IP conflict using UDP sendto() function
    ret = sendto(DHCP_SOCKET, (uint8_t*)"CHECK_IP_CONFLICT", 17, DHCP_allocated_ip, 5000);

    // RCR value restore
    setRCR(tmp);

    if (ret)
    {
    // Received ARP reply or etc : IP address conflict occur, DHCP Failed
    send_DHCP_DECLINE();
    ret = dhcp_time;
    while ((dhcp_time - ret) < 2);   // wait for 1s over; wait to complete to send DECLINE message;
    return 0;
    }
    else
    {
    // UDP send Timeout occurred : allocated IP address is unique, DHCP Success
    #ifdef _DHCP_DEBUG_
    Fox_ESS_Printf("\r\n> Check leased IP - OK\r\n");
    #endif
    return 1;
    }
}

///-------------------------------------------------------------------------------------------------
/// <summary>Timer interrupt handler(For checking dhcp lease time), Increase 'my_time' each one second.</summary>
///
/// <remarks>Tony Wang, 2021/6/28.</remarks>
///-------------------------------------------------------------------------------------------------

void DHCP_Timer_Handler(void)
{
    dhcp_time++;
}

///-------------------------------------------------------------------------------------------------
/// <summary>Resets the DHCP timeout.</summary>
///
/// <remarks>Tony Wang, 2021/6/28.</remarks>
///-------------------------------------------------------------------------------------------------

void reset_DHCP_timeout(void)//20180625
{
	dhcp_time = 0;
	dhcp_tick_cnt = 0;
	dhcp_tick_next = DHCP_WAIT_TIME;
	dhcp_retry_count = 0;
}

///-------------------------------------------------------------------------------------------------
/// <summary>Check DHCP timeout.</summary>
///
/// <remarks>Tony Wang, 2021/6/28.</remarks>
///
/// <returns>DHCP state.</returns>
///-------------------------------------------------------------------------------------------------

uint8_t check_DHCP_timeout(void)//20180625
{
    uint8_t ret = DHCP_RUNNING;

    if (dhcp_retry_count < MAX_DHCP_RETRY)
    {
        if (dhcp_tick_next < dhcp_time)
        {
            switch (dhcp_state)
            {
                case STATE_DHCP_DISCOVER:
                #ifdef _DHCP_DEBUG_  
                Fox_ESS_Printf("<<timeout>> state : STATE_DHCP_DISCOVER\r\n");
                #endif
                send_DHCP_DISCOVER();
                break;
                
                case STATE_DHCP_REQUEST:
                #ifdef _DHCP_DEBUG_  
                Fox_ESS_Printf("<<timeout>> state : STATE_DHCP_REQUEST\r\n");
                #endif
                send_DHCP_REQUEST();
                break;
                
                case STATE_DHCP_REREQUEST:
                #ifdef _DHCP_DEBUG_  
                Fox_ESS_Printf("<<timeout>> state : STATE_DHCP_REREQUEST\r\n");
                #endif				
                send_DHCP_REQUEST();
                break;

                default:
                break;
            }

            dhcp_time = 0;
            dhcp_tick_next = dhcp_time + DHCP_WAIT_TIME;
            dhcp_retry_count++;
        }
    }
    else
    { // timeout occurred
        switch (dhcp_state)
        {
            case STATE_DHCP_DISCOVER:
            dhcp_state = STATE_DHCP_READY;
            ret = DHCP_FAILED;
            break;

            case STATE_DHCP_REQUEST:
            case STATE_DHCP_REREQUEST:
            send_DHCP_DISCOVER();
            dhcp_time = 0;
            dhcp_state = STATE_DHCP_DISCOVER;
            break;
            
            default:
            break;
        }
        reset_DHCP_timeout();
    }
    return ret;
}

///-------------------------------------------------------------------------------------------------
/// <summary>Initializes the DHCP client.</summary>
///
/// <remarks>Tony Wang, 2021/6/28.</remarks>
///-------------------------------------------------------------------------------------------------

void DHCP_Client_Init(void)
{
    uint8_t ip_0[4] = { 0, };

    DHCP_XID = 0x12345678;

    memset(OLD_SIP, 0, sizeof(OLD_SIP));
    memset(DHCP_SIP, 0, sizeof(DHCP_SIP));
    memset(DHCP_REAL_SIP, 0, sizeof(GET_SN_MASK));
    memcpy(DHCP_CHADDR, ConfigMsg.mac, 6);

    iinchip_init();

    setSHAR((uint8_t*)DHCP_CHADDR);
    setSUBR(ip_0);
    setGAR(ip_0);
    setSIPR(ip_0);

    //clear ip setted flag
    dhcp_state = STATE_DHCP_READY;

    #ifdef _DHCP_DEBUG_
	Fox_ESS_Printf("SET W5500 MAC=%02x:%02x:%02x:%02x:%02x:%02x\r\n", DHCP_CHADDR[0], DHCP_CHADDR[1], DHCP_CHADDR[2], DHCP_CHADDR[3], DHCP_CHADDR[4], DHCP_CHADDR[5]);
	Fox_ESS_Printf("init_dhcp_client:%u\r\n", DHCP_SOCKET);
    #endif   
}

///-------------------------------------------------------------------------------------------------
/// <summary>Process the DHCP client.</summary>
///
/// <remarks>Tony Wang, 2021/6/28.</remarks>
///
/// <returns>An uint8_t.</returns>
///-------------------------------------------------------------------------------------------------

uint8_t check_DHCP_state(void)
{
    uint8_t  type = 0;
    uint8_t  ret = DHCP_RUNNING;

    if (getSn_SR(DHCP_SOCKET) != SOCK_CLOSED)
        type = parseDHCPMSG();
    else
    {
        if (!socket(DHCP_SOCKET, Sn_MR_UDP, DHCP_CLIENT_PORT, 0x00))
        {
            #ifdef _DHCP_DEBUG_
            Fox_ESS_Printf("DHCP port init error...\r\n");
            #endif
            dhcp_state = STATE_DHCP_READY;
            return DHCP_FAILED;
        }
    }

    switch (dhcp_state)
    {
        case STATE_DHCP_READY:
        DHCP_allocated_ip[0] = 0;
        DHCP_allocated_ip[1] = 0;
        DHCP_allocated_ip[2] = 0;
        DHCP_allocated_ip[3] = 0;

        send_DHCP_DISCOVER();
        /*dhcp_time = 0;*/
        reset_DHCP_timeout();
        dhcp_state = STATE_DHCP_DISCOVER;
        break;

        case STATE_DHCP_DISCOVER:
        if (type == DHCP_OFFER)
        {
            #ifdef _DHCP_DEBUG_
            Fox_ESS_Printf("> Receive DHCP_OFFER\r\n");
            #endif

            DHCP_allocated_ip[0] = pDHCPMSG->yiaddr[0];
            DHCP_allocated_ip[1] = pDHCPMSG->yiaddr[1];
            DHCP_allocated_ip[2] = pDHCPMSG->yiaddr[2];
            DHCP_allocated_ip[3] = pDHCPMSG->yiaddr[3];

            send_DHCP_REQUEST();
            dhcp_time = 0;
            dhcp_state = STATE_DHCP_REQUEST;
        }
        else
            ret = check_DHCP_timeout();
        break;

        case STATE_DHCP_REQUEST:
        if (type == DHCP_ACK)
        {
            reset_DHCP_timeout();
            #ifdef _DHCP_DEBUG_
                Fox_ESS_Printf("> Receive DHCP_ACK\r\n");
            #endif
            if (check_DHCP_leasedIP())
            {
                #ifdef _DHCP_DEBUG_
                Fox_ESS_Printf("DHCP Update\r\n");
                // Network info assignment from DHCP
                Fox_ESS_Printf("W5500EVB IP : %d.%d.%d.%d\r\n", DHCP_allocated_ip[0], DHCP_allocated_ip[1], DHCP_allocated_ip[2], DHCP_allocated_ip[3]);
                Fox_ESS_Printf("W5500EVB SN : %d.%d.%d.%d\r\n", DHCP_allocated_sn[0], DHCP_allocated_sn[1], DHCP_allocated_sn[2], DHCP_allocated_sn[3]);
                Fox_ESS_Printf("W5500EVB GW : %d.%d.%d.%d\r\n", DHCP_allocated_gw[0], DHCP_allocated_gw[1], DHCP_allocated_gw[2], DHCP_allocated_gw[3]);
                #endif
                dhcp_ip_assign();
                ret = DHCP_IP_ASSIGN;
                dhcp_state = STATE_DHCP_LEASED;
            }
            else
            {
                // IP address conflict occurred
                dhcp_ip_conflict();
                ret = DHCP_IP_CONFLICT;
                dhcp_state = STATE_DHCP_READY;
            }
        }
        else if (type == DHCP_NAK)
        {
            #ifdef _DHCP_DEBUG_
            Fox_ESS_Printf("> Receive DHCP_NACK\r\n");
            #endif
            reset_DHCP_timeout();
            dhcp_state = STATE_DHCP_DISCOVER;
        }
        else
            ret = check_DHCP_timeout();
	break;

        case STATE_DHCP_LEASED:
        ret = DHCP_IP_LEASED;
        if ((dhcp_lease_time != DEFAULT_LEASETIME) && ((dhcp_lease_time / 2) < dhcp_time))
        {
            #ifdef _DHCP_DEBUG_
            Fox_ESS_Printf("> Maintains the IP address \r\n");
            #endif

            type = 0;
            memcpy(OLD_allocated_ip, DHCP_allocated_ip, 4);
            DHCP_XID++;
            send_DHCP_REQUEST();
            reset_DHCP_timeout();
            dhcp_state = STATE_DHCP_REREQUEST;
        }
        break;

	case STATE_DHCP_REREQUEST:
        ret = DHCP_IP_LEASED;
        if (type == DHCP_ACK)
        {
            dhcp_retry_count = 0;
            if(memcmp(OLD_allocated_ip,DHCP_allocated_ip,4)!=0)
            {
                ret = DHCP_IP_CHANGED;

                //dhcp_ip_update();

                #ifdef _DHCP_DEBUG_
                Fox_ESS_Printf(">IP changed.\r\n");
                #endif					
            }
            #ifdef _DHCP_DEBUG_
            else
                Fox_ESS_Printf(">IP is continued.\r\n");
            #endif
            reset_DHCP_timeout();
            dhcp_state = STATE_DHCP_LEASED;
        }
        else if (type == DHCP_NAK)
        {
            #ifdef _DHCP_DEBUG_
            Fox_ESS_Printf("> Receive DHCP_NACK, Failed to maintain ip\r\n");
            #endif
            reset_DHCP_timeout();
            dhcp_state = STATE_DHCP_DISCOVER;
        }
        else
            ret = check_DHCP_timeout();
        break;

	default:
        dhcp_state = STATE_DHCP_READY;
        break;
    }
    return ret;
}

