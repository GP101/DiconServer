// Module Name: wsnbdef.cpp
//
// Description:
//    This file contains common definitions and functions used
//    by all the NetBIOS Winsock samples. 
//
// Compile:
//    See wsnbclnt.cpp, wsnbsvr.cpp, and wsnbdgs.cpp for compilation
//     instructions.
//
#include <winsock2.h>
#include <wsnetbs.h>

#include <stdio.h>
#include <stdlib.h>

#include "wsnbdef.h"

extern int iSocketType;

//
// Function: FindProtocol
//
// Description:
//    Search through the available network service providers for
//    AF_NETBIOS compatible protocols. The number of providers 
//    returned will be equal to 2 times the number of LANAs we
//    would have in NetBIOS. This is because there is two providers
//    for each LANA: one datagram and one session oriented provider.
//
BOOL FindProtocol(WSAPROTOCOL_INFO **wsapi, DWORD *dwCount)
{
    WSAPROTOCOL_INFO *lpProtocolBuf=NULL;
    DWORD             dwErr,
                      dwRet,
                      dwBufLen=0,
                      i;

    *dwCount = 0;
    if (SOCKET_ERROR != WSAEnumProtocols(NULL, lpProtocolBuf, 
                &dwBufLen))
    {
        // This should never happen as there is a NULL buffer
        //
        printf("WSAEnumProtocols failed!\n");
        return FALSE;
    }
    else if (WSAENOBUFS != (dwErr = WSAGetLastError()))
    {
        // We failed for some reason not relating to buffer size - 
        // also odd
        //
        printf("WSAEnumProtocols failed: %d\n", dwErr);
        return FALSE;
    }
    // Allocate the correct buffer size for WSAEnumProtocols as
    // well as the buffer to return
    //
    lpProtocolBuf = (WSAPROTOCOL_INFO *)GlobalAlloc(GMEM_FIXED, 
            dwBufLen);
    *wsapi = (WSAPROTOCOL_INFO *)GlobalAlloc(GMEM_FIXED, dwBufLen);

    if ((lpProtocolBuf == NULL) || (*wsapi == NULL))
    {
        printf("GlobalAlloc failed: %d\n", GetLastError());
        return FALSE;
    }
    dwRet = WSAEnumProtocols(NULL, lpProtocolBuf, &dwBufLen);
    if (dwRet == SOCKET_ERROR)
    {
        printf("WSAEnumProtocols failed: %d\n", WSAGetLastError());
        GlobalFree(lpProtocolBuf);
        return FALSE;
    }
    // Loop through the returned protocol information looking for those
    // that are in the AF_NETBIOS address family.
    //
    for (i=0; i < dwRet ;i++)
    {
        if (lpProtocolBuf[i].iAddressFamily == AF_NETBIOS)
        {
            if (iSocketType == SOCK_SEQPACKET)
            {
                if ( (lpProtocolBuf[i].dwServiceFlags1 & XP1_GUARANTEED_DELIVERY) &&
                        (lpProtocolBuf[i].dwServiceFlags1 & XP1_GUARANTEED_ORDER) &&
                        ((lpProtocolBuf[i].dwServiceFlags1 & XP1_CONNECTIONLESS) == 0) &&
                        (lpProtocolBuf[i].iSocketType == iSocketType))
                {
                    (*wsapi)[(*dwCount)++] = lpProtocolBuf[i];
                }
            }
            else if (iSocketType == SOCK_DGRAM)
            {
                if ( !(lpProtocolBuf[i].dwServiceFlags1 & XP1_GUARANTEED_DELIVERY) &&
                        !(lpProtocolBuf[i].dwServiceFlags1 & XP1_GUARANTEED_ORDER) &&
                        ((lpProtocolBuf[i].dwServiceFlags1 & XP1_CONNECTIONLESS) != 0) &&
                        (lpProtocolBuf[i].iSocketType == iSocketType))
                {
                    (*wsapi)[(*dwCount)++] = lpProtocolBuf[i];
                }
            }
        }
    }
    GlobalFree(lpProtocolBuf);
    return TRUE;
}

//
// Function: CullInvalidProviders
//
// Description:
//    This routine goes through a set of NetBIOS providers and verifies they
//    are valid. This is done by creating and binding a socket from each
//    provider. Some providers allow sockets to be created but will not allow
//    them to be bound while others will simply fail on socket creation.
//    If an unusuable provider is found it is removed from the array and the
//    provider count is updated.
//
void CullInvalidProviders(WSAPROTOCOL_INFO *info, DWORD *count)
{
    SOCKET      s;
    SOCKADDR_NB nbaddr;
    BOOL        bValid;
    int         limit,
                rc,
                i, j;

    if (count == NULL)
        return;

    SET_NETBIOS_SOCKADDR(&nbaddr, NETBIOS_UNIQUE_NAME, "JUNKTEST", 10);

    limit = *count;

    printf("starting with %d providers\n", limit);
    for(i=0; i < limit ;i++)
    {
        printf("%s\n", info[i].szProtocol);
    }
    printf("\n\n");

    i=0;
    while (i < limit)
    {
        bValid = FALSE;

        s = WSASocket(FROM_PROTOCOL_INFO, FROM_PROTOCOL_INFO, FROM_PROTOCOL_INFO,
                &info[i], 0, WSA_FLAG_OVERLAPPED);
        if (s != INVALID_SOCKET)
        {
            rc = bind(s, (SOCKADDR *)&nbaddr, sizeof(nbaddr));
            if (rc != SOCKET_ERROR)
            {
                bValid = TRUE;
            }
            closesocket(s);
        }

        if (bValid == FALSE)
        {
            printf("Invalid entry '%s'\n", info[i].szProtocol);
            for(j=i; j < limit-1 ;j++)
            {
                memcpy(&info[j], &info[j+1], sizeof(WSAPROTOCOL_INFO));
            }
            limit--;
            (*count)--;
        }
        else
        {
            i++;
        }
    }

    printf("resulting provider count: %d\n", *count);

    return;
}
