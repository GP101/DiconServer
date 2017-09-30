int WSAStartup(
    WORD wVersionRequested,
    LPWSADATA lpWSAData
    );

typedef struct WSAData
{
    WORD           wVersion;
    WORD           wHighVersion;
    char           szDescription[WSADESCRIPTION_LEN + 1];
    char           szSystemStatus[WSASYS_STATUS_LEN + 1];
    unsigned short iMaxSockets;
    unsigned short iMaxUdpDg;
    char FAR *     lpVendorInfo;
} WSADATA, *LPWSADATA;

int WSACleanup(void);

int WSAGetLastError(void);

#include <winsock2.h>

void main(void)
{
    WSADATA wsaData;

    // Initialize Winsock version 2.2

    if ((Ret = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
    {
        // NOTE: Since Winsock failed to load we cannot use 
        // WSAGetLastError to determine the specific error for
        // why it failed. Instead we can rely on the return 
        // status of WSAStartup.

        printf("WSAStartup failed with error %d\n", Ret);
        return;
    }

    // Setup Winsock communication code here 

    // When your application is finished call WSACleanup
    if (WSACleanup() == SOCKET_ERROR)
    {
        printf("WSACleanup failed with error %d\n", WSAGetLastError());
    }
}

struct sockaddr_in
{
    short           sin_family;
    u_short         sin_port;
    struct in_addr  sin_addr;
    char            sin_zero[8];
};

unsigned long inet_addr(
    const char FAR *cp
    );

u_long htonl(u_long hostlong);

int WSAHtonl(
    SOCKET s,
    u_long hostlong,
    u_long FAR * lpnetlong
    );

u_short htons(u_short hostshort);

int WSAHtons(
    SOCKET s,
    u_short hostshort,
    u_short FAR * lpnetshort
    );


u_long ntohl(u_long netlong);

int WSANtohl(
    SOCKET s,
    u_long netlong,
    u_long FAR * lphostlong
    );

u_short ntohs(u_short netshort);

int WSANtohs(
    SOCKET s,
    u_short netshort,
    u_short FAR * lphostshort
    );


SOCKADDR_IN InternetAddr;
INT nPortId = 5150;

InternetAddr.sin_family = AF_INET;

// Convert the proposed dotted Internet address 136.149.3.29
// to a four-byte integer, and assign it to sin_addr

InternetAddr.sin_addr.s_addr = inet_addr("136.149.3.29");

// The nPortId variable is stored in host-byte order. Convert
// nPortId to network-byte order, and assign it to sin_port.

InternetAddr.sin_port = htons(nPortId);

SOCKET socket(
    int af,
    int type,
    int protocol
    );

int bind(
    SOCKET                     s,
    const struct sockaddr FAR* name,
    int                        namelen
    );

SOCKET               s;
SOCKADDR_IN          tcpaddr;
int                  port = 5150;

s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

tcpaddr.sin_family = AF_INET;
tcpaddr.sin_port = htons(port);
tcpaddr.sin_addr.s_addr = htonl(INADDR_ANY);

bind(s, (SOCKADDR *)&tcpaddr, sizeof(tcpaddr));


int listen(
    SOCKET s,
    int    backlog
    );

SOCKET accept(
    SOCKET s,
struct sockaddr FAR* addr,
    int FAR* addrlen
    );

#include <winsock2.h>

void main(void)
{
    WSADATA              wsaData;
    SOCKET               ListeningSocket;
    SOCKET               NewConnection;
    SOCKADDR_IN          ServerAddr;
    SOCKADDR_IN          ClientAddr;
    int                  Port = 5150;

    // Initialize Winsock version 2.2

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Create a new socket to listen for client connections.

    ListeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Set up a SOCKADDR_IN structure that will tell bind that we
    // want to listen for connections on all interfaces using port
    // 5150. Notice how we convert the Port variable from host byte
    // order to network byte order.

    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(Port);
    ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Associate the address information with the socket using bind.

    bind(ListeningSocket, (SOCKADDR *)&ServerAddr,
        sizeof(ServerAddr));

    // Listen for client connections. We used a backlog of 5, which
    // is normal for many applications.

    listen(ListeningSocket, 5);

    // Accept a new connection when one arrives.

    NewConnection = accept(ListeningSocket, (SOCKADDR *)
        &ClientAddr, &ClientAddrLen));

    // At this point you can do two things with these sockets. Wait
    // for more connections by calling accept again on ListeningSocket
    // and start sending or receiving data on NewConnection. We will
    // describe how to send and receive data later in the chapter.

    // When you are finished sending and receiving data on the
    // NewConnection socket and are finished accepting new connections
    // on ListeningSocket, you should close the sockets using the
    // closesocket API. We will describe socket closure later in the 
    // chapter.

    closesocket(NewConnection);
    closesocket(ListeningSocket);

    // When your application is finished handling the connections, 
    // call WSACleanup.

    WSACleanup();
}

#include <winsock2.h>

void main(void)
{
    WSADATA              wsaData;
    SOCKET               s;
    SOCKADDR_IN          ServerAddr;
    int                  Port = 5150;

    // Initialize Winsock version 2.2

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Create a new socket to make a client connection.

    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Set up a SOCKADDR_IN structure that will be used to connect
    // to a listening server on port 5150. For demonstration
    // purposes, let's assume our server's IP address is 136.149.3.29.
    // Obviously, you will want to prompt the user for an IP address
    // and fill in this field with the user's data.

    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(Port);
    ServerAddr.sin_addr.s_addr = inet_addr("136.149.3.29");

    // Make a connection to the server with socket s.

    connect(s, (SOCKADDR *)&ServerAddr, sizeof(ServerAddr));

    // At this point you can start sending or receiving data on
    // the socket s. We will describe sending and receiving data
    // later in the chapter.

    // When you are finished sending and receiving data on socket s,
    // you should close the socket using the closesocket API. We will
    // describe socket closure later in the chapter.

    closesocket(s);

    // When your application is finished handling the connection, call
    // WSACleanup.

    WSACleanup();
}


int send(
    SOCKET s,
    const char FAR * buf,
    int len,
    int flags
    );

int WSASend(
    SOCKET s,
    LPWSABUF lpBuffers,
    DWORD dwBufferCount,
    LPDWORD lpNumberOfBytesSent,
    DWORD dwFlags,
    LPWSAOVERLAPPED lpOverlapped,
    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
    );

int recv(
    SOCKET s,
    char FAR* buf,
    int len,
    int flags
    );

int WSARecv(
    SOCKET s,
    LPWSABUF lpBuffers,
    DWORD dwBufferCount,
    LPDWORD lpNumberOfBytesRecvd,
    LPDWORD lpFlags,
    LPWSAOVERLAPPED lpOverlapped,
    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
    );

char sendbuff[2048];
int  nBytes = 2048;

// Fill sendbuff with 2048 bytes of data

// Assume s is a valid, connected stream socket
ret = send(s, sendbuff, nBytes, 0);

char sendbuff[2048];
int  nBytes = 2048,
nLeft,
idx;

// Fill sendbuff with 2048 bytes of data

// Assume s is a valid, connected stream socket
nLeft = nBytes;
idx = 0;

while (nLeft > 0)
{
    ret = send(s, &sendbuff[idx], nLeft, 0);
    if (ret == SOCKET_ERROR)
    {
        // Error
    }
    nLeft -= ret;
    idx += ret;
}


char    recvbuff[1024];
int     ret,
nLeft,
idx;

nLeft = 512;
idx = 0;

while (nLeft > 0)
{
    ret = recv(s, &recvbuff[idx], nLeft, 0);
    if (ret == SOCKET_ERROR)
    {
        // Error
    }
    idx += ret;
    nLeft -= ret;
}


int shutdown(
    SOCKET s,
    int how
    );

int closesocket(SOCKET s);

int recvfrom(
    SOCKET s,
    char FAR* buf,
    int len,
    int flags,
struct sockaddr FAR* from,
    int FAR* fromlen
    );

int WSARecvFrom(
    SOCKET s,
    LPWSABUF lpBuffers,
    DWORD dwBufferCount,
    LPDWORD lpNumberOfBytesRecvd,
    LPDWORD lpFlags,
struct sockaddr FAR * lpFrom,
    LPINT lpFromlen,
    LPWSAOVERLAPPED lpOverlapped,
    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
    );

#include <winsock2.h>

void main(void)
{
    WSADATA              wsaData;
    SOCKET               ReceivingSocket;
    SOCKADDR_IN          ReceiverAddr;
    int                  Port = 5150;
    char                 ReceiveBuf[1024];
    int                  BufLength = 1024;
    SOCKADDR_IN          SenderAddr;
    int                  SenderAddrSize = sizeof(SenderAddr);

    // Initialize Winsock version 2.2

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Create a new socket to receive datagrams on.

    ReceivingSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    // Set up a SOCKADDR_IN structure that will tell bind that we
    // want to receive datagrams from all interfaces using port
    // 5150.

    ReceiverAddr.sin_family = AF_INET;
    ReceiverAddr.sin_port = htons(Port);
    ReceiverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Associate the address information with the socket using bind.

    bind(ReceivingSocket, (SOCKADDR *)&SenderAddr, sizeof(SenderAddr));

    // At this point you can receive datagrams on your bound socket.
    recvfrom(ReceivingSocket, ReceiveBuf, BufLength, 0,
        (SOCKADDR *)&SenderAddr, &SenderAddrSize);

    // When your application is finished receiving datagrams close
    // the socket.

    closesocket(ReceivingSocket);

    // When your application is finished call WSACleanup.

    WSACleanup();
}

int sendto(
    SOCKET s,
    const char FAR * buf,
    int len,
    int flags,
    const struct sockaddr FAR * to,
    int tolen
    );


int WSASendTo(
    SOCKET s,
    LPWSABUF lpBuffers,
    DWORD dwBufferCount,
    LPDWORD lpNumberOfBytesSent,
    DWORD dwFlags,
    const struct sockaddr FAR * lpTo,
    int iToLen,
    LPWSAOVERLAPPED lpOverlapped,
    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
    );


#include <winsock2.h>

void main(void)
{
    WSADATA              wsaData;
    SOCKET               SendingSocket;
    SOCKADDR_IN          ReceiverAddr;
    int                  Port = 5150;
    char                 SendBuf[1024];
    int                  BufLength = 1024;

    // Initialize Winsock version 2.2

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Create a new socket to receive datagrams on.

    SendingSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    // Set up a SOCKADDR_IN structure that will identify who we
    // will send datagrams to. For demonstration purposes, let's
    // assume our receiver's IP address is 136.149.3.29 and waits
    // for datagrams on port 5150.

    ReceiverAddr.sin_family = AF_INET;
    ReceiverAddr.sin_port = htons(Port);
    ReceiverAddr.sin_addr.s_addr = inet_addr("136.149.3.29");

    // Send a datagram to the receiver.

    sendto(SendingSocket, SendBuf, BufLength, 0,
        (SOCKADDR *)&ReceiverAddr, sizeof(RecieverAddr));

    // When your application is finished sending datagrams close
    // the socket.

    closesocket(SendingSocket);

    // When your application is finished call WSACleanup.

    WSACleanup();
}

int getpeername(
    SOCKET s,
struct sockaddr FAR* name,
    int FAR* namelen
    );

int getsockname(
    SOCKET s,
struct sockaddr FAR* name,
    int FAR* namelen
    );

int WSADuplicateSocket(
    SOCKET s,
    DWORD dwProcessId,
    LPWSAPROTOCOL_INFO lpProtocolInfo
    );