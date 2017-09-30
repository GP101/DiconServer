// Nagle Algorithm
if there is new data to send
    if the window size >= MSS and available data is >= MSS
        send complete MSS segment now
    else
        if there is unconfirmed data still in the pipe
            enqueue data in the buffer until an acknowledge is received
        else
            send data immediately
        end if
    end if
end if

int WSAEnumProtocols(
    LPINT lpiProtocols,
    LPWSAPROTOCOL_INFO lpProtocolBuffer,
    LPDWORD lpdwBufferLength
);

typedef struct _WSAPROTOCOL_INFO {
    DWORD             dwServiceFlags1;
    DWORD             dwServiceFlags2;
    DWORD             dwServiceFlags3;
    DWORD             dwServiceFlags4;
    DWORD             dwProviderFlags;
    GUID              ProviderId;
    DWORD             dwCatalogEntryId;
    WSAPROTOCOLCHAIN  ProtocolChain;
    int               iVersion;
    int               iAddressFamily;
    int               iMaxSockAddr;
    int               iMinSockAddr;
    int               iSocketType;
    int               iProtocol;
    int               iProtocolMaxOffset;
    int               iNetworkByteOrder;
    int               iSecurityScheme;
    DWORD             dwMessageSize;
    DWORD             dwProviderReserved;
    TCHAR             szProtocol[WSAPROTOCOL_LEN + 1];
} WSAPROTOCOL_INFO, FAR * LPWSAPROTOCOL_INFO;