#include "KSocket.h"
#include "KCriticalSection.h"
#include "KIocp.h"


CONSTRUCTOR KSocket::KSocket()
    : m_sock( INVALID_SOCKET )
    , m_dwKey( 0 )
{
    m_sockaddr.sin_family = AF_INET;

    m_ovlReceive.m_eIoMode = KOverlapped::IO_RECEIVE;
    m_ovlSend.m_eIoMode = KOverlapped::IO_SEND;

    m_ovlReceive.hEvent = NULL;
    m_ovlSend.hEvent = NULL;
}

void KSocket::SetIoEventHandle( KOverlapped::EIoMode eMode_, HANDLE hEvent_ )
{
    //// Event must be destroyed after thread termination.
    //// So, Event is managed inside the thread class.

    switch( eMode_ ) {
    case KOverlapped::IO_SEND:
        m_ovlSend.hEvent = hEvent_;
        break;

    case KOverlapped::IO_RECEIVE:
        m_ovlReceive.hEvent = hEvent_;
        break;
    }
}

DESTRUCTOR KSocket::~KSocket()
{
    CloseSocket();
}

void KSocket::CloseSocket()
{
    CSLOCK( m_csSock )
    {
        if( m_sock != INVALID_SOCKET ) {
            ::shutdown( m_sock, SD_BOTH );
            ::closesocket( m_sock );
            m_sock = INVALID_SOCKET;
        }
    }
}

bool KSocket::ReceiveData()
{
    KCriticalSectionLock lock( m_csSock );

    if( m_sock == INVALID_SOCKET )
        return false;

    DWORD dwRead = 0;
    DWORD dwFlag = 0;

    m_ovlReceive.Clear();
    m_ovlReceive.m_wsaBuffer.buf = &m_ovlReceive.m_pBuffer[m_ovlReceive.m_dwRemained];
    m_ovlReceive.m_wsaBuffer.len = MAX_PACKET_SIZE - m_ovlReceive.m_dwRemained;

    int ret = ::WSARecv( m_sock     // socket
        , &m_ovlReceive.m_wsaBuffer // buffer pointer, size
        , 1                         // number of WSABUF structure
        , &dwRead                   // numer of bytes after Io.
        , &dwFlag                   // [in,out] Option Flag
        , &m_ovlReceive             // struct LPWSAOVERLAPPED
        , NULL );                   // callback when Io completed.

    if( SOCKET_ERROR == ret ) {
        switch( ::GetLastError() ) {
        case WSA_IO_PENDING: // the overlapped operation has been successfully initiated            
            return true;

        case WSAECONNRESET: // the virtual circuit was reset by the remote side.
            BEGIN_LOG( cwarn, L"The virtual circuit was reset by the remote side." );
            VIRTUAL OnCloseSocket();
            return false;

        default:
            BEGIN_LOG( cerr, L"WSARecv() Failed." )
                << LOG_NAMEVALUE( m_dwKey )
                << LOG_NAMEVALUE( ret );
            VIRTUAL OnCloseSocket();
            return false;
        }
    }

    return ret == 0;
}

void KSocket::OnIoCompleted( KOverlapped::EIoMode eMode_ )
{
    DWORD dwTransfered = 0;
    DWORD dwFlag = 0;
    LPWSAOVERLAPPED povl = NULL;

    switch( eMode_ ) {
    case KOverlapped::IO_SEND:
        povl = &m_ovlSend;
        break;
    case KOverlapped::IO_RECEIVE:
        povl = &m_ovlReceive;
        break;
    default:
        return;
    }

    CSLOCK( m_csSock )
    {
        ::WSAGetOverlappedResult( m_sock, povl, &dwTransfered, FALSE, &dwFlag );
    }

    if( eMode_ == KOverlapped::IO_SEND )
        OnSendCompleted( dwTransfered );
    else
        VIRTUAL OnReceiveCompleted( dwTransfered );
}

