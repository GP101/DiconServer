// Module Name: atalk.cpp
//
// Description:
//    This sample illustrates how to use the AppleTalk PAP and ADSP
//    protocol. This sample either acts as a sender or receiver.  
//    If acting as the receiver use the -z -t and -n options to 
//    specify the zone, type, and object name of your receiver 
//    (server).  If acting as the sender specify the -s options 
//    and the -z -t and -n options refer to the name of the 
//    AppleTalk endpoint to connect to (i.e. the receiver).
//
// Compile:
//    cl -o atalk atalk.cpp wsock32.lib
//
// Command Line Parameters/Options:
//    usage: atalk [-s] [-z Zone] [-t Type] [n Object] [-c x] [-b x] [-p str]
//       -s         Act as a sender\n");
//       -z Zone    AppleTalk zone\n");
//       -t Type    AppleTalk type name\n");
//       -n Object  AppleTalk object name\n");
//       -c x       Number of packets to send\n");
//       -b x       Buffer size to send\n");
//       -p str     Specifies protocol (PAP or ADSP)
//
#include <windows.h>
#include <winsock.h>
#include <atalkwsh.h>

#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_ZONE        "*"
#define DEFAULT_TYPE        "WindowsSockets"
#define DEFAULT_OBJECT      "AppletalkApp"

// The maximum size of a PAP message is 4096 bytes
//
#define MAX_BUFFER          4096
#define DEFAULT_COUNT       20
#define MAX_PROTO           32

BOOL bSender=FALSE;

char szZone[MAX_ENTITY],     // Zone to lookup
     szType[MAX_ENTITY],     // Type the object belongs to
     szObject[MAX_ENTITY];   // Appletalk object (name) to

DWORD dwSendSize=MAX_BUFFER, // Number of bytes to send
      dwCount=DEFAULT_COUNT; // Number of packets to send

int   iProto=ATPROTO_PAP,    // Protocol to use (PAP or ADSP)
      iSocketType=SOCK_RDM;  // Socket type of the given protocol
                             //   PAP  == SOCK_RDM while
                             //   ADSP == SOCK_STREAM

//
// Function: usage
//
// Description:
//    Print usage information and exit.
//
void usage()
{
    printf("usage: atalk [-s] [-z Zone] [-t Type] [-n Object] [-c x]\
 [-b x]\n");
    printf("       -s            Act as a sender\n");
    printf("       -z Zone       AppleTalk zone\n");
    printf("       -t Type       AppleTalk type name\n");
    printf("       -n Object     AppleTalk object name\n");
    printf("       -c x          Number of packets to send\n");
    printf("       -b x          Buffer size to send\n");
    printf("       -p [adsp|pap] Protocol to use\n");
    ExitProcess(1);
}

//
// Function: ValidateArgs
//
// Description:
//    This function parses the command line arguments and sets 
//    several global variables based upon the supplied commands.
//
void ValidateArgs(int argc, char **argv)
{
    int                i;
    
    if (argc > 1)
    {
        for (i = 1; i < argc; i++)
        {
            if ( (argv[i][0] == '-') || (argv[i][0] == '/')  )
            {
                switch (tolower(argv[i][1]) )
                {
                case '?':
                    usage();
                    break;

                case 's':   // act as a sender
                    bSender = TRUE;
                    break;

                case 'z':   // zone name
                    if(i+1 < argc)
                    {
                        strcpy(szZone,argv[i+1]);
                        ++i;
                    }
                    break;

                case 't':   // type name
                    if(i+1 < argc)
                    {
                        strcpy(szType,argv[i+1]);
                        ++i;
                    }
                    break;

                case 'n':   // object name
                    if(i+1 < argc)
                    {
                        strcpy(szObject,argv[i+1]);
                        ++i;
                    }
                    break;

                case 'c':   // number of packets to send
                    if(i+1 < argc)
                    {
                        dwCount = atoi(argv[i+1]);
                        ++i;
                    }
                    break;

                case 'b':   // size of packet to send
                    if(i+1 < argc)
                    {
                        dwSendSize = atoi(argv[i+1]);
                        if(dwSendSize > MAX_BUFFER)
                            usage();

                        ++i;
                    }
                    break;

                case 'p':   // protocol to use
                    if(i+1 < argc)
                    {
                        if(strlen(argv[i+1]) >= 3)
                        {
                            if(0 == _stricmp(argv[i+1],"adsp"))
                            {
                                iProto = ATPROTO_ADSP;
                                iSocketType = SOCK_STREAM;
                            } else if (0 == _stricmp(argv[i+1],"pap"))
                            {
                                iProto = ATPROTO_PAP;
                                iSocketType = SOCK_RDM;
                            }
                        }
                        ++i;
                    }
                    break;

                default:
                    usage();
                    break;
                
                } 
            }
        } 

    }
    
    return;
}

