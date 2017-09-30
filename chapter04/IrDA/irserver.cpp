// Module Name: irserver.cpp
//
// Description:
//    This sample illustrates a IrSock server. This sampe is targeted
//    towards Windows 98, Windows Me, Windows 2000 and Windows XP but 
//    can also be used on Windows CE.
//
//    There are conditional defines to makr the major differences in 
//    the platforms. Mainly, CE requires Winsock 1.1 while the others 
//    require 2.2.  Also, CE must be a Windows app so you can't use 
//    main(). The only thing you need to do to for CE is paste this 
//    file into a project file in VC to compile it for the given target 
//    processor type.
//
// Compile:
//    CE: Paste into VC project and target for your device. Link with 
//        winsock.lib
//    NT:
//        cl /D"_WIN32_WINNT" -o irserver irserver.cpp ws2_32.lib
//    Windows 98:
//        cl /D"_WIN32_WINDOWS" -o irserver irserver.cpp ws2_32.lib
//
// Command Line Parameters/Options:
//    None. The server is hardcode to listen on "MyServer". Change
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


#define IR_SERVICE_NAME      "MyServer"
#define MAX_BUFFER        4096

//
// Function: ClientThread
//
// Description:
//    This is a client thread that is spawned with each client
//    connection.  The thread simply reads data and writes it
//    back to the client until the socket is closed.
//
DWORD WINAPI ClientThread(LPVOID lpParam)
{
    SOCKET        s = (SOCKET)lpParam;
    int           ret;
    char          szRecvBuff[MAX_BUFFER];

    while (1)
    {
        //
        // Read data from client
        //

        if ((ret = recv(s, szRecvBuff, MAX_BUFFER, 0)) == SOCKET_ERROR)
        {
            printf("recv failed with error %d\n", WSAGetLastError());
            break;
        }

        if (ret == 0)
            break;

        printf("Successfully received %d bytes\n", ret);

        //
        // Echo data back to client
        //

        if ((ret = send(s, szRecvBuff, ret, 0)) == SOCKET_ERROR)
        {
            printf("send failed with error %d\n", WSAGetLastError());
            break;
        }

        printf("Successfully sent %d bytes\n", ret);
    }

    printf("Closing connection\n");

    closesocket(s);
    return 0;
}

//
// Function: main (Winmain)
//
// Description:
//    This is the main function of the server. The appropriate Winsock
//    library is loaded first, an IR listening socket is created, and
//    we bind the socket to our server name. Afterwards we block on
//    an accept() in a loop. Once a client connection is made, we
//    spawn a thread to handle th connection.
//
#ifdef _WIN32_WCE
int WINAPI WinMain(HANDLE hInstance, HANDLE hPrevIntance,
                   LPTSTR lpCmdLine, int nCmdShow)
#else
void main(int argc, char **argv)
#endif
{
    WSADATA       wsd;
    SOCKET        sock,
                  sockClient;
    SOCKADDR_IRDA irAddr = {AF_IRDA, 0, 0, 0, 0, "\0"},
                  remoteIrAddr;
    int           iIrSize = sizeof(SOCKADDR_IRDA);
    DWORD         dwId;
    BOOL          bDone=FALSE;
    HANDLE        hThread;
    WORD          wVersion;

#ifdef _WIN32_WCE
    wVersion = MAKEWORD(1,1);
#else
    wVersion = MAKEWORD(2,2);
#endif

    if (WSAStartup(wVersion, &wsd) != 0)
    {
        printf("Unable to load Winsock library!\n");
        return;
    }

    if ((sock = socket(AF_IRDA, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        printf("socket failed with error %d\n", WSAGetLastError());
        return;
    }

    strcpy(irAddr.irdaServiceName, IR_SERVICE_NAME);

    //
    // Bind our socket to the local service name
    //
    printf("Binding to service class name: %s\n", irAddr.irdaServiceName);

    if (bind(sock, (struct sockaddr *)&irAddr, sizeof(SOCKADDR_IRDA)) == SOCKET_ERROR)
    {
        printf("bind failed with error %d\n", WSAGetLastError());
        return;
    }
 
    listen(sock, 10);

    printf("Bound and listening\n");

    while (1)
    {
        if ((sockClient = accept(sock, (struct sockaddr *)&remoteIrAddr, &iIrSize)) == SOCKET_ERROR)
        {
            printf("accept failed with error %d\n", WSAGetLastError());
            return;
        }
 
        printf("Accepted a connection\n");

        // Process I/O in a seperate thread.

        hThread = CreateThread(NULL, 0, ClientThread, (LPVOID)sockClient, 0, &dwId);

        CloseHandle(hThread);
    }

    // Close and cleanup
    //
    closesocket(sock);

    WSACleanup();
}
