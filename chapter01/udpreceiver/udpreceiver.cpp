// Module Name: udpreceiver.cpp
//
// Description:
//
//    This sample illustrates how to develop a simple UDP receiver application
//    that awaits datagrams on port 5150. This sample is implemented as a 
//    console-style application and simply prints status messages when data is
//    received.
//
// Compile:
//
//    cl -o udpreceiver udpreceiver.cpp ws2_32.lib
//
// Command Line Options:
//
//    udpreceiver.exe
//
//    NOTE: There are no command parameters. 
//


#include <winsock2.h>
#include <stdio.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main(void)
{
   WSADATA              wsaData;
   SOCKET               ReceivingSocket;
   SOCKADDR_IN          ReceiverAddr;
   int                  Port = 5150;
   char                 ReceiveBuf[1024];
   int                  BufLength = 1024;
   SOCKADDR_IN          SenderAddr;
   int                  SenderAddrSize = sizeof(SenderAddr);
   int                  Ret;


   // Initialize Winsock version 2.2

   if ((Ret = WSAStartup(MAKEWORD(2,2), &wsaData)) != 0)
   {
      // NOTE: Since Winsock failed to load we cannot use WSAGetLastError 
      // to determine the error code as is normally done when a Winsock 
      // API fails. We have to report the return status of the function.

      printf("ERROR: WSAStartup failed with error %d\n", Ret);
      return 0;
   }

   
   // Create a new socket to receive datagrams on.
 
   if ((ReceivingSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))
       == INVALID_SOCKET)
   {
      printf("ERROR: socket failed with error %d\n", WSAGetLastError());
      WSACleanup();
      return 0;
   } 

   // Setup a SOCKADDR_IN structure that will tell bind that we
   // want to receive datagrams from all interfaces using port
   // 5150.

   ReceiverAddr.sin_family = AF_INET;
   ReceiverAddr.sin_port = htons(Port);    
   ReceiverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

   // Associate the address information with the socket using bind.

   if (bind(ReceivingSocket, (SOCKADDR *)&ReceiverAddr, sizeof(ReceiverAddr))
       == SOCKET_ERROR)
   {
      printf("ERROR: bind failed with error %d\n", WSAGetLastError());
      closesocket(ReceivingSocket);
      WSACleanup();
      return 0;
   }

   printf("We are ready to receive 1 datagram from any interface on port %d...\n",
          Port);

   // At this point you can receive datagrams on your bound socket.

   if ((Ret = recvfrom(ReceivingSocket, ReceiveBuf, BufLength, 0,
       (SOCKADDR *)&SenderAddr, &SenderAddrSize)) == SOCKET_ERROR)
   {
      printf("ERROR: recvfrom failed with error %d\n", WSAGetLastError());
      closesocket(ReceivingSocket);
      WSACleanup();
      return 0;
   }

   char ipbuf[INET_ADDRSTRLEN];
   printf("We successfully received %d bytes from address %s:%d.\n", Ret,
       inet_ntop(AF_INET, &SenderAddr.sin_addr, ipbuf, sizeof(ipbuf)), ntohs(SenderAddr.sin_port));


   // When your application is finished receiving datagrams close
   // the socket.

   closesocket(ReceivingSocket);

   // When your application is finished call WSACleanup.

   WSACleanup();

   return 0;
}
