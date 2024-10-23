#include <WinSock2.h>
#include "KNetLayer.h"
#include "KUserManager.h"
#include "KIocp.h"
#include "KIocpWorkerThread.h"
#include "KSession.h"
#include "KGen.h"
#include <functional>
#include "KDbLayer.h"
#include "KBaseServer.h"

using namespace std::placeholders;

void NetSendPacket( int nFlag_, LONGLONG nFrom_, LONGLONG nTo_, KPacketPtr spPacket_ )
{
    switch( nFlag_ ) {
    case SNDTO_DB:
        _KDbLayer->QueueingPacket( spPacket_ );
    	return;
    case SNDTO_SERVER:
        _KBaseServer->QueueingPacket( spPacket_ );
        return;
    case SNDTO_CENTER:
        // send to center server
        return;
    case SNDTO_CLIENT:
        _KUserManager->SendTo(nTo_, *spPacket_);
        return;
    case SNDTO_USER:
        _KUserManager->QueueingTo(nTo_, spPacket_);
        return;
    case SNDTO_REMOTE_DB:
        // send to remote Db
        return;
    case SNDTO_REMOTE_USER:
        {
            // nFrom : sender uid (user)
            // nTo   : receiver uid (user)
            KPacket packet;
            packet.m_nSenderUid = nFrom_;
            packet.SetData( EGSCN_DIRECT_TO_USER, *spPacket_ );
            // set to remote user
        }
        return;
    case SNDTO_REMOTE_SERVER:
        {
            // nFrom : userUid
            // nTo   : serverUid
            KPacket packet;
            packet.m_nSenderUid = nFrom_;
            packet.SetData( EGSCN_DIRECT_TO_RSERVER, *spPacket_ );
            // send to remote server
        }
        return;
    case SNDTO_REMOTE_CLIENT:
        {
            // nFrom : serveruid
            // nTo   : useruid
            spPacket_->m_nSenderUid = nTo_;
            KPacket packet;
            packet.m_nSenderUid = nFrom_;
            packet.SetData( EGSCN_DIRECT_TO_USER, *spPacket_ );
            _KUserManager->SendTo(nFrom_, packet);
        }
        return;
    default:
        BEGIN_LOG( cerr, L"nFlag_ : " << nFlag_ );
    }
}


CONSTRUCTOR KNetLayer::KNetLayer()
{
    m_usDefaultPort = 5150;
}

DESTRUCTOR KNetLayer::~KNetLayer()
{
}

bool KNetLayer::Initialize( DWORD dwParam_ )
{
    //// create socket and listen
    //// initialize Iocp

    KNetLayerInitParam* pInitParam = reinterpret_cast<KNetLayerInitParam*>( dwParam_ );
    assert( pInitParam != nullptr );

    m_kSocketAccepterThread.BindAndListen( m_usDefaultPort, pInitParam->pfnCreateUserCallback );

    const DWORD dwNumWorkingThreads = pInitParam->dwNumIocpThreads;
    // create Iocp working threads and run it.
    _KIocp.Initialize( dwNumWorkingThreads );

    m_kSocketAccepterThread.BeginThread(); // begin accepter thread.

    return true;
}

VIRTUAL void KNetLayer::Finalize()
{
    // end thread and finalize.
    m_kSocketAccepterThread.EndThread( 3000L );

    // end Iocp working threads.
    _KIocp.Finalize();
    Sleep( 100 );
}
