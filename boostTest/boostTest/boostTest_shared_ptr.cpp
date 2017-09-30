#include <boost/bind.hpp>
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

int main()
{
    KPacketPtr spPacket;
    spPacket.reset( new KPacket );
    if (spPacket != nullptr)
        std::cout << "first" << std::endl;
    if (spPacket != NULL)
        std::cout << "second" << std::endl;
    if (spPacket == nullptr)
        std::cout << "third" << std::endl;
}
