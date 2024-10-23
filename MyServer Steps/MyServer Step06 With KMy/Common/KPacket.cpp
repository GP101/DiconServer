#include "KPacket.h"

#undef _ENUM
#define _ENUM( id ) L#id,
const wchar_t*  KPacket::ms_szPacketId[] = { 
    #include "Packet.inc" 
};


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

const wchar_t* KPacket::GetIdWstr( const unsigned short usPacketId )
{
    if( usPacketId >= PACKETID_SENTINEL ) {
        return ms_szPacketId[PACKETID_SENTINEL];
    }
    return ms_szPacketId[usPacketId];
}
