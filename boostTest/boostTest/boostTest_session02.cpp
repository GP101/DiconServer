#include <boost/bind/bind.hpp>
#include <vector>
#include <algorithm>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <queue>

using namespace std::placeholders;

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
        m_queue.push(spPacket);
    }
    template<typename FUNCTOR>
    void ProcessAllPacket(FUNCTOR callback)
    {
        while (m_queue.empty() == false)
        {
            KPacketPtr spPacket = m_queue.front();
            m_queue.pop();
            callback(spPacket);
        }
    }
    //void OnPacket( KPacketPtr spPacket )
    //{
    //}
private:
    std::queue<KPacketPtr> m_queue;
};

class KSession
{
public:
    void Update()
    {
        KPacketPtr spPacket;
        spPacket.reset(new KPacket());
        m_kQueue.AddPacket(spPacket);
        m_kQueue.ProcessAllPacket(boost::bind(&KSession::OnPacket, this, _1));
    }
    virtual void OnPacket(KPacketPtr spPacket)
    {
        printf("%s\r\n", __FUNCTION__);
    }
private:
    KQueue m_kQueue;
};

class KMySession : public KSession
{
public:
    virtual void OnPacket(KPacketPtr spPacket)
    {
        printf("%s\r\n", __FUNCTION__);
    }
};

int main()
{
    KMySession session;
    session.Update();
}
