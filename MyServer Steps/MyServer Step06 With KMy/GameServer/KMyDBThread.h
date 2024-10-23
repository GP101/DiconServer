#pragma once
#include "KThread.h"
#include "KThreadManager.h"
#include "MyCenterServerPacket.h"


class KMyDbThread : public KThread
{
public:
    CONSTRUCTOR         KMyDbThread( const wchar_t* szDsnFile_ );
    virtual             ~KMyDbThread();

    virtual void        ThreadRun() override;
    virtual void        Update( DWORD dwElapsedTime_ );

protected:
    /// first definition of OnPacket()
    /// @note   it's not a virtual function
    void                OnPacket( IN KPacket* pkPacket );

protected:
    bool                m_bIsDriverConnection = false; // qff. for debugging.

protected:
    DECLARE_ON_PACKET( EDB_ON_START_REQ, KNetAddress );
    DECLARE_ON_PACKET( EDB_UPDATE_USER_INFO_REQ, KEDB_UPDATE_USER_INFO_REQ );
};
