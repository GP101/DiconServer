#include <WinSock2.h>
#include <ws2tcpip.h>
#include "KSocketAccepterThread.h"
#include "NetUtil.h"
#include "KSocket.h"
#include "KIocp.h"
#include <iostream>
#include "KGen.h"


CONSTRUCTOR KSocketAccepterThread::KSocketAccepterThread()
    : m_pfnCreateUser( NULL )
    , m_bNagleAlgOn( true )
{
}

void KSocketAccepterThread::_CloseSocket()
{
    if( m_sock != INVALID_SOCKET ) {
        ::shutdown( m_sock, SD_BOTH );
        ::closesocket( m_sock );
        m_sock = INVALID_SOCKET;
    }
}//KSocketAccepterThread::_CloseSocket()

void KSocketAccepterThread::BindAndListen( u_short usPort_
    , CreateUserCallback pfnCreateUser_
    , bool bNagleAlgOn_ )
{
    BOOST_ASSERT( GetThreadId() == 0 );

    m_pfnCreateUser = pfnCreateUser_;

    m_sock = ::WSASocket( AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED );
        //                                                            ^ overlapped I/O

    if( m_sock == INVALID_SOCKET ) {
        return;
    } // if

    char bOK = true;
    int nRet = 0;
    nRet = ::setsockopt( m_sock, SOL_SOCKET, SO_REUSEADDR, &bOK, sizeof( char ) );

    SOCKADDR_IN local;
    {
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = INADDR_ANY;
        local.sin_port = htons( usPort_ );
    }

    if( ::bind( m_sock, (SOCKADDR *) &local, sizeof( local ) ) == SOCKET_ERROR ) {
        _CloseSocket();
        return;
    } // if

    if( ::listen( m_sock, SOMAXCONN ) == SOCKET_ERROR ) {
        _CloseSocket();
        return;
    } // if

    return;
}

VIRTUAL void KSocketAccepterThread::ThreadRun()
{
    BEGIN_LOG( cout, L"Enter" )
        << END_LOG;

    DWORD dwPrevTime = ::GetTickCount();

    while( true ) {
        const DWORD dwCurrTime = ::GetTickCount();
        const DWORD dwElapsedTime = dwCurrTime - dwPrevTime;
        dwPrevTime = dwCurrTime;

        const DWORD ret = ::WaitForSingleObject( m_hQuitEvent, 0 );
        if( ret == WAIT_OBJECT_0 )
            break;

        VIRTUAL Update( dwElapsedTime );
    }// while

    BEGIN_LOG( cout, L"Exit" )
        << END_LOG;
}

VIRTUAL bool KSocketAccepterThread::BeginThread()
{
    return KThread::BeginThread();
}

VIRTUAL void KSocketAccepterThread::EndThread( DWORD dwTimeout_ )
{
    _CloseSocket();
    KThread::EndThread( dwTimeout_ );
}

VIRTUAL void KSocketAccepterThread::Update( DWORD dwElapsedTime_ )
{
    //// accept and receive

    SOCKET socket = INVALID_SOCKET;
    SOCKADDR_IN sinRemote;
    int size = sizeof(sinRemote);

    // create new accept socket for new user connection.
    socket = ::accept( m_sock, (SOCKADDR*)&sinRemote, &size );

    if( socket == INVALID_SOCKET ) {
        BEGIN_LOG( cerr, L"INVALID_SOCKET. WSAError : " )
            << NetUtil::GetWsaMsg();
        return;
    }

    const DWORD dwPrevTime = ::GetTickCount();

    if( m_bNagleAlgOn == false ) {
        int sock_opt = 1;
        if( ::setsockopt( socket, IPPROTO_TCP, TCP_NODELAY, (char*)&sock_opt, sizeof(sock_opt) ) == SOCKET_ERROR ) {
            BEGIN_LOG( cerr, L"" )
                << NetUtil::GetWsaMsg()
                << LOG_NAMEVALUE( WSAGetLastError() );
            return;
        }
    }

    //// create user

    KUserPtr spUser = m_pfnCreateUser();
    KSocket* pkSockObj = spUser->GetKSocket();

    DWORD dwElapTime = ::GetTickCount() - dwPrevTime;

    if( pkSockObj == NULL ) {
        BEGIN_LOG( cerr, L"Failed to create user" );
        _CloseSocket();
        return;
    }
    else {
        pkSockObj->SetSocketInfo( socket, sinRemote );

        //// bind socket to Iocp

        const bool isBindSocekt = _KIocp.BindSocket( pkSockObj );
        if( isBindSocekt == false ) {
            BEGIN_LOG( cout, L"error" )
                << END_LOG;
        }

        // start receiving
        const bool isReceiveData = pkSockObj->ReceiveData();
        if( isReceiveData == true ) {
            VIRTUAL pkSockObj->OnAcceptConnection();
        }
    }
    if( spUser.use_count() == 1 )
        spUser->Finalize();
}
