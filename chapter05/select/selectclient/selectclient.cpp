// Description:
//    This sample is the echo client. It connects to the TCP server,
//    sends data, and reads data back from the server.
//
// Command Line Options:
//    client [-p:x] [-s:IP] [-n:x] [-o]
//           -p:x      Remote port to send to
//           -s:IP     Server's IP address or hostname
//           -n:x      Number of times to send message
//           -o        Send messages only; don't receive
//
// Link to ws2_32.lib
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "ws2_32.lib")

#define DEFAULT_COUNT       20
#define DEFAULT_PORT        5150
#define DEFAULT_BUFFER      2048
#define DEFAULT_MESSAGE     "\'A test message from client\'"

char  szServer[128],                          // Server to connect to
szMessage[1024];                      // Message to send to sever
int   iPort = DEFAULT_PORT;    // Port on server to connect to
DWORD dwCount = DEFAULT_COUNT; // Number of times to send message
BOOL  bSendOnly = FALSE;                        // Send data only; don't receive

// Function: usage:
// Description: Print usage information and exit
void usage()
{
    printf("Chapter5TestClient: client [-p:x] [-s:IP] [-n:x] [-o]\n\n");
    printf("       -p:x      Remote port to send to\n");
    printf("       -s:IP     Server's IP address or hostname\n");
    printf("       -n:x      Number of times to send message\n");
    printf("       -o        Send messages only; don't receive\n");
    printf("\n");
}

// Function: ValidateArgs
// Description:
//    Parse the command line arguments, and set some global flags
//    to indicate what actions to perform
void ValidateArgs(int argc, char **argv)
{
    int    i;

    for (i = 1; i < argc; i++)
    {
        if ((argv[i][0] == '-') || (argv[i][0] == '/'))
        {
            switch (tolower(argv[i][1]))
            {
            case 'p':        // Remote port
                if (strlen(argv[i]) > 3)
                    iPort = atoi(&argv[i][3]);
                break;
            case 's':       // Server
                if (strlen(argv[i]) > 3)
                    strcpy_s(szServer, sizeof(szServer), &argv[i][3]);
                break;
            case 'n':       // Number of times to send message
                if (strlen(argv[i]) > 3)
                    dwCount = atol(&argv[i][3]);
                break;
            case 'o':       // Only send message; don't receive
                bSendOnly = TRUE;
                break;
            default:
                usage();
                break;
            }
        }
    }
}

// Function: main
// Description:
//    Main thread of execution. Initialize Winsock, parse the
//    command line arguments, create a socket, connect to the
//    server, and then send and receive data.
int main(int argc, char **argv)
{
    WSADATA       wsd;
    SOCKET        sClient;
    char          szBuffer[DEFAULT_BUFFER];
    int           ret, i;
    struct sockaddr_in server;
    struct hostent    *host = NULL;

    if (argc < 2)
    {
        usage();
        exit(1);
    }

    // Parse the command line and load Winsock
    ValidateArgs(argc, argv);
    if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
    {
        printf("Failed to load Winsock library! Error %d\n", WSAGetLastError());
        return 1;
    }
    else
        printf("Winsock library loaded successfully!\n");

    strcpy_s(szMessage, sizeof(szMessage), DEFAULT_MESSAGE);

    // Create the socket, and attempt to connect to the server
    sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sClient == INVALID_SOCKET)
    {
        printf("socket() failed with error code %d\n", WSAGetLastError());
        return 1;
    }
    else
        printf("socket() looks fine!\n");

    server.sin_family = AF_INET;
    server.sin_port = htons(iPort);
    server.sin_addr.s_addr = inet_addr(szServer);

    // If the supplied server address wasn't in the form
    // "aaa.bbb.ccc.ddd" it's a hostname, so try to resolve it
    if (server.sin_addr.s_addr == INADDR_NONE)
    {
        host = gethostbyname(szServer);
        if (host == NULL)
        {
            printf("Unable to resolve server %s\n", szServer);
            return 1;
        }
        else
            printf("The hostname resolved successfully!\n");

        CopyMemory(&server.sin_addr, host->h_addr_list[0], host->h_length);
    }

    if (connect(sClient, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
    {
        printf("connect() failed with error code %d\n", WSAGetLastError());
        return 1;
    }
    else
        printf("connect() is pretty damn fine!\n");

    // Send and receive data
    printf("Sending and receiving data if any...\n");

    for (i = 0; i < (int)dwCount; i++)
    {
        ret = send(sClient, szMessage, strlen(szMessage), 0);
        if (ret == 0)
            break;
        else if (ret == SOCKET_ERROR)
        {
            printf("send() failed with error code %d\n", WSAGetLastError());
            break;
        }

        printf("send() should be fine. Send %d bytes\n", ret);
        if (!bSendOnly)
        {
            ret = recv(sClient, szBuffer, DEFAULT_BUFFER, 0);
            if (ret == 0)        // Graceful close
            {
                printf("It is a graceful close!\n");
                break;
            }
            else if (ret == SOCKET_ERROR)
            {
                printf("recv() failed with error code %d\n", WSAGetLastError());
                break;
            }
            szBuffer[ret] = '\0';
            printf("recv() is OK. Received %d bytes: %s\n", ret, szBuffer);
        }
    }

    if (closesocket(sClient) == 0)
        printf("closesocket() is OK!\n");
    else
        printf("closesocket() failed with error code %d\n", WSAGetLastError());

    if (WSACleanup() == 0)
        printf("WSACleanup() is fine!\n");
    else
        printf("WSACleanup() failed with error code %d\n", WSAGetLastError());

    return 0;
}