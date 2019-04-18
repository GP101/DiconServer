#pragma once
#include "KPacket.h"
#include "KServerObject.h"
#include <queue>
#define _WINSOCKAPI_
#include <windows.h>
#include "KCriticalSection.h"


class KWorkQueue : public KServerObject
{
public:
                        KWorkQueue();
    virtual             ~KWorkQueue();

    virtual bool        Initialize( DWORD dwParam_ ) { return false; }
    virtual void        Finalize() 
                        {
                            KCriticalSectionLock lock( m_csPacketQueue );
                            std::queue<KPacketPtr> emptyQueue;
                            m_queuePacket.swap( emptyQueue );
                        }
    void                QueueingPacket( const KPacketPtr& spPacket );
    size_t              GetQueueSize()
                        {
                            KCriticalSectionLock lock( m_csPacketQueue );
                            return m_queuePacket.size();
                        }

    bool                GetKPacket( IN OUT KPacketPtr& spPacket );

    template<typename Func> 
    void                ProcessPackets( Func& functor ) // process all packets
                        {
                            // consume packet queue
                            KPacketPtr spPacket;

                            while( GetKPacket( spPacket ) ) {
                                if( spPacket != NULL ) {
                                    functor( spPacket.get() );  // call packet handler
                                }
                            }
                        }

protected:
    CRITICAL_SECTION    m_csPacketQueue;
    std::queue<KPacketPtr>
                        m_queuePacket;
};
