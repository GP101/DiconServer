#pragma once
#include "KUser.h"
#include "MyCenterServerPacket.h"
#define _WINSOCKAPI_
#include <windows.h>
#include "KTimer.h"


class KMyUser;
typedef std::shared_ptr<KMyUser> KMyUserPtr;

class KMyUser : public KUser
{
public:
                        KMyUser();
    virtual             ~KMyUser();

    enum ENUM_TIMER
    {
        TM_DB_UPDATE = 0, // for Dp update
        TM_CHAT_TICK,
        TM_WHISPER_TICK,
        TM_TOTAL_NUM,
    };

    virtual void        Update(DWORD dwElapsedTime_) override;

    LONGLONG            GetCharUid() const { return m_nCharacterUid; }
    const std::wstring& GetCharName() const { return m_wstrCharacterName; }
    int                 GetCharLevel() const { return m_iCharacterLevel; }

    // EXP & GP
    int                 GetExp() const { return m_iExp; }
    int                 GetGamePoint() const { return m_iGamePoint; }
    void                AddExp( int iExp );
    void                AddGamePoint( int iGamePoint_ );

protected:
    void                DbUpdateCharInfo();

    DECLARE_ON_PACKET( ECLGS_VERIFY_ACCOUNT_REQ, KECLGS_VERIFY_ACCOUNT_REQ );
    DECLARE_ON_PACKET( EGSCN_CHECK_UUID_ACK, KECLGS_VERIFY_ACCOUNT_REQ );
    DECLARE_ON_PACKET( EGSCN_USER_AUTHENTICATE_ACK, KECLGS_VERIFY_ACCOUNT_ACK );
    DECLARE_ON_PACKET( EGSCL_KICK_USER_NOT, KIdPacket );
    DECLARE_ON_PACKET( EDB_UPDATE_USER_INFO_FAILED_ACK, KEDB_UPDATE_USER_INFO_FAILED_ACK );
    DECLARE_ON_PACKET( ECLGS_USER_CHAT, KECLGS_USER_CHAT );
    DECLARE_ON_PACKET( ECLGS_LEAVE_PLAYER_REQ, KIdPacket );
    DECLARE_ON_PACKET( ECLGS_P2PINFO_NOT, KECLGS_P2PINFO_NOT );

    /// override OnPacket() in the KSession.
    virtual void        OnPacket( IN KPacket* pkPacket ) override;
    virtual void        OnDestroy();

private:
    // Character related stuff.
    LONGLONG            m_nCharacterUid;
    std::wstring        m_wstrCharacterName;
    int                 m_iCharacterLevel;
    int                 m_iExp;
    int                 m_iGamePoint;
    int                 m_iTargetExpToLevelUp;

    KTimer              m_kTimer[TM_TOTAL_NUM];
    std::vector<DWORD>  m_vecIp;
    std::vector<u_short>
                        m_vecPort;
};
