// Description:
//
//    This sample illustrates how to develop a simple echo server Winsock
//    application using the completeion port I/O model. This
//    sample is implemented as a console-style application and simply prints
//    messages when connections are established and removed from the server.
//    The application listens for TCP connections on port 5150 and accepts them
//    as they arrive. When this application receives data from a client, it
//    simply echos (this is why we call it an echo server) the data back in
//    it's original form until the client closes the connection.
//
//    Note: There are no command line options for this sample.
//
// Link to ws2_32.lib
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

#pragma comment(lib,"ws2_32.lib")

#define PORT 5150
#define DATA_BUFSIZE 8192

// Typedef definition
typedef struct
{
    OVERLAPPED Overlapped;
    WSABUF DataBuf;
    CHAR Buffer[DATA_BUFSIZE];
    DWORD BytesSEND;
    DWORD BytesRECV;
} PER_IO_OPERATION_DATA, *LPPER_IO_OPERATION_DATA;

// Structure definition
typedef struct
{
    SOCKET Socket;
} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

// Prototype
DWORD WINAPI ServerWorkerThread(LPVOID CompletionPortID);

int main(int argc, char **argv)
{
    SOCKADDR_IN InternetAddr;
    SOCKET Listen;
    HANDLE ThreadHandle;
    SOCKET Accept;
    HANDLE CompletionPort;
    SYSTEM_INFO SystemInfo;
    LPPER_HANDLE_DATA PerHandleData;
    LPPER_IO_OPERATION_DATA PerIoData;
    int i;
    DWORD RecvBytes;
    DWORD Flags;
    DWORD ThreadID;
    WSADATA wsaData;
    DWORD Ret;

    if ((Ret = WSAStartup((2, 2), &wsaData)) != 0)
    {
        printf("WSAStartup() failed with error %d\n", Ret);
        return 1;
    }
    else
        printf("WSAStartup() is OK!\n");

    // Setup an I/O completion port
    if ((CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0)) == NULL)
    {
        printf("CreateIoCompletionPort() failed with error %d\n", GetLastError());
        return 1;
    }
    else
        printf("CreateIoCompletionPort() is damn OK!\n");




    // Determine how many processors are on the system
    GetSystemInfo(&SystemInfo);
    // Create worker threads based on the number of processors available on the
    // system. Create two worker threads for each processor
    for (i = 0; i < (int)SystemInfo.dwNumberOfProcessors * 2; i++)
    {
        // Create a server worker thread and pass the completion port to the thread
        if ((ThreadHandle = CreateThread(NULL, 0, ServerWorkerThread, CompletionPort, 0, &ThreadID)) == NULL)
        {
            printf("CreateThread() failed with error %d\n", GetLastError());
            return 1;
        }
        else
            printf("CreateThread() is OK!\n");
        // Close the thread handle
        CloseHandle(ThreadHandle);
    }

    // Create a listening socket
    if ((Listen = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
    {
        printf("WSASocket() failed with error %d\n", WSAGetLastError());
        return 1;
    }
    else
        printf("WSASocket() is OK!\n");

    InternetAddr.sin_family = AF_INET;
    InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    InternetAddr.sin_port = htons(PORT);

    if (bind(Listen, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr)) == SOCKET_ERROR)
    {
        printf("bind() failed with error %d\n", WSAGetLastError());
        return 1;
    }
    else
        printf("bind() is fine!\n");

    // Prepare socket for listening
    if (listen(Listen, 5) == SOCKET_ERROR)
    {
        printf("listen() failed with error %d\n", WSAGetLastError());
        return 1;
    }
    else
        printf("listen() is working...\n");

    // Accept connections and assign to the completion port
    while (TRUE)
    {
        if ((Accept = WSAAccept(Listen, NULL, NULL, NULL, 0)) == SOCKET_ERROR)
        {
            printf("WSAAccept() failed with error %d\n", WSAGetLastError());
            return 1;
        }
        else
            printf("WSAAccept() looks fine!\n");

        // Create a socket information structure to associate with the socket
        if ((PerHandleData = (LPPER_HANDLE_DATA)GlobalAlloc(GPTR, sizeof(PER_HANDLE_DATA))) == NULL)
            printf("GlobalAlloc() failed with error %d\n", GetLastError());
        else
            printf("GlobalAlloc() for LPPER_HANDLE_DATA is OK!\n");
        return 1;
    }

    // Associate the accepted socket with the original completion port
    printf("Socket number %d got connected...\n", Accept);
    PerHandleData->Socket = Accept;

    if (CreateIoCompletionPort((HANDLE)Accept, CompletionPort, (DWORD)PerHandleData, 0) == NULL)
    {
        printf("CreateIoCompletionPort() failed with error %d\n", GetLastError());
        return 1;
    }
    else
        printf("CreateIoCompletionPort() is OK!\n");

    // Create per I/O socket information structure to associate with the WSARecv call below
    if ((PerIoData = (LPPER_IO_OPERATION_DATA)GlobalAlloc(GPTR, sizeof(PER_IO_OPERATION_DATA))) == NULL)
    {
        printf("GlobalAlloc() failed with error %d\n", GetLastError());
        return 1;
    }
    else
        printf("GlobalAlloc() for LPPER_IO_OPERATION_DATA is OK!\n");

    ZeroMemory(&(PerIoData->Overlapped), sizeof(OVERLAPPED));
    PerIoData->BytesSEND = 0;
    PerIoData->BytesRECV = 0;
    PerIoData->DataBuf.len = DATA_BUFSIZE;
    PerIoData->DataBuf.buf = PerIoData->Buffer;

    Flags = 0;
    if (WSARecv(Accept, &(PerIoData->DataBuf), 1, &RecvBytes, &Flags, &(PerIoData->Overlapped), NULL) == SOCKET_ERROR)
    {
        if (WSAGetLastError() != ERROR_IO_PENDING)
        {
            printf("WSARecv() failed with error %d\n", WSAGetLastError());
            return 1;
        }
    }
    else
        printf("WSARecv() is OK!\n");
}

