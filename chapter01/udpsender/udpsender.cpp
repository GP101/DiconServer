// Module Name: udpsender.cpp
//
// Description:
//
//    This sample illustrates how to develop a simple UDP sender application
//    that can send a simple "hello" message to a UDP receiver awaiting datagrams
//    on port 5150. This sample is implemented as a console-style application and 
//    simply prints status messages as data is sent to the server.
//
// Compile:
//
//    cl -o udpsender udpsender.cpp ws2_32.lib
//
// Command Line Options:
//
//    udpsender.exe <receiver IP address> 
//

//#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <stdio.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

void main(int argc, char **argv)
{
   WSADATA              wsaData;
   SOCKET               SendingSocket;
   SOCKADDR_IN          ReceiverAddr;
   int                  Port = 5150;
   int                  Ret;

   if (argc <= 1)
   {
      printf("USAGE: udpsender <receiver IP address>.\n");
      return;
   }

   // Initialize Winsock version 2.2

   if ((Ret = WSAStartup(MAKEWORD(2,2), &wsaData)) != 0)
   {
      // NOTE: Since Winsock failed to load we cannot use WSAGetLastError 
      // to determine the error code as is normally done when a Winsock 
      // API fails. We have to report the return status of the function.

      printf("ERROR: WSAStartup failed with error %d\n", Ret);
      return;
   }
   
   // Create a new socket to receive datagrams on.
 
   if ((SendingSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))
       == INVALID_SOCKET)
   {
      printf("ERROR: socket failed with error %d\n", WSAGetLastError());
      WSACleanup();
      return;
   }

   // Setup a SOCKADDR_IN structure that will identify who we
   // will send datagrams to. For demonstration purposes, let's
   // assume our receiver's IP address is 136.149.3.29 and waits
   // for datagrams on port 5150. Obviously you will want to prompt 
   // the user for an IP address and port number and fill these 
   // fields in with the data from the user.
   ReceiverAddr.sin_family = AF_INET;
   ReceiverAddr.sin_port = htons(Port);    
   //ReceiverAddr.sin_addr.s_addr = inet_addr(argv[1]);
   InetPtonA(AF_INET, argv[1], &ReceiverAddr.sin_addr.s_addr);

   // Send a datagram to the receiver.

   if ((Ret = sendto(SendingSocket, "Hello", 5, 0, 
       (SOCKADDR *)&ReceiverAddr, sizeof(ReceiverAddr))) == SOCKET_ERROR)
   {
      printf("ERROR: sendto failed with error %d\n", WSAGetLastError());
      closesocket(SendingSocket);
      WSACleanup();
      return;
   }

   // When your application is finished sending datagrams close
   // the socket.

   char ipbuf[INET_ADDRSTRLEN];
   printf("We successfully sent %d byte(s) to %s:%d.\n", Ret,
       inet_ntop(AF_INET, &ReceiverAddr.sin_addr, ipbuf, sizeof(ipbuf)),htons(ReceiverAddr.sin_port));

   closesocket(SendingSocket);

   // When your application is finished call WSACleanup.

   WSACleanup();
}

