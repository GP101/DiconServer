#include <stdio.h>

#ifdef _WIN32
#  include "winsock.h"
#else
#  include <netdb.h>
#  include <arpa/inet.h>
#endif

#pragma comment( lib, "ws2_32.lib")

static void initialise(void)
{
#ifdef _WIN32
    WSADATA data;
    if (WSAStartup(MAKEWORD(1, 1), &data) != 0)
    {
        fputs("Could not initialise Winsock.\n", stderr);
        exit(1);
    }
#endif
}

static void uninitialise(void)
{
#ifdef _WIN32
    WSACleanup();
#endif
}

int main(int argc, char *argv[])
{
    struct hostent *he;

    if (argc == 1)
        return -1;

    initialise();

    he = gethostbyname(argv[1]);
    if (he == NULL)
    {
        switch (h_errno)
        {
        case HOST_NOT_FOUND:
            fputs("The host was not found.\n", stderr);
            break;
        case NO_ADDRESS:
            fputs("The name is valid but it has no address.\n", stderr);
            break;
        case NO_RECOVERY:
            fputs("A non-recoverable name server error occurred.\n", stderr);
            break;
        case TRY_AGAIN:
            fputs("The name server is temporarily unavailable.", stderr);
            break;
        }
    }
    else
    {
        puts(inet_ntoa(*((struct in_addr *) he->h_addr_list[0])));
    }

    uninitialise();

    return he != NULL;
}