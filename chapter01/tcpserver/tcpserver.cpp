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

#pragma comment(lib, "ws2_32.lib")

void main(void)
{
   WSADATA              wsaData;
   SOCKET               ListeningSocket;
   SOCKET               NewConnection;
   SOCKADDR_IN          ServerAddr;
   SOCKADDR_IN          ClientAddr;
   int                  ClientAddrLen = sizeof(ClientAddr);
   int                  Port = 5150;
   int                  Ret;
   char                 DataBuffer[1024];

   // Initialize Winsock version 2.2

   if ((Ret = WSAStartup(MAKEWORD(2,2), &wsaData)) != 0)
   {
      // NOTE: Since Winsock failed to load we cannot use WSAGetLastError 
      // to determine the error code as is normally done when a Winsock 
      // API fails. We have to report the return status of the function.

      printf("WSAStartup failed with error %d\n", Ret);
      return;
   }
   
   // Create a new socket to listening for client connections.
 
   if ((ListeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) 
       == INVALID_SOCKET)
   {
      printf("socket failed with error %d\n", WSAGetLastError());
      WSACleanup();
      return;
   } 

   // Setup a SOCKADDR_IN structure that will tell bind that we
   // want to listen for connections on all interfaces using port
   // 5150. Notice how we convert the Port variable from host byte
   // order to network byte order.

   ServerAddr.sin_family = AF_INET;
   ServerAddr.sin_port = htons(Port);    
   ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

   // Associate the address information with the socket using bind.

   if (bind(ListeningSocket, (SOCKADDR *)&ServerAddr, sizeof(ServerAddr)) 
       == SOCKET_ERROR)
   {
      printf("bind failed with error %d\n", WSAGetLastError());
      closesocket(ListeningSocket);
      WSACleanup();
      return;
   }

   // Listen for client connections. We used a backlog of 5 which is
   // normal for many applications.

   if (listen(ListeningSocket, SOMAXCONN) == SOCKET_ERROR)
   {
      printf("listen failed with error %d\n", WSAGetLastError());
      closesocket(ListeningSocket);
      WSACleanup();
      return;
   } 

   printf("We are awaiting a connection on port %d.\n", Port);

   // Accept a new connection when one arrives.

   if ((NewConnection = accept(ListeningSocket, (SOCKADDR *) &ClientAddr,
                               &ClientAddrLen)) == INVALID_SOCKET)
   {
      printf("accept failed with error %d\n", WSAGetLastError());
      closesocket(ListeningSocket);
      WSACleanup();
      return;
   }


   printf("We successfully got a connection from %s:%d.\n",
          inet_ntoa(ClientAddr.sin_addr), ntohs(ClientAddr.sin_port));

   // At this point you can do two things with these sockets. Await
   // for more connections by calling accept again on ListeningSocket
   // and start sending or receiving data on NewConnection. For 
   // simplicity We will stop listening for more connections by closing
   // ListeningSocket. We will start sending and receiving data on
   // NewConnection.
   
   closesocket(ListeningSocket);

   // Start sending and receiving data on NewConnection. For simplicity,
   // we will just receive some data and report how many bytes were
   // received.

   printf("We are waiting to receive data...\n");

   if ((Ret = recv(NewConnection, DataBuffer, sizeof(DataBuffer), 0)) 
       == SOCKET_ERROR)
   {
      printf("recv failed with error %d\n", WSAGetLastError());
      closesocket(NewConnection);
      WSACleanup();
      return;
   } 

   printf("We successfully received %d byte(s).\n", Ret);   

   // For this application we do not plan to do anything else with the
   // connection so we will just close the connection.

   printf("We are now going to close the client connection.\n");

   closesocket(NewConnection);

   // When your application is finished handling the connections 
   // call WSACleanup.

   WSACleanup();
}
