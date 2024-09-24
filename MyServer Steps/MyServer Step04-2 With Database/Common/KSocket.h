#pragma once
#include "KGen.h"
#include <ws2tcpip.h>
#include <vector>
#include <boost/utility.hpp>
#include "KOverlapped.h"
#include "KCriticalSection.h"


class KSocket : public noncopyable
{
    friend class KIocp;

public:
    const static DWORD  SOCKEY_NULL_KEY = 0; ///< null Io completion key.

    CONSTRUCTOR         KSocket();
    virtual             ~KSocket();

public:
    bool                Connect( const char* szIP_, unsigned short usPort_ );
    void                CloseSocket();

    bool                SendData( const char* szData, int iSize );
    bool                ReceiveData();

    void                OnIoCompleted( KOverlapped::EIoMode eMode );

    void                SetIp( const char* szIP );
    void                SetPort( u_short usPort ) { m_sockaddr.sin_port = htons( usPort ); }
    void                SetSocketInfo( SOCKET socket, SOCKADDR_IN& sockAddr ) { m_sock = socket; m_sockaddr = sockAddr; }

    virtual void        OnReceiveCompleted( DWORD dwTransfered ) = 0;
    virtual void        OnCloseSocket() = 0;
    virtual void        OnAcceptConnection() = 0;

    bool                IsConnected() const { return m_sock != INVALID_SOCKET; }
    bool                IsSending() const { return m_ovlSend.m_bSending; }
    /// @desc   set Io completion key.
    void                SetKey( DWORD dwKey ) { m_dwKey = dwKey; }
    /// @desc   get Io completion key.
    DWORD               GetKey() const { return m_dwKey; }

    /// set external Event handle.
    void                SetIoEventHandle( KOverlapped::EIoMode eMode_, HANDLE hEvent_ );
    BOOL                CancelIo() { return ::CancelIo( (HANDLE)m_sock ); }

    unsigned int        GetIp() const { return m_sockaddr.sin_addr.S_un.S_addr; }
    const char*         GetIpStr() const;
    unsigned short      GetPort() const { return ntohs(m_sockaddr.sin_port); }

private:
    void                OnSendCompleted( DWORD dwTransfered );

protected:
    DWORD               m_dwKey;
    SOCKADDR_IN         m_sockaddr;
    KOverlapped         m_ovlReceive;
    KOverlapped         m_ovlSend;
    SOCKET              m_sock;
    KCriticalSection    m_csSock;
};