//
// Function: RegisterName
//
// Description:
//    Register an AppleTalk name on the network.
//
int RegisterName(SOCKET s, char *zone, char *type, char *obj)
{
    WSH_NBP_NAME    nbpname;
    int             ret;

    // Fill out the WSH_NBP_NAME structure
    //
    ZeroMemory(&nbpname, sizeof(nbpname));
    lstrcpy(nbpname.ZoneName, zone);
    nbpname.ZoneNameLen = strlen(zone);
    lstrcpy(nbpname.TypeName, type);
    nbpname.TypeNameLen = strlen(type);
    lstrcpy(nbpname.ObjectName, obj);
    nbpname.ObjectNameLen = strlen(obj);

    printf("Registering: %s:%s@%s\n", obj, type, zone);

    ret = setsockopt(s, SOL_APPLETALK, SO_REGISTER_NAME,
                (char *)&nbpname, sizeof(nbpname));
    if (ret == SOCKET_ERROR)
    {
        printf("setsockopt(SO_REGISTER_NAME) failed: %d\n",
            WSAGetLastError());
        return 1;
    }
    return 0;
}

//
// Function: FindName
//
// Description:
//    Lookup a given name on the AppleTalk network so that we can
//    get the network, node, and socket number associated with it
//    in order to make a connection.
//
int FindName(SOCKET s, char *zone, char *type, char *obj, 
             SOCKADDR_AT *addr)
{
    char              szLookup[MAX_BUFFER],
                     *lpLookup;
    PWSH_LOOKUP_NAME  lpLookupName=NULL;
    int               size = MAX_BUFFER,
                      ret;
    PWSH_NBP_TUPLE    lpTuple;

    lpLookupName = (PWSH_LOOKUP_NAME)szLookup;
    //
    // Fill in our WSH_LOOKUP_NAME structure with the name to find
    //
    strcpy(lpLookupName->LookupTuple.NbpName.ObjectName, obj);
    lpLookupName->LookupTuple.NbpName.ObjectNameLen = strlen(obj);
    strcpy(lpLookupName->LookupTuple.NbpName.TypeName, type);
    lpLookupName->LookupTuple.NbpName.TypeNameLen = strlen(type);
    strcpy(lpLookupName->LookupTuple.NbpName.ZoneName, zone);
    lpLookupName->LookupTuple.NbpName.ZoneNameLen = strlen(zone);

    ret = getsockopt(s, SOL_APPLETALK, SO_LOOKUP_NAME, 
                (char *)szLookup, &size);
    if (ret == SOCKET_ERROR)
    {
        printf("getsockopt() failed; %d\n", WSAGetLastError());
        return 1;
    }
    lpLookup = (char *)szLookup + sizeof(WSH_LOOKUP_NAME);
    lpTuple  = (PWSH_NBP_TUPLE)lpLookup;

printf("Found %d names!\n", lpLookupName->NoTuples);

    //
    // Note that we only return the first name returned
    // 
    addr->sat_family = AF_APPLETALK;
    addr->sat_net = lpTuple[0].Address.Network;
    addr->sat_node = lpTuple[0].Address.Node;
    addr->sat_socket = lpTuple[0].Address.Socket;

    printf("addr->sat_family = AF_APPLETALK\n");
    printf("addr->sat_net    = %d\n", addr->sat_net);
    printf("addr->sat_node   = %d\n", addr->sat_node);
    printf("addr->sat_socket = %d\n", addr->sat_socket);

    return 0;
}

