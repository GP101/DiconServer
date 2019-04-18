#pragma once
#include <boost/scoped_ptr.hpp>
#include "KThread.h"
#include "KWorkQueue.h"
#include "CommonPacket.h"
#include "KBaseSingleton.h"
#include "LayerUtil.h"


class KBaseServer
    : public KServerObject
    , public KBaseSingleton<KBaseServer>
{
    friend class KBaseSingleton<KBaseServer>;

public:
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

protected:
    void                LoadIniFile( const wchar_t* szFileName );

protected:
    KNetLayerInitParam  m_netLayerInitParam;

    /// server can receive a packet from other component.
    KWorkQueue          m_workQueue;
    std::wstring        m_strIniFilePath;
    HWND                m_hWnd;
};

#define _KBaseServer    KBaseServer::Instance()
