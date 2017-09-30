// link with Ws2_32.lib
#include <winsock2.h>
// For addrinfo structure
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment( lib, "ws2_32.lib")

int main(int argc, char **argv)
{
    // Declare and initialize variables
    WSADATA wsaData;
    int iResult;
    DWORD dwRetval;
    int i = 1;
    char *port = "7171";

    struct addrinfo *result = NULL;
    struct addrinfo *ptr = NULL;
    struct addrinfo hints;

    // Validate the parameters
    if (argc != 2)
    {
        printf("Usage: %s <hostname>\n", argv[0]);
        printf("  getaddrinfo() provides protocol-independent translation\n");
        printf("  from an ANSI host name to an IP address\n");
        printf("Example: %s www.contoso.com\n", argv[0]);
        return 1;
    }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup() failed with error code %d\n", iResult);
        return 1;
    }
    else
        printf("WSAStartup() is OK...\n");

    // Setup the hints address info structure
    // which is passed to the getaddrinfo() function
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Call getaddrinfo(). If the call succeeds,
    // the result variable will hold a linked list
    // of addrinfo structures containing response information
    dwRetval = getaddrinfo(argv[1], port, &hints, &result);
    if (dwRetval != 0)
    {
        printf("getaddrinfo() failed with error: %d\n", dwRetval);
        WSACleanup();
        return 1;
    }

    printf("getaddrinfo() returned success!\n");

    // Retrieve each address and print out the hex bytes
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {
        printf("\ngetaddrinfo() response %d\n", i++);
        printf("\tFlags: 0x%x\n", ptr->ai_flags);

        printf("\tFamily: ");
        switch (ptr->ai_family)
        {
        case AF_UNSPEC:
            printf("Unspecified\n");
            break;
        case AF_INET:
            printf("AF_INET (IPv4)\n");
            break;
        case AF_INET6:
            printf("AF_INET6 (IPv6)\n");
            break;
        case AF_NETBIOS:
            printf("AF_NETBIOS (NetBIOS)\n");
            break;
        default:
            printf("Other %ld\n", ptr->ai_family);
            break;
        }

        printf("\tSocket type: ");

        switch (ptr->ai_socktype)
        {
        case 0:
            printf("Unspecified\n");
            break;
        case SOCK_STREAM:
            printf("SOCK_STREAM (stream)\n");
            break;
        case SOCK_DGRAM:
            printf("SOCK_DGRAM (datagram) \n");
            break;
        case SOCK_RAW:
            printf("SOCK_RAW (raw) \n");
            break;
        case SOCK_RDM:
            printf("SOCK_RDM (reliable message datagram)\n");
            break;
        case SOCK_SEQPACKET:
            printf("SOCK_SEQPACKET (pseudo-stream packet)\n");
            break;
        default:
            printf("Other %ld\n", ptr->ai_socktype);
            break;
        }

        printf("\tProtocol: ");

        switch (ptr->ai_protocol) {
        case 0:
            printf("Unspecified\n");
            break;
        case IPPROTO_TCP:
            printf("IPPROTO_TCP (TCP)\n");
            break;
        case IPPROTO_UDP:
            printf("IPPROTO_UDP (UDP) \n");
            break;
        default:
            printf("Other %ld\n", ptr->ai_protocol);
            break;
        }

        printf("\tLength of this sockaddr: %d\n", ptr->ai_addrlen);
        printf("\tCanonical name: %s\n", ptr->ai_canonname);
    }
    // Release the allocated resource
    freeaddrinfo(result);
    // WSA clean up
    WSACleanup();

    return 0;
}