//
// Function: atalkrecv
// 
// Description:
//    On a socket read as much data as there is pending. For the PAP
//    protocol this means reading until WSARecvEx() returns without
//    the MSG_PARTIAL flag set. For ADSP we simply request to read
//    the maximum buffer size. If an error occurs we return the 
//    Winsock error; otherwise a zero is returned. For ADSP we are
//    assuming that the message from can be read on the first
//    attempt. 
//
int atalkrecv(SOCKET s, char *recvbuff, int *nRead)
{
    int        iFlags=0,
               ret;
    DWORD      dwErr;
    BOOL       bDone=FALSE;
    struct fd_set fdread;

    *nRead = 0;
    dwErr = 0;

    if (iProto == ATPROTO_PAP)
    {
        // For PAP we have to do a prime read first
        //
        ret = setsockopt(s, SOL_APPLETALK, SO_PAP_PRIME_READ,
                 recvbuff, MAX_BUFFER);
        if (ret == SOCKET_ERROR)
        {
            printf("setsockopt() failed: %d\n", 
                (dwErr = WSAGetLastError()));
            return dwErr;
        }
    }

    FD_ZERO(&fdread);
    FD_SET(s, &fdread);
    select(0, &fdread, NULL, NULL, NULL);

    while (!bDone)
    {
        // Make sure we receive the whole message (PAP only)
        //  For the ADSP protocol, WSARecvEx() will never
        //  return the MSG_PARTIAL flag.
        //
        if ((ret = WSARecvEx(s, &recvbuff[(*nRead)], 
            MAX_BUFFER, &iFlags)) == SOCKET_ERROR)
        {
            if ((dwErr = WSAGetLastError()) == WSAEWOULDBLOCK)
                break;
            else if (dwErr == WSAENOTCONN)
                break;
            printf("WSARecvEx() failed: %d\n", dwErr);
            return dwErr;
        }
        if (ret == 0)
        {
            printf("graceful close\n");
            break;
        }
        if ((iFlags & MSG_PARTIAL) == 0)
            bDone = TRUE;
        (*nRead) += ret;
    }
    return dwErr;
}

//
// Function: atalksend
//
// Description:
//    Send the specified number of bytes from the buffer on the 
//    given socket. Nothing special needs to be done for either
//    ADSP or PAP. For PAP, the message should only require one
//    send. For ADSP, it could require several send() calls as
//    it is stream oriented.
// 
int atalksend(SOCKET s, char *sendbuf, int nSend)
{
    DWORD        dwErr;
    int          ret,
                 nPos;
    struct fd_set fdwrite;

    dwErr = 0;
    FD_ZERO(&fdwrite);
    FD_SET(s, &fdwrite);
    select(0, NULL, &fdwrite, NULL, NULL);

    nPos = 0;
    while (nSend > 0)
    {
        if ((ret = send(s, &sendbuf[nPos], nSend, 0)) == SOCKET_ERROR)
        {
            if ((dwErr = WSAGetLastError()) != WSAEWOULDBLOCK)
            {
                printf("send() failed; %d\n", dwErr);
            }
            return dwErr;
        }
        nSend -= ret;
        nPos  += ret;
    }
    return dwErr;
}

//
// Function: ClientThread
//
// Description:
//    A desc goes here.
//
DWORD WINAPI ClientThread(LPVOID lpParam)
{
    SOCKET        s = (SOCKET)lpParam;
    DWORD         i;
    char          recvbuff[MAX_BUFFER];
    int           nRead=0;
    u_long        optval=1;
    int           ret;

    // Make the socket non-blocking, error notification is easier
    //  this way.
    //
    if (ioctlsocket(s, FIONBIO, &optval) == SOCKET_ERROR)
    {
        printf("ioctlsocket(FIONBIO) failed; %d\n", 
            WSAGetLastError());
        return 1;
    }
    // For PAP, receive and send the requested number of packets.
    // For ADSP, receive and send until the client closed the
    //  connection.
    //
    i = 0;
    do {
        if ((ret = atalkrecv(s, recvbuff, &nRead)) != 0)
        { 
            if (ret == WSAEWOULDBLOCK)
            {
                printf("huh?\n");
                continue;
            }
            else if (ret == WSAENOTCONN)
                break;
            else
            {
                printf("recv() failed: %d\n", ret);
                break;
            }
        }
        i++; 

        if (nRead > 0)
            printf("read %d bytes\n", nRead);

        if ((ret = atalksend(s, recvbuff, nRead)) != 0)
        {
            if (ret == WSAEWOULDBLOCK)
            {
                printf("duh?\n");
                continue;
            }
            else
            {
                printf("send() failed: %d\n", ret);
                break;
            }
        }
        printf("wrote %d bytes\n", nRead);
    } while (((i <= dwCount) && (iProto == ATPROTO_PAP)) ||
             (iProto == ATPROTO_ADSP));
    return 0;
}

