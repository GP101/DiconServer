// Description:
//
//    This sample illustrates how to develop a simple echo server Winsock
//    application using the Overlapped I/O model with callback routines.
//    This sample is implemented as a console-style application and simply prints
//    messages when connections are established and removed from the server.
//    The application listens for TCP connections on port 5150 and accepts them
//    as they arrive. When this application receives data from a client, it
//    simply echos (this is why we call it an echo server) the data back in
//    it's original form until the client closes the connection.
//
//    Note: There are no command line options for this sample.
// Link to ws2_32.lib
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

#pragma comment(lib,"ws2_32.lib")

#define PORT 5150
#define DATA_BUFSIZE 8192

typedef struct _SOCKET_INFORMATION {
    OVERLAPPED Overlapped;
    SOCKET Socket;
    CHAR Buffer[DATA_BUFSIZE];
    WSABUF DataBuf;
    DWORD BytesSEND;
    DWORD BytesRECV;
} SOCKET_INFORMATION, *LPSOCKET_INFORMATION;

void CALLBACK WorkerRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags);
DWORD WINAPI WorkerThread(LPVOID lpParameter);

SOCKET AcceptSocket;

int main(int argc, char **argv)
{
    WSADATA wsaData;
    SOCKET ListenSocket;
    SOCKADDR_IN InternetAddr;
    INT Ret;
    HANDLE ThreadHandle;
    DWORD ThreadId;
    WSAEVENT AcceptEvent;

    if ((Ret = WSAStartup((2, 2), &wsaData)) != 0)
    {
        printf("WSAStartup() failed with error %d\n", Ret);
        WSACleanup();
        return 1;
    }
    else
        printf("WSAStartup() is OK!\n");

    if ((ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
    {
        printf("Failed to get a socket %d\n", WSAGetLastError());
        return 1;
    }
    else
        printf("WSASocket() is pretty fine!\n");

    InternetAddr.sin_family = AF_INET;
    InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    InternetAddr.sin_port = htons(PORT);

    if (bind(ListenSocket, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr)) == SOCKET_ERROR)
    {
        printf("bind() failed with error %d\n", WSAGetLastError());
        return 1;
    }
    else
        printf("bind() is OK!\n");

    if (listen(ListenSocket, 5))
    {
        printf("listen() failed with error %d\n", WSAGetLastError());
        return 1;
    }
    else
        printf("listen() is OK!\n");

    if ((AcceptEvent = WSACreateEvent()) == WSA_INVALID_EVENT)
    {
        printf("WSACreateEvent() failed with error %d\n", WSAGetLastError());
        return 1;
    }
    else
        printf("WSACreateEvent() is OK!\n");

    // Create a worker thread to service completed I/O requests
    if ((ThreadHandle = CreateThread(NULL, 0, WorkerThread, (LPVOID)AcceptEvent, 0, &ThreadId)) == NULL)
    {
        printf("CreateThread() failed with error %d\n", GetLastError());
        return 1;
    }
    else
        printf("CreateThread() should be fine!\n");

    while (TRUE)
    {
        AcceptSocket = accept(ListenSocket, NULL, NULL);

        if (WSASetEvent(AcceptEvent) == FALSE)
        {
            printf("WSASetEvent() failed with error %d\n", WSAGetLastError());
            return 1;
        }
        else
            printf("WSASetEvent() should be working!\n");
    }
}

DWORD WINAPI WorkerThread(LPVOID lpParameter)
{
    DWORD Flags;
    LPSOCKET_INFORMATION SocketInfo;
    WSAEVENT EventArray[1];
    DWORD Index;
    DWORD RecvBytes;

    // Save the accept event in the event array
    EventArray[0] = (WSAEVENT)lpParameter;

    while (TRUE)
    {
        // Wait for accept() to signal an event and also process WorkerRoutine() returns
        while (TRUE)
        {
            Index = WSAWaitForMultipleEvents(1, EventArray, FALSE, WSA_INFINITE, TRUE);
            if (Index == WSA_WAIT_FAILED)
            {
                printf("WSAWaitForMultipleEvents() failed with error %d\n", WSAGetLastError());
                return FALSE;
            }
            else
                printf("WSAWaitForMultipleEvents() should be OK!\n");

            if (Index != WAIT_IO_COMPLETION)
            {
                // An accept() call event is ready - break the wait loop
                break;
            }
        }

        WSAResetEvent(EventArray[Index - WSA_WAIT_EVENT_0]);
        // Create a socket information structure to associate with the accepted socket
        if ((SocketInfo = (LPSOCKET_INFORMATION)GlobalAlloc(GPTR, sizeof(SOCKET_INFORMATION))) == NULL)
        {
            printf("GlobalAlloc() failed with error %d\n", GetLastError());
            return FALSE;
        }
        else
            printf("GlobalAlloc() for SOCKET_INFORMATION is OK!\n");

        // Fill in the details of our accepted socket
        SocketInfo->Socket = AcceptSocket;
        ZeroMemory(&(SocketInfo->Overlapped), sizeof(WSAOVERLAPPED));
        SocketInfo->BytesSEND = 0;
        SocketInfo->BytesRECV = 0;
        SocketInfo->DataBuf.len = DATA_BUFSIZE;
        SocketInfo->DataBuf.buf = SocketInfo->Buffer;

        Flags = 0;
        if (WSARecv(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &RecvBytes, &Flags,
            &(SocketInfo->Overlapped), WorkerRoutine) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                printf("WSARecv() failed with error %d\n", WSAGetLastError());
                return FALSE;
            }
        }
        else
            printf("WSARecv() is OK!\n");

        printf("Socket %d got connected...\n", AcceptSocket);
    }
    return TRUE;
}

