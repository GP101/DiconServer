HANDLE CompletionPort;
WSADATA wsd;
SYSTEM_INFO SystemInfo;
SOCKADDR_IN InternetAddr;
SOCKET Listen;
int i;

typedef struct _PER_HANDLE_DATA
{
    SOCKET        Socket;
    SOCKADDR_STORAGE  ClientAddr;
    // Other information useful to be associated with the handle
} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

// Load Winsock
StartWinsock(MAKEWORD(2, 2), &wsd);

// Step 1:
// Create an I/O completion port
CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

// Step 2:
// Determine how many processors are on the system
GetSystemInfo(&SystemInfo);

// Step 3:
// Create worker threads based on the number of
// processors available on the system. For this
// simple case, we create one worker thread for each processor.
for (i = 0; i < SystemInfo.dwNumberOfProcessors; i++)
{
    HANDLE ThreadHandle;

    // Create a server worker thread, and pass the
    // completion port to the thread. NOTE: the
    // ServerWorkerThread procedure is not defined in this listing.
    ThreadHandle = CreateThread(NULL, 0, ServerWorkerThread, CompletionPort, 0, NULL;
    // Close the thread handle
    CloseHandle(ThreadHandle);
}

// Step 4:
// Create a listening socket
Listen = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

InternetAddr.sin_family = AF_INET;
InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
InternetAddr.sin_port = htons(5150);
bind(Listen, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr));

// Prepare socket for listening
listen(Listen, 5);

while (TRUE)
{
    PER_HANDLE_DATA *PerHandleData = NULL;
    SOCKADDR_IN saRemote;
    SOCKET Accept;
    int RemoteLen;

    // Step 5:
    // Accept connections and assign to the completion port
    RemoteLen = sizeof(saRemote);
    Accept = WSAAccept(Listen, (SOCKADDR *)&saRemote, &RemoteLen);

    // Step 6:
    // Create per-handle data information structure to associate with the socket
    PerHandleData = (LPPER_HANDLE_DATA)GlobalAlloc(GPTR, sizeof(PER_HANDLE_DATA));

    printf("Socket number %d connected\n", Accept);
    PerHandleData->Socket = Accept;
    memcpy(&PerHandleData->ClientAddr, &saRemote, RemoteLen);

    // Step 7:
    // Associate the accepted socket with the completion port
    CreateIoCompletionPort((HANDLE)Accept, CompletionPort, (DWORD)PerHandleData, 0);

    // Step 8:
    //  Start processing I/O on the accepted socket.
    //  Post one or more WSASend() or WSARecv() calls on the socket using overlapped I/O.
    WSARecv(...);
}

DWORD WINAPI ServerWorkerThread(LPVOID lpParam)
{
    // The requirements for the worker thread will be discussed later.
}
return 0;
}

BOOL GetQueuedCompletionStatus(
    HANDLE CompletionPort,
    LPDWORD lpNumberOfBytesTransferred,
    PULONG_PTR lpCompletionKey,
    LPOVERLAPPED * lpOverlapped,
    DWORD dwMilliseconds
    );

typedef struct
{
    OVERLAPPED Overlapped;
    char       Buffer[DATA_BUFSIZE];
    int    BufferLen;
    int        OperationType;
} PER_IO_DATA;

