#pragma once
#include "KPacket.h"


#pragma pack( push, 1 )

struct KIdPacket
{
public:
    int                 m_id;

    template<typename Archive>
    void serialize( Archive& ar, const unsigned int version )
    {
        ar & m_id;
    }
};//class KIdPacket


struct KNetAddress
{
    std::wstring    m_wstrIp;
    unsigned short  m_usPort;

    template<typename Archive>
    void serialize( Archive& ar, const unsigned int version )
    {
        ar & m_wstrIp;
        ar & m_usPort;
    }
};

DECLARE_PACKET( ECLGS_VERIFY_ACCOUNT_REQ )
{
    LONGLONG            m_nUid;         // 클라이언트가 직접 채우지 않음.
    std::wstring        m_wstrUserId;
    std::wstring        m_wstrPassword;

    template<typename Archive>
    void serialize( Archive& ar, const unsigned int version )
    {
        ar & m_nUid;
        ar & m_wstrUserId;
        ar & m_wstrPassword;
    }
};

DECLARE_PACKET( ECLGS_VERIFY_ACCOUNT_ACK )
{
    int                 m_nOk;
    LONGLONG            m_nUserUid;

    template<typename Archive>
    void serialize( Archive& ar, const unsigned int version )
    {
        ar & m_nOk;
        ar & m_nUserUid;
    }
};

#pragma pack( pop )