void CALLBACK WorkerRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags)
{
    DWORD SendBytes, RecvBytes;
    DWORD Flags;

    // Reference the WSAOVERLAPPED structure as a SOCKET_INFORMATION structure
    LPSOCKET_INFORMATION SI = (LPSOCKET_INFORMATION)Overlapped;

    if (Error != 0)
    {
        printf("I/O operation failed with error %d\n", Error);
    }

    if (BytesTransferred == 0)
    {
        printf("Closing socket %d\n\n", SI->Socket);
    }

    if (Error != 0 || BytesTransferred == 0)
    {
        closesocket(SI->Socket);
        GlobalFree(SI);
        return;
    }

    // Check to see if the BytesRECV field equals zero. If this is so, then
    // this means a WSARecv call just completed so update the BytesRECV field
    // with the BytesTransferred value from the completed WSARecv() call
    if (SI->BytesRECV == 0)
    {
        SI->BytesRECV = BytesTransferred;
        SI->BytesSEND = 0;
    }
    else
    {
        SI->BytesSEND += BytesTransferred;
    }

    if (SI->BytesRECV > SI->BytesSEND)
    {
        // Post another WSASend() request.
        // Since WSASend() is not guaranteed to send all of the bytes requested,
        // continue posting WSASend() calls until all received bytes are sent
        ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));
        SI->DataBuf.buf = SI->Buffer + SI->BytesSEND;
        SI->DataBuf.len = SI->BytesRECV - SI->BytesSEND;

        if (WSASend(SI->Socket, &(SI->DataBuf), 1, &SendBytes, 0, &(SI->Overlapped), WorkerRoutine) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                printf("WSASend() failed with error %d\n", WSAGetLastError());
                return;
            }
        }
        else
            printf("WSASend() is OK!\n");
    }
    else
    {
        SI->BytesRECV = 0;

        // Now that there are no more bytes to send post another WSARecv() request
        Flags = 0;
        ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));
        SI->DataBuf.len = DATA_BUFSIZE;
        SI->DataBuf.buf = SI->Buffer;

        if (WSARecv(SI->Socket, &(SI->DataBuf), 1, &RecvBytes, &Flags, &(SI->Overlapped), WorkerRoutine) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                printf("WSARecv() failed with error %d\n", WSAGetLastError());
                return;
            }
        }
        else
            printf("WSARecv() is fine!\n");
    }
}