DWORD WINAPI ServerWorkerThread(LPVOID CompletionPortID)
{
    HANDLE CompletionPort = (HANDLE)CompletionPortID;
    DWORD BytesTransferred;
    LPPER_HANDLE_DATA PerHandleData;
    LPPER_IO_OPERATION_DATA PerIoData;
    DWORD SendBytes, RecvBytes;
    DWORD Flags;

    while (TRUE)
    {
        if (GetQueuedCompletionStatus(CompletionPort, &BytesTransferred,
            (LPDWORD)&PerHandleData, (LPOVERLAPPED *)&PerIoData, INFINITE) == 0)
        {
            printf("GetQueuedCompletionStatus() failed with error %d\n", GetLastError());
            return 0;
        }
        else
            printf("GetQueuedCompletionStatus() is OK!\n");

        // First check to see if an error has occurred on the socket and if so
        // then close the socket and cleanup the SOCKET_INFORMATION structure
        // associated with the socket
        if (BytesTransferred == 0)
        {
            printf("Closing socket %d\n", PerHandleData->Socket);
            if (closesocket(PerHandleData->Socket) == SOCKET_ERROR)
            {
                printf("closesocket() failed with error %d\n", WSAGetLastError());
                return 0;
            }
            else
                printf("closesocket() is fine!\n");

            GlobalFree(PerHandleData);
            GlobalFree(PerIoData);
            continue;
        }

        // Check to see if the BytesRECV field equals zero. If this is so, then
        // this means a WSARecv call just completed so update the BytesRECV field
        // with the BytesTransferred value from the completed WSARecv() call
        if (PerIoData->BytesRECV == 0)
        {
            PerIoData->BytesRECV = BytesTransferred;
            PerIoData->BytesSEND = 0;
        }
        else
        {
            PerIoData->BytesSEND += BytesTransferred;
        }

        if (PerIoData->BytesRECV > PerIoData->BytesSEND)
        {
            // Post another WSASend() request.
            // Since WSASend() is not guaranteed to send all of the bytes requested,
            // continue posting WSASend() calls until all received bytes are sent.
            ZeroMemory(&(PerIoData->Overlapped), sizeof(OVERLAPPED));
            PerIoData->DataBuf.buf = PerIoData->Buffer + PerIoData->BytesSEND;
            PerIoData->DataBuf.len = PerIoData->BytesRECV - PerIoData->BytesSEND;

            if (WSASend(PerHandleData->Socket, &(PerIoData->DataBuf), 1, &SendBytes, 0,
                &(PerIoData->Overlapped), NULL) == SOCKET_ERROR)
            {
                if (WSAGetLastError() != ERROR_IO_PENDING)
                {
                    printf("WSASend() failed with error %d\n", WSAGetLastError());
                    return 0;
                }
            }
            else
                printf("WSASend() is OK!\n");
        }
        else
        {
            PerIoData->BytesRECV = 0;
            // Now that there are no more bytes to send post another WSARecv() request
            Flags = 0;
            ZeroMemory(&(PerIoData->Overlapped), sizeof(OVERLAPPED));
            PerIoData->DataBuf.len = DATA_BUFSIZE;
            PerIoData->DataBuf.buf = PerIoData->Buffer;

            if (WSARecv(PerHandleData->Socket, &(PerIoData->DataBuf), 1, &RecvBytes, &Flags,
                &(PerIoData->Overlapped), NULL) == SOCKET_ERROR)
            {
                if (WSAGetLastError() != ERROR_IO_PENDING)
                {
                    printf("WSARecv() failed with error %d\n", WSAGetLastError());
                    return 0;
                }
            }
            else
                printf("WSARecv() is OK!\n");
        }
    }
}