#pragma once
#include "KPacket.h"
#include "CommonPacket.h"

#pragma pack( push, 1 )


DECLARE_PACKET( ECLGS_USER_CHAT )
{
    LONGLONG            m_uidChar;
    std::wstring        m_strChat;

    template<typename Archive>
    void serialize( Archive& ar, const unsigned int version )
    {
        ar & m_uidChar;
        ar & m_strChat;
    }
};

DECLARE_PACKET( EDB_UPDATE_USER_INFO_REQ )
{
    LONGLONG            m_uidUser; // user Uid
    u_char              m_iCharacterLevel;
    int                 m_iExp;
    int                 m_iGamePoint;

    // quest. inventory. equipped item info here. jintaeks on 2017-08-23_10-11
    template<typename Archive>
    void serialize( Archive& ar, const unsigned int version )
    {
        ar & m_uidUser;
        ar & m_iCharacterLevel;
        ar & m_iExp;
        ar & m_iGamePoint;
    }
};

DECLARE_PACKET( EDB_UPDATE_USER_INFO_FAILED_ACK )
{
    int                 m_nOk;
    int                 m_iExp;
    int                 m_iGamePoint;

    template<typename Archive>
    void serialize( Archive& ar, const unsigned int version )
    {
        ar & m_nOk;
        ar & m_iExp;
        ar & m_iGamePoint;
    }
};

DECLARE_PACKET( ECLGS_P2PINFO_NOT )
{
    std::vector<DWORD>  m_vecIp;
    std::vector<u_short>
                        m_vecPort;

    template<typename Archive>
    void serialize( Archive& ar, const unsigned int version )
    {
        ar & m_vecIp;
        ar & m_vecPort;
    }
};

#pragma pack( pop )
