#pragma once
#include "KGen.h"
#include <ws2tcpip.h>
#include <vector>
#include <boost/utility.hpp>
#include "KCriticalSection.h"


class KSocket : public boost::noncopyable
{
    static const int    MAX_PACKET_SIZE = 1024 * 16; // 16K

public:
    CONSTRUCTOR         KSocket();
    virtual             ~KSocket();

public:
    bool                Connect( const char* szIP_, unsigned short usPort_ );
    void                CloseSocket();

    bool                SendData( const char* szData, int iSize );
    bool                ReceiveData();

    void                SetIp( const char* szIP );
    void                SetPort( u_short usPort ) { m_sockaddr.sin_port = htons( usPort ); }
    void                SetSocketInfo( SOCKET socket, SOCKADDR_IN& sockAddr ) { m_sock = socket; m_sockaddr = sockAddr; }

    bool                IsConnected() const { return m_sock != INVALID_SOCKET; }

    BOOL                CancelIo() { return ::CancelIo( (HANDLE)m_sock ); }

    unsigned int        GetIp() const { return m_sockaddr.sin_addr.S_un.S_addr; }
    const char*         GetIpStr() const;
    unsigned short      GetPort() const { return ntohs(m_sockaddr.sin_port); }

protected:
    SOCKADDR_IN         m_sockaddr;

    WSABUF              m_wsaBuf;
    char                m_pBufferReceive[MAX_PACKET_SIZE];
    char                m_pBufferSend[MAX_PACKET_SIZE];

    SOCKET              m_sock;
    KCriticalSection    m_csSock;
};
