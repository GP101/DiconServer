//
// Sample: Illustrates getaddrinfo and getnameinfo
//
// Files:
//      resolver.cpp        - this file
//
// Description:
//      This sample illustrates how to use the new name resolution APIs:
//      getaddrinfo and getnameinfo. This sample takes a hostname or address
//      and performs an address resolution via getaddrinfo. This lookup will
//      return one or more binary addresses which will then be passed to
//      getnameinfo to perform a reverse lookup. The results of each query
//      are displayed to the console.
//
// Complile:
//      cl.exe -o resolver.exe resolver.cpp ws2_32.lib
//
// Usage:
//   resolver.exe [options]
//      -n name            Host name to resolve
//      -s port/service    Service name or port number to resolve
//      -a address-family  Preferred address family
//         4                   IPv4 (AF_INET)
//         6                   IPv6 (AF_INET6)
//      -t socket-type     Preferred socket type
//         stream              Stream socket (SOCK_STREAM)
//         dgram               Datagram socket (SOCK_DGRAM)
//      -p protocol        Preferred protocol
//         tcp                 TCP (IPPROTO_TCP)
//         udp                 UDP (IPPROTO_UDP)
//      -f flag            Flags passed to getname info (options may be present multiple times)
//         nofqdn               NI_NOFQDN
//         numerichost          NI_NUMERICHOST
//         namereqd             NI_NAMEREQD
//         numericserv          NI_NUMERICSERV
//         dgram                NI_DGRAM
//      -b                 Obtain the bind address for the given address family (i.e. AI_PASSIVE)
//

#include <winsock2.h>
#include <ws2tcpip.h>

// If running on Win9x or pre Windows XP uncomment the following
//#include <wspiapi.h>

#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "ws2_32.lib")

//
// Function: usage
//
// Description:
//    Prints usage information for application
//
void usage(char *progname)
{
    fprintf(stderr, "usage: %s [-n name] [-s port/service] [-a 4|6] [-t s|u] [-p tcp|udp] [-b]\n", progname);
    fprintf(stderr, "   -n name            Host name to resolve\n");
    fprintf(stderr, "   -s port/service    Service name or port number to resolve\n");
    fprintf(stderr, "   -a address-family  Preferred address family\n"
                    "      4                   IPv4 (AF_INET)\n"
                    "      6                   IPv6 (AF_INET6)\n");
    fprintf(stderr, "   -t socket-type     Preferred socket type\n"
                    "      stream              Stream socket (SOCK_STREAM)\n"
                    "      dgram               Datagram socket (SOCK_DGRAM)\n");
    fprintf(stderr, "   -p protocol        Preferred protocol\n"
                    "      tcp                 TCP (IPPROTO_TCP)\n"
                    "      udp                 UDP (IPPROTO_UDP)\n");
    fprintf(stderr, "   -f flag            Flags passed to getname info (may appear multiple times)\n"
                    "      nofqdn               NI_NOFQDN\n"
                    "      numerichost          NI_NUMERICHOST\n"
                    "      namereqd             NI_NAMEREQD\n"
                    "      numericserv          NI_NUMERICSERV\n"
                    "      dgram                NI_DGRAM\n");
    fprintf(stderr, "   -b                 Obtain the bind address for the given address family\n");
           
    ExitProcess(-1);
}

//
// Function: PrintParameters
//
// Description:
//    Prints the parameters passed into the application before attempting to
//    resolve the requested name and service.
//
void PrintParameters(char *host, char *serv, int af, int type, int proto, int flags)
{
    printf("\n");
    printf("Attempting to resolve the host: %s\n", (host ? host: "NULL"));
    printf("Service/port information is   : %s\n", (serv ? serv: "NULL"));
    if (af == AF_INET)
        printf("Requested address family      : AF_INET\n");
    else if (af == AF_INET6)
        printf("Requested address family      : AF_INET6\n");
    else
        printf("Requested address family      : NONE (default is AF_UNSPEC)\n");
    if (type == SOCK_STREAM)
        printf("Requested socket type         : SOCK_STREAM\n");
    else if (type == SOCK_DGRAM)
        printf("Requested socket type         : SOCK_DGRAM\n");
    else
        printf("Requested socket type         : NONE\n");
    if (proto == IPPROTO_TCP)
        printf("Requested protocol            : IPPROTO_TCP\n");
    else if (proto == IPPROTO_UDP)
        printf("Requested protocol            : IPPROTO_UDP\n");
    else
        printf("Requested protocol            : NONE\n");
    printf("Requested flags to getnameinfo: ");
    if (flags & NI_NOFQDN)
        printf("NI_NOFQDN ");
    if (flags & NI_NUMERICHOST)
        printf("NI_NUMERICHOST ");
    if (flags & NI_NAMEREQD)
        printf("NI_NAMEREQD ");
    if (flags & NI_NUMERICSERV)
        printf("NI_NUMERICSERV ");
    if (flags & NI_DGRAM)
        printf("NI_DGRAM ");
    printf("\n\n");
}

