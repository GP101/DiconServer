#include <winsock2.h>
#include <iomanip>
#include <sstream>
#include "KGen.h"
#include "KMyDbThread.h"
#include "KBaseServer.h"
#include "KUserLayer.h"
#include "CommonPacket.h"
#include "MyCenterServerPacket.h"
#include "NetError.h"
#include "KNetLayer.h"


CONSTRUCTOR KMyDbThread::KMyDbThread( const wchar_t* szDsnFile_ )
{
}

DESTRUCTOR KMyDbThread::~KMyDbThread()
{
}

VIRTUAL void KMyDbThread::ThreadRun()
{
    BEGIN_LOG( cout, L"Enter" )
        << END_LOG;

    DWORD dwPrevTime = ::GetTickCount();

    while( true ) {
        const DWORD dwCurrTime = ::GetTickCount();
        const DWORD dwElapsedTime = dwCurrTime - dwPrevTime;
        dwPrevTime = dwCurrTime;

        const DWORD ret = ::WaitForSingleObject( m_hQuitEvent, 1 );
        if( ret == WAIT_OBJECT_0 ) 
            break;

        VIRTUAL Update( dwElapsedTime );
    }

    BEGIN_LOG( cout, L"Exit" )
        << END_LOG;
}

VIRTUAL void KMyDbThread::Update( DWORD dwElapsedTime_ )
{
    KPacketPtr spPacket;

    // access the queue in the thread manager, get a packet from the quque.
    if( m_pkThreadMgr->GetKPacket( spPacket ) == false )
        return;

    if( spPacket == NULL ) {
        BEGIN_LOG( cout, L"Sub-Thread Terminated." )
            << END_LOG;
        SetEvent( m_hQuitEvent );
        return;
    }

    const DWORD dwPrevTime = ::GetTickCount();

    OnPacket( spPacket.get( ) );

    DWORD dwElapTime = ::GetTickCount() - dwPrevTime;

    // save processing info here to analyze performance.
    // jintaeks on 2017-09-16_09-24
}

void KMyDbThread::OnPacket( IN KPacket* pkPacket_ )
{
    switch( pkPacket_->m_usPacketId )
    {
        CASE_PACKET( EDB_ON_START_REQ, KNetAddress );
        CASE_PACKET( EDB_UPDATE_USER_INFO_REQ, KEDB_UPDATE_USER_INFO_REQ );

        default:
            BEGIN_LOG( cerr, L"Packet handler is not defined " << pkPacket_->GetIdWstr() );
    }//switch
}

void KMyDbThread::On_EDB_ON_START_REQ( LONGLONG nSenderUid_, KNetAddress& kPacket_ )
{
}

void KMyDbThread::On_EDB_UPDATE_USER_INFO_REQ( LONGLONG nSenderUid_, KEDB_UPDATE_USER_INFO_REQ& kPacket_ )
{
    KEDB_UPDATE_USER_INFO_FAILED_ACK kAck;
    kAck.m_nOk = NetError::ERR_UNKNOWN;

    NetSendTo( SNDTO_USER, kPacket_.m_uidUser, EDB_UPDATE_USER_INFO_FAILED_ACK, kAck );
}
