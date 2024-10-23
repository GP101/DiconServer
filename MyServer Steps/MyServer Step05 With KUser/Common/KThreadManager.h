#pragma once
#include "KWorkQueue.h"
#include "KThread.h"
#include <atltime.h>
#include "KGen.h"


class KThreadManager : public KServerObject
{
protected:
                        KThreadManager();
public:
    virtual             ~KThreadManager() noexcept(true);

public:
    virtual bool        Initialize( DWORD nThreadNum_ );
    virtual void        Finalize();

    void                BeginAllThread();
    void                EndAllThread();

    size_t              GetThreadNum() const { KCriticalSectionLock lock( m_csVecThread ); return m_vecThread.size(); }
    void                SetNumThread( size_t nThreadNum ); // dynamically change number of threads

    /// get a packet from the queue
    /// @note   will be accessed by multiple threads managed by this manager.
    bool                GetKPacket( KPacketPtr& spPacket );

    void                QueueingPacket( const KPacketPtr& spPacket )
                        {
                            m_workQueue.QueueingPacket( spPacket );
                        }

    int                 GetQueueSize()
                        {
                            //KCriticalSectionLock lock( m_csPacketQueue );
                            //return m_queuePacket.size();
                            return m_workQueue.GetQueueSize();
                        }

protected:      
    virtual KThread*    CreateThread() = 0;

    KWorkQueue          m_workQueue; // this queue is shared between threads

    std::vector<std::shared_ptr<KThread>>
                        m_vecThread;
    KThread*            m_vecThread2;
    KCriticalSection    m_csVecThread;
};
