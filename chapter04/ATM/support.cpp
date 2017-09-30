// Module Name: support.cpp
//
// Description:
//    This file contains various support routines used in the 
//    wsockatm.c file. The functions in this file deal with
//    enumerating local ATM interfaces as well as encoding
//    a character string addresses into NSAP style ATM addresses.
//
// Compile:
//    cl /c support.cpp
//
//
#include "support.h"

#include <stdio.h>
#include <stdlib.h>

UCHAR BtoH( CHAR ch );

//
// Function: GetNumATMInterfaces
//
// Description:
//    This function enumerates the number of ATM interfaces on
//    the local machine.
//
int  GetNumATMInterfaces(SOCKET s)
{
    DWORD  dwNum,
           nbytes=sizeof(DWORD);

    if (WSAIoctl(s, SIO_GET_NUMBER_OF_ATM_DEVICES, NULL, 0,
        (LPVOID)&dwNum, sizeof(DWORD), &nbytes, NULL, NULL) == SOCKET_ERROR)
    {
        return -1;
    }
    return dwNum;
}

//
// Function: GetATMAddress
//
// Description:
//    This function returns the ATM_ADDRESS corresponding to
//    the given interface number. The GetNumATMInterfaces function
//    returns the number of interfaces. These number can be 
//    passed as the device number into this function (ATM
//    devices begin with number 0).
//
BOOL GetATMAddress(SOCKET s,int device, ATM_ADDRESS *atmaddr)
{
    DWORD bytes;

    if (WSAIoctl(s, SIO_GET_ATM_ADDRESS, (LPVOID)&device, sizeof(DWORD),
        (LPVOID)atmaddr, sizeof(ATM_ADDRESS), &bytes, NULL, 
        NULL) == SOCKET_ERROR)
    {
        return FALSE;
    }
    return TRUE;
}

//
// Function: FindProtocol
// 
// Description: 
//    This function enumerates protocols installed on the system
//    with WSAEnumProtocols and searches for ATM transports.
//
WSAPROTOCOL_INFO *FindProtocol()
{
    WSAPROTOCOL_INFO *lpProtocolBuf=NULL,
                     *lpRet=NULL;
    DWORD             dwErr,
                      dwRet,
                      dwBufLen=0,
                      i;

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

    if (lpProtocolBuf == NULL)
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
    // that are in the AF_ATM address family.
    //
    for (i=0; i < dwRet ;i++)
    {
	if ( (lpProtocolBuf[i].iAddressFamily == AF_ATM) &&
             (lpProtocolBuf[i].iSocketType == SOCK_RAW) &&
             (lpProtocolBuf[i].iProtocol == ATMPROTO_AAL5) )
        {
            lpRet = (WSAPROTOCOL_INFO *) GlobalAlloc(GMEM_FIXED, sizeof(WSAPROTOCOL_INFO));
            memcpy(lpRet, &lpProtocolBuf[i], sizeof(WSAPROTOCOL_INFO));
            break;
        }
    }
    GlobalFree(lpProtocolBuf);

    return lpRet;
}

//
// Function: AtoH
//
// Description:
//    This function coverts the ATM address specified in the string
//    (ascii) format to the binary(hexadecimal) format.
//
void AtoH( CHAR *szDest, CHAR *szSource, INT iCount )
{
    
    while (iCount--)
    {
        *szDest++ = ( BtoH ( *szSource++ ) << 4 ) + BtoH ( *szSource++ );
    }
    return;
}

//
// Function: BtoH
//
// Description:
//    This function returns the equivalent binary value for 
//    an individual character specified in the ascii format.
//
UCHAR BtoH( CHAR ch )
{
    if ( ch >= '0' && ch <= '9' )
    {
        return ( ch - '0' );
    }

    if ( ch >= 'A' && ch <= 'F' )
    {
        return ( ch - 'A' + 0xA );
    }

    if (ch >= 'a' && ch <= 'f' )
    {
        return ( ch - 'a' + 0xA );
    }
    //
    // Illegal values in the address will not be excepted
    //
    return -1;
}
