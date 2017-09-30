#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

int main(int argc, char **argv)
{
    // Declare and initialize variables
    WSADATA wsaData;
    int iResult;

    DWORD dwRetval;

    struct sockaddr_in saGNI;
    char hostname[NI_MAXHOST]; // Set to the max value
    char servInfo[NI_MAXSERV]; // Set to the max value
    // Assume we plan to use TCP port 7777
    u_short port = 7777;

    // Validate the parameters
    if (argc != 2)
    {
        printf("Usage: %s <IPv4 address>\n", argv[0]);
        printf("  This program return hostname\n");
        printf("Example: %s 127.0.0.1\n", argv[0]);
        return 1;
    }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup() failed with error code %d\n", iResult);
        return 1;
    }

    printf("WSAStartup() looks OK...\n");
    // Set up sockaddr_in structure which is passed to the getnameinfo function
    saGNI.sin_family = AF_INET; // AF_INET6
    saGNI.sin_addr.s_addr = inet_addr(argv[1]);
    saGNI.sin_port = htons(port);

    // Call getnameinfo()
    dwRetval = getnameinfo((struct sockaddr *) &saGNI,
        sizeof (struct sockaddr),
        hostname,
        NI_MAXHOST, servInfo,
        NI_MAXSERV, NI_NUMERICSERV);

    if (dwRetval != 0)
    {
        printf("getnameinfo() failed with error code %ld\n", WSAGetLastError());
    }
    else
    {
        printf("getnameinfo() returned hostname \"%s\"\n", hostname);
    }

    // Terminate the use of Winsock
    WSACleanup();
    if (dwRetval != 0)
        return 1;
    else
        return 0;
}