//
// Function: main
//
// Description:
//    This routine takes the host and service information obtained from
//    the command line and calls getaddrinfo. The first getaddrinfo call
//    assumes that a numeric string address was passed. If that call fails
//    then it will attempt to resolve the name via DNS. After getaddrinfo
//    succeeds, then for each address returned we will call getnameinfo
//    to resolve it back to its hostname along with displaying the numeric
//    string address.
//
int __cdecl main(int argc, char **argv)
{
    WSADATA          wsd;
    struct addrinfo  hints,
                    *res=NULL,
                    *ptr;
    int              rc,
                     count,
                     i;

    // Variables for getnameinfo
    char             host[NI_MAXHOST],
                     serv[NI_MAXSERV],
                     numerichost[NI_MAXHOST];
    int              hostlen=NI_MAXHOST,
                     servlen=NI_MAXSERV,
                     numerichostlen=NI_MAXHOST,
                     flags=0;

    // Variables for getaddrinfo
    int              AddressFamily=AF_UNSPEC,
                     SocketType=0,
                     Protocol=0;
    char            *HostName=NULL,
                    *ServPort=NULL;
    BOOL             bGetBindAddress=FALSE;

    // Load Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
    {
        fprintf(stderr, "Unable to load Winsock: %d\n", GetLastError());
        return -1;
    }
    //
    // Parse the command line arguments
    //
    for(i=1; i < argc ;i++)
    {
        if ((argv[i][0] != '-') && (argv[i][0] != '/') && (strlen(argv[i]) < 2))
            usage(argv[0]);
        switch (tolower(argv[i][1]))
        {
            case 'a':       // address family
                if (i+1 >= argc)
                    usage(argv[0]);
                if (atoi(argv[i+1]) == 4)
                    AddressFamily = AF_INET;
                else if (atoi(argv[i+1]) == 6)
                    AddressFamily = AF_INET6;
                else
                    usage(argv[0]);
                i++;
                break;
            case 'b':       // get the bind address only
                bGetBindAddress = TRUE;
                break;
            case 'f':       // flags for getnameinfo
                if (i+1 >= argc)
                    usage(argv[0]);
                if (_strnicmp(argv[i+1], "nofqdn", 6) == 0)
                    flags |= NI_NOFQDN;
                else if (_strnicmp(argv[i+1], "numerichost", 11) == 0)
                    flags |= NI_NUMERICHOST;
                else if (_strnicmp(argv[i+1], "namereqd", 8) == 0)
                    flags |= NI_NAMEREQD;
                else if (_strnicmp(argv[i+1], "numericserv", 11) == 0)
                    flags |= NI_NUMERICSERV;
                else if (_strnicmp(argv[i+1], "dgram", 5) == 0)
                    flags |= NI_DGRAM;
                else
                    usage(argv[0]);
                i++;
                break;
            case 'n':       // name to resolve
                if (i+1 >= argc)
                    usage(argv[0]);
                HostName = argv[++i];
                break;
            case 'p':       // protocol to resove for
                if (i+1 >= argc)
                    usage(argv[0]);
                if (_strnicmp(argv[i+1], "tcp", 3) == 0)
                    Protocol = IPPROTO_TCP;
                else if (_strnicmp(argv[i+1], "udp", 3) == 0)
                    Protocol = IPPROTO_UDP;
                else
                    usage(argv[0]);
                i++;
                break;
            case 's':       // port/service to resolve
                if (i+1 >= argc)
                    usage(argv[0]);
                ServPort = argv[++i];
                break;
            case 't':       // socket type
                if (i+1 >= argc)
                    usage(argv[0]);
                if (_strnicmp(argv[i+1], "stream", 6) == 0)
                    SocketType = SOCK_STREAM;
                else if (_strnicmp(argv[i+1], "dgram", 5) == 0)
                    SocketType = SOCK_DGRAM;
                else
                    usage(argv[0]);
                i++;
                break;
            default:
                usage(argv[0]);
                break;
        }
    }
    //
    // Check for the minumum parameters required
    //
    if (!HostName && !ServPort)
    {
        fprintf(stderr, "\n\nYou must specify at least a host name or service name\n\n");
        usage(argv[0]);
    }

    if ((flags & NI_NUMERICSERV) == 0)
    {
        printf("\nNOTE: Without the NI_NUMERICSERV flag (-f numericserv) getnameinfo will\n"
               "      fail if the port information does not resolve to a well known service\n"
               "      like ftp (21), etc.\n\n");
    }

    PrintParameters(HostName, ServPort, AddressFamily, SocketType, Protocol, flags);

    //
    // Resolve the name/address - first assume that the name might be a string
    // literal address
    //
    memset(&hints, 0, sizeof(hints));
    if (bGetBindAddress == TRUE)
        hints.ai_flags = AI_PASSIVE;
    else
        hints.ai_flags = AI_NUMERICHOST;
    hints.ai_family    = AddressFamily;
    hints.ai_socktype  = SocketType;
    hints.ai_protocol  = Protocol;

    rc = getaddrinfo(
            HostName,
            ServPort,
           &hints,
           &res
            );
    if (rc != 0)
    {
        if (rc == WSAHOST_NOT_FOUND)
        {
            //
            // Host name to resolve isn't a string literal, now try a DNS lookup
            // The numerichost lookup is done first since it doesn't actually 
            //  send a request on the network and will fail quickly if it isn't a
            //  numeric address.
            //
            hints.ai_flags = AI_CANONNAME;
            rc = getaddrinfo(
                    HostName,
                    ServPort,
                   &hints,
                   &res
                    );
            if (rc != 0)
            {
                fprintf(stderr, "getaddrinfo failed: %d\n", rc);
                return -1;
            }
        }
        else
        {
            fprintf(stderr, "getaddrinfo failed: %d\n", rc);
            return -1;
        }
    }

    //
    // Count how many addresses were returned
    //
    count = 0;
    ptr   = res;
    while (ptr)
    {
        count++;
        ptr = ptr->ai_next;
    }

    printf("Hostname '%s' resolved to %d addresses\n\n", HostName, count);

    //
    //Do a reverse lookup on each of the resolved address
    //
    count = 1;
    ptr   = res;
    while (ptr)
    {
        host[0] = '\0';
        serv[0] = '\0';

        //
        // First convert the binary addresses returned from getaddrinfo into
        // their numeric string form so we can see the address returned.
        //
        rc = getnameinfo(
                ptr->ai_addr,
                ptr->ai_addrlen,
                numerichost,
                numerichostlen,
                NULL,
                0,
                NI_NUMERICHOST
                );
        if (rc != 0)
        {
            fprintf(stderr, "getnameinfo failed: %d\n", rc);
            return -1;
        }

        printf("[%d] Numeric address resolved: %s\n", count, numerichost);
        
        //
        // Now attemp to reverse lookup according the parameters passed in
        // by the user
        //
        rc = getnameinfo(
                ptr->ai_addr,
                ptr->ai_addrlen,
                host,
                hostlen,
                serv,
                servlen,
                flags
                );
        if (rc != 0)
        {
            fprintf(stderr, "getnameinfo failed: %d for the address: %s\n", rc, numerichost);
        }
        else
        {
            printf("[%d] Host:    %s\n", count, host);
            printf("[%d] Service: %s\n", count, serv);
        }

        printf("\n");

        count++;

        // Goto the next address
        ptr = ptr->ai_next;
    }

    // Free up the results from getaddrinfo
    freeaddrinfo(res);

    WSACleanup();
    
    return 0;
}
