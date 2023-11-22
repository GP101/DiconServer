#include <functional>
#include <vector>
#include <algorithm>
#include <iostream>
#include <memory>
#include <queue>

struct KPacket;
typedef std::shared_ptr<KPacket> KPacketPtr;
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
    void ProcessAllPacket()
    {
        while (m_queue.empty() == false)
        {
            KPacketPtr spPacket = m_queue.front();
            m_queue.pop();
            OnPacket(spPacket);
        }
    }
    virtual void OnPacket(KPacketPtr spPacket)
    {
    }
protected:
    std::queue<KPacketPtr> m_queue;
};

class KSession : public KQueue
{
public:
    void Update()
    {
        __super::ProcessAllPacket();
    }
    virtual void OnPacket(KPacketPtr spPacket) override
    {
        std::cout << __FUNCTION__ << std::endl;
    }
};

int main()
{
    KPacketPtr spPacket;
    spPacket.reset(new KPacket());

    KSession session;
    session.AddPacket(spPacket);
    session.AddPacket(spPacket);
    session.ProcessAllPacket();
}
