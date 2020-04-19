#include <boost/bind.hpp>
#include <vector>
#include <algorithm>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <queue>
#include "KCriticalSection.h"

struct KPacket;
typedef boost::shared_ptr<KPacket> KPacketPtr;
struct KPacket
{
};

class KQueue
{
public:
    void AddPacket(KPacketPtr spPacket)
    {
        KCriticalSectionLock lock(m_csQueue);
        m_queue.push(spPacket);
    }
    template<typename FUNCTOR>
    void ProcessAllPacket(FUNCTOR callback)
    {
        while (m_queue.empty() == false)
        {
            KPacketPtr spPacket;
            {
                KCriticalSectionLock lock(m_csQueue);
                if (m_queue.empty())
                    break;
                spPacket = m_queue.front();
                m_queue.pop();
            }
            callback(spPacket);
        }
    }
private:
    std::queue<KPacketPtr>  m_queue;
    KCriticalSection        m_csQueue;
};

class KSession
{
public:
    void AddPacket(KPacketPtr spPacket)
    {
        m_kQueue.AddPacket(spPacket);
    }
    void Update()
    {
        m_kQueue.ProcessAllPacket(boost::bind(&KSession::OnPacket, this, _1));
    }
    virtual void OnPacket(KPacketPtr spPacket)
    {
        printf("%s\r\n", __FUNCTION__);
    }
private:
    KQueue     m_kQueue;
};

class KMySession : public KSession
{
public:
    virtual void OnPacket( KPacketPtr spPacket )
    {
        printf( "%s\r\n", __FUNCTION__ );
    }
};

int main()
{
    KMySession session;
    KPacketPtr p0;
    KPacketPtr p1;
    p0.reset(new KPacket());
    p1.reset(new KPacket());
    session.AddPacket(p0);
    session.AddPacket( p1 );

    session.Update();
}
