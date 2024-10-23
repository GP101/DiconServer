#include <WinSock2.h>
#include <functional>
#include <sstream>
#include <iomanip>
#include "KUserManager.h"
#include "KMyGameServer.h"
#include "KMyDbThread.h"
#include "KDbLayer.h"
#include "KUser.h"
#include "KMyUserFsm.h"
#include "KUserLayer.h"
#include "KLog.h"

using namespace std::placeholders;

const wchar_t*  g_szConfigFile = L"config";


KThread* CreateDbThread( LPCWSTR szFileDSN )
{
    return new KMyDbThread( szFileDSN );
}

KUserPtr CreateUserForUserManager()
{
    KUserPtr spUser;
    spUser.reset( new KUser() );
    return spUser;
}

KFsmPtr CreateUserFsm()
{
    KFsmPtr spFsm;
    spFsm.reset( new KMyUserFsm() );
    return spFsm;
}

KUserPtr CreateUserForSocketAccepter()
{
    return _KUserManager->CreateUser();
}

CONSTRUCTOR KMyGameServer::KMyGameServer()
{
}

DESTRUCTOR KMyGameServer::~KMyGameServer()
{
}

VIRTUAL bool KMyGameServer::Initialize( DWORD dwParam_ )
{
    KUserManager::CreateInstance<KUserManager>( 0L );
    KUserLayer::CreateInstance<KUserLayer>( 0L ); // _KUserLayer

    // prepare initialize parameters.
    //
    m_dbLayerInitParam.dwNumThreads = 3;
    m_dbLayerInitParam.pfnCreateDbThread = &CreateDbThread;

    m_netLayerInitParam.dwNumIocpThreads = 4;
    m_netLayerInitParam.pfnCreateUserCallback = &CreateUserForSocketAccepter;

    m_userLayerInitParam.pfnCreateUserCallback = &CreateUserForUserManager;
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

    __super::Finalize();
    _KUserManager->Finalize();
}

bool KMyGameServer::DestroyAllSession()
{
    //// at this point all Iocp accepters of NetInterface are closed.

    BEGIN_LOG( cout, L"Number of current user : " )
        << _KUserManager->GetCount()
        << END_LOG;

    _KUserManager->ForEach(std::bind( &KUser::ReserveDestroy, _1 ) );
    _KUserManager->RegisterDeleteUserAll();

    const DWORD dwBeginTime = ::GetTickCount();

    // all user object must be deatroyed and all packets sent to router must be processed.
    while( _KUserManager->GetCount() != 0
        && ::GetTickCount() - dwBeginTime < m_dwShutDownWaitTime ) {
        ::Sleep( 500 );
    }

    bool bSuccess = false;
    bSuccess = ( _KUserManager->GetCount() == 0 );

    BEGIN_LOG( cout, L"" )
        << LOG_NAMEVALUE( bSuccess )
        << LOG_NAMEVALUE( m_dwShutDownWaitTime )
        << LOG_NAMEVALUE( _KUserManager->GetCount() )
        << LOG_NAMEVALUE( ::GetTickCount() - dwBeginTime );

    return bSuccess;
}

VIRTUAL void KMyGameServer::Update(DWORD dwElapsedTime_)
{
    VIRTUAL KBaseServer::Update(dwElapsedTime_);

    if( ::GetTickCount() - m_dwServerInfoUpdateInterval > ms_dwSendServerListInterval ) {
        //KDB_PACKET_SERVER_INFO_REQ kPacket;
        //kPacket.m_wstrIp         = NetUtil::GetLocalIpW();
        //kPacket.m_nCurUserNum   = KUserManager::GetKObj()->GetCount();
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
