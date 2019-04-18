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


struct KServerInfo
{
    int                 m_iServerPart;
    std::wstring        m_wstrName;
    std::string         m_wstrIp;
    USHORT              m_usPort;
    int                 m_iUserNum;
    int                 m_iMaxUserNum;
    int                 m_nUserProtocolVer;

    KServerInfo() {}
    KServerInfo( const KServerInfo& right ) { *this = right; }
    KServerInfo& operator=( const KServerInfo& right )
    {
        m_iServerPart       = right.m_iServerPart;
        m_wstrName           = right.m_wstrName;
        m_wstrIp             = right.m_wstrIp;
        m_usPort            = right.m_usPort;
        m_iUserNum          = right.m_iUserNum;
        m_iMaxUserNum       = right.m_iMaxUserNum;
        m_nUserProtocolVer  = right.m_nUserProtocolVer;
        return *this;
    }

    template<typename Archive>
    void serialize( Archive& ar, const unsigned int version )
    {
        ar & m_iServerPart;
        ar & m_wstrName;
        ar & m_wstrIp;
        ar & m_usPort;
        ar & m_iUserNum;
        ar & m_iMaxUserNum;
        ar & m_nUserProtocolVer;
    }
};

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

DECLARE_PACKET( EGSCN_VERIFY_ACCOUNT_ACK )
{
    int                 m_nOk;
    LONGLONG            m_nUid;
    short               m_sGroupId;
    std::wstring        m_wstrName;
    short               m_sMaxNum;
    KNetAddress         m_kNetAddress;
    int                 m_iVersion;

    template<typename Archive>
    void serialize( Archive& ar, const unsigned int version )
    {
        ar & m_nOk;
        ar & m_nUid;
        ar & m_sGroupId;
        ar & m_wstrName;
        ar & m_sMaxNum;
        ar & m_kNetAddress;
        ar & m_iVersion;
    }
};

#pragma pack( pop )
