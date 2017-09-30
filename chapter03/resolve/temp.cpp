struct sockaddr_in6 {
    short                sin6_family;
    u_short                       sin6_port;
    u_long             sin6_flowinfo;
    struct in6_addr sin6_addr;
    u_long             sin6_scope_id;
};


int getaddrinfo(
    const char FAR *nodename,
    const char FAR *servname,
    const struct addrinfo FAR *hints,
    struct addrinfo FAR *FAR *res
    );

struct addrinfo {
    int                    ai_flags;
    int                    ai_family;
    int                    ai_socktype;
    int                    ai_protocol;
    size_t              ai_addrlen;
    char                 *ai_canonname;
    struct sockaddr *ai_addr;
    struct addrinfo *ai_next;
};

// Declare and initialize variables.
char* ip = "127.0.0.1";
char* port = "7777";
struct addrinfo aiHints;
struct addrinfo *aiList = NULL;
int retVal;

// Setup the hints address info structure
// which is passed to the getaddrinfo() function
memset(&aiHints, 0, sizeof(aiHints));
aiHints.ai_family = AF_INET;
aiHints.ai_socktype = SOCK_STREAM;
aiHints.ai_protocol = IPPROTO_TCP;

// Call getaddrinfo(). If the call succeeds, the aiList variable
// will hold a linked list f addrinfo structures containing
// response information about the host
if ((retVal = getaddrinfo(ip, port, &aiHints, &aiList)) != 0)
{
    printf("getaddrinfo() failed with error code %d.\n", retVal);
}

int getnameinfo(
    const struct sockaddr FAR *sa,
    socklen_t salen,
    char FAR *host,
    DWORD hostlen,
    char FAR *serv,
    DWORD servlen,
    int flags
    );

INT WSAStringToAddress(
    LPTSTR AddressString,
    INT AddressFamily,
    LPWSAPROTOCOL_INFO lpProtocolInfo,
    LPSOCKADDR lpAddress,
    LPINT lpAddressLength
    );

struct servent FAR * getservbyname(
    const char FAR * name,
    const char FAR * proto
    );

HANDLE WSAAsyncGetServByName(
    HWND hWnd,
    unsigned int wMsg,
    const char FAR *name,
    const char FAR *proto,
    char FAR *buf,
    int buflen
    );

struct servent FAR *getservbyport(
    int port,
    const char FAR *proto
    );

HANDLE WSAAsyncGetServByPort(
    HWND hWnd,
    unsigned int wMsg,
    int port,
    const char FAR *proto,
    char FAR *buf,
    int buflen
    );

struct servent {
    char FAR *        s_name;
    char FAR * FAR *s_aliases;
    short                   s_port;
    char FAR *        s_proto
};
