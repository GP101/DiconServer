#include "KWorkQueue.h"
#include "KGen.h"


KWorkQueue::KWorkQueue()
{
    InitializeCriticalSection( &m_csPacketQueue );
}

KWorkQueue::~KWorkQueue()
{
    CSLOCK( m_csPacketQueue )
    {
        if( m_queuePacket.empty() == false ) {
            BEGIN_LOG( cwarn, "Object destroy with un-processed packets. Name : " << m_wstrName )
                << LOG_NAMEVALUE( m_queuePacket.size() );
        }
    }

    DeleteCriticalSection( &m_csPacketQueue );
}

bool KWorkQueue::GetKPacket( KPacketPtr& spPacket_ )
{
    KCriticalSectionLock lock( m_csPacketQueue );

    if( m_queuePacket.empty() )
        return false;

    spPacket_ = m_queuePacket.front();
    m_queuePacket.pop();

    return true;
}

void KWorkQueue::QueueingPacket( const KPacketPtr& spPacket_ )
{
    KCriticalSectionLock lock( m_csPacketQueue );

    m_queuePacket.push( spPacket_ );
}
