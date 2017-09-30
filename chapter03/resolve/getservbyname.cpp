#include <winsock2.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib,"ws2_32.lib")

int main(int argc, char **argv)
{
    // Declare and initialize variables
    WSADATA wsaData;
    char node_name[100] = "";
    struct hostent *hostptr;
    struct servent *servptr;
    // http://www.iana.org/assignments/port-numbers
    char services[12][10] = { "http", "ftp", "ssh", "telnet", "imap4", "smtp", "gopher", "finger", "pop3", "tftp", "mysql" };
    int i;

    // Validate the parameters
    if (argc != 2)
    {
        printf("Usage: %s <host_name>\n", argv[0]);
        printf("Example: %s www.yahoo.com\n", argv[0]);
        // Just return with error
        return 1;
    }

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("WSAStartup() failed with error code %ld\n", WSAGetLastError());
        return 1;
    }
    else
        printf("WSAStartup() looks fine!\n");

    strcpy_s(node_name, sizeof(node_name), argv[1]);
    // Here, name of server computer is expected, better to add
    // the server IP address functionality...
    hostptr = gethostbyname(node_name);

    if (hostptr == NULL)
    {
        printf("gethostbyname() failed with error code %ld\n", WSAGetLastError());
    }
    else
        printf("gethostbyname() is OK.\n Host name is %s\n", hostptr->h_name);

    for (i = 0; i <= 10; i++)
    {
        // The proto pointer is NULL, the getservbyname() will returns
        // the first service entry where name matches the s_name member
        // of the servent struct or the s_aliases member of the servent struct.
        servptr = getservbyname(services[i], NULL);

        if (servptr == NULL)
            printf("\ngetservbyname() - %s failed with error code %ld\n", services[i], WSAGetLastError());
        else
        {
            // Print the related info
            printf("\ngetservbyname() is OK - %s\n", services[i]);
            printf(" Service name is %s\n", servptr->s_name);
            // Need to cast to host-byte order
            printf(" Port number is %u\n", ntohs(servptr->s_port));
            printf(" Protocol is %s\n", servptr->s_proto);
        }
    }
    WSACleanup();
    return 0;
}