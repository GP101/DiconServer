#pragma once
#include "KMyUser.h"
#include "KUserManager.h"
#include "noncopyable.h"


class KMyUserManager : public KUserManager
{
public:
    CONSTRUCTOR         KMyUserManager();
    virtual             ~KMyUserManager();

    virtual bool        Initialize( DWORD dwParam_ ) override;
    virtual void        Finalize() override;

    bool                InsertUser( KMyUserPtr spUser );
    bool                DeleteByCharacterId( const LONGLONG nCid_ );

    KMyUserPtr          GetByCharacterId( const LONGLONG nCid_ );
    KMyUserPtr          GetByCharacterName( const std::wstring& strCharName );

    /// you must not use like this! 
    /// jintaeks on 2017-09-08_12-53
    //CRITICAL_SECTION& 
    //                    GetCriticalSection() { return m_csUser; }

    KMyUserPtr          At( int i ) const
                        {
                            return std::static_pointer_cast<KMyUser>( m_vecUser[i] );
                        }

    void                UnregisterCharInfo( IN KMyUser& kUser );
    void                RegisterCharInfo( IN KMyUser& kUser );
    virtual bool        QueueingByCharacterId( const LONGLONG nReceiverCid_, const KPacketPtr& spPacket ) override;

protected:
    virtual bool        DeleteUser( const LONGLONG nUid ) override;

protected:
    std::map<LONGLONG, KMyUserPtr>
                        m_mapCharacterId; // character uid.
    std::map<std::wstring, KMyUserPtr>
                        m_mapCharacterName; // character name.
};

#define _KMyUserManager std::static_pointer_cast<KMyUserManager,KUserManager>(KUserManager::Instance())
