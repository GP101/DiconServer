#pragma once
#include <boost/scoped_ptr.hpp>
#include "KThread.h"
#include "KWorkQueue.h"
#include "CommonPacket.h"
#include "KBaseSingleton.h"
#include "LayerUtil.h"


class KDbLayer;
class KUserLayer;
class KBaseServer
    : public KServerObject
    , public KBaseSingleton<KBaseServer>
{
    friend class KBaseSingleton<KBaseServer>;

public:
    static const DWORD  ms_dwSendServerListInterval = 60 * 1000;

                        KBaseServer();
    virtual             ~KBaseServer();

    virtual bool        Initialize( DWORD dwParam_ );
    virtual void        Finalize();

    virtual bool        DestroyAllSession() = 0;
    virtual void        Update(DWORD dwElapsedTime_);
    virtual void        OnPacket( IN KPacket* pkPacket_ ) = 0;
    void                QueueingPacket( const KPacketPtr& spPacket );

    void                SetHwnd( HWND hWnd ) { m_hWnd = hWnd; }
    HWND                GetHwnd() { return m_hWnd; }

    void                SetProtocolVersion( int nProtocolVer );
    void                SetShutDownWaitTime( DWORD dwWaitTime_ ) { m_dwShutDownWaitTime = dwWaitTime_; }

    /// called in Db thread
    void                SetServerList( IN const std::vector< KServerInfo >& vecServerList );
    /// called in User thread
    void                GetServerList( OUT std::vector< KServerInfo >& vecServerList );

    void                SetMailingList( const char* strList ) { m_strMailingList = NetUtil::NarrowToWideString( strList ); }

protected:
    void                LoadIniFile( const wchar_t* szFileName );

protected:
    KDbLayerInitParam   m_dbLayerInitParam;
    KNetLayerInitParam  m_netLayerInitParam;
    KUserLayerInitParam m_userLayerInitParam;

    /// server can receive a packet from other component.
    /// for example, KDbLayer can send a packet to the server, so we need a packet queue in the server.
    KWorkQueue          m_workQueue;
    std::wstring        m_strTagVersion;
    std::wstring        m_strMailingList;
    std::wstring        m_strIniFilePath;
    HWND                m_hWnd;

    DWORD               m_dwServerInfoUpdateInterval;
    KCriticalSection    m_csServerInfo; 
    std::vector< KServerInfo >
                        m_vecServerInfoList;

public:
    int                 m_nProtocolVersion;
    DWORD               m_dwShutDownWaitTime;
};

#define _KBaseServer    KBaseServer::Instance()
