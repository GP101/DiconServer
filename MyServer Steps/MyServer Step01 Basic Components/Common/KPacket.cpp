#include "KPacket.h"


KPacket::KPacket() : m_usPacketId( 0 ), m_nSenderUid( 0 )
{
}

KPacket::~KPacket()
{
}

KPacket& KPacket::operator=(const KPacket& right)
{
    m_nSenderUid    = right.m_nSenderUid;
    m_usPacketId     = right.m_usPacketId;
    m_buffer = right.m_buffer;
    return *this;
}
