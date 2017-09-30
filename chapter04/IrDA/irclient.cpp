// Module Name: irclient.cpp
//
// Description:
//    This sample illustrates how to create an IrSock client. This
//    sample is targeted for Windows 98, Windows Me, Windows 2000 and
//    Windows XP but can be used on Windows CE. 

//    There are conditional defines to mark the major
//    differences in the platforms. Mainly, CE requires Winsock 1.1
//    while the others require 2.2.  Also, CE must be a Windows app
//    so you can't use main(). The only thing you need to do to for
//    CE is paste this file into a project file in VC to compile it
//    for the given target processor type.
//
// Compile:
//    CE: Paste into VC project and target for your device. Link with 
//        winsock.lib
//    NT:
//        cl /D"_WIN32_WINNT" -o irclient irclient.cpp ws2_32.lib
//    Windows 98:
//        cl /D"_WIN32_WINDOWS" -o irclient irclient.cpp ws2_32.lib
//
// Command Line Parameters/Options:
//    None. The client is hardcode to attach to "MyServer". Change
//    the define IR_SERVICE_NAME if you desire something else (don't
//    forget to change the server too).
//
#ifdef _WIN32_WCE
#include <windows.h>
#include <winsock.h>
#else
#include <winsock2.h>
#endif

#include <af_irda.h>

#include <stdio.h>
#include <stdlib.h>

#define IR_SERVICE_NAME	"MyServer"
#define TEST_STRING	"This is a test of the client"
#define MAX_RETRIES     10
#define MAX_BUFFER      4096

// 
// Function: FindDevices
//
// Description:
//    This function attempts to locate any IR capable devices within
//    range. This is done by calling the IRLMP_ENUMDEVICES socket 
//    option. We call this several times in a loop to make a good
//    effort to find any.  Upon success a DEVICELIST structure is
//    filled in with the device IDs of the discovered device.
//
int FindDevices(SOCKET s, DEVICELIST *devlist)
{
    int          dwNumRetries=0,
                 dwDevListSz;
    int          ret;

    dwDevListSz = sizeof(*devlist);

    devlist->numDevice = 0; 
    while ((devlist->numDevice == 0) && (dwNumRetries <= MAX_RETRIES))
    {
        ret = getsockopt(s, SOL_IRLMP, IRLMP_ENUMDEVICES,
                           (char *)devlist, &dwDevListSz);
        if (ret == SOCKET_ERROR)
        {
	        fprintf(stderr, "getsockopt(IRLMP_ENUMDEVICES) failed: %d\n", 
		        WSAGetLastError());
            return 0;
        }
        dwNumRetries++;
        Sleep(1000);
    }
    if (dwNumRetries > MAX_RETRIES)
    {
	    return 0;
    }
    return devlist->numDevice;
}

// 
// Function: main (WinMain)
//
// Description:
//    This is the main function for the client. The appropriate
//    Winsock library is loaded, an IR socket is created, and then
//    we enumerate any IR devices within range. We then attempt to
//    connect to each of these devices to a particular service. 
//    The first connection attempt that succeeds, we take.  Once
//    connected we send and receive data.
//
#ifdef _WIN32_WCE
int WINAPI WinMain(HANDLE hInstance, HANDLE hPrevInstance,
                   LPTSTR lpCmdLine, int nCmdShow)
#else
void main(int argc, char **argv)
#endif
{
    WSADATA         wsd;
    SOCKET          sock;
    SOCKADDR_IRDA   irAddr = {AF_IRDA, 0, 0, 0, 0, "\0"};
    DWORD           dwIrSize = sizeof(SOCKADDR_IRDA),
                    i, j;
    int             ret;
    DEVICELIST      devlist;
    char            szRecvBuff[MAX_BUFFER];
    WORD            wVersion;

#ifdef _WIN32_WCE
    wVersion = MAEKWORD(1,1);
#else
    wVersion = MAKEWORD(2,2);
#endif

    if (WSAStartup(wVersion, &wsd) != 0)
    {
        printf("Unable to load the Winsock library!\n"); 
        return;
    }

    if ((sock = socket(AF_IRDA, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        printf("socket failed with error %d\n", WSAGetLastError());
        return;
    }

    if (FindDevices(sock, &devlist) == 0)
    {
        printf("No IrDA devices in range!\n");
        return;
    }

    printf("Found %d devices\n", devlist.numDevice);

    // 
    // Setup the SOCKADDR_IRDA structure with the service name we want 
    //  to connect to.
    //
    strcpy(irAddr.irdaServiceName, IR_SERVICE_NAME);

    for(i=0; i < devlist.numDevice ;i++)
    {
        for(j=0; j < 4 ;j++)
        {
            irAddr.irdaDeviceID[j] = devlist.Device[i].irdaDeviceID[j];
        }
        printf("Attempting connection to %x%x%x%x\n", 
                devlist.Device[i].irdaDeviceID[0],
                devlist.Device[i].irdaDeviceID[1],
                devlist.Device[i].irdaDeviceID[2],
                devlist.Device[i].irdaDeviceID[3]);
        if (connect(sock, (struct sockaddr *)&irAddr, 
                sizeof(SOCKADDR_IRDA)) == SOCKET_ERROR)
        {
            if (i == (devlist.numDevice - 1))
            {
                printf("Unable to locate service: '%s'\n", 
                    irAddr.irdaServiceName);
                return;
            }
            else
                printf("Connection attempt failed: %d\n", WSAGetLastError());
            continue;
        }
        else
            break;
    }


    //
    // Send the TEST_STRING to the server and receive it back.
    //

    if ((ret = send(sock, TEST_STRING, strlen(TEST_STRING), 0)) == SOCKET_ERROR)
    {
        printf("send failed with error %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return;
    }

    printf("Successfully sent %d bytes\n", ret);

    if ((ret = recv(sock, szRecvBuff, MAX_BUFFER, 0)) == SOCKET_ERROR)
    {
        printf("recv failed with error %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return;
    }

    printf("Successfully received %d bytes\n", ret);

    printf("Closing socket\n");

    closesocket(sock);
    WSACleanup();
}
