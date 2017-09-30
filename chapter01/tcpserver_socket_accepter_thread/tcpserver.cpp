// Module Name: tcpserver.cpp
//
// Description:
//
//    This sample illustrates how to develop a simple TCP server application
//    that listens for a TCP connection on port 5150 and receives data. This 
//    sample is implemented as a console-style application and simply prints 
//    status messages when a connection is accepted and when data is received 
//    by the server.
//
// Compile:
//
//    cl -o tcpserver tcpserver.cpp ws2_32.lib
//
// Command Line Options:
//
//    tcpserver.exe
//
//    NOTE: There are no command parameters. 
//


#include <winsock2.h>
#include <stdio.h>
#include "KUser.h"
#include "KSocketAccepterThread.h"

#pragma comment(lib, "ws2_32.lib")


KUserPtr CreateUser()
{
    KUserPtr    spUser;
    spUser.reset( new KUser() );
    return spUser;
}

void main(void)
{
    KSocketAccepterThread
                        socketAccepter;
   WSADATA              wsaData;
   int                  Ret;

   // Initialize Winsock version 2.2

   if ((Ret = WSAStartup(MAKEWORD(2,2), &wsaData)) != 0)
   {
      // NOTE: Since Winsock failed to load we cannot use WSAGetLastError 
      // to determine the error code as is normally done when a Winsock 
      // API fails. We have to report the return status of the function.

      printf("WSAStartup failed with error %d\n", Ret);
      return;
   }

   socketAccepter.BindAndListen( 5150, CreateUser );
   socketAccepter.BeginThread();

   printf( "Press any key to exit...\r\n" );
   _getch();

   socketAccepter.EndThread( INFINITE );
   

   // When your application is finished handling the connections 
   // call WSACleanup.

   WSACleanup();
}
