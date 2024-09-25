#include <WinSock2.h>
#include <functional>
#include <sstream>
#include <iomanip>
#include "KMyGameServer.h"
#include "KMyDbThread.h"
#include "KDbLayer.h"
#include "KUser.h"
#include "KMyUserFsm.h"
#include "KUserLayer.h"
#include "KLog.h"

using namespace std::placeholders;

const wchar_t*  g_szConfigFile = L"config";
//{{ qff. jintaeks on 2017-09-21_18-57
KUserPtr    g_spClientUser;
//}} qff

KThread* CreateDbThread( LPCWSTR szFileDSN )
{
    return new KMyDbThread( szFileDSN );
}

KUserPtr CreateUser()
{
    KUserPtr spUser;
    spUser.reset( new KUser() );
    g_spClientUser = spUser;
    g_spClientUser->Initialize( 0L );
    BEGIN_LOG(clog, L"") << END_LOG;
    return spUser;
}

KFsmPtr CreateUserFsm()
{
    KFsmPtr spFsm;
    spFsm.reset( new KMyUserFsm() );
    return spFsm;
}


CONSTRUCTOR KMyGameServer::KMyGameServer()
{
}

DESTRUCTOR KMyGameServer::~KMyGameServer()
{
}

VIRTUAL bool KMyGameServer::Initialize( DWORD dwParam_ )
{
    KUserLayer::CreateInstance<KUserLayer>( 0L ); // _KUserLayer

    // prepare initialize parameters.
    //
    m_dbLayerInitParam.dwNumThreads = 3;
    m_dbLayerInitParam.pfnCreateDbThread = &CreateDbThread;

    m_netLayerInitParam.dwNumIocpThreads = 4;
    m_netLayerInitParam.pfnCreateUserCallback = &CreateUser;

    m_userLayerInitParam.pfnCreateUserCallback = &CreateUser;
    m_userLayerInitParam.pfnCreateFsmCallback = &CreateUserFsm;

    const bool isInitialized = KBaseServer::Initialize( dwParam_ );
    if( isInitialized == false ) {
        BEGIN_LOG( cerr, L"error" ) << END_LOG;
        return false;
    }

    BEGIN_LOG( clog, L"" ) << END_LOG;

    return true;
}

VIRTUAL void KMyGameServer::Finalize()
{
    // save current server information to Db
    //
    //KDB_PACKET_SERVER_INFO_REQ kPacket;
    //kPacket.m_nProtocolVersion  = -1;
    //kPacket.m_wstrIp         = NetUtil::GetLocalIpW();
    //kPacket.m_nCurUserNum   = 0;
    //kPacket.m_bFirstDBWork  = false;
    //kPacket.m_hSyncEvent    = NULL;

    //KMyDbInterface()->QueueingPacket( DB_PACKET_SERVER_INFO_REQ );

    if( g_spClientUser != nullptr )
        g_spClientUser->Finalize();
    __super::Finalize();
}

bool KMyGameServer::DestroyAllSession()
{
    //// at this point all Iocp accepters of NetInterface are closed.

    return true;
}

VIRTUAL void KMyGameServer::Update(DWORD dwElapsedTime_)
{
    VIRTUAL KBaseServer::Update(dwElapsedTime_);

    if( ::GetTickCount() - m_dwServerInfoUpdateInterval > ms_dwSendServerListInterval ) {
        //KDB_PACKET_SERVER_INFO_REQ kPacket;
        //kPacket.m_wstrIp         = NetUtil::GetLocalIpW();
        //kPacket.m_nProtocolVersion  = m_nProtocolVersion;
        //kPacket.m_bFirstDBWork  = false;
        //kPacket.m_hSyncEvent    = NULL;

        //KMyDbInterface()->QueueingPacket( DB_PACKET_SERVER_INFO_REQ );

        m_dwServerInfoUpdateInterval = ::GetTickCount();
    }
}

void KMyGameServer::OnPacket( IN KPacket* pkPacket )
{
    KLOG << "Received Packet " << pkPacket->m_usPacketId;
}
