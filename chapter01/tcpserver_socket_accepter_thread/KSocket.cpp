#include "KSocket.h"
#include "KCriticalSection.h"


CONSTRUCTOR KSocket::KSocket()
    : m_sock( INVALID_SOCKET )
{
    m_sockaddr.sin_family = AF_INET;
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

bool KSocket::Connect( const char* szIp_, unsigned short usPort_ )
{
    //// create socket, connet and start receive

    KCriticalSectionLock lock( m_csSock );

    if( IsConnected() == true ) {
        BEGIN_LOG( cwarn, L"Already Connected." )
            << END_LOG;
        return true;
    }

    int ret = 0;

    m_sock = ::WSASocket( AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED );
    ////                                                              ^ overlapped Io

    if( m_sock == INVALID_SOCKET ) {
        BEGIN_LOG( cerr, "error" )
            << END_LOG;
        return false;
    }

    char bOK = true;
    const int nRet = ::setsockopt( m_sock, SOL_SOCKET, SO_REUSEADDR, &bOK, sizeof( char ) );
    if( nRet == SOCKET_ERROR ) {
        BEGIN_LOG( cerr, "error" )
            << END_LOG;
        return false;
    }

    if( szIp_ != nullptr )
        SetIp( szIp_ );

    if( usPort_ != 0 )
        SetPort( usPort_ );

    ret = ::connect( m_sock, ( struct sockaddr* )&m_sockaddr, sizeof( m_sockaddr ) );

    if( SOCKET_ERROR == ret ) {
        // can be caused by insufficient Tcp/ip buffer space.
        BEGIN_LOG( cerr, "error" )
            << END_LOG;

        CloseSocket();
        return false;
    }

    // start receiving.
    const bool isReceiveData = ReceiveData();
    if( isReceiveData == false ) {
        BEGIN_LOG( cerr, L"error" )
            << END_LOG;
        return false;
    }

    return  true;
}

bool KSocket::ReceiveData()
{
    KCriticalSectionLock lock( m_csSock );

    if( m_sock == INVALID_SOCKET )
        return false;

    DWORD dwRead = 0;
    DWORD dwFlag = 0;

    m_wsaBuf.buf = m_pBufferReceive;
    m_wsaBuf.len = MAX_PACKET_SIZE;
    int ret = ::WSARecv( m_sock     // socket
        , &m_wsaBuf                 // buffer pointer, size
        , 1                         // number of WSABUF structure
        , &dwRead                   // numer of bytes after Io.
        , &dwFlag                   // [in,out] Option Flag
        , NULL                      // struct LPWSAOVERLAPPED
        , NULL );                   // callback when Io completed.

    if( ret == 0 ) {
        BEGIN_LOG( clog, "WsaRecv" )
            << LOG_NAMEVALUE( dwRead )
            << END_LOG;
    }

    return ret == 0;
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

    // copy data to buffer.
    ::memmove( m_pBufferSend, szData_, iSize_ );

    {
        DWORD dwWrite = 0;

        m_wsaBuf.buf = m_pBufferSend;
        m_wsaBuf.len = MAX_PACKET_SIZE;
        int ret = ::WSASend( m_sock, &m_wsaBuf, 1, &dwWrite
            , 0, NULL, NULL );

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
            return true;
        }

        BEGIN_LOG( cerr, "error" )
            << END_LOG;

        return false;
    }

    return true;
}