void KSocket::OnSendCompleted( DWORD dwTransfered_ )
{
    KCriticalSectionLock lock( m_csSock );

    if( m_sock == INVALID_SOCKET )
        return;

    m_ovlSend.m_bSending = false;

    if( dwTransfered_ >= m_ovlSend.m_dwRemained ) {// sent all requested data
        m_ovlSend.m_dwRemained = 0;
        return;
    }

    int ret = SOCKET_ERROR;

    // there is remained data, send again
    m_ovlSend.m_dwRemained -= m_ovlSend.InternalHigh;
    memmove( &m_ovlSend.m_pBuffer[0], &m_ovlSend.m_pBuffer[dwTransfered_], m_ovlSend.m_dwRemained );

    DWORD dwWrite = 0;
    m_ovlSend.m_wsaBuffer.len = m_ovlSend.m_dwRemained;

    ret = ::WSASend( m_sock, &m_ovlSend.m_wsaBuffer, 1, &dwWrite, 0, &m_ovlSend, NULL );

    // Io is pending
    if( ret == SOCKET_ERROR && ::WSAGetLastError() == ERROR_IO_PENDING ) {
        m_ovlSend.m_bSending = true;
        return;
    }

    // all data was sent after WSASend().
    if( ret != SOCKET_ERROR ) {
        m_ovlSend.m_bSending = true;
        return;
    }

    BEGIN_LOG( cwarn, L"WSASend Failed. LastError : " )
        << END_LOG;

    OnCloseSocket();
}

void KSocket::SetIp( const char* szIP )
{
    //m_sockaddr.sin_addr.s_addr = inet_addr(szIP);
    m_sockaddr.sin_addr.s_addr = inet_pton(AF_INET, szIP, nullptr);
}

const char* KSocket::GetIpStr() const
{
    //return inet_ntoa(m_sockaddr.sin_addr);
    char buffer[80];
    return inet_ntop(AF_INET, (void*)(&m_sockaddr.sin_addr), buffer, sizeof(buffer));
}

bool KSocket::SendData( const char* szData_, int iSize_ )
{
    KCriticalSectionLock lock( m_csSock );

    if( m_sock == INVALID_SOCKET ) {
        BEGIN_LOG( cerr, L"error" ) << END_LOG;
        return false;
    }

    if( iSize_ <= 0 || szData_ == nullptr ) {
        BEGIN_LOG( cerr, L"error" ) << END_LOG;
        return false;
    }

    if( m_ovlSend.m_dwRemained + iSize_ > MAX_PACKET_SIZE ) {
        BEGIN_LOG( cwarn, L"Error: send buffer is Full: " << iSize_
            << L", remained : " << m_ovlSend.m_dwRemained
            << L", bSending : " << m_ovlSend.m_bSending );

        // data was not sent.
        //
        m_ovlSend.m_dwRemained = 0;
        m_ovlSend.m_bSending = false;

        OnCloseSocket();
        return false;
    }

    // copy data to buffer.
    ::memmove( &m_ovlSend.m_pBuffer[m_ovlSend.m_dwRemained], szData_, iSize_ );
    m_ovlSend.m_dwRemained += iSize_;

    // when last WSASend() is not pending.
    if( m_ovlSend.m_bSending == false ) {
        DWORD dwWrite = 0;
        m_ovlSend.m_wsaBuffer.len = m_ovlSend.m_dwRemained;

        int ret = ::WSASend( m_sock, &m_ovlSend.m_wsaBuffer, 1, &dwWrite
            , 0, &m_ovlSend, NULL );

        //BEGIN_LOG( cout, L"raw length : " ) << LOG_NAMEVALUE( kbuff_.GetLength() ) << LOG_NAMEVALUE( bsbuff );

        bool isSendOk = false;
        if( ret != SOCKET_ERROR ) {
            isSendOk = true;
        }
        else {
            if( ::WSAGetLastError() == ERROR_IO_PENDING )
                isSendOk = true;
        }

        if( isSendOk == true ) {
            m_ovlSend.m_bSending = true;
            return true;
        }

        BEGIN_LOG( cerr, "error" )
            << END_LOG;

        OnCloseSocket();
        return false;
    }

    return true;
}

void KSocket::OnReceiveCompleted( DWORD dwTransfered )
{
    BEGIN_LOG( clog, "OnReceiveCompleted" )
        << END_LOG;
}

void KSocket::OnCloseSocket()
{
    CloseSocket();
    BEGIN_LOG( clog, "OnCloseSocket" )
        << END_LOG;
}

void KSocket::OnAcceptConnection()
{
    BEGIN_LOG( clog, "OnAcceptConnection" )
        << END_LOG;
}
