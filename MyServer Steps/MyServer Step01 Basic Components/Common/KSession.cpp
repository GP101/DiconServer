#include "KGen.h"
#include "KSession.h"
#include "KPacket.h"
#include "KCriticalSection.h"
#include "NetUtil.h"
#include <time.h>
#include "CommonPacket.h"
#include <functional>
#include "KIocp.h"

using namespace std::placeholders;

CONSTRUCTOR KSession::KSession()
    : m_bDestroyReserved( false )
{
    m_kSessionSocket.m_pkSession = this;
}

DESTRUCTOR KSession::~KSession()
{
    BOOST_ASSERT( m_kSessionSocket.IsConnected() == false );
}

VIRTUAL bool KSession::Initialize( DWORD dwParam_ )
{
    return true;
}

VIRTUAL void KSession::Finalize()
{
    m_kSessionSocket.CloseSocket();

    _KIocp.DeleteCompletionKey( &m_kSessionSocket );
}

VIRTUAL void KSession::OnAcceptConnection()
{
    KPacket packet;
    packet.SetData( EGSCL_ACCEPT_CONNECTION_NOT, KIdPacket() );
    SendPacket( packet );
}

VIRTUAL void KSession::OnDestroy()
{
    m_kSessionSocket.CloseSocket();
    m_bDestroyReserved = false;

    // process pending packets.
    if( m_workQueue.GetQueueSize() > 0 )
        m_workQueue.ProcessPackets( std::bind( &KSession::OnPacket, this, _1 ) );
}

void KSession::OnReceiveData( IN std::vector<char>& buffer_ )
{
    KPacketPtr spPacket( new KPacket() );

    BufferToPacket( IN buffer_, IN OUT *spPacket );

    switch( spPacket->m_usPacketId ) {
    case ECLGS_HEART_BIT: // heart bit filtering - no queueing
        break;
    default:
        QueueingPacket( spPacket );
    }//switch
}

bool KSession::SendPacket( IN const KPacket& packet )
{
    // just return true when destruntion is reserved.
    if( m_bDestroyReserved ) return true;

    if( m_kSessionSocket.IsConnected() == false ) {
        BEGIN_LOG( cerr, L"error" )
            << END_LOG;
        return false;
    }

    std::vector<char>   buffer;
    PacketToBuffer( IN packet, OUT buffer );

    return m_kSessionSocket.SendData( (const char*)&buffer[0], buffer.size() );
}

VIRTUAL void KSession::Update(DWORD dwElapsedTime_)
{
    m_workQueue.ProcessPackets( std::bind( &KSession::OnPacket, this, _1 ) );

    if( m_bDestroyReserved == true ) {
        VIRTUAL OnDestroy();
        return;
    }
}

VIRTUAL void KSession::OnCloseSocket()
{
    m_bDestroyReserved = true;
}

bool KSession::IsRefCountLessThan( int nCount_ ) const
{
    // return true when current reference counter value is less than nCount_
    return GetRefCount() < nCount_;
}