//
// Function: main
//
// Description:
//    Load the Winsock library, parse the command line arguments, 
//    create a socket of the requested type, and either send or
//    receive data. For the PAP protocol make sure we receive
//    an entire message by checking for the MSG_PARTIAL flag on
//    return from WSARecvEx().  For ADSP, the receiver will 
//    continually loop until the connection is closed (as it is
//    a stream protocol).
//
int main(int argc, char **argv)
{
    WSADATA       wsd;
    SOCKET        s;
    SOCKADDR_AT   ataddr;
    int           ret,
                  nRead,
                  ataddrsz=sizeof(ataddr);

    strcpy(szZone, DEFAULT_ZONE);
    strcpy(szType, DEFAULT_TYPE);
    strcpy(szObject, DEFAULT_OBJECT);
    //
    // Get things started by validating command line args,
    //  loading the Winsock lib, and creating a socket of
    //  the requested type.
    //
    ValidateArgs(argc, argv);
    if (WSAStartup(MAKEWORD(1,1), &wsd) != 0)
    {
        printf("WSAStartup failed!\n");
        return 1;
    }

    s = socket(AF_APPLETALK, iSocketType, iProto);
    if (s == INVALID_SOCKET)
    {
        printf("socket() failed: %d\n", WSAGetLastError());
        return 1;
    }
    // Open a port on the AppleTalk network
    //
    ZeroMemory(&ataddr, sizeof(ataddr));
    ataddr.sat_family = AF_APPLETALK;
    ataddr.sat_socket = 0;
    if (bind(s, (SOCKADDR *)&ataddr, sizeof(ataddr)) == INVALID_SOCKET)
    {
        printf("bind() failed: %d\n", WSAGetLastError());
        return 1;
    }

    if (bSender)                // we are sending packets
    {
        SOCKADDR_AT   ataddr;
        char          sendbuff[MAX_BUFFER];
        DWORD         i;
        u_long        optval=1;

        // Find the given name on the AppleTalk network so we can 
        //  connect and send data to it!
        //
        if (FindName(s, szZone, szType, szObject, &ataddr) != 0)
        {
            printf("Unable to find receiver!\n");
            return 1;
        }
        // If we found a name, try to connect to it
        //
        printf("Connecting to: %s:%s@%s\n", szObject, szType, szZone);
        if (connect(s, (SOCKADDR *)&ataddr, 
                sizeof(ataddr)) == SOCKET_ERROR)
        {
            printf("connect() failed: %d\n", WSAGetLastError());
            return 1;
        }

        if (ioctlsocket(s, FIONBIO, &optval) == SOCKET_ERROR)
        {
            printf("ioctlsocket(FIONBIO) failed: %d\n",
                WSAGetLastError());
            return 1;
        }
        // Once we've connected, start sending data. Send the
        //  requested number of packets of the given size.
        //
        memset(sendbuff, 'a', dwSendSize);
        i=0;
        do {
            if ((ret = atalksend(s, sendbuff, dwSendSize)) != 0)
            {
                if (ret == WSAEWOULDBLOCK)
                {
                    printf("Client would block send\n");
                    continue;
                }
                else
                {
                    printf("send() failed: %d\n", ret);
                    break;
                }
            }
            i++;

            printf("Wrote %d bytes\n", dwSendSize);
        
            if ((ret = atalkrecv(s, sendbuff, &nRead)) != 0)
            {
                if (ret == WSAEWOULDBLOCK)
                {
                    printf("Client would block recv\n");
                    continue;
                }
                else
                {
                    printf("recv() failed: %d\n", ret);
                    break;
                }
            }
            printf("Read %d bytes\n", nRead);
        } while (i < dwCount);
    }
    else                        // we're receiving data
    {
        SOCKET        scli;
        BOOL          bDone=FALSE;
        HANDLE        hThread;
        DWORD         dwThreadId;
        struct fd_set fdread;

        // Register our name so others can connect to it
        //
        if (RegisterName(s, szZone, szType, szObject) != 0)
            return 1;

        listen(s, 8);

        while (!bDone)
        {
            FD_ZERO(&fdread);
            FD_SET(s, &fdread);
            select(0, &fdread, NULL, NULL, NULL);
            //
            // Once we receive a connection accept it
            //
            scli = accept(s, (SOCKADDR *)&ataddr, &ataddrsz);
            if (scli == INVALID_SOCKET)
            {
                printf("accept() failed: %d\n", WSAGetLastError());
                bDone = TRUE;
            }
            hThread = CreateThread(NULL, 0, ClientThread, (LPVOID)scli,
                        0, &dwThreadId);
            if (hThread == NULL)
            {
                printf("CreateThread() failed: %d\n", GetLastError());
                bDone = TRUE;
            }
            CloseHandle(hThread);
        }
    }
    closesocket(s);
    WSACleanup();

    return 0;
}
