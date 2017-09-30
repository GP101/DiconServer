// Module Name: tcpclient.cpp
//
// Description:
//
//    This sample illustrates how to develop a simple TCP client application
//    that can send a simple "hello" message to a TCP server listening on port 5150.
//    This sample is implemented as a console-style application and simply prints
//    status messages a connection is made and when data is sent to the server.
//
// Compile:
//
//    cl -o tcpclient tcpclient.cpp ws2_32.lib
//
// Command Line Options:
//
//    tcpclient.exe <server IP address> 
//

#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

void main(int argc, char **argv)
{
   WSADATA              wsaData;
   SOCKET               s;
   SOCKADDR_IN          ServerAddr;
   int                  Port = 5150;
   int                  Ret;

   if (argc <= 1)
   {
      printf("USAGE: tcpclient <Server IP address>.\n");
      return;
   }

   // Initialize Winsock version 2.2

   if ((Ret = WSAStartup(MAKEWORD(2,2), &wsaData)) != 0)
   {
      // NOTE: Since Winsock failed to load we cannot use WSAGetLastError 
      // to determine the error code as is normally done when a Winsock 
      // API fails. We have to report the return status of the function.

      printf("WSAStartup failed with error %d\n", Ret);
      return;
   }
   
   // Create a new socket to make a client connection.
 
   if ((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))
       == INVALID_SOCKET)
   {
      printf("socket failed with error %d\n", WSAGetLastError());
      WSACleanup();
      return;
   }
 
   // Setup a SOCKADDR_IN structure that will be used to connect
   // to a listening server on port 5150. For demonstration
   // purposes, we required the user to supply an IP address
   // on the command line and we filled this field in with the 
   // data from the user.

   ServerAddr.sin_family = AF_INET;
   ServerAddr.sin_port = htons(Port);    
   ServerAddr.sin_addr.s_addr = inet_addr(argv[1]);

   // Make a connection to the server with socket s.

   printf("We are trying to connect to %s:%d...\n",
          inet_ntoa(ServerAddr.sin_addr), htons(ServerAddr.sin_port));

   if (connect(s, (SOCKADDR *) &ServerAddr, sizeof(ServerAddr)) 
       == SOCKET_ERROR)
   {
      printf("connect failed with error %d\n", WSAGetLastError());
      closesocket(s);
      WSACleanup();
      return;
   } 

   printf("Our connection succeeded.\n");
      
   // At this point you can start sending or receiving data on
   // the socket s. We will just send a hello message to the server.

   printf("We will now try to send a hello message.\n");

   if ((Ret = send(s, "Hello", 5, 0)) == SOCKET_ERROR)
   {
      printf("send failed with error %d\n", WSAGetLastError());
      closesocket(s);
      WSACleanup();
      return;
   }

   printf("We successfully sent %d byte(s).\n", Ret);

   // When you are finished sending and receiving data on socket s,
   // you should close the socket.

   printf("We are closing the connection.\n");

   closesocket(s);

   // When your application is finished handling the connection, call
   // WSACleanup.

   WSACleanup();
}
