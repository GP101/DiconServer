#include <boost/bind/bind.hpp>
#include <vector>
#include <algorithm>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <queue>

struct KPacket;
typedef boost::shared_ptr<KPacket> KPacketPtr;
struct KPacket
{
};

class KQueue
{
public:
    void AddPacket( KPacketPtr spPacket )
    { 
        m_queue.push( spPacket );
    }
    void ProcessAllPacket()
    {
        while (m_queue.empty() == false)
        {
            KPacketPtr spPacket = m_queue.front();
            m_queue.pop();
            OnPacket( spPacket );
        }
    }
    void OnPacket( KPacketPtr spPacket )
    {
    }
private:
    std::queue<KPacketPtr> m_queue;
};

class KSession
{
public:
    void Update()
    {
        m_kQueue.ProcessAllPacket();
    }
    void OnPacket( KPacketPtr spPacket )
    {
    }
private:
    KQueue m_kQueue;
};

int main()
{
    KSession session;
}
